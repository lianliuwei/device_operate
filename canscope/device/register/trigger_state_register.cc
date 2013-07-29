#include "canscope/device/register/trigger_state_register.h"

#include "canscope/device/canscope_device_constants.h"

using namespace device;

namespace canscope {
TriggerStateRegister::TriggerStateRegister()
    : memory(kTriggerStateAddr, kTriggerStateSize)
    , pre_bit(memory.PtrByRelative(kTrigStateOffset), 2, true)
    , trig_bit(memory.PtrByRelative(kTrigStateOffset), 1, true)
    , clet_bit(memory.PtrByRelative(kTrigStateOffset), 0, true)
//     , trig_addr(memory.PtrByRelative(kTrigAddrOffset), false)
//     , trig_saddr(memory.PtrByRelative(kTrigSaddrOffset), false)
//     , trig_eaddr(memory.PtrByRelative(kTrigEaddrOffset), false) 
    {}

} // namespace canscope
