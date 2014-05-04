#pragma once

#include "base/memory/scoped_ptr.h"
#include "base/memory/ref_counted.h"
#include "base/synchronization/lock.h"

#include "depend_calc/calc_group.h"
#include "depend_calc/calc_group_walker.h"

#include "canscope/async_task.h"

class CalcExecuter : public base::RefCountedThreadSafe<CalcExecuter> {
public:
  // take ownership of group
  CalcExecuter(CalcGroup* group);

  void set_delegate(CalcDelegate* delegate);

  // run async
  // call must after previous task finish
  void StartRun(AsyncTaskHandle task);

private:
  friend class base::RefCountedThreadSafe<CalcExecuter>;
  ~CalcExecuter() {}

  // call after Run() reset group calc state
  void Reset();

  void StartAllRunnable();
  void RunItem(CalcItem* item);

  CalcDelegate* delegate_;
  scoped_ptr<CalcGroup> group_;

  base::Lock lock_;
  bool notify_end_;
  AsyncTaskHandle task_;
  scoped_ptr<CalcGroupWalker> walker_;
};
