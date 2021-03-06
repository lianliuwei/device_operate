#include "testing/gtest/include/gtest/gtest.h"
#include "testing/gmock/include/gmock/gmock.h"

#include "base/bind.h"
#include "base/json/json_string_value_serializer.h"
#include "base/json/json_reader.h"
#include "base/threading/thread.h"
#include "base/lazy_instance.h"
#include "base/run_loop.h"

#include "device/property/store_member.h"
#include "device/property/value_map_device_property_store.h"

#include "canscope/test/scoped_trace_event.h"
#include "canscope/device/property/property.h"
#include "canscope/device/property/property_delegate.h"
#include "canscope/async_task.h"
#include "canscope/async_task_observer_mock.h"

using namespace base;
using testing::_;
using testing::Return;
using testing::DoAll;
using namespace canscope::device;
using namespace device;

namespace {
static const char* kBoolMember = "test.bool_member";
static const char* kIntMember = "test.int_member";
}

class Device {
public:
  Device() {}
  ~Device() {}

  void Init(base::DictionaryValue* dict) {
    prefs_.Reset(dict);

    bool_member.Init(kBoolMember, &prefs_);
    int_member.Init(kIntMember, &prefs_);
  }

  MOCK_METHOD0(SetBoolMember, void());
  MOCK_METHOD0(SetIntMember, void());

  ValueMapDevicePropertyStore prefs_;

  BooleanStoreMember bool_member;
  IntegerStoreMember int_member;

};

class DeviceHandle : public Device
                   , public canscope::PropertyDelegate {
public:
  DeviceHandle(Device& device)
      : bool_property(this, bool_member, device.bool_member,
          Bind(&Device::SetBoolMember, Unretained(&device)),
          Bind(&DeviceHandle::bool_property_check, Unretained(this)))
      , int_property(this, int_member, device.int_member,
          Bind(&Device::SetIntMember, Unretained(&device)),
          Bind(&DeviceHandle::int_property_check, Unretained(this)))
      , device_(device) {
  }
  
  void InitFromDevice() {
    Init(device_.prefs_.Serialize());
  }

  virtual ~DeviceHandle() {

  }

  MOCK_METHOD2(bool_property_check, bool(bool, const std::string&));
  MOCK_METHOD2(int_property_check, bool(int, const std::string&));

  // canscope::PropertyDelegate
  virtual DevicePropertyStore* GetDevicePropertyStore() {
    return &(prefs_);
  }

  MOCK_METHOD0(IsBatchMode, bool());


  MOCK_METHOD1(PostDeviceTask, void(const Closure&));
  MOCK_METHOD0(IsDeviceThread, bool());

  virtual void FetchNewPref() {
    prefs_.ChangeContent(device_.prefs_.Serialize());
  }
  Device* device() {
    return &device_;
  }

  virtual void SetPropertyFinish(const std::string& reason) {}


  canscope::BooleanProperty bool_property;
  canscope::IntegerProperty int_property;

private:
  Device& device_;
};

// use Batch mode cache property, after out range, set to device and fetch new
// pref
class ScopedDeviceOperate {
public:  
  ScopedDeviceOperate(DeviceHandle& handle)
      : handle_(handle) {
    EXPECT_CALL(handle_, IsBatchMode()).WillRepeatedly(Return(true));

  }
  // only support one thread, no same thread need post task and WaitEvent.
  ~ScopedDeviceOperate() {
    EXPECT_CALL(handle_, IsBatchMode()).Times(0);
    handle_.device()->prefs_.ChangeContent(handle_.prefs_.Serialize());
  }
private:
  DeviceHandle& handle_;
  DISALLOW_COPY_AND_ASSIGN(ScopedDeviceOperate);
};
 
class DevicePropertyObserverMock
    : public DevicePropertyStore::Observer {
public:
  DevicePropertyObserverMock() {}
  virtual ~DevicePropertyObserverMock() {}

  MOCK_METHOD1(OnPreferenceChanged, void(const std::string&));
};

