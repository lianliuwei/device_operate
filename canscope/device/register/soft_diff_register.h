#pragma once

#include "device/bool_bind.h"
#include "device/uint8_bind.h"
#include "device/uint16_split_bind.h"
#include "device/register_memory.h"

namespace canscope {
class SoftDiffRegister {
public:
  device::RegisterMemory memory;

  Uint8Bind ch_sens_canh;
  Uint8Bind ch_sens_canl;
  Uint8Bind ch_sens_candiff;

  Uint16SplitBind ch_zero_canh;
  Uint16SplitBind ch_zero_canl;
  Uint16SplitBind ch_zero_candiff;

  BoolBind diff_ctrl;

  SoftDiffRegister();
  ~SoftDiffRegister() {}

private:
  DISALLOW_COPY_AND_ASSIGN(SoftDiffRegister);
};

} // namespace canscope