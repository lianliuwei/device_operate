#pragma once

#include <vector>

// TODO need IOBuffer no vector, the ptr is past to the bind, can no changed.
namespace device {

typedef std::vector<uint8> MemoryArea;
typedef std::vector<uint8>::iterator MemoryPtr;
// start, length, start ptr
typedef base::Callback<Error(uint32,uint32,MemoryPtr)> ReadCallback;
typedef base::Callback<Error(uint32,uint32,MemoryPtr)> WriteCallback;

class RegisterMemory {
 public:
  RegisterMemory(uint32 start_addr, 
                 uint32 length, 
                 ReadCallback read_callback, 
                 WriteCallback write_callback);

  MemoryPtr GetPtrByAbsoulute(uint32 start_addr);
  MemoryPtr GetPtrByRelative(uint32 offset);
  Error Read(uint32 start, uint32 length);
  Error Write(uint32 start, uint32 length);
  Error WriteByPtr(MemoryPtr ptr, uint32 length);

 protected:
  MemoryArea memory_;
  MemoryArea backup_;
  uint32 start_addr_;
 private:
  
};

}