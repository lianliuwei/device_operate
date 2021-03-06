#include "base/values.h"
#include "base/message_loop.h"

#include "common/common_thread.h"

#include "testing/gtest/include/gtest/gtest.h"
#include "testing/gmock/include/gmock/gmock.h"

#include "device/property/device_property_observer_mock.h"

#include "canscope/test/test_process.h"
#include "canscope/device/canscope_device.h"
#include "canscope/device/canscope_device_handle.h"
#include "canscope/canscope_notification_types.h"
#include "canscope/device/canscope_device_property_constants.h"
#include "canscope/device/scoped_device_property_commit.h"
#include "canscope/device/devices_manager.h"
#include "canscope/device/canscope_device_constants.h"
#include "canscope/test/speed_meter.h"
#include "canscope/device/config_util.h"
#include "canscope/device/sync_call.h"

using namespace std;
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

}

class DevicesManagerTest : public testing::Test
                         , public NotificationObserver {
public:
  DevicesManagerTest()
    : mock_ (NULL) {}
  virtual ~DevicesManagerTest() {}

  static void SetUpTestCase() {
    new TestProcess();
    GetTestProcess()->Init();
  }
  static void TearDownTestCase() {
    DestroyTestProcess();
  }

  void GetOscRawData();
  void GetValueTest();

  void StopDeviceManager() {
    DevicesManager::Get()->CloseDevice(device_path_);
  }

protected:

  DevicePropertyObserverMock* mock_;
  scoped_refptr<CANScopeDeviceFinder> finder_;
  string16 device_path_;
  scoped_refptr<CANScopeDevice> manager_;


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
    SyncCall sync_call(CommonThread::GetMessageLoopProxyForThread(CommonThread::DEVICE));
    sync_call.CallClosure(Bind(&DevicesManagerTest::InitDeviceManager, Unretained(this)));
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
      return;
    }
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


  NotificationRegistrar registrar_;
};

void DevicesManagerTest::GetValueTest() {
  ASSERT_TRUE(manager_.get() != NULL) << "no Devices Found";
  CANScopeDeviceHandle::Create(manager_);
 OscDeviceHandle* handle = &(CANScopeDeviceHandle::GetInstance(manager_)->
    osc_device_handle);

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

  CANScopeDeviceHandle::GetInstance(manager_)->DestroyHandle();

  CommonThread::PostTask(CommonThread::UI, FROM_HERE,
      Bind(&DevicesManagerTest::StopDeviceManager, Unretained(this)));
}

TEST_F(DevicesManagerTest, GetOscRawDataSpeed) {
  ASSERT_TRUE(manager_.get() != NULL) << "no Devices Found";
  scoped_refptr<OscRawDataQueue> data_queue =
      manager_->runner()->osc_data->RawDataQueue();
  OscRawDataQueue::Reader reader(data_queue);
  OscRawDataHandle raw_data;
  int64 num;
  SpeedMeter meter(TimeDelta::FromSeconds(1));
  while (true) {
    bool ret = reader.WaitGetBulk(&raw_data, &num);
//     if (ret) {
//       LOG(INFO) << "Get Raw Data"
//         << " detail: " << base::HexEncode(raw_data->data() + 1000, 200);
//     }
    meter.set_size(raw_data->size());
    if (meter.DeltaPass()) {
      cout << meter.FormatSpeedAndTotal() << endl;
    }
   if (meter.total_count() > 1000) {
     break;
   }
  }
  StopDeviceManager();
}

TEST_F(DevicesManagerTest, GetValue) {
 CommonThread::PostTask(CommonThread::UI, FROM_HERE,
      Bind(&DevicesManagerTest::GetValueTest, Unretained(this)));
  GetTestProcess()->MainMessageLoopRun();
}