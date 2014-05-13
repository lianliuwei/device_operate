#pragma once

#include "canscope/osc_chnl_calc/osc_chnl_data.h"

// three model
// 1. CAN_H    CAN_L   CAN_DIFF
// only two chnl is real, CAN_DIFF is special chnl, is calculate by software
// but need write config to hardware for eye 
// 2. CAN_DIFF CAN_USE
// 3. CAN_DIFF
// Hardware CAN_DIFF

enum ChnlTypeEnum {
  CHNL_CAN_H,
  CHNL_CAN_L,
  CHNL_CAN_DIFF,
  CHNL_CAN_USE,
};


// can not save data in chnl, because chnl is thread special and no be synced
// unit is V or S
class ChnlContainer;

class Chnl {
public:
  virtual ChnlTypeEnum ChnlType() const = 0; 

  virtual bool HasChnlData() const = 0;
  virtual const canscope::OscChnlData& ChnlData() const = 0;

  // vertical option
  virtual double VOffset() const = 0;
  virtual void SetVOffset(double offset) = 0;
  // VRange has limit option
  virtual int VRange() const = 0;
  virtual const std::vector<double>& VRangeOption() const = 0;
  virtual void SetVRange(int i) = 0;

  virtual bool IsTrigger() const = 0;
  virtual void BeTrigger() = 0;

  double VRangeValue();

protected:
  friend class ChnlContainer;

  Chnl() {}
  virtual ~Chnl() {}
};