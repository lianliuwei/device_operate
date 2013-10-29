#pragma once

#include <vector>

#include "base/basictypes.h"

#include "depend_calc/calc_data.h"

namespace canscope {

// chnl data with vertical offset 
class OscChnlData : public CalcData::Data {
public:
  OscChnlData(int size);
  virtual ~OscChnlData() {}

  int size() const;
  double* data();
  const double* data() const;

private:
  std::vector<double> data_;
};

} // namespace canscope
