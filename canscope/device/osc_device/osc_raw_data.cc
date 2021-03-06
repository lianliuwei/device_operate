#include "canscope/device/osc_device/osc_raw_data.h"

namespace canscope {

OscRawDataDeviceConfig::OscRawDataDeviceConfig(
    const ConfigManager::Config& config, bool hardware_diff)
    : OscDeviceProperty()
    , id_(config.id)
    , hardware_diff_(hardware_diff) {
  // config ownership still will ConfigManager
  Init(config.pref->DeepCopy());
}

bool OscRawDataDeviceConfig::SameConfig(
    const ConfigManager::Config& config) const {
  return config.id == id_;
}

OscRawData::OscRawData(DeviceType type, OscRawDataDeviceConfigHandle property)
    : property_(property)
    , own_(true)
    , type_(type) {
  property_->DetachFromThread();
  size_ = DeviceTypeDataSize(type);
  raw_data_ = new uint8[size_];
}

OscRawData::OscRawData(int size, OscRawDataDeviceConfigHandle property) 
    : property_(property)
    , own_(true) {
  // type are invalid
  property_->DetachFromThread();
  size_ = size;
  raw_data_ = new uint8[size];
}

OscRawData::OscRawData(uint8* raw_data, int size, bool own, 
                       DeviceType type, OscRawDataDeviceConfigHandle property)
    : property_(property)
    , raw_data_(raw_data)
    , size_(size)
    , own_(own_)
    , type_(type) {
  property_->DetachFromThread();
}

OscRawData::~OscRawData() {
  if (own_) {
    delete[] raw_data_;
    raw_data_ = NULL;
  }
}


int OscRawData::chnl_size() const {
  return size() / 2;
}

const OscDeviceProperty* OscRawData::property() const {
  return property_.get();  
}

OscDeviceProperty* OscRawData::property() {
  return property_.get();
}


void OscRawData::set_data(uint8* data, int size) {
  raw_data_ = data;
  size_ = size;
}


PooledOscRawData::PooledOscRawData(DeviceType type, 
                                   OscRawDataDeviceConfigHandle property, 
                                   scoped_refptr<MemoryUsagePool> pool)
    : OscRawData(NULL, 0, false, type, property)
    , pool_(pool)
    , alloc_ok_(false) {
  AllocAgain();
}

void PooledOscRawData::AllocAgain() {
  DCHECK(alloc_ok_ == false);
  int size = DeviceTypeDataSize(type());
  uint8* data = pool_->Alloc(size);
  alloc_ok_ = data != NULL;
  if (alloc_ok_) {
    set_data(data, size);
  }
}

PooledOscRawData::~PooledOscRawData() {
  if (alloc_ok_) {
    pool_->Free(data());
    set_data(NULL, 0);
  }
}

} // namespace canscope
