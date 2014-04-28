#pragma once

#include "base/values.h"
#include "base/threading/non_thread_safe.h"

namespace device {

class DevicePropertyStore : public base::NonThreadSafe {
public:
  class Observer {
  public:
    virtual void OnPreferenceChanged(const std::string& pref_name) = 0;

  protected:
    Observer() {}
    virtual ~Observer() {}
  };

  virtual void AddPrefObserver(const std::string& key, Observer* obs) = 0;
  virtual void RemovePrefObserver(const std::string& key, Observer* obs) = 0;
  virtual bool HasObserver(const std::string& key, Observer* obs) const = 0;

  virtual bool GetValue(const std::string& key,
                        const base::Value** result) const = 0;

  // Equivalent to GetValue but returns a mutable value.
  virtual bool GetMutableValue(const std::string& key,
                               base::Value** result) = 0;

  // Has path key
  virtual bool HasPrefPath(const std::string& key) const = 0;

  // Triggers a value changed notification. This function needs to be called
  // if one retrieves a list or dictionary with GetMutableValue and change its
  // value. SetValue takes care of notifications itself. Note that
  // ReportValueChanged will trigger notifications even if nothing has changed.
  virtual void ReportValueChanged(const std::string& key) = 0;

  // Sets a |value| for |key| in the store. Assumes ownership of |value|, which
  // must be non-NULL.
  virtual void SetValue(const std::string& key, base::Value* value) = 0;

  // Same as SetValue, but doesn't generate notifications. This is used by
  // PrefService::GetMutableUserPref() in order to put empty entries
  // into the user pref store. Using SetValue is not an option since existing
  // tests rely on the number of notifications generated.
  virtual void SetValueSilently(const std::string& key, base::Value* value) = 0;

  // Removes the value for |key|.
  virtual void RemoveValue(const std::string& key) = 0;

protected:
  DevicePropertyStore() {}
  virtual ~DevicePropertyStore() {}
};

} // namespace device
