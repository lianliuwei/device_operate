#pragma once

#include "canscope/device/usb_port_device_delegate.h"
#include "canscope/device/canscope_device_constants.h"

namespace canscope {

// open the first Device of DeviceList. if fault call gtest EXPECT_
class ScopeOpenDevice {
public:
  ScopeOpenDevice(UsbPortDeviceDelegate* device_delegate);
  ~ScopeOpenDevice();

  bool IsOpen() const { return open_; }
  DeviceType type() const  { return type_; }
private:
  static bool InitDevice(UsbPortDeviceDelegate* device_delegate, DeviceType* type);

  static void CloseDevice(UsbPort* usb_port) {
    usb_port->CloseDevice();
  }

  UsbPortDeviceDelegate* device_delegate_;
  bool open_;
  DeviceType type_;

  DISALLOW_COPY_AND_ASSIGN(ScopeOpenDevice);
};

} // namespace canscope