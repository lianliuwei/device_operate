#pragma once

#include "common/common_thread_manager.h"
#include "common/common_thread_delegate.h"
#include "common/notification/notification_service.h"
#include "common/notification/notification_registrar.h"
#include "common/notification/notification_observer.h"

#include "canscope/device_errors.h"
#include "canscope/app/canscope_chnl_calc.h"
#include "canscope/device/canscope_device.h"
#include "canscope/ui/canscope_window.h"

namespace canscope {

class CANScopeProcess : public common::CommonThreadManager
                      , public common::CommonThreadDelegate
                      , public common::NotificationObserver {
public:
  CANScopeProcess();
  virtual ~CANScopeProcess();

  void Init();

  void CreateUI();
  void CreateHandle();
  void CreateCalc();

  void Close();

  static CANScopeProcess* Get();
  static void Destroy();

  scoped_refptr<ChnlCalcResultQueue> GetChnlCalcResultQueue(CANScopeDevice* device);

private:
  // implement CommonThreadDelegate
  virtual void Init(common::CommonThread::ID id);
  virtual void CleanUp(common::CommonThread::ID id);

  virtual void Observe(int type, 
                       const common::NotificationSource& source, 
                       const common::NotificationDetails& details);

  struct CANRecordThread {
    scoped_ptr<common::NotificationService> notifier;
    scoped_ptr<canscope::device::ScopedDeviceError> device_error;
  };

  CANRecordThread threads_[common::CommonThread::ID_COUNT];

  scoped_ptr<CANScopeWindow> view_;
  scoped_refptr<CANScopeChnlCalc> chnl_calc_;
  scoped_refptr<CANScopeDeviceFinder> device_finder_;

  // now only take care one device;
  CANScopeDevice* device_;

  common::NotificationRegistrar registrar_;

  DISALLOW_COPY_AND_ASSIGN(CANScopeProcess);
};

} // namespace canscope