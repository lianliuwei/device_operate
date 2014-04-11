#include "canscope/device/canscope_device_property_constants.h"

namespace canscope {
// version
const char kVersionPath[]              = "Version";
// OscDevice
const char kOscDevice[]                = "OscDevice";
const char kOscCANHVoltRange[]         = "CANH.Range";
const char kOscCANHOffset[]            = "CANH.Offset";
const char kOscCANHCoupling[]          = "CANH.Coupling";
const char kOscCANLVoltRange[]         = "CANL.Range";
const char kOscCANLOffset[]            = "CANL.Offset";
const char kOscCANLCoupling[]          = "CANL.Coupling";
const char kOscCANDIFFVoltRange[]      = "CANDIFF.Range";
const char kOscCANDIFFOffset[]         = "CANDIFF.Offset";
const char kOscDiffCtrl[]              = "DiffCtrl";
const char kOscTimeBase[]              = "Time.Base";
const char kOscTimeOffset[]            = "Time.Offset";
const char kOscAutoTime[]              = "Trigger.AutoTime";
const char kOscTriggerSource[]         = "Trigger.Source";
const char kOscTriggerType[]           = "Trigger.Type";
const char kOscTriggerMode[]           = "Trigger.Mode";
const char kOscTriggerSens[]           = "Trigger.Sens";
const char kOscCompare[]               = "Trigger.Compare";
const char kOscTriggerVolt[]           = "Trigger.Volt";
const char kOscTimeParam[]             = "TimeParam";
// FrameDevice
const char kFrameDevice[]                = "FrameDevice";
const char kFrameDeviceEnable[]          = "DeviceEanble";
const char kFrameAckEnable[]             = "AckEnable";
const char kFrameSjaBtr[]                = "SjaBtr";
const char kFrameFrameStoragePercent[]   = "FrameStoragePerent";
const char kFrameBitSampleRate[]         = "BitSampleRate";
const char kFrameBit[]                   = "BitNum";
} // namespace canscope
