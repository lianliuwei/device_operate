#pragma once

#include "base/memory/ref_counted.h"

#include "depend_calc/calc_data.h"
#include "depend_calc/calc_delegate.h"

#include "canscope/device/osc_raw_data.h"

namespace canscope {

class ChnlCalcResult : public CalcData
                     , public base::RefCountedThreadSafe<ChnlCalcResult>
                     , public CalcDelegate {
public:
  ChnlCalcResult(OscRawDataHandle handle);
  virtual ~ChnlCalcResult() {}

  OscRawData* raw_data();

private:
  // CalcDelegate implement
  virtual void ItemFinish(CalcKey key, CalcItem* item, bool ret) {}

  OscRawDataHandle handle_;
};

} // namespace canscope
