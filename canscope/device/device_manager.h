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
  DeviceManager(scoped_refptr<base::SingleThreadTaskRunner> device_loop);

  bool IsDestroying();

  // call on any thread
  void StartDestroying();

  scoped_refptr<base::SingleThreadTaskRunner> run_thread();

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

protected:
  friend class base::RefCountedThreadSafe<DeviceManager>;
  virtual ~DeviceManager() {}

private:
  void DestroyPos();

  base::Lock lock_;

  bool destroy_;

  scoped_refptr<DeviceManager> own_ref_;
  scoped_refptr<base::SingleThreadTaskRunner> runner_;

  friend struct DeviceManagerTraits<DeviceManager>;
};
