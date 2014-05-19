#pragma once

#include "ui/gfx/transform.h"

// two type of coordinate, 
// [double, double] -> [int, int]  data to view
// [int, ] -> [int, ] wave data index to view x pos

int XInt(const gfx::Transform& transform, double x);
int YInt(const gfx::Transform& transform, double y);

double ReverseXDouble(const gfx::Transform& transform, int x);
double ReverseYDouble(const gfx::Transform& transform, int y);


int VectorToX(const gfx::Transform& transform, int index);
int XToVector(const gfx::Transform& transform, int x);


