#pragma once

#include "canscope/device/frame_device/frame_device.h"

#include "canscope/device/property/property.h"
#include "canscope/device/device_handle_base.h"
#include "canscope/device/register/fpga_send_register.h"

namespace canscope {

class FrameDeviceHandle : public DeviceHandleBase {
public:
  FrameDeviceHandle(FrameDevice* device);
  virtual ~FrameDeviceHandle() {}

  device::Error FpgaSend(const FpgaFrameData& data, uint8 send_num);
  device::Error SetAck(bool ack, uint8 ack_bit);

  // sync device call
  device::Error FpgaSend();
  device::Error SetDstb();
  device::Error SetAck();

  FpgaSendRegister fpga_send;

private:
  // implement DeviceHandleBase
  virtual canscope::ValueMapDevicePropertyStore* DevicePrefs() {
    return &(properties_.prefs_);
  }

  void FpgaSendImpl();

  FrameDevice* device_;

  FrameDeviceProperty properties_;

  // sync call
  device::Error last_err_;
};

} // namespace canscope
