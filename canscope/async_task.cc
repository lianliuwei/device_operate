#include "canscope/async_task.h"

#include "base/logging.h"
#include "base/location.h"
#include "base/bind.h"

using namespace base;

AsyncTask::AsyncTask()
    : status_(kNoStart) 
    , percent_(0)
    , message_loop_(MessageLoopProxy::current()) {
  DCHECK(message_loop_.get());
}

AsyncTaskStatus AsyncTask::status() {
  AutoLock lock(lock_);
  return status_;
}

int AsyncTask::GetProgress() {
  AutoLock lock(lock_);
  return static_cast<int>(percent_);
}

void AsyncTask::Cancel() {
  LOG_IF(WARNING, cancel_.IsSet()) 
      << "call cancel more than once, may the work thread no response";
  cancel_.Set();
}

void AsyncTask::AddObserver(AsyncTaskObserver* obs) {
  observer_list_.AddObserver(obs);
}

void AsyncTask::RemoveObserver(AsyncTaskObserver* obs) {
  observer_list_.RemoveObserver(obs);
}

bool AsyncTask::HasObserver(AsyncTaskObserver* obs) {
  return observer_list_.HasObserver(obs);
}


void AsyncTask::NotifyStart(base::Value* param) {
  message_loop_->PostTask(FROM_HERE, 
      Bind(&AsyncTask::NotifyStartImpl, this, param));
}

void AsyncTask::NotifyStartImpl(base::Value* param) {
  DCHECK(message_loop_->BelongsToCurrentThread());
  scoped_ptr<base::Value> scoped(param);
  {
  AutoLock lock(lock_);
  CHECK(status_ == kNoStart);
  status_ = kRunning;
  }
  FOR_EACH_OBSERVER(AsyncTaskObserver, observer_list_, 
      OnAsyncTaskStart(this, param));
}

void AsyncTask::NotifyProgress(bool add, double percent, 
                               base::Value* param) {
  if (add) {
    AutoLock lock(lock_);
    percent += percent_;
  }
  CHECK(0 <= percent && percent <= 100);
  message_loop_->PostTask(FROM_HERE, 
      Bind(&AsyncTask::NotifyProgressImpl, this, percent, param));
}


void AsyncTask::NotifyProgressImpl(double percent,
                                 base::Value* param) {
  DCHECK(message_loop_->BelongsToCurrentThread());
  scoped_ptr<base::Value> scoped(param);
  {
  AutoLock lock(lock_);
  CHECK(status_ == kRunning);
  CHECK(0 <= percent && percent <= 100);
  percent_ = percent;
  }
  FOR_EACH_OBSERVER(AsyncTaskObserver, observer_list_, 
      OnAsyncTaskProgress(this, static_cast<int>(percent), param));
}

void AsyncTask::NotifyError(base::Value* param) {
  message_loop_->PostTask(FROM_HERE, 
      Bind(&AsyncTask::NotifyFinishImpl, this, kError, param));
}

void AsyncTask::NotifyFinish(base::Value* param) {
  message_loop_->PostTask(FROM_HERE, 
      Bind(&AsyncTask::NotifyFinishImpl, this, kFinish, param));
}

void AsyncTask::NotifyCancel(base::Value* param) {
  message_loop_->PostTask(FROM_HERE, 
      Bind(&AsyncTask::NotifyFinishImpl, this, kCancel, param));
}

bool AsyncTask::UserCancel() {
  AutoLock lock(lock_);
  return cancel_.IsSet();
}

void AsyncTask::NotifyFinishImpl(AsyncTaskStatus status, 
                                base::Value* param) {
  DCHECK(status != kNoStart && status != kRunning);
  DCHECK(message_loop_->BelongsToCurrentThread());
  scoped_ptr<base::Value> scoped(param);
  {
  AutoLock lock(lock_);
  CHECK(status_ == kRunning);
  status_ = status;
  }
  FOR_EACH_OBSERVER(AsyncTaskObserver, observer_list_, 
      OnAsyncTaskFinish(this, status, param));
}
