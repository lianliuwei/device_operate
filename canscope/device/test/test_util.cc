#include "canscope/device/test/test_util.h"

#include <string>

#include "base/json/json_string_value_serializer.h"
#include "base/json/json_reader.h"
#include "testing/gtest/include/gtest/gtest.h"

using namespace base;

DictionaryValue* GetConfig(const char* config) {
  std::string content(config);
  JSONStringValueSerializer serializer(content);
  int error;
  std::string error_msg;
  Value* value = serializer.Deserialize(&error, &error_msg);
  EXPECT_TRUE(NULL != value);
  EXPECT_TRUE(value->IsType(Value::TYPE_DICTIONARY));
  DictionaryValue* dict_value;
  value->GetAsDictionary(&dict_value);

  return dict_value;
}
