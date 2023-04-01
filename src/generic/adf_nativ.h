/*
 * adf_nativ.h
 *
 * $ID$
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

#ifndef ADF_NATIV_H
#define ADF_NATIV_H

#include<stdio.h>
#include"adf_dev.h"

#define NATIVE_FILE  8001

#ifndef BOOL
#define BOOL int
#endif

#ifndef RETCODE
#define RETCODE int32_t
#endif

struct AdfNativeDevice {
    FILE* fd;
};

struct AdfNativeFunctions {

    /* called by adfMount() */
    RETCODE (*adfInitDevice)( struct AdfDevice * const dev,
                              const char * const       name,
                              const BOOL               ro );

    /* called by adfUnMount() */
    RETCODE (*adfReleaseDevice)(struct AdfDevice * const dev);

    /* called by adfReadBlock() */
    RETCODE (*adfNativeReadSector)( struct AdfDevice * const dev,
                                    const uint32_t           n,
                                    const unsigned           size,
                                    uint8_t * const          buf );

    /* called by adfWriteBlock() */
    RETCODE (*adfNativeWriteSector)( struct AdfDevice * const dev,
                                     const uint32_t           n,
                                     const unsigned           size,
                                     const uint8_t * const    buf );

    /* called by adfMount() */
    BOOL (*adfIsDevNative)( const char * const devName );
};

void adfInitNativeFct();

RETCODE myInitDevice ( struct AdfDevice * const dev,
                       const char * const       name,
                       const BOOL               ro );

RETCODE myReleaseDevice ( struct AdfDevice * const dev );

RETCODE myReadSector ( struct AdfDevice * const dev,
                       const uint32_t           n,
                       const unsigned           size,
                       uint8_t * const          buf );

RETCODE myWriteSector ( struct AdfDevice * const dev,
                        const uint32_t           n,
                        const unsigned           size,
                        const uint8_t * const    buf );

BOOL myIsDevNative ( const char * const );

#endif /* ADF_NATIV_H */

/*#######################################################################################*/
