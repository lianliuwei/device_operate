#pragma once

#include "device/simple_type_bind.h"
#include "device/bool_bind.h"
#include "device/register_memory.h"

namespace canscope {
class UsbCommand {
public:
  ::device::RegisterMemory memory;

  Uint16Bind cmd_id;
  BoolBind mode;
  Uint32Bind addr;
  Uint32Bind data_size;
  Uint32Bind func_addr;
  // data ptr after func_addr

  // read response struct is 
  // Uint16Bind cmd_id;
  // Uint16Bind reserve
  // data...
  uint8* write_data_ptr();
  static int write_data_max_size();
  uint8* read_data_ptr();
  static int read_data_max_size();

  int size() const;
  // reset memory to all zero
  void reset();
  
  UsbCommand();
  ~UsbCommand() {}

private:
  DISALLOW_COPY_AND_ASSIGN(UsbCommand);
};
} // namespace canscope
