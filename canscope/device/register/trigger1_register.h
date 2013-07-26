#pragma once

#include "device/simple_type_bind.h"
#include "device/register_memory.h"

namespace canscope {
class Trigger1Register {
public:
  device::RegisterMemory memory;

  Uint32Bind trig_pre;
  Uint32Bind trig_post;
  Uint32Bind auto_time;
  Uint32Bind div_coe;

  Trigger1Register();
  ~Trigger1Register() {}

private:
  DISALLOW_COPY_AND_ASSIGN(Trigger1Register);
};
}