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

#include <fcntl.h>
//#include <libgen.h>
#include <linux/fs.h>
#include <linux/hdreg.h>
#include <stdlib.h>
//#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "adf_err.h"
#include "adf_nativ.h"
#include "adf_env.h"


/*
 * adfLinuxInitDevice
 *
 * must fill 'dev->size'
 */
RETCODE adfLinuxInitDevice ( struct AdfDevice * const dev,
                             const char * const       name,
                             const BOOL               ro )
{
    struct AdfNativeDevice * nDev = ( struct AdfNativeDevice * )
        malloc ( sizeof ( struct AdfNativeDevice ) );

    if ( ! nDev ) {
        (*adfEnv.eFct)("adfLinuxInitDevice : malloc");
        return RC_ERROR;
    }

    dev->nativeDev = nDev;

    dev->readOnly = ro;
    if ( ! dev->readOnly ) {
        nDev->fd = open ( name, O_RDWR );
        dev->readOnly = ( nDev->fd < 0 ) ? TRUE : FALSE;
    }

    if ( dev->readOnly ) {
        nDev->fd = open ( name, O_RDONLY );
    }

    if ( nDev->fd < 0 ) {
        (*adfEnv.eFct)("adfLinuxInitDevice : cannot open device");
        free ( nDev );
        return RC_ERROR;
    }

    unsigned long long size = 0;

    /*
    unsigned long blocks = 0;
    if ( ioctl ( nDev->fd, BLKGETSIZE, &blocks ) < 0 ) {
        // fall-back to lseek
        size = ( unsigned long ) lseek ( nDev->fd, 0, SEEK_END );
        lseek ( nDev->fd, 0, SEEK_SET );
    } else {
        size = blocks * 512;
    }
    */
    if ( ioctl ( nDev->fd, BLKGETSIZE64, &size ) < 0 ) {
        // fall-back to lseek
        size = ( unsigned long long ) lseek ( nDev->fd, 0, SEEK_END );
        lseek ( nDev->fd, 0, SEEK_SET );
    }

    dev->size = (int) size;
    
    // https://docs.kernel.org/userspace-api/ioctl/hdio.html
    struct hd_geometry geom;
    if ( ioctl ( nDev->fd, HDIO_GETGEO, &geom ) == 0 ) {
        dev->heads     = geom.heads;
        dev->sectors   = geom.sectors;
        dev->cylinders = geom.cylinders;
    } else {
        // guessing: is whatever matches the size OK?
        dev->heads     = 2;
        dev->sectors   = dev->size / 4;
        dev->cylinders = dev->size / ( dev->sectors * dev->heads * 512 );
    }
    return RC_OK;
}


/*
 * adfLinuxReleaseDevice
 *
 * free native device
 */
RETCODE adfLinuxReleaseDevice ( struct AdfDevice * const dev )
{
    struct AdfNativeDevice * nDev = ( struct AdfNativeDevice * ) dev->nativeDev;
    close ( nDev->fd );
    free ( nDev );
    return RC_OK;
}


/*
 * adfLinuxReadSector
 *
 */
RETCODE adfLinuxReadSector ( struct AdfDevice * const dev,
                             const int32_t            n,
                             const int                size,
                             uint8_t * const          buf )
{
    struct AdfNativeDevice * nDev = ( struct AdfNativeDevice * ) dev->nativeDev;

    off_t offset = n * 512;
    if ( lseek ( nDev->fd, offset, SEEK_SET ) != offset ) {
        return RC_ERROR;
    }

    if ( write ( nDev->fd, buf, (size_t) size ) != (ssize_t) size )
        return RC_ERROR;

    return RC_OK;   
}


/*
 * adfLinuxWriteSector
 *
 */
RETCODE adfLinuxWriteSector ( struct AdfDevice * const dev,
                              const int32_t            n,
                              const int                size,
                              const uint8_t * const    buf )
{
    struct AdfNativeDevice * nDev = ( struct AdfNativeDevice * ) dev->nativeDev;

    off_t offset = n * 512;
    if ( lseek ( nDev->fd, offset, SEEK_SET ) != offset ) {
        return RC_ERROR;
    }

    if ( read ( nDev->fd, (void *) buf, (size_t) size ) != size )
        return RC_ERROR;

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

    nFct->adfInitDevice        = adfLinuxInitDevice;
    nFct->adfNativeReadSector  = adfLinuxReadSector;
    nFct->adfNativeWriteSector = adfLinuxWriteSector;
    nFct->adfReleaseDevice     = adfLinuxReleaseDevice;
    nFct->adfIsDevNative       = adfLinuxIsDevNative;
}


/*
 * adfLinuxIsDevNative
 *
 */
BOOL adfLinuxIsDevNative ( const char * const devName )
{
    //return ( strncmp ( devName, "/dev/", 5 ) == 0 );

    struct stat sb;
    if ( lstat ( devName, &sb ) == -1 ) {
        perror ( "adfLinuxIsDevNative: lstat" );
        exit ( EXIT_FAILURE );
    }

    return ( ( sb.st_mode & S_IFMT ) == S_IFBLK );
}
