#pragma once

#include "device/bool_bind.h"
#include "device/register_memory.h"

namespace canscope {
class AnalogSwitchRegister {
public:
  device::RegisterMemory memory;

  BoolBind coupling_canh;
  BoolBind coupling_canl;

  BoolBind attenuation_canh;
  BoolBind attenuation_canl;

  AnalogSwitchRegister();
  ~AnalogSwitchRegister() {}

private:
  DISALLOW_COPY_AND_ASSIGN(AnalogSwitchRegister);
};
} // namespace canscope