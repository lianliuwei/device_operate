#include "depend_calc/calc_executer.h"

#include "base/threading/sequenced_worker_pool.h"

#include "common/common_thread.h"

#include "depend_calc/calc_group_walker.h"

using namespace common;
using namespace std;
using namespace base;


CalcExecuter::CalcExecuter(CalcGroup* group)
    : delegate_(NULL)
    , group_(group)
    , notify_end_(true) {
  CHECK(group_);
}

void CalcExecuter::StartRun(AsyncTaskHandle task) {
  DCHECK(delegate_) << "must set delegate";
  task_.swap(task);
  Reset();
  StartAllRunnable();
}

void CalcExecuter::Reset() {
  DCHECK(notify_end_);
  walker_.reset(new CalcGroupWalker(group_));
  notify_end_ = false;
}

void CalcExecuter::StartAllRunnable() {
  SequencedWorkerPool* pool = CommonThread::GetBlockingPool();

  vector<CalcItem*> calc_items;
  {
  AutoLock lock(lock_);
  if (task_->UserCancel()) {
    // notify canceled only once.
    if (!notify_end_) {
      notify_end_ = true;
      task_->NotifyCancel(NULL);
    }
    return;
  }
  calc_items = walker_->StartAll();
  }
  for (vector<CalcItem*>::iterator it = calc_items.begin(); 
      it != calc_items.end(); 
      ++it) {
    pool->PostWorkerTask(FROM_HERE, Bind(&CalcExecuter::RunItem, 
        Unretained(this), *it));
  }
}

void CalcExecuter::RunItem(CalcItem* item) {
  {
  AutoLock lock(lock_);
  if (task_->UserCancel()) {
    // notify canceled only once.
    if (!notify_end_) {
      notify_end_ = true;
      task_->NotifyCancel(NULL);
    }
    return;
  }
  }

  bool ret = item->Run(delegate_);

  {
  AutoLock lock(lock_);
  walker_->Mark(item, ret);
  if (walker_->Finish()) {
    if (!notify_end_) {
      notify_end_ = true;
      task_->NotifyFinish(NULL);
    }
    return;
  }
  }
  StartAllRunnable();
}

