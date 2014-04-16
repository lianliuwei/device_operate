#include "canscope/device/canscope_device_finder.h"

#include "base/synchronization/lock.h"

#include "canscope/device/devices_manager.h"

using namespace std;
namespace canscope {

bool CANScopeDeviceFinder::LoopRunImp() {
  // EnumDevice
  std::vector<string16> devices;
  device::Error err = device_delegate_->EnumDevices(&devices);
  DCHECK(err == device::OK);
  int find_device_size = static_cast<int>(devices.size());

  scoped_refptr<DevicesManager> manager = DevicesManager::Get();
  // manager is destroyed
  if (manager.get() == NULL)
    return false;
  manager->DeviceListChanged(devices);

  // found something and stop
  if (stop_on_found_ && find_device_size > 0)
    return false;

  return !IsSingle();
}

CANScopeDeviceFinder::CANScopeDeviceFinder(
    scoped_refptr<base::SingleThreadTaskRunner> run_thread, 
    bool stop_on_found)
    : stop_on_found_(stop_on_found)
    , device_delegate_(CreateDeviceDelegate()) {
  set_run_thread(run_thread);
  next_loop_delay_ = base::TimeDelta::FromSeconds(1);
}

} // namespace canscope