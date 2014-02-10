#pragma once

#include <vector>

#include "base/string16.h"
#include "base/observer_list_threadsafe.h"

#include "canscope/device/threaded_loop_run.h"

namespace canscope {

// all Open Close CANScopeDeviceManager must through CANScopeDeviceFinder
// or must add CANScopeDeviceManager Destroy observer
// 
// Enum Devices and Check Devices online when offine.
class CANScopeDeviceFinder : public ThreadedLoopRun {
public:
  CANScopeDeviceFinder(scoped_refptr<base::SingleThreadTaskRunner> run_thread,
      bool stop_on_found);

  virtual ~CANScopeDeviceFinder() {}


private:
  // config before start
  bool stop_on_found_;


  // implement ThreadLoopRun
  virtual bool LoopRunImp() OVERRIDE;

  void NotifyStateChanged();
  void NotifyDeviceStateChanged();
  void NotifyDeviceListChanged();

  bool Reset();

  // open first_ executed?
  bool open_first_;

  DISALLOW_COPY_AND_ASSIGN(CANScopeDeviceFinder);
};

} // namespace canscope
