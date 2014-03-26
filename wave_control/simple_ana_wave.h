#pragma once

#include "wave_control/wave.h"
#include "wave_control/ana_wave_data.h"

class SimpleWaveDelegate {
public:

};

enum AnaWaveShowStyle {
  
};

class SimpleAnaWave : public Wave {
public:
  // implement wave
  virtual SimpleAnaWave* AsSimpleAnaWave() OVERRIDE { return this; }

  AnaWaveData& Data();

  // command
  // for do FFT.
  void DoRangeCommand(int command_id, WaveRange range);
 
  void DoCommand(int command_id);


  
protected:

private:

};