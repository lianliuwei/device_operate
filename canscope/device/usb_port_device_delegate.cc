#include "canscope/device/usb_port_device_delegate.h"

#include "canscope/device/register/device_info.h"
#include "canscope/device/register/canscope_device_register_constants.h"

using namespace canscope::device;
namespace canscope {

UsbPortDeviceDelegate::UsbPortDeviceDelegate() {}

UsbPortDeviceDelegate::~UsbPortDeviceDelegate() {}

Error UsbPortDeviceDelegate::WriteDevice(uint32 addr, uint8* buffer, int size) {
  bool ret = usb_port_.WriteEP1(addr, kUsbModelNormal, buffer, size);
  return ret ? OK : ERR_WRITE_DEVICE;
}

Error UsbPortDeviceDelegate::ReadDevice(uint32 addr, uint8* buffer, int size) {
  bool ret = usb_port_.ReadEP1(addr, kUsbModelNormal, buffer, size);
  return ret ? OK : ERR_READ_DEVICE;
}

Error UsbPortDeviceDelegate::GetDeviceInfo(DeviceInfo* device_info) {
  bool ret = usb_port_.GetDeviceInfo(
      device_info->memory.buffer(), 
      device_info->memory.size());
  return ret ? OK : ERR_READ_DEVICE;
}


Error UsbPortDeviceDelegate::EnumDevices(std::vector<string16>* devices) {
  bool ret = canscope::EnumDevices(devices);
  return ret ? OK : ERR_DEVICE_OPEN;
}  

Error UsbPortDeviceDelegate::OpenDevice(string16 device_path) {
  bool ret = usb_port_.OpenDevice(device_path);
  return ret ? OK : ERR_DEVICE_OPEN;
}

Error UsbPortDeviceDelegate::CloseDevice() {
  bool ret = usb_port_.CloseDevice();
  return ret ? OK : ERR_READ_DEVICE;
}

Error UsbPortDeviceDelegate::DownloadFPGA(uint8* buffer, int size) {
  bool ret = usb_port_.DownloadFPGA(buffer, size);
  return ret ? OK : ERR_WRITE_DEVICE;
}

Error UsbPortDeviceDelegate::ReadOscData(uint8* buffer, int size) {
  DCHECK(buffer);
  bool ret = usb_port_.ReadEP3(kScopeReadAddr, 
      kUsbModelStream, buffer, size);
  return ret ? OK : ERR_READ_DEVICE;
}

Error UsbPortDeviceDelegate::ReadFrameData(uint8* buffer, int size) {
  DCHECK(buffer);
  // TODO rewrite when usb_port return Error.
  bool ret = usb_port_.ReadEP3(kFramdReadAddr, kUsbModelStream, buffer, size);
  return ret ? device::OK : ERR_READ_DEVICE;
}

} // namespace canscope
