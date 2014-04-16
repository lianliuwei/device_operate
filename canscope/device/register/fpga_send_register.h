#pragma once

#include "device/bool_bind.h"
#include "device/simple_type_bind.h"
#include "device/uint16_split_bind.h"
#include "device/register_memory.h"

struct FpgaFrameData {
  uint8 data[20];
};

typedef FpgaFrameData FpgaDstbData;

namespace canscope {

class FpgaSendRegister {
public:
  ::device::RegisterMemory memory;
  
  Uint8Bind send_num;
  Uint8Bind bit_num;
  Uint16Bind btr_div;

  FpgaFrameData frame_data() const;
  void set_frame_data(const FpgaFrameData& data);

  BoolBind ack;
  Uint8Bind ack_bit;

  BoolBind start_send;
  BoolBind send_state;
  BoolBind set_ack;
  BoolBind ack_state;

  Uint16Bind dstb_start;
  Uint16Bind dstb_end;

  FpgaDstbData dstb_data() const;
  void set_dstb_data(const FpgaDstbData& data);

  BoolBind dstb;

  FpgaSendRegister();
  ~FpgaSendRegister() {}

  // send range
  int SendOffset() const;
  int SendSize() const;

  // state range
  int StateOffset() const;
  int StateSize() const;

private:
  DISALLOW_COPY_AND_ASSIGN(FpgaSendRegister);
};

} // namespace canscope
