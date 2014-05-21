#include "wave_control/osc_wave_group/common_osc_wave_group.h"

#include "ui/gfx/path.h"
#include "ui/gfx/size.h"
#include "ui/gfx/canvas.h"

namespace {

bool IsSet(int set, int test) {
  return (set & test) != 0;
}


SkBitmap CreateIndicateIcon(int x, int y, bool horiz, SkColor color) {
  gfx::Size ps(x, y);  
  gfx::Canvas scale_canvas(ps, ui::SCALE_FACTOR_100P, false);

  SkPaint paint;
  paint.setColor(color);
  int paint_width = horiz ? x / 3 : y / 3;
  paint.setAntiAlias(true);
  if (horiz) {
    paint.setStrokeWidth(paint_width + 1);
    scale_canvas.DrawLine(gfx::Point(x - paint_width/2, 1), 
                          gfx::Point(x - paint_width/2, y), 
                          paint);
    paint.setStrokeWidth(paint_width);
    scale_canvas.DrawLine(gfx::Point(x/3,  y/2 + 1), gfx::Point(x, y/2 + 1), paint);
    gfx::Path::Point arrow[3] =  { {x/3, y/2 + 1 - x/6}, 
                                   {0, y/2 + 1}, 
                                   {x/3, y/2 + 1 + x/6} };
    paint.setStrokeWidth(1);
    paint.setStyle(SkPaint::kFill_Style);
    scale_canvas.DrawPath(gfx::Path(arrow, arraysize(arrow)), paint);

  } else {
    paint.setStrokeWidth(paint_width + 1);
    scale_canvas.DrawLine(gfx::Point(1, paint_width/2), 
                          gfx::Point(x, paint_width/2), 
                          paint);
    paint.setStrokeWidth(paint_width);
    scale_canvas.DrawLine(gfx::Point(x/2 + 1,  0), 
                          gfx::Point(x/2 + 1, y - y/3), 
                          paint);
    gfx::Path::Point arrow[3] =  { {x/2 + 1 - y/6, y - y/3}, 
                                   {x/2 + 1, y }, 
                                   {x/2 + 1 +y/6,  y - y/3} };
    paint.setStrokeWidth(1);
    paint.setStyle(SkPaint::kFill_Style);
    scale_canvas.DrawPath(gfx::Path(arrow, arraysize(arrow)), paint);
  }

  return scale_canvas.ExtractImageRep().sk_bitmap();
}

}

void CommonOscWaveGroup::AddOscWave(OscWave* osc_wave) {
  DCHECK(osc_wave);

  bool create_trigger = true;
  bool create_horizontal = true;
  bool create_vertical = true;
  OscWaveRecord target_record;
  target_record.wave = osc_wave;

  for (size_t i = 0; i < osc_waves_.size(); ++i) {
    OscWaveRecord& record = osc_waves_[i];
    if (record.wave == osc_wave) {
      NOTREACHED();
    }
    if (target_record.trigger.get() == NULL) {
      if (record.wave->IsSameTrigger(osc_wave)) {
        create_trigger = false;
        target_record.trigger = record.trigger;
      }
    }
    if (target_record.horizontal.get() == NULL) {
      if (record.wave->IsSameHorizontal(osc_wave)) {
        create_horizontal = false;
        target_record.horizontal = record.horizontal;
      }
    }
    if (target_record.vertical.get() == NULL) {
      if (record.wave->IsSameVertical(osc_wave)) {
        create_vertical = false;
        target_record.vertical = record.vertical;
      }
    }
  }
 
  if (create_trigger) {
    RefTriggerPart* trigger = new RefTriggerPart(osc_wave, this);
    triggers_.push_back(trigger);
    target_record.trigger = trigger;
  }
  if (create_horizontal) {
    RefHorizontalPart* horizontal = new RefHorizontalPart(osc_wave, this);
    horizontals_.push_back(horizontal);
    target_record.horizontal = horizontal;
  }
  if (create_vertical) {
    RefVerticalPart* vertical = new RefVerticalPart(osc_wave, this);
    verticals_.push_back(vertical);
    target_record.vertical = vertical;
  }

  osc_wave->AddObserver(this);
  osc_waves_.push_back(target_record);

  if (create_trigger) {
    NotifyTriggerGroupChanged();
  }
  if (create_horizontal) {
    NotifyHorizontalGroupChanged();
  }
  if (create_vertical) {
    NotifyVerticalGroupChanged();
  }
}

