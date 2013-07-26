#pragma once

#include "device/simple_type_bind.h"
#include "device/register_memory.h"

namespace canscope {
class EyeRegister {
public:
  device::RegisterMemory memory;

  BoolBind eye_clr;
  Uint8Bind eye_center;
  Uint32Bind eye_ox;
  Uint32Bind eye_num;
  // EYE_CFG
  Uint32SubBind sampling_rate;
  Uint32SubBind data_source;

  BoolBind addr_clr;

  EyeRegister();
  ~EyeRegister() {}

private:
  DISALLOW_COPY_AND_ASSIGN(EyeRegister);
};