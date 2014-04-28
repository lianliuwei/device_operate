#include "device/property/value_map_device_property_store.h"

#include "base/memory/scoped_ptr.h"
#include "base/stringprintf.h"
#include "base/json/json_string_value_serializer.h"

using namespace base;

namespace {
class PropertyFormat : public base::debug::ConvertableToTraceFormat {
public:
  PropertyFormat(const char* command, const std::string& key, Value* value)
      : command_(command)
      , key_(&key)
      , pref_(NULL)
      , value_(value) {}

  PropertyFormat(const char* command, PrefValueMap* pref)
      : command_(command)
      , pref_(pref)
      , value_(NULL)
      , key_(NULL) {}

  PropertyFormat(const char* command, const std::string& key)
      : command_(command)
      , pref_(NULL)
      , value_(NULL)
      , key_(&key) {}

  virtual ~PropertyFormat() {}
  
  virtual void AppendAsTraceFormat(std::string* out) const OVERRIDE {
    out->append(buffer_);
  }

  void SnapshotToBuffer() {
    SnapshotToBufferImpl(&buffer_);
  } 

private:
  void SnapshotToBufferImpl(std::string* out) const  {
    out->append(StringPrintf("{\"command\":\"%s\",", command_));
    if (key_) {
      if (value_) {
        std::string output;
        JSONStringValueSerializer serializer(&output);
        bool ret = serializer.Serialize(*value_);
        DCHECK(ret);
        out->append(StringPrintf("\"%s\": %s", key_->c_str(), output.c_str()));
      } else {
        out->append(StringPrintf("\"%s\"", key_->c_str()));
      }
    } else if (pref_) {
      std::string output;
      PrefValueMap::const_iterator it = pref_->begin();
      out->append(StringPrintf("\"Pref\": {"));
      if (it != pref_->end()) {
        JSONStringValueSerializer serializer(&output);
        bool ret = serializer.Serialize(*it->second);
        DCHECK(ret);
        out->append(StringPrintf("\"%s\": %s", it->first.c_str(), output.c_str()));
        ++it;
      }
      for (; it != pref_->end(); ++it) {
        JSONStringValueSerializer serializer(&output);
        bool ret = serializer.Serialize(*it->second);
        DCHECK(ret);
        out->append(StringPrintf(", \"%s\": %s", it->first.c_str(), output.c_str()));
      }
      out->append(StringPrintf("}"));
    } else {
      NOTREACHED();
    }
    out->append("}");
  }

  const char* command_;
  const std::string* key_;
  Value* value_;
  PrefValueMap* pref_;
  std::string buffer_;
  DISALLOW_COPY_AND_ASSIGN(PropertyFormat);
};

}

namespace base {
namespace debug {
template<> 
template<>
void TraceScopedTrackableObject<device::ValueMapDevicePropertyStore*>::
    snapshot<scoped_ptr<PropertyFormat>* >(scoped_ptr<PropertyFormat>* snapshot) {
  bool ret = false;
  TRACE_EVENT_CATEGORY_GROUP_ENABLED("PropertyStore", &ret);
  // buffer the snapshot, because AppendAsTraceFormat() be called when
  // TraceLog Flush() be call, the pref and value is no exist by then.
  if (ret) {
    (*snapshot)->SnapshotToBuffer();
  }
  TRACE_EVENT_OBJECT_SNAPSHOT_WITH_ID(category_group_, name_, id_, 
      snapshot->PassAs<base::debug::ConvertableToTraceFormat>()); 
}

}
}

