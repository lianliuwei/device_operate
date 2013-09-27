#pragma once

#include "common/common_thread_impl.h"
#include "common/notification/notification_service.h"
#include "canscope/device_errors.h"

class CANScopeThread : public common::CommonThreadImpl {
public:
  explicit CANScopeThread(common::CommonThread::ID identifier);
  CANScopeThread(common::CommonThread::ID identifier,
                 base::MessageLoop* message_loop);
  virtual ~CANScopeThread();

  // base::Thread
  virtual void Init() OVERRIDE;
  virtual void CleanUp() OVERRIDE;
  

private:
  scoped_ptr<common::NotificationService> notifier_;
  scoped_ptr<canscope::device::ScopedDeviceError> device_error_;  
};
