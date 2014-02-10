#include "canscope/device_errors.h"

#include "base/debug/trace_event.h"
#include "base/lazy_instance.h"
#include "base/threading/thread_local.h"
#include "base/strings/stringize_macros.h"

using namespace canscope::device;
namespace {
base::LazyInstance<base::ThreadLocalPointer<ScopedDeviceError> > 
    lazy_tls_ptr = LAZY_INSTANCE_INITIALIZER;
}

namespace canscope {
namespace device {

Error LastDeviceError() {
  return lazy_tls_ptr.Pointer()->Get()->last_error;
}

void SetDeviceError(Error error) {
  Error last_error = LastDeviceError();
  DCHECK(last_error == OK) << "must CleanError() before call device operate";
  lazy_tls_ptr.Pointer()->Get()->last_error = error;
  TRACE_EVENT_INSTANT0("device_error", "change", TRACE_EVENT_SCOPE_THREAD);
}


void SetDeviceErrorIfNoError(Error error) {
  Error last_error = LastDeviceError();
  if (last_error == OK) {
    lazy_tls_ptr.Pointer()->Get()->last_error = error;
    TRACE_EVENT_INSTANT0("device_error", "change", TRACE_EVENT_SCOPE_THREAD);
  }
}

void CleanError() {
  lazy_tls_ptr.Pointer()->Get()->last_error = OK;
}


ScopedDeviceError::ScopedDeviceError()
    : last_error(OK) {
  DCHECK(!lazy_tls_ptr.Pointer()->Get());
  lazy_tls_ptr.Pointer()->Set(this);
}

ScopedDeviceError::~ScopedDeviceError() {
  DCHECK_EQ(lazy_tls_ptr.Pointer()->Get(), this);
  lazy_tls_ptr.Pointer()->Set(NULL);
}

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

const char* ErrorToString(Error error) {
  if (error == 0)
    return "device::OK";

  switch (error) {
#define DEVICE_ERROR(label, value) \
  case ERR_ ## label: \
  return "device::" STRINGIZE_NO_EXPANSION(ERR_ ## label);
#include "canscope/device_error_list.h"
#undef DEVICE_ERROR
  default:
    return "device::<unknown>";
  }
}


} // namespace device
} // namespace canscope
