#include "common/canscope_thread.h"


using namespace common;
using namespace canscope::device;

CANScopeThread::CANScopeThread(common::CommonThread::ID identifier)
    : CommonThreadImpl(identifier) { }

CANScopeThread::CANScopeThread(common::CommonThread::ID identifier, 
                               base::MessageLoop* message_loop)
    : CommonThreadImpl(identifier, message_loop) { }

void CANScopeThread::Init() {
  notifier_.reset(NotificationService::Create());
  device_error_.reset(new ScopedDeviceError());
}

void CANScopeThread::CleanUp() {
  device_error_.reset(NULL);
  notifier_.reset(NULL);
}

CANScopeThread::~CANScopeThread() {
  // see comment in CommonthreadImpl::~CommonThreadImpl
  Stop();
}
