#pragma once

#include "canscope/chnl/chnl.h"

class ChnlContainer {
public:
  // horizontal option
  virtual double HOffset() const = 0;
  virtual void SetHOffset(double offset) const = 0;
  // HRange has limit option
  virtual int HRange() const = 0;
  virtual const std::vector<double>& HRangeOption() const = 0;
  virtual void SetHRange(int i) = 0;

  Chnl* GetChnlByType(ChnlTypeEnum type);
  int ChnlCount() const;
  Chnl* GetChnlAt(int index);
  int ChnlAt(Chnl* chnl);
  
protected:
  ChnlContainer() {}
  virtual ~ChnlContainer();

  void AddChnl(Chnl* chnl);
  void AddChnlAt(Chnl* chnl, int index);
  void RemoveChnl(Chnl* chnl);
  void RemoveAll();

private:
  std::vector<Chnl*> chnls_;
};