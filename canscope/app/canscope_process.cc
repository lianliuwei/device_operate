#include "canscope/app/canscope_process.h"

#include "base/bind.h"

#include "canscope/canscope_notification_types.h"
#include "canscope/device/devices_manager.h"
#include "canscope/device/canscope_device_handle.h"
#include "canscope/device/canscope_device_finder.h"

#include "canscope/device/register/fpga_send_register.h"
#include "canscope/device/canscope_device_constants.h"

using namespace base;
using namespace common;
using namespace canscope;
using namespace canscope::device;

namespace {
canscope::CANScopeProcess* g_Process = NULL;


uint8 kDefaultFrameData[20] = {
  0x20,
  0x28,
  0x82,
  0x60,
  0x10,
  0x04,
  0x41,
  0x10,
  0x04,
  0x41,
  0x10,
  0x04,
  0x25,
  0x1c,
  0xfe,
  0x07,
  0x00,
  0x00,
  0x00,
  0x00,
};

void FPGASendTask(CANScopeDevice* device) {
  CANScopeDeviceHandle* device_handle = CANScopeDeviceHandle::GetInstance(device);
  DCHECK(device_handle);

//   FpgaFrameData fpga_data;
//   for (int i = 0; i < arraysize(fpga_data.data); ++i) {
//     fpga_data.data[i] = kDefaultFrameData[i];
//   }
//   for (int i = 0; i < 200; ++i) {
//     device_handle->frame_device_handle.FpgaSend(fpga_data, 80);
//     SleepMs(400);
//   }
//   
    
}

}

namespace canscope {

CANScopeProcess::CANScopeProcess()
    : device_(NULL)
    , wait_for_exit_(false) {

  if (g_Process) {
    NOTREACHED();
  }
  g_Process = this;

  for (int i = 0; i < CommonThread::ID_COUNT; ++i) {
    CommonThread::SetDelegate(static_cast<CommonThread::ID>(i), this);
  }

}


CANScopeProcess::~CANScopeProcess() {
  for (int i = 0; i < CommonThread::ID_COUNT; ++i) {
    CommonThread::SetDelegate(static_cast<CommonThread::ID>(i), NULL);
  }
}

void CANScopeProcess::Init(common::CommonThread::ID id) {
  threads_[id].notifier.reset(NotificationService::Create());
  threads_[id].device_error.reset(new ScopedDeviceError);
}

void CANScopeProcess::Init() {
  CommonThreadManager::Init();

  registrar_.Add(this, NOTIFICATION_DEVICE_MANAGER_CREATED, 
      NotificationService::AllSources());
  registrar_.Add(this, NOTIFICATION_DEVICE_MANAGER_START_DESTROY, 
      NotificationService::AllSources());

  DevicesManager::Create(
      CommonThread::GetMessageLoopProxyForThread(CommonThread::DEVICE), true, false, false);

  device_finder_ = 
      new CANScopeDeviceFinder(CommonThread::GetMessageLoopProxyForThread(CommonThread::DEVICE), false);
  device_finder_->Start();
  
}

void CANScopeProcess::CleanUp(common::CommonThread::ID id) {
  threads_[id].notifier.reset(NULL);
  threads_[id].device_error.reset(NULL);
}

CANScopeProcess* CANScopeProcess::Get() {
  return g_Process;
}

// static
void CANScopeProcess::Destroy() {
  CANScopeProcess::Get()->ShutdownThreadsAndCleanUp();
  delete g_Process;
  g_Process = NULL;

}

void CANScopeProcess::Observe(int type, 
                              const NotificationSource& source, 
                              const NotificationDetails& details) {
  if (type == NOTIFICATION_DEVICE_MANAGER_CREATED) {
    Source<CANScopeDevice> device_source(source);
    if (device_) {
      LOG(WARNING) << "no take care of multi-device";
      return;
    }
    device_ = device_source.ptr();
    CreateCalc();
    CreateHandle();
    CreateUI();

  } else if (type == NOTIFICATION_DEVICE_MANAGER_START_DESTROY) {
    Source<CANScopeDevice> device_source(source);
    DCHECK(device_ == device_source.ptr());
    device_ = NULL;
    if (wait_for_exit_)
      MessageLoop::current()->Quit();

  } else {
    NOTREACHED();
  }
}

void CANScopeProcess::CreateUI() {
  DCHECK(view_.get() == NULL);
  DCHECK(device_ != NULL);
  view_.reset(new CANScopeWindow(device_));
}

void CANScopeProcess::CreateCalc() {
  DCHECK(device_ != NULL);
  scoped_refptr<OscRawDataQueue> raw_queue = device_->runner()->osc_data->RawDataQueue();
  chnl_calc_ = new CANScopeChnlCalc(
      CommonThread::GetMessageLoopProxyForThread(CommonThread::CALC), raw_queue);
  chnl_calc_->Start();
}

void CANScopeProcess::CreateHandle() {
  DCHECK(device_);
  CANScopeDeviceHandle::Create(device_);
  CommonThread::PostTask(CommonThread::TASK, FROM_HERE, 
      Bind(IgnoreResult(&CANScopeDeviceHandle::Create), device_));
}

void CANScopeProcess::Close() {
  view_.release();
  chnl_calc_->Stop();
  chnl_calc_ = NULL;
  device_finder_->Stop();
  device_finder_ = NULL;
  DevicesManager::Destroy();
  wait_for_exit_ = true;
}

scoped_refptr<ChnlCalcResultQueue> CANScopeProcess::GetChnlCalcResultQueue(
    CANScopeDevice* device) {
  DCHECK(device == device_);
  return chnl_calc_->UICalcQueue();
}

void CANScopeProcess::SendFrame() {
  CommonThread::PostTask(CommonThread::TASK, FROM_HERE, Bind(&FPGASendTask, Unretained(device_)));
}

} // namespace canscope