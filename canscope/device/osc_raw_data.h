#pragma once

#include "base/memory/ref_counted.h"

// Osc Config for RawData
// just add RefCount
class OscRawDataDeviceConfig : public OscDeviceProperty 
                             , public base::RefCountedThreadSafe<
                                  OscRawDataDeviceConfig> {
public:
  OscRawDataDeviceConfig(const ConfigManager::Config& config);
  virtual ~OscRawDataDeviceConfig();

  bool SameConfig(const ConfigManager::Config& config);

  int id();

private:
  int id_;

  DISALLOW_COPY_AND_ASSIGN(OscRawDataDeviceConfig);
};

typedef scoped_refptr<OscRawDataDeviceConfig> OscRawDataDeviceConfigHandle;


class OscRawData : public RefCountedThreadSafe<OscRawData> {
public:
  int size() const;
  uint8* data();
  const uint8* data() const;
 
  OscDeviceProperty* property();
  int id();

private:
  std::vector<uint8> raw_data_;
  DeviceType type_;

  OscRawDataDeviceConfigHandle property_;

  DISALLOW_COPY_AND_ASSIGN(OscRawData);
};

typedef scoped_refptr<OscRawData> OscRawDataHandle;
