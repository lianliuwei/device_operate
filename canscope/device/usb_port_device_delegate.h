#pragma once

#include "canscope/device/device_delegate.h"

#include "canscope/device/usb/usb_port.h"

namespace canscope {
class UsbPortDeviceDelegate : public DeviceDelegate {
public:
  UsbPortDeviceDelegate();
  virtual ~UsbPortDeviceDelegate();

  // implement DeviceDelegate
  virtual bool WriteDevice(uint32 addr, uint8* buffer, int size);
  virtual bool ReadDevice(uint32 addr, uint8* buffer, int size);
  virtual bool GetDeviceInfo(DeviceInfo* device_info);

  UsbPort* usb_port_ptr() {
    return &usb_port;
  }
  UsbPort usb_port;
};
} // namespace canscope
