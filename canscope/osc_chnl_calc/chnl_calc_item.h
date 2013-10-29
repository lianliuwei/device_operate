#pragma once

#include "base/compiler_specific.h"

#include "canscope/osc_chnl_calc/osc_chnl_data.h"

#include "depend_calc/calc_delegate.h"

namespace canscope {

class ChnlCalcItem : public CalcItem {
public:
  // can_h_key as id
  ChnlCalcItem(std::string name, CalcKey can_h_key, CalcKey can_l_key);
  virtual ~ChnlCalcItem() {}

private:
  // CalcItem Implement
  virtual bool Run(CalcDelegate* delegate) OVERRIDE;

  virtual CalcItem* Clone() const OVERRIDE;

  CalcKey can_h_key_;
  CalcKey can_l_key_;
};

} // namespace canscope
