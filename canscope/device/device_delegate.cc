#include "canscope/device/device_delegate.h"

#include "base/command_line.h"

#include "canscope/device/usb_port_device_delegate.h"
#include "canscope/device_simulate/simulate_device_delegate.h"

namespace canscope {

// TODO add device_simulate here
DeviceDelegate* canscope::CreateDeviceDelegate() {
  bool no_simulate = CommandLine::ForCurrentProcess()->HasSwitch("No-Simulate");
  
  if (!no_simulate) {
    return new SimulateDeviceDelegate(true);
  } else {
    return new UsbPortDeviceDelegate();
  }
}

} // namespace canscope