#pragma once

#include "wave_control/ana_wave_data.h"

namespace canscope {

class ChnlAnaData : public AnaWaveData {
public:
  ChnlAnaData();
  virtual ChnlAnaData();

  // implement AnaWaveData
  virtual WaveRange data_range();
  virtual double* data();
  virtual int size();

private:

  DISALLOW_COPY_AND_ASSIGN(ChnlAnaData);
};

} // namespace canscope