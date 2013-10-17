#pragma once

#include <string>

#include "base/basictypes.h"

class CalcDelegate;
class CalcExecuter;

typedef void* CalcKey;

class CalcItem {
public:
  CalcItem(const char* name, CalcKey id);
  CalcItem(std::string name, CalcKey id);
  virtual ~CalcItem() {}
  
  std::string name() const { return name_; }
  
  virtual CalcItem* Clone() const;

  // to id what this calc about.
  // use the ptr of the obj summit this object.
  // and use this id to get calc result.
  CalcKey id() const { return id_; }

private:
  friend class CalcExecuter;

  // may be call on other thread.
  virtual bool Run(CalcDelegate* delegate);

  std::string name_;
  CalcKey id_;

  DISALLOW_COPY_AND_ASSIGN(CalcItem);
};
