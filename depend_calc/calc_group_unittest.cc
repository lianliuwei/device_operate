#include "testing/gtest/include/gtest/gtest.h"

#include <string>

#include "base/file_util.h"
#include "base/files/file_path.h"
#include "depend_calc/calc_group.h"

using namespace std;
using namespace base;

namespace {
static const char* kGroupName = "TestGroup";
static const char* kItem1Name = "Item1";
static const char* kItem2Name = "Item2";
static const char* kItem3Name = "Item3";
static const char* kItem4Name = "Item4";

void VectorEqual(const vector<CalcItem*>& left, 
                 CalcItem* right[], size_t right_size) {
  EXPECT_EQ(right_size, left.size());
  for (size_t i = 0; i < right_size; ++i) {
    vector<CalcItem*>::const_iterator it = find(left.begin(), left.end(), right[i]);
    EXPECT_NE(left.end(), it);
  }
}
}

TEST(CalcGroupTest, NULLItem) {
  string name(kGroupName);
  CalcGroup group(name);
}

TEST(CalcGroupTest, OneItem) {
  string name(kGroupName);
  CalcGroup group(name);
  CalcItem* item = new CalcItem(kItem1Name, const_cast<char*>(kItem1Name));
  group.AddCalcItem(item);
}

// item2 <- item1
TEST(CalcGroupTest, TwoItem) {
  string name(kGroupName);
  CalcGroup group(name);
  CalcItem* item1 = new CalcItem(kItem1Name, const_cast<char*>(kItem1Name));
  group.AddCalcItem(item1);
  CalcItem* item2 = new CalcItem(kItem2Name, const_cast<char*>(kItem2Name));
  group.AddCalcItem(item2);
  group.SetDepend(item2, item1);
  
  vector<CalcItem*> free_node = group.FreeNode();
  {
  CalcItem* expect_items[] = { item2 };
  VectorEqual(free_node, expect_items, arraysize(expect_items));
  }

   free_node = group.NodeSuccess(item2);
  {
  CalcItem* expect_items[] = { item1 };
  VectorEqual(free_node, expect_items, arraysize(expect_items));
  }
}

 // item2 <- item1 item2 <- item3
TEST(CalcGroupTest, ThreeForkItem) {
  string name(kGroupName);
  CalcGroup group(name);
  CalcItem* item1 = new CalcItem(kItem1Name, const_cast<char*>(kItem1Name));
  group.AddCalcItem(item1);
  CalcItem* item2 = new CalcItem(kItem2Name, const_cast<char*>(kItem2Name));
  group.AddCalcItem(item2);
  CalcItem* item3 = new CalcItem(kItem3Name, const_cast<char*>(kItem3Name));
  group.AddCalcItem(item3);
  group.SetDepend(item2, item1);
  group.SetDepend(item2, item3);

  vector<CalcItem*> free_node = group.FreeNode();
  {
  CalcItem* expect_items[] = { item2 };
  VectorEqual(free_node, expect_items, arraysize(expect_items));
  }

  free_node = group.NodeSuccess(item2);
  {
  CalcItem* expect_items[] = { item1, item3 };
  VectorEqual(free_node, expect_items, arraysize(expect_items));
  }
  free_node = group.NodeSuccess(item1);
  {
  VectorEqual(free_node, NULL, 0);
  }
  free_node = group.NodeSuccess(item3);
  {
  VectorEqual(free_node, NULL, 0);
  }
}

 // item2 <- item1 item2 <- item3
TEST(CalcGroupTest, ThreeCombineItem) {
  string name(kGroupName);
  CalcGroup group(name);
  CalcItem* item1 = new CalcItem(kItem1Name, const_cast<char*>(kItem1Name));
  group.AddCalcItem(item1);
  CalcItem* item2 = new CalcItem(kItem2Name, const_cast<char*>(kItem2Name));
  group.AddCalcItem(item2);
  CalcItem* item3 = new CalcItem(kItem3Name, const_cast<char*>(kItem3Name));
  group.AddCalcItem(item3);
  group.SetDepend(item1, item2);
  group.SetDepend(item3, item2);

  vector<CalcItem*> free_node = group.FreeNode();
  {
  CalcItem* expect_items[] = { item1, item3 };
  VectorEqual(free_node, expect_items, arraysize(expect_items));
  }

  free_node = group.NodeSuccess(item1);
  {
  VectorEqual(free_node, NULL, 0);
  }

  free_node = group.NodeSuccess(item3);
  {
  CalcItem* expect_items[] = { item2 };
  VectorEqual(free_node, expect_items, arraysize(expect_items));
  }
  free_node = group.NodeSuccess(item2);
  {
  VectorEqual(free_node, NULL, 0);
  }
}

 // item2 <- item1 item2 <- item3
