#include "canscope/device/register/trigger1_register.h"

#include "canscope/device/canscope_device_constants.h"

using namespace device;

namespace canscope {
Trigger1Register::Trigger1Register()
    : memory(kTrigger1Addr, kTrigger1Size)
    , trig_pre(memory.PtrByRelative(kTrigPreOffset), false)
    , trig_post(memory.PtrByRelative(kTrigPostOffset), false)
    , auto_time(memory.PtrByRelative(kAutoTimeOffset), false)
    , div_coe(memory.PtrByRelative(kDivCoeOffset), false) {}

} // namespace canscope
