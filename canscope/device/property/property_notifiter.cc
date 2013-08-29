#include "canscope/device/property/property_notifiter.h"

#include "base/logging.h"
#include "base/stl_util.h"

PropertyNotifiter::PropertyNotifiter() {
}


PropertyNotifiter::~PropertyNotifiter() {
  DCHECK(thread_checker_.CalledOnValidThread());

  // Verify that there are no pref observers when we shut down.
  for (PrefObserverMap::iterator it = pref_observers_.begin();
       it != pref_observers_.end(); ++it) {
    PrefObserverList::Iterator obs_iterator(*(it->second));
    if (obs_iterator.GetNext()) {
      LOG(WARNING) << "pref observer found at shutdown " << it->first;
    }
  }

  STLDeleteContainerPairSecondPointers(pref_observers_.begin(),
                                       pref_observers_.end());
  pref_observers_.clear();
}

void PropertyNotifiter::AddPrefObserver(const std::string& key,
                                        PrefObserver* obs) {
  // Get the pref observer list associated with the path.
  PrefObserverList* observer_list = NULL;
  const PrefObserverMap::iterator observer_iterator =
      pref_observers_.find(key);
  if (observer_iterator == pref_observers_.end()) {
    observer_list = new PrefObserverList;
    pref_observers_[key] = observer_list;
  } else {
    observer_list = observer_iterator->second;
  }

  // Add the pref observer. ObserverList will DCHECK if it already is
  // in the list.
  observer_list->AddObserver(obs);
}

void PropertyNotifiter::RemovePrefObserver(const std::string& key,
                                           PrefObserver* obs) {
  DCHECK(thread_checker_.CalledOnValidThread());

  const PrefObserverMap::iterator observer_iterator =
      pref_observers_.find(key);
  if (observer_iterator == pref_observers_.end()) {
    return;
  }

  PrefObserverList* observer_list = observer_iterator->second;
  observer_list->RemoveObserver(obs);
}

bool PropertyNotifiter::HasObserver(const std::string& key, 
                                    PrefObserver* obs) const {
  DCHECK(thread_checker_.CalledOnValidThread());

  const PrefObserverMap::const_iterator observer_iterator =
      pref_observers_.find(key);
  if (observer_iterator == pref_observers_.end()) {
    return false;
  }

  PrefObserverList* observer_list = observer_iterator->second;
  return observer_list->HasObserver(obs);
}

void PropertyNotifiter::FireObservers(const std::string& path) {
  DCHECK(thread_checker_.CalledOnValidThread());

  const PrefObserverMap::iterator observer_iterator =
      pref_observers_.find(path);
  if (observer_iterator == pref_observers_.end())
    return;

  FOR_EACH_OBSERVER(PrefObserver,
                    *(observer_iterator->second),
                    OnPreferenceChanged(path));
}

void PropertyNotifiter::AttachThread() {
  thread_checker_.DetachFromThread();
}


