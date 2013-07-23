#pragma once

#include "base/basictypes.h"
#include "base/logging.h"


// bind to a continuity memory region
// bind_type copy must be cheap, because pass by value.
template <typename bind_type>
class OneByteTypeBind {
public:
  static const int kByteSize = sizeof(bind_type);

  OneByteTypeBind(uint8* ptr)
    : ptr_(ptr) {}
  ~OneByteTypeBind() {}

  bind_type value() const {
    return *(bit_cast<bind_type*, uint8*>(ptr_));
  }

  void set_value(bind_type value) {
    *(bit_cast<bind_type*, uint8*>(ptr_)) = value;
  }

private:
  uint8* ptr_;
  DISALLOW_COPY_AND_ASSIGN(OneByteTypeBind);
};

typedef OneByteTypeBind<uint8> Uint8Bind;
typedef OneByteTypeBind<schar> CharBind;
typedef OneByteTypeBind<schar> Int8Bind;
