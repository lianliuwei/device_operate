#pragma once

#include <string>

#include "base/memory/ref_counted.h"
#include "base/synchronization/waitable_event.h"
#include "base/synchronization/lock.h"

class ConfigManager;

namespace base {
class Value;
}

namespace canscope {
class ValueMapDevicePropertyStore;
}

class DeviceBase {
public:
  DeviceBase(ConfigManager* config_manager);
  virtual ~DeviceBase() {}

  bool Lock(int* seq, std::string lock_name);

  bool CheckLock(int seq);

  void UnLock(int seq);

  bool IsLocked();

  // wait for unlock
  // when wake up, the device may still be lock, check again.
  void TryWaitLock();

  // must CheckConfig() before LoadFromConfig()
  void LoadFromConfig();
  void InitFromConfig();
  virtual bool CheckConfig(base::Value** error_info) { return true; }
  void UpdateConfig(const std::string& reason);
  ConfigManager* config_manager() { return config_manager_; }

protected:
  virtual void LockStatusChanged() = 0;

  virtual canscope::ValueMapDevicePropertyStore* DevicePrefs() = 0;

private:
  int IncSeq();
  
  base::Lock lock_;
  int seq_;

  std::string lock_name_;
  base::WaitableEvent event_;
  DeviceBase* father_;

  ConfigManager* config_manager_;
};

// for single operate, no lock.
const int kNullSeq = 0;

// if continue or just return error when set device when device is lock.
bool ReturnErrorOrContinue();