TEST(CalcGroupTest, CloneGroup) {
  string name(kGroupName);
  CalcGroup group(name);
  CalcItem* item1 = new CalcItem(kItem1Name, const_cast<char*>(kItem1Name));
  group.AddCalcItem(item1);
  CalcItem* item2 = new CalcItem(kItem2Name, const_cast<char*>(kItem2Name));
  group.AddCalcItem(item2);
  CalcItem* item3 = new CalcItem(kItem3Name, const_cast<char*>(kItem3Name));
  group.AddCalcItem(item3);
  group.SetDepend(item1, item2);
  group.SetDepend(item3, item2);

  scoped_ptr<CalcGroup> clone_group(group.Clone());
  CalcItem* clone_item1 = clone_group->GetCalcItem(const_cast<char*>(kItem1Name));
  EXPECT_NE(clone_item1, item1);
  EXPECT_EQ(clone_item1->name(), item1->name());

  CalcItem* clone_item2 = clone_group->GetCalcItem(const_cast<char*>(kItem2Name));
  EXPECT_NE(clone_item2, item2);
  EXPECT_EQ(clone_item2->name(), item2->name());

  CalcItem* clone_item3 = clone_group->GetCalcItem(const_cast<char*>(kItem3Name));
  EXPECT_NE(clone_item3, item3);
  EXPECT_EQ(clone_item3->name(), item3->name());
}

// item1 <- item2 item2 <- item3 item2 <- item4
TEST(CalcGroupTest, FourForkItem) {
  string name(kGroupName);
  CalcGroup group(name);
  CalcItem* item1 = new CalcItem(kItem1Name, const_cast<char*>(kItem1Name));
  group.AddCalcItem(item1);
  CalcItem* item2 = new CalcItem(kItem2Name, const_cast<char*>(kItem2Name));
  group.AddCalcItem(item2);
  CalcItem* item3 = new CalcItem(kItem3Name, const_cast<char*>(kItem3Name));
  group.AddCalcItem(item3);
  CalcItem* item4 = new CalcItem(kItem4Name, const_cast<char*>(kItem4Name));
  group.AddCalcItem(item4);
  group.SetDepend(item1, item2);
  group.SetDepend(item2, item3);
  group.SetDepend(item2, item4);

  vector<CalcItem*> free_node = group.FreeNode();
  {
  CalcItem* expect_items[] = { item1 };
  VectorEqual(free_node, expect_items, arraysize(expect_items));
  }

  free_node = group.NodeSuccess(item1);
  {
  CalcItem* expect_items[] = { item2 };
  VectorEqual(free_node, expect_items, arraysize(expect_items));
  }

  free_node = group.NodeSuccess(item2);
  {
  CalcItem* expect_items[] = { item3, item4 };
  VectorEqual(free_node, expect_items, arraysize(expect_items));
  }
  free_node = group.NodeSuccess(item3);
  {
  VectorEqual(free_node, NULL, 0);
  }
  free_node = group.NodeSuccess(item4);
  {
  VectorEqual(free_node, NULL, 0);
  }
}

// item1 <- item2 item2 <- item4 item3 <- item4
TEST(CalcGroupTest, FourCombineItem) {
  string name(kGroupName);
  CalcGroup group(name);
  CalcItem* item1 = new CalcItem(kItem1Name, const_cast<char*>(kItem1Name));
  group.AddCalcItem(item1);
  CalcItem* item2 = new CalcItem(kItem2Name, const_cast<char*>(kItem2Name));
  group.AddCalcItem(item2);
  CalcItem* item3 = new CalcItem(kItem3Name, const_cast<char*>(kItem3Name));
  group.AddCalcItem(item3);
  CalcItem* item4 = new CalcItem(kItem4Name, const_cast<char*>(kItem4Name));
  group.AddCalcItem(item4);
  group.SetDepend(item1, item2);
  group.SetDepend(item2, item4);
  group.SetDepend(item3, item4);

  vector<CalcItem*> free_node = group.FreeNode();
  {
  CalcItem* expect_items[] = { item1, item3 };
  VectorEqual(free_node, expect_items, arraysize(expect_items));
  }

  free_node = group.NodeSuccess(item1);
  {
  CalcItem* expect_items[] = { item2 };
  VectorEqual(free_node, expect_items, arraysize(expect_items));
  }

  free_node = group.NodeSuccess(item2);
  {
  VectorEqual(free_node, NULL, 0);
  }
  free_node = group.NodeSuccess(item3);
  {
  CalcItem* expect_items[] = { item4 };
  VectorEqual(free_node, expect_items, arraysize(expect_items));
  }
  free_node = group.NodeSuccess(item4);
  {
  VectorEqual(free_node, NULL, 0);
  }
}

