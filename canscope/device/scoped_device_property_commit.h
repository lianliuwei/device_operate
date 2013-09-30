#pragma once

#include <string>

#include "base/callback.h"
#include "base/synchronization/waitable_event.h"

#include "canscope/async_task.h"
#include "canscope/device_errors.h"

class DeviceHandleBase;
class DeviceBase;

class ScopedDevicePropertyCommit {
public:
  // sync commit
  ScopedDevicePropertyCommit(DeviceHandleBase* handle, 
                             const char* reason,
                             base::Closure commit_call);
  ScopedDevicePropertyCommit(DeviceHandleBase* handle,
                             const char* reason);
  // async commit
  ScopedDevicePropertyCommit(DeviceHandleBase* handle, 
                             AsyncTaskHandle async_task, 
                             const char* reason,
                             base::Closure commit_call);
  ScopedDevicePropertyCommit(DeviceHandleBase* handle, 
                             AsyncTaskHandle async_task,
                             const char* reason);

  ~ScopedDevicePropertyCommit();

private:
  void Init();

  void CommitSync();
  void CommitAsync();

  void CommitSyncImpl(base::DictionaryValue* value);
  static void CommitAsyncImpl(base::DictionaryValue* value, 
                              AsyncTaskHandle async_task, 
                              base::Closure commit_call,
                              std::string reason,
                              DeviceBase* device);

  base::Closure commit_call_;
  AsyncTaskHandle async_task_;
  DeviceHandleBase* handle_;
  DeviceBase* device_;
  std::string reason_;

  base::WaitableEvent event_;
  // error happen on device
  canscope::device::Error device_error_;

  bool old_batch_;
  DISALLOW_COPY_AND_ASSIGN(ScopedDevicePropertyCommit);
};
