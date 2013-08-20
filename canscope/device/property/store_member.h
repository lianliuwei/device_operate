#pragma once

#include <string>
#include <vector>

#include "base/basictypes.h"
#include "base/bind.h"
#include "base/callback_forward.h"
#include "base/files/file_path.h"
#include "base/logging.h"
#include "base/memory/ref_counted.h"
#include "base/message_loop_proxy.h"
#include "base/values.h"

#include "canscope/device/property/device_property_store.h"

// modify from base/prefs/pref_member.h
// NOTICE NamedChangeCallback are no call when setting for current StoreMember
namespace canscope {

namespace subtle {

class StoreMemberBase : public DevicePropertyStore::Observer {
public:
  // Type of callback you can register if you need to know the name of
  // the pref that is changing.
  typedef base::Callback<void(const std::string&)> NamedChangeCallback;
  
  DevicePropertyStore* prefs() { return prefs_; }
  const DevicePropertyStore* prefs() const { return prefs_; }

protected:
  StoreMemberBase();
  virtual ~StoreMemberBase();

  void Init(const char* pref_name, DevicePropertyStore* prefs,
      const NamedChangeCallback& observer);
  void Init(const char* pref_name, DevicePropertyStore* prefs);

  void Destroy();

  // DevicePropertyStore::Observer
  virtual void OnPreferenceChanged(const std::string& pref_name) OVERRIDE;

  void VerifyValuePrefName() const {
    DCHECK(!pref_name_.empty());
  }

  // This method is used to do the actual sync with the preference.
  // Note: it is logically const, because it doesn't modify the state
  // seen by the outside world. It is just doing a lazy load behind the scenes.
  void UpdateValueFromPref(const base::Closure& callback) const;

  // Verifies the preference name, and lazily loads the preference value if
  // it hasn't been loaded yet.
  void VerifyPref() const;

  const std::string& pref_name() const { return pref_name_; }

  // Used to allow registering plain base::Closure callbacks.
  static void InvokeUnnamedCallback(const base::Closure& callback,
                                    const std::string& pref_name);

  void CheckOnCorrectThread() const {
    DCHECK(IsOnCorrectThread());
  }

private:
  // This method actually updates the value. It should only be called from
  // the thread the StoreMember is on.
  virtual bool UpdateValueInternal(const base::Value& value) const = 0;

  bool IsOnCorrectThread() const;

  // Ordered the members to compact the class instance.
  std::string pref_name_;
  NamedChangeCallback observer_;
  DevicePropertyStore* prefs_;

  scoped_refptr<base::MessageLoopProxy> thread_loop_;

protected:
  bool setting_value_;

};

}  // namespace subtle

template <typename ValueType>
class StoreMember : public subtle::StoreMemberBase {
public:
  StoreMember() {}
  virtual ~StoreMember() {}

  StoreMember(const char* pref_name, DevicePropertyStore* prefs,
              const NamedChangeCallback& observer) {
    Init(pref_name, prefs, observer);
  }

  StoreMember(const char* pref_name, DevicePropertyStore* prefs,
              const base::Closure& observer) {
    Init(pref_name, prefs, observer);
  }
  StoreMember(const char* pref_name, DevicePropertyStore* prefs) {
    Init(pref_name, prefs);
  }

  // Do the actual initialization of the class.  Use the two-parameter
  // version if you don't want any notifications of changes.  This
  // method should only be called on the UI thread.
  void Init(const char* pref_name, DevicePropertyStore* prefs,
            const NamedChangeCallback& observer) {
    subtle::StoreMemberBase::Init(pref_name, prefs, observer);
  }
  void Init(const char* pref_name, DevicePropertyStore* prefs,
            const base::Closure& observer) {
    subtle::StoreMemberBase::Init(
        pref_name, prefs,
        base::Bind(&StoreMemberBase::InvokeUnnamedCallback, observer));
  }
  void Init(const char* pref_name, DevicePropertyStore* prefs) {
    subtle::StoreMemberBase::Init(pref_name, prefs);
  }

  void Destroy() {
    subtle::StoreMemberBase::Destroy();
  }

  // Retrieve the value of the member variable.
  // This method should only be used from the thread the StoreMember is currently
  // on, which is the UI thread unless changed by |MoveToThread|.
  ValueType value() const {
    VerifyPref();
    return value_;
  }

  // Set the value of the member variable.
  // This method should only be called on the UI thread.
  void set_value(const ValueType& value) {
    CheckOnCorrectThread();
    VerifyValuePrefName();
    setting_value_ = true;
    UpdatePref(value);
    setting_value_ = false;
  }
  
private:
  virtual bool UpdateValueInternal(const base::Value& value) const OVERRIDE;

  // This method is used to do the actual sync with pref of the specified type.
  void UpdatePref(const ValueType& value);


  mutable ValueType value_;

  DISALLOW_COPY_AND_ASSIGN(StoreMember);
};

template <>
void StoreMember<bool>::UpdatePref(const bool& value);
template <>
bool StoreMember<bool>::UpdateValueInternal(const Value& value) const;

template <>
void StoreMember<int>::UpdatePref(const int& value);
template <>
bool StoreMember<int>::UpdateValueInternal(const Value& value) const;

template <>
void StoreMember<double>::UpdatePref(const double& value);
template <>
bool StoreMember<double>::UpdateValueInternal(const Value& value) const;

template <>
void StoreMember<std::string>::UpdatePref(const std::string& value);
template <>
bool StoreMember<std::string>::UpdateValueInternal(const Value& value) const;

template <>
void StoreMember<base::FilePath>::UpdatePref(const base::FilePath& value);
template <>
bool StoreMember<base::FilePath>::UpdateValueInternal(const Value& value) const;

typedef StoreMember<bool> BooleanStoreMember;
typedef StoreMember<int> IntegerStoreMember;
typedef StoreMember<double> DoubleStoreMember;
typedef StoreMember<std::string> StringStoreMember;
typedef StoreMember<base::FilePath> FilePathStoreMember;

} // namespace canscope
