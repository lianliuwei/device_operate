#include "canscope/device/device_manager.h"

#include "base/bind.h"
#include "base/location.h"

using namespace base;

DeviceManager::DeviceManager(scoped_refptr<base::SingleThreadTaskRunner> device_loop)
    : destroy_(false) 
    , own_ref_(this)
    , runner_(device_loop) {
  DCHECK(device_loop);
}

bool DeviceManager::IsDestroying() {
  AutoLock lock(lock_);
  return destroy_ == true;
}

void DeviceManager::StartDestroying() {
  {
  AutoLock lock(lock_);
  if (destroy_)
    return;
  destroy_ = true;
  }
  runner_->PostTask(FROM_HERE, 
      Bind(&DeviceManager::DestroyPos, this));
}

void DeviceManager::DestroyPos() {
  DestroyImpl();
  // this may cause self destroy, so call it last
  own_ref_ = NULL;
}

void DeviceManager::DeleteDevice() {
  // use the raw ptr, or will break RefCount in_dtor CHECK
  runner_->PostTask(FROM_HERE, 
      Bind(&DeviceManager::DeleteDeviceImpl, Unretained(this)));
}

scoped_refptr<base::SingleThreadTaskRunner> DeviceManager::run_thread() {
  return runner_;
}
