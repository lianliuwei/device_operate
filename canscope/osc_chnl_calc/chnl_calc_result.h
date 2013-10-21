#pragma once

#include "depend_calc/calc_data.h"

class ChnlCalcResult : public CalcData
                     , public base::RefCountedThreadSafe<ChnlCalcResult>
                     , public CalcDelegate {
public:
  ChnlCalcResult();
  virtual ~ChnlCalcResult();

  void* chnl_h();
  void* chnl_l();
  void* chnl_diff();

  OscRawData* raw_data();

private:
  virtual void ItemFinish(CalcKey key, CalcItem* item, bool ret);

  OscRawDataHandle handle_;
};
