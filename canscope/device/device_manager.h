#pragma once

#include "base/memory/ref_counted.h"
#include "base/synchronization/lock.h"

template<typename T>
struct DeviceManagerTraits {
  static void Destruct(const T* x) {
    const_cast<T*>(x)->DeleteDevice();
  }
};

// call StartDestroying()
// 1. DeviceBase notify the DeviceHandle
// 2. Handle Drop DeviceBase ref.
// 3. ref call struct DeviceManagerTraits<DeviceBase>::Destruct()
// 4. DeleteDevice() be called.
// 5. DeleteDevice PostTask To Delete DeviceBase
class DeviceManager : public base::RefCountedThreadSafe<
    DeviceManager, DeviceManagerTraits> {
public:
  DeviceManager(MessageLoopProxy* device_loop);
  ~DeviceManager() {}

  bool IsDestroying();

  // call on any thread
  void StartDestroying();
protected:
  // call from DefaultRefCountedThreadSafeTraits, and Post Task
  // to delete device on Device
  void DeleteDevice();
  
  // impl for Notify Handle to Drop RefCount
  // may be call on any thread
  virtual void DestroyImpl() = 0;

  // Destroy this object on device thread
  virtual void DeleteDeviceImpl() = 0;

private:
  base::Lock lock_;

  bool destroy_;

  scoped_refptr<DeviceManager> own_ref_;
  scoped_refptr<MessageLoopProxy> device_loop_;

  friend struct DeviceManagerTraits<T>;
};
