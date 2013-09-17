#include "canscope/device/device_base.h"

#include "base/logging.h"

using namespace base;

DeviceBase::DeviceBase()
    : seq_(kNullSeq)
    , lock_name_("")
    // init no lock
    , event_(true, true) {

}

bool DeviceBase::Lock(int* seq, std::string lock_name) {
  {
  DCHECK(seq);
  AutoLock scoped(lock_);   
  // already be lock
  if (seq_ != kNullSeq)
    return false;
  *seq = seq_ = IncSeq();
  lock_name_ = lock_name;
  event_.Reset();
  }
  // release lock before notify, or may dead lock.
  LockStatusChanged();
  return true;
}

void DeviceBase::UnLock(int seq) {
  {
  AutoLock scoped(lock_);
  CHECK(seq == seq_);
  seq_ = kNullSeq;
  lock_name_ = "";
  event_.Signal();
  }
  LockStatusChanged();
}

bool DeviceBase::IsLocked() {
  AutoLock scoped(lock_);
  return seq_ != kNullSeq;
}

void DeviceBase::Wait() {
  event_.Wait();
}

int DeviceBase::IncSeq() {
  ++seq_;
  // pass kNullSeq
  if (seq_ == kNullSeq)
    ++seq_;
  return seq_;
}

bool DeviceBase::CheckLock(int seq) {
  AutoLock scoped(lock_);
  return seq_ == seq;
}
