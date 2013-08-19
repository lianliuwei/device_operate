#include "canscope/device/property/value_map_device_property_store.h"

namespace canscope {



void ValueMapDevicePropertyStore::AddPrefObserver(const std::string& key, 
                                                  Observer* obs) {
  notifier_.AddPrefObserver(key, obs);
}

void ValueMapDevicePropertyStore::RemovePrefObserver(const std::string& key, 
                                                     Observer* obs) {
  notifier_.RemovePrefObserver(key, obs);
}

bool ValueMapDevicePropertyStore::HasObserver(const std::string& key, 
                                              Observer* obs) const {
  return notifier_.HasObserver(key, obs);
}

bool ValueMapDevicePropertyStore::GetValue(const std::string& key, 
                                           const base::Value** result) const {
  return prefs_.GetValue(key, result);
}

bool ValueMapDevicePropertyStore::GetMutableValue(const std::string& key, 
                                                  base::Value** result) {
  return prefs_.GetValue(key, result);
}

bool ValueMapDevicePropertyStore::HasPrefPath(const std::string& key) const {
  return GetValue(key, NULL);
}

void ValueMapDevicePropertyStore::ReportValueChanged(const std::string& key) {
  return notifier_.FireObservers(key);
}

void ValueMapDevicePropertyStore::SetValue(const std::string& key, 
                                           base::Value* value) {
 DCHECK(value);
  scoped_ptr<Value> new_value(value);
  Value* old_value = NULL;
  prefs_.GetValue(key, &old_value);
  if (!old_value || !value->Equals(old_value)) {
    prefs_.SetValue(key, new_value.release());
    ReportValueChanged(key);
  }
}

void ValueMapDevicePropertyStore::SetValueSilently(const std::string& key, 
                                                   base::Value* value) {
DCHECK(value);
  scoped_ptr<Value> new_value(value);
  Value* old_value = NULL;
  prefs_.GetValue(key, &old_value);
  if (!old_value || !value->Equals(old_value)) {
    prefs_.SetValue(key, new_value.release());
  }
}

void ValueMapDevicePropertyStore::RemoveValue(const std::string& key) {
  if (prefs_.RemoveValue(key))
    ReportValueChanged(key);
}

void ValueMapDevicePropertyStore::MarkNeedsEmptyValue(const std::string& key) {
  keys_need_empty_value_.insert(key);
}


} // namespace
