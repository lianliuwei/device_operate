#include "canscope/app/canscope_process.h"

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
  GetTestProcess()->ShutdownThreadsAndCleanUp();
  delete g_Process;
  g_Process = NULL;

}

} // namespace canscope