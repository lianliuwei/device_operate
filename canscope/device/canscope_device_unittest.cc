#include "base/values.h"
#include "base/message_loop.h"

#include "common/common_thread.h"

#include "base/json/json_file_value_serializer.h"

#include "testing/gtest/include/gtest/gtest.h"
#include "testing/gmock/include/gmock/gmock.h"

#include "canscope/test/test_process.h"
#include "canscope/device/canscope_device.h"
#include "canscope/device/canscope_device_handle.h"
#include "canscope/canscope_notification_types.h"
#include "canscope/device/canscope_device_property_constants.h"
#include "canscope/device/device_thread_mock.h"
#include "canscope/device/property/device_property_observer_mock.h"
#include "canscope/device/scoped_device_property_commit.h"
#include "canscope/device/test/test_util.h"
#include "canscope/device/sync_call.h"

using namespace base;
using namespace common;
using namespace canscope;

using testing::Invoke;

namespace {
static const char kCANScopeConfig [] =  {" \
{ \"OscDevice\" : \
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
  }, \
  \"FrameDevice\" : \
  { \
    \"DeviceEnable\": true, \
    \"AckEnable\": true, \
    \"SjaBtr\": 5184, \
    \"FrameStoragePercent\": 50.0, \
    \"BitSampleRate\": 5000, \
    \"BitNum\": 180 \
  } \
} \
"};


bool ActionIsDeviceThread() {
  return CommonThread::CurrentlyOn(CommonThread::DEVICE);
}

}

class CANScopeDeviceTest : public testing::Test
                         , public NotificationObserver {
public:
  CANScopeDeviceTest()
    : mock_ (NULL)
    , manager_(NULL) {}
  virtual ~CANScopeDeviceTest() {}

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
  void StartQuit() {
    manager_->StartDestroying();
  }
  void SetValueAndNotifyCheck();

protected:
  DevicePropertyObserverMock* mock_;

  OscDeviceHandle* GetOscDeviceHandle() {
    return &(CANScopeDeviceHandle::GetInstance(manager_)->osc_device_handle); 
  }
  OscDevice* GetOscDevice() {
    return manager_->osc_device();
  }

  CANScopeDevice* GetCANScopeDevice() {
    return manager_;
  }

private:
  virtual void SetUp() {
    SyncCall sync_call(CommonThread::GetMessageLoopProxyForThread(CommonThread::DEVICE));
    sync_call.set_callback(Bind(&CANScopeDeviceTest::CreateCANScopeDevice, Unretained(this)));
    sync_call.Call();
    CANScopeDeviceHandle::Create(manager_);
    registrar_.Add(this, NOTIFICATION_DEVICE_MANAGER_DESTROYED, 
        Source<DeviceManager>(manager_));
  }

  virtual void TearDown() {
    registrar_.RemoveAll();
  }

  void CreateCANScopeDevice() {
    manager_ = CANScopeDevice::Create(MessageLoopProxy::current());
    manager_->Init(GetConfig(kCANScopeConfig));
  }

  virtual void Observe(int type, 
                       const NotificationSource& source, 
                       const NotificationDetails& details) {
    if (type == NOTIFICATION_DEVICE_MANAGER_DESTROYED) {
      MessageLoop::current()->QuitWhenIdle();
    }
  }
  CANScopeDevice* manager_;
  NotificationRegistrar registrar_;
};

