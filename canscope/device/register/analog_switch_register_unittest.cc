#include "testing/gtest/include/gtest/gtest.h"

#include "base/basictypes.h"

#include "canscope/device/register/analog_switch_register.h"

using namespace canscope;

TEST(AnalogSwitchRegisterTest, SetContentAndCheckProperty) {
  uint8 analog_switch_content[0x4] 
      = {0xA, // 0000 1010
         0X1, 0x2, 0x3,  // reserve
      };
  AnalogSwitchRegister analog_switch;
  memcpy(analog_switch.memory.buffer(), analog_switch_content, 
    arraysize(analog_switch_content));

  EXPECT_EQ(false, analog_switch.coupling_canl.value());
  EXPECT_EQ(true, analog_switch.coupling_canh.value());
  EXPECT_EQ(false, analog_switch.attenuation_canl.value());
  EXPECT_EQ(true, analog_switch.attenuation_canh.value());
}
