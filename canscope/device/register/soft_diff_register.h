#pragma once

#include "device/bool_bind.h"
#include "device/simple_type_bind.h"
#include "device/uint16_split_bind.h"
#include "device/register_memory.h"

namespace canscope {
class SoftDiffRegister {
public:
  ::device::RegisterMemory memory;

  Uint16Bind sja_btr;
  BoolBind  sys_cfg;
  Uint8Bind ch_sens_canh;
  Uint8Bind ch_sens_canl;
  Uint8Bind ch_sens_candiff;

  Uint16SplitBind ch_zero_canh;
  Uint16SplitBind ch_zero_canl;
  Uint16SplitBind ch_zero_candiff;
  
  Uint16Bind fil_div;
  BoolBind diff_ctrl;

  SoftDiffRegister();
  ~SoftDiffRegister() {}

  // sys range
  static const int kSysOffset;
  static const int kSysSize;

  // fil_div range
  static const int kFilDivOffset;
  static const int kFilDivSize;

  // chnl range
  static const int kChnlOffset;
  static const int kChnlSize;

private:
  DISALLOW_COPY_AND_ASSIGN(SoftDiffRegister);
};

} // namespace canscope