void CommonOscWaveGroup::RemoveOscWave(OscWave* osc_wave) {
  DCHECK(osc_wave);
  trigger_changed_ = false;
  horizontal_changed_ = false;
  vertical_changed_ = false;
  for (size_t i = 0; i < osc_waves_.size(); ++i) {
    OscWaveRecord& record = osc_waves_[i];
    if (record.wave == osc_wave) {
      osc_wave->RemoveObserver(this);
      osc_waves_.erase(osc_waves_.begin() + i);
      break;
    }
    if (i == osc_waves_.size() - 1) {
      NOTREACHED();
    }
  }
  std::vector<int> trigger_index;
  // update the ref to the remove wave to other wave.
  if (!trigger_changed_) {
    OscWave* new_wave = SameTriggerWave(osc_wave);
    DCHECK(new_wave);
    for (int i = 0; i < trigger_count(); ++i) {
      RefTriggerPart* part = static_cast<RefTriggerPart*>(trigger_at(i));
      if (part->osc_wave() == osc_wave) {
        part->set_osc_wave(new_wave);    
        trigger_index.push_back(i);
      }
    }
  }
  std::vector<int> horizontal_index;
  if (!horizontal_changed_) {
    OscWave* new_wave = SameHorizontalWave(osc_wave);
    DCHECK(new_wave);
    for (int i = 0; i < horizontal_count(); ++i) {
      RefHorizontalPart* part = static_cast<RefHorizontalPart*>(horizontal_at(i));
      if (part->osc_wave() == osc_wave) {
        part->set_osc_wave(new_wave);
        horizontal_index.push_back(i);
      }
    }
  }
  std::vector<int> vertical_index;
  if (!vertical_changed_) {
    OscWave* new_wave = SameVerticalWave(osc_wave);
    DCHECK(new_wave);
    for (int i = 0; i < vertical_count(); ++i) {
      RefVerticalPart* part = static_cast<RefVerticalPart*>(vertical_at(i));
      if (part->osc_wave() == osc_wave) {
        part->set_osc_wave(new_wave);
        vertical_index.push_back(i);
      }
    }
  }
  for (size_t i = 0; i < trigger_index.size(); ++i) {
    NotifyTriggerChanged(trigger_index[i]);
  }

  for (size_t i = 0; i < horizontal_index.size(); ++i) {
    NotifyHorizontalChanged(horizontal_index[i]);
  }
  for (size_t i = 0; i < vertical_index.size(); ++i) {
    NotifyHorizontalChanged(vertical_index[i]);
  }
  if (trigger_changed_) {
    NotifyTriggerGroupChanged();
  }
  if (horizontal_changed_) {
    NotifyHorizontalGroupChanged();
  }
  if (vertical_changed_) {
    NotifyVerticalGroupChanged();
  }
}

OscWave* CommonOscWaveGroup::SameTriggerWave(OscWave* osc_wave) {
  for (size_t i = 0; i < osc_waves_.size(); ++i) {
    OscWaveRecord& record = osc_waves_[i];
    if (record.wave == osc_wave) {
      continue;
    }
    if (record.wave->IsSameTrigger(osc_wave)) {
      return record.wave;
    }
  }
  return NULL;
}

OscWave* CommonOscWaveGroup::SameHorizontalWave(OscWave* osc_wave) {
  for (size_t i = 0; i < osc_waves_.size(); ++i) {
    OscWaveRecord& record = osc_waves_[i];
    if (record.wave == osc_wave) {
      continue;
    }
    if (record.wave->IsSameHorizontal(osc_wave)) {
      return record.wave;
    }
  }
  return NULL;
}

OscWave* CommonOscWaveGroup::SameVerticalWave(OscWave* osc_wave) {
  for (size_t i = 0; i < osc_waves_.size(); ++i) {
    OscWaveRecord& record = osc_waves_[i];
    if (record.wave == osc_wave) {
      continue;
    }
    if (record.wave->IsSameVertical(osc_wave)) {
      return record.wave;
    }
  }
  return NULL;
}

