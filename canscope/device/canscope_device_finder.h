#pragma once

#include <vector>

#include "base/string16.h"
#include "base/observer_list_threadsafe.h"

#include "canscope/device/threaded_loop_run.h"
#include "canscope/device/canscope_device_manager.h"

namespace canscope {

// all Open Close CANScopeDeviceManager must through CANScopeDeviceFinder
// or must add CANScopeDeviceManager Destroy observer
// 
// Enum Devices and Check Devices online when offine.
class CANScopeDeviceFinder : public ThreadedLoopRun {
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
    observer_list_.AddObserver(obs);
  }

  void RemoveObserver(Observer* obs) {
    observer_list_.RemoveObserver(obs);
  }
 
  CANScopeDeviceFinder(scoped_refptr<base::SingleThreadTaskRunner> run_thread,
      bool stop_on_found);

  virtual ~CANScopeDeviceFinder();


private:
  // config before start
  bool stop_on_found_;


  // implement ThreadLoopRun
  virtual bool LoopRunImp() OVERRIDE;
  virtual void OnStateChanged() OVERRIDE;

  void NotifyStateChanged();
  void NotifyDeviceStateChanged();
  void NotifyDeviceListChanged();

  bool Reset();

  // open first_ executed?
  bool open_first_;

  ObserverListThreadSafe<Observer> observer_list_;


  DISALLOW_COPY_AND_ASSIGN(CANScopeDeviceFinder);
};

} // namespace canscope
