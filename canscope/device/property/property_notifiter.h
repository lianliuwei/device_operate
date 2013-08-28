#pragma once

#include <list>
#include <string>

#include "base/callback.h"
#include "base/compiler_specific.h"
#include "base/hash_tables.h"
#include "base/observer_list.h"
#include "base/threading/thread_checker.h"

#include "canscope/device/property/device_property_store.h"

class PropertyNotifiter {
 public:
  typedef canscope::DevicePropertyStore::Observer PrefObserver;

  PropertyNotifiter();
  virtual ~PropertyNotifiter();

  // If the pref at the given path changes, we call the observer's
  // OnPreferenceChanged method.
  void AddPrefObserver(const std::string& key, PrefObserver* observer);
  void RemovePrefObserver(const std::string& key, PrefObserver* observer);
  bool HasObserver(const std::string& key, PrefObserver* obs) const;

  // For the given pref_name, fire any observer of the pref. Virtual so it can
  // be mocked for unit testing.
  void FireObservers(const std::string& path);

 protected:
  // A map from pref names to a list of observers. Observers get fired in the
  // order they are added. These should only be accessed externally for unit
  // testing.
  typedef ObserverList<PrefObserver> PrefObserverList;
  typedef base::hash_map<std::string, PrefObserverList*> PrefObserverMap;

  const PrefObserverMap* pref_observers() const { return &pref_observers_; }

 private:
  PrefObserverMap pref_observers_;

  base::ThreadChecker thread_checker_;

  DISALLOW_COPY_AND_ASSIGN(PropertyNotifiter);
};
