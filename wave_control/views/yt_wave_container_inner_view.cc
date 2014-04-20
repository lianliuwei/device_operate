#include "wave_control/views/yt_wave_container_inner_view.h"

#include "wave_control/wave_visitor.h"
#include "wave_control/yt_wave_container.h"
#include "wave_control/views/transform_util.h"
#include "wave_control/views/wave_control_view_factory.h"
#include "wave_control/views/wave_control_views_constants.h"
#include "wave_control/views/all_fill_layout.h"
#include "wave_control/views/handle_point_view.h"

using namespace ui;
using namespace std;
using namespace views;

namespace {
static const int kFrontSize = 1;
static const int kHandlePointViewID = 0;

bool IsSet(int set, int test) {
  return (set & test) != 0;
}

class HorizOffsetBar : public HandleBarDelegate
                     , public OscWaveGroupObserver {
public:
  HorizOffsetBar(OscWaveGroup* wave_group, YTWaveContainerInnerView* view);
  virtual ~HorizOffsetBar();

  // implement HandleBarDelegate
  virtual bool is_horiz() { return true; }

  // implement HandleBarModel
  virtual int Count();
  virtual int GetID(int index) { return index; } // index == id
  virtual string16 GetText(int ID);
  virtual SkColor GetColor(int ID);
  virtual const gfx::Image& GetIcon(int ID);
  virtual int GetOffset(int ID);
  virtual bool IsEnable(int ID) { return true; }
  virtual bool IsVisible(int ID);

private:
  // implement HandleBarObserver
  virtual void OnHandlePressed(int id, int offset, bool horiz) {}
  virtual void OnHandleReleased(int id) {}
  virtual void OnHandleMove(int ID, int offset);
  virtual void OnHandleActive(int ID);

  // implement OscWaveGroupObserver
  virtual void OnPartGroupChanged() { NotifyModelChanged(); }
  virtual void OnPartChanged(int id) { NotifyHandleChanged(id); }
  virtual void OnPartMoved(int id){ NotifyHandleMoved(id); }

  OscWaveGroup* wave_group_;
  YTWaveContainerInnerView* view_;

  DISALLOW_COPY_AND_ASSIGN(HorizOffsetBar);
};

HorizOffsetBar::HorizOffsetBar(OscWaveGroup* wave_group, 
                               YTWaveContainerInnerView* view)
    : wave_group_(wave_group)
    , view_(view) {
  wave_group->AddHorizontalObserver(this);        
}

HorizOffsetBar::~HorizOffsetBar() {
  wave_group_->RemoveHorizontalObserver(this);
}

int HorizOffsetBar::Count() {
  return wave_group_->horizontal_count();
}

string16 HorizOffsetBar::GetText(int ID) {
  return wave_group_->horizontal_at(ID)->text();
}

SkColor HorizOffsetBar::GetColor(int ID) {
  return wave_group_->horizontal_at(ID)->color();
}

const gfx::Image& HorizOffsetBar::GetIcon(int ID) {
  return wave_group_->horizontal_at(ID)->icon();
}

int HorizOffsetBar::GetOffset(int ID) {
  OscWave* wave = wave_group_->horizontal_at(ID)->osc_wave();
  gfx::Transform transform = view_->OscWaveTransform(wave);
  return TransformX(transform, 0);
}

bool HorizOffsetBar::IsVisible(int ID) {
  return wave_group_->horizontal_at(ID)->show();
}

void HorizOffsetBar::OnHandleMove(int ID, int offset) {
  OscWave* wave = wave_group_->horizontal_at(ID)->osc_wave();
  gfx::Transform transform = view_->OscWaveTransform(wave);
  int logic_offset = TransformReverseX(transform, offset);
  double old_offset = wave->horizontal_offset();
  wave->MoveToX(YTWaveContainerInnerView::ToOscOffset(old_offset, logic_offset));
}

void HorizOffsetBar::OnHandleActive(int ID) {
  OscWave* wave = wave_group_->horizontal_at(ID)->osc_wave();
  view_->SelectWave(wave);  
}

class TriggerBar : public HandleBarDelegate
                 , public OscWaveGroupObserver {
public:
  TriggerBar(OscWaveGroup* wave_group, YTWaveContainerInnerView* view);
  virtual ~TriggerBar();

  // implement HandleBarDelegate
  virtual bool is_horiz() { return false; }

  // implement HandleBarModel
  virtual int Count();
  virtual int GetID(int index) { return index; } // index == id
  virtual string16 GetText(int ID);
  virtual SkColor GetColor(int ID);
  virtual const gfx::Image& GetIcon(int ID);
  virtual int GetOffset(int ID);
  virtual bool IsEnable(int ID) { return true; }
  virtual bool IsVisible(int ID);

private:
  // implement HandleBarObserver
  virtual void OnHandlePressed(int id, int offset, bool horiz) {}
  virtual void OnHandleReleased(int id) {}
  virtual void OnHandleMove(int ID, int offset);
  virtual void OnHandleActive(int ID);

  // implement OscWaveGroupObserver
  virtual void OnPartGroupChanged() { NotifyModelChanged(); }
  virtual void OnPartChanged(int id) { NotifyHandleChanged(id); }
  virtual void OnPartMoved(int id){ NotifyHandleMoved(id); }

  void OnOscWaveVerticalMoved(OscWave* osc_wave);

  OscWaveGroup* wave_group_;
  YTWaveContainerInnerView* view_;

  DISALLOW_COPY_AND_ASSIGN(TriggerBar);
};


TriggerBar::TriggerBar(OscWaveGroup* wave_group, 
                       YTWaveContainerInnerView* view)
    : wave_group_(wave_group)
    , view_(view) {
  wave_group->AddTriggerObserver(this);
}

TriggerBar::~TriggerBar() {
  wave_group_->RemoveTriggerObserver(this);
}

int TriggerBar::Count() {
  return wave_group_->trigger_count();
}

string16 TriggerBar::GetText(int ID) {
  return wave_group_->trigger_at(ID)->text();
}

SkColor TriggerBar::GetColor(int ID) {
  return wave_group_->trigger_at(ID)->color();
}

const gfx::Image& TriggerBar::GetIcon(int ID) {
  return wave_group_->trigger_at(ID)->icon();
}

bool TriggerBar::IsVisible(int ID) {
  return wave_group_->trigger_at(ID)->show();
}

int TriggerBar::GetOffset(int ID) {
  TriggerPart* trigger = wave_group_->trigger_at(ID);
  OscWave* wave = trigger->osc_wave();
  bool relate = trigger->IsRelate();
  OscWave* trigger_wave = trigger->trigger_wave();
  double offset = trigger->offset();
  // if is relate the trigger_offset value relate to the wave zero pos
  // so transform use this offset.
  // if is no relate, the trigger_offset value relate to the wave wave range.
  // the pos in the OscWaveTransform need to add trigger vertical_offset.
  // yet this is trick
  if (!relate) {
    offset = trigger_wave->vertical_offset() + offset;
  }
  gfx::Transform transform = view_->OscWaveTransform(trigger_wave);
  return TransformY(transform, offset);
}

void TriggerBar::OnHandleMove(int ID, int offset) {
  TriggerPart* trigger = wave_group_->trigger_at(ID);
  OscWave* wave = trigger->osc_wave();
  bool relate = trigger->IsRelate();
  OscWave* trigger_wave = trigger->trigger_wave();
  double base_y = 0;
  if (!relate) {
    base_y = -trigger_wave->vertical_offset();
  }
  gfx::Transform transform = view_->OscWaveTransform(trigger_wave);
  double new_offset = TransformReverseY(transform, offset);
  trigger_wave->MoveTrigger(new_offset - base_y);
}

void TriggerBar::OnHandleActive(int ID) {
  TriggerPart* trigger = wave_group_->trigger_at(ID);
  view_->SelectWave(trigger->trigger_wave());
}

// TODO need add OtherWave observer
class WaveBar : public HandleBarDelegate
              , public OscWaveGroupObserver
              , public SimpleAnaWaveObserver {
public:
  WaveBar(OscWaveGroup* wave_group, YTWaveContainerInnerView* view);
  virtual ~WaveBar();

  // implement HandleBarDelegate
  virtual bool is_horiz() { return false; }

  // implement HandleBarModel
  virtual int Count();
  virtual int GetID(int index) { return index; } // index == id
  virtual string16 GetText(int ID);
  virtual SkColor GetColor(int ID);
  virtual const gfx::Image& GetIcon(int ID);
  virtual int GetOffset(int ID);
  virtual bool IsEnable(int ID) { return true; }
  virtual bool IsVisible(int ID);

  void AddOtherWave(Wave* wave);
  void RemoveOtherWave(Wave* wave);
  bool HasOtherWave(Wave* wave);

private:
  class OtherWaveVisitor : public WaveVisitor {
  public:
    OtherWaveVisitor(Wave* other_wave, YTWaveContainerInnerView* view);
    virtual ~OtherWaveVisitor() {}

    int GetOffset();
    void MoveToY(int offset);
    void AddObserver(WaveBar* wave_bar);
    void RemoveObserver(WaveBar* wave_bar);

  private:
    enum VisitorType {
      kOffset,
      kMoveToY,
      kAddObserver,
      kRemoveObserver,
    };

    // implement WaveVisitor
    virtual void VisitOscWave(OscWave* wave) { NOTREACHED(); }
    virtual void VisitSimpleAnaWave(SimpleAnaWave* wave);
    virtual void VisitSimpleDigitWave(SimpleDigitWave* wave) { NOTREACHED(); }

    VisitorType type_;
    int offset_;
    int ret_offset_;
    Wave* other_wave_;
    WaveBar* wave_bar_;
    YTWaveContainerInnerView* view_;

    DISALLOW_COPY_AND_ASSIGN(OtherWaveVisitor);
  };
  
  // implement HandleBarObserver
  virtual void OnHandlePressed(int id, int offset, bool horiz) {}
  virtual void OnHandleReleased(int id) {}
  virtual void OnHandleMove(int ID, int offset);
  virtual void OnHandleActive(int ID);

  // implement OscWaveGroupObserver
  virtual void OnPartGroupChanged() { NotifyModelChanged(); }
  virtual void OnPartChanged(int id) { NotifyHandleChanged(id); }
  virtual void OnPartMoved(int id){ NotifyHandleMoved(id); }

  // implement SimpleAnaWaveObserver
  virtual void OnSimpleAnaWaveChanged(SimpleAnaWave* ana_wave, int change_set);

  int OtherWaveIndex(Wave* wave);

  OscWaveGroup* wave_group_;
  YTWaveContainerInnerView* view_;

  std::vector<Wave*> other_wave_;

  DISALLOW_COPY_AND_ASSIGN(WaveBar);
};

WaveBar::OtherWaveVisitor::OtherWaveVisitor(Wave* other_wave, 
                                            YTWaveContainerInnerView* view) 
  : other_wave_(other_wave)
  , view_(view) {

}

void WaveBar::OtherWaveVisitor::VisitSimpleAnaWave(SimpleAnaWave* wave){
  if (type_ == kOffset) {
    ret_offset_ = view_->GetYOffset(wave);
  } else if (type_ == kMoveToY) {
    view_->MoveToY(wave, offset_);
  } else if (type_ == kAddObserver) {
    wave->AddObserver(wave_bar_); 
  } else if (type_ == kRemoveObserver) {
    wave->RemoveObserver(wave_bar_);
  } else {
    NOTREACHED();
  }
}

int WaveBar::OtherWaveVisitor::GetOffset() {
  type_ = kOffset;
  other_wave_->Accept(this);
  return ret_offset_;
}

void WaveBar::OtherWaveVisitor::MoveToY(int offset) {
  type_ = kMoveToY;
  offset_ = offset;
  other_wave_->Accept(this);
}

void WaveBar::OtherWaveVisitor::AddObserver(WaveBar* wave_bar) {
  type_ = kAddObserver;
  wave_bar_ = wave_bar;
  other_wave_->Accept(this);
}

void WaveBar::OtherWaveVisitor::RemoveObserver(WaveBar* wave_bar) {
  type_ = kRemoveObserver;
  wave_bar_ = wave_bar;
  other_wave_->Accept(this);
}

WaveBar::WaveBar(OscWaveGroup* wave_group, YTWaveContainerInnerView* view)
    : wave_group_(wave_group)
    , view_(view) {
  wave_group->AddVerticalObserver(this);
}

WaveBar::~WaveBar() {
  wave_group_->RemoveVerticalObserver(this);
}

int WaveBar::Count() {
  return wave_group_->vertical_count() + other_wave_.size();
}

string16 WaveBar::GetText(int ID) {
  if (ID < wave_group_->vertical_count()) {
    return wave_group_->vertical_at(ID)->text();
  } else {
    int index = ID - wave_group_->vertical_count();
    return other_wave_[index]->name();
  }
}

SkColor WaveBar::GetColor(int ID) {
  if (ID < wave_group_->vertical_count()) {
    return wave_group_->vertical_at(ID)->color();
  } else {
    int index = ID - wave_group_->vertical_count();
    return other_wave_[index]->color();
  }
}

const gfx::Image& WaveBar::GetIcon(int ID) {
  if (ID < wave_group_->vertical_count()) {
    return wave_group_->vertical_at(ID)->icon();
  } else {
    int index = ID - wave_group_->vertical_count();
    return other_wave_[index]->icon();
  }
}

bool WaveBar::IsVisible(int ID) {
  if (ID < wave_group_->vertical_count()) {
    return wave_group_->vertical_at(ID)->show();
  } else {
    return true;
  }
}

int WaveBar::GetOffset(int ID) {
  if (ID < wave_group_->vertical_count()) {
    OscWave* wave = wave_group_->vertical_at(ID)->osc_wave();
    gfx::Transform transform = view_->OscWaveTransform(wave);
    return TransformY(transform, 0);
  } else {
    int index = ID - wave_group_->vertical_count();
    OtherWaveVisitor visitor(other_wave_[index], view_);
    return visitor.GetOffset();
  }
}

void WaveBar::OnHandleMove(int ID, int offset) {
  if (ID < wave_group_->vertical_count()) {
    OscWave* wave = wave_group_->vertical_at(ID)->osc_wave();
    gfx::Transform transform = view_->OscWaveTransform(wave);
    int logic_offset = TransformReverseY(transform, offset);
    double old_offset = wave->vertical_offset();
    wave->MoveToY(YTWaveContainerInnerView::ToOscOffset(old_offset, logic_offset));
  } else {
    int index = ID - wave_group_->vertical_count();
    OtherWaveVisitor visitor(other_wave_[index], view_);
    return visitor.MoveToY(offset);
  }
}

void WaveBar::OnHandleActive(int ID) {
  if (ID < wave_group_->vertical_count()) {
    view_->SelectWave(wave_group_->vertical_at(ID)->osc_wave());
  } else {
    int index = ID - wave_group_->vertical_count();
    view_->SelectWave(other_wave_[index]);
  }
}
void WaveBar::AddOtherWave(Wave* wave) {
  DCHECK(!HasOtherWave(wave));
  other_wave_.push_back(wave);
  OtherWaveVisitor visitor(wave, view_);
  visitor.AddObserver(this);
  NotifyModelChanged();
}

void WaveBar::RemoveOtherWave(Wave* wave) {
  vector<Wave*>::iterator it = find(other_wave_.begin(), other_wave_.end(), wave);
  DCHECK(it != other_wave_.end());
  OtherWaveVisitor visitor(wave, view_);
  visitor.RemoveObserver(this);
  other_wave_.erase(it);
  NotifyModelChanged();
}

bool WaveBar::HasOtherWave(Wave* wave) {
  for (size_t i = 0; i < other_wave_.size(); ++i) {
    if (other_wave_[i] == wave) {
      return true;
    }
  }
  return false;
}

int WaveBar::OtherWaveIndex(Wave* wave) {
  for (size_t i = 0; i < other_wave_.size(); ++i) {
    if (other_wave_[i] == wave) {
      return i;
    }
  }
  NOTREACHED();
  return 0;
}

void WaveBar::OnSimpleAnaWaveChanged(SimpleAnaWave* ana_wave, int change_set) {
  if (IsSet(change_set, SimpleAnaWave::kVertical)) {
    int i = OtherWaveIndex(ana_wave);
    NotifyHandleMoved(i  + wave_group_->vertical_count());
  }
}

class YTWaveVisitor : public WaveVisitor {
public:
  YTWaveVisitor(YTWaveContainerInnerView* view);
  virtual ~YTWaveVisitor() {}

