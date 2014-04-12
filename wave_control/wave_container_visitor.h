#pragma once

class WaveContainer;
class YTWaveContainer;
class YLWaveContainer;
class XYWaveContainer;

class WaveContainerVisitor {
public:
  WaveContainerVisitor() {}

  virtual void VisitYTWaveContainer(YTWaveContainer* wave_container) = 0;
  virtual void VisitYLWaveContainer(YLWaveContainer* wave_container) = 0;
  virtual void VisitXYWaveContainer(XYWaveContainer* wave_container) = 0;

protected:
  virtual ~WaveContainerVisitor() {}
};