#pragma once

#include "wave_control/wave_control.h"

class YTWaveContainer;

// sort the WaveContainer by the order define in cc
class ClassifyWaveControl : public WaveControl {
public:
  ClassifyWaveControl() {}
  virtual ~ClassifyWaveControl() {}

  // override WaveControl
  virtual void AddWaveContainer(WaveContainer* container) OVERRIDE;
  // implement WaveControl
  virtual bool CANMoveWaveTo(Wave* wave, WaveContainer* container);
  virtual bool CANCreateWaveContainerAt(Wave* wave, size_t index);
  virtual void MoveWaveTo(Wave* wave, WaveContainer* container);
  virtual void CreateWaveContainerAt(Wave* wave, size_t index);

protected:
  virtual YTWaveContainer* CreateYTWaveContainer() = 0;

private:
};