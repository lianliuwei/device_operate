#pragma once

#include "device/register_property.h"

class RegisterGroupDelegate {
 public:
  
 protected:
  
 private:
  
};
class RegisterGroup : public RegisterPropertyDelegate {
public:
  class Delegate {
    Error Read();
    Error Write();
  };
  
  virtual void NeedWriteMemory(char* ptr, int length) OVERRIDE {
    if (!write_)
      return;
    Error error = delegate_->Write(ptr, length);
    SetGlobalError(error);
  }
  virtual void NeedReadMemory(char* ptr, int length) OVERRIDE;

  void ReadAll() {
    if (!read_)
      return;
    Error error = delegate_->Read(register_memory_.ptr(), 
        register_memory_.size());
    SetGlobalError(error);
  }
  void WriteAll();

  void SetMemory(const RegisterMemory& memory);
  RegisterMemory GetMemory() const;
protected:

private:
  RegisterMemory register_memory_;
  Delegate delegate_;
  bool write_;
  bool read_;
};

class ScopeRecoverRegisterGroup {
 public:
  
 protected:
  
 private:
  
};

class ScopeWriteRegisterGroup {
 public:
  
 protected:
  
 private:
  
};

void GroupOperate(RegisterGroup* group, Uint2 cmp, Uint30 time) {
  ScopeWriteRegisterGroup scoped(group);
  group->cmp.Set(cmp);
  group->time.Set(time);
  // No error can happen.
  //if(HasError())
  //  scoped.GiveUp();
  AnotherGroupOperate(group);
}