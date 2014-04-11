#pragma once

namespace canscope {

// version
extern const char kVersionPath[];
const int kVersion = 1;
// OscDevice
extern const char kOscDevice[];
extern const char kOscCANHVoltRange[];
extern const char kOscCANHOffset[];
extern const char kOscCANHCoupling[];
extern const char kOscCANLVoltRange[];
extern const char kOscCANLOffset[];
extern const char kOscCANLCoupling[];
extern const char kOscCANDIFFVoltRange[];
extern const char kOscCANDIFFOffset[];
extern const char kOscDiffCtrl[];
extern const char kOscTimeBase[];
extern const char kOscTimeOffset[];
extern const char kOscAutoTime[];
extern const char kOscTriggerSource[];
extern const char kOscTriggerType[];
extern const char kOscTriggerMode[];
extern const char kOscTriggerSens[];
extern const char kOscCompare[];
extern const char kOscTriggerVolt[];
extern const char kOscTimeParam[];
// FrameDevice
extern const char kFrameDevice[];
extern const char kFrameDeviceEnable[];
extern const char kFrameAckEnable[];
extern const char kFrameSjaBtr[];
extern const char kFrameFrameStoragePercent[];
extern const char kFrameBitSampleRate[];
extern const char kFrameBit[];

}; // namespace canscope
