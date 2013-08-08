#include "canscope/device/register/soft_diff_register.h"

#include "canscope/device/canscope_device_constants.h"

using namespace device;

namespace canscope {
SoftDiffRegister::SoftDiffRegister()
    : memory(kSoftDiffAddr, kSoftDiffSize)
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
    , filtering(memory.PtrByRelative(kFilteringOffset)) {

}

} // namespace canscope
