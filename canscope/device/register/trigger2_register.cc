#include "canscope/device/register/trigger2_register.h"

#include "canscope/device/canscope_device_constants.h"

using namespace device;

namespace canscope {
Trigger2Register::Trigger2Register()
    : memory(kTrigger2Addr, kTrigger2Size)
    , trig_source(memory.PtrByRelative(kTrigSourceOffset), false, 0, 2)
    , cmp_high(memory.PtrByRelative(kTrigAddrOffset + 1))
    , cmp_low(memory.PtrByRelative(kTrigAddrOffset))
    , trig_mode(memory.PtrByRelative(kTrigTypeOffset), false, 31, 31)
    , trig_type(memory.PtrByRelative(kTrigTypeOffset), false, 0, 2)
    , compare(memory.PtrByRelative(kTrigTimeOffset), false, 30, 31)
    , trig_time(memory.PtrByRelative(kTrigTimeOffset), false, 0, 29) {}

} // namespace canscope
