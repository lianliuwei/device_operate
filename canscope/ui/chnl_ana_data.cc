#include "canscope/ui/chnl_ana_data.h"

#include "canscope/ui/chnl_wave.h"

namespace canscope {


WaveRange ChnlAnaData::data_range() {
  return wave_->horizontal_range();
}

double* ChnlAnaData::data() {
  if (!chnl_->HasChnlData()) {
    return NULL;
  }
  return const_cast<double*>(chnl_->ChnlData().data());
}

int ChnlAnaData::size() {
  if (!chnl_->HasChnlData()) {
    return NULL;
  }
  return chnl_->ChnlData().size();
}

ChnlAnaData::ChnlAnaData(ChnlWave* wave, ::Chnl* chnl)
    : wave_(wave)
    , chnl_(chnl) {

}


} // namespace canscope