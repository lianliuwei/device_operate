#include "device/memory_usage_pool.h"

#include "base/logging.h"

using namespace std;
using namespace base;

uint8* MemoryUsagePool::Alloc(int size) {
  AutoLock lock(lock_);

  if (HitCeiling(size)) {
    return NULL;
  }

  uint8* buffer = new uint8[size];
  alloced_.insert(make_pair(buffer, size));
  memory_usage_ += size;
  return buffer;
}

void MemoryUsagePool::Free(uint8* free) {
  bool notify = false;
  HaveFreeCallbackType have_free;
  int amount;
  {
  AutoLock lock(lock_);
  map<uint8*, int>::iterator it = alloced_.find(free);
  DCHECK(it != alloced_.end());
  delete[] free;
  int size = it->second;
  alloced_.erase(it);
  memory_usage_ -= size;

  // notify call back if reach amount
  amount = memory_limit_ - memory_usage_;
  if (need_call_ && need_amount_ <= amount) {
    notify = true;
    need_call_ = false;
    have_free = have_free_;
  }
  }
  if (notify && !have_free.is_null()) {
    have_free.Run(amount);
  }
}

int MemoryUsagePool::Left() {
  AutoLock lock(lock_);
  return memory_limit_ - memory_usage_;
}

void MemoryUsagePool::SetFreeCallback(HaveFreeCallbackType have_free, int amount) {
  bool notify = false;
  int left;
  {
  AutoLock lock(lock_);
  left = memory_limit_ - memory_usage_;
  if (left < amount) {
    need_call_ = true;
    need_amount_ = amount;
    have_free_ = have_free;
    return;
  } else  {
    notify = true;
  }
  }
  // quick notify path
  if (notify && !have_free.is_null()) {
    have_free.Run(left);
  }
}

bool MemoryUsagePool::HitCeiling(int size) {
  return size + memory_usage_ > memory_limit_;
}

void MemoryUsagePool::CancelCallback() {
  AutoLock lock(lock_);
  need_call_ = false;
  need_amount_ = -1;
  have_free_.Reset();
}

MemoryUsagePool::MemoryUsagePool(int memory_limit)
  : memory_limit_(memory_limit)
  , memory_usage_(0)
  , need_amount_(-1)
  , need_call_(false) {

}

MemoryUsagePool::~MemoryUsagePool() {
  LOG_IF(WARNING, alloced_.size() != 0) << "have memory no free";
}

