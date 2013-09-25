#pragma once

#include "base/values.h"
#include "base/synchronization/lock.h"

// call on any thread
class ConfigManager {
public:
  ConfigManager();
  ~ConfigManager();

  struct Config {
    int id;
    // ConfigManager keep ownership
    base::Value* pref;
  };

  // must call after first pref update
  Config GetLast();

  // take value ownership
  void Update(base::Value* value);

  void UpdateAndNotify(base::Value* value);

private:

  base::Lock lock_;

  // current id.
  int current_id_;

  typedef std::vector<Config> ConfigHistory;
  ConfigHistory config_history_;

  DISALLOW_COPY_AND_ASSIGN(ConfigManager);
};
