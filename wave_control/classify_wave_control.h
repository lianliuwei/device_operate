#pragma once

#include "wave_control/wave_control.h"

// sort the WaveContainer by the order define in cc
class ClassifyWaveControl : public WaveControl {
public:
  ClassifyWaveControl() {}
  virtual ~ClassifyWaveControl() {}

  // override WaveControl
  virtual void AddWaveContainer(WaveContainer* container) OVERRIDE;

private:
};