bool CommonOscWaveGroup::HasOscWave(OscWave* osc_wave) {
  for (size_t i = 0; i < osc_waves_.size(); ++i) {
    OscWaveRecord& record = osc_waves_[i];
    if (record.wave == osc_wave) {
      return true;
    }
  }
  return false;
}

void CommonOscWaveGroup::OnTriggerDelete(TriggerPart* trigger) {
  Triggers::iterator it = find(triggers_.begin(), triggers_.end(), trigger);
  DCHECK(it != triggers_.end());
  triggers_.erase(it);
  trigger_changed_ = true;
}

void CommonOscWaveGroup::OnHorizontalDelete(HorizontalPart* horizontal) {
  Horizontals::iterator it = find(horizontals_.begin(), horizontals_.end(), horizontal);
  DCHECK(it != horizontals_.end());
  horizontals_.erase(it);
  horizontal_changed_ = true;
}

void CommonOscWaveGroup::OnVerticalDelete(VerticalPart* vertical) {
  Verticals::iterator it = find(verticals_.begin(), verticals_.end(), vertical);
  DCHECK(it != verticals_.end());
  verticals_.erase(it);
  vertical_changed_ = true;
}

bool CommonOscWaveGroup::NeedUpdateTriggerOffset(TriggerPart* trigger, OscWave* osc_wave) {
  if (!trigger->IsRelate()) {
    return false;
  }
  return trigger->trigger_wave() == osc_wave;
}

void CommonOscWaveGroup::OnOscWaveChanged(OscWave* osc_wave, int change_set) {
  OscWaveRecord& record = GetOscWaveRecord(osc_wave);

  if (IsSet(change_set, OscWave::kTrigger)) {
    record.trigger->UpdateIcon();
    NotifyTriggerChanged(TriggerIndex(record.trigger.get()));
  }
  if (IsSet(change_set, OscWave::kTriggerOffset)) {
    NotifyTriggerMoved(TriggerIndex(record.trigger.get()));
  }
  if (IsSet(change_set, OscWave::kHorizontal)) {
    record.horizontal->UpdateIcon();
    NotifyHorizontalChanged(HorizontalIndex(record.horizontal.get()));
  }
  if (IsSet(change_set, OscWave::kHorizontalOffset)) {
    NotifyHorizontalMoved(HorizontalIndex(record.horizontal.get()));
  }
  if (IsSet(change_set, OscWave::kVertical)) {
    NotifyVerticalChanged(VerticalIndex(record.vertical.get()));
  }
  if (IsSet(change_set, OscWave::kVerticalOffset)) {
    NotifyVerticalMoved(VerticalIndex(record.vertical.get()));
    for (size_t i = 0; i < triggers_.size(); ++i) {
      if (NeedUpdateTriggerOffset(triggers_[i], osc_wave)) {
        NotifyTriggerMoved(TriggerIndex(triggers_[i]));
      }
    }
  }
}

OscWaveRecord& CommonOscWaveGroup::GetOscWaveRecord(OscWave* osc_wave) {
  for (size_t i = 0; i < osc_waves_.size(); ++i) {
    OscWaveRecord& record = osc_waves_[i];
    if (record.wave == osc_wave) {
      return record;
    }
  }
  NOTREACHED();
  return osc_waves_[0];
}

int CommonOscWaveGroup::TriggerIndex(TriggerPart* part) {
  for (size_t i = 0; i < triggers_.size(); ++i) {
    if (triggers_[i] == part) {
      return i;
    }
  }
  NOTREACHED();
  return 0;
}

int CommonOscWaveGroup::HorizontalIndex(HorizontalPart* part) {
  for (size_t i = 0; i < horizontals_.size(); ++i) {
    if (horizontals_[i] == part) {
      return i;
    }
  }
  NOTREACHED();
  return 0;
}

int CommonOscWaveGroup::VerticalIndex(VerticalPart* part) {
  for (size_t i = 0; i < verticals_.size(); ++i) {
    if (verticals_[i] == part) {
      return i;
    }
  }
  NOTREACHED();
  return 0;
}

CommonOscWaveGroup::CommonOscWaveGroup()
    : trigger_changed_(false)
    , horizontal_changed_(false)
    , vertical_changed_(false) {

}

