#include "canscope/device/canscope_device_runner.h"

#include "base/path_service.h"

namespace canscope {


device::Error CANScopeRunner::InitDevice() {
  DCHECK(!inited_) << "device already init";
  device::Error error = InitDeviceImpl(device_path_);
  if (error != device::OK) {
    return error;
  }
  status_ = kOnline;
  inited_ = true;
  if (start_on_device_online) {
    StartAll();
  }
  return error;
}

device::Error CANScopeRunner::ReOnline() {
  DCHECK(inited_) << "device need InitDevice";
  CloseDeviceImpl();
  device::Error error = InitDeviceImpl(device_path_);
  if (error != device::OK) {
    return error;
  }
  inited_ = true;
  if (rerun_on_back_online) {
    ReRunAll();
  }

  return error;
}

device::Error CANScopeRunner::InitDeviceImpl(string16 device_path) {
  UsbPortDeviceDelegate* device_delegate = canscope_->device_delegate_;
  // INFO now  just usb_port, if add more port change device_delegate interface
  UsbPort* usb_port = device_delegate->usb_port_ptr();
  bool ret = false;
  ret = usb_port->OpenDevice(device_path);
  if (!ret) {
    return device::ERR_DEVICE_OPEN;
  }
  ret = device_delegate->GetDeviceInfo(&device_info_);
  if (!ret) {
    usb_port->CloseDevice();
    return device::ERR_DEVICE_INFO;
  }
  // config FPGA
  if (device_info_.fpage_version.value() != 0xFFFFFFFF) {
    return device::OK;
  }
  std::string content;
  base::FilePath fpga_file;
  base::PathService(base::BasePathKey::DIR_EXE, &fpga_file);
  fpga_file = fpga_file.Append(_T("CANScope.dll"));
  ret = file_util::ReadFileToString(fpga_file, &content); 
  if (!ret) {
    usb_port->CloseDevice();
    return device::ERR_DEVICE_LOADFPGAFILE;
  }
  ret = usb_port->DownloadFPGA((uint8*)(content.c_str()), content.size());
  if (!ret) {
    usb_port->CloseDevice();
    return device::ERR_DEVICE_DOWNLOADFPGA;
  }
  return device::OK;
}

void CANScopeRunner::CloseDeviceImpl() {
  UsbPortDeviceDelegate* device_delegate = canscope_->device_delegate_;
  UsbPort* usb_port = device_delegate->usb_port_ptr();
  bool ret = usb_port->CloseDevice();
  DCHECK(!ret);
}

// NOTE may DevicesManager manager the state is better
void CANScopeRunner::DeviceStateChanged() {
  scoped_refptr<DevicesManager> manager = DevicesManager::Get();
  DeviceStatus status = manager->GetDeviceStatus(device_path_);
  if (status == status_)
    return;
  if (status == kOffline) {
  } else if (status == kOnline) {
    device::Error error = ReOnline();
    if (error == device::OK) {
      status_ = kOnline;
    }
  }
}

void CANScopeRunner::DeviceListChanged() {}

} // namespace canscope
