#pragma once

#include "build/build_config.h"

#include "device/endian.h"

// type must be native, no value type. because need to >> value.
template<typename type, bool big_endian>
class EndianBind {
public:
  typedef type BindType;

  static const int kByteSize;
  
  EndianBind(char* ptr);

  type value() const {
    type val;
    if (!big_endian) {
      ReadLittleEndian(ptr, val);
    } else {
      ReadBigEndian(ptr, val);
    }
    return val;
  }

  void set_value(type value) {
    if (!big_endian) {
      // can using reinterpret_cast to implement?
      WriteLittleEndian(ptr, value);
    } else {
      WriteBigEndian(ptr, value);
    }
  }

protected:

private:
  char* ptr;
};

template<typename type, bool big_endian>
EndianBind::kByteSize = sizeof(type);

typedef EndianBind<uint32, true> Uint32BEBind;
typedef EndianBind<uint32, false> Uint32LEBind;
typedef EndianBind<int32, true> Int32BEBind;
typedef EndianBind<int32, false> Int32LEBind;