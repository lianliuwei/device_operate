#include "canscope/device_simulate/simulate_device_delegate.h"

#include "base/logging.h"
#include "canscope/device/register/canscope_device_register_constants.h"
#include "canscope/device/register/frame_storage_register.h"
#include "canscope/device/register/fpga_send_register.h"
#include "canscope/device/canscope_device_constants.h"

using namespace canscope::device;

namespace {
static const TCHAR kDevicePath[] = L"CANScopeDeviceSimulate";
}
namespace canscope {

SimulateDeviceDelegate::SimulateDeviceDelegate(bool check_thread) 
    : opened_(false)
    , load_fpga_(false)
    , check_thread_(check_thread)
    , group_()
    , osc_device_(&group_, DT_CS1203) {
  // pro device
  group_.device_info.fpage_version.set_value(0xFFFFFFFF);
  group_.device_info.device_type.set_value(0x1203);
}

Error SimulateDeviceDelegate::EnumDevices(std::vector<string16>* devices) {
  CheckThread();
  DCHECK(devices);
  devices->clear();
  devices->push_back(kDevicePath);
  return OK;
}

Error SimulateDeviceDelegate::OpenDevice(string16 device_path) {
  CheckThread();
  if (device_path == kDevicePath) {
    DCHECK(!opened_) << "open device twice";
    opened_ = true;
    return OK;
  } else {
    return ERR_DEVICE_OPEN;    
  }
}

Error SimulateDeviceDelegate::CloseDevice() {
  CheckThread();
  DCHECK(opened_) << "close device no open";
  opened_ = false;
  return OK;
}

Error SimulateDeviceDelegate::DownloadFPGA(uint8* buffer, int size) {
  CheckOpen();
  DCHECK(!load_fpga_) << "load fpga twice";
  group_.device_info.fpage_version.set_value(0x1234);
  load_fpga_ = true;
  return OK;
}

Error SimulateDeviceDelegate::GetDeviceInfo(DeviceInfo* device_info) {
  CheckOpen();
  DCHECK(device_info);
  ::device::MemoryContent content(group_.device_info.memory);
  content.SetMemory(&(device_info->memory));
  return OK;
}

Error SimulateDeviceDelegate::WriteDevice(uint32 addr, uint8* buffer, int size) {
  CheckOpen();
  CheckConfig();
  return group_.WriteDevice(addr, buffer, size);
}

Error SimulateDeviceDelegate::ReadDevice(uint32 addr, uint8* buffer, int size) {
  CheckOpen();
  CheckConfig();
  return group_.ReadDevice(addr, buffer, size);
}

device::Error SimulateDeviceDelegate::ReadOscData(uint8* buffer, int size) {
  CheckOpen();
  CheckConfig();
  return osc_device_.ReadOscData(buffer, size);
}

device::Error SimulateDeviceDelegate::ReadFrameData(uint8* buffer, int size) {
  CheckOpen();
  CheckConfig();
  return ERR_READ_DEVICE;
}

void SimulateDeviceDelegate::CheckOpen() {
  CheckThread();
  CHECK(opened_);
}

void SimulateDeviceDelegate::CheckConfig() {
  CHECK(opened_);
  CHECK(load_fpga_);
}

void SimulateDeviceDelegate::CheckThread() {
  if (check_thread_) {
    CHECK(thread_check_.CalledOnValidThread());
  }
}

void SimulateDeviceDelegate::DetachFromThread() {
  thread_check_.DetachFromThread();
  osc_device_.DetachFromThread();
}

} // namespace canscope