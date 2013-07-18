#pragma once

#include "base/basictypes.h"
#include "base/logging.h"

// bind to Register Memory bit.
// bacause use memory ptr, so Bind can no copy.
class BoolBind {
public:
  static const int kByteSize = 1;

  BoolBind(uint8* ptr, int index, bool no_invert) 
      : ptr_(ptr)
      , index_(index)
      , invert_(!no_invert) {
    DCHECK(index >= 0 && index <= 7);
    DCHECK(ptr);
  }
  
  ~BoolBind() {}

  bool value() const {
    bool ret = ((*ptr_) & (1 << index_)) != 0;
    return invert_ ? !ret : ret;
  }
 
  void set_value(bool value) {
    if (invert_)
      value = !value;
    if (value) 
      *ptr_ |= (1 << index_);
    else
      *ptr_ &= ~(1 << index_);
  }

  void Toggle() {
    *ptr_ ^= (1 << index_);
  }

private:
  uint8* ptr_;
  int index_;
  bool invert_;

  DISALLOW_COPY_AND_ASSIGN(BoolBind);
};
