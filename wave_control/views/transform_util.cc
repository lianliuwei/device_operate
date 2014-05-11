#include "wave_control/views/transform_util.h"

#include <cmath>

#include "base/logging.h"
#include "ui/gfx/point.h"
#include "ui/gfx/point3_f.h"

namespace {
int SymmetricRound(double x) {
  return static_cast<int>(
    x > 0
    ? std::floor(x + 0.5f)
    : std::ceil(x - 0.5f));
}
}

int XInt(const gfx::Transform& transform, double x) {
  SkScalar p[4] = {
    SkDoubleToScalar(x),
    0,
    0,
    1};
    transform.matrix().map(p);
    return SymmetricRound(p[0]);
}

int YInt(const gfx::Transform& transform, double y) {
  SkScalar p[4] = {
    0,
    SkDoubleToScalar(y),
    0,
    1};
    transform.matrix().map(p);
    return SymmetricRound(p[1]);
}

double ReverseXDouble(const gfx::Transform& transform, int x) {
  gfx::Point3F point(x, 0, 0);
  bool ret = transform.TransformPointReverse(point);
  CHECK(ret) << "Invalid transform matrix";
  return point.x();
}


double ReverseYDouble(const gfx::Transform& transform, int y) {
  gfx::Point3F point(0, y, 0);
  bool ret = transform.TransformPointReverse(point);
  CHECK(ret) << "Invalid transform matrix";
  return point.y();
}

int VectorToX(const gfx::Transform& transform, int index) {
  gfx::Point point(index, 0);
  transform.TransformPoint(point);
  return point.x();
}

int XToVerctor(const gfx::Transform& transform, int x) {
  gfx::Point point(x, 0);
  bool ret = transform.TransformPointReverse(point);
  CHECK(ret) << "Invalid transform matrix";
  return point.x();
}
