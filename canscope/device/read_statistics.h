#pragma once

#include "canrecord/canrecord_export.h"

#include "base/synchronization/lock.h"

// thread safe read statistics
class CANRECORD_DEVICE_EXPORT ReadStatistics {
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