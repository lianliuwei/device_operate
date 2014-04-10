#include "canscope/device/register/wave_read_ctrl_register.h"

#include "canscope/device/register/canscope_device_register_constants.h"

namespace canscope {
WaveReadCtrlRegister::WaveReadCtrlRegister()
    : memory(kWaveReadCtrlAddr, kWaveReadCtrlSize)
    , read_start(memory.PtrByRelative(kReadStartOffset), false)
    , section_start(memory.PtrByRelative(kSectionStartOffset), false)
    , section_end(memory.PtrByRelative(kSectionEndOffset), false) {

}

} // namespace canscope
