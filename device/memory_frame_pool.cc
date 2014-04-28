#include "device/memory_frame_pool.h"

#include "base/logging.h"

using namespace std;
using namespace base;

uint8* MemoryFramePool::Alloc() {
  AutoLock lock(lock_);

  if (free.size() != 0) {
    uint8* buffer = free.front();
    free.pop();
    DCHECK(buffer != NULL);
    alloced.push_back(buffer);
    return buffer;
  }
  if (HitCeiling()) {
    return NULL;
  }
  uint8* buffer = new uint8[frame_size_];
  alloced.push_back(buffer);
  return buffer;
}

void MemoryFramePool::Free(uint8* frame) {
  bool notify = false;
  Closure have_free;

  DCHECK(frame != NULL);
  {
  AutoLock lock(lock_);

  list<uint8*>::iterator it = find(alloced.begin(), alloced.end(), frame);
  CHECK(it != alloced.end()) << "this frame no belong here";
  alloced.erase(it);
  if (free.size() == 0) {
    notify = true;
    have_free = have_free_;
  }
  free.push(frame);
  }
  if (notify && !have_free.is_null()) {
    have_free.Run();
  }
}

int MemoryFramePool::Left() {
  AutoLock lock(lock_);
  return pool_size_ - alloced.size();
}

bool MemoryFramePool::HitCeiling() {
  size_t total = alloced.size() + free.size();
  return static_cast<int>(total) >= pool_size_;
}

MemoryFramePool::MemoryFramePool(int frame_size, int pool_size)
    : frame_size_(frame_size)
    , pool_size_(pool_size) {
  DCHECK(frame_size);
  DCHECK(pool_size);
}

MemoryFramePool::~MemoryFramePool() {
  ReleaseFree();
  LOG_IF(WARNING, (alloced.size())) << "Frame still no return";
}

void MemoryFramePool::ReleaseFree() {
  uint8* buffer;
  while (free.size()) {
    buffer = free.front();
    free.pop();
    delete[] buffer;
  }
}

void MemoryFramePool::set_have_free_callback(base::Closure have_free) {
  bool notify = false;
  {
  AutoLock lock(lock_);
  have_free_ = have_free;
  notify = free.size() != 0;
  }
  if (notify && !have_free.is_null()) {
    have_free.Run();
  }
}