namespace device {

ValueMapDevicePropertyStore::ValueMapDevicePropertyStore()
    : trace_object_("PropertyStore", "ValueMapDevicePropertyStore", this) {

}

void ValueMapDevicePropertyStore::AddPrefObserver(const std::string& key, 
                                                  Observer* obs) {
  notifier_.AddPrefObserver(key, obs);
}

void ValueMapDevicePropertyStore::RemovePrefObserver(const std::string& key, 
                                                     Observer* obs) {
  notifier_.RemovePrefObserver(key, obs);
}

bool ValueMapDevicePropertyStore::HasObserver(const std::string& key, 
                                              Observer* obs) const {
  return notifier_.HasObserver(key, obs);
}

bool ValueMapDevicePropertyStore::GetValue(const std::string& key, 
                                           const base::Value** result) const {
  return prefs_.GetValue(key, result);
}

bool ValueMapDevicePropertyStore::GetMutableValue(const std::string& key, 
                                                  base::Value** result) {
  return prefs_.GetValue(key, result);
}

bool ValueMapDevicePropertyStore::HasPrefPath(const std::string& key) const {
  return GetValue(key, NULL);
}

void ValueMapDevicePropertyStore::ReportValueChanged(const std::string& key) {
  return notifier_.FireObservers(key);
}

void ValueMapDevicePropertyStore::SetValue(const std::string& key, 
                                           base::Value* value) {
  DCHECK(value);
  
  scoped_ptr<Value> new_value(value);
  Value* old_value = NULL;
  prefs_.GetValue(key, &old_value);
  if (!old_value || !value->Equals(old_value)) {
    prefs_.SetValue(key, new_value.release());
    scoped_ptr<PropertyFormat> format(
        new PropertyFormat("SetValue", key, value));
    trace_object_.snapshot(&format);
    ReportValueChanged(key);
  }
}

void ValueMapDevicePropertyStore::SetValueInner(const std::string& key, 
                                                base::Value* value) {
  DCHECK(value);
  
  scoped_ptr<Value> new_value(value);
  Value* old_value = NULL;
  prefs_.GetValue(key, &old_value);
  if (!old_value || !value->Equals(old_value)) {
    prefs_.SetValue(key, new_value.release());
    ReportValueChanged(key);
  }
}

void ValueMapDevicePropertyStore::SetValueSilently(const std::string& key, 
                                                   base::Value* value) {
  DCHECK(value);
  scoped_ptr<Value> new_value(value);
  Value* old_value = NULL;
  prefs_.GetValue(key, &old_value);
  if (!old_value || !value->Equals(old_value)) {
    prefs_.SetValue(key, new_value.release());
    scoped_ptr<PropertyFormat> format(
        new PropertyFormat("SetValueSliently", key, value));
    trace_object_.snapshot(&format);
  }
}

void ValueMapDevicePropertyStore::RemoveValue(const std::string& key) {
  if (prefs_.RemoveValue(key)) {
    scoped_ptr<PropertyFormat> format(
        new PropertyFormat("RemoveValue", key));
    trace_object_.snapshot(&format);
    ReportValueChanged(key);
  }
}

void ValueMapDevicePropertyStore::Reset(DictionaryValue* value_owned) {
  scoped_ptr<DictionaryValue> dict(value_owned);
  prefs_.Clear();
  for (DictionaryValue::Iterator it(*value_owned); 
      !it.IsAtEnd(); it.Advance()) {
    prefs_.SetValue(it.key(), it.value().DeepCopy());
  }
  scoped_ptr<PropertyFormat> format(
      new PropertyFormat("Reset", &prefs_));
  trace_object_.snapshot(&format);
}

DictionaryValue* ValueMapDevicePropertyStore::Serialize() {
  DictionaryValue* dict = new DictionaryValue();
  for (PrefValueMap::const_iterator it = prefs_.begin(); 
      it != prefs_.end(); ++it) {
    // use SetValue() will lost metadata, result in can not
    // Deserialize, item info lost.
    dict->SetWithoutPathExpansion(it->first, it->second->DeepCopy());
  }
  return dict;
}


void ValueMapDevicePropertyStore::ChangeContent(DictionaryValue* value_owned) {
 scoped_ptr<DictionaryValue> dict(value_owned);
  for (DictionaryValue::Iterator it(*value_owned); 
      !it.IsAtEnd(); it.Advance()) {
    DCHECK(HasPrefPath(it.key()));
    SetValueInner(it.key(), it.value().DeepCopy());
  }
  scoped_ptr<PropertyFormat> format(
      new PropertyFormat("ChangeContent", &prefs_));
  trace_object_.snapshot(&format);
}

void ValueMapDevicePropertyStore::DetachFromThread() {
  // DevicePropertyStore is NonThreadSafe
  DevicePropertyStore::DetachFromThread();
  notifier_.DetachFromThread();
}

} // namespace device
