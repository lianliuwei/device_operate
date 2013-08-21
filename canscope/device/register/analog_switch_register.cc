#include "canscope/device/register/analog_switch_register.h"

#include "canscope/device/register/canscope_device_register_constants.h"

using namespace device;

namespace canscope {
AnalogSwitchRegister::AnalogSwitchRegister()
    : memory(kAnalogSwitchAddr, kAnalogSwitchSize)
    , coupling_canl(memory.PtrByRelative(kAnalogSwitchOffset), 0, true)
    , coupling_canh(memory.PtrByRelative(kAnalogSwitchOffset), 1, true)
    , attenuation_canl(memory.PtrByRelative(kAnalogSwitchOffset), 2, true)
    , attenuation_canh(memory.PtrByRelative(kAnalogSwitchOffset), 3, true) {}

} // namespace canscope
