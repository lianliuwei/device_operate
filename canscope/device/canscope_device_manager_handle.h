#pragma once

#include "common/notification/notification_observer.h"
#include "common/notification/notification_registrar.h"

#include "canscope/device/canscope_device_manager.h"
#include "canscope/device/osc_device_handle.h"

namespace canscope {

class CANScopeDeviceManagerHandle : public common::NotificationObserver {
public:
  // each messageloop thread has at most one DeviceManagerHandle
  static CANScopeDeviceManagerHandle* GetInstance();
  
  static CANScopeDeviceManagerHandle* Create();
  // receive manager destroy notify
  // or user call this.
  void DestroyHandle();

  OscDeviceHandle* osc_device_handle() { return osc_device_handle_.get(); }

  virtual void Observe(int type, 
                       const common::NotificationSource& source, 
                       const common::NotificationDetails& details);

private:
  void Init();

  CANScopeDeviceManagerHandle(CANScopeDeviceManager* manager);
  virtual ~CANScopeDeviceManagerHandle() {}

  common::NotificationRegistrar registrar_;

  scoped_ptr<OscDeviceHandle> osc_device_handle_;

  scoped_refptr<CANScopeDeviceManager> device_manager_;
};

} // namespace canscope
