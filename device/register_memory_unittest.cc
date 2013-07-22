#include "device/register_memory.h"

#include "testing/gtest/include/gtest/gtest.h"

using namespace device;

TEST(RegisterMemoryTest, Normal) {
  RegisterMemory memory(0x900010, 100);
  uint8* ptr = memory.buffer();
  for (int i = 0; i < 100; ++i) {
    ptr[i] = static_cast<uint8>(i);
  }
  EXPECT_EQ(*memory.PtrByRelative(80), 80);
  EXPECT_EQ(*memory.PtrByAbsoulute(0x900010 + 80) , 80);
  EXPECT_EQ(*memory.PtrByRelative(0), 0);
  EXPECT_EQ(*memory.PtrByAbsoulute(0x900010) , 0);
  EXPECT_EQ(*memory.PtrByRelative(16), 16);
  EXPECT_EQ(*memory.PtrByAbsoulute(0x900020) , 16);
}

TEST(MemoryContentTest, Normal) {
  RegisterMemory memory(0x900010, 100);
  uint8* ptr = memory.buffer();
  for (int i = 0; i < 100; ++i) {
    ptr[i] = static_cast<uint8>(i);
  }
  MemoryContent content(memory);
  EXPECT_TRUE(content.HasContent());
  EXPECT_EQ(content.size(), 100);
  EXPECT_EQ(*(content.buffer() + 80), 80);
  EXPECT_EQ(*(content.buffer()), 0);
  EXPECT_EQ(*(content.buffer() + 16), 16);
  // no change
  ptr[80] = 100;
  ptr[16] = 100;
  ptr[0] = 100;
  EXPECT_EQ(*memory.PtrByRelative(80), 100);
  EXPECT_EQ(*memory.PtrByRelative(0), 100);
  EXPECT_EQ(*memory.PtrByRelative(16), 100);
  EXPECT_EQ(*(content.buffer() + 80), 80);
  EXPECT_EQ(*(content.buffer()), 0);
  EXPECT_EQ(*(content.buffer() + 16), 16);
  // set back to content
  content.SetMemory(&memory);
  EXPECT_EQ(*memory.PtrByRelative(80), 80);
  EXPECT_EQ(*memory.PtrByRelative(0), 0);
  EXPECT_EQ(*memory.PtrByRelative(16), 16);
}