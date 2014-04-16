#pragma once

#include "common/notification/notification_observer.h"
#include "common/notification/notification_registrar.h"

#include "canscope/device/canscope_device.h"
#include "canscope/device/osc_device/osc_device_handle.h"

namespace canscope {

class CANScopeDeviceHandle : public common::NotificationObserver {
public:
  // each messageloop thread has at most one DeviceManagerHandle
  static CANScopeDeviceHandle* GetInstance(CANScopeDevice* manager);
  
  static CANScopeDeviceHandle* Create(CANScopeDevice* manager);

  // receive manager destroy notify
  // or user call this.
  void DestroyHandle();

  OscDeviceHandle* osc_device_handle() { return osc_device_handle_.get(); }

private:
  virtual void Observe(int type, 
                       const common::NotificationSource& source, 
                       const common::NotificationDetails& details);

  void Init();

  CANScopeDeviceHandle(CANScopeDevice* manager);
  virtual ~CANScopeDeviceHandle();

  common::NotificationRegistrar registrar_;

  scoped_ptr<OscDeviceHandle> osc_device_handle_;

  scoped_refptr<CANScopeDevice> device_manager_;

  DISALLOW_COPY_AND_ASSIGN(CANScopeDeviceHandle);
};

} // namespace canscope