  void AddWave(Wave* wave);
  void RemoveWave(Wave* wave);
  void SetAxis(Wave* wave);

private:
  enum VisitorType {
    kAddWave,
    kRemoveWave,
    kSetAxis,
  };

  // implement WaveVisitor
  virtual void VisitOscWave(OscWave* wave);
  virtual void VisitSimpleAnaWave(SimpleAnaWave* wave);
  virtual void VisitSimpleDigitWave(SimpleDigitWave* wave) { NOTREACHED(); }

  VisitorType type_;
  YTWaveContainerInnerView* view_;

  DISALLOW_COPY_AND_ASSIGN(YTWaveVisitor);
};

YTWaveVisitor::YTWaveVisitor(YTWaveContainerInnerView* view)
    : view_(view) {

}

void YTWaveVisitor::AddWave(Wave* wave) {
  type_ = kAddWave;
  wave->Accept(this);
}

void YTWaveVisitor::RemoveWave(Wave* wave) {
  type_ = kRemoveWave;
  wave->Accept(this);
}

void YTWaveVisitor::SetAxis(Wave* wave) {
  type_ = kSetAxis;
  wave->Accept(this);
}

void YTWaveVisitor::VisitOscWave(OscWave* wave) {
  if (type_ == kAddWave) {
    view_->wave_group_->AddOscWave(wave);
  } else if (type_ == kRemoveWave) {
    view_->wave_group_->RemoveOscWave(wave);
  } else if (type_ == kSetAxis) {
    int v_div = wave->vertical_div() / 2;
    int h_div = wave->horizontal_div() / 2;
    view_->SetGrid(v_div, h_div);
  } else {
    NOTREACHED();
  }
}

void YTWaveVisitor::VisitSimpleAnaWave(SimpleAnaWave* wave) {
  if (type_ == kAddWave) {
    view_->wave_bar_->AddOtherWave(wave);
  } else if (type_ == kRemoveWave) {
    view_->wave_bar_->RemoveOtherWave(wave);
  } else if (type_ == kSetAxis) {
    // use last axis config.
  } else {
    NOTREACHED();
  }
}

}

