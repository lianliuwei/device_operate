#pragma once

#include "device/simple_type_bind.h"
#include "device/uint32_sub_bind.h"
#include "device/register_memory.h"

namespace canscope {
class Trigger2Register {
public:
  device::RegisterMemory memory;

  Uint32SubBind trig_source;
  // CmpVol
  Uint8Bind cmp_high;
  Uint8Bind cmp_low;
  // TrigType
  Uint32SubBind trig_mode;
  Uint32SubBind trig_type;
  // TrigTime
  Uint32SubBind compare;
  Uint32SubBind trig_time;
  

  Trigger2Register();
  ~Trigger2Register() {}

private:
  DISALLOW_COPY_AND_ASSIGN(Trigger2Register);
};
}