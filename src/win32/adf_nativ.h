/* Win32/adf_nativ.h
 *
 * Win32 specific drive access routines for ADFLib
 * Copyright 1999 by Dan Sutherland <dan@chromerhino.demon.co.uk>
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
#define RETCODE long
#endif

struct AdfNativeDevice {
	void *hDrv;
};

struct AdfNativeFunctions {
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

void adfInitNativeFct();

RETCODE Win32InitDevice ( struct AdfDevice * const dev,
                          const char * const       name,
                          const BOOL               ro );

RETCODE Win32ReleaseDevice ( struct AdfDevice * const dev );

RETCODE Win32ReadSector ( struct AdfDevice * const dev,
                          const uint32_t           n,
                          const unsigned           size,
                          uint8_t * const          buf );

RETCODE Win32WriteSector ( struct AdfDevice * const dev,
                           const uint32_t           n,
                           const unsigned           size,
                           const uint8_t * const    buf );

BOOL Win32IsDevNative ( const char * const devName );

#endif /* ndef ADF_NATIV_H */
