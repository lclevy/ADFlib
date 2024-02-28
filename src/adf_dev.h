
#ifndef ADF_DEV_H
#define ADF_DEV_H

#include "adf_types.h"
#include "adf_err.h"
#include "adf_dev_driver.h"
#include "adf_prefix.h"
#include "adf_vol.h"


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
    char * name;
    int devType;               /* see below */
    bool readOnly;
    uint32_t size;                /* in bytes */

    uint32_t cylinders;            /* geometry */
    uint32_t heads;
    uint32_t sectors;

    const struct AdfDeviceDriver * drv;
    void *                   drvData;   /* driver-specific device data,
                                           (private, use only in the driver code!) */
    bool mounted;

    // stuff available when mounted
    int nVol;                  /* partitions */
    struct AdfVolume** volList;
};


/*
 * adfDevCreate and adfDevOpen
 *
 * creates or open an ADF device without reading any data (ie. without finding volumes)
 *
 * An created/opened device either has to be mounted (to be used with functions
 * requiring volume data) or only functions using block access on the device
 * level (with adfDevRead/WriteBlock) can be used
 * (ie. this applies to adfCreateFlop/Hd); in general this level of access
 * is for: partitioning / formatting / creating file system data / cloning
 * the whole device on _device_ block level - and similar.
 */

PREFIX struct AdfDevice * adfDevCreate ( const char * const driverName,
                                         const char * const name,
                                         const uint32_t     cylinders,
                                         const uint32_t     heads,
                                         const uint32_t     sectors );

PREFIX struct AdfDevice * adfDevOpen ( const char * const  name,
                                       const AdfAccessMode mode );

/*
 * adfDevOpenWithDriver
 *
 * allows to avoid automatic driver selection done in adfOpenDev and enforce
 * opening a file/device with the driver specified by its name
 * (esp. useful for custom, user-implemented device drivers)
 */
PREFIX struct AdfDevice * adfDevOpenWithDriver ( const char * const  driverName,
                                                 const char * const  name,
                                                 const AdfAccessMode mode );

PREFIX void adfDevClose ( struct AdfDevice * const dev );


PREFIX int adfDevType ( const struct AdfDevice * const dev );
PREFIX void adfDevInfo ( const struct AdfDevice * const dev );

PREFIX ADF_RETCODE adfDevMount ( struct AdfDevice * const dev );
PREFIX void adfDevUnMount ( struct AdfDevice * const dev );


ADF_RETCODE adfDevReadBlock ( struct AdfDevice * const dev,
                              const uint32_t           pSect,
                              const uint32_t           size,
                              uint8_t * const          buf );

ADF_RETCODE adfDevWriteBlock ( struct AdfDevice * const dev,
                               const uint32_t           pSect,
                               const uint32_t           size,
                               const uint8_t * const    buf );
#endif  /* ADF_DEV_H */
