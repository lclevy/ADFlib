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
                       const AdfAccessMode      mode )
{
    (void) dev, (void) name, (void) mode;
    return RC_ERROR;
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
    return RC_ERROR;
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
    return RC_ERROR;
}


/*
 * myReleaseDevice
 *
 * free native device
 */
RETCODE myReleaseDevice ( struct AdfDevice * const dev )
{
    (void) dev;
    return RC_ERROR;
}


/*
 * myIsDevNative
 *
 */
BOOL myIsDevNative ( const char * const devName )
{
    return FALSE;
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

/*##########################################################################*/