void HandleBarDelegate::AddObserver(HandleBarModelObserver* observer) {
  observer_list_.AddObserver(observer);
}

void HandleBarDelegate::RemoveObserver(HandleBarModelObserver* observer) {
  observer_list_.RemoveObserver(observer);
}

bool HandleBarDelegate::HasObserver(HandleBarModelObserver* observer) {
  return observer_list_.HasObserver(observer);
}

void HandleBarDelegate::NotifyModelChanged() {
  FOR_EACH_OBSERVER(HandleBarModelObserver, observer_list_, OnModelChanged());
}

void HandleBarDelegate::NotifyHandleChanged(int id) {
  FOR_EACH_OBSERVER(HandleBarModelObserver, observer_list_, OnHandleChanged(id));
}

void HandleBarDelegate::NotifyHandleMoved(int id) {
  FOR_EACH_OBSERVER(HandleBarModelObserver, observer_list_, OnHandleMoved(id));
}

YTWaveContainerInnerView::YTWaveContainerInnerView(YTWaveContainer* container)
    : container_(container) {
  set_border(Border::CreateSolidBorder(kBorderThickness, kBorderColor));
  SetLayoutManager(new AllFillLayout());

  wave_group_.reset(container_->CreateOscWaveGroup());
  wave_bar_.reset(new WaveBar(wave_group_.get(), this));
  horiz_offset_bar_.reset(new HorizOffsetBar(wave_group_.get(), this));
  trigger_bar_.reset(new TriggerBar(wave_group_.get(), this));

  // add HandlePoint
  HandlePointView* view = new HandlePointView();
  AddChildViewAt(view, kHandlePointViewID);

  container->AddObserver(this);
  // fetch Wave
  ListItemsAdded(0, container->item_count());

  SetGrid(kDefaultVDiv, kDefaultHDiv);
}

