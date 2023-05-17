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

#include "adf_dev.h"

#define NATIVE_FILE  8001

#ifndef BOOL
#define BOOL int
#endif

#ifndef RETCODE
#define RETCODE int32_t
#endif

struct AdfNativeDevice {
    //FILE * fd;
    int fd;
};

struct AdfNativeFunctions {
    struct AdfNativeFunctions *next;

    RETCODE (*adfInitDevice)( struct AdfDevice * const dev,
                              const char * const       name,
                              const BOOL               ro );

    RETCODE (*adfReleaseDevice)( struct AdfDevice * const dev );

    RETCODE (*adfNativeReadSector)( struct AdfDevice * const dev,
                                    const uint32_t           n,
                                    const unsigned           size,
                                    uint8_t * const          buf );

    RETCODE (*adfNativeWriteSector)( struct AdfDevice * const dev,
                                     const uint32_t           n,
                                     const unsigned           size,
                                     const uint8_t * const    buf );

    BOOL (*adfIsDevNative)( const char * const devName );
};

struct AdfNativeFunctions *adfInitNativeFct();

RETCODE adfLinuxInitDevice ( struct AdfDevice * const dev,
                             const char * const       name,
                             const BOOL               ro );

RETCODE adfLinuxReleaseDevice ( struct AdfDevice * const dev );

RETCODE adfLinuxReadSector ( struct AdfDevice * const dev,
                             const uint32_t           n,
                             const unsigned           size,
                             uint8_t * const          buf );

RETCODE adfLinuxWriteSector ( struct AdfDevice * const dev,
                              const uint32_t           n,
                              const unsigned           size,
                              const uint8_t * const    buf );

BOOL adfLinuxIsDevNative ( const char * const devName );

#endif /* ADF_NATIV_H */
