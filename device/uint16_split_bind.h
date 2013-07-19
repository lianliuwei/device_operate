#pragma once

#include "base/basictypes.h"

// read uint8 uint16_store[2] and convert to uint16 according to the big_endian
// uint16_store[0] = *(ptr + index_0)
// uint16_store[1] = *(ptr + index_1)
class Uint16SplitBind {
public:
  Uint16SplitBind(uint8* ptr, int index_0, int index_1, bool big_endian);
  ~Uint16SplitBind() {}

  uint16 value() const;
  void set_value(uint16 value);

private:
  uint8* ptr_;
  int index_0_;
  int index_1_;
  bool big_endian_;

  DISALLOW_COPY_AND_ASSIGN(Uint16SplitBind);
};