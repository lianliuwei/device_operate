#include "canscope/device/canscope_device_finder.h"

#include "base/synchronization/lock.h"

#include "canscope/device/usb/usb_port.h"

using namespace std;
namespace canscope {

bool CANScopeDeviceFinder::LoopRunImp() {
  // EnumDevice
  std::vector<string16> devices;
  bool ret = EnumDevices(&devices);
  DCHECK(ret);
  std::vector<string16> copy_devices(devices);
  
  // MergeToDeviceList
  bool list_change = false;
  bool state_change = false;
  {
  base::AutoLock lock(lock_);
  // using set operate
  vector<string16> off_list;
  for (DeviceStatusMap::iterator it = device_to_status_.begin(); 
      it != device_to_status_.end(); ++it) {
    vector<string16>::iterator finded = 
      find(devices.begin(), devices.end(), it->first);
    // already exist
    if (finded != devices.end()) {
      devices.erase(finded);
      if (it->second == kOffline) {
        it->second = kOnline;
        state_change = true;
      }
    } else {
      off_list.push_back(it->first);
    }
  }
  for (std::vector<string16>::iterator it = off_list.begin();
      it != off_list.end(); ++it) {
    if (device_to_status_[*it] == kExist) {
      list_change = true;
      device_to_status_.erase(*it);
    } else if (device_to_status_[*it] == kOnline) {
      state_change = true;
      device_to_status_[*it] = kOffline;
      LOG(INFO) << "enum device find device offline";
    }
  }
  for (std::vector<string16>::iterator it = devices.begin(); 
      it != devices.end(); ++it) {
    list_change = true;
    device_to_status_.insert(std::make_pair(*it, kExist));
  }
  }

  // if need OpenDevice
  int find_device_size = static_cast<int>(copy_devices.size());
  if (auto_open_first_ && !open_first_ && find_device_size > 0) {
    OpenDevice(devices[0]);
    state_change = true;
    open_first_ = true;
  }
  if (auto_open_all_ || auto_close_offline_) {
    vector<string16> need_open;
    vector<string16> need_close;
    {
    base::AutoLock lock(lock_);
    for (DeviceStatusMap::iterator it = device_to_status_.begin(); 
        it != device_to_status_.end(); ++it) {
      if (it->second == kExist) {
        need_open.push_back(it->first);
      } else if (it->second == kOffline) {
        need_close.push_back(it->first);
      }
    }
    }
    if (auto_open_all_) {
      for (std::vector<string16>::iterator it = need_open.begin();
        it != need_open.end(); ++it) {
          OpenDevice(*it);
          state_change = true;
      }
    }

    if (auto_close_offline_) {
      for (std::vector<string16>::iterator it = need_close.begin();
        it != need_close.end(); ++it) {
          CloseDevice(*it);
          list_change = true;
      }
    }

  }
  if (list_change)
    NotifyDeviceListChanged();
  if (!list_change && state_change)
    NotifyDeviceStateChanged();

  // found something and stop
  if (stop_on_found_ && find_device_size > 0)
    return false;

  if (IsSingle())
    return false;

  return true;
}

CANScopeDeviceFinder::CANScopeDeviceFinder(
    scoped_refptr<base::SingleThreadTaskRunner> run_thread, 
    bool stop_on_found, 
    bool auto_open_first, 
    bool auto_open_all, 
    bool auto_close_offline)
    : stop_on_found_(stop_on_found) 
    , auto_open_first_(auto_open_first)
    , auto_open_all_(auto_open_all) 
    , auto_close_offline_(auto_close_offline)
    , open_first_(false) {
  DCHECK(!(auto_open_first && auto_open_all));
  set_run_thread(run_thread);
}

void CANScopeDeviceFinder::NotifyStateChanged() {
  observer_list_.Notify(&Observer::StateChanged);
}

void CANScopeDeviceFinder::NotifyDeviceStateChanged() {
  observer_list_.Notify(&Observer::DeviceStateChanged);
}

void CANScopeDeviceFinder::NotifyDeviceListChanged() {
  observer_list_.Notify(&Observer::DeviceListChanged);
}

void CANScopeDeviceFinder::OnStateChanged() {
  NotifyStateChanged();
}


} // namespace canscope