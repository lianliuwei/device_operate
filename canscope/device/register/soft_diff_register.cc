#include "canscope/device/register/soft_diff_register.h"

#include "canscope/device/register/canscope_device_register_constants.h"

using namespace device;

namespace canscope {
SoftDiffRegister::SoftDiffRegister()
    : memory(kSoftDiffAddr, kSoftDiffSize)
    , sja_btr(memory.PtrByRelative(kSjaBtrOffset), false)
    , sys_cfg(memory.PtrByRelative(kSysCfgOffset), 7, true)
    , ch_sens_canh(memory.PtrByRelative(kChSensCanhOffset))
    , ch_sens_canl(memory.PtrByRelative(kChSensCanlOffset)) 
    , ch_sens_candiff(memory.PtrByRelative(kChSensCandiffOffset))
    , ch_zero_canh(memory.PtrByRelative(0), 
          static_cast<int>(kChZerolCanhOffset), 
          static_cast<int>(kChZerohCanhOffset), false)
    , ch_zero_canl(memory.PtrByRelative(0), 
          static_cast<int>(kChZerolCanlOffset), 
          static_cast<int>(kChZerohCanlOffset), false)
    , ch_zero_candiff(memory.PtrByRelative(0), 
          static_cast<int>(kChZerolCandiffOffset), 
          static_cast<int>(kChZerohCandiffOffset), false)
    , diff_ctrl(memory.PtrByRelative(kDiffCtrlOffset), 0, true)
    , fil_div(memory.PtrByRelative(kFilteringOffset), false) {

}

// static 
const int SoftDiffRegister::kSysOffset = kSjaBtrOffset;
// static 
const int SoftDiffRegister::kSysSize = 4;
// static 
const int SoftDiffRegister::kFilDivOffset = kFilteringOffset;
// static 
const int SoftDiffRegister::kFilDivSize = 4;
// static 
const int SoftDiffRegister::kChnlOffset = kChSensCanhOffset;
// static
const int SoftDiffRegister::kChnlSize = 0x14;


} // namespace canscope
