#include "testing/gtest/include/gtest/gtest.h"
#include "testing/gmock/include/gmock/gmock.h"

#include "canscope/test/scoped_trace_event.h"
#include "canscope/device/config_util.h"
#include "canscope/test/test_process.h"
#include "canscope/osc_chnl_calc/chnl_calc_manager.h"
#include "canscope/osc_chnl_calc/chnl_calc_item.h"
#include "canscope/osc_chnl_calc/can_diff_calc_item.h"
#include "canscope/app/freq_time.h"

using namespace canscope;
using namespace base;
using namespace common;
using testing::Invoke;
using testing::AnyNumber;

namespace {

void ActionQuitUI() {
  CommonThread::UnsafeGetMessageLoopForThread(CommonThread::UI)->
      PostTask(FROM_HERE, MessageLoop::QuitClosure());
}

class ChnlCalcObserverMock {
public:
  ChnlCalcObserverMock()
      : count_(0) {}
  ~ChnlCalcObserverMock() {}

  MOCK_METHOD0(CalcFinish, void());
  MOCK_METHOD0(NotifyUI, void());

  void set_expect_count(int count) {
    expect_count_ = count;
  }

  int expect_count() const {
    return expect_count_;
  }

  void QuitUIWhen() {
    ++count_;
    if (count_ == expect_count_)
      ActionQuitUI();
  }

private:
  int count_;
  int expect_count_;

  DISALLOW_COPY_AND_ASSIGN(ChnlCalcObserverMock);
};

class ChnlCalcManagerDelegate : public ChnlCalcManager::Delegate {
public:
  ChnlCalcManagerDelegate() 
      : time_(60) {}
  virtual ~ChnlCalcManagerDelegate() {}

  // implment ChnlCalcManager::Observer
  virtual bool CalcForUI() { return time_.Hit(); }
  virtual void MayNotifyUI(scoped_refptr<ChnlCalcResult> result);
  virtual void RecordResult(scoped_refptr<ChnlCalcResult> result);
private:
  FreqTime time_;

  DISALLOW_COPY_AND_ASSIGN(ChnlCalcManagerDelegate);
};

class ChnlCalcManagerTest : public testing::Test {
public:
  ChnlCalcManagerTest() 
      : count_(0)
      , trace_event_("ChnlCalcManager", base::FilePath(L"trace_event.json"))
    {}
  virtual ~ChnlCalcManagerTest() {}

  static void SetUpTestCase() {
    new TestProcess();
    GetTestProcess()->Init();
  }
  static void TearDownTestCase() {
    DestroyTestProcess();
  }
  
  ChnlCalcObserverMock mock;

  void StartProduce();
  void ProduceOne();

protected:
  virtual void SetUp();

  virtual void TearDown();


  scoped_ptr<ChnlCalcManagerDelegate> delegate_;
  scoped_ptr<ChnlCalcManager> manager_;
  scoped_ptr<base::Value> default_config_;
  int count_;
  ScopedTraceEvent trace_event_;
};


static void* kCANHKey = (void*)(0x01);
static void* kCANLKey = (void*)(0x02);
static void* kCANDIFFKey = (void*)(0x03);

static const char kOscConfig [] =  {" \
{ \
  \"CANH.Range\" : 3, \
  \"CANH.Offset\" : 0.0, \
  \"CANH.Coupling\" : 1, \
  \"CANL.Range\" : 3, \
  \"CANL.Offset\" : 0.0, \
  \"CANL.Coupling\" : 1, \
  \"CANDIFF.Range\" : 3, \
  \"CANDIFF.Offset\" : 0.0, \
  \"DiffCtrl\" : 0, \
  \"Time.Base\" : 4, \
  \"Time.Offset\" : 0.0, \
  \"Trigger.AutoTime\" : 100.0, \
  \"Trigger.Source\" : 2, \
  \"Trigger.Type\" : 0, \
  \"Trigger.Mode\" : 0, \
  \"Trigger.Sens\" : 0, \
  \"Trigger.Compare\" : 0, \
  \"Trigger.Volt\" : 0.0, \
  \"TimeParam\" : 1.0 \
} \
"};


static ChnlCalcManagerTest* gCurrentTest = NULL;

void ChnlCalcManagerDelegate::MayNotifyUI(scoped_refptr<ChnlCalcResult> result) {
  gCurrentTest->mock.NotifyUI();
}

void ChnlCalcManagerDelegate::RecordResult(scoped_refptr<ChnlCalcResult> result) {
  gCurrentTest->mock.CalcFinish();
}

void ChnlCalcManagerTest::SetUp() {
  gCurrentTest = this;
}

void ChnlCalcManagerTest::TearDown() {
  DCHECK(gCurrentTest == this);
  gCurrentTest = NULL;
}

}


