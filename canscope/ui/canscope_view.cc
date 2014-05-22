#include "canscope/ui/canscope_view.h"

#include "base/stringprintf.h"
#include "ui/gfx/rect.h"
#include "ui/gfx/vector2d.h"
#include "ui/views/bubble/bubble_delegate.h"
#include "ui/views/layout/box_layout.h"
#include "ui/base/resource/resource_bundle.h"

#include "grit/ui_resources.h"

#include "wave_control/views/wave_control_views_constants.h"
#include "wave_control/views/fill_box_layout.h"
#include "wave_control/views/wave_control_view_factory.h"

#include "canscope/device/canscope_device_handle.h"
#include "canscope/app/canscope_process.h"

using namespace ui;
using namespace base;
using namespace views;

namespace {
string16 FPSText(double fps) {
  return StringPrintf(L"FPS: %4.1f", fps);
};

class FPSSpeed {
public:
  FPSSpeed() 
      : count_speed_(0)
      , last_count_(0) 
      , last_(Time::Now()) {}
  ~FPSSpeed() {}

  void Update(int64 count) {
    TimeDelta pass_time = Time::Now() - last_;
    last_ = Time::Now();
    int64 last_count = last_count_;
    last_count_ = count;
    count_speed_ = (count - last_count) / pass_time.InSecondsF();
  }
  double Speed() { return count_speed_; }

private:
  double count_speed_;
  int64 last_count_;
  base::Time last_;

DISALLOW_COPY_AND_ASSIGN(FPSSpeed);
};

void SetChildBoard(views::View* const parent, int depth) {
  if (depth == 0) {
    return;
  }
  for (int i = 0; i < parent->child_count(); ++i) {
    parent->child_at(i)->set_border(
      views::Border::CreateSolidBorder(1, SkColorSetRGB(255, 255, 255)));
    SetChildBoard(parent->child_at(i), depth - 1);
  }
}

void LayoutChild(views::View* const parent, int depth) {
  if (depth == 0) {
    return;
  }
  for (int i = 0; i < parent->child_count(); ++i) {
    parent->child_at(i)->Layout();
    LayoutChild(parent->child_at(i), depth - 1);
  }
}

void ConfigButton(TextButton* button) {
  button->set_alignment(TextButtonBase::ALIGN_CENTER);
  button->SetEnabledColor(SkColorSetRGB(240, 240, 240));
  button->SetDisabledColor(SkColorSetRGB(125, 125, 125));
  button->SetHoverColor(SkColorSetRGB(255, 255, 255));
}

class UsageBubble : public BubbleDelegateView {
public:
  UsageBubble(View* anchor)
      : BubbleDelegateView(anchor, BubbleBorder::LEFT_TOP) {
    set_shadow(BubbleBorder::BIG_SHADOW);
    set_color(SK_ColorCYAN);
  }

private:
  virtual void Init() OVERRIDE {
    SetLayoutManager(new BoxLayout(BoxLayout::kVertical, 20, 20, 0));
    Label* label = new Label();
    label->SetMultiLine(true);
    label->SetHorizontalAlignment(gfx::ALIGN_LEFT);
    label->SetText(
L"示波器使用方式\n\n\
滚轮: 移动水平坐标\n\
Shift+滚轮: 缩放垂直范围\n\
Ctrl+滚轮: 缩放水平范围\n\
拖动四周指示: 移动相关坐标\n\
Shift+鼠标拖动波形: 移动波形其他容器");
    AddChildView(label);
  }

  DISALLOW_COPY_AND_ASSIGN(UsageBubble);
};
  
}

