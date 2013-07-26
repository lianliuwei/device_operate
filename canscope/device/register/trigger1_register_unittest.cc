#include "testing/gtest/include/gtest/gtest.h"

#include "base/basictypes.h"

#include "canscope/device/register/trigger1_register.h"

using namespace canscope;

TEST(Trigger1RegisterTest, SetContentAndCheckProperty) {
  uint8 trigger1_content[0x10] 
      = {0x01, 0x02, 0x03, 0x04,
         0x05, 0x06, 0x07, 0x08,
         0x09, 0x0A, 0x0B, 0x0C,
         0x0D, 0x0E, 0x0F, 0x10,
      };
  Trigger1Register trigger1;
  memcpy(trigger1.memory.buffer(), trigger1_content, 
    arraysize(trigger1_content));

  EXPECT_EQ(0x04030201, trigger1.trig_pre.value());
  EXPECT_EQ(0x08070605, trigger1.trig_post.value());
  EXPECT_EQ(0x0C0B0A09, trigger1.auto_time.value());
  EXPECT_EQ(0x100F0E0D, trigger1.div_coe.value());
}
