#pragma once

#include "device/bool_bind.h"
#include "device/simple_type_bind.h"
#include "device/uint16_split_bind.h"
#include "device/register_memory.h"

namespace canscope {

class FrameStorageRegister {
public:
  ::device::RegisterMemory memory;
  
  Uint32Bind frame_depth;
  Uint32Bind frame_num;
  BoolBind overflow;

  FrameStorageRegister();
  ~FrameStorageRegister() {}

private:
  DISALLOW_COPY_AND_ASSIGN(FrameStorageRegister);
};

} // namespace canscope
