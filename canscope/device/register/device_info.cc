#include "canscope/device/register/device_info.h"

#include "canscope/device/register/canscope_device_register_constants.h"

namespace canscope {
DeviceInfo::DeviceInfo()
    : memory(kDeviceInfoAddr, kDeviceInfoSize)
    , id(memory.PtrByRelative(kDeviceInfoIDOffset), false)
    , loader_version(memory.PtrByRelative(kDeviceInfoLoaderVersionOffset), false)
    , device_type(memory.PtrByRelative(kDeviceInfoDeviceTypeOffset), false)
    , firmware_version(memory.PtrByRelative(kDeviceInfofirmwareVersionOffset), false)
    , fpage_version(memory.PtrByRelative(kDeviceInfoFpgaVersionOffset), false)
    , device_serial_num(memory.PtrByRelative(kDeviceInfoDeviceSerialNumOffset), false)
    , produce_data(memory.PtrByRelative(kDeviceInfoProduceDateOffset), false)
    , usb_speed(memory.PtrByRelative(kDeviceInfoUsbSpeedOffset), false)
    , firmware_update_flag(memory.PtrByRelative(kDeviceInfoFirmwareUpdateFlagOffset), false)
    , voltage_12(memory.PtrByRelative(kDeviceInfoVoltage12Offset), false)
    , voltage_1p2(memory.PtrByRelative(kDeviceInfoVoltage1p2Offset), false)
    , voltage_3p3(memory.PtrByRelative(kDeviceInfoVoltage3p3Offset), false)
    , voltage_2p5(memory.PtrByRelative(kDeviceInfoVoltage2p5Offset), false)
    , voltage_5(memory.PtrByRelative(kDeviceInfoVoltage5Offset), false)
    , fpga_core_temp(memory.PtrByRelative(kDeviceInfoFpgaCoreTempOffset), false)
    , env_temp(memory.PtrByRelative(kDeviceInfoEnvTempOffset), false) {}

canscope::DeviceType DeviceInfo::GetDeviceType() {
  uint32 dt = device_type.value();
  dt -= 0x1201;
  switch (dt) {
  case 0: return DT_CS1201;
  case 1: return DT_CS1202;
  case 2: return DT_CS1203;
  default: NOTREACHED(); return DT_CS1201;
  }
}

bool DeviceInfo::IsProVersion() {
  return GetDeviceType() == DT_CS1203;
}

} // namespace canscope