#include "canscope/device/devices_manager.h"

#include "base/synchronization/lock.h"

#include "canscope/device/usb/usb_port.h"
#include "canscope/device/canscope_device_constants.h"
#include "canscope/device/config_util.h"

using namespace std;

namespace {
static scoped_refptr<canscope::DevicesManager> g_manager;
}

namespace canscope {

// static
void DevicesManager::Create(scoped_refptr<base::SingleThreadTaskRunner> run_thread, 
                            bool auto_open_first, 
                            bool auto_open_all, 
                            bool auto_close_offline) {
  DCHECK(g_manager.get() == NULL);
  g_manager = new DevicesManager(run_thread, 
      auto_open_first, auto_open_all, auto_close_offline);
}

// static
void DevicesManager::Destroy() {
 DCHECK(g_manager.get() != NULL);
 g_manager->DestroyDevices();
 g_manager = NULL;
}

scoped_refptr<DevicesManager> DevicesManager::Get() {
  return g_manager;
}

DevicesManager::DevicesManager(
    scoped_refptr<base::SingleThreadTaskRunner> run_thread, 
    bool auto_open_first, 
    bool auto_open_all, 
    bool auto_close_offline)
    : run_thread_(run_thread)
    , auto_open_first_(auto_open_first)
    , auto_open_all_(auto_open_all) 
    , auto_close_offline_(auto_close_offline)
    , open_first_(false) {
  DCHECK(!(auto_open_first && auto_open_all));
  DCHECK(run_thread.get() != NULL);
  observer_list_ = new ObserverListThreadSafe<DevicesManagerObserver>();
}

void DevicesManager::NotifyDeviceStateChanged() {
  observer_list_->Notify(&DevicesManagerObserver::DeviceStateChanged);
}

void DevicesManager::NotifyDeviceListChanged() {
  observer_list_->Notify(&DevicesManagerObserver::DeviceListChanged);
}

void DevicesManager::DeviceListChanged(std::vector<string16> devices) {
  DCHECK(run_thread_->BelongsToCurrentThread());

  // EnumDevice
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
      device_to_status_[*it] = kOffline;
      state_change = true;

      LOG(INFO) << "enum device find device offline";
    }
  }
  for (std::vector<string16>::iterator it = devices.begin(); 
      it != devices.end(); ++it) {
    device_to_status_.insert(std::make_pair(*it, kExist));
    list_change = true;
  }
  }

  // if need OpenDevice
  int find_device_size = static_cast<int>(copy_devices.size());
  if (auto_open_first_ && !open_first_ && find_device_size > 0) {
    bool ret = OpenDeviceImpl(copy_devices[0], false);
    state_change = true;
    // when open try next time.
    open_first_ = ret;
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
          OpenDeviceImpl(*it, false);
          state_change = true;
      }
    }

    if (auto_close_offline_) {
      for (std::vector<string16>::iterator it = need_close.begin();
        it != need_close.end(); ++it) {
          CloseDeviceImpl(*it, false);
          list_change = true;
      }
    }

  }
  if (list_change)
    NotifyDeviceListChanged();
  if (!list_change && state_change)
    NotifyDeviceStateChanged();
}

bool DevicesManager::OpenDeviceImpl(string16 device_path, bool notify) {
  DCHECK(run_thread_->BelongsToCurrentThread());
  {
    base::AutoLock lock(lock_);
    DeviceStatusMap::iterator it =  device_to_status_.find(device_path);
    // device no exist
    if (it == device_to_status_.end()) {
      return false;
    }
    DeviceStatus status = it->second;
    // already open
    if (status == kOnline || status == kOffline)
      return false;
  }
  scoped_refptr<CANScopeDevice> device = 
      CANScopeDevice::Create(run_thread_);
  CANScopeDeviceRunner* runner = device->runner();
  runner->Init(device_path);
  runner->auto_init_device = kDefaultAutoInitDevice;
  runner->start_on_device_online = kDefaultStartOnDeviceOnline;
  runner->rerun_on_back_online = kDefaultRerunOnbackOnline;
  device::Error error = runner->InitDevice();
  if (error != device::OK) {
    LOG(WARNING) << "Device Open Error: " << ErrorToString(error) << 
      " Device: " << device_path;

    device->StartDestroying();
    return false;
  }
  AddObserver(runner);
  {
    base::AutoLock lock(lock_);
    DeviceStatusMap::iterator it =  device_to_status_.find(device_path);
    // device no exist
    DCHECK(it != device_to_status_.end());
    it->second = kOnline;

    PathDeviceMap::iterator device_it = path_to_device_.find(device_path);
    DCHECK(device_it == path_to_device_.end());
    path_to_device_[device_path] = device;
  }
  if (notify) {
    NotifyDeviceStateChanged();
  }
  device->Init(GetDefaultConfig());
  device->SetAll();
  device->NotifyCreated();
  return true;

}

