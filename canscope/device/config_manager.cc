#include "canscope/device/config_manager.h"

#include "base/memory/singleton.h"
#include "base/stl_util.h"

#include "common/notification/notification_service.h"

#include "canscope/canscope_notification_types.h"

using namespace base;
using namespace common;


ConfigManager::Config ConfigManager::GetLast() {
  AutoLock lock(lock_);
  DCHECK(config_history_.size()) << "init config first";
  return config_history_.back();
}

void ConfigManager::Update(base::Value* value) {
  AutoLock lock(lock_);
  Config config = {current_id_, value};
  config_history_.push_back(config);
  ++current_id_;
}

void ConfigManager::UpdateAndNotify(base::Value* value) {
  Update(value);
  NotifyAll(canscope::NOTIFICATION_DEVICE_PREFS_UPDATE, 
      Source<ConfigManager>(this), 
      NotificationService::NoDetails());
}

ConfigManager::ConfigManager()
    : current_id_(0) {}

ConfigManager::~ConfigManager() {
  ConfigHistory::iterator begin = config_history_.begin();
  ConfigHistory::iterator end = config_history_.end();
  while (begin != end) {
    delete begin->pref;
    ++begin;
  }
  config_history_.clear();
}
