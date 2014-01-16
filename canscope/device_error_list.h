// This file intentionally does not have header guards, it's included
// inside a macro to generate enum values.
// 
// This file contains the list of device errors.
// 
// Ranges:
//      10 - 99 normal error
//    100 - 199 usb port error 
//    200 - 299 pci port error
//    400 - 499 check error

DEVICE_ERROR(DEVICE_NO_START, -11)

// device is in CLOSING state, can no operate device
DEVICE_ERROR(DEVICE_CLOSING, -12)

DEVICE_ERROR(READ_DEVICE, -13)

DEVICE_ERROR(WRITE_DEVICE, -14)

DEVICE_ERROR(DEVICE_LOCEKD, -15)

DEVICE_ERROR(DEVICE_OPEN, -16)

DEVICE_ERROR(DEVICE_INFO, -17)

DEVICE_ERROR(DEVICE_LOADFPGAFILE, -18)

DEVICE_ERROR(DEVICE_DOWNLOADFPGA, -19)

// check error happen on handle
DEVICE_ERROR(OVERFLOW, -400)

DEVICE_ERROR(UNDERFLOW, -401)

// no specific error
DEVICE_ERROR(INVAILD_VALUE, -499)
