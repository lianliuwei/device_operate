#pragma once

#include <Guiddef.h>
#include "base/basictypes.h"

namespace canscope {
// device GUID
const GUID GUID_CLASS_CANSCOPE ={
    0xc062ae2f, 0xe3fe, 0x48f3, 0x9c, 0xed, 0x98, 0x58, 0xfc, 0xc1, 0xbc, 0xc3,
};

// command
static const uint16 WREP1 = 0xFC03;
static const uint16 RDEP1 = 0xFB04;
static const uint16 WREP2 = 0xFA05;
static const uint16 RDEP2 = 0xF906;
static const uint16 WREP3 = 0xF807;
static const uint16 RDEP3 = 0xF708;
static const uint16 DOWNLOAD_FPGA = 0xFE01;
static const uint16 SSTQ = 0xFD02;
static const uint16 WRITE_SUCCESS = 0x55AA;
// model
enum UsbMode {
  kUsbModelNormal = 0x0,
  kUsbModelStream = 0x1,
};

static const uint32 CMD_MAX_LEN = 48;
static const uint32 DATA_MAX_LEN = 0x10000;

// command struct
static const uint32 kUsbCommandAddr = 0x0; // no addr need
static const uint32 kUsbCommandIDOffset = 0x0;
static const uint32 kUsbCommandModeOffset = 0x2;
static const uint32 kUsbCommandAddrOffset = 0x4;
static const uint32 kUsbCommandDataSizeOffset = 0x8;
static const uint32 kUsbCommandFucAddrOffset = 0xC;
static const uint32 kUsbCommandSize = kUsbCommandFucAddrOffset + 0x4;
static const uint32 kUsbCommandReadOffset = 0x4;

// DeviceInfo struct
static const uint32 kDeviceInfoAddr = 0x00;
static const uint32 kDeviceInfoIDOffset = 0x00;
static const uint32 kDeviceInfoLoaderVersionOffset = 0x04;
static const uint32 kDeviceInfoDeviceTypeOffset = 0x08;
static const uint32 kDeviceInfofirmwareVersionOffset = 0x0C;
static const uint32 kDeviceInfoFpgaVersionOffset = 0x10;
static const uint32 kDeviceInfoDeviceSerialNumOffset = 0x14;
static const uint32 kDeviceInfoProduceDateOffset = 0x18;
static const uint32 kDeviceInfoUsbSpeedOffset = 0x1C;
static const uint32 kDeviceInfoFirmwareUpdateFlagOffset = 0x1E;
static const uint32 kDeviceInfoVoltage12Offset = 0x20;
static const uint32 kDeviceInfoVoltage1p2Offset = 0x22;
static const uint32 kDeviceInfoVoltage3p3Offset = 0x24;
static const uint32 kDeviceInfoVoltage2p5Offset = 0x26;
static const uint32 kDeviceInfoVoltage5Offset = 0x28;
static const uint32 kDeviceInfoFpgaCoreTempOffset = 0x2A;
static const uint32 kDeviceInfoEnvTempOffset = 0x2C;
static const uint32 kDeviceInfoSize = kDeviceInfoEnvTempOffset + 0x2 + 0x12;

} // namespace canscope
