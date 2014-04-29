#pragma once

#include "base/synchronization/lock.h"

// thread safe read statistics
class ReadStatistics {
public:
  ReadStatistics();
  ~ReadStatistics() {}

  void Get(int64* size, int64* count);

  void Update(int64 size);
  void Update(int64 size, int64 count);
  void Set(int64 size, int64 count);
  void Reset();

private:
  base::Lock lock_;

  int64 size_;
  int64 count_;
};