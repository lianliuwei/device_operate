#include "canscope/app/canscope_chnl_calc.h"

#include "canscope/chnl/canscope_chnl_constants.h"
#include "canscope/osc_chnl_calc/can_diff_calc_item.h"
#include "canscope/osc_chnl_calc/chnl_calc_item.h"

using namespace base;
using namespace canscope;

namespace {
static const int kDefaultFreq = 60;
static const int kMinFreq = 10;
static const int kFreqStep = 5;
static const int kUIQueueMaxSize = 5;

// pass ownership
CalcGroup* BuildChnlCalcGroup() {
  CalcGroup* chnl_group = new CalcGroup("chnl");
  ChnlCalcItem* chnl_calc = new ChnlCalcItem("ChnlCalc", kCANHKey, kCANLKey);
  ChnlDiffCalcItem* diff_calc = new ChnlDiffCalcItem("DiffCalc", 
      kCANHKey, kCANLKey, kCANDIFFKey);
  chnl_group->AddCalcItem(chnl_calc);
  chnl_group->AddCalcItem(diff_calc);
  chnl_group->SetDepend(kCANHKey, kCANDIFFKey);

  return chnl_group;
}

}

namespace canscope {

CANScopeChnlCalc::CANScopeChnlCalc(scoped_refptr<base::SingleThreadTaskRunner> run_thread, 
                                   scoped_refptr<OscRawDataQueue> raw_data_queue)
    : run_thread_(run_thread)
    , raw_data_queue_(raw_data_queue)
    , running_(false)
    , time_(kDefaultFreq) {
  chnl_queue_ = new ChnlCalcResultQueue(false, false);
  ui_queue_ = new ChnlCalcResultQueue(true, false);
}

CANScopeChnlCalc::~CANScopeChnlCalc() {

}

void CANScopeChnlCalc::Start() {
  run_thread_->PostTask(FROM_HERE, Bind(&CANScopeChnlCalc::StartImpl, this));
}

bool CANScopeChnlCalc::IsRunning() {
  AutoLock lock(lock_);
  return running_;
}

void CANScopeChnlCalc::Stop() {
  run_thread_->PostTask(FROM_HERE, Bind(&CANScopeChnlCalc::StopImpl, this));
}

void CANScopeChnlCalc::StartImpl() {
  DCHECK(!running_);
  running_ = true;

  raw_data_reader_.reset(new OscRawDataQueue::Reader(raw_data_queue_.get()));
  raw_data_reader_->set_have_data_callback(Bind(&CANScopeChnlCalc::OnRawDataArrive, this));
  raw_data_reader_->CallbackOnReady();
  
  chnl_calc_.reset(new ChnlCalcManager(this));
  CalcGroup* chnl_calc = BuildChnlCalcGroup();
  chnl_calc_->SetChnlCalc(chnl_calc);
  // no just use same
  chnl_calc_->SetUICalc(chnl_calc->Clone("ui"));
}

void CANScopeChnlCalc::StopImpl() {
  DCHECK(running_);
  running_ = false;
  raw_data_reader_.reset(NULL);
  chnl_calc_.reset(NULL);
}

void CANScopeChnlCalc::OnRawDataArrive() {
  raw_data_reader_->CallbackOnReady();
  OscRawDataHandle raw_data;
  int64 seq;
  bool ret = raw_data_reader_->GetResult(&raw_data, &seq);
  DCHECK(ret);
  chnl_calc_->RawDataCollected(raw_data);
}

bool CANScopeChnlCalc::CalcForUI() {
  return time_.Hit();
}

void CANScopeChnlCalc::MayNotifyUI(scoped_refptr<ChnlCalcResult> result) {
  // ui speed control
  if (ui_queue_->bulk_num() > kUIQueueMaxSize) {
    ui_queue_->RecycleOne();
    double freq = time_.freq();
    freq -= kFreqStep;
    if (freq < kMinFreq) {
      freq = kMinFreq;
    }
    time_.set_freq(freq);
  } else if (ui_queue_->bulk_num() == 0) {
    double freq = time_.freq();
    freq += kFreqStep;
    time_.set_freq(freq);
  }

  ui_queue_->PushBulk(result);
  chnl_queue_->PushBulk(result);
}

void CANScopeChnlCalc::RecordResult(scoped_refptr<ChnlCalcResult> result) {
  chnl_queue_->PushBulk(result);
}

} // namespace canscope