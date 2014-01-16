#pragma once

#include <vector>

#include "base/string16.h"
#include "base/observer_list_threadsafe.h"

#include "canscope/device/threaded_loop_run.h"
#include "canscope/device/canscope_device_manager.h"

namespace canscope {

enum DeviceStatus {
  kExist,
  kOnline,
  kOffline,
  kNoExist,
//  Device Collecter status
//   kStart,
//   kStop,
};

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
    // offline <-> online
    virtual void DeviceStateChanged() = 0;
    // include DeviceStateChanged
    virtual void DeviceListChanged() = 0;

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
      bool stop_on_found, 
      bool auto_open_first, bool auto_open_all, 
      bool auto_close_offline);

  virtual ~CANScopeDeviceFinder();


  // device
  std::vector<string16> GetDeviceList();
  // return false if device path is no exist in current device list
  // happen when UI click open, by the device is pull out
  bool OpenDevice(string16 device_path);
  DeviceStatus GetDeviceStatus(string16 device_path) const;
  scoped_refptr<CANScopeDeviceManager> GetDeviceManager(string16 device_path) const;   
  bool CloseDevice(string16 device_path);

private:
  // config before start
  bool stop_on_found_;

  // only one can be true
  bool auto_open_first_;
  bool auto_open_all_;

  bool auto_close_offline_;

  // implement ThreadLoopRun
  virtual bool LoopRunImp();
  virtual void OnStateChanged();

  void NotifyStateChanged();
  void NotifyDeviceStateChanged();
  void NotifyDeviceListChanged();

  bool Reset();

  // open first_ executed?
  bool open_first_;

  typedef std::map<string16, DeviceStatus> DeviceStatusMap;
  DeviceStatusMap device_to_status_;

  typedef std::map<string16, scoped_refptr<CANScopeDeviceManager>> PathDeviceMap;
  PathDeviceMap path_to_device_;

  ObserverListThreadSafe<Observer> observer_list_;

  base::Lock lock_;

  DISALLOW_COPY_AND_ASSIGN(CANScopeDeviceFinder);
};

} // namespace canscope
