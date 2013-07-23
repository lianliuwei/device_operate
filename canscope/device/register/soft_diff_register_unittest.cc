#include "testing/gtest/include/gtest/gtest.h"

#include "base/basictypes.h"

#include "canscope/device/register/soft_diff_register.h"

using namespace canscope;

TEST(SoftDiffRegisterTest, SetContentAndCheckProperty) {
  uint8 soft_diff_content[0x1C] 
      = {0x0, 0X1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 
         4, // CAN-H sens
         16, // CAN-L sens
         80, // CAN-diff sens
         0, // reserve
         0x2, // CAN-H zero l
         0x4, // CAN-L zero l
         0x6, // CAN-DIFF zero l
         0, // reserve
         1, // diff-ctrl
         2, 3, 4, 5, 6, 7, 8,// reserve
         0x1, // CAN-H zero h
         0x3, // CAN-L zero h
         0x5, // CAN-DIFF zero h
         2, // reserve
      };
  SoftDiffRegister soft_diff;
  memcpy(soft_diff.memory.buffer(), soft_diff_content, 
      arraysize(soft_diff_content));
  
  EXPECT_EQ(4, soft_diff.ch_sens_canh.value());
  EXPECT_EQ(16, soft_diff.ch_sens_canl.value());
  EXPECT_EQ(80, soft_diff.ch_sens_candiff.value());
  EXPECT_EQ(0x0102, soft_diff.ch_zero_canh.value());
  EXPECT_EQ(0x0304, soft_diff.ch_zero_canl.value());
  EXPECT_EQ(0x0506, soft_diff.ch_zero_candiff.value());
  EXPECT_EQ(true, soft_diff.diff_ctrl.value());

}