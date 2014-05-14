#pragma once

#include "common/common_thread_manager.h"
#include "common/common_thread_delegate.h"
#include "common/notification/notification_service.h"
#include "canscope/device_errors.h"

namespace canscope {

class CANScopeProcess : public common::CommonThreadManager
                      , public common::CommonThreadDelegate {
public:
  CANScopeProcess();
  virtual ~CANScopeProcess();

  void Init();

  static CANScopeProcess* Get();
  static void Destroy();

private:
  // implement CommonThreadDelegate
  virtual void Init(common::CommonThread::ID id);
  virtual void CleanUp(common::CommonThread::ID id);

  struct CANRecordThread {
    scoped_ptr<common::NotificationService> notifier;
    scoped_ptr<canscope::device::ScopedDeviceError> device_error;
  };

  CANRecordThread threads_[common::CommonThread::ID_COUNT];
};

CANScopeProcess* GetTestProcess();
void DestroyTestProcess();

} // namespace canscope