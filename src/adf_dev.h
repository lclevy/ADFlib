
#ifndef __ADF_DEV_H__
#define __ADF_DEV_H__

#include <stdio.h>

#include "adf_defs.h"
#include "adf_vol.h"
#include "prefix.h"


struct Partition {
    int32_t startCyl;
    int32_t lenCyl;
    char* volName;
    int volType;
};

/* ----- DEVICES ----- */

#define DEVTYPE_FLOPDD 		1
#define DEVTYPE_FLOPHD 		2
#define DEVTYPE_HARDDISK 	3
#define DEVTYPE_HARDFILE 	4

struct AdfDevice {
    int devType;               /* see below */
    BOOL readOnly;
    int32_t size;                 /* in bytes */

    int nVol;                  /* partitions */
    struct AdfVolume** volList;

    int32_t cylinders;            /* geometry */
    int32_t heads;
    int32_t sectors;

    BOOL isNativeDev;
    union {
        void *nativeDev;
        FILE *fd;
    };
};


PREFIX struct AdfDevice * adfOpenDev ( const char * const filename,
                                       const BOOL         ro );
PREFIX void adfCloseDev ( struct AdfDevice * const dev );

PREFIX int adfDevType ( struct AdfDevice * dev );
PREFIX void adfDeviceInfo ( struct AdfDevice * dev );

PREFIX struct AdfDevice * adfMountDev ( const char * const filename,
                                        const BOOL         ro );
PREFIX void adfUnMountDev ( struct AdfDevice * const dev );

//struct AdfDevice* adfCreateDev(char* filename, int32_t cylinders, int32_t heads, int32_t sectors);

RETCODE adfReadBlockDev ( struct AdfDevice * const dev,
                          const int32_t            pSect,
                          const int32_t            size,
                          uint8_t * const          buf );

RETCODE adfWriteBlockDev ( struct AdfDevice * const dev,
                           const int32_t            pSect,
                           const int32_t            size,
                           const uint8_t * const    buf );
#endif
