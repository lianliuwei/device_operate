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
  OscDeviceProperty* property() { return handle_->property(); }
  bool hardware_diff() const { return handle_->hardware_diff(); }

private:
  virtual ~ChnlCalcResult() {}
 
  // CalcDelegate implement
  virtual void ItemFinish(CalcKey key, CalcItem* item, bool ret) {}
  virtual void AddCalcDelegateRef();
  virtual void ReleaseCalcDelegateRef();

  OscRawDataHandle handle_;

  DISALLOW_COPY_AND_ASSIGN(ChnlCalcResult);
};

} // namespace canscope
