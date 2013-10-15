#pragma once

class CalcDelegate;

typedef int CalcKey;

class CalcItem {
public:

  std::string name();
  
  virtual CalcItem* Clone();

  // to id what this calc about.
  // use the ptr of the obj summit this object.
  // and use this id to get calc result.
  CalcKey id();
protected:
  // may be call on other thread.
  virtual bool Run(CalcDelegate* delegate);

private:

};