// item1 <- item2 item2 <- item3 item2 <- item4
TEST(CalcGroupTest, FourForkItemFailed) {
  string name(kGroupName);
  CalcGroup group(name);
  CalcItem* item1 = new CalcItem(kItem1Name, const_cast<char*>(kItem1Name));
  group.AddCalcItem(item1);
  CalcItem* item2 = new CalcItem(kItem2Name, const_cast<char*>(kItem2Name));
  group.AddCalcItem(item2);
  CalcItem* item3 = new CalcItem(kItem3Name, const_cast<char*>(kItem3Name));
  group.AddCalcItem(item3);
  CalcItem* item4 = new CalcItem(kItem4Name, const_cast<char*>(kItem4Name));
  group.AddCalcItem(item4);
  group.SetDepend(item1, item2);
  group.SetDepend(item2, item3);
  group.SetDepend(item2, item4);

  vector<CalcItem*> free_node = group.FreeNode();
  {
  CalcItem* expect_items[] = { item1 };
  VectorEqual(free_node, expect_items, arraysize(expect_items));
  }

  group.NodeFailed(item1);
  free_node = group.FreeNode();
  {
  VectorEqual(free_node, NULL, 0);
  }
}

// item1 <- item2 item2 <- item4 item3 <- item4
TEST(CalcGroupTest, FourCombineItemFailed) {
  string name(kGroupName);
  CalcGroup group(name);
  CalcItem* item1 = new CalcItem(kItem1Name, const_cast<char*>(kItem1Name));
  group.AddCalcItem(item1);
  CalcItem* item2 = new CalcItem(kItem2Name, const_cast<char*>(kItem2Name));
  group.AddCalcItem(item2);
  CalcItem* item3 = new CalcItem(kItem3Name, const_cast<char*>(kItem3Name));
  group.AddCalcItem(item3);
  CalcItem* item4 = new CalcItem(kItem4Name, const_cast<char*>(kItem4Name));
  group.AddCalcItem(item4);
  group.SetDepend(item1, item2);
  group.SetDepend(item2, item4);
  group.SetDepend(item3, item4);

  vector<CalcItem*> free_node = group.FreeNode();
  {
  CalcItem* expect_items[] = { item1, item3 };
  VectorEqual(free_node, expect_items, arraysize(expect_items));
  }
  group.NodeFailed(item3);
  free_node = group.FreeNode();
  {
  CalcItem* expect_items[] = { item1 };
  VectorEqual(free_node, expect_items, arraysize(expect_items));
  }
  free_node = group.NodeSuccess(item1);
  {
  CalcItem* expect_items[] = { item2 };
  VectorEqual(free_node, expect_items, arraysize(expect_items));
  }
}

TEST(CalcGroupTest, Graphviz) {
  string name(kGroupName);
  CalcGroup group(name);
  CalcItem* item1 = new CalcItem(kItem1Name, const_cast<char*>(kItem1Name));
  group.AddCalcItem(item1);
  CalcItem* item2 = new CalcItem(kItem2Name, const_cast<char*>(kItem2Name));
  group.AddCalcItem(item2);
  CalcItem* item3 = new CalcItem(kItem3Name, const_cast<char*>(kItem3Name));
  group.AddCalcItem(item3);
  CalcItem* item4 = new CalcItem(kItem4Name, const_cast<char*>(kItem4Name));
  group.AddCalcItem(item4);
  group.SetDepend(item1, item2);
  group.SetDepend(item2, item4);
  group.SetDepend(item3, item4);

  string result = group.DumpAsGraphviz();
  file_util::WriteFile(FilePath(L"calc_group"), result.c_str(), result.length());
}
