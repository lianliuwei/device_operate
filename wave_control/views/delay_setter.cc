#include "wave_control/views/delay_setter.h"

using namespace base;

DelaySetter::DelaySetter(int ms, SetValueCallbackType callback) 
    : callback_(callback)
    , session_(false)
    , value_(-1)
    , timer_(FROM_HERE, 
             TimeDelta::FromMilliseconds(ms), 
             Bind(&DelaySetter::OnDelayPast, Unretained(this)), 
             false) {
}

DelaySetter::DelaySetter(int ms) 
    : timer_(FROM_HERE, 
             TimeDelta::FromMilliseconds(ms), 
             Bind(&DelaySetter::OnDelayPast, Unretained(this)), 
             false)
    , session_(false)
    , value_(-1) {
}

void DelaySetter::SetCallback(SetValueCallbackType callback) {
  callback_ = callback;
}

int DelaySetter::GetValue() const {
  return value_;
}

void DelaySetter::OnDelayPast() {
  if (!callback_.is_null()) {
    callback_.Run(true, value_);
  }
}

void DelaySetter::BeginSet(int value) {
  DCHECK(!session_);
  session_ = true;
  value_ = value;

  if (!callback_.is_null()) {
    callback_.Run(true, value_);
  }
}

void DelaySetter::SetValue(int value) {
  DCHECK(session_);
  value_ = value;
  if (timer_.IsRunning()) {
  } else {
    timer_.Reset();
  }
  if (!callback_.is_null()) {
    callback_.Run(false, value_);
  }
}

void DelaySetter::EndSet(int value) {
  DCHECK(session_);
  session_ = false;
  value_ = value;
  if (!callback_.is_null()) {
    callback_.Run(true, value_);
  }
}

