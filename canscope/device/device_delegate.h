#pragma once

#include <vector>

#include "base/basictypes.h"
#include "base/string16.h"

#include "canscope/device_errors.h"

namespace canscope {
class DeviceInfo;

// set to hardware
class DeviceDelegate {
public:
  virtual device::Error EnumDevice(std::vector<string16>* devices) = 0;
  virtual device::Error OpenDevice(string16 device_path) = 0;
  virtual device::Error CloseDevice();
  virtual device::Error DownloadFPGA(uint8* buffer, int size) = 0;
  // FPGA or arm control
  virtual device::Error WriteDevice(uint32 addr, uint8* buffer, int size) = 0;
  virtual device::Error ReadDevice(uint32 addr, uint8* buffer, int size) = 0;
  virtual device::Error GetDeviceInfo(DeviceInfo* device_info) = 0;
  virtual device::Error ReadOscData(uint8* buffer, int size) = 0;
  virtual device::Error ReadFrameData(uint8* buffer, int size) = 0;
  // I2C

  DeviceDelegate() {}
  virtual ~DeviceDelegate() {}
};

} // namespace canscope