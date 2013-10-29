#pragma once

#include "base/compiler_specific.h"

#include "canscope/osc_chnl_calc/osc_chnl_data.h"

class ChnlDiffCalcItem : public CalcItem {
public:
  // can_diff_key as id
  ChnlDiffCalcItem(std::string name, 
      CalcKey can_h_key, CalcKey can_l_key, CalcKey can_diff_key);
  virtual ~ChnlDiffCalcItem() {}

private:
  // may be call on other thread.
  virtual bool Run(CalcDelegate* delegate) OVERRIDE;

  virtual CalcItem* Clone() const OVERRIDE;

  CalcKey can_h_key_;
  CalcKey can_l_key_;
  CalcKey can_diff_key_;
};
