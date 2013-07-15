#pragma once

#include "base/basictypes.h"
#include "base/logging.h"

// bind to Register Memory bit.
template<int index, bool invert>
class BoolBind {
public:
  BoolBind(char* ptr) 
      : ptr_((uint8*)(ptr)) {
    DCHECK(index >= 0 && index <= 7);
    DCHECK(ptr);
  }
  
  ~BoolBind() {}

  bool value() const {
    bool ret = ((*ptr_) & (1 << index)) != 0;
    return invert ? !ret : ret;
  }
 
  void set_value(bool value) {
    if (invert)
      value = !value;
    if (value) 
      *ptr_ |= (1 << index);
    else
      *ptr_ &= ~(1 << index);
  }

  void Toggle(int index) {
    *ptr_ ^= (1 << index);
  }

private:
  uint8* ptr_;
};


typedef BoolBind<0, false> BoolBind0;
typedef BoolBind<0, true> BoolBind0Invert;
typedef BoolBind<1, false> BoolBind1;
typedef BoolBind<1, true> BoolBind1Invert;
typedef BoolBind<2, false> BoolBind2;
typedef BoolBind<2, true> BoolBind2Invert;
typedef BoolBind<3, false> BoolBind3;
typedef BoolBind<3, true> BoolBind3Invert;
typedef BoolBind<4, false> BoolBind4;
typedef BoolBind<4, true> BoolBind4Invert;
typedef BoolBind<5, false> BoolBind5;
typedef BoolBind<5, true> BoolBind5Invert;
typedef BoolBind<6, false> BoolBind6;
typedef BoolBind<6, true> BoolBind6Invert;
typedef BoolBind<7, false> BoolBind7;
typedef BoolBind<7, true> BoolBind7Invert;