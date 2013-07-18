#pragma once

#include "base/basictypes.h"
#include "base/logging.h"

// bind to Uint8
class Uint8Bind {
public:
  static const int kByteSize = 1;
  
  Uint8Bind(uint8* ptr)
      : ptr_(ptr) {}
  ~Uint8Bind() {}

  uint8 value() const {
    return *ptr_;
  }

  void set_value(uint8 value) {
    *ptr_ = value;
  }

private:
  uint8* ptr_;
};