#include "canrecord/device/read_statistics.h"

using namespace base;


ReadStatistics::ReadStatistics() {
  size_ = 0;
  count_ = 0;
}

void ReadStatistics::Get(int64* size, int64* count) {
  AutoLock lock(lock_);

  if (size) {
    *size = size_;
  }
  if (count) {
    *count = count_;
  }
}

void ReadStatistics::Update(int64 size) {
  AutoLock lock(lock_);
  size_ += size;
  count_ += 1;
}

void ReadStatistics::Update(int64 size, int64 count) {
  AutoLock lock(lock_);
  size_ += size;
  count_ += count;
}

void ReadStatistics::Set(int64 size, int64 count) {
  AutoLock lock(lock_);
  size_ = size;
  count_ = count;
}

void ReadStatistics::Reset() {
  AutoLock lock(lock_);
  size_ = 0;
  count_ = 0;
}




