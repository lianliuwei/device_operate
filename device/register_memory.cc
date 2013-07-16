#include "device/register_memory.h"

#include "base/logging.h"

namespace device {

RegisterMemory::RegisterMemory(uint32 start_addr, int length)
  : start_addr_(start_addr) {
  DCHECK(length >= 0);
  memory_.resize(static_cast<size_t>(length));
  memset(&memory_[0], 0x0, memory_.size());
}

uint8* RegisterMemory::PtrByAbsoulute(uint32 start_addr) {
  DCHECK(start_addr_ <= start_addr);
  int offset = start_addr - start_addr_;
  return PtrByRelative(static_cast<uint32>(offset));
}

uint8* RegisterMemory::PtrByRelative(uint32 offset) {
  DCHECK(offset < memory_.size());
  return &memory_[offset];
}

int RegisterMemory::size() {
  return static_cast<int>(memory_.size());
}

uint8* RegisterMemory::buffer() {
  return &memory_[0];
}

} // namespace device