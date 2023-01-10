
#ifndef __ADF_DEV_H__
#define __ADF_DEV_H__

#include "adf_defs.h"
#include "prefix.h"

PREFIX struct Device * adfOpenDev ( char * filename, BOOL ro );
PREFIX void adfCloseDev ( struct Device * dev );

int adfDevType ( struct Device * dev );
PREFIX void adfDeviceInfo ( struct Device * dev );

PREFIX struct Device* adfMountDev ( char* filename, BOOL ro );
PREFIX void adfUnMountDev ( struct Device * dev );

#endif
