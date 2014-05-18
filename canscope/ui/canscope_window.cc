#include "canscope/ui/canscope_window.h"

#include "ui/views/widget/widget.h"

#include "canscope/app/canscope_process.h"

namespace canscope {

std::wstring CANScopeWindow::GetWindowTitle() const  {
  return L"CANScope osilloscope";
}

void CANScopeWindow::WindowClosing() {
  CANScopeProcess::Get()->Close();
}

views::Widget* CANScopeWindow::GetWidget() {
  return contents_->GetWidget();
}

const views::Widget* CANScopeWindow::GetWidget() const  {
  return contents_->GetWidget();
}

CANScopeWindow::CANScopeWindow(CANScopeDevice* device) {
  contents_ = new CANScopeView(device);

  views::Widget* window =
      views::Widget::CreateWindowWithBounds(this, gfx::Rect(0, 0, 850, 600));

  window->Show();
}

} // namespace canscope