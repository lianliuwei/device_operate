#pragma once

#include "ui/gfx/transform.h"

int TransformX(const gfx::Transform transform, int x);
int TransformX(const gfx::Transform transform, double x);

int TransformReverseX(const gfx::Transform transform, int x);

int TransformY(const gfx::Transform transform, double y);

int TransformReverseY(const gfx::Transform transform, int y);