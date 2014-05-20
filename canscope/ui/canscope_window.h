#pragma once

#include "ui/views/widget/widget_delegate.h"

#include "canscope/ui/canscope_view.h"

namespace canscope {

class CANScopeWindow : public views::WidgetDelegate {
public:
  CANScopeWindow(CANScopeDevice* device);
  virtual ~CANScopeWindow() {}

  void Close();

private:
  // views::WidgetDelegate implementation:
  virtual bool CanResize() const OVERRIDE { return true; }
  virtual bool CanMaximize() const OVERRIDE { return true; }
  virtual string16 GetWindowTitle() const OVERRIDE;
  virtual views::View* GetContentsView() OVERRIDE { return contents_.get(); }
  virtual void WindowClosing() OVERRIDE;
  virtual views::Widget* GetWidget() OVERRIDE;
  virtual const views::Widget* GetWidget() const OVERRIDE;
  virtual void DeleteDelegate() OVERRIDE;

  scoped_ptr<CANScopeView> contents_;

  DISALLOW_COPY_AND_ASSIGN(CANScopeWindow);
};

} // namespace canscope