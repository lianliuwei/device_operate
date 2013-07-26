#pragma once

#include "base/basictypes.h"
#include "base/logging.h"

#include "device/simple_type_bind.h"

class Uint32SubBind {
public:
  Uint32SubBind(uint8* ptr, bool big_endian, int start_index, int end_index);
  ~Uint32SubBind();
  
  uint32 value() const;
  void set_value(uint32 value);

  // 1bit max value is 1, 2bit max value is 3
  // 3bit max value is 7 ...
  // min value is always 0
  uint32 MaxValue() const;

private:
  Uint32Bind bind_;
  int start_index_;
  int end_index_;
};