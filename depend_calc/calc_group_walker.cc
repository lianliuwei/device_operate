#include "depend_calc/calc_group_walker.h"

#include <vector>

#include "base/logging.h"

CalcGroupWalker::CalcGroupWalker(CalcGroup* group) {
  DCHECK(group->NoCycle());
  graph_.reset(group->Clone());

  std::vector<CalcItem*> free_node = graph_->FreeNode();
  runnable_.insert(runnable_.end(), free_node.begin(), free_node.end());
}

CalcItem* CalcGroupWalker::StartOne() {
  DCHECK(runnable_.size() > 0);
  CalcItem* item = runnable_.front();
  runnable_.pop_front();
  current_.push_back(item);
  return item;
}

std::vector<CalcItem*> CalcGroupWalker::StartAll() {
  std::vector<CalcItem*> items(runnable_.begin(), runnable_.end());
  runnable_.clear();
  current_.insert(current_.end(), items.begin(), items.end());
  return items;
}

void CalcGroupWalker::Mark(CalcItem * item, bool success) {
  std::deque<CalcItem*>::iterator it = std::find(current_.begin(), 
                                                 current_.end(), 
                                                 item);
  DCHECK(it != current_.end());

  if (success) {
    std::vector<CalcItem*> free_node = graph_->NodeSuccess(item);
    runnable_.insert(runnable_.end(), free_node.begin(), free_node.end());
  } else {
    graph_->NodeFailed(item);
  }
}

bool CalcGroupWalker::Finish() {
  return (current_.size() + runnable_.size()) == 0;
}
