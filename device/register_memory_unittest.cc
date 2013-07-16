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