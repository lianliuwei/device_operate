#pragma once

#include <vector>

#include "base/string16.h"
#include "base/observer_list_threadsafe.h"

#include "canscope/device/threaded_loop_run.h"
#include "canscope/device/canscope_device.h"
#include "canscope/device/devices_manager_observer.h"

namespace canscope {


// all Open Close CANScopeDevice must through CANScopeDeviceFinder
// or must add CANScopeDevice Destroy observer
// 
// Enum Devices and Check Devices online when offine.
class DevicesManager : public base::RefCountedThreadSafe<DevicesManager> {
public:
  void AddObserver(DevicesManagerObserver* obs) {
    observer_list_->AddObserver(obs);
  }

  void RemoveObserver(DevicesManagerObserver* obs) {
    observer_list_->RemoveObserver(obs);
  }
 
  // Create and Destroy
  static void Create(scoped_refptr<base::SingleThreadTaskRunner> run_thread,
                     bool auto_open_first, bool auto_open_all, 
                     bool auto_close_offline);

  static void Destroy();

  static scoped_refptr<DevicesManager> Get();
  
  // CANScopeDeviceFinder callback
  // call only device thread.
  void DeviceListChanged(std::vector<string16> devices);

  // device
  std::vector<string16> GetDeviceList();
  // return false if device path is no exist in current device list
  // happen when UI click open, by the device is pull out
  void OpenDevice(string16 device_path);
  void CloseDevice(string16 device_path);
  DeviceStatus GetDeviceStatus(string16 device_path) const;
  scoped_refptr<CANScopeDevice> GetDeviceManager(string16 device_path) const;   

  
  scoped_refptr<base::SingleThreadTaskRunner> run_thread() const {
    return run_thread_;
  }

private:
  DevicesManager(scoped_refptr<base::SingleThreadTaskRunner> run_thread,
      bool auto_open_first, bool auto_open_all, 
      bool auto_close_offline);

  friend class base::RefCountedThreadSafe<DevicesManager>;
  virtual ~DevicesManager() {}

  // run on run_thread
  bool OpenDeviceImpl(string16 device_path, bool notify);
  bool CloseDeviceImpl(string16 device_path, bool notify);
  void DestroyDevicesImpl();
  void DestroyDevices();

  // only one can be true
  bool auto_open_first_;
  bool auto_open_all_;

  bool auto_close_offline_;

  void NotifyDeviceStateChanged();
  void NotifyDeviceListChanged();

  // open first_ executed?
  bool open_first_;

  typedef std::map<string16, DeviceStatus> DeviceStatusMap;
  DeviceStatusMap device_to_status_;

  typedef std::map<string16, scoped_refptr<CANScopeDevice>> PathDeviceMap;
  PathDeviceMap path_to_device_;

  scoped_refptr<ObserverListThreadSafe<DevicesManagerObserver>> observer_list_;

  scoped_refptr<base::SingleThreadTaskRunner> run_thread_;

  mutable base::Lock lock_;

  DISALLOW_COPY_AND_ASSIGN(DevicesManager);
};

} // namespace canscope
