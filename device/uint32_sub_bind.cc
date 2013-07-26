#include "device/uint32_sub_bind.h"

#include "device/bitfield_helpers.h"

Uint32SubBind::Uint32SubBind(uint8* ptr, 
                             bool big_endian,
                             int start_index, 
                             int end_index)
    : bind_(ptr, big_endian)
    , start_index_(start_index)
    , end_index_(end_index) {
  DCHECK(0 <= start_index && start_index <= 31);
  DCHECK(0 <= end_index && end_index <= 31);
  // make start_index < end_index
  if (start_index > end_index) {
    start_index_ = end_index;
    end_index_ = start_index;
  }
}

Uint32SubBind::~Uint32SubBind() {

}

uint32 Uint32SubBind::MaxValue() const {
  return (2U << (end_index_-start_index_)) -1;
}

uint32 Uint32SubBind::value() const {
  uint32 length = end_index_ - start_index_ + 1;
  uint32 shift = start_index_;
  uint32 value_mark = 1U + ((1U << (length-1)) - 1U) * 2U; 
  return (bind_.value() >> shift) & value_mark;
}

void Uint32SubBind::set_value(uint32 value) {
  CHECK(value <= MaxValue());
  uint32 length = end_index_ - start_index_ + 1;
  uint32 shift = start_index_;
  uint32 value_mark = 1U + ((1U << (length-1)) - 1U) * 2U; 
  
  value = (value & value_mark) << shift;

  uint32 temp = bind_.value();
  temp = (temp & ~(value_mark << shift)) | value;
  bind_.set_value(temp);
}


