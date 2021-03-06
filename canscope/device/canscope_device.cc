#include "canscope/device/canscope_device.h"

#include <string>

#include "base/values.h"

#include "common/notification/notification_service.h"

#include "canscope/canscope_notification_types.h"
#include "canscope/device/canscope_device_property_constants.h"
#include "canscope/device/config_util.h"

using namespace common;
using namespace std;
using namespace base;


namespace canscope {

CANScopeDevice* CANScopeDevice::Create(
    scoped_refptr<base::SingleThreadTaskRunner> runner) {
  DCHECK(runner->BelongsToCurrentThread());
  CANScopeDevice* device = new CANScopeDevice(runner);

  return device;
}

void CANScopeDevice::DestroyImpl() {
  if (!notify_created_) {
    return;
  }
  NotifyAll(NOTIFICATION_DEVICE_MANAGER_START_DESTROY, 
      Source<CANScopeDevice>(this), 
      NotificationService::NoDetails());
}

void CANScopeDevice::DeleteDeviceImpl() {
  DCHECK(run_thread()->BelongsToCurrentThread());
  if (notify_created_) {
    NotifyAll(NOTIFICATION_DEVICE_MANAGER_DESTROYED, 
        Source<CANScopeDevice>(this), 
        NotificationService::NoDetails());
  }
  delete this;
}

void CANScopeDevice::NotifyCreated() {
  DCHECK(!notify_created_);
  notify_created_ = true;
  NotifyAll(NOTIFICATION_DEVICE_MANAGER_CREATED, 
      Source<CANScopeDevice>(this), 
      NotificationService::NoDetails());
}

CANScopeDevice::CANScopeDevice(
    scoped_refptr<base::SingleThreadTaskRunner> device_loop)
    : DeviceManager(device_loop)
    , device_delegate_(CreateDeviceDelegate())
    , osc_device_config_()
    , osc_device_(device_delegate_.get(), &osc_device_config_)
    , frame_device_config_()
    , frame_device_(device_delegate_.get(), &frame_device_config_, osc_device_.soft_diff())
    , runner_(this)
    , notify_created_(false) {
  osc_device_.set_run_thread(run_thread());
  frame_device_.set_run_thread(run_thread());
}

void CANScopeDevice::Init(base::Value* value) {
  scoped_ptr<Value> config_value(value);
  // osc device
  DictionaryValue* osc_device_value = TakeSubDict(value, kOscDevice);
  DCHECK(osc_device_value);
  osc_device_config_.Update(osc_device_value);
  osc_device_.InitFromConfig();
  osc_device_.Init();

  // frame device
  DictionaryValue* frame_device_value = TakeSubDict(value, kFrameDevice);
  DCHECK(frame_device_value);
  frame_device_config_.Update(frame_device_value);
  frame_device_.InitFromConfig();
  frame_device_.Init();
}

void CANScopeDevice::LoadConfig(base::Value* value) {
  scoped_ptr<Value> config_value(value);
  // osc device
  DictionaryValue* osc_device_value = TakeSubDict(value, kOscDevice);
  DCHECK(osc_device_value);
  osc_device_config_.Update(osc_device_value);
  osc_device_.LoadFromConfig();

  // frame device
  DictionaryValue* frame_device_value = TakeSubDict(value, kFrameDevice);
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

// TODO need check error
void CANScopeDevice::SetAll() {
  osc_device_.SetAll();
  frame_device_.SetAll();
}


}
