#include "canscope/device/device_delegate.h"

#include "canscope/device/usb_port_device_delegate.h"

namespace canscope {

// TODO add device_simulate here
DeviceDelegate* canscope::CreateDeviceDelegate() {
  return new UsbPortDeviceDelegate();
}

} // namespace canscope