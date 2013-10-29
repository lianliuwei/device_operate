#pragma once

#include <deque>

#include "depend_calc/calc_group.h"

class CalcGroupWalker {
public:
  // no take ownership of group
  CalcGroupWalker(const CalcGroup* group);
  ~CalcGroupWalker() {}

  std::vector<CalcItem*> Runnable();
  std::vector<CalcItem*> Current();
  bool Finish();

  CalcItem* StartOne();
  std::vector<CalcItem*> StartAll();
  void Mark(CalcItem * item, bool success);

private:
  scoped_ptr<CalcGroup> graph_;
  std::deque<CalcItem*> runnable_;
  std::deque<CalcItem*> current_;

  DISALLOW_COPY_AND_ASSIGN(CalcGroupWalker);
};