YTWaveContainerInnerView::~YTWaveContainerInnerView() {
  container_->RemoveObserver(this);

  trigger_bar_.reset();
  horiz_offset_bar_.reset();
  wave_bar_.reset();
  wave_group_.reset();
}

// static
double YTWaveContainerInnerView::ToOscOffset(double old_offset, double move_delta) {
  return old_offset + (-move_delta);
}

int YTWaveContainerInnerView::GetYOffset(SimpleAnaWave* wave) {
  return GetSimpleAnaWaveView(wave)->GetYOffset();
}

void YTWaveContainerInnerView::MoveToY(SimpleAnaWave* wave, int offset) {
  GetSimpleAnaWaveView(wave)->MoveToY(offset);
}

void YTWaveContainerInnerView::ListItemsAdded(size_t start, size_t count) {
  YTWaveVisitor visitor(this);

  wave_record_.reserve(container_->item_count());
  for (size_t i = 0; i < count; ++i) {
    Wave* wave = container_->GetItemAt(start + i);
    View* view = WaveControlViewFactory::GetInstance()->Create(wave, 
        static_cast<YTWaveContainerView*>(this->parent()));
    this->AddChildViewAt(view, WaveIDToViewID(start + i));
    wave_record_.insert(wave_record_.begin() + start + i, wave);

    // notify add wave
    visitor.AddWave(wave);
  }
  if (start == 0) {
    UpdateAxis();
  }
}