namespace {
static const char kTestConfig[] =  {" \
{ \
  \"test.bool_member\" : false, \
  \"test.int_member\" : 3 \
} \
"};

DictionaryValue* GetDefaultConfig() {
  std::string content(kTestConfig);
  JSONStringValueSerializer serializer(content);
  Value* value = serializer.Deserialize(NULL, NULL);
  EXPECT_TRUE(NULL != value);
  EXPECT_TRUE(value->IsType(Value::TYPE_DICTIONARY));
  DictionaryValue* dict_value;
  value->GetAsDictionary(&dict_value);
  return dict_value;
}

}

ACTION_P(CallFuncError, device_error) {
  // clean last error first
  CleanError();
  SetDeviceError(device_error);
}

ACTION(NotReached) {
  NOTREACHED();
}

class DeviceThread : public base::Thread {
public:
  DeviceThread() 
      : Thread("device") {}
  virtual ~DeviceThread() {}

protected:
  virtual void Init() {
    device_error_.reset(new ScopedDeviceError());
  }
  virtual void CleanUp() {
    device_error_.reset(NULL);
  }

private:
  scoped_ptr<ScopedDeviceError> device_error_;
};

ACTION_P2(CheckDeviceThread, device_thread, no_revert) {
  bool ret = MessageLoop::current() == device_thread->message_loop();
  EXPECT_EQ(true, no_revert ? ret : !ret);
}

ACTION_P(ReturnIsDeviceThread, device_thread) {
  bool ret = MessageLoop::current() == device_thread->message_loop();
  return ret;
}

ACTION_P(PostDeviceTaskAction, device_thread) {
  device_thread->message_loop()->PostTask(FROM_HERE, arg0);
}

class PropertyTest : public testing::Test {
public:
  PropertyTest() 
    : handle(device)
    , trace_event_("Property,PropertyStore", base::FilePath(L"trace_event.json"))
    , created_thread_(false){}
 
  void Verify() {
    testing::Mock::VerifyAndClearExpectations(&handle);
    testing::Mock::VerifyAndClearExpectations(&device);
  }

  void VerifyObserver() {
    testing::Mock::VerifyAndClearExpectations(&async_observer);
  }

  void InitOneThread() {
    ON_CALL(handle, IsDeviceThread()).WillByDefault(Return(true));
    ON_CALL(handle, PostDeviceTask(_)).WillByDefault(NotReached());
    ON_CALL(handle, IsBatchMode()).WillByDefault(Return(false));
  }

  void InitCrossThread() {
    created_thread_ = true;
    device_thread.Start();
    // HACK delete device from this thread
    device.prefs_.DetachFromThread();

    // mock Post and check device thread
    ON_CALL(handle, IsDeviceThread())
        .WillByDefault(ReturnIsDeviceThread(&device_thread));
    ON_CALL(handle, PostDeviceTask(_))
        .WillByDefault(PostDeviceTaskAction(&device_thread));
    ON_CALL(handle, IsBatchMode()).WillByDefault(Return(false));
  }

  void CheckAsyncStart();
  void CheckAsyncFinish();

protected:
  virtual void SetUp() {
    device.Init(GetDefaultConfig());
    handle.InitFromDevice();
  }

  virtual void TearDown() {
    if (created_thread_) {
      device_thread.Stop();
    }
  }

  Device device;
  DeviceHandle handle;
  DeviceThread device_thread;
  AsyncTaskObserverMock async_observer;
  AsyncTaskHandle async_task;
  base::Closure quit;
private:
  ScopedDeviceError device_error_;
  ScopedTraceEvent trace_event_;
  bool created_thread_;
  
};

TEST_F(PropertyTest, GetValueOneThread) {
  InitOneThread();

  // get value
  EXPECT_CALL(device, SetBoolMember()).Times(0);
  EXPECT_CALL(device, SetIntMember()).Times(0);
  EXPECT_CALL(handle, bool_property_check(_, _)).Times(0);
  EXPECT_CALL(handle, int_property_check(_, _)).Times(0);

  EXPECT_EQ(false, handle.bool_property.value());
  EXPECT_EQ(3, handle.int_property.value());

  Verify();
}

