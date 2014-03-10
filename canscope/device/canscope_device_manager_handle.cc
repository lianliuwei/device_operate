#include "canscope/device/canscope_device_manager_handle.h"

#include <map>

#include "base/lazy_instance.h"
#include "base/threading/thread_local.h"
#include "base/message_loop/message_loop_proxy.h"

#include "common/notification/notification_source.h"

#include "canscope/canscope_notification_types.h"

using namespace std;
using namespace canscope;

namespace {

typedef map<CANScopeDeviceManager*, CANScopeDeviceManagerHandle*> HandleMap;

base::LazyInstance<
  base::ThreadLocalPointer<HandleMap> > 
    handle_tls_ptr = LAZY_INSTANCE_INITIALIZER;
}

namespace canscope {

// static
CANScopeDeviceManagerHandle* CANScopeDeviceManagerHandle::GetInstance(
    CANScopeDeviceManager* manager) {
  HandleMap* handle_map = handle_tls_ptr.Pointer()->Get();
  HandleMap::iterator it = handle_map->find(manager);

  return it == handle_map->end() ? NULL : it->second;
}

// static
CANScopeDeviceManagerHandle* CANScopeDeviceManagerHandle::Create(
    CANScopeDeviceManager* manager) {
  DCHECK(manager);
  HandleMap* handle_map = handle_tls_ptr.Pointer()->Get();
  if (!handle_map) {
    handle_map = new HandleMap;
    handle_tls_ptr.Pointer()->Set(handle_map);
  }
  HandleMap::iterator it = handle_map->find(manager);
  DCHECK(it == handle_map->end()) 
      << "one thread at most has one DeviceManagerHandle per DeviceManager";
  CANScopeDeviceManagerHandle* handle = new CANScopeDeviceManagerHandle(manager);
  handle_map->insert(make_pair(manager, handle));
  
  handle->Init();
  return handle;
}

void CANScopeDeviceManagerHandle::DestroyHandle() { 
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

void CANScopeDeviceManagerHandle::Init() {
  osc_device_handle_.reset(new OscDeviceHandle(device_manager_->osc_device()));
}

CANScopeDeviceManagerHandle::~CANScopeDeviceManagerHandle() {
  device_manager_ = NULL;
}


}
