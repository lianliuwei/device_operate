#pragma once

#include "base/basictypes.h"

namespace canscope {
// SSTQ
// DeviceInfo struct
const uint32 kDeviceInfoAddr = 0x00;
const uint32 kDeviceInfoIDOffset = 0x00;
const uint32 kDeviceInfoLoaderVersionOffset = 0x04;
const uint32 kDeviceInfoDeviceTypeOffset = 0x08;
const uint32 kDeviceInfofirmwareVersionOffset = 0x0C;
const uint32 kDeviceInfoFpgaVersionOffset = 0x10;
const uint32 kDeviceInfoDeviceSerialNumOffset = 0x14;
const uint32 kDeviceInfoProduceDateOffset = 0x18;
const uint32 kDeviceInfoUsbSpeedOffset = 0x1C;
const uint32 kDeviceInfoFirmwareUpdateFlagOffset = 0x1E;
const uint32 kDeviceInfoVoltage12Offset = 0x20;
const uint32 kDeviceInfoVoltage1p2Offset = 0x22;
const uint32 kDeviceInfoVoltage3p3Offset = 0x24;
const uint32 kDeviceInfoVoltage2p5Offset = 0x26;
const uint32 kDeviceInfoVoltage5Offset = 0x28;
const uint32 kDeviceInfoFpgaCoreTempOffset = 0x2A;
const uint32 kDeviceInfoEnvTempOffset = 0x2C;
const uint32 kDeviceInfoSize = kDeviceInfoEnvTempOffset + 0x2 + 0x12;

// EP1
const uint32 kBaseAddr = 0x82000000;
// sja1000 register
const uint32 kSJA1000Addr = kBaseAddr + 0x0;
const uint32 kSJABtrOffset = 0x0;
const uint32 kSJASlientOffset = 0x2;
const uint32 kSJAFrameDataOffset = 0x4;
const uint32 kSJASendStateOffset = 0x14;
const uint32 kSJASAlcOffset = kSJASendStateOffset;
const uint32 kSJASEccOffset = kSJASendStateOffset + 1;
const uint32 kSJAIntOffset = kSJASendStateOffset + 2;
const uint32 kSJAStateOffset = kSJASendStateOffset + 3;
const int kSJA1000Size = kSJASendStateOffset + 0x4;
// wave storage register
const uint32 kWaveStorageAddr = kBaseAddr + 0x80;
const uint32 kFrameLenOffset = 0x4;
const uint32 kWaveStartOffset = 0x8;
const uint32 kWEaveEndOffset = 0xC;
const uint32 kDivCoreOffset = 0x10;
const uint32 KWaveLostOffset = 0x14;
const int kWaveStorageSize = KWaveLostOffset + 0x4;
// soft diff register
const uint32 kSoftDiffAddr = kBaseAddr + 0xC0;
const uint32 kSjaBtrOffset = 0x0;
const uint32 kSysCfgOffset = 0x3;
const uint32 kChSensCanhOffset = 0x8;
const uint32 kChSensCanlOffset = 0x9;
const uint32 kChSensCandiffOffset = 0xA;
const uint32 kChZerolCanhOffset = 0xC;
const uint32 kChZerolCanlOffset = 0xD;
const uint32 kChZerolCandiffOffset = 0xE;
const uint32 kDiffCtrlOffset = 0x10;
const uint32 kFilteringOffset = 0x14;
const uint32 kChZerohCanhOffset = 0x18;
const uint32 kChZerohCanlOffset = 0x19;
const uint32 kChZerohCandiffOffset = 0x1A;
const int kSoftDiffSize = kChZerohCandiffOffset + 0x1 + 0x1;
// frame storage register
const uint32 kFrameStorageAddr = kBaseAddr + 0x100;
const uint32 kFrameDepthOffset = 0x0;
const uint32 kFrameNumOffset = 0x4;
const uint32 kFrameOverFlowOffset = 0x8;
const int kFrameStorageSize = kFrameOverFlowOffset + 0x4;
// wave read ctrl register
const uint32 kWaveReadCtrlAddr = kBaseAddr + 0x110;
const uint32 kReadStartOffset = 0x0;
const uint32 kSectionStartOffset = 0x4;
const uint32 kSectionEndOffset = 0x8;
const int kWaveReadCtrlSize = kSectionEndOffset + 0x4;
// analog ctrl register
const uint32 kAnalogCtrlAddr = kBaseAddr + 0x140;
const uint32 kGainCh1Offset = 0x8;
const uint32 kOffsetCh1Offset = 0xC;
const uint32 kGainCh2Offset = 0x10;
const uint32 kOffsetCh2Offset = 0x14;
const int kAnalogCtrlSize  = kOffsetCh2Offset + 0x4;
// analog switch register
const uint32 kAnalogSwitchAddr = kBaseAddr + 0x160;
const uint32 kAnalogSwitchOffset = 0x0;
const int kAnalogSwitchSize = kAnalogSwitchOffset + 0x4;
// trigger1 register
const uint32 kTrigger1Addr = kBaseAddr + 0x180;
const uint32 kTrigPreOffset = 0x0;
const uint32 kTrigPostOffset = 0x4;
const uint32 kAutoTimeOffset = 0x8;
const uint32 kDivCoeOffset = 0xC;
const int kTrigger1Size = kDivCoeOffset + 0x4;
// trigger state
const uint32 kTriggerStateAddr = kBaseAddr + 0x190;
const uint32 kTrigStateOffset = 0x0;
const uint32 kTrigAddrOffset = 0x4;
const uint32 kTrigSaddrOffset = 0x8;
const uint32 kTrigEaddrOffset = 0xC;
const int kTriggerStateSize = kTrigEaddrOffset + 0x4;
// trigger2 register
const uint32 kTrigger2Addr = kBaseAddr + 0x1A0;
const uint32 kTrigSourceOffset = 0x0;
const uint32 kCmpVolOffset = 0x4;
const uint32 kTrigTypeOffset = 0x8;
const uint32 kTrigTimeOffset = 0xC;
const int kTrigger2Size = kTrigTimeOffset + 0x4;
// scope ctrl register
const uint32 kScopeCtrlAddr = kBaseAddr + 0x1B0;
const uint32 kScopeCtrlOffset = 0x0;
const int kScopeCtrlSize = kScopeCtrlOffset + 0x4;
// fpga send register
const uint32 kFpgaSendAddr = kBaseAddr + 0x1380;
const uint32 kBtrDivOffset = 0x0;
const uint32 kBitNumOffset = 0x2;
const uint32 kSendNumOffset = 0x3;
const uint32 kFpgaSendFrameDataOffset = 0x4;
const uint32 kAckBitOffset = 0x18;
const uint32 kAckOffset = 0x19;
const uint32 kSendStateOffset = 0x1C;
const uint32 kAckStateOffset = 0x1C;
const uint32 kDstbStartOffset = 0x20;
const uint32 kDstbEndOffset = 0x22;
const uint32 kDstbDataOffset = 0x24;
const uint32 kDstbOffset = 0x38;
const int kFpgaSendSize = kDstbOffset + 0x4;

// EP2

// EP3
// scope data read addr
const uint32 kScopeReadAddr = kBaseAddr + 0x138;
// frame data read addr
const uint32 kFramdReadAddr = kBaseAddr + 0x128;
}; // namespace canscope