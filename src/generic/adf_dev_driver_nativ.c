/*
 * generic/adf_dev_driver_nativ.c
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
 *  along with ADFLib; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#include<stdlib.h>
#include<string.h>
#include "adf_dev_driver_nativ.h"
#include"adf_err.h"
#include "adf_env.h"


/*
 * myInitDevice
 *
 * must fill 'dev->size'
 */
static struct AdfDevice * myInitDevice ( const char * const  name,
                                        const AdfAccessMode mode )
{
    (void) name, (void) mode;
    return NULL;
}


/*
 * myReadSector
 *
 */
ADF_RETCODE myReadSector ( struct AdfDevice * const dev,
                       const uint32_t           n,
                       const unsigned           size,
                       uint8_t * const          buf )
{
    (void) dev, (void) n, (void) size, (void) buf;
    return ADF_RC_ERROR;
}


/*
 * myWriteSector
 *
 */
ADF_RETCODE myWriteSector ( struct AdfDevice * const dev,
                        const uint32_t           n,
                        const unsigned           size,
                        const uint8_t * const    buf )
{
    (void) dev, (void) n, (void) size, (void) buf;
    return ADF_RC_ERROR;
}


/*
 * myReleaseDevice
 *
 * free native device
 */
ADF_RETCODE myReleaseDevice ( struct AdfDevice * const dev )
{
    (void) dev;
    return ADF_RC_ERROR;
}


/*
 * myIsDevNative
 *
 */
static bool myIsDevNative(void)
{
    return false;
}


static bool myIsDevice ( const char * const devName )
{
    (void) devName;
    return false;
}


const struct AdfDeviceDriver adfDeviceDriverNative = {
    .name        = "native generic",
    .data        = NULL,
    .createDev   = NULL,
    .openDev     = myInitDevice,
    .closeDev    = myReleaseDevice,
    .readSector  = myReadSector,
    .writeSector = myWriteSector,
    .isNative    = myIsDevNative,
    .isDevice    = myIsDevice
};


/*##########################################################################*/
