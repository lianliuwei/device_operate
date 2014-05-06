#include "wave_control/views/handle_bar.h"

#include "base/stl_util.h"
#include "wave_control/views/handle_bar_model.h"
#include "wave_control/views/handle_bar_observer.h"

using namespace base;
using namespace views;

HandleBar::HandleBar(HandleBarModel* model, 
                     bool is_horiz, 
                     gfx::Font font,
                     int start, int end)
    : model_(NULL)
    , is_horiz_(is_horiz)
    , font_(font) {
  SetMoveRange(start, end);
  SetModel(model);

}

HandleBar::~HandleBar() {
  SetModel(NULL);
}

void HandleBar::OnModelChanged() {
  UpdateFromModel();
}

void HandleBar::OnHandleChanged(int ID) {
  UpdateHandle(ID);
}

void HandleBar::OnHandleMoved(int ID) {
  UpdateHandlePos(ID);
}

bool HandleBar::IsHorizontal() const {
  return is_horiz_;
}

void HandleBar::SetModel(HandleBarModel* model) {
  if (model == model_)
    return;

  if (model_)
    model_->RemoveObserver(this);
  model_ = model;
  if (model_)
    model_->AddObserver(this);
  UpdateFromModel();
}

void HandleBar::ActiveHandle(int ID) {
  Handle* handle = GetHandle(ID);
  // move the handle to the end so it's draw first, look like it's on top.
  ReorderChildView(handle, -1);
  // draw the handle to show the on top effect.
  handle->SchedulePaint();
  // notify the observer the ID handle is active.
  NotifyHandleActive(ID);
}


bool HandleBar::GetActiveHandleID(int* id) {
  if (child_count() == 0) {
    return false;
  }
  int ID;
  View* view = child_at(child_count() - 1);
  Handle* handle = static_cast<Handle*>(view);
  ID = handle->tag();
  if (id) {
    *id = ID;
  }
  return true;
}

void HandleBar::OnHandlePressed(int id, int dest) {
  Handle* handle = GetHandle(id);
  DCHECK(handle) << "can not find the handle from ID: " << id;
  int offset = CalculateOffset(dest, IsHorizontal() ? handle->width() :
    handle->height());
  NotifyHandlePressed(id, offset);
}


void HandleBar::OnHandleReleased(int id) {
  NotifyHandleReleased(id);
}

void HandleBar::OnHandleMove(int ID, int dest) {
  Handle* handle = GetHandle(ID);
  DCHECK(handle) << "can not find the handle from ID: " << ID;
  int offset = CalculateOffset(dest, IsHorizontal() ? handle->width() :
                               handle->height());
  NotifyHandleMove(ID, offset);
}

void HandleBar::UpdateFromModel() {
  for (std::vector<Handle*>::iterator it = handles_.begin();
       it != handles_.end();
       it++) {
    RemoveChildView(*it);
    delete *it;
  }
  handles_.clear();
  // may be no model
  if (model_) {
    for (int i = 0; i < model_->Count(); i++) {
      Handle* handle = new Handle(this);
      SetHandle(handle, model_->GetID(i));
      SetHandlePos(handle, model_->GetID(i));
      AddChildView(handle);
      handles_.push_back(handle);
    }
  }
}

void HandleBar::UpdateHandle(int ID) {
  Handle* handle = GetHandle(ID);
  SetHandle(handle, ID);
  SetHandlePos(handle, ID);
}

void HandleBar::UpdateHandlePos(int ID) {
  Handle* handle = GetHandle(ID);
  SetHandlePos(handle, ID);
}

// offset = dest + width / 2 - start_
int HandleBar::CalculateOffset(int dest, int width) const {
  // now the middle of the handle control is the offset value
  return dest + width / 2 - start_;
}

int HandleBar::CalculateDest(int offset, int width) const {
  // from the middle calcuate the handle x or y
  return offset - width / 2 + start_;
}

