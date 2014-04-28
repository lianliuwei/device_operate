#include "canscope/device/scoped_device_property_commit.h"

#include "device/property/value_map_device_property_store.h"

#include "canscope/device/device_base.h"
#include "canscope/device/device_handle_base.h"

ScopedDevicePropertyCommit::ScopedDevicePropertyCommit(
    DeviceHandleBase* handle, 
    const char* reason,
    base::Closure commit_call)
    : commit_call_(commit_call) 
    , device_(handle->device()) 
    , handle_(handle)
    , reason_(reason)
    , event_(true, false)
    , old_batch_(false) {
  Init();
}

ScopedDevicePropertyCommit::ScopedDevicePropertyCommit(
    DeviceHandleBase* handle, 
    const char* reason)
    : device_(handle->device()) 
    , handle_(handle)
    , reason_(reason)
    , event_(true, false)
    , old_batch_(false) {
  Init();
}

ScopedDevicePropertyCommit::ScopedDevicePropertyCommit(
    DeviceHandleBase* handle, 
    AsyncTaskHandle async_task, 
    const char* reason, 
    base::Closure commit_call)
    : commit_call_(commit_call) 
    , async_task_(async_task)
    , device_(handle->device()) 
    , handle_(handle)
    , reason_(reason)
    , event_(true, false)
    , old_batch_(false){
  Init();
}

ScopedDevicePropertyCommit::ScopedDevicePropertyCommit(
    DeviceHandleBase* handle, 
    AsyncTaskHandle async_task, 
    const char* reason)
    : async_task_(async_task)
    , device_(handle->device()) 
    , handle_(handle)
    , reason_(reason)
    , event_(true, false)
    , old_batch_(false) {
  Init();
}


void ScopedDevicePropertyCommit::Init() {
  old_batch_ = handle_->IsBatchMode();
  if (old_batch_) {
    DLOG(WARNING) << 
        "batch commit in batch commit, check the code see if necessary";
  }
  handle_->SetBatchMode(true);
}

ScopedDevicePropertyCommit::~ScopedDevicePropertyCommit() {
  if (async_task_.get()) {
    CommitAsync();
  } else {
    CommitSync();
  }
  handle_->SetBatchMode(old_batch_);
}

void ScopedDevicePropertyCommit::CommitSync() {
  DictionaryValue* value = handle_->DevicePrefs()->Serialize();
  handle_->PostDeviceTask(
      base::Bind(&ScopedDevicePropertyCommit::CommitSyncImpl, 
          base::Unretained(this), value));
  
  event_.Wait();
  event_.Reset();

  // set this thread error from device thread.
  SetDeviceError(device_error_);

  handle_->FetchNewPref();
}

void ScopedDevicePropertyCommit::CommitSyncImpl(base::DictionaryValue* value) {
  canscope::device::CleanError();

  device_->DevicePrefs()->ChangeContent(value);
  if (!commit_call_.is_null()) {
    commit_call_.Run();
  }
  device_->UpdateConfig(reason_);

  device_error_ = canscope::device::LastDeviceError();

  event_.Signal();
}

void ScopedDevicePropertyCommit::CommitAsync() {
  async_task_->NotifyStart(NULL);
  DictionaryValue* value = handle_->DevicePrefs()->Serialize();
  // build AsyncCommit to finish commit because in async mode 
  // this obj will be destroy already when PostTask start run.
  handle_->PostDeviceTask(
      Bind(ScopedDevicePropertyCommit::CommitAsyncImpl, 
          value, async_task_, commit_call_, reason_, device_));
}

// static 
void ScopedDevicePropertyCommit::CommitAsyncImpl(base::DictionaryValue* value, 
                                                 AsyncTaskHandle async_task, 
                                                 base::Closure commit_call,
                                                 std::string reason,
                                                 DeviceBase* device) {
  if (async_task->UserCancel()) {
    async_task->NotifyCancel(NULL);
    return;
  }
  canscope::device::CleanError();

  device->DevicePrefs()->ChangeContent(value);
  if (!commit_call.is_null()) {
    commit_call.Run();
  }
  device->UpdateConfig(reason);

  if (canscope::device::LastDeviceError() == canscope::device::OK) {
    async_task->NotifyFinish(NULL);
  } else {
    async_task->NotifyError(
        canscope::device::ErrorAsDictionary(
            canscope::device::LastDeviceError()));
  }
}
