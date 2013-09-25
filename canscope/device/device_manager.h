#pragma once

#include "base/memory/ref_counted.h"
#include "base/synchronization/lock.h"
#include "base/message_loop/message_loop_proxy.h"

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
    DeviceManager, DeviceManagerTraits<DeviceManager> > {
public:
  DeviceManager(base::MessageLoopProxy* device_loop);
  virtual ~DeviceManager() {}

  bool IsDestroying();

  // call on any thread
  void StartDestroying();
protected:
  // call from DefaultRefCountedThreadSafeTraits, and Post Task
  // to delete device on Device
  void DeleteDevice();
  
  // impl for Notify Handle to Drop RefCount
  // call on device_loop_ thread.
  virtual void DestroyImpl() = 0;

  // Destroy this object on device thread
  // call on device_loop_ thread.
  virtual void DeleteDeviceImpl() = 0;

private:
  void DestroyPos();

  base::Lock lock_;

  bool destroy_;

  scoped_refptr<DeviceManager> own_ref_;
  scoped_refptr<base::MessageLoopProxy> device_loop_;

  friend struct DeviceManagerTraits<DeviceManager>;
};
