#pragma once

#include <set>

#include "base/debug/trace_event.h"
#include "base/prefs/pref_value_map.h"

#include "device/property/device_property_store.h"
#include "device/property/property_notifiter.h"

namespace device {
class ValueMapDevicePropertyStore : public DevicePropertyStore {
public:
  ValueMapDevicePropertyStore();
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

  // init from value_owned
  // no FireObserver
  void Reset(DictionaryValue* value_owned);
  DictionaryValue* Serialize();

  // value_owned item must be in store, no allow add item
  // for store revert to old state.
  void ChangeContent(DictionaryValue* value_owned);

  // can only be access on the thread call AttachThread()
  void DetachFromThread();

private:
  void SetValueInner(const std::string& key, base::Value* value);

  PropertyNotifiter notifier_;
  PrefValueMap prefs_;

  base::debug::TraceScopedTrackableObject<ValueMapDevicePropertyStore*> 
      trace_object_;

  DISALLOW_COPY_AND_ASSIGN(ValueMapDevicePropertyStore);
};

} // namespace device
