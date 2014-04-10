#include "testing/gtest/include/gtest/gtest.h"

#include "base/basictypes.h"

#include "canscope/device/register/frame_storage_register.h"

using namespace canscope;

TEST(FrameStorageRegisterTest, SetContentAndCheckProperty) {
  uint8 trigger_state_content[0xC] 
      = {0x40, 0x1A, 0x00, 0x00, // frame_depth
         0x00, 0x12, 0x34, 0x56, // frame_num
         0x1,
         0x00, 0x00, 0x00, // reserved
      };
  FrameStorageRegister trigger_state;
  memcpy(trigger_state.memory.buffer(), trigger_state_content, 
    arraysize(trigger_state_content));

  EXPECT_EQ(0x1A40, trigger_state.frame_depth.value());
  EXPECT_EQ(0x56341200, trigger_state.frame_num.value());
  EXPECT_EQ(true, trigger_state.overflow.value());
}
