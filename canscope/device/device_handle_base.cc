#include "canscope/device/device_handle_base.h"

#include "common/notification/notification_source.h"
#include "device/property/value_map_device_property_store.h"

#include "canscope/canscope_notification_types.h"
#include "canscope/device/config_manager.h"

using namespace base;
using namespace common;
using namespace canscope;

DeviceHandleBase::DeviceHandleBase(DeviceBase* device)
    : device_(device)
    , current_pref_(0)
    , batch_mode_(false) {
 registrar_.Add(this, NOTIFICATION_DEVICE_PREFS_UPDATE, 
      common::Source<ConfigManager>(device->config_manager()));
}

::device::DevicePropertyStore* DeviceHandleBase::GetDevicePropertyStore() {
  return DevicePrefs();
}

void DeviceHandleBase::PostDeviceTask(const base::Closure& task) {
  device_->run_thread()->PostTask(FROM_HERE, task);
}


bool DeviceHandleBase::IsDeviceThread() {
  return device_->run_thread()->BelongsToCurrentThread();
}

void DeviceHandleBase::FetchNewPref() {
  ConfigManager* config_manager = device_->config_manager();
  DCHECK(config_manager);
  ConfigManager::Config config = config_manager->GetLast();
  if (config.id == current_pref_)
    return;
  current_pref_ = config.id;
  DCHECK(config.pref->IsType(Value::TYPE_DICTIONARY));
  DictionaryValue* dict_value;
  config.pref->GetAsDictionary(&dict_value);
  DevicePrefs()->ChangeContent(dict_value->DeepCopy());
}

void DeviceHandleBase::SetPropertyFinish(const std::string& reason) {
  device_->UpdateConfig(reason);
}

void DeviceHandleBase::InitHandle() {
  ConfigManager* config_manager = device_->config_manager();
  DCHECK(config_manager);
  ConfigManager::Config config = config_manager->GetLast();
  current_pref_ = config.id;
  DCHECK(config.pref->IsType(Value::TYPE_DICTIONARY));
  DictionaryValue* dict_value;
  config.pref->GetAsDictionary(&dict_value);
  DevicePrefs()->Reset(dict_value->DeepCopy());
}

void DeviceHandleBase::Observe(int type, 
                               const common::NotificationSource& source, 
                               const common::NotificationDetails& details) {
  if (type == NOTIFICATION_DEVICE_PREFS_UPDATE) {
    FetchNewPref();
    return;
  }
  NOTREACHED();
}

scoped_refptr<SingleThreadTaskRunner> DeviceHandleBase::DeviceTaskRunner() {
  return device_->run_thread();
}
