#pragma once

#include <deque>

#include "depend_calc/calc_group.h"

class CalcGroupWalker {
public: 
  CalcGroupWalker(CalcGroup* group);
  ~CalcGroupWalker() {}

  std::vector<CalcItem*> Runnable();
  std::vector<CalcItem*> Current();
  bool Finish();

  CalcItem* StartOne();
  std::vector<CalcItem*> StartAll();
  void Mark(CalcItem * item, bool success);

private:
  void FreeNode();

  scoped_ptr<CalcGroup> graph_;
  std::deque<CalcItem*> runnable_;
  std::deque<CalcItem*> current_;
};