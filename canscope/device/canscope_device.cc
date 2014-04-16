#include "canscope/device/canscope_device.h"

#include <string>

#include "base/values.h"

#include "common/notification/notification_service.h"

#include "canscope/canscope_notification_types.h"
#include "canscope/device/canscope_device_property_constants.h"

using namespace common;
using namespace std;
using namespace base;

namespace {

base::DictionaryValue* GetSubDict(base::Value* value, string key) {
  if (!value) {
    return NULL;
  }
  if (!value->IsType(Value::TYPE_DICTIONARY)) {
    return NULL;
  }
  DictionaryValue* dict_value;
  value->GetAsDictionary(&dict_value);

  DictionaryValue* sub_dict;
  bool ret = dict_value->GetDictionary(key, &sub_dict);
  if (!ret) {
    return NULL;
  }
  return sub_dict;
}

}

namespace canscope {

CANScopeDevice* CANScopeDevice::Create(
    scoped_refptr<base::SingleThreadTaskRunner> runner) {
  DCHECK(runner->BelongsToCurrentThread());
  CANScopeDevice* device = new CANScopeDevice(runner);
  NotifyAll(NOTIFICATION_DEVICE_MANAGER_CREATED, 
      Source<CANScopeDevice>(device), 
      NotificationService::NoDetails());
  return device;
}

void CANScopeDevice::DestroyImpl() {
  NotifyAll(NOTIFICATION_DEVICE_MANAGER_START_DESTROY, 
      Source<CANScopeDevice>(this), 
      NotificationService::NoDetails());
}

void CANScopeDevice::DeleteDeviceImpl() {
  DCHECK(run_thread()->BelongsToCurrentThread());
  NotifyAll(NOTIFICATION_DEVICE_MANAGER_DESTROYED, 
      Source<CANScopeDevice>(this), 
      NotificationService::NoDetails());

  delete this;
}

CANScopeDevice::CANScopeDevice(
    scoped_refptr<base::SingleThreadTaskRunner> device_loop)
    : DeviceManager(device_loop)
    , device_delegate_(CreateDeviceDelegate())
    , osc_device_config_()
    , osc_device_(device_delegate_.get(), &osc_device_config_)
    , frame_device_config_()
    , frame_device_(device_delegate_.get(), &frame_device_config_, osc_device_.soft_diff())
    , runner_(this) {
  osc_device_.set_run_thread(run_thread());
  frame_device_.set_run_thread(run_thread());
}

void CANScopeDevice::Init(base::Value* value) {
  // osc device
  DictionaryValue* osc_device_value = GetSubDict(value, kOscDevice);
  DCHECK(osc_device_value);
  osc_device_config_.Update(osc_device_value);
  osc_device_.InitFromConfig();
  osc_device_.Init();

  // frame device
  DictionaryValue* frame_device_value = GetSubDict(value, kFrameDevice);
  DCHECK(frame_device_value);
  frame_device_config_.Update(frame_device_value);
  frame_device_.InitFromConfig();
  frame_device_.Init();
}

void CANScopeDevice::LoadConfig(base::Value* value) {
  // osc device
  DictionaryValue* osc_device_value = GetSubDict(value, kOscDevice);
  DCHECK(osc_device_value);
  osc_device_config_.Update(osc_device_value);
  osc_device_.LoadFromConfig();

  // frame device
  DictionaryValue* frame_device_value = GetSubDict(value, kFrameDevice);
  DCHECK(frame_device_value);
  frame_device_config_.Update(frame_device_value);
  frame_device_.InitFromConfig();
}

base::DictionaryValue* CANScopeDevice::SaveConfig() {
  scoped_ptr<DictionaryValue> dict_value(new DictionaryValue);
  {
  ConfigManager::Config config = osc_device_config_.GetLast();
  dict_value->Set(kOscDevice, config.pref->DeepCopy());
  }
  {
  ConfigManager::Config config = frame_device_config_.GetLast();
  dict_value->Set(kFrameDevice, config.pref->DeepCopy());
  }
  
  return dict_value.release();
}

void CANScopeDevice::DeviceTypeDetected(DeviceType type) {
  osc_device_.set_device_type(type);
  frame_device_.set_device_type(type);
}


}
