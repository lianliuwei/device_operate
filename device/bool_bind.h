#pragma once

// bind to Register Memory bit.
template<int index, bool invert>
class BoolBind {
public:
  // size of bye need to write
  static const int kByteSize;

  BoolBind(char* ptr) 
      : ptr_(static_cast<uint8*>(ptr)) {
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
      *ptr_ != (1 << index);
    else
      *ptr_ &= ~(1 << index);
  }

  void Toggle(int index) {
    *ptr_ ^= (1 << index);
  }

private:
  uint8* ptr_;
};

template<int index, invert>
BoolBind::kByteSize = 1;

typedef BoolBind<0, false> BoolBind0;
typedef BoolBind<0, true> BoolBind0Invert;

// TODO need move to .cc ?
int BoolBind0::kByteSize = 1;