void CANScopeDeviceTest::GetValueTest() {
  OscDeviceHandle* handle = GetOscDeviceHandle();

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

TEST_F(CANScopeDeviceTest, GetValue) {
  CommonThread::PostTask(CommonThread::UI, FROM_HERE, 
      Bind(&CANScopeDeviceTest::GetValueTest, Unretained(this)));
  GetTestProcess()->MainMessageLoopRun();
}

TEST_F(CANScopeDeviceTest, SetValue) {
  OscDeviceHandle* handle = GetOscDeviceHandle();

  EXPECT_EQ(k8V, handle->volt_range_can_h.value());
  handle->volt_range_can_h.set_value(k1V);
  EXPECT_EQ(k1V, handle->volt_range_can_h.value());

  StartQuit();
  GetTestProcess()->MainMessageLoopRun();
}

void CANScopeDeviceTest::SetValueFileThread() {
  CANScopeDeviceHandle::Create(manager_);
  OscDeviceHandle* handle = &(CANScopeDeviceHandle::GetInstance(manager_)->
      osc_device_handle);

  EXPECT_EQ(k8V, handle->volt_range_can_h.value());
  handle->volt_range_can_h.set_value(k1V);
  EXPECT_EQ(k1V, handle->volt_range_can_h.value());
}

void CANScopeDeviceTest::SetValueAndNotifyCheck() {
  OscDeviceHandle* handle = GetOscDeviceHandle();

  EXPECT_EQ(k1V, handle->volt_range_can_h.value());
  handle->volt_range_can_h.RemovePrefObserver(mock_);
  mock_ = NULL;
  StartQuit();
}

TEST_F(CANScopeDeviceTest, SetValueAndNotify) {
  OscDeviceHandle* handle = GetOscDeviceHandle();

  DevicePropertyObserverMock mock;
  mock_ = &mock;
  handle->volt_range_can_h.AddPrefObserver(&mock);
  EXPECT_CALL(mock, OnPreferenceChanged(kOscCANHVoltRange)).Times(1)
      .WillOnce(InvokeWithoutArgs(this, 
          &CANScopeDeviceTest::SetValueAndNotifyCheck));

  CommonThread::PostTask(CommonThread::FILE, FROM_HERE, 
      Bind(&CANScopeDeviceTest::SetValueFileThread, Unretained(this)));
  GetTestProcess()->MainMessageLoopRun();
}


namespace {
void DeviceNoChange(OscDevice* device) {

  EXPECT_EQ(k8V, device->range_can_h.value());
  EXPECT_EQ(0.0, device->offset_can_h.value());
  EXPECT_EQ(kAC, device->coupling_can_h.value());
}

void DeviceChanged(OscDevice* device) {

  EXPECT_EQ(k1V, device->range_can_h.value());
  EXPECT_EQ(1.0, device->offset_can_h.value());
  EXPECT_EQ(kDC, device->coupling_can_h.value());
}

}

TEST_F(CANScopeDeviceTest, BatchSetValue) {
  OscDeviceHandle* handle = GetOscDeviceHandle();

  OscDevice* device = GetOscDevice();
  SyncCall sync_call(CommonThread::GetMessageLoopProxyForThread(CommonThread::DEVICE));
  {
  ScopedDevicePropertyCommit batch(handle, "BatchSetValueTest");
  handle->volt_range_can_h.set_value(k1V);
  handle->offset_can_h.set_value(1.0);
  handle->coupling_can_h.set_value(kDC);
  // handle change but device no change, device change after out of scope
  EXPECT_EQ(k1V, handle->volt_range_can_h.value());
  EXPECT_DOUBLE_EQ(1.0, handle->offset_can_h.value());
  EXPECT_EQ(kDC, handle->coupling_can_h.value());
  sync_call.CallClosure(Bind(&DeviceNoChange, device));
  }
  sync_call.CallClosure(Bind(&DeviceChanged, device));

  StartQuit();
  GetTestProcess()->MainMessageLoopRun();
}

TEST_F(CANScopeDeviceTest, SaveConfig) {
  scoped_ptr<DictionaryValue> dict_value(
      GetCANScopeDevice()->SaveConfig());
  std::string key(kVersionPath);
  dict_value->Set(key, new FundamentalValue(kVersion));
  FilePath path(L"canscope_config.json");
  JSONFileValueSerializer serializer(path);
  bool ret = serializer.Serialize(*(dict_value.get()));
  DCHECK(ret);
}
