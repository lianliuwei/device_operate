#include "canscope/osc_chnl_calc/chnl_calc_manager.h"


namespace canscope {

ChnlCalcManager::ChnlCalcManager(Delegate* delegate)
    : delegate_(delegate) {
  DCHECK(delegate);
}

ChnlCalcManager::~ChnlCalcManager() {
  if (!current_task_.get())
    return;
  current_task_->RemoveObserver(this);
}

void ChnlCalcManager::RawDataCollected(OscRawDataHandle raw_data) {
  raw_data_queue_.push_back(raw_data);
  ProcessRaw();
}

void ChnlCalcManager::Cancel() {
  if (!current_task_.get())
    return;
  current_task_->RemoveObserver(this);
  current_task_->Cancel();
}

void ChnlCalcManager::SetChnlCalc(CalcGroup* group) {
  chnl_calc_ = new CalcExecuter(group);
}

void ChnlCalcManager::SetUICalc(CalcGroup* group) {
  ui_calc_ = new CalcExecuter(group); 
}

void ChnlCalcManager::ReProcessRawForUI() {
  if (!current_raw_data_.get())
    return;
  // cancel last one
  Cancel();
  StartRun(true);
}

void ChnlCalcManager::ProcessRaw() {
  // current only one task is running.
  if (current_task_.get() 
      && (current_task_->IsReady() || current_task_->IsRunning())) {
    return;
  }
  if (!raw_data_queue_.size()) 
    return;
  current_raw_data_ = raw_data_queue_.front();
  raw_data_queue_.pop_front();

  StartRun(false);
}

void ChnlCalcManager::StartRun(bool force_ui) {
  AsyncTaskHandle old_handle;
  if (current_task_.get()) {
    current_task_->RemoveObserver(this);
    DCHECK(!current_task_->IsRunning());
  }
  old_handle = current_task_;
  current_task_ = new AsyncTask;
  current_task_->AddObserver(this);
  CalcExecuter* calc_exec = NULL;
  if (!force_ui) {
    is_ui_calc_ = delegate_->CalcForUI();
    calc_exec = is_ui_calc_ ? ui_calc_.get() : chnl_calc_.get();
  } else {
    calc_exec = ui_calc_.get();
    is_ui_calc_ = true;
  }
  scoped_refptr<ChnlCalcResult> old_result = calc_result_;
  calc_result_ = new ChnlCalcResult(current_raw_data_);
  calc_exec->set_delegate(calc_result_.get());
  calc_exec->StartRun(current_task_);
}

void ChnlCalcManager::OnAsyncTaskFinish(AsyncTask* task, 
                                        AsyncTaskStatus status, 
                                        base::Value* param) {
  DCHECK (task == current_task_.get());
  scoped_refptr<ChnlCalcResult> last_result = calc_result_;
  bool is_ui_calc = is_ui_calc_;
  // cache calc_result and ProcessRaw immediately, for better concurrent
  ProcessRaw();
  delegate_->RecordResult(calc_result_);
  if (is_ui_calc)
    delegate_->MayNotifyUI(calc_result_);
}

} // namespace canscope
