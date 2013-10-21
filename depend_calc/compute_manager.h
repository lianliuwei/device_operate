
class RawData;

class ComputeManager : public AsyncTaskObserver {
public:
  void RawDataCollected(vector<char>* raw_data) {
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
    calc_exec.Reset();
    calc_result.Reset();
    calc_exec.set_delegate(&calc_result);
    calc_exec.StartRun(&current_task);

  }

  virtual void OnAsyncTaskFinish(AsyncTask* task, AsyncTaskStatus status, 
    base::Value* param) {
      ProcessRaw();
      RecordResult(&calc_result);
      MayNotifyUI();
  }

  CalcExecuter* ChoiceExecuter() {

  }

  void MayNotifyUI() {

  }

  void Cancel() {
    if (!current_task->IsRunning())
      return;
    current_task->Cancel();
    calc_result.Canceled();
  }

  void RecordResult(ChnlCalc* calc_result) {

  }

public:
  // ui calc less then 60Hz, more is waste CPU.
  CalcExecuter chnl_calc;
  CalcExecuter ui_calc;
  ChnlCalc calc_result;

  vector<RawData> raw_data_;
  AsyncTaskHandle current_task;
};
