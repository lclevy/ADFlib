/*
 * adf_nativ.c
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


#include<stdlib.h>
#include<string.h>
#include"adf_nativ.h"
#include"adf_err.h"
#include "adf_env.h"


/*
 * myInitDevice
 *
 * must fill 'dev->size'
 */
RETCODE myInitDevice ( struct AdfDevice * const dev,
                       const char * const       name,
                       const BOOL               ro )
{
    (void) name;
    struct AdfNativeDevice * nDev = ( struct AdfNativeDevice * )
        malloc ( sizeof ( struct AdfNativeDevice ) );
    if (!nDev) {
        (*adfEnv.eFct)("myInitDevice : malloc");
        return RC_ERROR;
    }
    dev->nativeDev = nDev;
    if (!ro)
        /* check if device is writable, if not, force readOnly to TRUE */
        dev->readOnly = FALSE;
    else
        /* mount device as read only */
        dev->readOnly = TRUE;

    dev->size = 0;

    return RC_OK;
}


/*
 * myReadSector
 *
 */
RETCODE myReadSector ( struct AdfDevice * const dev,
                       const uint32_t           n,
                       const unsigned           size,
                       uint8_t * const          buf )
{
    (void) dev, (void) n, (void) size, (void) buf;
    return RC_OK;
}


/*
 * myWriteSector
 *
 */
RETCODE myWriteSector ( struct AdfDevice * const dev,
                        const uint32_t           n,
                        const unsigned           size,
                        const uint8_t * const    buf )
{
    (void) dev, (void) n, (void) size, (void) buf;
    return RC_OK;
}


/*
 * myReleaseDevice
 *
 * free native device
 */
RETCODE myReleaseDevice ( struct AdfDevice * const dev )
{
    struct AdfNativeDevice * nDev;

    nDev = (struct AdfNativeDevice *) dev->nativeDev;

	free(nDev);

    return RC_OK;
}


/*
 * adfInitNativeFct
 *
 */
void adfInitNativeFct()
{
    struct AdfNativeFunctions * nFct =
        ( struct AdfNativeFunctions * ) adfEnv.nativeFct;

    nFct->adfInitDevice = myInitDevice ;
    nFct->adfNativeReadSector = myReadSector ;
    nFct->adfNativeWriteSector = myWriteSector ;
    nFct->adfReleaseDevice = myReleaseDevice ;
    nFct->adfIsDevNative = myIsDevNative;
}


/*
 * myIsDevNative
 *
 */
BOOL myIsDevNative ( const char * const devName )
{
    return (strncmp(devName,"/dev/",5)==0);
}
/*##########################################################################*/
