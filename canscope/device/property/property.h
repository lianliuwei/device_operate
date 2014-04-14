#pragma once

#include <string>
#include "base/callback.h"
#include "base/debug/trace_event.h"

#include "base/synchronization/waitable_event.h"

#include "canscope/device/property/property_delegate.h"
#include "canscope/device/property/store_member.h"
#include "canscope/device_errors.h"
#include "canscope/async_task.h"

namespace canscope {

template<typename Type, typename StoreType = StoreMember<Type>>
class Property {
public:
  typedef base::Callback<bool(Type, const std::string&)> CheckValueCallback;

  Property(PropertyDelegate* delegate,
      StoreType& member, StoreType& device_member,
      base::Closure call_back, CheckValueCallback check_value)
      : delegate_(delegate)
      , member_(member)
      , device_member_(device_member)
      , call_back_(call_back)
      , check_value_(check_value)
      , event_(true, false)
      , async_count_(0) {}

 Property(PropertyDelegate* delegate, 
      StoreType& member, StoreType& device_member,
      base::Closure call_back)
      : delegate_(delegate)
      , member_(member)
      , device_member_(device_member)
      , call_back_(call_back)
      , event_(true, false)
      , async_count_(0) {}

  Property(PropertyDelegate* delegate, 
      StoreType& member, StoreType& device_member)
      : delegate_(delegate)
      , member_(member)
      , device_member_(device_member)
      , event_(true, false)
      , async_count_(0){}

  ~Property() {
    {
    base::AutoLock lock(lock_);
    CHECK(async_count_ == 0) << "must wait for all async task finish before destroy Property";
    }
  }

  Type value() const {
    return member_.value();
  }

  // sync call
  // call on the thread it belong
  void set_value(const Type& value) {
    TRACE_EVENT0("Property", "SetValueSync");
    TRACE_EVENT_FLOW_BEGIN0("Property", "SetValueSync", this);

    // clean this thread error
    canscope::device::CleanError();

    if (!CheckValue(value)) {
      TRACE_EVENT_FLOW_END0("Property", "SetValueSync", this);
      // General check error
      canscope::device::SetDeviceErrorIfNoError(
          canscope::device::ERR_INVAILD_VALUE);
      return;
    }
    TRACE_EVENT_FLOW_STEP0("Property", "SetValueSync", this, "CheckValue");
    SetValueImpl(value);
    TRACE_EVENT_FLOW_END0("Property", "SetValueSync", this);
  }

  // async set_value
  void set_value(const Type& value, AsyncTaskHandle async_task) {
    TRACE_EVENT_FLOW_BEGIN0("Property", "SetValueAsync", async_task.get());

    async_task->NotifyStart(NULL);
    if (!CheckValue(value)) {
      TRACE_EVENT_FLOW_END0("Property", "SetValueAsync", async_task.get());
      // General check error
      canscope::device::SetDeviceErrorIfNoError(
          canscope::device::ERR_INVAILD_VALUE);
      async_task->NotifyError(
          canscope::device::ErrorAsDictionary(LastDeviceError()));
      TRACE_EVENT_FLOW_END0("Property", "SetValueAsync", async_task.get());
      return;
    }
    DCHECK(!IsDeviceThread());
    TRACE_EVENT_FLOW_STEP0("Property", "SetValueAsync",
        async_task.get(), "CheckValue");
    {
    base::AutoLock lock(lock_);
    ++async_count_;
    }
    delegate_->PostDeviceTask(Bind(&Property::SetValueAsyncImpl,
        base::Unretained(this), value, async_task.get()));
  }

  bool CheckValue(const Type& value) {
    if (check_value_.is_null()) {
      return true;
    }
    return check_value_.Run(value, member_.pref_name());
  }

