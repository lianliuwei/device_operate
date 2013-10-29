#pragma once

#include "base/memory/ref_counted.h"

#include "canscope/device/osc_device_property.h"
#include "canscope/device/config_manager.h"
// for DeviceType
#include "canscope/device/osc_device.h"

namespace canscope {
// Osc Config for RawData
// just add RefCount
class OscRawDataDeviceConfig : public OscDeviceProperty 
                             , public base::RefCountedThreadSafe<
                                  OscRawDataDeviceConfig> {
public:
  OscRawDataDeviceConfig(const ConfigManager::Config& config);
  virtual ~OscRawDataDeviceConfig() {}

  bool SameConfig(const ConfigManager::Config& config) const;

  int id() const;

private:
  int id_;

  DISALLOW_COPY_AND_ASSIGN(OscRawDataDeviceConfig);
};

typedef scoped_refptr<OscRawDataDeviceConfig> OscRawDataDeviceConfigHandle;


class OscRawData : public base::RefCountedThreadSafe<OscRawData> {
public:
  OscRawData(DeviceType type, OscRawDataDeviceConfigHandle property);
  OscRawData(int size, OscRawDataDeviceConfigHandle property);
  ~OscRawData() {}

  int size() const;
  int chnl_size() const;
  uint8* data();
  const uint8* data() const;
 
  const OscDeviceProperty* property() const;
  OscDeviceProperty* property();

  int id() const;

private:
  std::vector<uint8> raw_data_;
  DeviceType type_;

  OscRawDataDeviceConfigHandle property_;

  DISALLOW_COPY_AND_ASSIGN(OscRawData);
};

typedef scoped_refptr<OscRawData> OscRawDataHandle;

// move to canscope_device.h
inline size_t DeviceTypeDataSize(DeviceType type) {
  return 2000 * 2;
}

} // namespace canscope
