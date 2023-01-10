
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

//struct Device* adfCreateDev(char* filename, int32_t cylinders, int32_t heads, int32_t sectors);

//RETCODE adfReadBlockDev( struct Device* dev, int32_t nSect, int32_t size, uint8_t* buf );
//RETCODE adfWriteBlockDev(struct Device* dev, int32_t nSect, int32_t size, uint8_t* buf );


#endif
