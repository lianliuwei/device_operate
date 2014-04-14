#include "canscope/device/sync_call.h"

#include "base/location.h"
#include "base/bind.h"

using namespace base;

void SyncCall::CallImpl() {
  if (!callback_.is_null()) {
    callback_.Run();
  }
  event_.Signal();
}

void SyncCall::Call() {
  // on the same thread
  if (task_runner_->BelongsToCurrentThread()) {
    if (callback_.is_null()) {
      callback_.Run();
      return;
    }  
  }
  task_runner_->PostTask(FROM_HERE, Bind(&SyncCall::CallImpl, Unretained(this)));
  event_.Wait();
  event_.Reset();
}

SyncCall::SyncCall(scoped_refptr<SingleThreadTaskRunner> task_runner)
    : task_runner_(task_runner)
    , event_(true, false) {
}
