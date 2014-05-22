#pragma once

#include "base/timer.h"
#include "base/callback.h"

class DelaySetter {
public:
  // void OnSetValue(bool real_set, double value);
  typedef base::Callback<void(bool, int)> SetValueCallbackType;

  DelaySetter(int ms, SetValueCallbackType callback);
  explicit DelaySetter(int ms);
  ~DelaySetter() {}

  void SetCallback(SetValueCallbackType callback);

  bool InSession() const { return session_; }

  // BeginSet -> *SetValue EndSet
  void BeginSet(int value);
  void SetValue(int value);
  void EndSet(int value);

  int GetValue() const;

private:
  bool session_;

  void OnDelayPast();

  base::Timer timer_;

  int value_;
  SetValueCallbackType callback_;

  DISALLOW_COPY_AND_ASSIGN(DelaySetter);
};


