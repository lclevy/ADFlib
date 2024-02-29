/* Win32/adf_dev_driver_nativ.c - Win32 specific drive-access routines for ADFLib
 *
 * Modified for Win32 by Dan Sutherland <dan@chromerhino.demon.co.uk>
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

#include <windows.h>
#include <stdlib.h>
#include <string.h>

#include "adf_str.h"
#include "adf_err.h"
#include "adf_dev_driver_nativ.h"
#include "adf_env.h"
#include "nt4_dev.h"


struct AdfNativeDevice {
       void *hDrv;
};

static bool Win32IsDevice ( const char * const devName );
static ADF_RETCODE Win32ReleaseDevice ( struct AdfDevice * const dev );


static struct AdfDevice * Win32InitDevice ( const char * const  lpstrName,
                                            const AdfAccessMode mode )
{
	if ( ! Win32IsDevice ( lpstrName ) ) {
		return NULL;
	}

	struct AdfDevice * dev = malloc ( sizeof ( struct AdfDevice ) );
	if ( dev == NULL ) {
		adfEnv.eFct ( "Win32InitDevice : malloc error" );
		return NULL;
	}

	dev->readOnly = ( mode != ADF_ACCESS_MODE_READWRITE );

	dev->drvData = malloc ( sizeof ( struct AdfNativeDevice ) );
	if ( dev->drvData == NULL ) {
		adfEnv.eFct("Win32InitDevice : malloc data error");
		free ( dev );
		return NULL;
	}

	/* convert device name to something usable by Win32 functions */
	if (strlen(lpstrName) != 3) {
		(*adfEnv.eFct)("Win32InitDevice : invalid drive specifier");
		free ( dev->drvData );
		free ( dev );
		return NULL;
	}

	char strTempName[3];
	strTempName[0] = lpstrName[1];
	strTempName[1] = lpstrName[2];
	strTempName[2] = '\0';

	void ** const hDrv = &( ( (struct AdfNativeDevice *) dev->drvData )->hDrv );
	*hDrv = NT4OpenDrive ( strTempName );
	if ( *hDrv == NULL ) {
		(*adfEnv.eFct)("Win32InitDevice : NT4OpenDrive");
		free ( dev->drvData );
		free ( dev );
		return NULL;
	}

        NT4DriveGeometry_t geometry;
        if ( ! NT4GetDriveGeometry ( *hDrv, &geometry ) ) {
            (*adfEnv.eFct)("Win32InitDevice : error getting drive geometry");
            Win32ReleaseDevice ( dev );
            return NULL;
        }

        // no support for disks with non 512-byte sectors (-> to improve?)
        if ( geometry.bytesPerSector != 512 ) {
            (*adfEnv.eFct)("Win32InitDevice : non 512-byte sector size");
            Win32ReleaseDevice ( dev );
            return NULL;
        }

        dev->cylinders = geometry.cylinders;
        dev->heads     = geometry.tracksPerCylinder;
        dev->sectors   = geometry.sectorsPerTrack;

	//dev->size = NT4GetDriveSize(nDev->hDrv);
        dev->size = geometry.cylinders *
                    geometry.tracksPerCylinder *
                    geometry.sectorsPerTrack *
                    geometry.bytesPerSector;

	dev->devType = adfDevType ( dev );
	dev->nVol    = 0;
	dev->volList = NULL;
	dev->mounted = false;
	dev->name    = strdup ( lpstrName );
	dev->drv     = &adfDeviceDriverNative;

	return dev;
}


static ADF_RETCODE Win32ReadSector ( struct AdfDevice * const dev,
                                     const uint32_t           n,
                                     const unsigned           size,
                                     uint8_t * const          buf )
{
	void ** const hDrv = &( ( (struct AdfNativeDevice *) dev->drvData )->hDrv );

	if ( ! NT4ReadSector( *hDrv, (long) n, size, buf ) ) {
		(*adfEnv.eFct)("Win32InitDevice : NT4ReadSector");
		return ADF_RC_ERROR;													/* BV */
	}

	return ADF_RC_OK;
}


static ADF_RETCODE Win32WriteSector ( struct AdfDevice * const dev,
                                      const uint32_t           n,
                                      const unsigned           size,
                                      const uint8_t * const    buf )
{
	void ** const hDrv = &( ( (struct AdfNativeDevice *) dev->drvData )->hDrv );

	if ( ! NT4WriteSector ( *hDrv, (long) n, size, buf) ) {
		(*adfEnv.eFct)("Win32InitDevice : NT4WriteSector");
		return ADF_RC_ERROR;													/* BV */
	}

	return ADF_RC_OK;
}


static ADF_RETCODE Win32ReleaseDevice ( struct AdfDevice * const dev )
{
	void ** const hDrv = &( ( (struct AdfNativeDevice *) dev->drvData )->hDrv );

	if ( ! NT4CloseDrive ( *hDrv ) )
		return ADF_RC_ERROR;													/* BV */
	free ( dev->name );
	free ( dev->drvData );
	free ( dev );

	return ADF_RC_OK;
}

static bool Win32IsDevNative ( void )
{
    return true;
}


static bool Win32IsDevice ( const char * const devName )
{
        return devName[0] == '|';
}


const struct AdfDeviceDriver adfDeviceDriverNative = {
    .name        = "native win32",
    .data        = NULL,
    .createDev   = NULL,
    .openDev     = Win32InitDevice,
    .closeDev    = Win32ReleaseDevice,
    .readSector  = Win32ReadSector,
    .writeSector = Win32WriteSector,
    .isNative    = Win32IsDevNative,
    .isDevice    = Win32IsDevice
};
