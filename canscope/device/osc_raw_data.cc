#include "canscope/device/osc_raw_data.h"

namespace canscope {

OscRawDataDeviceConfig::OscRawDataDeviceConfig(
    const ConfigManager::Config& config)
    : OscDeviceProperty()
    , id_(config.id) {
  // config ownership still will ConfigManager
  Init(config.pref->DeepCopy());
}

bool OscRawDataDeviceConfig::SameConfig(
    const ConfigManager::Config& config) const {
  return config.id == id_;
}

int OscRawDataDeviceConfig::id() const {
  return id_;
}

OscRawData::OscRawData(DeviceType type, OscRawDataDeviceConfigHandle property)
    : property_(property)
    , raw_data_(DeviceTypeDataSize(type)) {
  property_->DetachFromThread();
}

int OscRawData::size() const {
  return static_cast<int>(raw_data_.size());
}


int OscRawData::chnl_size() const {
  return size() / 2;
}

const uint8* OscRawData::data() const {
  return &raw_data_[0];
}

uint8* OscRawData::data() {
  return &raw_data_[0];
}

const OscDeviceProperty* OscRawData::property() const {
  return property_.get();  
}

OscDeviceProperty* OscRawData::property() {
  return property_.get();
}

int OscRawData::id() const {
  return property_->id();
}

} // namespace canscope
