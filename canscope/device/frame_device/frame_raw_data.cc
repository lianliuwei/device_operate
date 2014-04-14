#include "canscope/device/frame_device/frame_raw_data.h"

#include "base/logging.h"

namespace canscope {

FrameRawData::FrameRawData(int size)
    : own_(true)
    , size_(size)
    , real_size_(0) {
  raw_data_ = new uint8[size];
}

FrameRawData::FrameRawData(uint8* raw_data, int size, bool own) 
    : own_(own)
    , raw_data_(raw_data) 
    , size_(size) {
}

FrameRawData::~FrameRawData() {
  if (own_) {
    delete[] raw_data_;
    raw_data_ = NULL;
  }
}

void FrameRawData::set_data(uint8* data, int size) {
  raw_data_ = data;
  size_ = size;
}

PooledFrameRawData::PooledFrameRawData(int size, scoped_refptr<MemoryUsagePool> pool)
    : FrameRawData(NULL, 0, false)
    , pool_(pool)
    , need_size_(size)
    , alloc_ok_(false) {
  AllocAgain();
}

void PooledFrameRawData::AllocAgain() {
  DCHECK(alloc_ok_ == false);
  uint8* data = pool_->Alloc(need_size_);
  alloc_ok_ = data != NULL;
  if (alloc_ok_) {
    set_data(data, need_size_);
  }
}

PooledFrameRawData::~PooledFrameRawData() {
  if (alloc_ok_) {
    pool_->Free(data());
    set_data(NULL, 0);
  }
}

} // namespace canscope