#include "depend_calc/calc_item.h"

CalcItem::CalcItem(std::string name, CalcKey id)
    : name_(name)
    , id_(id) {
}

CalcItem* CalcItem::Clone() const {
  return new CalcItem(name_, id_);
}

bool CalcItem::Run(CalcDelegate* delegate) {
  // do nothing
  return true;
}
