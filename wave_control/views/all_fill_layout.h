#pragma once

#include "base/compiler_specific.h"
#include "ui/views/layout/layout_manager.h"
#include "ui/views/view.h"

class AllFillLayout : public views::LayoutManager {
public:
  AllFillLayout();
  virtual ~AllFillLayout();

  // Overridden from LayoutManager:
  virtual void Layout(views::View* host) OVERRIDE;
  virtual gfx::Size GetPreferredSize(views::View* host) OVERRIDE;

private:
  DISALLOW_COPY_AND_ASSIGN(AllFillLayout);
};
