#pragma once

#include <vector>

#include "base/observer_list.h"

#include "wave_control/views/handle_bar_model_observer.h"
#include "wave_control/views/handle.h"

class HandleBarModel;
class HandleBarObserver;

// have n Handle, the user can drag and move handle. the HandleBar notify the 
// HandleBarModel when the handle pos change. now the middle of the handle 
// represent the offset.
class HandleBar : public views::View
                , public HandleBarModelObserver {
public:
  HandleBar(HandleBarModel* model, bool is_horiz, 
            gfx::Font font,
            int start, int end);
  // the views delete handle auto
  virtual ~HandleBar();

  bool IsHorizontal() const;

  void SetModel(HandleBarModel* model);
  HandleBarModel* model() const {
    return model_;
  }

  void AddObserver(HandleBarObserver* observer) { 
    observer_list_.AddObserver(observer); 
  }
  void RemoveObserver(HandleBarObserver* observer) {
    observer_list_.RemoveObserver(observer);
  }
  bool HasObserver(HandleBarObserver* observer) {
    return observer_list_.HasObserver(observer);
  }
  
  // put the Active Handle on the Top
  void ActiveHandle(int ID);

  void OnHandlePressed(int id, int dest);
  void OnHandleReleased(int id);
  // the Handle call this method to set it's offset. the handle must no set
  // the offset by itself, the model may be restrict the offset for some 
  // reason.
  // dest is the x or y the Handle need to be.
  void OnHandleMove(int ID, int dest);

  // HandleBarModelObserver methods.
  virtual void OnModelChanged();
  virtual void OnHandleChanged(int ID);
  virtual void OnHandleMoved(int ID);

  // set the move range of the Handle if Handle outside of the Range show
  // indicate. 
  // after call this must call layout to replace the handle according to the
  // new start and end.
  // TODO using start and length, for more excat.
  void SetMoveRange(int start, int end);

  void SetFont(const gfx::Font& font);
  // Return the font used by this button.
  gfx::Font font() const {
    return font_;
  }

  // if vertical, the size width is the most wide of handles PerferredSize.
  // the height is no meaning
  virtual gfx::Size GetPreferredSize() OVERRIDE;

  // if vertical, the size width is the most wide of handles of handle Minimumsize.
  // the height is the most high of handle Minimumsize.
  virtual gfx::Size GetMinimumSize()OVERRIDE;

protected:
  // reset the HandleBar and using the model to create Handles.
  virtual void UpdateFromModel();

  // subclass can use for replace the default Create Handle method
  virtual void UpdateHandle(int ID);

  // only reset the pos of the handle
  virtual void UpdateHandlePos(int ID);

  virtual Handle* GetHandle(int ID) const;

  // ui:views methods
  // if the HandleBar is vertical, all Handle has the same width, the perferred
  // height, and place by the offset.
  // must set the MoveRange 
  virtual void Layout() OVERRIDE;

  // from the handle dest arg to the offset set to model
  virtual int CalculateOffset(int dest, int width) const;

  // from the offset to the pos of Handle
  virtual int CalculateDest(int offset, int width) const;

private:
  // set the handle correct, the handle is subclass from the textBotton, need to 
  // set the three type icon, any many color.
  void SetHandle(Handle* handle, int ID);
  void SetHandlePos(Handle* handle, int ID);

  void NotifyHandlePressed(int id, int offset);
  void NotifyHandleMove(int id, int offset);
  void NotifyHandleReleased(int id);
  void NotifyHandleActive(int id);

  const bool is_horiz_;

  std::vector<Handle*> handles_;

  ObserverList<HandleBarObserver> observer_list_;

  HandleBarModel* model_;

  gfx::Font font_;

  // the start_ and end_ is set by the SetMoveRange. the start_ is less than the
  // end_ the start_ is the point of the offset zero. start_ is using to calculate
  // the offset.
  int start_;

  int end_;

  DISALLOW_COPY_AND_ASSIGN(HandleBar);
};

