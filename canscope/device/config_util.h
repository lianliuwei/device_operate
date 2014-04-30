#pragma once

#include <string>

#include "base/values.h"

namespace canscope {

extern const char kDefaultCANScopeConfig[];

base::DictionaryValue* GetConfig(const char* config);
base::DictionaryValue* GetSubDict(base::Value* value, std::string key);

base::DictionaryValue* GetDefaultOscDeviceConfig();
base::DictionaryValue* GetDefaultFrameDeviceConfig();
} // namespace canscope