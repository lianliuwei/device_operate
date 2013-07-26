#pragma once

#include "device/simple_type_bind.h"
#include "device/register_memory.h"

namespace canscope {
class Trigger1Register {
public:
  device::RegisterMemory memory;

  Uint32Bind TrigPre;
  Uint32Bind TrigPost;
  Uint32Bind AutoTime;
  Uint32Bind SdivCoe;

  Trigger1Register();
  ~Trigger1Register() {}

private:
  DISALLOW_COPY_AND_ASSIGN(Trigger1Register);
};
}