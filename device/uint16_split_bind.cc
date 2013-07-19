#include "device/uint16_split_bind.h"

#include "base/logging.h"

#include "device/endian.h"

Uint16SplitBind::Uint16SplitBind(uint8* ptr, 
                                 int index_0, int index_1, 
                                 bool big_endian)
    : ptr_(ptr)
    , index_0_(index_0)
    , index_1_(index_1)
    , big_endian_(big_endian) {
  DCHECK(ptr);
}

uint16 Uint16SplitBind::value() const {
  uint8 store[2];
  store[0] = *(ptr_ + index_0_);
  store[1] = *(ptr_ + index_1_);
  uint16 ret;
  big_endian_ ? ReadBigEndian(bit_cast<char*, uint8*>(store), &ret) 
      : ReadLittleEndian(bit_cast<char*, uint8*>(store), &ret);
  return ret;
}

void Uint16SplitBind::set_value(uint16 value) {
  uint8 store[2];
  big_endian_ ? WriteBigEndian(bit_cast<char*, uint8*>(store), value) 
      : WriteLittleEndian(bit_cast<char*, uint8*>(store), value);
  *(ptr_ + index_0_) = store[0];
  *(ptr_ + index_1_) = store[1];
 
}
