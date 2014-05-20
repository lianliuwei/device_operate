#pragma once

#include "ui/views/view.h"

#include "wave_control/yt_wave_container.h"
#include "wave_control/views/yt_wave_container_inner_view.h"
#include "wave_control/views/handle_bar.h"

class WaveControlView;

// just layout the handle bar and view.
class YTWaveContainerView : public views::View {
public:
  YTWaveContainerView(YTWaveContainer* container, WaveControlView* wave_control_view);
  virtual ~YTWaveContainerView();

  void show_wave_bar(bool visible);
  bool is_show_wave_bar();
  void show_horiz_offset_bar();
  bool is_show_horiz_offset_bar();
  void show_trigger_bar();
  bool is_show_trigger_bar();

  void ActiveWaveBarHandle(int ID);
  bool GetWaveBarActiveHandleID(int* id);

  void set_wave_bar_size(int wave_bar_size);

private:
  // Overridden from the views::View
  virtual void Layout() OVERRIDE;

  HandleBar* CreateHandleBar(HandleBarDelegate* delegate, 
                             views::TextButtonBase::TextAlignment align);

  YTWaveContainerInnerView* yt_view_;

  // the handlebars are the container's children. but the YTView manager
  // those.
  HandleBar* wave_bar_;
  HandleBar* horiz_offset_bar_;
  HandleBar* trigger_bar_;

  bool show_wave_bar_;
  bool show_horiz_offset_bar_;
  bool show_trigger_bar_;

  bool set_wave_bar_size_;
  int wave_bar_size_;

  DISALLOW_COPY_AND_ASSIGN(YTWaveContainerView);
};