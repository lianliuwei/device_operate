#include "testing/gtest/include/gtest/gtest.h"

#include <string>

#include "base/string_number_conversions.h"

#include "canscope/device/osc_device/osc_device.h"
#include "canscope/device/usb_port_device_delegate.h"
#include "canscope/device/test/scoped_open_device.h"
#include "canscope/device/config_util.h"
#include "canscope/device/config_manager.h"
#include "canscope/device/osc_device/osc_device.h"
#include "canscope/device/osc_device/osc_device_handle.h"
#include "canscope/device/osc_device/osc_data_collecter.h"
#include "canscope/device/sync_call.h"
#include "canscope/test/test_process.h"
#include "canscope/test/speed_meter.h"

using namespace canscope;
using namespace std;
using namespace base;
using namespace common;
using namespace canscope::device;

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
}

#define EXPECT_TRUE_OR_RET(ret) \
{ \
  EXPECT_EQ(true, (ret)); \
  if(!(ret)) \
  return; \
}

#define EXPECT_OK_OR_RET(err) \
{ \
  EXPECT_EQ(canscope::device::OK, (err)) << canscope::device::ErrorToString((err)); \
  if((err) != canscope::device::OK) \
    return; \
}

class OscDeviceTest : public testing::Test {
public:
  OscDeviceTest()
      : device_delegate_(CreateDeviceDelegate())
      , open_device_(device_delegate_.get()) {

  }
  ~OscDeviceTest() {}

protected:
  virtual void SetUp() {
    EXPECT_TRUE_OR_RET(open_device_.IsOpen());
    device_delegate_->DetachFromThread();
    new TestProcess();
    GetTestProcess()->Init();

    SyncCall sync_call(CommonThread::GetMessageLoopProxyForThread(CommonThread::DEVICE));
    sync_call.CallClosure(Bind(&OscDeviceTest::InitDevice, Unretained(this)));
    osc_device_handle_.reset(new OscDeviceHandle(osc_device_.get()));
  }

  virtual void TearDown() {
    EXPECT_TRUE_OR_RET(open_device_.IsOpen());

    osc_device_handle_.reset(NULL);
    SyncCall sync_call(CommonThread::GetMessageLoopProxyForThread(CommonThread::DEVICE));
    sync_call.CallClosure(Bind(&OscDeviceTest::DeleteDevice, Unretained(this)));

    DestroyTestProcess();
  }
  
  void InitDevice() {
    osc_device_.reset(new OscDevice(device_delegate_.get(), 
        &osc_device_config_));
    osc_device_->set_run_thread(MessageLoopProxy::current());
    osc_device_->set_device_type(open_device_.type());
    osc_device_config_.Update(GetConfig(kOscConfig));
    osc_device_->InitFromConfig();
    osc_device_->Init();
    osc_device_->SetAll();

    osc_data_collecter_ = new OscDataCollecter(device_delegate_.get(), osc_device_.get());
    osc_data_collecter_->set_run_thread(MessageLoopProxy::current());
  }

  void DeleteDevice() {
    osc_data_collecter_->Stop();
    osc_data_collecter_ = NULL;
    osc_device_.reset(NULL);
    Error error = device_delegate_->CloseDevice();

    EXPECT_OK_OR_RET(error);
  }

  bool IsOpen() {
    return open_device_.IsOpen();
  }

protected:
  ConfigManager osc_device_config_;
  scoped_ptr<DeviceDelegate> device_delegate_;
  ScopeOpenDevice open_device_;
  scoped_ptr<OscDevice> osc_device_;
  scoped_ptr<OscDeviceHandle> osc_device_handle_;
  scoped_refptr<OscDataCollecter> osc_data_collecter_;
};

TEST_F(OscDeviceTest, Collect) {
  EXPECT_TRUE_OR_RET(IsOpen());

  osc_data_collecter_->Start();
  scoped_refptr<OscRawDataQueue> data_queue =
      osc_data_collecter_->RawDataQueue();
  OscRawDataQueue::Reader reader(data_queue);
  OscRawDataHandle raw_data;
  int64 num;
  SpeedMeter meter(TimeDelta::FromSeconds(1));
  while (true) {
    bool ret = reader.WaitGetBulk(&raw_data, &num);
    if (ret) {
       //cout << "Get Raw Data"
       //    << " detail: " << base::HexEncode(raw_data->data() + 1000, 200) << endl;
    }
    meter.set_size(raw_data->size());
    if (meter.DeltaPass()) {
      cout << meter.FormatSpeedAndTotal() << endl;
    }
   if (meter.total_count() > 1000) {
     break;
   }
  }
}
