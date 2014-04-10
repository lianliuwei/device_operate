#pragma once

#include "device/bool_bind.h"
#include "device/simple_type_bind.h"
#include "device/uint16_split_bind.h"
#include "device/register_memory.h"

namespace canscope {

class WaveReadCtrlRegister {
public:
  ::device::RegisterMemory memory;
  
  Uint32Bind read_start;
  Uint32Bind section_start;
  Uint32Bind section_end;

  WaveReadCtrlRegister();
  ~WaveReadCtrlRegister() {}

private:
  DISALLOW_COPY_AND_ASSIGN(WaveReadCtrlRegister);
};

} // namespace canscope
