#include "testing/gtest/include/gtest/gtest.h"

#include "base/basictypes.h"

#include "canscope/device/register/sja1000_register.h"

using namespace canscope;

TEST(SJA1000RegisterTest, SetContentAndCheckProperty) {
  uint8 sja100_content[0x18] 
     = {0x00, 0x14, // btr
        0x01, // slient
        0x00, // reserved
        0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x07, 0x08,
        0x09, 0x0A, 0x0B, 0x0C,
        0x0D, // frame_data
        0x0, 0x0, 0x0, // reserved
        0x12, // alc
        0x13, // ecc
        0xD2, // int
        0x80,
  };
  SJA1000Register sja100;
  memcpy(sja100.memory.buffer(), sja100_content, 
    arraysize(sja100_content));
  
  EXPECT_EQ(0x1400, sja100.sja_btr.value());
  EXPECT_EQ(true, sja100.slient.value());
  FrameData data = sja100.frame_data();
  for (int i = 0; i < 13; ++i) {
    EXPECT_EQ(data.data[i], i + 1);
  }
  EXPECT_EQ(true, sja100.int_bus.value());
  EXPECT_EQ(true, sja100.int_send.value());
  EXPECT_EQ(true, sja100.int_alc.value());
  EXPECT_EQ(0x12, sja100.s_alc.value());
  EXPECT_EQ(0x13, sja100.s_ecc.value());
  EXPECT_EQ(true, sja100.cfg.value());
  EXPECT_EQ(true, sja100.sending.value());
  EXPECT_EQ(0x80D21312, sja100.send_state.value());
}