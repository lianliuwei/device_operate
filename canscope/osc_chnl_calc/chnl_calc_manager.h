#pragma once

#include <vector>

#include "base/memory/ref_counted.h"

#include "depend_calc/calc_group.h"
#include "depend_calc/calc_executer.h"

#include "canscope/async_task.h"
#include "canscope/osc_chnl_calc/freq_time.h"
#include "canscope/osc_chnl_calc/chnl_calc_result.h"
#include "canscope/device/osc_raw_data.h"

namespace canscope {

class ChnlCalcManager : public AsyncTaskObserver {
public:
  ChnlCalcManager();
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

  void MayNotifyUI(scoped_refptr<ChnlCalcResult> result);

  void RecordResult(scoped_refptr<ChnlCalcResult> result);

  // ui calc less then 60Hz, more is waste CPU.
  scoped_refptr<CalcExecuter> chnl_calc_;
  scoped_refptr<CalcExecuter> ui_calc_;

  scoped_refptr<ChnlCalcResult> calc_result_;
  AsyncTaskHandle current_task_;
  bool is_ui_calc_;

  typedef std::deque<OscRawDataHandle> OscRawDataQueue;
  OscRawDataQueue raw_data_queue_;
  OscRawDataHandle current_raw_data_;

  FreqTime ui_freq_;
};

} // namespace canscope
