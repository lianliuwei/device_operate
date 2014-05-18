#include "canscope/app/canscope_process.h"

#include "canscope/canscope_notification_types.h"
#include "canscope/device/devices_manager.h"
#include "canscope/device/canscope_device_handle.h"

using namespace common;
using namespace canscope::device;

namespace {
canscope::CANScopeProcess* g_Process = NULL;
}

namespace canscope {

CANScopeProcess::CANScopeProcess() {
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
  }
  NOTREACHED();
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
      CommonThread::GetMessageLoopProxyForThread(CommonThread::FILE), raw_queue);
}

void CANScopeProcess::CreateHandle() {
  DCHECK(device_);
  CANScopeDeviceHandle::Create(device_);
  // TODO may create handle on calibrate thread.
}

void CANScopeProcess::Close() {
  view_.reset(NULL);
  chnl_calc_ = NULL;
  DevicesManager::Destroy();
  MessageLoop::current()->Quit();
}

scoped_refptr<ChnlCalcResultQueue> CANScopeProcess::GetChnlCalcResultQueue(
    CANScopeDevice* device) {
  DCHECK(device == device_);
  return chnl_calc_->UICalcQueue();
}

} // namespace canscope