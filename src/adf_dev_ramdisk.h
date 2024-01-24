#ifndef ADF_DEV_RAMDISK_H
#define ADF_DEV_RAMDISK_H

#include "adf_err.h"
#include "adf_nativ.h"

#define RAMDISK_SIZE (2 * 11 * 80 * 512)
#define RAMDISK_DEVICE_NAME "RAM:"

extern uint8_t ramdiskData[RAMDISK_SIZE];

extern struct AdfNativeFunctions ramdiskDevice;

#endif
