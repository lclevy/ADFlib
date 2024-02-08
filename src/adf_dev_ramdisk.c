/*
 * ramdisk.c
 *
 * $Id$
 *
 *  This file is part of ADFLib.
 *
 *  ADFLib is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  ADFLib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Foobar; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <stdlib.h>
#include <string.h>

#include "adf_dev_ramdisk.h"
#include "adf_env.h"

static struct AdfDevice * ramdiskCreate ( const char * const name,
                                          const uint32_t     cylinders,
                                          const uint32_t     heads,
                                          const uint32_t     sectors )
{
    struct AdfDevice * dev = ( struct AdfDevice * )
        malloc ( sizeof ( struct AdfDevice ) );
    if ( dev == NULL ) {
        adfEnv.eFct("ramdiskCreate : malloc error");
        return NULL;
    }

    dev->readOnly  = FALSE; // ( mode != ADF_ACCESS_MODE_READWRITE );
    dev->heads     = heads;
    dev->sectors   = sectors;
    dev->cylinders = cylinders;
    dev->size      = cylinders * heads * sectors * 512;

    dev->drvData = malloc ( dev->size );
    if ( dev->drvData == NULL ) {
        adfEnv.eFct("ramdiskCreate : malloc data error");
        free ( dev );
        return NULL;
    }

    dev->devType   = adfDevType ( dev );
    dev->nVol      = 0;
    dev->volList   = NULL;
    dev->mounted   = FALSE;
    dev->name      = strdup ( name );
    dev->drv       = &adfDeviceDriverRamdisk;

    return dev;
}


static RETCODE ramdiskRelease ( struct AdfDevice * const dev )
{
    free ( dev->drvData );
    free ( dev->name );
    free ( dev );
    return RC_OK;
}


static RETCODE ramdiskReadSector ( struct AdfDevice * const dev,
                                   const uint32_t           n,
                                   const unsigned           size,
                                   uint8_t * const          buf )
{
    unsigned int offset = n * 512;
    if ( offset > dev->size ||
         (offset + size) > dev->size)
    {
        return RC_ERROR;
    }
    memcpy ( buf, &( (uint8_t *) (dev->drvData) )[offset], size );
    return RC_OK;
}

static RETCODE ramdiskWriteSector ( struct AdfDevice * const dev,
                                    const uint32_t           n,
                                    const unsigned           size,
                                    const uint8_t * const    buf )
{
    unsigned int offset = n * 512;
    if ( offset > dev->size ||
         (offset + size) > dev->size )
    {
        return RC_ERROR;
    }
    memcpy ( &( (uint8_t *) (dev->drvData) )[offset], buf, size );
    return RC_OK;
}


static BOOL ramdiskIsDevNative ( void )
{
    return FALSE;
}


const struct AdfDeviceDriver adfDeviceDriverRamdisk = {
    .name        = "ramdisk",
    .data        = NULL,
    .createDev   = ramdiskCreate,
    .openDev     = NULL,
    .closeDev    = ramdiskRelease,
    .readSector  = ramdiskReadSector,
    .writeSector = ramdiskWriteSector,
    .isNative    = ramdiskIsDevNative,
    .isDevice    = NULL
};
