#pragma once

#include "base/values.h"
#include "base/synchronization/lock.h"

namespace {
class ConfigManagerServices;
}
template <class T>
void STLDeleteValues(T* container);

// call on any thread
class ConfigManager {
public:
  struct Config {
    int id;
    // ConfigManager keep ownership
    base::Value* pref;
  };

  static ConfigManager* Get(const std::string& name);
  
  // must call after first pref update
  Config GetLast();

  // take value ownership
  void Update(base::Value* value);

  void UpdateAndNotify(base::Value* value);

private:
  friend class ConfigManagerServices;
  template <class T>
  friend void STLDeleteValues(T* container);
  ConfigManager();
  ~ConfigManager();

  base::Lock lock_;

  // current id.
  int current_id_;

  typedef std::vector<Config> ConfigHistory;
  ConfigHistory config_history_;

  DISALLOW_COPY_AND_ASSIGN(ConfigManager);
};
