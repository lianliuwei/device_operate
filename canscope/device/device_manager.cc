#include "canscope/device/device_manager.h"


DeviceManager::DeviceManager(MessageLoopProxy* device_loop)
    : destroy_(false) 
    , own_ref_(this)
    , device_loop_(device_loop) {
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
  device_loop_->PostTask(FROM_HERE, 
      Bind(&DeviceManager::DestroyPos, this));
}

void DeviceManager::DestroyPos() {
  DestroyImpl();
  // this may cause self destroy, so call it last
  own_ref_ = NULL;
}

void DeviceManager::DeleteDevice() {
  device_loop_->PostTask(FROM_HERE, 
      Bind(&DeviceManager::DeleteDeviceImpl, this));
}