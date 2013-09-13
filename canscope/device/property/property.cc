#include "canscope/device/property/property.h"

namespace canscope {

base::DictionaryValue* ErrorAsDictionary(device::Error error) {
  base::DictionaryValue* value_ptr = new base::DictionaryValue();
  value_ptr->SetInteger("error", error);
  return value_ptr;
}

canscope::device::Error ErrorFromDictonary(base::Value* value) {
  DCHECK(value);
  DCHECK(value->IsType(base::Value::TYPE_DICTIONARY));
  base::DictionaryValue* value_ptr;
  bool ret = value->GetAsDictionary(&value_ptr);
  DCHECK(ret);
  int error;
  value_ptr->GetInteger("error", &error);
  return static_cast<canscope::device::Error>(error);
}

} // namespace canscope
