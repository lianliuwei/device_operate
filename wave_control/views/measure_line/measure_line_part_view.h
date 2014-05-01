#pragma once

#include "ui/views/view.h"

// horiz mean line
class MeasureLinePartView : public views::View {
public:
  class Delegate {
  public:
    virtual ~Delegate() {}

    virtual void OnPosChanged(MeasureLinePartView* part_view, int pos) = 0;
  };

  MeasureLinePartView(bool horiz, Delegate* delegate);
  virtual ~MeasureLinePartView() {}

  // overridden from views::View
  virtual void OnPaint(gfx::Canvas* canvas) OVERRIDE;
  virtual bool OnMousePressed(const ui::MouseEvent& event) OVERRIDE;
  virtual bool OnMouseDragged(const ui::MouseEvent& event) OVERRIDE;
  virtual void OnMouseReleased(const ui::MouseEvent& event) OVERRIDE;
  virtual gfx::NativeCursor GetCursor(const ui::MouseEvent& event) OVERRIDE;

  bool horiz() const { return horiz_;}
  // measure x y for vertical horiz
  int line_point() const; 
  // for parent layout
  gfx::Rect RectForPoint(int point, gfx::Rect parent_rect);

  // if horiz this mean last x, vertical mean last y.
  int other_pos();

private:
  bool horiz_;
  gfx::Point start_point_;
  Delegate* delegate_;
  int other_pos_;

  DISALLOW_COPY_AND_ASSIGN(MeasureLinePartView);
};