TEST_F(PropertyTest, SetHandleOneThread) {
  InitOneThread();

  // set from handle
  EXPECT_CALL(device, SetBoolMember()).Times(1);
  EXPECT_CALL(device, SetIntMember()).Times(1);
  EXPECT_CALL(handle, bool_property_check(true, kBoolMember))
      .Times(1).WillOnce(Return(true));
  EXPECT_CALL(handle, int_property_check(44, kIntMember))
      .Times(1).WillOnce(Return(true));

  handle.bool_property.set_value(true);
  EXPECT_EQ(true, handle.bool_property.value());
  EXPECT_EQ(true, device.bool_member.value());
  handle.int_property.set_value(44);
  EXPECT_EQ(OK, LastDeviceError());
  EXPECT_EQ(44, handle.int_property.value());
  EXPECT_EQ(44, device.int_member.value());

  Verify();
}

TEST_F(PropertyTest, CheckFalseOneThread) {
  InitOneThread();

  // check false
  EXPECT_CALL(device, SetBoolMember()).Times(0);
  EXPECT_CALL(device, SetIntMember()).Times(0);
  EXPECT_CALL(handle, bool_property_check(true, kBoolMember))
      .Times(1).WillOnce(Return(false));
  EXPECT_CALL(handle, int_property_check(10, kIntMember))
      .Times(1).WillOnce(Return(false));

  handle.bool_property.set_value(true);
  EXPECT_EQ(ERR_INVAILD_VALUE, LastDeviceError());
  EXPECT_EQ(false, handle.bool_property.value());
  EXPECT_EQ(false, device.bool_member.value());
  handle.int_property.set_value(10);
  EXPECT_EQ(ERR_INVAILD_VALUE, LastDeviceError());
  EXPECT_EQ(3, handle.int_property.value());
  EXPECT_EQ(3, device.int_member.value());

  Verify();
}

TEST_F(PropertyTest, ObserverOneThread) {
  InitOneThread();

  // observer
  DevicePropertyObserverMock handle_observer;

  handle.bool_property.AddPrefObserver(&handle_observer);

  EXPECT_CALL(device, SetBoolMember()).Times(1);
  EXPECT_CALL(handle, bool_property_check(true, kBoolMember))
      .Times(1).WillOnce(Return(true));
  EXPECT_CALL(handle_observer, OnPreferenceChanged(kBoolMember)).Times(1);

  handle.bool_property.set_value(true);

  EXPECT_CALL(device, SetBoolMember()).Times(1);
  EXPECT_CALL(handle, bool_property_check(true, kBoolMember))
      .Times(1).WillOnce(Return(true));
  EXPECT_CALL(handle_observer, OnPreferenceChanged(_)).Times(0);

  handle.bool_property.set_value(true);

  handle.bool_property.RemovePrefObserver(&handle_observer);

  Verify();
}

TEST_F(PropertyTest, CallFuncErrorOneThread) {
  InitOneThread();

  // SetBoolMember return error
  EXPECT_CALL(device, SetBoolMember()).Times(1).WillOnce(
      CallFuncError(ERR_READ_DEVICE));
  EXPECT_CALL(handle, bool_property_check(true, kBoolMember))
      .Times(1).WillOnce(Return(true));
  handle.bool_property.set_value(true);
  EXPECT_EQ(ERR_READ_DEVICE, LastDeviceError());
  EXPECT_EQ(true, handle.bool_property.value());
  EXPECT_EQ(true, device.bool_member.value());

  Verify();
  
  EXPECT_CALL(device, SetBoolMember()).Times(1).WillOnce(
      CallFuncError(ERR_WRITE_DEVICE));
  EXPECT_CALL(handle, bool_property_check(false, kBoolMember))
      .Times(1).WillOnce(Return(true));

  handle.bool_property.set_value(false);
  EXPECT_EQ(ERR_WRITE_DEVICE, LastDeviceError());
  EXPECT_EQ(false, handle.bool_property.value());
  EXPECT_EQ(false, device.bool_member.value());

  Verify();
}

