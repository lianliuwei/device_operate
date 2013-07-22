#pragma once

#include <vector>
#include "base/basictypes.h"
#include "base/memory/scoped_ptr.h"

// the ptr is past to the bind, can no changed.
namespace device {

class RegisterMemory {
 public:
  RegisterMemory(uint32 start_addr, 
                 int length);

  uint8* PtrByAbsoulute(uint32 start_addr);
  uint8* PtrByRelative(uint32 offset);
  
  int size() const;
  const uint8* buffer() const;
  uint8* buffer();

private:
  typedef std::vector<uint8> MemoryArea;
  MemoryArea memory_;

  uint32 start_addr_;
  
  DISALLOW_COPY_AND_ASSIGN(RegisterMemory);
};

class MemoryContent {
public:
  MemoryContent();
  explicit MemoryContent(const RegisterMemory& memory);
  explicit MemoryContent(int size);

  bool HasContent() const;
  int size() const;
  const uint8* buffer() const;
  uint8* buffer();

  void swap(MemoryContent* content);
  void SetContent(uint8* ptr, int size);
  // change size, will alloc new uint8[]
  void SetSize(int size);
  // copy content_ to memory
  // size must be equal.
  void SetMemory(RegisterMemory* memory) const;

private:
  int size_;
  scoped_ptr<uint8[]> content_;
};

} // namespace device