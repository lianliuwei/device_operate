#include "canscope/device/canscope_device_runner.h"

#include "base/base_paths.h"
#include "base/path_service.h"
#include "base/file_util.h"

#include "canscope/device/canscope_device.h"
#include "canscope/device/devices_manager.h"

using namespace base;

namespace canscope {

CANScopeRunner::CANScopeRunner(CANScopeDevice* canscope)
    : canscope_(canscope)
    , inited_(false)
    // when runner is create the device is online,
    // or this obj will be destroy immediate
    , status_(kOnline) {

}

CANScopeRunner::~CANScopeRunner() {
  CloseDevice();
}

void CANScopeRunner::Init(string16 device_path) {
  DCHECK(!inited_) << "set device_path before inited";
  device_path_ = device_path;

  // create osc Data
  osc_data = new OscDataCollecter(canscope_->device_delegate(), canscope_->osc_device());
  scoped_refptr<SingleThreadTaskRunner> run_thread = canscope_->run_thread();
  osc_data->set_run_thread(run_thread);
}

device::Error CANScopeRunner::InitDevice() {
  DCHECK(!inited_) << "device already init";
  device::Error error = InitDeviceImpl(device_path_);
  if (error != device::OK) {
    return error;
  }
  status_ = kOnline;
  inited_ = true;
  canscope_->DeviceTypeDetected(GetDeviceType());
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

void CANScopeRunner::CloseDevice() {
  if (!inited_)
    return;
  StopAll();
  CloseDeviceImpl();
  inited_ = false;
}

device::Error CANScopeRunner::InitDeviceImpl(string16 device_path) {
  UsbPortDeviceDelegate* device_delegate = canscope_->device_delegate();
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
  PathService::Get(base::DIR_EXE, &fpga_file);
  // pro use different FPGA file.
  fpga_file = fpga_file.Append(
      device_info_.IsProVersion() ? L"CANScopePro.dll" : L"CANScope.dll");
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
  UsbPortDeviceDelegate* device_delegate = canscope_->device_delegate();
  UsbPort* usb_port = device_delegate->usb_port_ptr();
  bool ret = usb_port->CloseDevice();
  DCHECK(ret);
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

void CANScopeRunner::StartAll() {
  osc_data->Start();
}

void CANScopeRunner::StopAll() {
  osc_data->Stop();
}

void CANScopeRunner::ReRunAll() {
  osc_data->ReRun();
}

canscope::DeviceType CANScopeRunner::GetDeviceType() {
  DCHECK(inited_);
  return device_info_.GetDeviceType();
}

} // namespace canscope
