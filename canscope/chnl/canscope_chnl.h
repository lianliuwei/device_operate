#pragma once

#include "canscope/chnl/chnl.h"
#include "canscope/osc_chnl_calc/chnl_calc_result.h"
#include "canscope/device/osc_device/osc_device_handle.h"

namespace canscope {

class CANScopeChnl : public ::Chnl {
public:
  // handle can be null, mean the only use for read.
  CANScopeChnl(canscope::Chnl chnl, 
      scoped_refptr<ChnlCalcResult> result, OscDeviceHandle* handle);
  virtual ~CANScopeChnl() {}

  void set_type(ChnlTypeEnum type) { type_ = type; }
  void UpdateResult(scoped_refptr<ChnlCalcResult> result) { result_ = result; }

  // implement ::Chnl
  virtual ChnlTypeEnum ChnlType() const;
  virtual bool HasChnlData() const;
  virtual const canscope::OscChnlData& ChnlData() const;
  virtual double VOffset() const;
  virtual void SetVOffset(double offset);
  virtual int VRange() const;
  virtual const std::vector<double>& VRangeOption() const;
  virtual void SetVRange(int i);
  virtual bool IsTrigger() const;
  virtual void BeTrigger();

private:
  VoltRangeStoreMember* GetVoltRange() const;
  Property<VoltRange>* SetVoltRange();
  ::device::DoubleStoreMember* GetVoltOffset() const;
  Property<double>* SetVoltOffset();
  void* Key() const;

  OscDeviceProperty* device_property() const;

  scoped_refptr<ChnlCalcResult> result_;
  OscDeviceHandle* handle_;

  ChnlTypeEnum type_;
  canscope::Chnl chnl_;
  std::vector<double> v_range_;

  DISALLOW_COPY_AND_ASSIGN(CANScopeChnl);
};

} // namespace canscope