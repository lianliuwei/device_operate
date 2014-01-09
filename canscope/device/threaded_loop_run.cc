#include "canscope/device/threaded_loop_run.h"

#include "base/bind.h"
#include "base/location.h"

using namespace base;

// start_ single_ stop_ represent operate, running_ represent task state
void ThreadedLoopRun::Start() {
  {
    base::AutoLock lock(lock_);
    if (start_ || single_)
      return;
    start_ = true;
  }
  run_thread_->PostTask(FROM_HERE, Bind(&ThreadedLoopRun::LoopRun, this));
}

void ThreadedLoopRun::Single() {
  {
    base::AutoLock lock(lock_);
    if (start_ || single_)
      return;
    single_ = true;
  }
  run_thread_->PostTask(FROM_HERE, Bind(&ThreadedLoopRun::LoopRun, this));
}

void ThreadedLoopRun::Stop() {
  {
    base::AutoLock lock(lock_);
    if (!start_ || !single_ || stop_)
      return;
    stop_ = true;
  }

}

bool ThreadedLoopRun::IsRunning() const {
  base::AutoLock lock(lock_);
  return running_;
}

bool ThreadedLoopRun::NeedStop() const {
  base::AutoLock lock(lock_);
  return stop_;
}

bool ThreadedLoopRun::IsSingle() const {
  base::AutoLock lock(lock_);
  return single_;
}


void ThreadedLoopRun::Reset() {
  running_ = false;
  start_ = false;
  single_ = false;
  stop_ = false;
}

void ThreadedLoopRun::LoopRun() {
  {
    base::AutoLock lock(lock_);
    if (stop_) {
      Reset();
      return;
    }
    if (!running_) {
      running_ = true;
      OnStateChanged();
    }
  }
  bool ret = LoopRunImp();

  {
    base::AutoLock lock(lock_);
    if (stop_) { 
      Reset();
      OnStateChanged();
      return;
    }
    if (!ret ) {
      Reset();
      OnStateChanged();
      return;
    }
  }
  run_thread_->PostDelayedTask(FROM_HERE, 
      Bind(&ThreadedLoopRun::LoopRun, this), next_loop_delay_);
}

ThreadedLoopRun::ThreadedLoopRun() {
  Reset();
}
