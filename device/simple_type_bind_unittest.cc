#include "testing/gtest/include/gtest/gtest.h"

#include "device/simple_type_bind.h"

TEST(SimpleTypeBindTest, Size) {
  EXPECT_EQ(1, Uint8Bind::kByteSize);
  EXPECT_EQ(2, Uint16Bind::kByteSize);
  EXPECT_EQ(4, Uint32Bind::kByteSize);
  EXPECT_EQ(8, Uint64Bind::kByteSize);
  EXPECT_EQ(1, CharBind::kByteSize);
  EXPECT_EQ(1, Int8Bind::kByteSize);
  EXPECT_EQ(2, Int16Bind::kByteSize);
  EXPECT_EQ(4, Int32Bind::kByteSize);
  EXPECT_EQ(8, Int64Bind::kByteSize);
  EXPECT_EQ(4, FloatBind::kByteSize);
  EXPECT_EQ(8, DoubleBind::kByteSize);
}

TEST(SimpleTypeBindTest, Uint16BindTest) {
  uint8 content[] = {0x01, 0x02};
  Uint16Bind little(content, false);
  EXPECT_EQ(0x0201, little.value());
  Uint16Bind big(content, true);
  EXPECT_EQ(0x0102, big.value());
  little.set_value(0x0304);
  EXPECT_EQ(content[0], 0x04);
  EXPECT_EQ(content[1], 0x03);
  EXPECT_EQ(little.value(), 0x0304);
  EXPECT_EQ(big.value(), 0x0403);
  big.set_value(0x0506);
  EXPECT_EQ(content[0], 0x05);
  EXPECT_EQ(content[1], 0x06);
  EXPECT_EQ(big.value(), 0x0506);
  EXPECT_EQ(little.value(), 0x0605);
}

TEST(SimpleTypeBindTest, Uint32BindTest) {
  uint8 content[] = {0x01, 0x02, 0x03, 0x04};
  Uint32Bind little(content, false);
  EXPECT_EQ(0x04030201, little.value());
  Uint32Bind big(content, true);
  EXPECT_EQ(0x01020304, big.value());
  little.set_value(0x05060708);
  EXPECT_EQ(content[0], 0x08);
  EXPECT_EQ(content[1], 0x07);
  EXPECT_EQ(content[2], 0x06);
  EXPECT_EQ(content[3], 0x05);
  EXPECT_EQ(little.value(), 0x05060708);
  EXPECT_EQ(big.value(), 0x08070605);
  big.set_value(0x090A0B0C);
  EXPECT_EQ(content[0], 0x09);
  EXPECT_EQ(content[1], 0x0A);
  EXPECT_EQ(content[2], 0x0B);
  EXPECT_EQ(content[3], 0x0C);
  EXPECT_EQ(big.value(), 0x090A0B0C);
  EXPECT_EQ(little.value(), 0x0C0B0A09);
}