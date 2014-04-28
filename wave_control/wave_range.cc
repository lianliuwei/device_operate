#include "wave_control/wave_range.h"

#include <xutility>

void WaveRange::MoveCenter(double offset) {
  double old_offset = (begin + end) / 2;
  begin = begin + (offset - old_offset);
  end = end + (offset - old_offset);
}

bool WaveRange::InRange(double offset) {
  double min_value = std::min(begin, end);
  double max_value = std::max(begin, end);
  return min_value <= offset && offset <= max_value;
}
