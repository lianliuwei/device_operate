#pragma once

#include "common/notification/notification_types.h"

namespace canscope {

enum CANScopeNotificationType {
  // start from content end
  CANSCOPE_NOTIFICATION_START = common::NOTIFICATION_CONTENT_END,
  
  // device manager notification, source is device manager ptr, details is NULL
  NOTIFICATION_DEVICE_MANAGER_CREATED,

  // ditto
  NOTIFICATION_DEVICE_MANAGER_START_DESTROY,

  // ditto
  NOTIFICATION_DEVICE_MANAGER_DESTROYED,

  // device handle notification
  NOTIFICATION_DEVICE_MANAGER_HANDLE_CREATED,

  // source is config manager ptr, details is NULL
  NOTIFICATION_DEVICE_PREFS_UPDATE,

  // source is DeviceBase ptr, details is NULL
  NOTIFICATION_DEVICE_LOCK_STATUS_CHANGED,
};

}
