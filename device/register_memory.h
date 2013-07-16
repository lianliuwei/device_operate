#pragma once

#include <vector>
#include "base/basictypes.h"

// the ptr is past to the bind, can no changed.
namespace device {

class RegisterMemory {
 public:
  RegisterMemory(uint32 start_addr, 
                 int length);

  uint8* PtrByAbsoulute(uint32 start_addr);
  uint8* PtrByRelative(uint32 offset);
  
  int size();
  uint8* buffer();

private:
  typedef std::vector<uint8> MemoryArea;
  MemoryArea memory_;

  uint32 start_addr_;
  
  DISALLOW_COPY_AND_ASSIGN(RegisterMemory);
};

}