bool DevicesManager::CloseDeviceImpl(string16 device_path, bool notify) {
  DCHECK(run_thread_->BelongsToCurrentThread());
  scoped_refptr<CANScopeDevice> device;  
  {
    base::AutoLock lock(lock_);
    DeviceStatusMap::iterator it =  device_to_status_.find(device_path);
    // device no exist
    if (it == device_to_status_.end()) {
      return false;
    }
    DeviceStatus status = it->second;
    // no opened
    if (status == kExist)
      return false;
    PathDeviceMap::iterator device_it = path_to_device_.find(device_path);
    DCHECK(device_it != path_to_device_.end());
    device = device_it->second;
    DCHECK(device.get() != NULL);
  }
  CANScopeDeviceRunner* runner = device->runner();
  RemoveObserver(runner);
  runner->CloseDevice();
  device->StartDestroying();
  {
    base::AutoLock lock(lock_);
    DeviceStatusMap::iterator it =  device_to_status_.find(device_path);
    // device no exist
    DCHECK(it != device_to_status_.end());
    it->second = kExist;

    PathDeviceMap::iterator device_it = path_to_device_.find(device_path);
    DCHECK(device_it != path_to_device_.end());
    path_to_device_.erase(device_it);
  }
  if (notify) {
    NotifyDeviceStateChanged();
  }
  return true;
}

void DevicesManager::DestroyDevices() {
  if (!run_thread_->BelongsToCurrentThread()) {
    run_thread_->PostTask(FROM_HERE, 
      Bind(&DevicesManager::DestroyDevices, this));
    return;
  } else {
    DestroyDevicesImpl();
  }
}

void DevicesManager::DestroyDevicesImpl() {
  PathDeviceMap close_devices;
  {
    base::AutoLock lock(lock_);
    close_devices = path_to_device_;
  }
  for (PathDeviceMap::iterator it = close_devices.begin(); 
      it != close_devices.end(); ++it) {
    CloseDeviceImpl(it->first, false);
  }
  NotifyDeviceStateChanged();
}

void DevicesManager::OpenDevice(string16 device_path) {
  if (!run_thread_->BelongsToCurrentThread()) {
    run_thread_->PostTask(FROM_HERE, 
        Bind(&DevicesManager::OpenDevice, this, device_path));
    return;
  } else {
    OpenDeviceImpl(device_path, true);
  }
}

void DevicesManager::CloseDevice(string16 device_path) {
  if (!run_thread_->BelongsToCurrentThread()) {
    run_thread_->PostTask(FROM_HERE, 
      Bind(&DevicesManager::CloseDevice, this, device_path));
    return;
  } else {
    CloseDeviceImpl(device_path, true);
  }
}

DeviceStatus DevicesManager::GetDeviceStatus(string16 device_path) const {
  base::AutoLock lock(lock_);
  DeviceStatusMap::const_iterator it =  device_to_status_.find(device_path);
  // device no exist
  if (it == device_to_status_.end()) {
    return kNoExist;
  }
  return it->second;
}

scoped_refptr<CANScopeDevice> DevicesManager::GetDeviceManager(string16 device_path) const {
  base::AutoLock lock(lock_);
  PathDeviceMap::const_iterator device_it = path_to_device_.find(device_path);
  if (device_it == path_to_device_.end()) {
    return NULL;
  }
  return device_it->second;
}

std::vector<string16> DevicesManager::GetDeviceList() {
  std::vector<string16> devices;

  {
  base::AutoLock lock(lock_);
  for (DeviceStatusMap::iterator it = device_to_status_.begin(); 
    it != device_to_status_.end(); ++it) {
      devices.push_back(it->first);
  }
  }
  return devices;
}

} // namespace canscope