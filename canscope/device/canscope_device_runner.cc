#include "canscope/device/canscope_device_runner.h"

#include "base/base_paths.h"
#include "base/path_service.h"
#include "base/file_util.h"

#include "canscope/device/canscope_device.h"
#include "canscope/device/devices_manager.h"

using namespace base;
using namespace canscope::device;

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
  DeviceDelegate* device_delegate = canscope_->device_delegate();
  Error err;
  err = device_delegate->OpenDevice(device_path);
  if (err != OK) {
    return device::ERR_DEVICE_OPEN;
  }
  err = device_delegate->GetDeviceInfo(&device_info_);
  if (err != OK) {
    device_delegate->CloseDevice();
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
  bool ret;
  ret = file_util::ReadFileToString(fpga_file, &content);
  if (!ret) {
    device_delegate->CloseDevice();
    return device::ERR_DEVICE_LOADFPGAFILE;
  }
  err = device_delegate->DownloadFPGA((uint8*)(content.c_str()), content.size());
  if (err != OK) {
    device_delegate->CloseDevice();
    return device::ERR_DEVICE_DOWNLOADFPGA;
  }
  return device::OK;
}

void CANScopeRunner::CloseDeviceImpl() {
  DeviceDelegate* device_delegate = canscope_->device_delegate();
  Error err = device_delegate->CloseDevice();
  DCHECK(err == OK);
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
  frame_data->Start();
}

void CANScopeRunner::StopAll() {
  osc_data->Stop();
  frame_data->Stop();
}

void CANScopeRunner::ReRunAll() {
  osc_data->ReRun();
  frame_data->ReRun();
}

canscope::DeviceType CANScopeRunner::GetDeviceType() {
  DCHECK(inited_);
  return device_info_.GetDeviceType();
}

} // namespace canscope
