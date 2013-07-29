#include "testing/gtest/include/gtest/gtest.h"

#include "base/basictypes.h"

#include "canscope/device/register/trigger_state_register.h"

using namespace canscope;

TEST(TriggerStateRegisterTest, SetContentAndCheckProperty) {
  uint8 trigger_state_content[0x10] 
      = {0x0D, // 1101 0000
         0xFF, 0xFF, 0xFF, // reserve
         0xFF, 0xFF, 0xFF, 0xFF, // reserve
         0xFF, 0xFF, 0xFF, 0xFF, // reserve
      };
  TriggerStateRegister trigger_state;
  memcpy(trigger_state.memory.buffer(), trigger_state_content, 
    arraysize(trigger_state_content));
  EXPECT_EQ(true, trigger_state.clet_bit.value());
  EXPECT_EQ(false, trigger_state.trig_bit.value());
  EXPECT_EQ(true, trigger_state.pre_bit.value());
}
