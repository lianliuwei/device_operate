#include "canscope/device/config_util.h"

#include <string>

#include "base/json/json_string_value_serializer.h"
#include "base/json/json_reader.h"
#include "base/logging.h"

#include "canscope/device/canscope_device_property_constants.h"

using namespace base;

namespace canscope {

const char kDefaultCANScopeConfig [] =  {" \
{ \"OscDevice\" : \
  { \
    \"CANH.Range\" : 3, \
    \"CANH.Offset\" : 0.0, \
    \"CANH.Coupling\" : 1, \
    \"CANL.Range\" : 3, \
    \"CANL.Offset\" : 0.0, \
    \"CANL.Coupling\" : 1, \
    \"CANDIFF.Range\" : 3, \
    \"CANDIFF.Offset\" : 0.0, \
    \"DiffCtrl\" : 0, \
    \"Time.Base\" : 4, \
    \"Time.Offset\" : 0.0, \
    \"Trigger.AutoTime\" : 100.0, \
    \"Trigger.Source\" : 2, \
    \"Trigger.Type\" : 0, \
    \"Trigger.Mode\" : 0, \
    \"Trigger.Sens\" : 0, \
    \"Trigger.Compare\" : 0, \
    \"Trigger.Volt\" : 0.0, \
    \"TimeParam\" : 1.0 \
  }, \
  \"FrameDevice\" : \
  { \
    \"DeviceEnable\": true, \
    \"AckEnable\": true, \
    \"SjaBtr\": 5184, \
    \"FrameStoragePercent\": 50.0, \
    \"BitSampleRate\": 5000, \
    \"BitNum\": 180 \
  } \
} \
"};

DictionaryValue* GetConfig(const char* config) {
  std::string content(config);
  JSONStringValueSerializer serializer(content);
  int error;
  std::string error_msg;
  Value* value = serializer.Deserialize(&error, &error_msg);
  CHECK(NULL != value);
  CHECK(value->IsType(Value::TYPE_DICTIONARY));
  DictionaryValue* dict_value;
  value->GetAsDictionary(&dict_value);

  return dict_value;
}

base::DictionaryValue* GetSubDict(base::Value* value, std::string key) {
  if (!value) {
    return NULL;
  }
  if (!value->IsType(Value::TYPE_DICTIONARY)) {
    return NULL;
  }
  DictionaryValue* dict_value;
  value->GetAsDictionary(&dict_value);

  DictionaryValue* sub_dict;
  bool ret = dict_value->GetDictionary(key, &sub_dict);
  if (!ret) {
    return NULL;
  }
  return sub_dict;
}

base::DictionaryValue* GetDefaultOscDeviceConfig() {
  return GetSubDict(GetConfig(kDefaultCANScopeConfig), kOscDevice);
}

base::DictionaryValue* GetDefaultFrameDeviceConfig() {
  return GetSubDict(GetConfig(kDefaultCANScopeConfig), kFrameDevice);
}

} // namespace canscope