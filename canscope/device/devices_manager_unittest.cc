#include "base/values.h"
#include "base/message_loop.h"
#include "base/synchronization/waitable_event.h"
#include "base/json/json_string_value_serializer.h"
#include "base/json/json_file_value_serializer.h"
#include "base/json/json_reader.h"

#include "common/common_thread.h"

#include "testing/gtest/include/gtest/gtest.h"
#include "testing/gmock/include/gmock/gmock.h"

#include "canscope/test/test_process.h"
#include "canscope/device/canscope_device_manager.h"
#include "canscope/device/canscope_device_manager_handle.h"
#include "canscope/canscope_notification_types.h"
#include "canscope/device/canscope_device_property_constants.h"
#include "canscope/device/device_thread_mock.h"
#include "canscope/device/property/device_property_observer_mock.h"
#include "canscope/device/scoped_device_property_commit.h"
#include "canscope/device/devices_manager.h"
#include "canscope/device/canscope_device_constants.h"

using namespace std;
using namespace base;
using namespace common;
using namespace canscope;

using testing::Invoke;

namespace {
static const char kOscConfig [] =  {" \
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
  } \
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

class DevicesManagerTest : public testing::Test
                         , public NotificationObserver {
public:
  DevicesManagerTest()
    : event_(true, false)
    , mock_ (NULL) {}
  virtual ~DevicesManagerTest() {}

  static void SetUpTestCase() {
    ON_CALL(g_DeviceThreadMock.Get(), 
        IsDeviceThread()).WillByDefault(Invoke(&ActionIsDeviceThread));
    new TestProcess();
    GetTestProcess()->Init();
  }
  static void TearDownTestCase() {
    DestroyTestProcess();
  }

  void GetOscRawData() {
    CommonThread::PostDelayedTask(CommonThread::UI, FROM_HERE, 
      Bind(&DevicesManagerTest::StopDeviceManager, Unretained(this)),
      base::TimeDelta::FromSeconds(3));
  }

  void StopDeviceManager() {
    DevicesManager::Get()->CloseDevice(device_path_);
  }

protected:

  DevicePropertyObserverMock* mock_;
  scoped_refptr<CANScopeDeviceFinder> finder_;
  string16 device_path_;
  scoped_refptr<CANScopeDeviceManager> manager_;


private:
  virtual void SetUp() {
    scoped_refptr<SingleThreadTaskRunner> device_thread = 
      CommonThread::GetMessageLoopProxyForThread(common::CommonThread::DEVICE);
    DevicesManager::Create(device_thread, false, false, false);
    finder_ = new CANScopeDeviceFinder(device_thread, true);
    finder_->Start();

    // wait for DeviceEnum
    SleepMs(1000);
    vector<string16> device_list =  DevicesManager::Get()->GetDeviceList();
    if (device_list.size() == 0)
      return;
    device_path_ = device_list[0];
    DevicesManager::Get()->OpenDevice(device_path_);
    CommonThread::PostTask(CommonThread::DEVICE, FROM_HERE, 
        Bind(&DevicesManagerTest::InitDeviceManager, Unretained(this)));
    event_.Wait();
    if (manager_.get() == NULL)
      return;
    registrar_.Add(this, NOTIFICATION_DEVICE_MANAGER_START_DESTROY, 
        Source<DeviceManager>(manager_.get()));
  }

  virtual void TearDown() {
    registrar_.RemoveAll();
    DevicesManager::Destroy();
  }

  void InitDeviceManager() {
    manager_ = DevicesManager::Get()->GetDeviceManager(device_path_);
    if (manager_.get() == NULL) {
      event_.Signal();
      return;
    }
    manager_->Init(GetDefaultConfig());
    event_.Signal();
  }

  virtual void Observe(int type, 
                       const NotificationSource& source, 
                       const NotificationDetails& details) {
    if (type == NOTIFICATION_DEVICE_MANAGER_START_DESTROY) {
      manager_ = NULL;
      finder_->Stop();
      MessageLoop::current()->QuitWhenIdle();
    }
  }


  WaitableEvent event_;
  NotificationRegistrar registrar_;
};

TEST_F(DevicesManagerTest, GetOscRawData) {
  ASSERT_TRUE(manager_.get() != NULL) << "no Devices Finded";
  CommonThread::PostTask(CommonThread::UI, FROM_HERE, 
      Bind(&DevicesManagerTest::GetOscRawData, Unretained(this)));
  GetTestProcess()->MainMessageLoopRun();
}