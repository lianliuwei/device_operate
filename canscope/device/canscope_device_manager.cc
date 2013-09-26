#include "canscope/device/canscope_device_manager.h"

#include "common/notification/notification_service.h"
#include "common/common_thread.h"

#include "canscope/canscope_notification_types.h"

using namespace common;

namespace {
static canscope::CANScopeDeviceManager* g_DeviceManager = NULL;
}
namespace canscope {

CANScopeDeviceManager* CANScopeDeviceManager::GetInstance() {
  return g_DeviceManager;
}

CANScopeDeviceManager* CANScopeDeviceManager::Create() {
  DCHECK(CommonThread::CurrentlyOn(CommonThread::DEVICE));
  CHECK(g_DeviceManager == NULL);
  g_DeviceManager = new CANScopeDeviceManager(
      CommonThread::GetMessageLoopProxyForThread(CommonThread::DEVICE));
  NotifyAll(NOTIFICATION_DEVICE_MANAGER_CREATED, 
      Source<CANScopeDeviceManager>(g_DeviceManager), 
      NotificationService::NoDetails());
  return g_DeviceManager;
}

void CANScopeDeviceManager::DestroyImpl() {
  NotifyAll(NOTIFICATION_DEVICE_MANAGER_START_DESTROY, 
      Source<CANScopeDeviceManager>(this), 
      NotificationService::NoDetails());
}

void CANScopeDeviceManager::DeleteDeviceImpl() {
  DCHECK(CommonThread::CurrentlyOn(CommonThread::DEVICE));
  NotifyAll(NOTIFICATION_DEVICE_MANAGER_DESTROYED, 
      Source<CANScopeDeviceManager>(this), 
      NotificationService::NoDetails());

  DCHECK(g_DeviceManager);
  g_DeviceManager = NULL;

  delete this;
}

CANScopeDeviceManager::CANScopeDeviceManager(
    base::MessageLoopProxy* device_loop)
    : DeviceManager(device_loop)
    , osc_device_(&device_delegate_, &osc_device_config_) {}

}
