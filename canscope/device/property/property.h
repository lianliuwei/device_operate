#pragma once

#include <string>
#include "base/callback.h"
#include "base/synchronization/waitable_event.h"

#include "canscope/device/property/property_delegate.h"
#include "canscope/device/property/store_member.h"

namespace canscope {

template<typename Type, typename StoreType = StoreMember<Type>>
class Property {
public:
  typedef base::Callback<bool(Type, const std::string&)> CheckValueCallback;

  Property(PropertyDelegate* delegate, std::string name, 
      StoreType& member, StoreType& device_member, 
      base::Closure call_back, CheckValueCallback check_value)
      : delegate_(delegate)
      , name_(name)
      , member_(member)
      , device_member_(device_member)
      , call_back_(call_back)
      , check_value_(check_value)
      , event_(false, false) {
    device_name_ = delegate_->device_name();
  }

  ~Property() {
  }

  Type value() const {
    return member_.value();
  }

  // sync call
  // call on the thread it belong
  void set_value(const Type& value) {
    if (!CheckValue(value)) {
      return;
    }
    SetValueImpl(value);
  }

  // async set_value
  //void set_value(Type& value, AsyncTask async_task) {
  //  if (!CheckValue(value)) {
  //    async_task.NotifyCheckError();
  //    return;
  //  }
  //  DCHECK(!IsDeviceThread());
  //  delegate_->PostDeviceTask(Bind(&SetValueAsyncImpl, value, async_task));
  //}

  bool CheckValue(const Type& value) {
    return check_value_.Run(value, name_);
  }

  // observer
  void AddPrefObserver(DevicePropertyStore::Observer* obs) {
    delegate_->GetDevicePropertyStore()->AddPrefObserver(name_, obs);
  }
  void RemovePrefObserver(DevicePropertyStore::Observer* obs) {
    delegate_->GetDevicePropertyStore()->RemovePrefObserver(name_, obs);
  }
  bool HasObserver(DevicePropertyStore::Observer* obs) const {
    return delegate_->GetDevicePropertyStore()->HasObserver(name_, obs);
  }

private:
  void SetValueImpl(const Type& value) {
    if (delegate_->IsBathMode()) {
      member_.set_value(value);
      return;
    }
    // Post
    if (!IsDeviceThread()) {
      delegate_->PostDeviceTask(
          base::Bind(&Property::SetValueImplSync, 
              base::Unretained(this), value));
      WaitForCallFinish();
      delegate_->FetchNewPref();
      return;
    } else {
      // same thread set_value() no need to wait. or will Death Lock
      device_member_.set_value(value);
      delegate_->FetchNewPref();

      CallCallback();

      return;
    }
  }

  void SetValueImplSync(Type value) {
    DCHECK(IsDeviceThread());
    device_member_.set_value(value);
    CallCallback();
    // TODO check and record last error
    // CheckLastError();
    SignalFinish();
  }

  //void SetValueAsyncImpl(Type& value, AsyncTask async_task) {
  //  DCHECK(IsDeviceThread());
  //  device_member_.set_value(value);
  //  CallCallback();
  //  AyncTask()->NotifyFinish(GetLastError());
  //}

  void CallCallback() {
    call_back_.Run();
    //ConfigManager::Get(device_name_)
    //    ->ConfigUpdate("set property" + name_);
  }

  void WaitForCallFinish() {
    event_.Wait();
  }

  void SignalFinish() {
    event_.Signal();
  }

  PropertyDelegate* delegate_;
  StoreType& member_;
  StoreType& device_member_;
  base::Closure call_back_;
  CheckValueCallback check_value_;
  std::string name_;
  // cache delegate_->device_name() for access on otherthread
  std::string device_name_;

  // sync call
  base::WaitableEvent event_;
};

typedef Property<bool> BooleanProperty;
typedef Property<int> IntegerProperty;
typedef Property<double> DoubleProperty;
typedef Property<std::string> StringProperty;
typedef Property<base::FilePath> FilePathProperty;

} // namespace canscope
