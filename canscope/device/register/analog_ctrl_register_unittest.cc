#include "testing/gtest/include/gtest/gtest.h"

#include "base/basictypes.h"

#include "canscope/device/register/analog_ctrl_register.h"

using namespace canscope;

TEST(AnalogCtrlRegisterTest, SetContentAndCheckProperty) {
  uint8 analog_ctrl_content[0x18] 
     = {0x0, 0X1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, // reserve
        0x01, 0x02, 0x03, 0x04, // CAN-L GAIN
        0x05, 0x06, 0x07, 0x08, // CAN-L OFFSET
        0x09, 0x0A, 0x0B, 0x0C, // CAN-H GAIN
        0x0D, 0x0E, 0x0F, 0x10, // CAN-H OFFSET
  };
  AnalogCtrlRegister analog_ctrl;
  memcpy(analog_ctrl.memory.buffer(), analog_ctrl_content, 
    arraysize(analog_ctrl_content));
  
  EXPECT_EQ(0x04030201, analog_ctrl.can_l_gain.value());
  EXPECT_EQ(0x08070605, analog_ctrl.can_l_offset.value());
  EXPECT_EQ(0x0C0B0A09, analog_ctrl.can_h_gain.value());
  EXPECT_EQ(0x100F0E0D, analog_ctrl.can_h_offset.value());
}