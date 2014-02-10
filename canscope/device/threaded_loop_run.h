#pragma once

#include "base/basictypes.h"

#include "base/memory/ref_counted.h"
#include "base/single_thread_task_runner.h"
#include "base/synchronization/lock.h"
#include "base/observer_list_threadsafe.h"

// start a loop run task
// single only to notify the sub class to stop when finish one job
// because some task need more then one callback
class ThreadedLoopRun : public base::RefCountedThreadSafe<ThreadedLoopRun> {
public:
  class Observer {
  public:
    // task state
    virtual void StateChanged() = 0;
   
  protected:
    Observer() {}
    virtual ~Observer() {}
  };

  void AddObserver(Observer* obs) {
    observer_list_->AddObserver(obs);
  }

  void RemoveObserver(Observer* obs) {
    observer_list_->RemoveObserver(obs);
  }

  ThreadedLoopRun();

  // must set run_thread before call Start()
  void set_run_thread(scoped_refptr<base::SingleThreadTaskRunner> run_thread) {
    run_thread_ = run_thread;
  }
  scoped_refptr<base::SingleThreadTaskRunner> run_thread() const {
    return run_thread_;
  }

  void Start();
  void Single();
  void Stop();

  bool IsRunning() const;


protected:
  bool NeedStop() const;
  bool IsSingle() const;

  // all call on run thread
  // return true if continue run
  virtual bool LoopRunImp() = 0;
  // notify state changed
  virtual void OnStateChanged();
  // called when loop going to stop state
  virtual void OnStop();

  void NotifyStateChanged();

  // no lock
  base::TimeDelta next_loop_delay_;

  friend class base::RefCountedThreadSafe<ThreadedLoopRun>;
  virtual ~ThreadedLoopRun() {}

private:
  void LoopRun();
  void Reset();

  bool start_;
  bool single_;
  bool stop_;
  bool running_;

  mutable base::Lock lock_;

  scoped_refptr<base::SingleThreadTaskRunner> run_thread_;

  scoped_refptr<ObserverListThreadSafe<Observer>> observer_list_;

  DISALLOW_COPY_AND_ASSIGN(ThreadedLoopRun);
};