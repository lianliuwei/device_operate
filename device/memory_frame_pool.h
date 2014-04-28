#pragma once

#include <list>
#include <queue>

#include "base/basictypes.h"
#include "base/memory/ref_counted.h"
#include "base/synchronization/lock.h"
#include "base/callback.h"

// TODO add lock
// TODO add free change notify.
class MemoryFramePool : public base::RefCountedThreadSafe<MemoryFramePool> {
public:
  MemoryFramePool(int frame_size, int pool_size);

  // return NULL mean pool full.
  uint8* Alloc();
  void Free(uint8* frame);

  int frame_size() const { return frame_size_; }
  int pool_size() const { return pool_size_; }
  
  // Left Frame
  int Left();

  // from zero free to at least one free
  // after reset the callback the have_free may be call. 
  // duty to need callback in no lock state
  void set_have_free_callback(base::Closure have_free);

private:
  friend class base::RefCountedThreadSafe<MemoryFramePool>;
  ~MemoryFramePool();

  bool HitCeiling();
  void ReleaseFree();

  std::list<uint8*> alloced;
  std::queue<uint8*> free;
  
  int frame_size_;
  int pool_size_;
  
  base::Closure have_free_;

  base::Lock lock_;
  DISALLOW_COPY_AND_ASSIGN(MemoryFramePool);
};