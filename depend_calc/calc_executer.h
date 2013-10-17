#pragma once

#include "base/synchronization/lock.h"

#include "canscope/async_task.h"

#include "depend_calc/calc_group.h"

class CalcExecuter {
public:
  // no take own of group
  // group must out live of CalcExecuter
  CalcExecuter(CalcGroup* group);
  ~CalcExecuter();

  void set_delegate(CalcDelegate* delegate);

  // run async
  // call must after previous task finish
  void StartRun(AsyncTaskHandle task);

private:
  // call after Run() reset group calc state
  void Reset();

  void StartAllRunnable();
  void RunItem(CalcItem* item);

  CalcDelegate* delegate_;
  CalcGroup* group_;

  base::Lock lock_;
  bool notify_end_;
  AsyncTaskHandle task_;
  scoped_ptr<CalcGroupWalker> walker_;
};
