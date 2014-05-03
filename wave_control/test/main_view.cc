#include "wave_control/test/example_view.h"

#include "ui/views/controls/button/text_button.h"
#include "ui/views/controls/label.h"
#include "wave_control/test/dragged_thumb_view.h"

using namespace views;
using namespace gfx;
using namespace std;

class TestView : public View {
public:
  TestView();
  virtual ~TestView() {}

  virtual void Layout();
  virtual bool OnMousePressed(const ui::MouseEvent& event);
  virtual bool OnMouseDragged(const ui::MouseEvent& event);
  virtual void OnMouseReleased(const ui::MouseEvent& event);
  virtual void OnMouseCaptureLost();


  TextButton* button_;
  Label* label_;
  scoped_ptr<DraggedThumbView> drag_view_;
};

TestView::TestView() {
  set_background(Background::CreateSolidBackground(SkColorSetRGB(255, 255, 255)));
  button_ = new TextButton(NULL, L"Test For Drag");
  label_ = new Label(L"Label for Drag");
  label_->SetEnabledColor(SkColorSetRGB(0, 0, 0));
  label_->SetBackgroundColor(SkColorSetRGB(255, 255, 255));
  AddChildView(button_);
  AddChildView(label_);
}

void TestView::Layout() {
  Rect b_rect(Point(50, 50), Size(100, 50));
  Rect l_rect(Point(50, 250), Size(100, 50));
  button_->SetBoundsRect(b_rect);
  label_->SetBoundsRect(l_rect);
}

bool TestView::OnMousePressed(const ui::MouseEvent& event) {
  vector<View*> renderers;
  renderers.push_back(this);
  vector<Rect> r_bounds;
  r_bounds.push_back(GetLocalBounds());
  Rect paint_bounds = GetLocalBounds();
  paint_bounds.Offset(Vector2d(10, 10));
  drag_view_.reset(new DraggedThumbView(renderers, r_bounds, paint_bounds));  

  return true;
}

bool TestView::OnMouseDragged(const ui::MouseEvent& event) {
  Point point = event.location();
  ConvertPointToScreen(this, &point);
  drag_view_->MoveTo(point);

  return true;
}

void TestView::OnMouseReleased(const ui::MouseEvent& event) {
  drag_view_.reset(NULL);
}

void TestView::OnMouseCaptureLost() {
  drag_view_.reset(NULL);
}


void ExampleView::Init() {
  DCHECK(contents_ == NULL) << "Run called more than once.";
  contents_ = new TestView;

  views::Widget* window =
    views::Widget::CreateWindowWithBounds(this, gfx::Rect(0, 0, 850, 400));

  window->Show();
}

ExampleView* ExampleView::CreateInstance() {
  return new ExampleView();
}