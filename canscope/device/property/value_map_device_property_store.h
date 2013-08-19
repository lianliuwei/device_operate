#pragma once

#include <set>

#include "base/prefs/pref_value_map.h"

#include "canscope/device/property/device_property_store.h"
#include "canscope/device/property/property_notifiter.h"

namespace canscope {
class ValueMapDevicePropertyStore : public DevicePropertyStore {
public:
  ValueMapDevicePropertyStore() {}
  virtual ~ValueMapDevicePropertyStore() {}

  // implement DevicePropertyStore
  virtual void AddPrefObserver(const std::string& key, Observer* obs);
  virtual void RemovePrefObserver(const std::string& key, Observer* obs);
  virtual bool HasObserver(const std::string& key, Observer* obs) const;
  virtual bool GetValue(const std::string& key,
                        const base::Value** result) const;
  virtual bool GetMutableValue(const std::string& key,
                               base::Value** result);
  virtual bool HasPrefPath(const std::string& key) const;
  virtual void ReportValueChanged(const std::string& key);
  virtual void SetValue(const std::string& key, base::Value* value);
  virtual void SetValueSilently(const std::string& key, base::Value* value);
  virtual void RemoveValue(const std::string& key);
  virtual void MarkNeedsEmptyValue(const std::string& key);

private:
  PropertyNotifiter notifier_;
  PrefValueMap prefs_;

  std::set<std::string> keys_need_empty_value_;
};

} // namespace
