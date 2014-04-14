#pragma once

#include "base/callback.h"
#include "base/single_thread_task_runner.h"
#include "base/synchronization/waitable_event.h"

class SyncCall {
public:
  SyncCall(scoped_refptr<base::SingleThreadTaskRunner> task_runner);
  ~SyncCall() {}

  void set_callback(base::Closure callback) { callback_ = callback; }
  void Call();

private:
  void CallImpl();

  base::WaitableEvent event_;
  base::Closure callback_;
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;

  DISALLOW_COPY_AND_ASSIGN(SyncCall);
};
