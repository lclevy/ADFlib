
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

struct adfDevice {
    int devType;               /* see below */
    BOOL readOnly;
    int32_t size;                 /* in bytes */

    int nVol;                  /* partitions */
    struct adfVolume** volList;

    int32_t cylinders;            /* geometry */
    int32_t heads;
    int32_t sectors;

    BOOL isNativeDev;
    union {
        void *nativeDev;
        FILE *fd;
    };
};


PREFIX struct adfDevice * adfOpenDev ( char * filename, BOOL ro );
PREFIX void adfCloseDev ( struct adfDevice * dev );

PREFIX int adfDevType ( struct adfDevice * dev );
PREFIX void adfDeviceInfo ( struct adfDevice * dev );

PREFIX struct adfDevice * adfMountDev ( char * filename, BOOL ro );
PREFIX void adfUnMountDev ( struct adfDevice * dev );

//struct adfDevice* adfCreateDev(char* filename, int32_t cylinders, int32_t heads, int32_t sectors);

RETCODE adfReadBlockDev ( struct adfDevice * dev,
                          int32_t            pSect,
                          int32_t            size,
                          uint8_t *          buf );

RETCODE adfWriteBlockDev ( struct adfDevice * dev,
                           int32_t            pSect,
                           int32_t            size,
                           uint8_t *          buf );
#endif
