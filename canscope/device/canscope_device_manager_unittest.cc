#include "base/message_loop.h"
#include "base/synchronization/waitable_event.h"
#include "base/json/json_string_value_serializer.h"
#include "base/json/json_reader.h"

#include "common/common_thread_manager.h"
#include "common/common_thread.h"

#include "testing/gtest/include/gtest/gtest.h"
#include "testing/gmock/include/gmock/gmock.h"

#include "canscope/device/canscope_device_manager.h"
#include "canscope/device/canscope_device_manager_handle.h"
#include "canscope/canscope_notification_types.h"
#include "canscope/device/canscope_device_property_constants.h"
#include "canscope/device/device_thread_mock.h"
#include "canscope/device/property/device_property_observer_mock.h"

using namespace base;
using namespace common;
using namespace canscope;

using testing::Invoke;

namespace {
static const char kOscConfig [] =  {" \
{ \
  \"CANH.Range\" : 3, \
  \"CANH.Offset\" : 0.0, \
  \"CANH.Coupling\" : 1, \
  \"CANL.Range\" : 3, \
  \"CANL.Offset\" : 0.0, \
  \"CANL.Coupling\" : 1, \
  \"CANDIFF.Range\" : 3, \
  \"CANDIFF.Offset\" : 0.0, \
  \"DiffCtrl\" : 0, \
  \"Time.Base\" : 4, \
  \"Time.Offset\" : 0.0, \
  \"Trigger.AutoTime\" : 100.0, \
  \"Trigger.Source\" : 2, \
  \"Trigger.Type\" : 0, \
  \"Trigger.Mode\" : 0, \
  \"Trigger.Sens\" : 0, \
  \"Trigger.Compare\" : 0, \
  \"Trigger.Volt\" : 0.0, \
  \"TimeParam\" : 1.0 \
} \
"};

DictionaryValue* GetDefaultConfig() {
  std::string content(kOscConfig);
  JSONStringValueSerializer serializer(content);
  Value* value = serializer.Deserialize(NULL, NULL);
  EXPECT_TRUE(NULL != value);
  EXPECT_TRUE(value->IsType(Value::TYPE_DICTIONARY));
  DictionaryValue* dict_value;
  value->GetAsDictionary(&dict_value);
  return dict_value;
}

bool ActionIsDeviceThread() {
  return CommonThread::CurrentlyOn(CommonThread::DEVICE);
}

}

class TestProcess : public common::CommonThreadManager {
public:
  TestProcess();
  virtual ~TestProcess() {}

private:
};
namespace {
TestProcess* g_TestProcess = NULL;
}


TestProcess::TestProcess() {
  if (g_TestProcess) {
    NOTREACHED();
  }
  g_TestProcess = this;
}

TestProcess* GetTestProcess() {
  return g_TestProcess;
}
void DestroyTestProcess() {
  GetTestProcess()->ShutdownThreadsAndCleanUp();
  delete g_TestProcess;
  g_TestProcess = NULL;
}

class CANScopeDeviceManagerTest : public testing::Test
                                , public NotificationObserver {
public:
  CANScopeDeviceManagerTest()
    : event_(true, false)
    , mock_ (NULL) {}
  virtual ~CANScopeDeviceManagerTest() {}

  static void SetUpTestCase() {
    ON_CALL(g_DeviceThreadMock.Get(), 
        IsDeviceThread()).WillByDefault(Invoke(&ActionIsDeviceThread));
    new TestProcess();
    GetTestProcess()->Init();
  }
  static void TearDownTestCase() {
    DestroyTestProcess();
  }

  void GetValueTest();
  void SetValueFileThread();
  static void StartQuit() {
    CANScopeDeviceManager::GetInstance()->StartDestroying();
  }
  void SetValueAndNotifyCheck();

protected:
    DevicePropertyObserverMock* mock_;

private:
  virtual void SetUp() {
    CommonThread::PostTask(CommonThread::DEVICE, FROM_HERE, 
        Bind(&CANScopeDeviceManagerTest::CreateDeviceManager, Unretained(this)));
    event_.Wait();
    CANScopeDeviceManagerHandle::Create();
    registrar_.Add(this, NOTIFICATION_DEVICE_MANAGER_DESTROYED, 
        Source<DeviceManager>(CANScopeDeviceManager::GetInstance()));
  }

  virtual void TearDown() {
    registrar_.RemoveAll();
  }

  void CreateDeviceManager() {
    CANScopeDeviceManager* manager = CANScopeDeviceManager::Create();
    manager->osc_device_config_test()->Update(GetDefaultConfig());
    manager->osc_device()->InitFromConfig();
    manager->osc_device()->Init();
    event_.Signal();
  }

  virtual void Observe(int type, 
                       const NotificationSource& source, 
                       const NotificationDetails& details) {
    if (type == NOTIFICATION_DEVICE_MANAGER_DESTROYED) {
      MessageLoop::current()->QuitWhenIdle();
    }
  }

  WaitableEvent event_;
  NotificationRegistrar registrar_;
};

