#include "device/bool_bind.h"

#include "testing/gtest/include/gtest/gtest.h"

TEST(BoolBindTest, TestBoolBind0) {
  char memory = 0x0;
  BoolBind0 bind0(&memory);
  EXPECT_EQ(bind0.value(), false);
  bind0.set_value(true);
  EXPECT_EQ(bind0.value(), true);
  EXPECT_EQ(0x1, static_cast<uint8>(memory));
  bind0.set_value(false);
  EXPECT_EQ(bind0.value(), false);
  EXPECT_EQ(0x0, static_cast<uint8>(memory));
}

TEST(BoolBindTest, TestBoolBind0Invert) {
  char memory = 0x0;
  BoolBind0Invert bind0(&memory);
  EXPECT_EQ(bind0.value(), true);
  bind0.set_value(false);
  EXPECT_EQ(bind0.value(), false);
  EXPECT_EQ(0x1, static_cast<uint8>(memory));
  bind0.set_value(true);
  EXPECT_EQ(bind0.value(), true);
  EXPECT_EQ(0x0, static_cast<uint8>(memory));
}

TEST(BoolBindTest, TestBoolBind7) {
  char memory = 0x0;
  BoolBind7 bind7(&memory);
  EXPECT_EQ(bind7.value(), false);
  bind7.set_value(true);
  EXPECT_EQ(bind7.value(), true);
  EXPECT_EQ(0x80, static_cast<uint8>(memory));
  bind7.set_value(false);
  EXPECT_EQ(bind7.value(), false);
  EXPECT_EQ(0x0, static_cast<uint8>(memory));
}

TEST(BoolBindTest, TestBoolBind7Invert) {
  char memory = 0x0;
  BoolBind7Invert bind7(&memory);
  EXPECT_EQ(bind7.value(), true);
  bind7.set_value(false);
  EXPECT_EQ(bind7.value(), false);
  EXPECT_EQ(0x80, static_cast<uint8>(memory));
  bind7.set_value(true);
  EXPECT_EQ(bind7.value(), true);
  EXPECT_EQ(0x0, static_cast<uint8>(memory));
}

TEST(BoolBindTest, TestBindTwo) {
  char memory = 0x0;
  BoolBind0 bind0(&memory);
  BoolBind5 bind5(&memory);
  BoolBind7 bind7(&memory);
  EXPECT_EQ(bind0.value(), false);
  EXPECT_EQ(bind5.value(), false);
  EXPECT_EQ(bind7.value(), false);
  bind0.set_value(true);
  EXPECT_EQ(bind0.value(), true);
  EXPECT_EQ(bind5.value(), false);
  EXPECT_EQ(bind7.value(), false);
  EXPECT_EQ(0x1, static_cast<uint8>(memory));
  bind5.set_value(true);
  EXPECT_EQ(bind0.value(), true);
  EXPECT_EQ(bind5.value(), true);
  EXPECT_EQ(bind7.value(), false);
  EXPECT_EQ(0x21, static_cast<uint8>(memory));
  bind7.set_value(true);
  EXPECT_EQ(bind0.value(), true);
  EXPECT_EQ(bind5.value(), true);
  EXPECT_EQ(bind7.value(), true);
  EXPECT_EQ(0xA1, static_cast<uint8>(memory));
  bind0.set_value(false);
  EXPECT_EQ(bind0.value(), false);
  EXPECT_EQ(bind5.value(), true);
  EXPECT_EQ(bind7.value(), true);
  EXPECT_EQ(0xA0, static_cast<uint8>(memory));
  bind5.set_value(false);
  EXPECT_EQ(bind0.value(), false);
  EXPECT_EQ(bind5.value(), false);
  EXPECT_EQ(bind7.value(), true);
  EXPECT_EQ(0x80, static_cast<uint8>(memory));
  bind7.set_value(false);
  EXPECT_EQ(bind0.value(), false);
  EXPECT_EQ(bind5.value(), false);
  EXPECT_EQ(bind7.value(), false);
  EXPECT_EQ(0x00, static_cast<uint8>(memory));
}