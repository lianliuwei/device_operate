#pragma once


#include "depend_calc/calc_data.h"
#include "depend_calc/calc_delegate.h"

#include "canscope/device/osc_device/osc_raw_data.h"

namespace canscope {

class ChnlCalcResult : public CalcData
                     , public CalcDelegate {
public:
  ChnlCalcResult(OscRawDataHandle handle);

  OscRawData* raw_data();

private:
  virtual ~ChnlCalcResult() {}
 
  // CalcDelegate implement
  virtual void ItemFinish(CalcKey key, CalcItem* item, bool ret) {}

  OscRawDataHandle handle_;
};

} // namespace canscope
