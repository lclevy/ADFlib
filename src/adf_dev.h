
#ifndef __ADF_DEV_H__
#define __ADF_DEV_H__

#include "adf_types.h"
#include "adf_err.h"
#include "adf_vol.h"
#include "prefix.h"

#include <stdio.h>

struct Partition {
    int32_t startCyl;
    int32_t lenCyl;
    char* volName;
    uint8_t volType;
};

/* ----- DEVICES ----- */

#define DEVTYPE_FLOPDD 		1
#define DEVTYPE_FLOPHD 		2
#define DEVTYPE_HARDDISK 	3
#define DEVTYPE_HARDFILE 	4

struct AdfDevice {
    int devType;               /* see below */
    BOOL readOnly;
    uint32_t size;                /* in bytes */

    int nVol;                  /* partitions */
    struct AdfVolume** volList;

    uint32_t cylinders;            /* geometry */
    uint32_t heads;
    uint32_t sectors;

    BOOL isNativeDev;
    struct AdfNativeFunctions *nativeFct;
    void *nativeDev; /* for use by native functions */

    FILE *fd;
};


PREFIX struct AdfDevice * adfOpenDev ( const char * const  filename,
                                       const AdfAccessMode mode );
PREFIX void adfCloseDev ( struct AdfDevice * const dev );

PREFIX int adfDevType ( struct AdfDevice * dev );
PREFIX void adfDeviceInfo ( struct AdfDevice * dev );

PREFIX struct AdfDevice * adfMountDev ( const char * const  filename,
                                        const AdfAccessMode mode );
PREFIX void adfUnMountDev ( struct AdfDevice * const dev );

//struct AdfDevice* adfCreateDev(char* filename, int32_t cylinders, int32_t heads, int32_t sectors);

RETCODE adfReadBlockDev ( struct AdfDevice * const dev,
                          const uint32_t           pSect,
                          const uint32_t           size,
                          uint8_t * const          buf );

RETCODE adfWriteBlockDev ( struct AdfDevice * const dev,
                           const uint32_t           pSect,
                           const uint32_t           size,
                           const uint8_t * const    buf );
#endif
