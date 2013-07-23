#include <vector>

#include "testing/gtest/include/gtest/gtest.h"

#include "device/one_byte_type_bind.h"


using namespace std;

TEST(Uint8BindTest, ReadTest) {
  vector<uint8> memory;
  for (int i = 0; i < 10; ++i) {
    memory.push_back(static_cast<uint8>(i));
  }
  vector<Uint8Bind*> binds;
  for (int i = 0; i < 10; ++i) {
    binds.push_back(new Uint8Bind(&memory[i]));
  }
  for (int i = 0; i < 10; ++i) {
    EXPECT_EQ(binds[i]->value(), static_cast<uint8>(i));
  }
  for (int i = 0; i < 10; ++i) {
    delete binds[i];
  }
}

TEST(Uint8BindTest, WriteTest) {
  vector<uint8> memory;
  memory.resize(10);
  vector<Uint8Bind*> binds;
  for (int i = 0; i < 10; ++i) {
    binds.push_back(new Uint8Bind(&memory[i]));
  }
  for (int i = 0; i < 10; ++i) {
    binds[i]->set_value(static_cast<uint8>(i));
  }
  for (int i = 0; i < 10; ++i) {
    EXPECT_EQ(binds[i]->value(), static_cast<uint8>(i));
  }
  for (int i = 0; i < 10; ++i) {
    EXPECT_EQ(memory[i], static_cast<uint8>(i));
  }
  for (int i = 0; i < 10; ++i) {
    delete binds[i];
  }
}