namespace canscope {

CANScopeView::CANScopeView(CANScopeDevice* device)
    : start_(NULL)
    , stop_(NULL) 
    , debug_(NULL)
    , change_model_(NULL)
    , button_group_(NULL)
    , fps_(NULL)
    , osc_view_(NULL) 
    , timer_(true, true)
    , device_(device)
    , speed_(new FPSSpeed)
    , inited_(false) {
  DCHECK(device);

  set_background(Background::CreateSolidBackground(SkColorSetRGB(0, 0, 0)));

  button_group_ = new View;
  button_group_->SetLayoutManager(
      new FillBoxLayout(FillBoxLayout::kHorizontal, 1, 2, 4));
  AddChildView(button_group_);
  usage_ = new ImageButton(this);
  usage_->SetImage(Button::STATE_NORMAL, 
      ResourceBundle::GetSharedInstance().GetImageSkiaNamed(IDR_APP_LIST_TOOLS_NORMAL));
  usage_->SetImage(Button::STATE_HOVERED, 
      ResourceBundle::GetSharedInstance().GetImageSkiaNamed(IDR_APP_LIST_TOOLS_HOVER));
  usage_->SetImage(Button::STATE_PRESSED, 
      ResourceBundle::GetSharedInstance().GetImageSkiaNamed(IDR_APP_LIST_TOOLS_PRESSED));
  button_group_->AddChildView(usage_);
  start_ = new TextButton(this, L"Start");
  ConfigButton(start_);
  button_group_->AddChildView(start_);
  stop_ = new TextButton(this, L"Stop");
  button_group_->AddChildView(stop_);
  ConfigButton(stop_);
  change_model_ = new TextButton(this, L"Model");
  ConfigButton(change_model_);
  button_group_->AddChildView(change_model_);
  debug_ = new TextButton(this, L"Debug");
  ConfigButton(debug_);
  button_group_->AddChildView(debug_);

  fps_ = new Label(FPSText(0.0));
  fps_->SetBackgroundColor(SkColorSetRGB(0, 0, 0));
  fps_->SetEnabledColor(SkColorSetRGB(255, 0, 0));
  AddChildView(fps_);

  CANScopeDeviceHandle* device_handle = CANScopeDeviceHandle::GetInstance(device_);
  DCHECK(device_handle);
  scoped_refptr<ChnlCalcResultQueue> queue = 
      CANScopeProcess::Get()->GetChnlCalcResultQueue(device_);
  osc_.reset(new Oscilloscope(&(device_handle->osc_device_handle), queue));

  WaveControlViewFactory::GetInstance()->set_wave_bar_size(60);
  osc_view_ = WaveControlViewFactory::GetInstance()->Create(osc_.get());
  AddChildView(osc_view_);

  UpdateButton();

  device_handle->GetOscDataCollecter()->AddObserver(this);

  timer_.Start(FROM_HERE, TimeDelta::FromMilliseconds(1000), 
      Bind(&CANScopeView::UpdateFPS, Unretained(this)));
}

CANScopeView::~CANScopeView() {
  CANScopeDeviceHandle* device_handle = CANScopeDeviceHandle::GetInstance(device_);
  device_handle->GetOscDataCollecter()->RemoveObserver(this);
}

void CANScopeView::Layout() {
  gfx::Size b_size = button_group_->GetPreferredSize();
  gfx::Rect v_rect = GetLocalBounds();
  b_size.Enlarge(26, 0);
  button_group_->SetBoundsRect(gfx::Rect(v_rect.origin(), b_size));
  
  LayoutFPS();

  v_rect.Offset(gfx::Vector2d(0, b_size.height()));
  v_rect.Intersect(GetLocalBounds());
  osc_view_->SetBoundsRect(v_rect);
}

void CANScopeView::ButtonPressed(views::Button* sender, const ui::Event& event) {
  CANScopeDeviceHandle* device_handle = CANScopeDeviceHandle::GetInstance(device_);
  if (sender == start_) {
    device_handle->GetOscDataCollecter()->Start();
  } else if (sender == stop_) {
    device_handle->GetOscDataCollecter()->Stop();
  } else if (sender == debug_) {
    SetChildBoard(osc_view_, 5);
    osc_view_->SchedulePaint();
  } else if (sender == change_model_) {
    Oscilloscope::ShowModel model = osc_->show_model();
    model = model == Oscilloscope::kSeparate ? 
        Oscilloscope::kCombine : Oscilloscope::kSeparate;
    osc_->set_show_model(model);
  } else if (sender == usage_) {
    UsageBubble* bubble = new UsageBubble(sender);
    BubbleDelegateView::CreateBubble(bubble);
    bubble->StartFade(true);
  } else {
    NOTREACHED();
  }
}

void CANScopeView::StateChanged() {
  UpdateButton();
}

void CANScopeView::UpdateButton() {
  CANScopeDeviceHandle* device_handle = CANScopeDeviceHandle::GetInstance(device_);
  bool start = device_handle->GetOscDataCollecter()->IsRunning();
  stop_->SetEnabled(start);
  start_->SetEnabled(!start);
}

void CANScopeView::UpdateFPS() {
  speed_->Update(osc_->read_count());
  fps_->SetText(FPSText(speed_->Speed()));
  LayoutFPS();
}

void CANScopeView::LayoutFPS() {
  gfx::Rect v_rect = GetLocalBounds();
  gfx::Size l_size = fps_->GetPreferredSize();
  gfx::Point l_origin = v_rect.top_right() + gfx::Vector2d(-l_size.width(), 0);
  fps_->SetBoundsRect(gfx::Rect(l_origin, l_size));
}

void CANScopeView::ViewHierarchyChanged(const ViewHierarchyChangedDetails& details) {
  if (details.is_add && GetWidget() && !inited_) {
    inited_ = true;

    UsageBubble* bubble = new UsageBubble(usage_);
    BubbleDelegateView::CreateBubble(bubble);
    bubble->StartFade(true);    
  }
}
} // namespace canscope