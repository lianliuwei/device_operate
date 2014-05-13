#pragma once

#include "device/sequenced_bulk_queue.h"
#include "canscope/osc_chnl_calc/chnl_calc_manager.h"
#include "canscope/device/osc_device/osc_data_collecter.h"
#include "canscope/app/freq_time.h"

namespace canscope {

typedef SequencedBulkQueue<scoped_refptr<ChnlCalcResult> > ChnlCalcResultQueue;

class CANScopeChnlCalc : public base::RefCountedThreadSafe<CANScopeChnlCalc>
                       , public ChnlCalcManager::Delegate {
public:
  CANScopeChnlCalc(scoped_refptr<base::SingleThreadTaskRunner> run_thread, 
                   scoped_refptr<OscRawDataQueue> raw_data_queue);

  // only ui call these
  void Start();
  bool IsRunning();
  void Stop();

  scoped_refptr<ChnlCalcResultQueue> ChnlCalcQueue() { return chnl_queue_; }
  scoped_refptr<ChnlCalcResultQueue> UICalcQueue() { return ui_queue_; }

  scoped_refptr<base::SingleThreadTaskRunner> run_thread() const {
    return run_thread_;
  }

  // TODO add FFT calc to depend Calc
private:
  void StartImpl();
  void StopImpl();

  // implement ChnlCalcManager::Delegate
  virtual bool CalcForUI();
  virtual void MayNotifyUI(scoped_refptr<ChnlCalcResult> result);
  virtual void RecordResult(scoped_refptr<ChnlCalcResult> result);

  base::Lock lock_;
  bool running_;

  void OnRawDataArrive();

  scoped_ptr<ChnlCalcManager> chnl_calc_;

  scoped_refptr<ChnlCalcResultQueue> chnl_queue_;
  scoped_refptr<ChnlCalcResultQueue> ui_queue_;

  scoped_refptr<base::SingleThreadTaskRunner> run_thread_;
  scoped_refptr<OscRawDataQueue> raw_data_queue_;
  scoped_ptr<OscRawDataQueue::Reader> raw_data_reader_;

  FreqTime time_;

  DISALLOW_COPY_AND_ASSIGN(CANScopeChnlCalc);
};

} // namespace canscope