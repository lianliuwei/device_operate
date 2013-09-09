// This file intentionally does not have header guards, it's included
// inside a macro to generate enum values.
// 
// This file contains the list of device errors.
// 
// Ranges:
//       0 - 99 normal error
//    100 - 199 usb port error 
//    200 - 299 pci port error
//    400 - 499 check error

DEVICE_ERROR(DEVICE_NO_START, -1)

// device is in CLOSING state, can no operate device
DEVICE_ERROR(DEVICE_CLOSING, -2)

DEVICE_ERROR(READ_DEVICE, -3)

DEVICE_ERROR(WRITE_DEVICE, -4)

DEVICE_ERROR(DEVICE_LOCEKD, -5)

// check error happen on handle
DEVICE_ERROR(OVERFLOW, -400)

DEVICE_ERROR(UNDERFLOW, -401)

// no specific error
DEVICE_ERROR(INVAILD_VALUE, -499)
