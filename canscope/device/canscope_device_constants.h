#pragma once

#include "base/basictypes.h"

namespace canscope {
// EP1
static const uint32 kBaseAddr = 0x82000000;
// soft diff register
static const uint32 kSoftDiffAddr = kBaseAddr + 0xC0;
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

// EP2

// EP3
// scope data read addr
static const uint32 kScopeReadAddr = kBaseAddr + 0x138;
// size dependent on canscope type.
// static const int kScopeReadSize = 4000;

};