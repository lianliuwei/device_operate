#pragma once

#include "wave_control/ana_wave_data.h"
#include "canscope/chnl/chnl.h"

namespace canscope {

class ChnlWave;

class ChnlAnaData : public AnaWaveData {
public:
  ChnlAnaData(ChnlWave* wave, ::Chnl* chnl);
  virtual ~ChnlAnaData();

  // implement AnaWaveData
  virtual WaveRange data_range();
  virtual double* data();
  virtual int size();

private:
  OscWave* wave_;
  ::Chnl* chnl_;

  DISALLOW_COPY_AND_ASSIGN(ChnlAnaData);
};

} // namespace canscope