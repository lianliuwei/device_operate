#pragma once

#include "base/memory/ref_counted.h"

#include "device/memory_usage_pool.h"

#include "canscope/device/osc_device/osc_device_property.h"
#include "canscope/device/config_manager.h"
// for DeviceType
#include "canscope/device/osc_device/osc_device.h"

namespace canscope {
// Osc Config for RawData
// just add RefCount
class OscRawDataDeviceConfig : public OscDeviceProperty 
                             , public base::RefCountedThreadSafe<
                                  OscRawDataDeviceConfig> {
public:
  OscRawDataDeviceConfig(const ConfigManager::Config& config, bool hardware_diff);

  bool SameConfig(const ConfigManager::Config& config) const;
  bool hardware_diff() const { return hardware_diff_; }

  int id() const { return id_; }

private:
  friend class base::RefCountedThreadSafe<OscRawDataDeviceConfig>;
  ~OscRawDataDeviceConfig() {}

  int id_;
  bool hardware_diff_;

  DISALLOW_COPY_AND_ASSIGN(OscRawDataDeviceConfig);
};

typedef scoped_refptr<OscRawDataDeviceConfig> OscRawDataDeviceConfigHandle;


class OscRawData : public base::RefCountedThreadSafe<OscRawData> {
public:
  OscRawData(DeviceType type, OscRawDataDeviceConfigHandle property);

  int size() const { return size_; }
  int chnl_size() const;
  uint8* data() { return raw_data_; }
  const uint8* data() const { return raw_data_; }
  DeviceType type() const { return type_; }

  OscDeviceProperty* property();
  const OscDeviceProperty* property() const;
  bool hardware_diff() const { return property_->hardware_diff(); }

  int id() const { return property_->id(); }

protected:
  OscRawData(uint8* raw_data, int size, bool own, DeviceType type, OscRawDataDeviceConfigHandle property);

  void set_data(uint8* data, int size);

  friend class base::RefCountedThreadSafe<OscRawData>;
  virtual ~OscRawData();

private:
  // test only
  OscRawData(int size, OscRawDataDeviceConfigHandle property);

  uint8* raw_data_;
  int size_;
  bool own_;
  DeviceType type_;

  OscRawDataDeviceConfigHandle property_;

  DISALLOW_COPY_AND_ASSIGN(OscRawData);
};

typedef scoped_refptr<OscRawData> OscRawDataHandle;


// check IsAllocOk Ofter ctor, if no alloc ok, this object is invalid. just destroy it.
class PooledOscRawData : public OscRawData {
public:
  PooledOscRawData(DeviceType type, 
                   OscRawDataDeviceConfigHandle property, 
                   scoped_refptr<MemoryUsagePool> pool);

  bool IsAllocOK() const { return alloc_ok_; }
  void AllocAgain();

private:
  virtual ~PooledOscRawData();

  scoped_refptr<MemoryUsagePool> pool_;

  bool alloc_ok_;

  DISALLOW_COPY_AND_ASSIGN(PooledOscRawData);
};
} // namespace canscope
