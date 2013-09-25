#include "canscope/device/scoped_device_lock.h"

#include "base/logging.h"

ScopedDeviceLock::ScopedDeviceLock(
  DeviceBase* deivce, std::string name, bool wait)
    : device_(deivce)
    , seq_(kNullSeq) {
  CHECK(device_);
  while(true) {
    bool ret = device_->Lock(&seq_, name);
    if (ret)
      return;
    if (wait) {
      device_->TryWaitLock();
    } else {
      return;
    }
  }
}

ScopedDeviceLock::~ScopedDeviceLock() {
  if (IsLocked()) {
    device_->UnLock(seq_);
  }
}

bool ScopedDeviceLock::IsLocked() {
  return seq_ != kNullSeq;
}
