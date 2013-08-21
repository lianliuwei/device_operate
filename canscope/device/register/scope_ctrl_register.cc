#include "canscope/device/register/scope_ctrl_register.h"

#include "canscope/device/register/canscope_device_register_constants.h"

using namespace device;

namespace canscope {
ScopeCtrlRegister::ScopeCtrlRegister()
    : memory(kScopeCtrlAddr, kScopeCtrlSize)
    , scope_ctrl(memory.PtrByRelative(kScopeCtrlOffset), 0, true) {}

} // namespace canscope
