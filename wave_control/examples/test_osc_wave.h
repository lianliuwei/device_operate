#pragma once

#include "base/memory/scoped_ptr.h"

#include "wave_control/osc_wave.h"

class TestAnaData : public AnaWaveData {
public:
  TestAnaData() {}
  virtual ~TestAnaData() {}

  virtual WaveRange data_range()  {
    return data_range_;
  }
  virtual double* data()  {
    return &(data_[0]);
  }

  virtual int size()  {
    return data_.size();
  }
  WaveRange data_range_;
  std::vector<double> data_;
};

class ChangedNotify {
public:
  ChangedNotify() {}
  virtual void OnNotifyChanged(int changed_part) = 0;

protected:
  virtual ~ChangedNotify() {};
};

struct Horiz : public base::RefCounted<Horiz> {
  WaveRange range;
  double offset;
  int div;
  int windows_size;
  SkColor color;
  
  void AddObserver(ChangedNotify* observer) { observer_list_.AddObserver(observer); }
  void RemoveObserver(ChangedNotify* observer) { observer_list_.RemoveObserver(observer); }
  void NotifyChanged(int changed_part) { 
    FOR_EACH_OBSERVER(ChangedNotify, observer_list_, OnNotifyChanged(changed_part)); 
  }

private:
  ObserverList<ChangedNotify> observer_list_;
};

struct Vertical : public base::RefCounted<Vertical> {
  WaveRange range;
  double offset;
  int div;
  int windows_size;
  SkColor color;
};

struct Trigger : public base::RefCounted<Trigger> {
  WaveRange offset_range;
  int offset;
  OscWave* trigger_wave;

  void AddObserver(ChangedNotify* observer) { observer_list_.AddObserver(observer); }
  void RemoveObserver(ChangedNotify* observer) { observer_list_.RemoveObserver(observer); }
  void NotifyChanged(int changed_part) { 
    FOR_EACH_OBSERVER(ChangedNotify, observer_list_, OnNotifyChanged(changed_part)); 
  }
private:
  ObserverList<ChangedNotify> observer_list_;
};

class TestOscWave : public OscWave
                  , public ChangedNotify {
public:
  TestOscWave() {}
  virtual ~TestOscWave();
  // implement wave
  virtual string16 name();
  virtual SkColor color();
  virtual const gfx::Image& icon();

  void AddHoriz(Horiz* horiz);
  void AddTrigger(Trigger* trig);
  void AddVertical(Vertical* vertical);

  // implement OscWave
  virtual AnaWaveData& Data();
  virtual void MoveToX(double pos);
  virtual void MoveToY(double pos);
  virtual void MoveTrigger(double pos);
  virtual void ZoomInX();
  virtual void ZoomOutX();
  virtual void ZoomInY();
  virtual void ZoomOutY();

  virtual bool IsSameTrigger(OscWave* osc_wave);
  virtual WaveRange trigger_offset_range();
  virtual double trigger_offset();
  virtual bool trigger_show();
  virtual bool trigger_is_relate();
  virtual OscWave* trigger_wave();
  virtual SkColor trigger_color();
  virtual bool IsSameVertical(OscWave* osc_wave);
  virtual int vertical_div();
  virtual int vertical_window_size();
  virtual WaveRange vertical_range();
  virtual WaveRange vertical_offset_range();
  virtual double vertical_offset();
  virtual SkColor vertical_color();
  virtual bool vertical_show();
  virtual bool IsSameHorizontal(OscWave* osc_wave);
  virtual int horizontal_div();
  virtual int horizontal_window_size();
  virtual WaveRange horizontal_range();
  virtual WaveRange horizontal_offset_range();
  virtual double horizontal_offset();
  virtual SkColor horizontal_color();
  virtual bool horizontal_show();

  virtual void DoCommand(int command_id);
  virtual void DoRangeCommand(int command_id, WaveRange range);

  virtual void OnNotifyChanged(int changed_part);

public:
  string16 name_;
  SkColor color_;
  gfx::Image icon_;
  scoped_ptr<TestAnaData> wave_data_;

private:
  scoped_refptr<Horiz> horiz_;
  scoped_refptr<Vertical> vertical_;
  scoped_refptr<Trigger> trigger_;
};

std::vector<TestOscWave*> CreateWaves();
