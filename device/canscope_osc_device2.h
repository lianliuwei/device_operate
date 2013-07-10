#pragma once

namespace canscope {

class OscDevice : public Device {
 public:
  Property<bool> osc_start_;

  BoolBind0& start_osc_;
 protected:
  
  bool GetValue(string pref_name, value* );
  Error LoadPref(PrefStore* pref);
  SetToDevice();
  PrefStore pref;

 private:
  
};

// sync
class OscDeviceHandle {
 public:
  // async need life control.
  class ErrorHandle{
    OnError(Error error);  
  };
  Property<bool> osc_start_;
  
  void Start();
  void Stop();
  bool IsStart();

  Error GetLastError();
 protected:
  void NotifyProperyUpdate(string pref_name);
  void UpdateConfig(PrefStore* );
  void SetValue(String name, Value* value);
  void SetValue(String name, Value* value, ErrorHandle* handle);
  
 private:
  
};

{
  handle->osc_start_.Set(false);
  if (handle->GetLastError())
    do something;
  HANDLE_ERROR();

  {
    // handle operate
    ScopedDeviceWrite scope();
    handle->attr1.Set();
    handle->attr2.Set();
    CheckError();
    PropertyType attr = handle->attr3.Get();
    CheckError();
  }
  HANDLE_ERROR();
}

{
  // LoadPref, restore.
  PrefStore* pref;
  OscDevice* device;
  device->NoSetDevice();
  bool ret = device->LoadPref(pref);
  if (!ret)
    device->Restore();
  else
    device->SetDevice();  
}

{


}

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
    if (!CheckValue(value)) {
      return;
    }
    bind_.set_value(value);
    UpdatePref(value);
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
    Type temp;
    bool ret = ConvertValueToType(value, &temp);
    if (!ret) {
      HandleError(DEVICE_PROPERTY_VALUE_CONVERT);
      ResetValue();
      return;
    }
    bind_.set_value(temp);
  }

  bool CheckValue(Type& value);

  void HandleError(device::Error error);

private:
  PropertyDelegate* delegate_;
  string name_;
  // keep the last right value to reset the Pref when it is Wrong.
  Value* last_value_;
};

}