  // observer
  void AddPrefObserver(DevicePropertyStore::Observer* obs) {
    delegate_->GetDevicePropertyStore()->
        AddPrefObserver(member_.pref_name(), obs);
  }
  void RemovePrefObserver(DevicePropertyStore::Observer* obs) {
    delegate_->GetDevicePropertyStore()->
        RemovePrefObserver(member_.pref_name(), obs);
  }
  bool HasObserver(DevicePropertyStore::Observer* obs) const {
    return delegate_->GetDevicePropertyStore()->
        HasObserver(member_.pref_name(), obs);
  }

private:
  void SetValueImpl(const Type& value) {
    if (delegate_->IsBatchMode()) {
      member_.set_value(value);
      return;
    }
    // Post
    if (!IsDeviceThread()) {
      delegate_->PostDeviceTask(
          base::Bind(&Property::SetValueImplSync,
              base::Unretained(this), value));
      WaitForCallFinish();

      // set this thread error from device thread.
      SetDeviceError(device_error_);

      TRACE_EVENT_FLOW_STEP0("Property", "SetValueSync", this, "FetchNewPref");
      delegate_->FetchNewPref();
      return;
    } else {
      TRACE_EVENT_FLOW_STEP0("Property", "SetValueSync", this, "SetValueDeviceThread");
      // same thread set_value() no need to wait. or will Death Lock
      device_member_.set_value(value);
      CallCallback();

      // use device thread error

      TRACE_EVENT_FLOW_STEP0("Property", "SetValueSync", this, "FetchNewPref");
      delegate_->FetchNewPref();

      return;
    }
  }

  void SetValueImplSync(Type value) {
    TRACE_EVENT0("Property", "SetValueDeviceThread");
    TRACE_EVENT_FLOW_STEP0("Property", "SetValueSync", this, "SetValueDeviceThread");

    canscope::device::CleanError();

    DCHECK(IsDeviceThread());
    device_member_.set_value(value);
    CallCallback();
    
    // TODO check and record last error
    // CheckLastError();
    device_error_ = canscope::device::LastDeviceError();

    SignalFinish();
  }

  void SetValueAsyncImpl(Type value, AsyncTaskHandle async_task) {
    DCHECK(IsDeviceThread());

    if (async_task->UserCancel()) {
      async_task->NotifyCancel(NULL);
      TRACE_EVENT_FLOW_STEP0("Property", "SetValueAsync",
          async_task.get(), "UserCancel");
      TRACE_EVENT_FLOW_END0("Property", "SetValueAsync", async_task.get());
      return;
    }

    TRACE_EVENT0("Property", "SetValueDeviceThread");
    TRACE_EVENT_FLOW_STEP0("Property", "SetValueAsync",
        async_task.get(), "SetValueDeviceThread");

    canscope::device::CleanError();

    device_member_.set_value(value);
    CallCallback();

    if (canscope::device::LastDeviceError() == canscope::device::OK) {
      async_task->NotifyFinish(NULL);
    } else {
      async_task->NotifyError(
          canscope::device::ErrorAsDictionary(
              canscope::device::LastDeviceError()));
    }

    TRACE_EVENT_FLOW_END0("Property", "SetValueAsync", async_task.get());
    {
    base::AutoLock lock(lock_);
    --async_count_;
    }
  }

  void CallCallback() {
    if (!call_back_.is_null()) {
      call_back_.Run();
    }
    std::string reason;
    reason = "SetProperty: " + device_member_.pref_name();
    delegate_->SetPropertyFinish(reason);
  }

  void WaitForCallFinish() {
    event_.Wait();
    event_.Reset();
  }

  void SignalFinish() {
    event_.Signal();
  }

  PropertyDelegate* delegate_;
  StoreType& member_;
  StoreType& device_member_;
  base::Closure call_back_;
  CheckValueCallback check_value_;
  // error happen on device
  canscope::device::Error device_error_;

  // sync call
  base::WaitableEvent event_;

  // async call
  int async_count_;
  base::Lock lock_;
};

typedef Property<bool> BooleanProperty;
typedef Property<int> IntegerProperty;
typedef Property<double> DoubleProperty;
typedef Property<std::string> StringProperty;
typedef Property<base::FilePath> FilePathProperty;

} // namespace canscope
