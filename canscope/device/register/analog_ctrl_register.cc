#include "canscope/device/register/analog_ctrl_register.h"

#include "canscope/device/canscope_device_constants.h"

using namespace device;

namespace canscope {
AnalogCtrlRegister::AnalogCtrlRegister()
    : memory(kAnalogCtrlAddr, kAnalogCtrlSize)
    , can_l_gain(memory.PtrByRelative(kGainCh1Offset), false)
    , can_l_offset(memory.PtrByRelative(kOffsetCh1Offset), false)
    , can_h_gain(memory.PtrByRelative(kGainCh2Offset), false)
    , can_h_offset(memory.PtrByRelative(kOffsetCh2Offset), false) {}

} // namespace canscope
