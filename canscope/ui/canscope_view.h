#pragma once

#include "ui/views/view.h"
#include "ui/views/controls/button/text_button.h"
#include "ui/views/controls/button/image_button.h"
#include "ui/views/controls/label.h"

#include "canscope/ui/oscilloscope.h"
#include "canscope/device/canscope_device.h"
#include "canscope/device/threaded_loop_run.h"

namespace {
class FPSSpeed;
}

namespace canscope {

class CANScopeView : public views::View
                   , public views::ButtonListener
                   , public ThreadedLoopRun::Observer {
public:
  CANScopeView(CANScopeDevice* device);
  virtual ~CANScopeView();

private:
  // implement views::ButtonListener
  virtual void ButtonPressed(views::Button* sender, const ui::Event& event);

  // implement ThreadLoopRun::Observer
  virtual void StateChanged();

  // override views::View
  virtual void Layout() OVERRIDE;
  virtual void ViewHierarchyChanged(const ViewHierarchyChangedDetails& details) OVERRIDE;

  void UpdateButton();
  void UpdateFPS();
  void LayoutFPS();

  views::TextButton* start_;
  views::TextButton* stop_;
  views::TextButton* debug_;
  views::TextButton* change_model_;
  views::ImageButton* usage_;
  bool inited_;

  views::View* button_group_;

  views::Label* fps_;
  scoped_ptr<FPSSpeed> speed_;
  views::View* osc_view_;
  scoped_ptr<Oscilloscope> osc_;

  base::Timer timer_;

  CANScopeDevice* device_;
  
  DISALLOW_COPY_AND_ASSIGN(CANScopeView);
};

} // namespace canscope