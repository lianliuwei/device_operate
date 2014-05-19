#pragma once

#include "base/observer_list.h"
#include "ui/base/models/list_model_observer.h"
#include "ui/base/models/simple_menu_model.h"
#include "ui/views/view.h"
#include "ui/views/controls/menu/menu_runner.h"

#include "wave_control/wave_container.h"
#include "wave_control/osc_wave_group/osc_wave_group.h"
#include "wave_control/views/axis_background.h"
#include "wave_control/views/handle_bar_model.h"
#include "wave_control/views/handle_bar_observer.h"
#include "wave_control/views/handle_bar_model_observer.h"
#include "wave_control/views/measure_line/measure_line_container_view.h"

class HandleBarDelegate : public HandleBarModel
                        , public HandleBarObserver {
public:
  HandleBarDelegate() {}

  virtual bool is_horiz() = 0;

  // implement HandleBarModel
  virtual void AddObserver(HandleBarModelObserver* observer);
  virtual void RemoveObserver(HandleBarModelObserver* observer);
  virtual bool HasObserver(HandleBarModelObserver* observer);

protected:
  virtual ~HandleBarDelegate() {}

  void NotifyModelChanged();
  void NotifyHandleChanged(int id);
  void NotifyHandleMoved(int id);

  ObserverList<HandleBarModelObserver> observer_list_;

  DISALLOW_COPY_AND_ASSIGN(HandleBarDelegate);
};
namespace {
class HorizOffsetBar;
class TriggerBar;
class WaveBar;
class YTWaveVisitor;
}

class SimpleAnaWaveView;
class YTWaveContainerView;
class WaveDragController;

// real YTWaveContainer show view, the YTWaveContainerView
// just frame
class YTWaveContainerInnerView : public views::View
                               , public ui::ListModelObserver
                               , public MeasureLineContainerView::Delegate
                               , public ui::SimpleMenuModel::Delegate {
public:
  YTWaveContainerInnerView(YTWaveContainer* container);
  virtual ~YTWaveContainerInnerView();

  // get the HandleBarDelegate for move wave and select wave. the YTViewContainer 
  // use this delegate to create handlebar. and it control the Bar
  // show or no and where it show. YTView control how many handle show, and
  // where it show, how it show. the HorizOffsetBar and TriggerBar is the same
  HandleBarDelegate* GetWaveBarDelegate();
  HandleBarDelegate* GetHorizOffsetBarDelegate();
  HandleBarDelegate* GetTriggerBarDelegate();

  // add this to all HandleBar
  HandleBarObserver* HandlePointDelegate();

  // osc coord transform
  gfx::Transform OscWaveTransform(OscWave* osc_wave);
  static double ToOscOffset(double old_offset, double move_delta);

  // SimpleAnaWave coord transform
  gfx::Transform SimpleAnaWaveTransform(SimpleAnaWave* ana_wave);
  int GetYOffset(SimpleAnaWave* wave);
  void MoveToY(SimpleAnaWave* wave, int offset);
  SimpleAnaWaveView* GetSimpleAnaWaveView(SimpleAnaWave* ana_wave);

  void SelectWave(Wave* wave);

  // see commit in axis_painter.h
  bool NormalSize(gfx::Size& size);

  // now assert the border width are all the same, it user for the container
  // to layout the assist handlebar.
  int BorderThickness();

  bool HasWave(Wave* wave);

  // override views::View
  gfx::Size GetMinimumSize() OVERRIDE;
  virtual View* GetEventHandlerForPoint(const gfx::Point& point) OVERRIDE;
  virtual void Layout() OVERRIDE;
  virtual void PaintChildren(gfx::Canvas* canvas) OVERRIDE;
  virtual bool OnMousePressed(const ui::MouseEvent& event) OVERRIDE;
  virtual bool OnMouseDragged(const ui::MouseEvent& event) OVERRIDE;
  virtual void OnMouseReleased(const ui::MouseEvent& event) OVERRIDE;
  virtual void OnMouseCaptureLost() OVERRIDE;
  virtual void ViewHierarchyChanged(const ViewHierarchyChangedDetails& details) OVERRIDE;

  bool ContainerEvent(const ui::MouseEvent& event);

private:
  // implement MeasureLineContainerView::Delegate
  virtual Wave* GetMeasureWave();
  virtual const gfx::Transform GetMeasureWaveTransform();

  // implement SimpleMenuModel::Delegate
  virtual bool IsCommandIdChecked(int command_id) const;
  virtual bool IsCommandIdEnabled(int command_id) const;
  virtual bool GetAcceleratorForCommandId(int command_id, ui::Accelerator* accelerator);
  virtual void ExecuteCommand(int command_id, int event_flags);

  // implement ui::ListModelObserver
  virtual void ListItemsAdded(size_t start, size_t count);
  virtual void ListItemsRemoved(size_t start, size_t count);
  virtual void ListItemMoved(size_t index, size_t target_index);
  virtual void ListItemsChanged(size_t start, size_t count);

  void OnSelectWaveChanged();

  WaveDragController* GetDragController();

  AxisBackground* get_axis_background();
  void SetGrid(int v_grid, int h_grid);
  void UpdateAxis();
  int WaveIDToViewID(int wave_id);

  ui::SimpleMenuModel* GetMenuModel();
  void CancelMenu();

  YTWaveContainerView* container_view() const;

  friend class YTWaveVisitor;

  bool inited_;

  scoped_ptr<WaveBar> wave_bar_;
  scoped_ptr<HorizOffsetBar> horiz_offset_bar_;
  scoped_ptr<TriggerBar> trigger_bar_;

  scoped_ptr<OscWaveGroup> wave_group_;

  MeasureLineContainerView* measure_line_view_;
  bool inited_measure_line_;

  scoped_ptr<views::MenuRunner> menu_runner_;
  scoped_ptr<ui::SimpleMenuModel> menu_model_;

  YTWaveContainer* container_;

  // HACK for sync with wave LiistModel, the RemoveWave need Wave ptr.
  std::vector<Wave*> wave_record_;

  DISALLOW_COPY_AND_ASSIGN(YTWaveContainerInnerView);
};