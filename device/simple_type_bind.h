#pragma once

#include "base/basictypes.h"
#include "base/logging.h"

#include "device/one_byte_type_bind.h"
#include "device/endian.h"

// bind to a continuity memory region
// bind_type copy must be cheap, because pass by value.
template <typename bind_type>
class SimpleTypeBind {
public:
  static const int kByteSize = sizeof(bind_type);

  SimpleTypeBind(uint8* ptr, bool big_endian)
      : ptr_(ptr)
      , big_endian_(big_endian) {}
  ~SimpleTypeBind() {}
  
  bind_type value() const {
    bind_type ret;
    big_endian_ ? ReadBigEndian(bit_cast<char*, uint8*>(ptr_), &ret) 
        : ReadLittleEndian(bit_cast<char*, uint8*>(ptr_), &ret);
    return ret;
  }

  void set_value(bind_type value) {
    char store[kByteSize];
    big_endian_ ? WriteBigEndian(store, value) 
        : WriteLittleEndian(store, value);
    memcpy(ptr_, store, kByteSize);
  }

private:
  uint8* ptr_;
  bool big_endian_;
  DISALLOW_COPY_AND_ASSIGN(SimpleTypeBind);
};

// no need BigEndian littleEndian
// typedef SimpleTypeBind<uint8> Uint8Bind;
typedef SimpleTypeBind<uint16> Uint16Bind;
typedef SimpleTypeBind<uint32> Uint32Bind;
typedef SimpleTypeBind<uint64> Uint64Bind;
// typedef SimpleTypeBind<schar> CharBind;
// typedef SimpleTypeBind<schar> Int8Bind;
typedef SimpleTypeBind<int16> Int16Bind;
typedef SimpleTypeBind<int32> Int32Bind;
typedef SimpleTypeBind<int64> Int64Bind;
typedef SimpleTypeBind<float> FloatBind;
typedef SimpleTypeBind<double> DoubleBind;

