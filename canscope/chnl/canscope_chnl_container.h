#include "canscope/chnl/chnl_container.h"

#include "canscope/chnl/canscope_chnl.h"
#include "canscope/osc_chnl_calc/chnl_calc_result.h"
#include "canscope/device/osc_device/osc_device_handle.h"

namespace canscope {

class CANScopeChnlContainer : public ChnlContainer {
public:
  // handle can be null, mean the only use for read.
  CANScopeChnlContainer(scoped_refptr<ChnlCalcResult> result, OscDeviceHandle* handle);
  virtual ~CANScopeChnlContainer();

  void UpdateResult(scoped_refptr<ChnlCalcResult> result);

  // implement ChnlContainer
  virtual double HOffset() const;
  virtual void SetHOffset(double offset) const;
  virtual int HRange() const;
  virtual const std::vector<double>& HRangeOption() const;
  virtual void SetHRange(int i);
  
private:
  void UpdateContainer(bool hardware_diff, bool init);
  void UpdateHRange(DeviceType type, bool init);

  OscDeviceProperty* device_property() const;

  scoped_ptr<CANScopeChnl> can_h_;
  scoped_ptr<CANScopeChnl> can_l_;
  scoped_ptr<CANScopeChnl> can_diff_;

  bool hardware_diff_;
  DeviceType type_;

  scoped_refptr<ChnlCalcResult> result_;
  OscDeviceHandle* handle_;

  std::vector<double> h_range_;

  DISALLOW_COPY_AND_ASSIGN(CANScopeChnlContainer);
};

} // namespace canscope