void SetUpManager(ChnlCalcManager* manager) {
  CalcGroup* chnl_group = new CalcGroup("chnl");
  ChnlCalcItem* chnl_calc = new ChnlCalcItem("ChnlCalc", kCANHKey, kCANLKey);
  ChnlDiffCalcItem* diff_calc = new ChnlDiffCalcItem("DiffCalc", 
    kCANHKey, kCANLKey, kCANDIFFKey);
  chnl_group->AddCalcItem(chnl_calc);
  chnl_group->AddCalcItem(diff_calc);
  chnl_group->SetDepend(kCANHKey, kCANDIFFKey);
  DCHECK(chnl_group->NoCycle());

  CalcGroup* ui_group = chnl_group->Clone("ui");

  manager->SetChnlCalc(chnl_group);
  manager->SetUICalc(ui_group);
}

TEST_F(ChnlCalcManagerTest, ProcessOne) {
  ChnlCalcManagerDelegate observer;
  ChnlCalcManager chnl_calc_manager(&observer);
  SetUpManager(&chnl_calc_manager);

  EXPECT_CALL(mock, CalcFinish()).Times(1).WillOnce(Invoke(&ActionQuitUI));
  EXPECT_CALL(mock, NotifyUI()).Times(1);
  scoped_ptr<base::Value> config_value(GetConfig(kOscConfig));
  ConfigManager::Config config = { 1,  config_value.get() };
  OscRawDataHandle osc_raw_data = new OscRawData(DT_CS1202, 
      new OscRawDataDeviceConfig(config, false));
  chnl_calc_manager.RawDataCollected(osc_raw_data);

  GetTestProcess()->MainMessageLoopRun();
}


void ChnlCalcManagerTest::StartProduce() {
  delegate_.reset(new ChnlCalcManagerDelegate);
  manager_.reset(new ChnlCalcManager(delegate_.get()));
  SetUpManager(manager_.get());
  default_config_.reset(GetConfig(kOscConfig));
  ProduceOne();
}

void ChnlCalcManagerTest::ProduceOne() {
  ConfigManager::Config config = { count_,  default_config_.get() };
  OscRawDataHandle osc_raw_data = new OscRawData(DT_CS1202, 
    new OscRawDataDeviceConfig(config, false));
  manager_->RawDataCollected(osc_raw_data);

  ++count_;
  if (count_ == mock.expect_count())
    return;
  CommonThread::PostTask(CommonThread::CALC, FROM_HERE, 
      Bind(&ChnlCalcManagerTest::ProduceOne, Unretained(this)));
}

void TraceUIEvent() {
  TRACE_EVENT_INSTANT0("ChnlCalcManager", "NotifyUI", TRACE_EVENT_SCOPE_THREAD);
}

TEST_F(ChnlCalcManagerTest, ProcessMany) {
  mock.set_expect_count(1000);
  EXPECT_CALL(mock, CalcFinish()).Times(1000)
      .WillRepeatedly(Invoke(&(this->mock), &(ChnlCalcObserverMock::QuitUIWhen)));
  EXPECT_CALL(mock, NotifyUI()).WillRepeatedly(Invoke(&TraceUIEvent));

  CommonThread::PostTask(CommonThread::CALC, FROM_HERE, 
      Bind(&ChnlCalcManagerTest::StartProduce, Unretained(this)));

  GetTestProcess()->MainMessageLoopRun();
}
