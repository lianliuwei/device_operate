#pragma once

#include "base/values.h"

// parse config, fault will EXPECT_TRUE
base::DictionaryValue* GetConfig(const char* config);