void YTWaveContainerInnerView::ListItemsRemoved(size_t start, size_t count) {
  YTWaveVisitor visitor(this);
  vector<Wave*> need_remove_wave;
  View::Views need_remove;
  need_remove.reserve(count);
  for (size_t i = 0; i < count; ++i) {
    need_remove.push_back(this->child_at(WaveIDToViewID(start + i)));
    need_remove_wave.push_back(wave_record_[start]);
    wave_record_.erase(wave_record_.begin() + start);
  }
  for (size_t i = 0; i < need_remove.size(); ++i) {
    this->RemoveChildView(need_remove[i]);
    // notify remove wave
    visitor.RemoveWave(need_remove_wave[i]);  
    delete need_remove[i];
  }
  if (start == 0) {
    UpdateAxis();
  }
}

void YTWaveContainerInnerView::ListItemMoved(size_t index, size_t target_index) {
  this->ReorderChildView(this->child_at(WaveIDToViewID(index)), WaveIDToViewID(target_index));
  Wave* wave = wave_record_[index];
  wave_record_.erase(wave_record_.begin() + index);
  wave_record_.insert(wave_record_.begin() + target_index, wave);

  if (target_index == 0 || index == 0) {
    UpdateAxis();
  }
}

void YTWaveContainerInnerView::ListItemsChanged(size_t start, size_t count) {
  ListItemsRemoved(start, count);
  ListItemsAdded(start, count);

  if (start == 0) {
    UpdateAxis();
  }
}

