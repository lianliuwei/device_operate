#include "depend_calc/calc_group.h"

#include <deque>

#include "base/logging.h"
#include "base/stl_util.h"

using namespace std;
using namespace base;

namespace {
struct KeyEqual {
  CalcKey key;
  bool operator() (CalcItem* item) {
    return item->id() == key;
  }
};

}

CalcGroup::CalcGroup(const std::string& name) 
  : name_(name) {

}

CalcGroup::~CalcGroup() {
  STLDeleteElements(&all_nodes_);
}

void CalcGroup::AddCalcItem(CalcItem* item) {
  DCHECK(!HasCalcItem(item)) << "item already be added";
  all_nodes_.push_back(item);
}

void CalcGroup::RemoveCalcItem(CalcItem* item) {
  DCHECK(HasCalcItem(item)) << "item no exist";

  all_nodes_.erase(std::remove(all_nodes_.begin(),
                               all_nodes_.end(),
                               item),
                   all_nodes_.end());

  // Remove all dependency edges that contain this node.
  EdgeMap::iterator it = edges_.begin();
  while (it != edges_.end()) {
    EdgeMap::iterator temp = it;
    ++it;

    if (temp->first == item || temp->second == item)
      edges_.erase(temp);
  }

  delete item;
}

bool CalcGroup::HasCalcItem(CalcItem* item) {
 vector<CalcItem*>::iterator it = std::find(all_nodes_.begin(), 
                                            all_nodes_.end(), 
                                            item);
  return it != all_nodes_.end();
}

bool CalcGroup::HasCalcItem(CalcKey key) {
  return GetCalcItem(key) != NULL;
}

CalcItem* CalcGroup::GetCalcItem(CalcKey key) {
  KeyEqual key_equal = { key };
  vector<CalcItem*>::iterator it = std::find_if(all_nodes_.begin(), 
                                                all_nodes_.end(), 
                                                key_equal);
  return it != all_nodes_.end() ? *it : NULL;
}

void CalcGroup::SetDepend(CalcItem* er, CalcItem* ee) {
  DCHECK(HasCalcItem(er));
  DCHECK(HasCalcItem(ee));
  edges_.insert(std::make_pair(er, ee));
}

void CalcGroup::SetDepend(CalcKey er, CalcKey ee) {
  SetDepend(GetCalcItem(er), GetCalcItem(ee));
}

CalcGroup* CalcGroup::Clone() {
  CalcGroup* group = new CalcGroup(name_);
  group->all_nodes_ = all_nodes_;
  group->edges_ = edges_;
  return group;
}

vector<CalcItem*> CalcGroup::FreeNode() {
  std::vector<CalcItem*> queue;

  std::copy(all_nodes_.begin(),
    all_nodes_.end(),
    std::back_inserter(queue));

  std::vector<CalcItem*>::iterator queue_end = queue.end();
  for (EdgeMap::const_iterator it = edges_.begin();
    it != edges_.end(); ++it) {
      queue_end = std::remove(queue.begin(), queue_end, it->second);
  }
  queue.erase(queue_end, queue.end());

  return queue;
}

std::vector<CalcItem*> CalcGroup::NodeSuccess(CalcItem* node) {
  std::vector<CalcItem*> output;
  std::pair<EdgeMap::iterator, EdgeMap::iterator> range =
      edges_.equal_range(node);
  EdgeMap::iterator it = range.first;
  while (it != range.second) {
    CalcItem* dest = it->second;
    EdgeMap::iterator temp = it;
    ++it;
    edges_.erase(temp);

    bool has_incoming_edges = false;
    for (EdgeMap::iterator jt = edges_.begin(); jt != edges_.end(); ++jt) {
      if (jt->second == dest) {
        has_incoming_edges = true;
        break;
      }
    }

    if (!has_incoming_edges)
      output.push_back(dest);
  }
  delete node;
  return output;
}

void CalcGroup::NodeFailed(CalcItem* node) {
  std::deque<CalcItem*> queue;
  queue.push_back(node);

  while (!queue.empty()) {
    CalcItem* node = queue.front();
    queue.pop_front();

    std::pair<EdgeMap::iterator, EdgeMap::iterator> range =
        edges_.equal_range(node);
    EdgeMap::iterator it = range.first;
    while (it != range.second) {
      CalcItem* dest = it->second;
      EdgeMap::iterator temp = it;
      it++;
      edges_.erase(temp);
      queue.push_back(dest);
    }
    delete node;
  }
}
