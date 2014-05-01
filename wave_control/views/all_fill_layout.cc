#include "wave_control/views/all_fill_layout.h"

using namespace views;

AllFillLayout::AllFillLayout() {}

AllFillLayout::~AllFillLayout() {}

void AllFillLayout::Layout(View* host) {
  for (int i = 0; i < host->child_count(); ++i) {
    View* frame_view = host->child_at(i);
    frame_view->SetBoundsRect(host->GetContentsBounds());
  }
}

gfx::Size AllFillLayout::GetPreferredSize(View* host) {
  gfx::Rect rect(host->child_at(0)->GetPreferredSize());
  rect.Inset(-host->GetInsets());
  return rect.size();
}

void AllFillLayout::ViewAdded(View* host, View* view) {
  view->SetBoundsRect(host->GetContentsBounds());
}
