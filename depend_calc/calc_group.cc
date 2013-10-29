#include "depend_calc/calc_group.h"

#include <deque>

#include "base/logging.h"
#include "base/stl_util.h"
#include "base/bind.h"

using namespace std;
using namespace base;

namespace {
struct KeyEqual {
  CalcKey key;
  bool operator() (CalcItem* item) {
    return item->id() == key;
  }
};

std::string NodeName(CalcItem* item) {
  return item->name();
}

}

CalcGroup::CalcGroup(const std::string name) 
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

bool CalcGroup::HasCalcItem(CalcItem* item) const {
 vector<CalcItem*>::const_iterator it = std::find(all_nodes_.begin(), 
                                                  all_nodes_.end(), 
                                                  item);
  return it != all_nodes_.end();
}

bool CalcGroup::HasCalcItem(CalcKey key) const {
  return GetCalcItem(key) != NULL;
}

CalcItem* CalcGroup::GetCalcItem(CalcKey key) const {
  KeyEqual key_equal = { key };
  vector<CalcItem*>::const_iterator it = std::find_if(all_nodes_.begin(), 
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

CalcGroup* CalcGroup::Clone(std::string name) const {
  CalcGroup* group = new CalcGroup(name);
  typedef std::map<CalcItem*, CalcItem*> ItemMap;
  ItemMap item_map;
  
  // clone all node
  for (std::vector<CalcItem*>::const_iterator it = all_nodes_.begin();
      it != all_nodes_.end(); ++it) {
    CalcItem* clone_item = (*it)->Clone();
    DCHECK(clone_item);
    group->all_nodes_.push_back(clone_item);
    item_map.insert(std::make_pair(*it, clone_item));
  }

  // copy map according node map
  for (EdgeMap::const_iterator it = edges_.begin();
      it != edges_.end(); ++it) {
    group->edges_.insert(
        std::make_pair(item_map[it->first], item_map[it->second]));
  }
  return group;
}

vector<CalcItem*> CalcGroup::FreeNode() const {
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
  DeleteIsolateNode(node);
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
      ++it;
      edges_.erase(temp);
      queue.push_back(dest);

      // clear all edge dest point to other
      EdgeMap::iterator jt = edges_.begin();
      while (jt != edges_.end()) {
        // use temp to key jt valid
        EdgeMap::iterator temp = jt;
        ++jt;
        if (temp->second == dest) {
          edges_.erase(temp);
        }
      }
    }
    DeleteIsolateNode(node);
  }
}

void CalcGroup::DeleteIsolateNode(CalcItem* node) {
  all_nodes_.erase(std::remove(all_nodes_.begin(),
                               all_nodes_.end(),
                               node),
                   all_nodes_.end());
  delete node;
}

std::string CalcGroup::DumpAsGraphviz(
    const std::string& toplevel_name, 
    const base::Callback<std::string(CalcItem*)>& 
        node_name_callback) const {
std::string result("digraph {\n");

  result.append("  /* Dependencies */\n");
  for (EdgeMap::const_iterator it = edges_.begin(); it != edges_.end(); ++it) {
    result.append("  ");
    result.append(node_name_callback.Run(it->second));
    result.append(" -> ");
    result.append(node_name_callback.Run(it->first));
    result.append(";\n");
  }
 
  result.append("\n  /* Toplevel node */\n");
  result.append("  ");
  result.append(toplevel_name);
  result.append(" [shape=box];\n");

  result.append("}\n");
  return result;
}

std::string CalcGroup::DumpAsGraphviz() const {
  return DumpAsGraphviz(name_, Bind(&NodeName));
}

bool CalcGroup::NoCycle() const {
  // Step 1: Build a set of nodes with no incoming edges.
  std::deque<CalcItem*> queue;
  std::copy(all_nodes_.begin(),
            all_nodes_.end(),
            std::back_inserter(queue));

  std::deque<CalcItem*>::iterator queue_end = queue.end();
  for (EdgeMap::const_iterator it = edges_.begin();
       it != edges_.end(); ++it) {
    queue_end = std::remove(queue.begin(), queue_end, it->second);
  }
  queue.erase(queue_end, queue.end());

  // Step 2: Do the Kahn topological sort.
  EdgeMap edges(edges_);
  while (!queue.empty()) {
    CalcItem* node = queue.front();
    queue.pop_front();

    std::pair<EdgeMap::iterator, EdgeMap::iterator> range =
        edges.equal_range(node);
    EdgeMap::iterator it = range.first;
    while (it != range.second) {
      CalcItem* dest = it->second;
      EdgeMap::iterator temp = it;
      it++;
      edges.erase(temp);

      bool has_incoming_edges = false;
      for (EdgeMap::iterator jt = edges.begin(); jt != edges.end(); ++jt) {
        if (jt->second == dest) {
          has_incoming_edges = true;
          break;
        }
      }

      if (!has_incoming_edges)
        queue.push_back(dest);
    }
  }

  if (!edges.empty()) {
    // Dependency graph has a cycle.
    return false;
  }
  return true;
}
