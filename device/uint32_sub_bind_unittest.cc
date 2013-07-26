#include "testing/gtest/include/gtest/gtest.h"

#include "device/uint32_sub_bind.h"

TEST(Uint32SubBindTest, OneBitRead) {
  uint32 memory = 0x101;
  uint8* ptr = bit_cast<uint8*, uint32*>(&memory);
  Uint32SubBind bind0(ptr, false, 0, 0);
  EXPECT_EQ(1U, bind0.value());
  Uint32SubBind bind1(ptr, false, 1, 1);
  EXPECT_EQ(0U, bind1.value());
  Uint32SubBind bind8(ptr, false, 8, 8);
  EXPECT_EQ(1U, bind8.value());
}

TEST(Uint32SubBindTest, TwoBitRead) {
  uint32 memory = 0xC0F; // 1100 0000 1111
  uint8* ptr = bit_cast<uint8*, uint32*>(&memory);
  Uint32SubBind bind01(ptr, false, 0, 1);
  EXPECT_EQ(3U, bind01.value());
  Uint32SubBind bind56(ptr, false, 5, 6);
  EXPECT_EQ(0U, bind56.value());
  Uint32SubBind bindab(ptr, false, 0xA, 0xB);
  EXPECT_EQ(3U, bindab.value());
}

TEST(Uint32SubBindTest, AllBitRead) {
  uint32 memory = 0xEFABDC0F;
  uint8* ptr = bit_cast<uint8*, uint32*>(&memory);
  Uint32SubBind bind(ptr, false, 0, 31);
  EXPECT_EQ(0xEFABDC0F, bind.value());
}

TEST(Uint32SubBindTest, MaxValue) {
  uint8* ptr = NULL;
  Uint32SubBind bind1(ptr, true, 10, 10);
  EXPECT_EQ(1U, bind1.MaxValue());
  Uint32SubBind bind2(ptr, true, 2, 3);
  EXPECT_EQ(3U, bind2.MaxValue());
  Uint32SubBind bind3(ptr, true, 4, 6);
  EXPECT_EQ(7U, bind3.MaxValue());
  Uint32SubBind bind4(ptr, true, 12, 15);
  EXPECT_EQ(0xFU, bind4.MaxValue());
  Uint32SubBind bind10(ptr, true, 0, 9);
  EXPECT_EQ(0x3FFU, bind10.MaxValue());
  Uint32SubBind bind32(ptr, true, 0, 31);
  EXPECT_EQ(0xFFFFFFFFU, bind32.MaxValue());

}

TEST(Uint32SubBindTest, BigIndexFirst) {
  uint32 memory = 0xEFABDC0F;
  uint8* ptr = bit_cast<uint8*, uint32*>(&memory);
  Uint32SubBind bind(ptr, false, 31, 0);
  EXPECT_EQ(0xEFABDC0F, bind.value());
}

TEST(Uint32SubBindTest, OneBitWrite) {
  uint32 memory = 0x101;
  uint8* ptr = bit_cast<uint8*, uint32*>(&memory);
  Uint32SubBind bind4(ptr, false, 4, 4);
  bind4.set_value(1);
  EXPECT_EQ(0x111, memory);
}

TEST(Uint32SubBindTest, TwoBitWrite) {
  uint32 memory = 0x101;
  uint8* ptr = bit_cast<uint8*, uint32*>(&memory);
  Uint32SubBind bind45(ptr, false, 4, 5);
  bind45.set_value(0x3);
  EXPECT_EQ(0x131, memory);
  Uint32SubBind bind9a(ptr, false, 8, 9);
  bind9a.set_value(0x2);
  EXPECT_EQ(0x231, memory);
  bind9a.set_value(0x0);
  EXPECT_EQ(0x031, memory);
}

TEST(Uint32SubBindTest, OutRange) {
  uint32 memory = 0x101;
  uint8* ptr = bit_cast<uint8*, uint32*>(&memory);
  Uint32SubBind bind45(ptr, false, 4, 5);
  EXPECT_DEATH(bind45.set_value(0x4), "");
}