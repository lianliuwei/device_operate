#pragma once

#include "depend_calc/calc_item.h"

class CalcDelegate {
public:
  CalcDelegate() {}

  // may be  multithread call
  virtual void ItemFinish(CalcKey key, CalcItem* item, bool ret) = 0;

protected:
  virtual ~CalcDelegate() {}
};
