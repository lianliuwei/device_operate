#pragma once

#include <map>

#include "base/memory/ref_counted.h"
#include "base/synchronization/lock.h"
#include "base/callback.h"

// simple keep trace the memory usage.
class MemoryUsagePool : public base::RefCountedThreadSafe<MemoryUsagePool> {
public:
  // void HaveFreeCallBack(int amount);
  typedef base::Callback<void(int)> HaveFreeCallbackType;

  MemoryUsagePool(int memory_limit);
  ~MemoryUsagePool();

  uint8* Alloc(int size);
  void Free(uint8* free);

  // Left memory
  int Left();

  // callback when have amount memory free
  void SetFreeCallback(HaveFreeCallbackType have_free, int amount);

  void CancelCallback();

private:
  bool HitCeiling(int size);

  HaveFreeCallbackType have_free_;

  std::map<uint8*, int> alloced_;
  
  int memory_limit_;
  int memory_usage_;

  int need_amount_;
  bool need_call_;

  base::Lock lock_;
};