TEST_F(PropertyTest, BatchModeOneThread) {
  InitOneThread();

  EXPECT_CALL(device, SetBoolMember()).Times(0);
  EXPECT_CALL(device, SetIntMember()).Times(0);
  EXPECT_CALL(handle, bool_property_check(true, kBoolMember))
      .Times(1).WillOnce(Return(true));
  EXPECT_CALL(handle, int_property_check(44, kIntMember))
      .Times(1).WillOnce(Return(true));
  {
  ScopedDeviceOperate device_operate(handle);
 
  handle.bool_property.set_value(true);
  EXPECT_EQ(true, handle.bool_property.value());
  EXPECT_EQ(false, device.bool_member.value());
  handle.int_property.set_value(44);
  EXPECT_EQ(44, handle.int_property.value());
  EXPECT_EQ(3, device.int_member.value());
  }
  EXPECT_EQ(true, handle.bool_property.value());
  EXPECT_EQ(true, device.bool_member.value());
  EXPECT_EQ(44, handle.int_property.value());
  EXPECT_EQ(44, device.int_member.value());

  Verify();
}

TEST_F(PropertyTest, SetHandleCrossThread) {
  InitCrossThread();

  // set from handle
  EXPECT_CALL(device, SetBoolMember())
      .Times(1).WillOnce(CheckDeviceThread(&device_thread, true));
  EXPECT_CALL(device, SetIntMember())
      .Times(1).WillOnce(CheckDeviceThread(&device_thread, true));
  EXPECT_CALL(handle, bool_property_check(true, kBoolMember))
      .Times(1).WillOnce(DoAll(CheckDeviceThread(&device_thread, false),
                               Return(true)));
  EXPECT_CALL(handle, int_property_check(44, kIntMember))
      .Times(1).WillOnce(DoAll(CheckDeviceThread(&device_thread, false),
                               Return(true)));

  handle.bool_property.set_value(true);
  EXPECT_EQ(OK, LastDeviceError());
  EXPECT_EQ(true, handle.bool_property.value());
  EXPECT_EQ(true, device.bool_member.value());
  handle.int_property.set_value(44);
  EXPECT_EQ(OK, LastDeviceError());
  EXPECT_EQ(44, handle.int_property.value());
  EXPECT_EQ(44, device.int_member.value());

  Verify();
}

TEST_F(PropertyTest, ObserverCrossThread) {
  InitCrossThread();

  // observer
  DevicePropertyObserverMock handle_observer;

  handle.bool_property.AddPrefObserver(&handle_observer);

  EXPECT_CALL(device, SetBoolMember()).Times(1);
  EXPECT_CALL(handle, bool_property_check(false, kBoolMember))
    .Times(1).WillOnce(Return(true));
  EXPECT_CALL(handle_observer, OnPreferenceChanged(_)).Times(0);

  handle.bool_property.set_value(false);

  EXPECT_CALL(device, SetBoolMember()).Times(1);
  EXPECT_CALL(handle, bool_property_check(true, kBoolMember))
      .Times(1).WillOnce(Return(true));
  EXPECT_CALL(handle_observer, OnPreferenceChanged(kBoolMember))
      .Times(1).WillOnce(CheckDeviceThread(&device_thread, false));

  handle.bool_property.set_value(true);

  handle.bool_property.RemovePrefObserver(&handle_observer);

  Verify();
}

TEST_F(PropertyTest, CheckFalseCrossThread) {
  InitCrossThread();

  // check false
  EXPECT_CALL(device, SetBoolMember()).Times(0);
  EXPECT_CALL(device, SetIntMember()).Times(0);
  EXPECT_CALL(handle, bool_property_check(true, kBoolMember))
    .Times(1).WillOnce(Return(false));
  EXPECT_CALL(handle, int_property_check(10, kIntMember))
    .Times(1).WillOnce(Return(false));

  handle.bool_property.set_value(true);
  EXPECT_EQ(ERR_INVAILD_VALUE, LastDeviceError());
  EXPECT_EQ(false, handle.bool_property.value());
  EXPECT_EQ(false, device.bool_member.value());
  handle.int_property.set_value(10);
  EXPECT_EQ(ERR_INVAILD_VALUE, LastDeviceError());
  EXPECT_EQ(3, handle.int_property.value());
  EXPECT_EQ(3, device.int_member.value());

  Verify();
}

