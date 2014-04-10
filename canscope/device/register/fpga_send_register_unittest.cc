#include "testing/gtest/include/gtest/gtest.h"

#include "base/basictypes.h"

#include "canscope/device/register/fpga_send_register.h"

using namespace canscope;

TEST(FpgaSendRegisterTest, SetContentAndCheckProperty) {
  uint8 fpga_send_content[0x3C] 
      = {0x01, 0x02,// btr_div
         0x07, // bit num
         0x08, // send num
         0x10, 0x11, 0x12, 0x13, 
         0x14, 0x15, 0x16, 0x17,
         0x18, 0x19, 0x1A, 0x1B,
         0x1C, 0x1D, 0x1E, 0x1F,
         0x20, 0x21, 0x22, 0x23, // frame_data
         0x33, // ack bit
         0x1, // ack
         0x00, 0x00,
         0x3, // send_state ack_state
         0x00, 0x00, 0x00,
         0x41, 0x42, // dstb_start
         0x43, 0x44, // dstb_end
         0x50, 0x51, 0x52, 0x53, 
         0x54, 0x55, 0x56, 0x57,
         0x58, 0x59, 0x5A, 0x5B,
         0x5C, 0x5D, 0x5E, 0x5F,
         0x60, 0x61, 0x62, 0x63,  // dstb_data
         0x1, // bstb
         0x00, 0x00, 0x00, // reserved
      };
  FpgaSendRegister fpga_send;
  memcpy(fpga_send.memory.buffer(), fpga_send_content, 
    arraysize(fpga_send_content));

  EXPECT_EQ(0x0201, fpga_send.btr_div.value());
  EXPECT_EQ(0x07, fpga_send.bit_num.value());
  EXPECT_EQ(0x08, fpga_send.send_num.value());
  FpgaFrameData frame;
  frame = fpga_send.frame_data();
  for (int i = 0; i < 20; ++i) {
    EXPECT_EQ(i + 0x10, frame.data[i]);
  }
  EXPECT_EQ(0x33, fpga_send.ack_bit.value());
  EXPECT_EQ(true, fpga_send.ack.value());
  EXPECT_EQ(0x4241, fpga_send.dstb_start.value());
  EXPECT_EQ(0x4443, fpga_send.dstb_end.value());
  FpgaDstbData data;
  data = fpga_send.dstb_data();
  for (int i = 0; i < 20; ++i) {
    EXPECT_EQ(i + 0x50, data.data[i]);
  }
  EXPECT_EQ(true, fpga_send.dstb.value());
}
