#include "testing/gtest/include/gtest/gtest.h"

#include "base/basictypes.h"

#include "canscope/device/register/trigger2_register.h"

using namespace canscope;

TEST(Trigger2RegisterTest, SetContentAndCheckProperty) {
  uint8 trigger2_content[0x10] 
      = {0xFD, // 11111 101 // trig_source
         0xFF, 0xFF, 0xFF, // reserve
         0xAB, // cmp_low
         0xCD, // cmp_high
         0xFF, 0xFF, // reserve
         0xFA, // 11111 010 trig_type
         0x0A, 0x0B, // reserve
         0x80, // 1 0000000 trig_mode
         0x0D, 0x0E, 0x0F, 0xBD, // 10 11... compare trig_time
      };
  Trigger2Register trigger2;
  memcpy(trigger2.memory.buffer(), trigger2_content, 
    arraysize(trigger2_content));
  EXPECT_EQ(0x5, trigger2.trig_source.value());
  EXPECT_EQ(0xAB, trigger2.cmp_low.value());
  EXPECT_EQ(0xCD, trigger2.cmp_high.value());
  EXPECT_EQ(0x2, trigger2.trig_type.value());
  EXPECT_EQ(0x1, trigger2.trig_mode.value());
  EXPECT_EQ(0x2, trigger2.compare.value());
  EXPECT_EQ(0x3D0F0E0D, trigger2.trig_time.value());
  
}