CommonOscWaveGroup::~CommonOscWaveGroup() {
  for (size_t i = 0; i < osc_waves_.size(); ++i) {
    OscWaveRecord& record = osc_waves_[i];
    record.wave->RemoveObserver(this);
  }
  osc_waves_.clear();
}

// RefTriggerPart

SkColor RefTriggerPart::color() {
  return osc_wave_->trigger_color();
}

bool RefTriggerPart::show() {
  return osc_wave_->trigger_show();
}

string16 RefTriggerPart::text() {
  return L"";
}

const gfx::Image& RefTriggerPart::icon() {
  return icon_;
}

WaveRange RefTriggerPart::offset_range() {
  return osc_wave_->trigger_offset_range();
}

double RefTriggerPart::offset() {
  return osc_wave_->trigger_offset();
}

void RefTriggerPart::set_offset(double offset) {
  osc_wave_->MoveTrigger(offset);
}

bool RefTriggerPart::IsRelate() {
  return osc_wave_->trigger_is_relate();
}

OscWave* RefTriggerPart::trigger_wave() {
  return osc_wave_->trigger_wave();
}


void RefTriggerPart::UpdateIcon() {
  icon_ = gfx::Image::CreateFrom1xBitmap(
      CreateIndicateIcon(15, 19, true, color()));
}

RefTriggerPart::RefTriggerPart(OscWave* osc_wave, CommonOscWaveGroup* wave_group)
    : osc_wave_(osc_wave)
    , wave_group_(wave_group) {
  DCHECK(osc_wave);
  DCHECK(wave_group);
  UpdateIcon();
}

RefTriggerPart::~RefTriggerPart() {
  wave_group_->OnTriggerDelete(this);
}

// RefHorizontalPart
SkColor RefHorizontalPart::color() {
  return osc_wave_->horizontal_color();
}

bool RefHorizontalPart::show() {
  return osc_wave_->horizontal_show();
}

string16 RefHorizontalPart::text() {
  return L"";
}

const gfx::Image& RefHorizontalPart::icon() {
  return icon_;
}

WaveRange RefHorizontalPart::range() {
  return osc_wave_->horizontal_range();
}

WaveRange RefHorizontalPart::offset_range() {
  return osc_wave_->horizontal_offset_range();
}

double RefHorizontalPart::offset() {
  return osc_wave_->horizontal_offset();
}

int RefHorizontalPart::div() {
  return osc_wave_->horizontal_div();
}

int RefHorizontalPart::window_size() {
  return osc_wave_->horizontal_window_size();
}

RefHorizontalPart::RefHorizontalPart(OscWave* osc_wave, CommonOscWaveGroup* wave_group)
    : osc_wave_(osc_wave)
    , wave_group_(wave_group) {
  DCHECK(osc_wave);
  DCHECK(wave_group);
  UpdateIcon();
}

RefHorizontalPart::~RefHorizontalPart() {
  wave_group_->OnHorizontalDelete(this);
}

void RefHorizontalPart::UpdateIcon() {
  icon_ = gfx::Image::CreateFrom1xBitmap(
    CreateIndicateIcon(19, 15, false, color()));
}

// RefVerticalPart
SkColor RefVerticalPart::color() {
  return osc_wave_->vertical_color();
}

bool RefVerticalPart::show() {
  return osc_wave_->vertical_show();
}

string16 RefVerticalPart::text() {
  return osc_wave_->name();
}

const gfx::Image& RefVerticalPart::icon() {
  return icon_;
}

WaveRange RefVerticalPart::range() {
  return osc_wave_->vertical_range();
}

WaveRange RefVerticalPart::offset_range() {
  return osc_wave_->vertical_offset_range();
}

double RefVerticalPart::offset() {
  return osc_wave_->vertical_offset();
}

int RefVerticalPart::div() {
  return osc_wave_->vertical_div();
}

int RefVerticalPart::window_size() {
  return osc_wave_->vertical_window_size();
}

RefVerticalPart::RefVerticalPart(OscWave* osc_wave, CommonOscWaveGroup* wave_group)
    : osc_wave_(osc_wave)
    , wave_group_(wave_group) {
  DCHECK(osc_wave);
  DCHECK(wave_group);
}

RefVerticalPart::~RefVerticalPart() {
  wave_group_->OnVerticalDelete(this);
}

