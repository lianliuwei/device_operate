#include "testing/gtest/include/gtest/gtest.h"

#include "device/uint16_split_bind.h"

// assumed on intel little endian
TEST(Uint16SplitBindTest, LittleEndian) {
  uint16 value16 = 0x0102;
  Uint16SplitBind bind16(bit_cast<uint8*, uint16*>(&value16), 0, 1, false);
  EXPECT_EQ(0x0102, bind16.value());
  EXPECT_EQ(value16, 0x0102);
  bind16.set_value(0x0304);
  EXPECT_EQ(0x0304, bind16.value());
  EXPECT_EQ(value16, 0x0304);
}

TEST(Uint16SplitBindTest, IndexLittleEndian) {
  uint32 value32 = 0x01020304;
  Uint16SplitBind bind0(bit_cast<uint8*, uint32*>(&value32), 0, 2, false);
  Uint16SplitBind bind1(bit_cast<uint8*, uint32*>(&value32), 1, 3, false);
  EXPECT_EQ(bind0.value(), 0x0204);
  EXPECT_EQ(bind1.value(), 0x0103);
  EXPECT_EQ(value32, 0x01020304);
  bind0.set_value(0x0608);
  bind1.set_value(0x0507);
  EXPECT_EQ(bind0.value(), 0x0608);
  EXPECT_EQ(bind1.value(), 0x0507);
  EXPECT_EQ(value32, 0x05060708);
}
// assumed on intel little endian
TEST(Uint16SplitBindTest, BigEndian) {
  uint16 value16 = 0x0201;
  Uint16SplitBind bind16(bit_cast<uint8*, uint16*>(&value16), 0, 1, true);
  EXPECT_EQ(0x0102, bind16.value());
  EXPECT_EQ(value16, 0x0201);
  bind16.set_value(0x0304);
  EXPECT_EQ(0x0304, bind16.value());
  EXPECT_EQ(value16, 0x0403);
}

TEST(Uint16SplitBindTest, IndexBigEndian) {
  uint8 value32[] = {0x01, 0x02, 0x03, 0x04};
  Uint16SplitBind bind0(value32, 0, 2, true);
  Uint16SplitBind bind1(value32, 1, 3, true);
  EXPECT_EQ(bind0.value(), 0x0103);
  EXPECT_EQ(bind1.value(), 0x0204);
  uint32 value = *bit_cast<uint32*, uint8*>(value32);
  EXPECT_EQ(value, 0x04030201);
  bind0.set_value(0x0507);
  bind1.set_value(0x0608);
  EXPECT_EQ(bind0.value(), 0x0507);
  EXPECT_EQ(bind1.value(), 0x0608);
  value = *bit_cast<uint32*, uint8*>(value32);
  EXPECT_EQ(value, 0x08070605);
}
