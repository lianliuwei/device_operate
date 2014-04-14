#pragma once

#include "base/basictypes.h"

namespace canscope {
// SSTQ
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

// EP1
static const uint32 kBaseAddr = 0x82000000;
// sja1000 register
static const uint32 kSJA1000Addr = kBaseAddr + 0x0;
static const uint32 kSJABtrOffset = 0x0;
static const uint32 kSJASlientOffset = 0x2;
static const uint32 kSJAFrameDataOffset = 0x4;
static const uint32 kSJASendStateOffset = 0x14;
static const uint32 kSJASAlcOffset = kSJASendStateOffset;
static const uint32 kSJASEccOffset = kSJASendStateOffset + 1;
static const uint32 kSJAIntOffset = kSJASendStateOffset + 2;
static const uint32 kSJAStateOffset = kSJASendStateOffset + 3;
static const int kSJA1000Size = kSJASendStateOffset + 0x4;
// wave storage register
static const uint32 kWaveStorageAddr = kBaseAddr + 0x80;
static const uint32 kFrameLenOffset = 0x4;
static const uint32 kWaveStartOffset = 0x8;
static const uint32 kWEaveEndOffset = 0xC;
static const uint32 kDivCoreOffset = 0x10;
static const uint32 KWaveLostOffset = 0x14;
static const int kWaveStorageSize = KWaveLostOffset + 0x4;
// soft diff register
static const uint32 kSoftDiffAddr = kBaseAddr + 0xC0;
static const uint32 kSjaBtrOffset = 0x0;
static const uint32 kSysCfgOffset = 0x3;
static const uint32 kChSensCanhOffset = 0x8;
static const uint32 kChSensCanlOffset = 0x9;
static const uint32 kChSensCandiffOffset = 0xA;
static const uint32 kChZerolCanhOffset = 0xC;
static const uint32 kChZerolCanlOffset = 0xD;
static const uint32 kChZerolCandiffOffset = 0xE;
static const uint32 kDiffCtrlOffset = 0x10;
static const uint32 kFilteringOffset = 0x14;
static const uint32 kChZerohCanhOffset = 0x18;
static const uint32 kChZerohCanlOffset = 0x19;
static const uint32 kChZerohCandiffOffset = 0x1A;
static const int kSoftDiffSize = kChZerohCandiffOffset + 0x1 + 0x1;
// frame storage register
static const uint32 kFrameStorageAddr = kBaseAddr + 0x100;
static const uint32 kFrameDepthOffset = 0x0;
static const uint32 kFrameNumOffset = 0x4;
static const uint32 kFrameOverFlowOffset = 0x8;
static const int kFrameStorageSize = kFrameOverFlowOffset + 0x4;
// wave read ctrl register
static const uint32 kWaveReadCtrlAddr = kBaseAddr + 0x110;
static const uint32 kReadStartOffset = 0x0;
static const uint32 kSectionStartOffset = 0x4;
static const uint32 kSectionEndOffset = 0x8;
static const int kWaveReadCtrlSize = kSectionEndOffset + 0x4;
// analog ctrl register
static const uint32 kAnalogCtrlAddr = kBaseAddr + 0x140;
static const uint32 kGainCh1Offset = 0x8;
static const uint32 kOffsetCh1Offset = 0xC;
static const uint32 kGainCh2Offset = 0x10;
static const uint32 kOffsetCh2Offset = 0x14;
static const int kAnalogCtrlSize  = kOffsetCh2Offset + 0x4;
// analog switch register
static const uint32 kAnalogSwitchAddr = kBaseAddr + 0x160;
static const uint32 kAnalogSwitchOffset = 0x0;
static const int kAnalogSwitchSize = kAnalogSwitchOffset + 0x4;
// trigger1 register
static const uint32 kTrigger1Addr = kBaseAddr + 0x180;
static const uint32 kTrigPreOffset = 0x0;
static const uint32 kTrigPostOffset = 0x4;
static const uint32 kAutoTimeOffset = 0x8;
static const uint32 kDivCoeOffset = 0xC;
static const int kTrigger1Size = kDivCoeOffset + 0x4;
// trigger state
static const uint32 kTriggerStateAddr = kBaseAddr + 0x190;
static const uint32 kTrigStateOffset = 0x0;
static const uint32 kTrigAddrOffset = 0x4;
static const uint32 kTrigSaddrOffset = 0x8;
static const uint32 kTrigEaddrOffset = 0xC;
static const int kTriggerStateSize = kTrigEaddrOffset + 0x4;
// trigger2 register
static const uint32 kTrigger2Addr = kBaseAddr + 0x200;
static const uint32 kTrigSourceOffset = 0x0;
static const uint32 kCmpVolOffset = 0x4;
static const uint32 kTrigTypeOffset = 0x8;
static const uint32 kTrigTimeOffset = 0xC;
static const int kTrigger2Size = kTrigTimeOffset + 0x4;
// scope ctrl register
static const uint32 kScopeCtrlAddr = kBaseAddr + 0x210;
static const uint32 kScopeCtrlOffset = 0x0;
static const int kScopeCtrlSize = kScopeCtrlOffset + 0x4;
// fpga send register
static const uint32 kFpgaSendAddr = kBaseAddr + 0x800;
static const uint32 kBtrDivOffset = 0x0;
static const uint32 kBitNumOffset = 0x2;
static const uint32 kSendNumOffset = 0x3;
static const uint32 kFpgaSendFrameDataOffset = 0x4;
static const uint32 kAckBitOffset = 0x18;
static const uint32 kAckOffset = 0x19;
static const uint32 kSendStateOffset = 0x1C;
static const uint32 kAckStateOffset = 0x1C;
static const uint32 kDstbStartOffset = 0x20;
static const uint32 kDstbEndOffset = 0x22;
static const uint32 kDstbDataOffset = 0x24;
static const uint32 kDstbOffset = 0x38;
static const int kFpgaSendSize = kDstbOffset + 0x4;

// EP2

// EP3
// scope data read addr
static const uint32 kScopeReadAddr = kBaseAddr + 0x138;
// frame data read addr
static const uint32 kFramdReadAddr = kBaseAddr + 0x128;
}; // namespace canscope