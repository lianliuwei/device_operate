#include "canscope/ui/canscope_window.h"

#include "base/time.h"
#include "ui/views/widget/widget.h"

#include "canscope/app/canscope_process.h"

using namespace base;

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

CANScopeWindow::CANScopeWindow(CANScopeDevice* device)
    : timer_(true, false) {
  CANScopeView* view = new CANScopeView(device);
  contents_.reset(view);

  views::Widget* window =
      views::Widget::CreateWindowWithBounds(this, gfx::Rect(100, 100, 850, 600));

  window->Show();
  timer_.Start(FROM_HERE, TimeDelta::FromMilliseconds(800), 
    Bind(&CANScopeView::InitShow, Unretained(view)));
}

void CANScopeWindow::DeleteDelegate() {
  delete this;
}

} // namespace canscope