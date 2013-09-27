#include "canscope/device/device_handle_base.h"

#include "common/common_thread.h"
#include "common/notification/notification_source.h"

#include "canscope/canscope_notification_types.h"
#include "canscope/device/config_manager.h"
#include "canscope/device/property/value_map_device_property_store.h"

using namespace common;
using namespace canscope;

DeviceHandleBase::DeviceHandleBase(DeviceBase* device)
    : device_(device)
    , current_pref_(0)
    , batch_mode_(false) {
 registrar_.Add(this, NOTIFICATION_DEVICE_PREFS_UPDATE, 
      common::Source<ConfigManager>(device->config_manager()));
}

DevicePropertyStore* DeviceHandleBase::GetDevicePropertyStore() {
  return DevicePrefs();
}

void DeviceHandleBase::PostDeviceTask(const base::Closure& task) {
  CommonThread::PostTask(CommonThread::DEVICE, FROM_HERE, task);
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


#ifndef UNIT_TEST
namespace {

bool Is DeviceThread() {
  return CommonThread::CurrentlyOn(CommonThread::DEVICE);
}

}
#endif
