#pragma once

#include "base/time.h"

#include "canscope/device/osc_device/osc_device_property.h"
#include "canscope/device_simulate/canscope_device_register_group.h"

namespace canscope {

class OscDeviceSimulate : public OscDeviceProperty {
public:
  OscDeviceSimulate(CANScopeDeviceRegisterGroup* group, DeviceType device_type);
  ~OscDeviceSimulate() {}

  device::Error ReadOscData(uint8* buffer, int size);

private:
  void UpdateVoltConfig();
  void UpdateTimeBase();
  void UpdateTriggerBase();
  double GetVoltRange(Chnl chnl);

  device::Error OnTriggerState(::device::RegisterMemory* memory, 
      uint32 offset, int size, bool read);
  
  CANScopeDeviceRegisterGroup* group_;
  base::Time start_time_;
  DeviceType device_type_;

  DISALLOW_COPY_AND_ASSIGN(OscDeviceSimulate);
};

} // namespace canscope