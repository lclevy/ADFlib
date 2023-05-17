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
#include "ramdisk.h"

uint8_t ramdiskData[RAMDISK_SIZE];

static RETCODE ramdiskInit( struct AdfDevice * const dev,
                            const char * const       name,
                            const BOOL               ro )
{
    dev->readOnly = ro;
    dev->heads = 2;
    dev->sectors = 11;
    dev->cylinders = 80;
    dev->size = RAMDISK_SIZE;
    return RC_OK;
}


static RETCODE ramdiskRelease ( struct AdfDevice * const dev )
{
    return RC_OK;
}


static RETCODE ramdiskReadSector ( struct AdfDevice * const dev,
                                   const uint32_t           n,
                                   const unsigned           size,
                                   uint8_t * const          buf )
{
    unsigned int offset = n * 512;
    if (offset > RAMDISK_SIZE || (offset + size) > RAMDISK_SIZE) {
        return RC_ERROR;
    }
    memcpy(buf, &ramdiskData[offset], size);
    return RC_OK;
}

static RETCODE ramdiskWriteSector ( struct AdfDevice * const dev,
                                    const uint32_t           n,
                                    const unsigned           size,
                                    const uint8_t * const    buf )
{
    unsigned int offset = n * 512;
    if (offset > RAMDISK_SIZE || (offset + size) > RAMDISK_SIZE) {
        return RC_ERROR;
    }
    memcpy(&ramdiskData[offset], buf, size);
    return RC_OK;
}

static BOOL ramdiskIsDevNative ( const char * const devName )
{
    return strcmp(devName, RAMDISK_DEVICE_NAME) == 0;
}

struct AdfNativeFunctions ramdiskDevice = {
    NULL,
    &ramdiskInit,
    &ramdiskRelease,
    &ramdiskReadSector,
    &ramdiskWriteSector,
    &ramdiskIsDevNative
};