void HandleBar::SetHandle(Handle* handle, int ID) {
  handle->SetVisible(model_->IsVisible(ID));
  handle->SetEnabled(model_->IsEnable(ID));
  handle->SetFont(font_);
  SkColor color = model_->GetColor(ID);
  handle->SetEnabledColor(color);
  handle->SetDisabledColor(color);
  handle->SetHoverColor(color);
  handle->ClearMaxTextSize(); // so can be small
  handle->SetText(model_->GetText(ID));
  const gfx::Image image = model_->GetIcon(ID);
  if (!image.IsEmpty()) {
    const gfx::ImageSkia* skia = image.ToImageSkia();
    handle->SetIcon(*skia);
    handle->SetHoverIcon(*skia);
    handle->SetPushedIcon(*skia);
  }
  
  handle->set_tag(ID);
  handle->set_icon_placement(TextButton::ICON_ON_LEFT);
  handle->set_alignment(TextButton::ALIGN_CENTER);
}

void HandleBar::SetHandlePos(Handle* handle, int ID) {
  int w = IsHorizontal() ? handle->width() : handle->height();
  int dest = CalculateDest(model_->GetOffset(ID), w);
  IsHorizontal() ? handle->SetX(dest) : handle->SetY(dest);
  // out of range no show
  // must no interrupt the Drag Handle, can move outside but after that can no
  // click the handle again
  int offset = model_->GetOffset(ID);
  // using the real index pos, the CalculateDest() is the handle control offset 
  // no the index pos, model_->GetOffset(ID) is the logic offset no the real pos
  offset += start_;
  bool enable = handle->enabled();
  // SetVisible will break mouse capture
  //if (offset < start_ || offset > end_) {
  //    handle->SetVisible(false);
  //} else {
  //  //restore the model enable state
  //  handle->SetVisible(model_->IsEnable(ID));
  //}
}

void HandleBar::SetMoveRange(int start, int end) {
  CHECK(start < end);
  bool need_redraw = false;
  if (start_ != start) {
      start_ = start;
      need_redraw = true;
  }
  if (end_ != end) {
      end_ = end;
      need_redraw = true;
  }
  if (need_redraw) {
      // need repost the handles
      InvalidateLayout();
  }
  
}

void HandleBar::Layout() {
  for (std::vector<Handle*>::iterator it = handles_.begin();
       it != handles_.end(); it++) {
    (*it)->SetSize(IsHorizontal() ?
                   gfx::Size((*it)->GetPreferredSize().width(), height())
                   : gfx::Size(width(), (*it)->GetPreferredSize().height()));
    SetHandlePos(*it, (*it)->tag());
  }
}

gfx::Size HandleBar::GetPreferredSize() {
  if (IsHorizontal()) {
    int height = 0;
    for (std::vector<Handle*>::iterator it = handles_.begin();
         it != handles_.end(); it++)
      height = std::max(height, (*it)->GetPreferredSize().height());
    return gfx::Size(0, height);
  } else {
    int width = 0;
    for (std::vector<Handle*>::iterator it = handles_.begin();
         it != handles_.end(); it++)
      width = std::max(width, (*it)->GetPreferredSize().width());
    return gfx::Size(width, 0);
  }
}

gfx::Size HandleBar::GetMinimumSize() {
  int height = 0;
  int width = 0;
  for (std::vector<Handle*>::iterator it = handles_.begin();
       it != handles_.end(); it++) {
    height = std::max(height, (*it)->GetMinimumSize().height());
    width = std::max(width, (*it)->GetMinimumSize().width());
  }
  return gfx::Size(width, height);
}

Handle* HandleBar::GetHandle(int ID) const {
  for (std::vector<Handle*>::const_iterator it = handles_.begin();
       it != handles_.end(); it++)
    if ((*it)->tag() == ID)
      return *it;
  NOTREACHED();
  return NULL;
}

void HandleBar::NotifyHandlePressed(int id, int offset) {
  FOR_EACH_OBSERVER(HandleBarObserver, observer_list_, OnHandlePressed(id, offset, is_horiz_));
}

void HandleBar::NotifyHandleMove(int id, int offset) {
  FOR_EACH_OBSERVER(HandleBarObserver, observer_list_, OnHandleMove(id, offset));
}

void HandleBar::NotifyHandleReleased(int id) {
  FOR_EACH_OBSERVER(HandleBarObserver, observer_list_, OnHandleReleased(id));
}

void HandleBar::NotifyHandleActive(int id) {
  FOR_EACH_OBSERVER(HandleBarObserver, observer_list_, OnHandleActive(id));

}
