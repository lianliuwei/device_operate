#pragma once

#include <map>
#include <string>
#include <vector>

#include "base/callback.h"
#include "base/gtest_prod_util.h"

#include "depend_calc/calc_item.h"

class CalcGroupWalker;

class CalcGroup {
public:
  friend class CalcGroupWalker;

  CalcGroup(const std::string& name);
  ~CalcGroup();

  // take ownership, item can not be add twice
  void AddCalcItem(CalcItem* item);
  void RemoveCalcItem(CalcItem* item);
  bool HasCalcItem(CalcItem* item) const;
  bool HasCalcItem(CalcKey key) const;
  CalcItem* GetCalcItem(CalcKey key) const;

  // cycle Depend will break
  // er <- ee
  void SetDepend(CalcItem* er, CalcItem* ee);
  void SetDepend(CalcKey er, CalcKey ee);

  bool HasCycle();

  // pass out ownership CalcGroup
  CalcGroup* Clone() const;

  std::string name() const { return name_; }

  // Returns representation of the dependency graph in graphviz format.
  std::string DumpAsGraphviz(
      const std::string& toplevel_name,
      const base::Callback<std::string(CalcItem*)>&
      node_name_callback) const;

  std::string DumpAsGraphviz() const;

private:
  FRIEND_TEST_ALL_PREFIXES(CalcGroupTest, TwoItem);
  FRIEND_TEST_ALL_PREFIXES(CalcGroupTest, ThreeForkItem);
  FRIEND_TEST_ALL_PREFIXES(CalcGroupTest, ThreeCombineItem);
  FRIEND_TEST_ALL_PREFIXES(CalcGroupTest, FourForkItem);
  FRIEND_TEST_ALL_PREFIXES(CalcGroupTest, FourCombineItem);
  FRIEND_TEST_ALL_PREFIXES(CalcGroupTest, FourForkItemFailed);
  FRIEND_TEST_ALL_PREFIXES(CalcGroupTest, FourCombineItemFailed);

  std::vector<CalcItem*> FreeNode() const;

  // return all Node depend only on node, and remove node
  // node must be free node
  std::vector<CalcItem*> NodeSuccess(CalcItem* node);
  
  // remove node and item depend on node
  // node must be free node
  void NodeFailed(CalcItem* node);

  // node is isolate, delete it from all node
  void DeleteIsolateNode(CalcItem* node);

  typedef std::multimap<CalcItem*, CalcItem*> EdgeMap;

  // Keeps track of all live nodes.
  std::vector<CalcItem*> all_nodes_;

  // Keeps track of edges of the dependency graph.
  EdgeMap edges_;

  std::string name_;

  DISALLOW_COPY_AND_ASSIGN(CalcGroup);
};
