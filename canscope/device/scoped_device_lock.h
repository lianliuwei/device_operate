#pragma once

#include <string>

#include "canscope/device/device_base.h"

class ScopedDeviceLock {
public:
  ScopedDeviceLock(DeviceBase* deivce, std::string name, bool wait);
  ~ScopedDeviceLock();

  bool IsLocked();
  int seq() { return seq_; }

private:
  int seq_;
  DeviceBase* device_;
  DISALLOW_COPY_AND_ASSIGN(ScopedDeviceLock);
};
