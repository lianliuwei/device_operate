#pragma once

#include "device/bool_bind.h"
#include "device/simple_type_bind.h"
#include "device/uint16_split_bind.h"
#include "device/register_memory.h"

namespace canscope {

class WaveStorageRegister {
public:
  ::device::RegisterMemory memory;
  
  Uint32Bind frame_len;
  Uint32Bind wave_start;
  Uint32Bind wave_end;
  Uint32Bind div_coe;
  Uint32Bind wave_lost;

  WaveStorageRegister();
  ~WaveStorageRegister() {}

private:
  DISALLOW_COPY_AND_ASSIGN(WaveStorageRegister);
};

} // namespace canscope
