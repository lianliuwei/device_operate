#pragma once

#include "device/simple_type_bind.h"
#include "device/register_memory.h"

namespace canscope {
class AnalogCtrlRegister {
public:
  device::RegisterMemory memory;

  Uint32Bind can_l_gain;
  Uint32Bind can_l_offset;

  Uint32Bind can_h_gain;
  Uint32Bind can_h_offset;

  AnalogCtrlRegister();
  ~AnalogCtrlRegister() {}

private:
  DISALLOW_COPY_AND_ASSIGN(AnalogCtrlRegister);
};
}