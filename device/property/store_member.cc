#include "device/property/store_member.h"

#include "base/bind_helpers.h"
#include "base/callback.h"
#include "base/location.h"

namespace device {
namespace subtle {

StoreMemberBase::StoreMemberBase()
    : prefs_(NULL),
      setting_value_(false) {
}

StoreMemberBase::~StoreMemberBase() {
  Destroy();
}

void StoreMemberBase::Init(const char* pref_name,
                           DevicePropertyStore* prefs,
                           const NamedChangeCallback& observer) {
  observer_ = observer;
  Init(pref_name, prefs);
}

void StoreMemberBase::Init(const char* pref_name,
                           DevicePropertyStore* prefs) {
  DCHECK(pref_name);
  DCHECK(prefs);
  DCHECK(pref_name_.empty());  // Check that Init is only called once.
  prefs_ = prefs;
  pref_name_ = pref_name;
  // Check that the preference is registered.
  DCHECK(prefs_->HasPrefPath(pref_name_))
      << pref_name << " not registered.";

  // Add ourselves as a pref observer so we can keep our local value in sync.
  prefs_->AddPrefObserver(pref_name, this);

  // record current thread, can only operate on this thread
  thread_loop_ = base::MessageLoopProxy::current();
}

void StoreMemberBase::Destroy() {
  if (prefs_ && !pref_name_.empty()) {
    prefs_->RemovePrefObserver(pref_name_, this);
    prefs_ = NULL;
  }
}

void StoreMemberBase::OnPreferenceChanged(const std::string& pref_name) {
  VerifyValuePrefName();
  UpdateValueFromPref((!setting_value_ && !observer_.is_null()) ?
      base::Bind(observer_, pref_name) : base::Closure());
}

void StoreMemberBase::UpdateValueFromPref(const base::Closure& callback) const {
  VerifyValuePrefName();
  DCHECK(prefs_->HasPrefPath(pref_name_));
  CheckOnCorrectThread();
  base::ScopedClosureRunner closure_runner(callback);
  const base::Value* value = NULL; 
  bool rv = prefs_->GetValue(pref_name_, &value);
  DCHECK(rv && value != NULL);
  rv = UpdateValueInternal(*value);
  DCHECK(rv);
}

void StoreMemberBase::VerifyPref() const {
  VerifyValuePrefName();
  UpdateValueFromPref(base::Closure());
}

void StoreMemberBase::InvokeUnnamedCallback(const base::Closure& callback,
                                            const std::string& pref_name) {
  callback.Run();
}

bool StoreMemberBase::IsOnCorrectThread() const {
  // In unit tests, there may not be a message loop.
  return thread_loop_ == NULL || thread_loop_->BelongsToCurrentThread();
}

void StoreMemberBase::DetachFromThread() {
  thread_loop_ = NULL;
}


} // namespace subtle

} // namespace device
