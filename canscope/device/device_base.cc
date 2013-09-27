#include "canscope/device/device_base.h"

#include "base/logging.h"
#include "base/values.h"

#include "canscope/device/config_manager.h"
#include "canscope/device/property/value_map_device_property_store.h"

using namespace base;

DeviceBase::DeviceBase(ConfigManager* config_manager)
    : seq_(kNullSeq)
    , config_manager_(config_manager)
    , lock_name_("")
    // init no lock
    , event_(true, true) {

}

bool DeviceBase::Lock(int* seq, std::string lock_name) {
  {
  DCHECK(seq);
  AutoLock scoped(lock_);   
  // already be lock
  if (seq_ != kNullSeq)
    return false;
  *seq = seq_ = IncSeq();
  lock_name_ = lock_name;
  event_.Reset();
  }
  // release lock before notify, or may dead lock.
  LockStatusChanged();
  return true;
}

void DeviceBase::UnLock(int seq) {
  {
  AutoLock scoped(lock_);
  CHECK(seq == seq_);
  seq_ = kNullSeq;
  lock_name_ = "";
  event_.Signal();
  }
  LockStatusChanged();
}

bool DeviceBase::IsLocked() {
  AutoLock scoped(lock_);
  return seq_ != kNullSeq;
}

void DeviceBase::TryWaitLock() {
  event_.Wait();
}

int DeviceBase::IncSeq() {
  ++seq_;
  // pass kNullSeq
  if (seq_ == kNullSeq)
    ++seq_;
  return seq_;
}

bool DeviceBase::CheckLock(int seq) {
  AutoLock scoped(lock_);
  return seq_ == seq;
}

void DeviceBase::LoadFromConfig() {
  if (DevicePrefs() == NULL)
    return;
  if (config_manager_ == NULL)
    return;
  ConfigManager::Config config = config_manager_->GetLast();
  DCHECK(config.pref->IsType(Value::TYPE_DICTIONARY));
  DictionaryValue* dict_value;
  config.pref->GetAsDictionary(&dict_value);
  DevicePrefs()->ChangeContent(dict_value->DeepCopy());  
}

void DeviceBase::InitFromConfig() {
  if (DevicePrefs() == NULL)
    return;
  if (config_manager_ == NULL)
    return;
  ConfigManager::Config config = config_manager_->GetLast();
  DCHECK(config.pref->IsType(Value::TYPE_DICTIONARY));
  DictionaryValue* dict_value;
  config.pref->GetAsDictionary(&dict_value);
  DevicePrefs()->Reset(dict_value->DeepCopy());  
}

void DeviceBase::UpdateConfig(const std::string& reason) {
  if (DevicePrefs() == NULL)
    return;
  if (config_manager_ == NULL)
    return;
  config_manager_->UpdateAndNotify(DevicePrefs()->Serialize());
}


