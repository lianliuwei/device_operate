#include "canscope/device_simulate/canscope_device_register_group.h"

namespace canscope {

CANScopeDeviceRegisterGroup::CANScopeDeviceRegisterGroup() {
  // NOTE no device info, is no in register address space
  AddRegister(&(analog_ctrl.memory));
  AddRegister(&(analog_switch.memory));
  AddRegister(&(fpga_send.memory));
  AddRegister(&(frame_storage.memory));
  AddRegister(&(scope_ctrl.memory));
  AddRegister(&(sja1000.memory));
  AddRegister(&(soft_diff.memory));
  AddRegister(&(trigger1.memory));
  AddRegister(&(trigger_state.memory));
  AddRegister(&(trigger2.memory));
  AddRegister(&(wave_read_ctrl.memory));
  AddRegister(&(wave_storage.memory));  
}

void CANScopeDeviceRegisterGroup::AddRegister(::device::RegisterMemory* memory) {
  DCHECK(memory);
  for (size_t i = 0; i < registers_.size(); ++i) {
    if (registers_[i].memory == memory) {
      NOTREACHED() << "repeat add memory";
    }
  }
  RegisterRecord record = { memory, };
  registers_.push_back(record);
}

void CANScopeDeviceRegisterGroup::SetCallback(const::device::RegisterMemory& memory, 
                                              RegisterCallback callback) {
  for (size_t i = 0; i < registers_.size(); ++i) {
    if (registers_[i].memory == const_cast<::device::RegisterMemory*>(&memory)) {
      if (!registers_[i].callback.is_null()) {
        LOG(WARNING) << "overwrite exist callback";
      }
      registers_[i].callback = callback;
      return;
    }
  }
  NOTREACHED() << "no register memory";
}

device::Error CANScopeDeviceRegisterGroup::WriteDevice(uint32 addr, uint8* buffer, int size) {
  DCHECK(size > 0);
  for (size_t i = 0; i < registers_.size(); ++i) {
    RegisterRecord record = registers_[i];
    uint32 memory_addr = record.memory->start_addr();
    int memory_size = record.memory->size();
    uint32 start_addr = addr;
    uint32 end_addr = addr + size;
    if (start_addr >= memory_addr + memory_size || end_addr <= memory_addr) {
      continue;
    }
    // NOTE may need break buffer to set to many buffer
    if (!(start_addr >= memory_addr && end_addr <= memory_addr + memory_size)) {
       NOTREACHED() << "write range over register";
    }
    
    scoped_ptr<uint8[]> back_up(new uint8[size]);
    memcpy(back_up.get(), record.memory->PtrByAbsoulute(addr), size);
    // temp write
    memcpy(record.memory->PtrByAbsoulute(addr), buffer, size);
    device::Error err = device::OK;
    if (!record.callback.is_null()) {
      err = record.callback.Run(record.memory, addr - memory_addr, size, false);
      if (err != device::OK) {
        // if err roll back
        memcpy(record.memory->PtrByAbsoulute(addr), back_up.get(), size);
      }
    }
    return err;
  }
  NOTREACHED() << "write to space no register";
  return device::OK;
}

device::Error CANScopeDeviceRegisterGroup::ReadDevice(uint32 addr, uint8* buffer, int size) {
  DCHECK(size > 0);
  for (size_t i = 0; i < registers_.size(); ++i) {
    RegisterRecord record = registers_[i];
    uint32 memory_addr = record.memory->start_addr();
    int memory_size = record.memory->size();
    uint32 start_addr = addr;
    uint32 end_addr = addr + size;
    if (start_addr >= memory_addr + memory_size || end_addr <= memory_addr) {
      continue;
    }
    // NOTE may need break buffer to set to many buffer
    if (!(start_addr >= memory_addr && end_addr <= memory_addr + memory_size)) {
       NOTREACHED() << "read range over register";
    }
    
    device::Error err = device::OK;
    if (!record.callback.is_null()) {
      err = record.callback.Run(record.memory, addr - memory_addr, size, true);
      if (err != device::OK) {
        return err;
      }
    }
    // copy if no err
    memcpy(buffer, record.memory->PtrByAbsoulute(addr), size);
    return err;
  }
  NOTREACHED() << "read no register space";
  return device::OK;
}

} // namespace canscope