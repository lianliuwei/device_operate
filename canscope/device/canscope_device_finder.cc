#include "canscope/device/canscope_device_finder.h"

#include "base/synchronization/lock.h"

#include "canscope/device/usb/usb_port.h"
#include "canscope/device/devices_manager.h"

using namespace std;
namespace canscope {

bool CANScopeDeviceFinder::LoopRunImp() {
  // EnumDevice
  std::vector<string16> devices;
  bool ret = EnumDevices(&devices);
  DCHECK(ret);
  int find_device_size = static_cast<int>(devices.size());

  scoped_refptr<DevicesManager> manager = DevicesManager::Get();
  manager->DeviceListChanged(devices);

  // found something and stop
  if (stop_on_found_ && find_device_size > 0)
    return false;

  if (IsSingle())
    return false;

  return true;
}

CANScopeDeviceFinder::CANScopeDeviceFinder(
    scoped_refptr<base::SingleThreadTaskRunner> run_thread, 
    bool stop_on_found)
    : stop_on_found_(stop_on_found) {
  set_run_thread(run_thread);
}

void CANScopeDeviceFinder::NotifyStateChanged() {
  observer_list_.Notify(&Observer::StateChanged);
}

void CANScopeDeviceFinder::OnStateChanged() {
  NotifyStateChanged();
}

} // namespace canscope