gfx::Transform YTWaveContainerInnerView::OscWaveTransform(OscWave* osc_wave) {
  int id = container_->WaveAt(osc_wave);
  OscWaveView* wave_view = static_cast<OscWaveView*>(child_at(WaveIDToViewID(id)));
  return wave_view->data_transform();
}

HandleBarDelegate* YTWaveContainerInnerView::GetWaveBarDelegate() {
  return wave_bar_.get();
}

HandleBarDelegate* YTWaveContainerInnerView::GetHorizOffsetBarDelegate() {
  return horiz_offset_bar_.get();
}

HandleBarDelegate* YTWaveContainerInnerView::GetTriggerBarDelegate() {
  return trigger_bar_.get();
}

HandleBarObserver* YTWaveContainerInnerView::HandlePointDelegate() {
  return static_cast<HandlePointView*>(child_at(kHandlePointViewID));
}

bool YTWaveContainerInnerView::NormalSize(gfx::Size& size) {
  // keep space for the border
  size.Enlarge(-GetInsets().width(), -GetInsets().height());
  if (get_axis_background()->NormalSize(size)) {
    size.Enlarge(GetInsets().width(), GetInsets().height());
    return true;
  }
  return false;
}

int YTWaveContainerInnerView::BorderThickness() {
  return kBorderThickness;
}

AxisBackground* YTWaveContainerInnerView::get_axis_background() {
  Background* bg = background();
  DCHECK(bg) << "no background exist";
  return static_cast<AxisBackground*>(bg);
}

gfx::Size YTWaveContainerInnerView::GetMinimumSize() {
  gfx::Size size = get_axis_background()->GetMinimumSize();
  size.Enlarge(GetInsets().width(), GetInsets().height());
  return size;
}

void YTWaveContainerInnerView::SetGrid(int v_grid, int h_grid) {
  set_background(
    new AxisBackground(kWaveViewBackgroundColor, kAxisLineColor, 
                       kLineWidth, 
                       kAxisGridColor, 
                       v_grid, kVGridDiv, 
                       h_grid, kHGridDiv));
  // need check parent is there, may be call in creating, no add to parent yet
  if (parent()) {
    parent()->Layout();
  }
  SchedulePaint();
}

void YTWaveContainerInnerView::UpdateAxis() {
  Wave* wave = container_->GetSelectWave();
  YTWaveVisitor visitor(this);
  visitor.SetAxis(wave);
}

void YTWaveContainerInnerView::SelectWave(Wave* wave) {
  container_->SelectWave(wave);
}

gfx::Transform YTWaveContainerInnerView::SimpleAnaWaveTransform(SimpleAnaWave* ana_wave) {
  return GetSimpleAnaWaveView(ana_wave)->data_transform();
}

SimpleAnaWaveView* YTWaveContainerInnerView::GetSimpleAnaWaveView(SimpleAnaWave* ana_wave) {
  int id = container_->WaveAt(ana_wave);
  SimpleAnaWaveView* wave_view = static_cast<SimpleAnaWaveView*>(child_at(WaveIDToViewID(id)));
  return wave_view;
}

int YTWaveContainerInnerView::WaveIDToViewID(int wave_id) {
  return wave_id + kFrontSize;
}

