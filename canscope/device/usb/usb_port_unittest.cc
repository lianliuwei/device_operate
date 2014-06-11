#include "testing/gtest/include/gtest/gtest.h"

#include <tchar.h>

#include "base/bind.h"
#include "base/file_util.h"
#include "base/path_service.h"
#include "base/base_paths.h"
#include "base/files/file_path.h"
#include "base/json/json_file_value_serializer.h"
#include "base/command_line.h"
#include "base/strings/string_number_conversions.h"

#include "canscope/device/usb/usb_port.h"
#include "canscope/device/usb_port_device_delegate.h"
#include "canscope/device/register/device_info.h"
#include "canscope/device/test/scoped_open_device.h"

using namespace std;
using namespace base;
using namespace canscope;


namespace {
Value* GetConfig() {
  FilePath config_dir;  
  PathService::Get(base::DIR_EXE, &config_dir);
  CommandLine* command_line = CommandLine::ForCurrentProcess();
  FilePath command_path = command_line->GetSwitchValuePath("config_path");
  FilePath config_file = config_dir.Append(L"usb_port_unittest_config.json");
  JSONFileValueSerializer json_file(
      command_path.empty() ? config_file : command_path);
  json_file.set_allow_trailing_comma(true);
  int error_code;
  string error_message;
  scoped_ptr<Value> config(json_file.Deserialize(&error_code, &error_message));
  CHECK(config.get() != NULL) 
      << "error_code: " << JSONFileValueSerializer::GetErrorMessageForCode(error_code)
      << "error_message: " << error_message; 
  
  return config.release();
}

enum RWType {
  kR,
  kW,
  kRW,
};

enum AddrType {
  kEP1,
  kEP2,
  kEP3,
//  kI2C,
};

struct Cmd {
  RWType rw_type;
  AddrType addr_type;
  uint32 addr;
  vector<uint8> data;
  int size;
};

ListValue* GetAsList(Value* value) {
  CHECK(value->IsType(Value::TYPE_LIST));
  ListValue* list;
  value->GetAsList(&list);
  return list;
}

DictionaryValue* GetAsDict(Value* value) {
  CHECK(value->IsType(Value::TYPE_DICTIONARY));
  DictionaryValue* dict;
  value->GetAsDictionary(&dict);
  return dict;
}


string DictGetStr(DictionaryValue* value, string key, int i) {
  string out;
  bool ret = value->GetString(key, &out);
  CHECK(ret) << "item: " << i << " key: " << key;
  return out;
}

int DictGetInt(DictionaryValue* value, string key, int i) {
  int out;
  bool ret = value->GetInteger(key, &out);
  CHECK(ret) << "item: " << i << " key: " << key;
  return out;
}

uint32 DictGetHex(DictionaryValue* value, string key, int i) {
  uint64 out;
  bool ret = HexStringToUInt64(DictGetStr(value, key, i), &out);
  CHECK(ret) << "item: " << i << " key: " << key;
  return static_cast<uint32>(out);
}

vector<uint8> DictGetBytes(DictionaryValue* value, string key, int i) {
  vector<uint8> out;
  bool ret = HexStringToBytes(DictGetStr(value, key, i), &out);
  CHECK(ret) << "item: " << i << " key: " << key;
  return out;
}

vector<Cmd> ParseCmd(Value* value) {
  CHECK(value);
  ListValue* list = GetAsList(value);
  vector<Cmd> cmds;
  int i =0;
  for (ListValue::iterator it = list->begin(); 
      it != list->end(); ++it, ++i) {
    Cmd cmd;
    DictionaryValue* cmd_value = GetAsDict(*it);
    string type_str = DictGetStr(cmd_value, "type", i);

    if (type_str == "rw") {
      cmd.rw_type = kRW;
    } else if (type_str == "r") {
      cmd.rw_type = kR;
    } else if (type_str == "w") {
      cmd.rw_type = kW;
    } else {
      NOTREACHED();
    }
    string addr_type = DictGetStr(cmd_value, "addr_type", i);
    if (addr_type == "EP1") {
      cmd.addr_type = kEP1;
    } else if (addr_type == "EP2") {
      cmd.addr_type = kEP2;
    } else if (addr_type == "EP3") {
      cmd.addr_type = kEP3;
    } else {
      NOTREACHED();
    }
    cmd.addr = DictGetHex(cmd_value, "addr", i);
    if (cmd.rw_type == kR) {
      cmd.size = DictGetInt(cmd_value, "size", i);
    } else {
      cmd.data = DictGetBytes(cmd_value, "data", i);
    }
    cmds.push_back(cmd);
  }
  return cmds;
}

bool VectorEqual(const vector<uint8>& left, const vector<uint8>& right) {
  if (left.size() != right.size())
    return false;
  for (size_t i = 0; i < left.size(); ++i) {
    if (left[i] != right[i])
      return false;
  }
  return true;
}

}