void CANScopeDeviceManagerTest::GetValueTest() {
  OscDeviceHandle* handle = CANScopeDeviceManagerHandle::GetInstance()->
    osc_device_handle();

  EXPECT_EQ(k8V, handle->volt_range_can_h.value());
  EXPECT_DOUBLE_EQ(0.0, handle->offset_can_h.value());
  EXPECT_EQ(kAC, handle->coupling_can_h.value());
  EXPECT_EQ(k8V, handle->volt_range_can_l.value());
  EXPECT_DOUBLE_EQ(0.0, handle->offset_can_l.value());
  EXPECT_EQ(kAC, handle->coupling_can_l.value());
  EXPECT_EQ(kSub, handle->diff_ctrl.value());
  EXPECT_EQ(k20us, handle->time_base.value());
  EXPECT_DOUBLE_EQ(0.0, handle->time_offset.value());
  EXPECT_DOUBLE_EQ(100, handle->auto_time.value());
  EXPECT_EQ(kTriggerSourceCANDIFF, handle->trigger_source.value());
  EXPECT_EQ(kRisingEdge, handle->trigger_type.value());
  EXPECT_EQ(kAuto, handle->trigger_mode.value());
  EXPECT_EQ(kDefault, handle->trigger_sens.value());
  EXPECT_EQ(kGreater, handle->compare.value());
  EXPECT_DOUBLE_EQ(0.0, handle->trigger_volt.value());
  EXPECT_DOUBLE_EQ(1.0, handle->time_param.value());

  StartQuit();
}

TEST_F(CANScopeDeviceManagerTest, GetValue) {
  CommonThread::PostTask(CommonThread::UI, FROM_HERE, 
      Bind(&CANScopeDeviceManagerTest::GetValueTest, Unretained(this)));
  GetTestProcess()->MainMessageLoopRun();
}

TEST_F(CANScopeDeviceManagerTest, SetValue) {
  OscDeviceHandle* handle = CANScopeDeviceManagerHandle::GetInstance()->
      osc_device_handle();
  EXPECT_EQ(k8V, handle->volt_range_can_h.value());
  handle->volt_range_can_h.set_value(k1V);
  EXPECT_EQ(k1V, handle->volt_range_can_h.value());

  StartQuit();
  GetTestProcess()->MainMessageLoopRun();
}

void CANScopeDeviceManagerTest::SetValueFileThread() {
  CANScopeDeviceManagerHandle::Create();
  OscDeviceHandle* handle = CANScopeDeviceManagerHandle::GetInstance()->
      osc_device_handle();
  EXPECT_EQ(k8V, handle->volt_range_can_h.value());
  handle->volt_range_can_h.set_value(k1V);
  EXPECT_EQ(k1V, handle->volt_range_can_h.value());
}

void CANScopeDeviceManagerTest::SetValueAndNotifyCheck() {
  OscDeviceHandle* handle = CANScopeDeviceManagerHandle::GetInstance()->
      osc_device_handle();
  EXPECT_EQ(k1V, handle->volt_range_can_h.value());
  handle->volt_range_can_h.RemovePrefObserver(mock_);
  mock_ = NULL;
  StartQuit();
}

TEST_F(CANScopeDeviceManagerTest, SetValueAndNotify) {
  OscDeviceHandle* handle = CANScopeDeviceManagerHandle::GetInstance()->
      osc_device_handle();
  DevicePropertyObserverMock mock;
  mock_ = &mock;
  handle->volt_range_can_h.AddPrefObserver(&mock);
  EXPECT_CALL(mock, OnPreferenceChanged(kOscCANHVoltRange)).Times(1)
      .WillOnce(InvokeWithoutArgs(this, 
          &CANScopeDeviceManagerTest::SetValueAndNotifyCheck));

  CommonThread::PostTask(CommonThread::FILE, FROM_HERE, 
      Bind(&CANScopeDeviceManagerTest::SetValueFileThread, Unretained(this)));
  GetTestProcess()->MainMessageLoopRun();
}
