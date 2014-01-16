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
