#include "canscope/device/canscope_device_handle.h"

#include <map>

#include "base/lazy_instance.h"
#include "base/threading/thread_local.h"
#include "base/message_loop/message_loop_proxy.h"

#include "common/notification/notification_source.h"

#include "canscope/canscope_notification_types.h"

using namespace std;
using namespace canscope;

namespace {

typedef map<CANScopeDevice*, CANScopeDeviceHandle*> HandleMap;

base::LazyInstance<
  base::ThreadLocalPointer<HandleMap> > 
    handle_tls_ptr = LAZY_INSTANCE_INITIALIZER;
}

namespace canscope {

// static
CANScopeDeviceHandle* CANScopeDeviceHandle::GetInstance(
    CANScopeDevice* manager) {
  HandleMap* handle_map = handle_tls_ptr.Pointer()->Get();
  HandleMap::iterator it = handle_map->find(manager);

  return it == handle_map->end() ? NULL : it->second;
}

// static
CANScopeDeviceHandle* CANScopeDeviceHandle::Create(
    CANScopeDevice* manager) {
  DCHECK(manager);
  HandleMap* handle_map = handle_tls_ptr.Pointer()->Get();
  if (!handle_map) {
    handle_map = new HandleMap;
    handle_tls_ptr.Pointer()->Set(handle_map);
  }
  HandleMap::iterator it = handle_map->find(manager);
  DCHECK(it == handle_map->end()) 
      << "one thread at most has one DeviceManagerHandle per DeviceManager";
  CANScopeDeviceHandle* handle = new CANScopeDeviceHandle(manager);
  handle_map->insert(make_pair(manager, handle));
  
  return handle;
}

void CANScopeDeviceHandle::DestroyHandle() { 
  HandleMap* handle_map = handle_tls_ptr.Pointer()->Get();
  HandleMap::iterator it = handle_map->find(device_manager_.get());
  DCHECK(it != handle_map->end()) << "Handle no register in handle map";

  // delete first and then erase and then check handle map
  delete it->second;

  handle_map->erase(it);

  if (handle_map->size() == 0) {
    delete handle_map;
    handle_tls_ptr.Pointer()->Set(NULL);
  }
}

CANScopeDeviceHandle::CANScopeDeviceHandle(CANScopeDevice* manager)
    : device_manager_(manager)
    , osc_device_handle(manager->osc_device())
    , frame_device_handle(manager->frame_device()) {
  DCHECK(manager);
  registrar_.Add(this, NOTIFICATION_DEVICE_MANAGER_START_DESTROY, 
      common::Source<CANScopeDevice>(manager));
}

void CANScopeDeviceHandle::Observe(int type, 
                                   const common::NotificationSource& source, 
                                   const common::NotificationDetails& details) {
  DestroyHandle();
}


CANScopeDeviceHandle::~CANScopeDeviceHandle() {
  device_manager_ = NULL;
}

scoped_refptr<OscDataCollecter> CANScopeDeviceHandle::GetOscDataCollecter() {
  return device_manager_->runner()->osc_data;
}

}
