#pragma once

namespace canscope {

class CANScopeDevice {
 public:
  RegisterMemory osc_register_;
  // BoolBind0 osc_start_;
  BoolProperty osc_start_property;
 protected:
  
 private:
  
};

// notify On HardwareProperty change.
template <typename type>
class HardwareProperty {

  virtual void Init() {
    OnPrefChanged();
  }
  void set(type& value) {
    set_value(name_, ConvertToValue(Value));
    
    SetBind(value);
    NotifyPropertyChange();
  }

  type get() const {
    return GetBind();
  }

protected:
  // OnPrefChange
  virtual void OnPrefChanged() {
    type value = ValueToType();
    SetBind(value);
  }

  void NotifyPropertyChange() {
    // notify string name.
  }

  virtual void SetBind(type& value) {

  }
  
  virtual type GetBind() const {

  }

private:
  BoolBind bind_;
  uint8* addr_;
  string name_;
};

//class BoolProperty : public HardwareProperty<bool> {
// public:
//  virtual void SetBind(type& value) {
//    bind_.set_value(value);
//  }
//  
//  virtual type GetBind() const {
//    bind_.value();
//  }
// protected:
//  
// private:
//
//};
}
{

class RHL {
  BoolProperty enable;
  RangeProperty value;
  
};

class int14 {
  int32 AsInt32();
  bool SetAsInt32(int32 val);
};

class RangeReg {
  int max;
  int min;
  int step;
  bool MoveToStep(int step);

};


RangeCheck check = rhl.value.GetCheck();

// RegScopeTrigger
class RegScope {
  Uint32LEProperty trig_pre;
  Uint32LEProperty trig_post;
  Uint32LEProperty auto_time;
  Uint32LEProperty div_coe;
  BoolBind0 collect_finish;
  BoolBind1 trigged;
  BoolBind2 pre_trigged;
  Uint32LEProperty trigger_addr;
  Uint32LEProperty trigger_start_addr;
  Uint32LEProperty trigger_end_addr;
  Enum3BitProperty trigger_source;
  Uint8Property comp_high;
  Uint8Property comp_low;
  Enum3BitProperty trigger_type;
  BoolBind7 no_auto;
  Enum3BitProperty trigger_cmp;
  Uint30Property trigger_time;
  BoolBind0 scoped_ctrl;

  RegisterMemory scope_regeister_memory;

  void Read();
  void Write();
};

template<int start, int end>
class RegScopeScopeWrite {
  RegScopeScopeWrite();
  ~RegScopeScopeWrite();
};

class OscDevice {
  int StartBat();
  void IncBat();

  void Start();
  void Stop();
  void SetState(OscDeviceState state);

  ValueProperty offset_;
  PrefMember pre_watch_;

  SetDevice();
};

class OscDeviceScopeBat() {

};

class OscDeviceHandle {
  HandleDoubleProperty offset_;
  
  DeviceError GetError();
};

class HandleDoubleProperty {
  DeviceError Set(double val);
  double Get();
};

}
