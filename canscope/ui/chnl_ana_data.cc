#include "canscope/ui/chnl_ana_data.h"

namespace canscope {


WaveRange ChnlAnaData::data_range() {
  return wave_->horizontal_range();
}

double* ChnlAnaData::data() {
  if (!chnl_->HasChnlData()) {
    return NULL;
  }
  return chnl_->ChnlData().data();
}

int ChnlAnaData::size() {
  if (!chnl_->HasChnlData()) {
    return NULL;
  }
  return chnl_->ChnlData().size();
}


} // namespace canscope