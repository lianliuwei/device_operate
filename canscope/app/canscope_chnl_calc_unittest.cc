#include "testing/gtest/include/gtest/gtest.h"

#include "canscope/app/canscope_chnl_calc.h"
#include "canscope/device/test/scoped_open_device.h"
#include "canscope/device/config_util.h"
#include "canscope/device/config_manager.h"
#include "canscope/device/osc_device/osc_device.h"
#include "canscope/device/osc_device/osc_data_collecter.h"
#include "canscope/device/sync_call.h"
#include "canscope/test/test_process.h"
#include "canscope/test/speed_meter.h"
#include "canscope/device_errors.h"

using namespace std;
using namespace base;
using namespace common;
using namespace canscope;
using namespace canscope::device;

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

class CANScopeChnlCalcTest : public testing::Test {
public:
  CANScopeChnlCalcTest() 
      : device_delegate_(CreateDeviceDelegate())
      , open_device_(device_delegate_.get())
    {}

  virtual ~CANScopeChnlCalcTest() {}

  static void SetUpTestCase() {
    new TestProcess();
    GetTestProcess()->Init();
  }
  static void TearDownTestCase() {
    DestroyTestProcess();
  }
  
protected:
  virtual void SetUp() {
    EXPECT_TRUE_OR_RET(open_device_.IsOpen());
    device_delegate_->DetachFromThread();

    SyncCall sync_call(CommonThread::GetMessageLoopProxyForThread(CommonThread::DEVICE));
    sync_call.CallClosure(Bind(&CANScopeChnlCalcTest::InitDevice, Unretained(this)));

    chnl_calc_ = new CANScopeChnlCalc(
        CommonThread::GetMessageLoopProxyForThread(CommonThread::FILE),
        osc_data_collecter_->RawDataQueue());
  }

  virtual void TearDown() {
    EXPECT_TRUE_OR_RET(open_device_.IsOpen());

    SyncCall sync_call(CommonThread::GetMessageLoopProxyForThread(CommonThread::DEVICE));
    sync_call.CallClosure(Bind(&CANScopeChnlCalcTest::DeleteDevice, Unretained(this)));

  }

  void InitDevice() {
    osc_device_.reset(new OscDevice(device_delegate_.get(), 
        &osc_device_config_));
    osc_device_->set_run_thread(MessageLoopProxy::current());
    osc_device_->set_device_type(open_device_.type());
    osc_device_config_.Update(GetDefaultOscDeviceConfig());
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

    // HACK or close will fault
    device_delegate_->DetachFromThread();
    EXPECT_OK_OR_RET(error);
  }

  scoped_refptr<CANScopeChnlCalc> chnl_calc_;
  ConfigManager osc_device_config_;
  scoped_ptr<OscDevice> osc_device_;
  scoped_refptr<OscDataCollecter> osc_data_collecter_;
  scoped_ptr<DeviceDelegate> device_delegate_;
  ScopedOpenDevice open_device_;
};

TEST_F(CANScopeChnlCalcTest, normal) {
  osc_data_collecter_->Start();
  chnl_calc_->Start();
  ChnlCalcResultQueue::Reader reader(chnl_calc_->UICalcQueue().get());

  scoped_refptr<ChnlCalcResult> chnl_data;
  int64 num;
  SpeedMeter meter(TimeDelta::FromSeconds(1));
  while (true) {
    bool ret = reader.WaitGetBulk(&chnl_data, &num);
    if (ret) {
       //cout << "Get Raw Data"
       //    << " detail: " << base::HexEncode(raw_data->data() + 1000, 200) << endl;
    }
    meter.set_size(100);
    if (meter.DeltaPass()) {
      cout << meter.FormatSpeedAndTotal() << endl;
    }
   if (meter.total_count() > 1000) {
     break;
   }
  }

  chnl_calc_->Stop();
}