#pragma once

#include <vector>

#include "base/memory/ref_counted.h"

#include "depend_calc/calc_group.h"
#include "depend_calc/calc_executer.h"

#include "canscope/async_task.h"
#include "canscope/osc_chnl_calc/freq_time.h"
#include "canscope/osc_chnl_calc/chnl_calc_result.h"
#include "canscope/device/osc_device/osc_raw_data.h"

namespace canscope {

// no thread safe.
class ChnlCalcManager : public AsyncTaskObserver {
public:
  class Delegate {
  public:
    Delegate() {}
    virtual ~Delegate() {}

    virtual bool CalcForUI() = 0;
    virtual void MayNotifyUI(scoped_refptr<ChnlCalcResult> result) = 0;
    virtual void RecordResult(scoped_refptr<ChnlCalcResult> result) = 0;
  };

  ChnlCalcManager(Delegate* delegate);
  ~ChnlCalcManager();

  void RawDataCollected(OscRawDataHandle raw_data);
  
  void Cancel();

  // take ownership of group
  void SetChnlCalc(CalcGroup* group);
  void SetUICalc(CalcGroup* group);
  
  void ReProcessRawForUI();

private:
  void ProcessRaw();

  virtual void OnAsyncTaskFinish(AsyncTask* task, 
                                 AsyncTaskStatus status, 
                                 base::Value* param) OVERRIDE;

  void StartRun(bool force_ui);

 
  // ui calc less then 60Hz, more is waste CPU.
  scoped_refptr<CalcExecuter> chnl_calc_;
  scoped_refptr<CalcExecuter> ui_calc_;

  scoped_refptr<ChnlCalcResult> calc_result_;
  AsyncTaskHandle current_task_;
  bool is_ui_calc_;

  std::deque<OscRawDataHandle> raw_data_queue_;
  OscRawDataHandle current_raw_data_;

  Delegate* delegate_;
};

} // namespace canscope
