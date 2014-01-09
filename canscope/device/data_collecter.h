#pragma once

#include "base/memory/ref_counted.h"
#include "base/callback.h"

// call Start() Stop() according thread and get notify at each thread
// the add observer


class DataCollecter : public base::RefCountedThreadSafe<DataCollecter> {
public:
  class Observer {
  public:
    void StateChanged() = 0;

  protected:
    Observer() {}
    virtual ~Observer() {}
  };

  typedef Callback<void()> RunOnFaultDelegate;

  DataCollecter(scoped_refptr<SingleThreadTaskRunner> runner, RunOnFaultDelegate run_on_fault);
  virtual ~DataCollecter();

  // operate command
  void Start();
  void Stop();
  void Single();
  
  // data collect freq 
  void SetFreq(double freq);
  double Freq() const;

  bool IsStart() const;
  // call when back online
  void ReRun();

  int Count();

  void AddObserver();
  void RemoveObserver();

protected:
  ObserverListThreadSafe<Observer> observer_;
  base::Lock lock_;
private:
  
};

enum OscDataCollecterState {
    
};

class OscDataCollecter : public DataCollecter {
public:
  typedef base::Callback<void(OscRawDataHandle)> DataCollectedCallback;

  OscDataCollecter(DataCollectedCallback call_back);
protected:

private:
  bool calibrated;

  void OnDataCollected();
};