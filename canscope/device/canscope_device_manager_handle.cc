#include "canscope/device/canscope_device_manager_handle.h"

#include "base/lazy_instance.h"
#include "base/threading/thread_local.h"
#include "base/message_loop/message_loop_proxy.h"

#include "common/notification/notification_source.h"

#include "canscope/canscope_notification_types.h"

namespace {
base::LazyInstance<
  base::ThreadLocalPointer<canscope::CANScopeDeviceManagerHandle> > 
    handle_tls_ptr = LAZY_INSTANCE_INITIALIZER;
}

namespace canscope {

// static
CANScopeDeviceManagerHandle* CANScopeDeviceManagerHandle::GetInstance() {
  return handle_tls_ptr.Pointer()->Get();
}

// static
CANScopeDeviceManagerHandle* CANScopeDeviceManagerHandle::Create() {
  DCHECK(base::MessageLoopProxy::current()) 
      << "handle need create in thread with message loop";
  DCHECK(handle_tls_ptr.Pointer()->Get() == NULL) 
      << "one thread at most has one DeviceManagerHandle";
  DCHECK(CANScopeDeviceManager::GetInstance()) 
      << "CANScopeDeviceManager need be existed";
  handle_tls_ptr.Pointer()->Set(
      new CANScopeDeviceManagerHandle(CANScopeDeviceManager::GetInstance()));
  return GetInstance();
}

void CANScopeDeviceManagerHandle::DestroyHandle() {
  device_manager_ = NULL;
  handle_tls_ptr.Pointer()->Set(NULL);
  delete this;
}

CANScopeDeviceManagerHandle::CANScopeDeviceManagerHandle(
    CANScopeDeviceManager* manager)
    : device_manager_(manager) {
  DCHECK(manager);
  registrar_.Add(this, NOTIFICATION_DEVICE_MANAGER_START_DESTROY, 
      common::Source<CANScopeDeviceManager>(manager));
}

void CANScopeDeviceManagerHandle::Observe(int type, 
                                          const common::NotificationSource& source, 
                                          const common::NotificationDetails& details) {
  DestroyHandle();
}

}
