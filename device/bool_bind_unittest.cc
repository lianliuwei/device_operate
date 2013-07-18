#include "device/bool_bind.h"

#include "testing/gtest/include/gtest/gtest.h"

TEST(BoolBindTest, TestBoolBind0) {
  uint8 memory = 0x0;
  BoolBind bind0(&memory, 0, true);
  EXPECT_EQ(bind0.value(), false);
  bind0.set_value(true);
  EXPECT_EQ(bind0.value(), true);
  EXPECT_EQ(0x1, memory);
  bind0.set_value(false);
  EXPECT_EQ(bind0.value(), false);
  EXPECT_EQ(0x0, memory);
}

TEST(BoolBindTest, TestToggle) {
  uint8 memory = 0x0;
  BoolBind bind0(&memory, 0, true);
  EXPECT_EQ(bind0.value(), false);
  bind0.set_value(true);
  EXPECT_EQ(bind0.value(), true);
  EXPECT_EQ(0x1, memory);
  bind0.Toggle();
  EXPECT_EQ(bind0.value(), false);
  EXPECT_EQ(0x0, memory);
}

TEST(BoolBindTest, TestBoolBind0Invert) {
  uint8 memory = 0x0;
  BoolBind bind0(&memory, 0, false);
  EXPECT_EQ(bind0.value(), true);
  bind0.set_value(false);
  EXPECT_EQ(bind0.value(), false);
  EXPECT_EQ(0x1, memory);
  bind0.set_value(true);
  EXPECT_EQ(bind0.value(), true);
  EXPECT_EQ(0x0, memory);
}

TEST(BoolBindTest, TestByteSize) {
  EXPECT_EQ(BoolBind::kByteSize, 1);
}

TEST(BoolBindTest, TestBoolBind7) {
  uint8 memory = 0x0;
  BoolBind bind7(&memory, 7, true);
  EXPECT_EQ(bind7.value(), false);
  bind7.set_value(true);
  EXPECT_EQ(bind7.value(), true);
  EXPECT_EQ(0x80, memory);
  bind7.set_value(false);
  EXPECT_EQ(bind7.value(), false);
  EXPECT_EQ(0x0, memory);
}

TEST(BoolBindTest, TestBoolBind7Invert) {
    uint8 memory = 0x0;
  BoolBind bind7(&memory, 7, false);
  EXPECT_EQ(bind7.value(), true);
  bind7.set_value(false);
  EXPECT_EQ(bind7.value(), false);
  EXPECT_EQ(0x80, memory);
  bind7.set_value(true);
  EXPECT_EQ(bind7.value(), true);
  EXPECT_EQ(0x0, memory);
}

TEST(BoolBindTest, TestBindTwo) {
  uint8 memory = 0x0;
  BoolBind bind0(&memory, 0, true);
  BoolBind bind5(&memory, 5, true);
  BoolBind bind7(&memory, 7, true);

  EXPECT_EQ(bind0.value(), false);
  EXPECT_EQ(bind5.value(), false);
  EXPECT_EQ(bind7.value(), false);
  bind0.set_value(true);
  EXPECT_EQ(bind0.value(), true);
  EXPECT_EQ(bind5.value(), false);
  EXPECT_EQ(bind7.value(), false);
  EXPECT_EQ(0x1, memory);
  bind5.set_value(true);
  EXPECT_EQ(bind0.value(), true);
  EXPECT_EQ(bind5.value(), true);
  EXPECT_EQ(bind7.value(), false);
  EXPECT_EQ(0x21, memory);
  bind7.set_value(true);
  EXPECT_EQ(bind0.value(), true);
  EXPECT_EQ(bind5.value(), true);
  EXPECT_EQ(bind7.value(), true);
  EXPECT_EQ(0xA1, memory);
  bind0.set_value(false);
  EXPECT_EQ(bind0.value(), false);
  EXPECT_EQ(bind5.value(), true);
  EXPECT_EQ(bind7.value(), true);
  EXPECT_EQ(0xA0, memory);
  bind5.set_value(false);
  EXPECT_EQ(bind0.value(), false);
  EXPECT_EQ(bind5.value(), false);
  EXPECT_EQ(bind7.value(), true);
  EXPECT_EQ(0x80, memory);
  bind7.set_value(false);
  EXPECT_EQ(bind0.value(), false);
  EXPECT_EQ(bind5.value(), false);
  EXPECT_EQ(bind7.value(), false);
  EXPECT_EQ(0x00, memory);
}