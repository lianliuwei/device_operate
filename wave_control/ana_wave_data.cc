#include "wave_control/ana_wave_data.h"

#include "base/logging.h"

#include "ui/gfx/transform.h"
#include "wave_control/views/transform_util.h"

double AnaWaveData::GetValue(double offset) {
  double* data_ptr = data();
  DCHECK(data_ptr);
  gfx::Transform transform;
  WaveRange range = data_range();
  if (size() == 1) {
    return data_ptr[0];
  }
  double range_size =  range.end - range.begin;
  transform.Translate(range.begin, 0);
  transform.Scale(range_size / (size() - 1), 1);
  int index = TransformReverseX(transform, offset);
  CHECK(index >= 0 && index <= size() - 1);
  return data_ptr[index];
}

PeakValue AnaWaveData::GetRangePeak(int start, int range_size) {
  double* data_ptr = data();
  int data_size = size();
  DCHECK(data_ptr);
  DCHECK(start >=0 && start < data_size - 1);
  DCHECK(range_size > 0 &&  start + range_size - 1 < data_size - 1);
  double max = data_ptr[start]; 
  double min = data_ptr[start];
  for (int i = start; i < start + range_size; ++i) {
    if (max < data_ptr[i]) {
      max = data_ptr[i];
    }
    if (min > data_ptr[i]) {
      min = data_ptr[i];
    }
  }
  PeakValue peak = { data_ptr[start], data_ptr[start + range_size - 1], max, min };
  return peak;
}

double AnaWaveData::MaxY() {
  double* data_ptr = data();
  int data_size = size();
  DCHECK(data_ptr);
  double max = data_ptr[0]; 
  for (int i = 0; i < data_size; ++i) {
    if (data_ptr[i] > max) {
      max = data_ptr[i];
    }
  }
  return max;
}

double AnaWaveData::MinY() {
  double* data_ptr = data();
  int data_size = size();
  DCHECK(data_ptr);
  double min = data_ptr[0]; 
  for (int i = 0; i < data_size; ++i) {
    if (data_ptr[i] < min) {
      min = data_ptr[i];
    }
  }
  return min;
}
