#pragma once

#include "device/simple_type_bind.h"
#include "device/register_memory.h"

namespace canscope {
class DeviceInfo {
public:
  device::RegisterMemory memory;
  
  Uint16Bind id;
  Uint32Bind loader_version;
  Uint32Bind device_type;
  Uint32Bind firmware_version;
  Uint32Bind fpage_version;
  Uint32Bind device_serial_num;
  Uint32Bind produce_data;
  Uint16Bind usb_speed;
  Uint16Bind firmware_update_flag;
  Uint16Bind voltage_12;
  Uint16Bind voltage_1p2;
  Uint16Bind voltage_3p3;
  Uint16Bind voltage_2p5;
  Uint16Bind voltage_5;
  Uint16Bind fpga_core_temp;
  Uint16Bind env_temp;
    
  DeviceInfo();
  ~DeviceInfo() {}

private:
  DISALLOW_COPY_AND_ASSIGN(DeviceInfo);
};
} // namespace canscope