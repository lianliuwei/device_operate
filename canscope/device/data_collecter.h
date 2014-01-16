#pragma once

#include "base/memory/ref_counted.h"
#include "base/callback.h"

#include "canscope/device/thread_loop_run.h"

// call Start() Stop() according thread and get notify at each thread
// the add observer

class DataCollecter : public ThreadedLoopRun {
public:
  class Observer {
  public:
    void StateChanged() = 0;

  protected:
    Observer() {}
    virtual ~Observer() {}
  };

  void AddObserver();
  void RemoveObserver();

  typedef base::Callback<void()> RunOnFaultDelegate;

  DataCollecter(scoped_refptr<SingleThreadTaskRunner> runner,
      RunOnFaultDelegate run_on_fault);
  virtual ~DataCollecter();


  // data collect freq 
  void SetFreq(double freq);
  double Freq() const;
  
  // call when back online
  void ReRun();

  void set_stop_by_offine(bool value) { stop_by_offine_ = value; }
  bool stop_by_offine() const { return stop_by_offine_; }

  int Count() const;

  enum LoopState {
    // call loop next message loop
    NEXT_LOOP,
    // call loop in this message loop
    IMMEDIATE,
    // stop, wait for some one call star again.
    STOP,
  };


protected:
  ObserverListThreadSafe<Observer> observer_;
  bool stop_by_offine_;


  base::Lock lock_;
  
  // implement ThreadLoopRun
  virtual void OnStateChanged() OVERRIDE;

  virtual LoopState OnLoopRun() = 0;

private:
  // implement ThreadLoopRun
  virtual bool LoopRunImp() OVERRIDE;

};



