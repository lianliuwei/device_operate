#include "canscope/device/canscope_device_manager.h"

#include <string>

#include "base/values.h"

#include "common/notification/notification_service.h"
#include "common/common_thread.h"

#include "canscope/canscope_notification_types.h"
#include "canscope/device/canscope_device_property_constants.h"

using namespace common;
using namespace std;
using namespace base;

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

void CANScopeDeviceManager::Init(base::Value* value) {
  DCHECK(value);
  DCHECK(value->IsType(Value::TYPE_DICTIONARY));
  DictionaryValue* dict_value;
  value->GetAsDictionary(&dict_value);
  string key(kOscDevice);
  DCHECK(dict_value->HasKey(key));
  
  DictionaryValue* osc_device_value;
  bool ret = dict_value->GetDictionary(key, &osc_device_value);
  DCHECK(ret);
  osc_device_config_.Update(osc_device_value);
  osc_device_.InitFromConfig();
  osc_device_.Init();
}

void CANScopeDeviceManager::LoadConfig(base::Value* value) {
  DCHECK(value);
  DCHECK(value->IsType(Value::TYPE_DICTIONARY));
  DictionaryValue* dict_value;
  value->GetAsDictionary(&dict_value);
  string key(kOscDevice);
  DCHECK(dict_value->HasKey(key));

  DictionaryValue* osc_device_value;
  bool ret = dict_value->GetDictionary(key, &osc_device_value);
  DCHECK(ret);
  osc_device_config_.Update(osc_device_value);
  osc_device_.LoadFromConfig();
}

base::DictionaryValue* CANScopeDeviceManager::SaveConfig() {
  scoped_ptr<DictionaryValue> dict_value(new DictionaryValue);
  string key(kOscDevice);
  ConfigManager::Config config = osc_device_config_.GetLast();
  dict_value->Set(key, config.pref->DeepCopy());

  return dict_value.release();
}

}
