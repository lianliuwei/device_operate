#pragma once

#include "device/simple_type_bind.h"
#include "device/bool_bind.h"
#include "device/register_memory.h"

namespace canscope {
  class TriggerStateRegister {
  public:
    device::RegisterMemory memory;
    
    // TRIG_STATE
    BoolBind pre_bit;
    BoolBind trig_bit;
    BoolBind clet_bit; // collect bit

// no used
//     Uint32Bind trig_addr;
//     Uint32Bind trig_saddr; // trigger start addr
//     Uint32Bind trig_eaddr; // trigger end addr

    TriggerStateRegister();
    ~TriggerStateRegister() {}

  private:
    DISALLOW_COPY_AND_ASSIGN(TriggerStateRegister);
  };
}