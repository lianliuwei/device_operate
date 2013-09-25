#pragma once

#include "common/notification/notification_types.h"

namespace canscope {

enum CANScopeNotificationType {
  // start from content end
  CANSCOPE_NOTIFICATION_START = common::NOTIFICATION_CONTENT_END,
  
  // device manager notification, source is device manager ptr, details is NULL
  NOTIFICATION_DEVICE_MANAGER_CREATED,

  NOTIFICATION_DEVICE_MANAGER_START_DESTROY,

  NOTIFICATION_DEVICE_MANAGER_DESTROYED,

  // device handle notification
  NOTIFICATION_DEVICE_MANAGER_HANDLE_CREATED,

  NOTIFICATION_DEVICE_PREFS_UPDATE,
};

}
