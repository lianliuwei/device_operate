#pragma once

#include "base/memory/ref_counted.h"

#include "device/memory_usage_pool.h"

namespace canscope {

class FrameRawData : public base::RefCountedThreadSafe<FrameRawData> {
public:
  // self alloc data
  FrameRawData(int size);
  uint8* data() { return raw_data_; }
  const uint8* data() const { return raw_data_; }

  int size() const { return size_; }
  int real_size() const { return real_size_; }
  void set_real_size(int size) { real_size_ = size; }

protected:
  FrameRawData(uint8* raw_data, int size, bool own);

  void set_data(uint8* data, int size);

  friend class base::RefCountedThreadSafe<FrameRawData>;
  virtual ~FrameRawData();

private:
  uint8* raw_data_;
  int size_;
  int real_size_;
  bool own_;

  DISALLOW_COPY_AND_ASSIGN(FrameRawData);
};

typedef scoped_refptr<FrameRawData> FrameRawDataHandle;

// check IsAllocOk Ofter ctor, if no alloc ok, this object is invalid. just destroy it.
class PooledFrameRawData : public FrameRawData {
public:
  PooledFrameRawData(int size, scoped_refptr<MemoryUsagePool> pool);

  bool IsAllocOK() const { return alloc_ok_; }
  void AllocAgain();

private:
  virtual ~PooledFrameRawData();

  scoped_refptr<MemoryUsagePool> pool_;

  bool alloc_ok_;

  int need_size_;

  DISALLOW_COPY_AND_ASSIGN(PooledFrameRawData);
};

} // namespace canscope