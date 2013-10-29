#include "canscope/osc_chnl_calc/osc_chnl_data.h"

namespace canscope {

canscope::OscChnlData::OscChnlData(int size)
    : data_(size) {
}

int canscope::OscChnlData::size() const {
  return static_cast<int>(data_.size());
}

double* canscope::OscChnlData::data() {
  return &data_[0];
}

const double* canscope::OscChnlData::data() const {
  return &data_[0];
}

} // namespace canscope
