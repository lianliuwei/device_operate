#pragma once

#include "common/notification/notification_observer.h"
#include "common/notification/notification_registrar.h"

#include "canscope/device/canscope_device.h"
#include "canscope/device/osc_device/osc_device_handle.h"
#include "canscope/device/frame_device/frame_device_handle.h"

namespace canscope {

class CANScopeDeviceHandle : public common::NotificationObserver {
public:
  // each messageloop thread has at most one DeviceManagerHandle
  static CANScopeDeviceHandle* GetInstance(CANScopeDevice* manager);
  
  static CANScopeDeviceHandle* Create(CANScopeDevice* manager);

  // receive manager destroy notify
  // or user call this.
  void DestroyHandle();

  OscDeviceHandle osc_device_handle;
  FrameDeviceHandle frame_device_handle;

private:
  virtual void Observe(int type, 
                       const common::NotificationSource& source, 
                       const common::NotificationDetails& details);


  CANScopeDeviceHandle(CANScopeDevice* manager);
  virtual ~CANScopeDeviceHandle();

  common::NotificationRegistrar registrar_;

  scoped_refptr<CANScopeDevice> device_manager_;

  DISALLOW_COPY_AND_ASSIGN(CANScopeDeviceHandle);
};

} // namespace canscope
