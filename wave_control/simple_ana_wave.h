#pragma once

#include "wave_control/wave.h"
#include "wave_control/ana_wave_data.h"

enum AnaWaveShowStyle {
  
};

class SimpleAnaWave : public Wave {
public:
  SimpleAnaWave() {}
  virtual ~SimpleAnaWave() {}

  // implement wave
  virtual SimpleAnaWave* AsSimpleAnaWave() OVERRIDE { return this; }
  virtual Type type_id() const OVERRIDE { return kSimpleAna; }

  virtual AnaWaveData& Data() = 0;

  // just for save the range, so no notify to the view.
  // all change is done be the view.
  virtual WaveRange vertical_range() = 0;
  virtual void set_vertical_range(const WaveRange& wave_range) = 0;
  virtual double vertical_offset() = 0;
  virtual void set_vertical_offset(double offset) = 0;

  virtual WaveRange horizontal_range() = 0;
  virtual void set_horizontal_range(const WaveRange& wave_range) = 0;
  virtual double horizontal_offset() = 0;
  virtual void set_horizontal_offset(double offset) = 0;

  // command
  // for do FFT.
  virtual void DoRangeCommand(int command_id, WaveRange range) = 0;
 
  virtual void DoCommand(int command_id) = 0;

private:

};
