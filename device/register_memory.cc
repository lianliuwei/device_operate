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

int RegisterMemory::size() const {
  return static_cast<int>(memory_.size());
}

const uint8* RegisterMemory::buffer() const {
  return &memory_[0];
}

uint8* RegisterMemory::buffer() {
  return &memory_[0];
}


MemoryContent::MemoryContent() 
    : size_(0) {}

MemoryContent::MemoryContent(const RegisterMemory& memory)
    : size_(memory.size())
    , content_(new uint8[memory.size()]) {
  memcpy(content_.get(), memory.buffer(), memory.size());  
}

MemoryContent::MemoryContent(int size) {
  SetSize(size);
}

bool MemoryContent::HasContent() const {
  return size_ != 0;
}

int MemoryContent::size() const {
  return size_;
}

const uint8* MemoryContent::buffer() const {
  return content_.get();
}

uint8* MemoryContent::buffer() {
  return content_.get();
}

void MemoryContent::swap(MemoryContent* content) {
  size_ = content->size_;
  content_.reset(content->content_.release());
}

void MemoryContent::SetContent(uint8* ptr, int size) {
  CHECK(size != 0);
  CHECK(ptr);
  CHECK(size_ == size);
  memcpy(content_.get(), ptr, size);
}

void MemoryContent::SetSize(int size) {
  size_ = size;
  content_.reset(size == 0 ? NULL : new uint8[size]);
}

void MemoryContent::SetMemory(RegisterMemory* memory) const {
  CHECK(memory);
  CHECK(size_ == memory->size());
  CHECK(size_ != 0);
  memcpy(memory->buffer(), content_.get(), size_);
}

} // namespace device