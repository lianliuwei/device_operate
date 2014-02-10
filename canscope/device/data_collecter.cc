#include "canscope/device/data_collecter.h"

void DataCollecter::ReRun() {
  // if user click single before this, start may lose
  // if Single() before offine, Start() is call wrong.
  // but these are acceptable
  if (stop_by_offine_) {
    Start();
    stop_by_offine_ = false;
  }
}

bool DataCollecter::LoopRunImp() {
  LoopState state = STOP;
  do {
    state = OnLoopRun();
  } while (state == IMMEDIATE);

  return state == NEXT_LOOP;
}

void DataCollecter::SetFreq(double freq) {
  next_loop_delay_ = 
      base::TimeDelta::FromMilliseconds(static_cast<double>(1000.0/freq));
}

double DataCollecter::Freq() const {
  return 1.0 / next_loop_delay_.InSecondsF();
}
