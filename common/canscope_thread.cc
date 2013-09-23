#include "common/canscope_thread.h"


using namespace common;

CANScopeThread::CANScopeThread(common::CommonThread::ID identifier)
    : CommonThreadImpl(identifier) { }

CANScopeThread::CANScopeThread(common::CommonThread::ID identifier, 
                               base::MessageLoop* message_loop)
    : CommonThreadImpl(identifier, message_loop) { }

void CANScopeThread::Init() {
  notifier_.reset(NotificationService::Create());
}

void CANScopeThread::CleanUp() {
  notifier_.reset(NULL);
}
