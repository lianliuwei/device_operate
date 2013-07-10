#pragma once

#include "device/register_memory.h"

class RegisterPropertyDelegate {
  virtual void NeedWriteMemory(char* ptr, int length) = 0;
  virtual void NeedReadMemory(char* ptr, int length) = 0;
};

typename<class MemoryBind>
class RegisterProperty {
public:
  typedef MemoryBind Type;

  RegisterProperty(RegisterPropertyDelegate* delegate);

  void Set(Type val) {
    bind_.set_value(val);
    delegate_->NeedWriteMemory(bind_.ptr(), MemoryBind::kByteSize);
  }

  Type Get() const {
    delegate_->NeedReadMemory(bind_.ptr(), MemoryBind::kByteSize);
    return bind_.get_value();
  }

protected:

private:
  MemoryBind bind_;
  RegisterPropertyDelegate* delegate_;
};
