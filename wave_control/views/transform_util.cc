#include "wave_control/views/transform_util.h"

#include <cmath>

#include "base/logging.h"
#include "ui/gfx/point.h"

namespace {
int SymmetricRound(double x) {
  return static_cast<int>(
    x > 0
    ? std::floor(x + 0.5f)
    : std::ceil(x - 0.5f));
}
}

int TransformX(const gfx::Transform transform, int x) {
  gfx::Point point(x, 0);
  transform.TransformPoint(point);
  return point.x();
}

int TransformX(const gfx::Transform transform, double x) {
  SkScalar p[4] = {
    SkDoubleToScalar(x),
    0,
    0,
    1};
    transform.matrix().map(p);
    return SymmetricRound(p[0]);
}

int TransformReverseX(const gfx::Transform transform, int x) {
  gfx::Point point(x, 0);
  bool ret = transform.TransformPointReverse(point);
  CHECK(ret) << "Invalid transform matrix";
  return point.x();
}

int TransformY(const gfx::Transform transform, double y) {
  SkScalar p[4] = {
    0,
    SkDoubleToScalar(y),
    0,
    1};
    transform.matrix().map(p);
    return SymmetricRound(p[1]);
}

int TransformReverseY(const gfx::Transform transform, int y) {
  gfx::Point point(0, y);
  bool ret = transform.TransformPointReverse(point);
  CHECK(ret) << "Invalid transform matrix";
  return point.y();
}

