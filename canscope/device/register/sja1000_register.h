#pragma once

#include "device/bool_bind.h"
#include "device/simple_type_bind.h"
#include "device/register_memory.h"

struct FrameData {
 uint8 data[13];
};

namespace canscope {

class SJA1000Register {
public:
  ::device::RegisterMemory memory;
  
  Uint16Bind sja_btr;
  BoolBind slient;
  
  // read only
  Uint32Bind send_state;
  // sned_state detail
  BoolBind int_bus; // interrupt of sja10000
  BoolBind int_send;
  BoolBind int_alc;
  BoolBind cfg;
  BoolBind sending;
  Uint8Bind s_alc; // valid when int_alc enable
  Uint8Bind s_ecc; // valid when bus enable

  FrameData frame_data() const;
  void set_frame_data(const FrameData& data);

  SJA1000Register();
  ~SJA1000Register() {}

  // sys range
  int SysOffset() const;
  int SysSize() const;

private:
  DISALLOW_COPY_AND_ASSIGN(SJA1000Register);
};

} // namespace canscope
