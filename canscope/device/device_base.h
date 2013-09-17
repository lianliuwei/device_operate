#pragma once

#include <string>

#include "base/memory/ref_counted.h"
#include "base/synchronization/waitable_event.h"
#include "base/synchronization/lock.h"

class DeviceBase {
public:
  DeviceBase();
  virtual ~DeviceBase() {}

  bool Lock(int* seq, std::string lock_name);

  bool CheckLock(int seq);

  void UnLock(int seq);

  bool IsLocked();

  // wait for unlock
  // when wake up, the device may still be lock, check again.
  void Wait();

protected:
  virtual void LockStatusChanged() = 0;

private:
  int IncSeq();
  
  base::Lock lock_;
  int seq_;

  std::string lock_name_;
  base::WaitableEvent event_;
  DeviceBase* father_;
};

// for single operate, no lock.
const int kNullSeq = 0;

// if continue or just return error when set device when device is lock.
bool ReturnErrorOrContinue();
