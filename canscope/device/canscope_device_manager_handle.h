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


  virtual void Observe(int type, 
                       const common::NotificationSource& source, 
                       const common::NotificationDetails& details);

private:
  CANScopeDeviceManagerHandle(CANScopeDeviceManager* manager);
  virtual ~CANScopeDeviceManagerHandle();

  common::NotificationRegistrar registrar_;

  OscDeviceHandle* osc_device_handle_;

  scoped_refptr<CANScopeDeviceManager> device_manager_;
};

} // namespace canscope
