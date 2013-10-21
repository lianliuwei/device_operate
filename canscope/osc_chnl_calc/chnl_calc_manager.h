#pragma once

class ChnlCalcManager : public AsyncTaskObserver {
public:
  void RawDataCollected(scoped_refptr<OscRawData> raw_data) {
    raw_data_.push_back(raw_data);
    ProcessRaw();
  }

  void ProcessRaw() {
    // current only one task is running.
    if (current_task->IsRunning()) {
      return;
    }
    if (!raw_data_.size()) 
      return;
    current_task = new AsyncTask;
    CalcExecuter* calc_exec = ChoiceExecuter();
    calc_result = new ChnlCalcResult();
    calc_exec.set_delegate(calc_result.get());
    calc_exec.StartRun(&current_task);
  }

  virtual void OnAsyncTaskFinish(AsyncTaskHandle task, AsyncTaskStatus status, 
    base::Value* param) {
      DCHECK(task.get() == current_task.get());
      scoped_refptr<ChnlCalcResult> last_result = calc_result;
      // cache calc_result and ProcessRaw immediately, for better concurrent
      ProcessRaw();
      RecordResult(calc_result);
      MayNotifyUI(calc_result);
  }

  CalcExecuter* ChoiceExecuter() {

  }

  void MayNotifyUI(scoped_refptr<ChnlCalcResult> result) {

  }


  void RecordResult(scoped_refptr<ChnlCalcResult> result) {

  }


  void Cancel() {
    if (!current_task->IsRunning())
      return;
    current_task->Cancel();
    calc_result->Canceled();
  }

private:
  // ui calc less then 60Hz, more is waste CPU.
  CalcExecuter chnl_calc;
  CalcExecuter ui_calc;
  scoped_refptr<ChnlCalcResult> calc_result;

  typedef vector<scoped_refptr<OscRawData>> OscRawDataVector;
  OscRawDataVector raw_data_;
  AsyncTaskHandle current_task;
};