TEST_F(PropertyTest, CallFuncErrorCrossThread) {
  InitCrossThread();

  // SetBoolMember return error
  EXPECT_CALL(device, SetBoolMember()).Times(1).WillOnce(
      DoAll(CheckDeviceThread(&device_thread, true), 
          CallFuncError(ERR_READ_DEVICE)));
  EXPECT_CALL(handle, bool_property_check(true, kBoolMember))
      .Times(1).WillOnce(
          DoAll(CheckDeviceThread(&device_thread, false), Return(true)));

  handle.bool_property.set_value(true);
  EXPECT_EQ(ERR_READ_DEVICE, LastDeviceError());
  EXPECT_EQ(true, handle.bool_property.value());
  EXPECT_EQ(true, device.bool_member.value());

  Verify();

  EXPECT_CALL(device, SetBoolMember()).Times(1).WillOnce(
      DoAll(CheckDeviceThread(&device_thread, true), 
        CallFuncError(ERR_WRITE_DEVICE)));
  EXPECT_CALL(handle, bool_property_check(false, kBoolMember))
      .Times(1).WillOnce(
          DoAll(CheckDeviceThread(&device_thread, false), Return(true)));

  handle.bool_property.set_value(false);
  EXPECT_EQ(ERR_WRITE_DEVICE, LastDeviceError());
  EXPECT_EQ(false, handle.bool_property.value());
  EXPECT_EQ(false, device.bool_member.value());

  Verify();
}

TEST_F(PropertyTest, AsyncSetValue) {
  InitCrossThread();
  MessageLoop message_loop;

  // start is notify before 
  EXPECT_CALL(handle, bool_property_check(true, kBoolMember))
      .Times(1).WillOnce(DoAll(CheckDeviceThread(&device_thread, false),
                               Return(true)));
  EXPECT_CALL(device, SetBoolMember())
      .Times(1).WillOnce(CheckDeviceThread(&device_thread, true));

  // set from handle
  async_task = AsyncTaskHandle(new AsyncTask());
  async_task->AddObserver(&async_observer);

  EXPECT_CALL(async_observer, OnAsyncTaskStart(async_task.get(), NULL))
      .Times(1).WillOnce(
          WithoutArgs(
            Invoke(static_cast<PropertyTest*>(this), &PropertyTest::CheckAsyncStart)));
  EXPECT_CALL(async_observer, OnAsyncTaskFinish(_, _, _)).Times(0);
  EXPECT_CALL(async_observer, OnAsyncTaskProgress(_, _, _)).Times(0);

  
  handle.bool_property.set_value(true, async_task);
  
  RunLoop run_loop;
  quit = run_loop.QuitClosure();
  run_loop.Run();
}

void PropertyTest::CheckAsyncStart() {
  VerifyObserver();
  
  EXPECT_CALL(async_observer, OnAsyncTaskStart(_, _)).Times(0);
  EXPECT_CALL(async_observer, OnAsyncTaskFinish(async_task.get(), kFinish, NULL))
      .Times(1).WillOnce(
            WithoutArgs(Invoke(this, &PropertyTest::CheckAsyncFinish)));
  EXPECT_CALL(async_observer, OnAsyncTaskProgress(_, _, _)).Times(0);
}

void PropertyTest::CheckAsyncFinish() {
  VerifyObserver();
  Verify();

  // no fetch error and property now
  // EXPECT_EQ(OK, LastDeviceError());
  // EXPECT_EQ(true, handle.bool_property.value());
  EXPECT_EQ(true, device.bool_member.value());

  MessageLoopProxy::current()->PostTask(FROM_HERE, quit);
}