#define EXPECT_TRUE_OR_RET(ret) \
  EXPECT_EQ(true, (ret)); \
  if(!(ret)) \
  return

TEST(UsbPortTest, EnumDevices) {
  std::vector<string16> devices;
  bool ret = EnumDevices(&devices);
  EXPECT_TRUE_OR_RET(ret);
  for (size_t i = 0; i < devices.size(); ++i) {
    cout << "Device " << i << ": " << devices[i] << endl;
  }
}

namespace canscope {

TEST(UsbPortTest, LoadAndTest) {
  scoped_ptr<Value> value(GetConfig());
  vector<Cmd> cmds = ParseCmd(value.get());
  
  scoped_ptr<UsbPortDeviceDelegate> device_delegate(new UsbPortDeviceDelegate);
  ScopedOpenDevice device_open(device_delegate.get());
  EXPECT_TRUE_OR_RET(device_open.IsOpen());
  
  UsbPort* port = &(device_delegate->usb_port_);
  bool ret;
  for (int i = 0; i < static_cast<int>(cmds.size()); ++i) {
    Cmd& current = cmds[i];
    // bool ReadWriteCallback(uint8* buffer, int size);
    typedef Callback<bool(uint8*, int)> ReadWriteType;
    ReadWriteType read_callback;
    ReadWriteType write_callback;
    if (current.addr_type == kEP1) {
      read_callback = Bind(&UsbPort::ReadEP1, Unretained(port), current.addr, kUsbModelNormal);
      write_callback = Bind(&UsbPort::WriteEP1, Unretained(port), current.addr, kUsbModelNormal);

    } else if (current.addr_type == kEP2) {
      read_callback = Bind(&UsbPort::ReadEP2, Unretained(port), current.addr, kUsbModelStream);
      write_callback = Bind(&UsbPort::WriteEP2, Unretained(port), current.addr, kUsbModelStream);

    } else if (current.addr_type == kEP3) {
      read_callback = Bind(&UsbPort::ReadEP3, Unretained(port), current.addr, kUsbModelStream);
      if (current.rw_type == kR || current.rw_type == kRW) {
        EXPECT_TRUE(false) << "no EP3 write support";
        continue;
      }
    }

    cout << "Test "<< i << endl;
    if (current.rw_type == kW) {
      ret = write_callback.Run(&(current.data[0]), current.data.size());
      EXPECT_EQ(true, ret);

    } else if (current.rw_type == kR) {
      vector<uint8> buffer(current.size);
      ret = read_callback.Run(&(buffer[0]), buffer.size());
      EXPECT_EQ(true, ret);

      cout << "Reg Read data: " << HexEncode(&(buffer[0]), buffer.size())<<endl;

    } else if (current.rw_type == kRW) {
      ret = write_callback.Run(&(current.data[0]), current.data.size());
      EXPECT_EQ(true, ret);
      if (!ret)
        continue;
      vector<uint8> buffer(current.data.size());
      ret = read_callback.Run(&(buffer[0]), buffer.size());
      EXPECT_EQ(true, ret);
      if (!ret)
        continue;
      EXPECT_TRUE(VectorEqual(current.data, buffer)) 
        << "reg ReadWrite: get data: " << HexEncode(&(buffer[0]), buffer.size())
        << " expect data: " << HexEncode(&(current.data[0]), current.data.size());
    }

  } 
}

}