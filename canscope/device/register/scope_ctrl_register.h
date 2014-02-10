#pragma once

#include "device/bool_bind.h"
#include "device/register_memory.h"

namespace canscope {
class ScopeCtrlRegister {
public:
  ::device::RegisterMemory memory;

  BoolBind scope_ctrl;
  
  ScopeCtrlRegister();
  ~ScopeCtrlRegister() {}

private:
  DISALLOW_COPY_AND_ASSIGN(ScopeCtrlRegister);
};
} // namespace canscope