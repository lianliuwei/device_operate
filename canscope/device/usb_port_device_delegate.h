#pragma once

#include "canscope/device/device_delegate.h"

#include "canscope/device/usb/usb_port.h"

namespace canscope {
class UsbPortDeviceDelegate : public DeviceDelegate {
public:
  // implement DeviceDelegate
  virtual device::Error EnumDevices(std::vector<string16>* devices);
  virtual device::Error OpenDevice(string16 device_path);
  virtual device::Error CloseDevice();
  virtual device::Error DownloadFPGA(uint8* buffer, int size);
  // FPGA or arm control
  virtual device::Error WriteDevice(uint32 addr, uint8* buffer, int size);
  virtual device::Error ReadDevice(uint32 addr, uint8* buffer, int size);
  virtual device::Error GetDeviceInfo(DeviceInfo* device_info);
  virtual device::Error ReadOscData(uint8* buffer, int size);
  virtual device::Error ReadFrameData(uint8* buffer, int size);
  // I2C

  UsbPortDeviceDelegate();
  virtual ~UsbPortDeviceDelegate();


private:
  UsbPort usb_port_;
};
} // namespace canscope
