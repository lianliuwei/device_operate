#include "canscope/device/usb_port_device_delegate.h"

#include "canscope/device/register/device_info.h"

namespace canscope {
UsbPortDeviceDelegate::UsbPortDeviceDelegate() {}

UsbPortDeviceDelegate::~UsbPortDeviceDelegate() {}

bool UsbPortDeviceDelegate::WriteDevice(uint32 addr, uint8* buffer, int size) {
  return usb_port.WriteEP1(addr, kUsbModelNormal, buffer, size);
}

bool UsbPortDeviceDelegate::ReadDevice(uint32 addr, uint8* buffer, int size) {
  return usb_port.ReadEP1(addr, kUsbModelNormal, buffer, size);
}

bool UsbPortDeviceDelegate::GetDeviceInfo(DeviceInfo* device_info) {
  DCHECK(device_info);
  return usb_port.GetDeviceInfo(device_info->memory.buffer(), 
      device_info->memory.size());
}

} // namespace canscope
