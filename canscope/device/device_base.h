#pragma once

#include <string>

#include "base/memory/ref_counted.h"
#include "base/synchronization/waitable_event.h"
#include "base/synchronization/lock.h"
#include "base/single_thread_task_runner.h"

class ConfigManager;
class ScopedDevicePropertyCommit;

namespace base {
class Value;
}

namespace device {
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

   
  // must set run_thread before call other
  void set_run_thread(scoped_refptr<base::SingleThreadTaskRunner> run_thread) {
    run_thread_ = run_thread;
  }
  scoped_refptr<base::SingleThreadTaskRunner> run_thread() const {
    return run_thread_;
  }

protected:
  friend class ScopedDevicePropertyCommit;

  virtual void LockStatusChanged();

  virtual ::device::ValueMapDevicePropertyStore* DevicePrefs() = 0;

private:
  int IncSeq();
  
  base::Lock lock_;
  int seq_;

  std::string lock_name_;
  base::WaitableEvent event_;
  DeviceBase* father_;

  ConfigManager* config_manager_;

  scoped_refptr<base::SingleThreadTaskRunner> run_thread_;
};

// for single operate, no lock.
const int kNullSeq = 0;

// if continue or just return error when set device when device is lock.
bool ReturnErrorOrContinue();
