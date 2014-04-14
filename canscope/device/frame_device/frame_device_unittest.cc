#include "testing/gtest/include/gtest/gtest.h"

#include "canscope/device/osc_device.h"
#include "canscope/device/usb_port_device_delegate.h"
#include "canscope/device/test/scoped_open_device.h"
#include "canscope/device/test/test_util.h"
#include "canscope/device/frame_device/frame_device.h"
#include "canscope/device/frame_device/frame_device_handle.h"
#include "canscope/device/sync_call.h"
#include "canscope/device/register/soft_diff_register.h"
#include "canscope/device/config_manager.h"
#include "canscope/test/test_process.h"

using namespace canscope;
using namespace std;
using namespace base;
using namespace common;

namespace {

static const char kFrameConfig[] = {" \
{ \
  \"DeviceEnable\": true, \
  \"AckEnable\": true, \
  \"SjaBtr\": 5184, \
  \"FrameStoragePercent\": 50.0, \
  \"BitSampleRate\": 5000, \
  \"BitNum\": 180 \
} \
"};

}

#define EXPECT_TRUE_OR_RET(ret) \
{ \
  EXPECT_EQ(true, (ret)); \
  if(!(ret)) \
  return; \
}

class FrameDeviceTest : public testing::Test {
public:
  FrameDeviceTest()
      : device_delegate_()
      , open_device_(&device_delegate_) {

  }
  ~FrameDeviceTest() {}

protected:
  virtual void SetUp() {
    new TestProcess();
    GetTestProcess()->Init();

    SyncCall sync_call(CommonThread::GetMessageLoopProxyForThread(CommonThread::DEVICE));
    sync_call.set_callback(Bind(&FrameDeviceTest::InitDevice, Unretained(this)));
    sync_call.Call();
    frame_device_handle_.reset(new FrameDeviceHandle(frame_device_.get()));
  }
  
  virtual void TearDown() {
    frame_device_handle_.reset(NULL);
    SyncCall sync_call(CommonThread::GetMessageLoopProxyForThread(CommonThread::DEVICE));
    sync_call.set_callback(Bind(&FrameDeviceTest::DeleteDevice, Unretained(this)));
    sync_call.Call();

    DestroyTestProcess();
  }

  void InitDevice() {
    EXPECT_TRUE_OR_RET(open_device_.IsOpen());
    frame_device_.reset(new FrameDevice(&device_delegate_, 
          &frame_device_config_, &soft_diff_));
    frame_device_->set_run_thread(MessageLoopProxy::current());
    frame_device_->set_device_type(open_device_.type());
    frame_device_config_.Update(GetConfig(kFrameConfig));
    frame_device_->InitFromConfig();
    frame_device_->Init();
    frame_device_->SetAll();
  }

  void DeleteDevice() {
    frame_device_.reset(NULL);
  }

  ConfigManager frame_device_config_;
  UsbPortDeviceDelegate device_delegate_;
  ScopeOpenDevice open_device_;
  scoped_ptr<FrameDevice> frame_device_;
  scoped_ptr<FrameDeviceHandle> frame_device_handle_;
  SoftDiffRegister soft_diff_;
};

#define EXPECT_OK_OR_RET(err) \
{ \
  EXPECT_EQ(canscope::device::OK, (err)) << ErrorToString((err)); \
  if(!(err)) \
  return; \
}

TEST_F(FrameDeviceTest, Send) {
  FpgaFrameData data;
  for (int i = 0; i < arraysize(data.data); ++i) {
    data.data[i] = i;
  }
  canscope::device::Error err = frame_device_handle_->FpgaSend(data, 1);
  EXPECT_OK_OR_RET(err);

}