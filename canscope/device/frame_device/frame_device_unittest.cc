#include "testing/gtest/include/gtest/gtest.h"

#include "base/string_number_conversions.h"

#include "canscope/device/device_delegate.h"
#include "canscope/device/test/scoped_open_device.h"
#include "canscope/device/config_util.h"
#include "canscope/device/frame_device/frame_device.h"
#include "canscope/device/frame_device/frame_device_handle.h"
#include "canscope/device/frame_device/frame_data_collecter.h"
#include "canscope/device/sync_call.h"
#include "canscope/device/register/soft_diff_register.h"
#include "canscope/device/register/sja1000_register.h"
#include "canscope/device/config_manager.h"
#include "canscope/device/test/device_util.h"
#include "canscope/test/test_process.h"

using namespace std;
using namespace base;
using namespace common;
using namespace canscope;
using namespace canscope::device;

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
      : device_delegate_(CreateDeviceDelegate())
      , open_device_(device_delegate_.get()) {

  }
  ~FrameDeviceTest() {}

protected:
  virtual void SetUp() {
    EXPECT_TRUE_OR_RET(open_device_.IsOpen());

    new TestProcess();
    GetTestProcess()->Init();

    SyncCall sync_call(CommonThread::GetMessageLoopProxyForThread(CommonThread::DEVICE));
    sync_call.CallClosure(Bind(&FrameDeviceTest::InitDevice, Unretained(this)));
    frame_device_handle_.reset(new FrameDeviceHandle(frame_device_.get()));
  }
  
  virtual void TearDown() {
    frame_device_handle_.reset(NULL);
    SyncCall sync_call(CommonThread::GetMessageLoopProxyForThread(CommonThread::DEVICE));
    sync_call.CallClosure(Bind(&FrameDeviceTest::DeleteDevice, Unretained(this)));

    DestroyTestProcess();
  }

  void InitDevice() {
    frame_device_.reset(new FrameDevice(device_delegate_.get(), 
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
  scoped_ptr<DeviceDelegate> device_delegate_;
  ScopedOpenDevice open_device_;
  scoped_ptr<FrameDevice> frame_device_;
  scoped_ptr<FrameDeviceHandle> frame_device_handle_;
  SoftDiffRegister soft_diff_;
};

#define EXPECT_OK_OR_RET(err) \
{ \
  EXPECT_EQ(canscope::device::OK, (err)) << canscope::device::ErrorToString((err)); \
  if((err) != canscope::device::OK) \
    return; \
}

TEST_F(FrameDeviceTest, Send) {
  FpgaFrameData data;
  for (int i = 0; i < arraysize(data.data); ++i) {
    data.data[i] = i;
  }
  Error err;

  {
  // HACK no send one first lead to read frame_storage fault
  // happen one time
  // may be concurrency write device lead.
  FrameStorageRegister frame_storage;
  err = ReadDevice(device_delegate_.get(), frame_storage.memory);
  EXPECT_OK_OR_RET(err);
  }

  err = frame_device_handle_->FpgaSend(data, 100);
  EXPECT_OK_OR_RET(err);
  
  FrameStorageRegister frame_storage;
  err = ReadDevice(device_delegate_.get(), frame_storage.memory);
  EXPECT_OK_OR_RET(err);
  cout << "Frame Num: " << frame_storage.frame_num.value() * 1.0 / kFrameSize << endl;
  EXPECT_GT(frame_storage.frame_num.value(), 0u);
}

TEST_F(FrameDeviceTest, SendAndReceive) {
  scoped_refptr<FrameDataCollecter> data_collecter(
      new FrameDataCollecter(device_delegate_.get(), frame_device_.get(), true));
  data_collecter->set_run_thread(
      CommonThread::GetMessageLoopProxyForThread(CommonThread::DEVICE));
  data_collecter->Start();
  FrameRawDataQueue::Reader reader(data_collecter->RawDataQueue());
  Error err;

  // send and receive
  FpgaFrameData data;
  for (int i = 0; i < arraysize(data.data); ++i) {
    data.data[i] = i;
  }

  err = frame_device_handle_->FpgaSend(data, 1);
  EXPECT_OK_OR_RET(err);

  FrameRawDataHandle raw_data;
  bool ret = reader.WaitGetBulk(&raw_data, NULL);
  EXPECT_TRUE(ret);
  cout << "raw_data: " << HexEncode(raw_data->data(), raw_data->real_size()) << endl;
}

TEST(FrameDeviceTest2, TestRegister) {
  scoped_ptr<DeviceDelegate> device_delegate(CreateDeviceDelegate());
  ScopedOpenDevice open_device(device_delegate.get());
  Error err;

  EXPECT_TRUE_OR_RET(open_device.IsOpen());
  
  SJA1000Register sja1000;
  sja1000.slient.set_value(true);
  sja1000.sja_btr.set_value(0x1400);
  err = WriteDeviceRange(device_delegate.get(), sja1000.memory, 0, 4);
  EXPECT_OK_OR_RET(err);
  while (true) {
    err = ReadDeviceRange(device_delegate.get(), sja1000.memory, 0x14, 4);
    EXPECT_OK_OR_RET(err);
    if (sja1000.cfg.value()) {
      break;
    }
  }

  FrameStorageRegister frame_storage;
  frame_storage.frame_depth.set_value(48 * 1000);
  err = WriteDevice(device_delegate.get(), frame_storage.memory);  
  EXPECT_OK_OR_RET(err);

  WaveStorageRegister wave_storage;
  wave_storage.frame_len.set_value(0x36000);
  wave_storage.wave_start.set_value(200000);
  wave_storage.wave_end.set_value(200000000);
  err = WriteDevice(device_delegate.get(), wave_storage.memory);
  EXPECT_OK_OR_RET(err);

  SleepMs(100);
  SoftDiffRegister soft_diff;
  soft_diff.sja_btr.set_value(0x1440);
  soft_diff.sys_cfg.set_value(true);
  soft_diff.fil_div.set_value(5);
  err = WriteDeviceRange(device_delegate.get(), soft_diff.memory, 
      soft_diff.FilDivOffset(), soft_diff.FilDivSize());  
  EXPECT_OK_OR_RET(err);
  err = WriteDeviceRange(device_delegate.get(), soft_diff.memory, 
      soft_diff.SysOffset(), soft_diff.SysSize());
  EXPECT_OK_OR_RET(err);

  FpgaSendRegister send_register;
  send_register.btr_div.set_value(99);
  send_register.send_num.set_value(20);
  FpgaFrameData data;
  for (int i = 0; i < arraysize(data.data); ++i) {
    data.data[i] = i;
  }
  send_register.set_frame_data(data);
  err = WriteDeviceRange(device_delegate.get(),
      send_register.memory, 0, 0x1C);
  send_register.start_send.set_value(true);
  err = WriteDeviceRange(device_delegate.get(),
    send_register.memory, send_register.StateOffset(), send_register.StateSize());
  EXPECT_OK_OR_RET(err);

  SleepMs(100);
  err = ReadDevice(device_delegate.get(), frame_storage.memory);
  EXPECT_OK_OR_RET(err);
  EXPECT_GT(frame_storage.frame_num.value(), 0u);

  EXPECT_OK_OR_RET(err);
}