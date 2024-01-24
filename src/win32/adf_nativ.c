/* Win32/adf_nativ.c - Win32 specific drive-access routines for ADFLib
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


/* Modified 29/8/00 by Gary Harris.
** - Added a third, Boolean argument to Win32InitDevice() to avoid a compilation warning
**   caused by the mismatch with the number of arguments in ADFLib's adfInitDevice().
*/

#include <windows.h>
#include <stdlib.h>
#include <string.h>

#include "adf_str.h"
#include "adf_err.h"

#include "adf_nativ.h"
#include "adf_env.h"
#include "nt4_dev.h"


struct AdfNativeDevice {
       void *hDrv;
};


static RETCODE Win32ReleaseDevice ( struct AdfDevice * const dev );


static RETCODE Win32InitDevice ( struct AdfDevice * const dev,
                                 const char * const       lpstrName,
                                 const AdfAccessMode      mode )
{
	struct AdfNativeDevice * nDev = ( struct AdfNativeDevice * )
            malloc ( sizeof(struct AdfNativeDevice) );
	if (!nDev) {
		(*adfEnv.eFct)("Win32InitDevice : malloc");
		return RC_ERROR;													/* BV */
	}

	/* convert device name to something usable by Win32 functions */
	if (strlen(lpstrName) != 3) {
		(*adfEnv.eFct)("Win32InitDevice : invalid drive specifier");
		return RC_ERROR;													/* BV */
	}

	char strTempName[3];
	strTempName[0] = lpstrName[1];
	strTempName[1] = lpstrName[2];
	strTempName[2] = '\0';

	nDev->hDrv = NT4OpenDrive(strTempName);

	if (nDev->hDrv == NULL) {
		(*adfEnv.eFct)("Win32InitDevice : NT4OpenDrive");
		return RC_ERROR;													/* BV */
	}

        NT4DriveGeometry_t geometry;
        if ( ! NT4GetDriveGeometry ( nDev->hDrv, &geometry ) ) {
            (*adfEnv.eFct)("Win32InitDevice : error getting drive geometry");
            Win32ReleaseDevice ( dev );
            return RC_ERROR;
        }

        // no support for disks with non 512-byte sectors (-> to improve?)
        if ( geometry.bytesPerSector != 512 ) {
            (*adfEnv.eFct)("Win32InitDevice : non 512-byte sector size");
            Win32ReleaseDevice ( dev );
            return RC_ERROR;
        }

        dev->cylinders = geometry.cylinders;
        dev->heads     = geometry.tracksPerCylinder;
        dev->sectors   = geometry.sectorsPerTrack;

	//dev->size = NT4GetDriveSize(nDev->hDrv);
        dev->size = geometry.cylinders *
                    geometry.tracksPerCylinder *
                    geometry.sectorsPerTrack *
                    geometry.bytesPerSector;

	dev->nativeDev = nDev;

	return RC_OK;
}


static RETCODE Win32ReadSector ( struct AdfDevice * const dev,
                                 const uint32_t           n,
                                 const unsigned           size,
                                 uint8_t * const          buf )
{
	struct AdfNativeDevice * tDev =
            ( struct AdfNativeDevice * ) dev->nativeDev;

	if (! NT4ReadSector(tDev->hDrv, (long) n, size, buf)) {
		(*adfEnv.eFct)("Win32InitDevice : NT4ReadSector");
		return RC_ERROR;													/* BV */
	}

	return RC_OK;
}


static RETCODE Win32WriteSector ( struct AdfDevice * const dev,
                                  const uint32_t           n,
                                  const unsigned           size,
                                  const uint8_t * const    buf )
{
	struct AdfNativeDevice * tDev =
            ( struct AdfNativeDevice * ) dev->nativeDev;

	if (! NT4WriteSector(tDev->hDrv, (long) n, size, buf)) {
		(*adfEnv.eFct)("Win32InitDevice : NT4WriteSector");
		return RC_ERROR;													/* BV */
	}

	return RC_OK;
}


static RETCODE Win32ReleaseDevice ( struct AdfDevice * const dev )
{
	struct AdfNativeDevice * nDev =
            ( struct AdfNativeDevice * ) dev->nativeDev;

	if (! NT4CloseDrive(nDev->hDrv))
		return RC_ERROR;													/* BV */

	free(nDev);

	return RC_OK;
}


static BOOL Win32IsDevNative ( const char * const devName )
{
        return devName[0] == '|';
}


struct AdfNativeFunctions adfWindowsNativeDevice = {
    NULL,
    &Win32InitDevice,
    &Win32ReleaseDevice,
    &Win32ReadSector,
    &Win32WriteSector,
    &Win32IsDevNative
};


struct AdfNativeFunctions *adfInitNativeFct() {
    return &adfWindowsNativeDevice;
}
