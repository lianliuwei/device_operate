#pragma once

namespace canscope {

class PropertyDelegate {
  void WriteMemory(uint8* addr, uint32 length);
  PrefStore* GetPrefStore();
};

template<typename Type>
class Property : public PrefStore::Observer {
public:
  Property(PropertyDelegate* delegate, string name, uint8* addr);
  // need getter setter
  Type Get() const {
    bind_.get_value();
  }
  void Set(Type& value) {
    if (!CheckValue(value))
      return;
    bind_.set_value(value);
    UpdatePref(Get());
    WriteMemory(addr, bind_.GetLength());
  }

  
  // PrefStore::Observer
  virtual void OnPrefValueChanged(const std::string& key) {
    Value* value;
    bool ret = delegate_->GetPrefStore()->GetValue(&value);
    DCHECK(ret);
    //if (!ret) {
    //  // LOG(ERROR) << name_ << " Property no relate Value";
    //  HandleError(DEVICE_PROPERTY_VALUE_MISS);
    //  return;
    //}
    UpdateValueInternal(*value);
  }

  virtual void OnInitializationCompleted(bool succeeded) {
    if (succeeded) {
      OnPrefValueChanged(name_);
    }
  }

  void UpdatePref(const ValueType& value);

  bool UpdateValueInternal(const base::Value& value) const {
    bool ret = ConvertValueToType(value, &temp);
    if (!ret) {
      HandleError(DEVICE_PROPERTY_VALUE_CONVERT);
      return;
    }
    bind_.set_value(temp);
  }

  bool CheckValue(Type& value);
  Type SetToValue(Value);
  void HandleError(device::Error error);

private:
  PropertyDelegate* delegate_;
  string name_;
};

template<typename MemoryBind>
class BoolProperty : public Property<bool> {

  MemoryBind bool_bind_;
};

}