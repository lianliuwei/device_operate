#pragma once

#include "base/basictypes.h"

namespace canscope {
class DeviceInfo;

// set to hardware
class DeviceDelegate {
public:
  // FPGA or arm control
  virtual bool WriteDevice(uint32 addr, uint8* buffer, int size) = 0;
  virtual bool ReadDevice(uint32 addr, uint8* buffer, int size) = 0;
  virtual bool GetDeviceInfo(DeviceInfo* device_info) = 0;
  // I2C

protected:
  DeviceDelegate() {}
  virtual ~DeviceDelegate() {}
};

} // namespace canscope