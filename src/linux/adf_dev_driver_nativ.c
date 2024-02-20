/*
 * linux/adf_dev_driver_nativ.c
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

#include <fcntl.h>
//#include <libgen.h>
#include <linux/fs.h>
#include <linux/hdreg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "adf_err.h"
#include "adf_dev_driver_nativ.h"
#include "adf_env.h"


struct AdfNativeDevice {
    int fd;
};

static BOOL adfLinuxIsBlockDevice ( const char * const devName );

/*
 * adfLinuxInitDevice
 *
 */
static struct AdfDevice * adfLinuxInitDevice ( const char * const  name,
                                               const AdfAccessMode mode )
{
    if ( ! adfLinuxIsBlockDevice ( name ) ) {
        return NULL;
    }

    struct AdfDevice * dev = ( struct AdfDevice * )
        malloc ( sizeof ( struct AdfDevice ) );
    if ( dev == NULL ) {
        (*adfEnv.eFct)("adfLinuxInitDevice : malloc error");
        return NULL;
    }

    dev->readOnly = ( mode != ADF_ACCESS_MODE_READWRITE );

    dev->drvData = malloc ( sizeof ( struct AdfNativeDevice ) );
    if ( dev->drvData == NULL ) {
        adfEnv.eFct("adfLinuxInitDevice : malloc data error");
        free ( dev );
        return NULL;
    }

    int * const fd = &( ( (struct AdfNativeDevice *) dev->drvData )->fd );

    if ( ! dev->readOnly ) {
        *fd = open ( name, O_RDWR );
        dev->readOnly = ( *fd < 0 ) ? TRUE : FALSE;
    }

    if ( dev->readOnly ) {
        *fd = open ( name, O_RDONLY );
    }

    if ( *fd < 0 ) {
        (*adfEnv.eFct)("adfLinuxInitDevice : cannot open device");
        free ( dev->drvData );
        free ( dev );
        return NULL;
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
    if ( ioctl ( *fd, BLKGETSIZE64, &size ) < 0 ) {
        // fall-back to lseek
        size = ( unsigned long long ) lseek ( *fd, 0, SEEK_END );
        lseek ( *fd, 0, SEEK_SET );
    }

    dev->size = (uint32_t) size;
    
    // https://docs.kernel.org/userspace-api/ioctl/hdio.html
    struct hd_geometry geom;
    if ( ioctl ( *fd, HDIO_GETGEO, &geom ) == 0 ) {
        dev->heads     = geom.heads;
        dev->sectors   = geom.sectors;
        dev->cylinders = geom.cylinders;
    } else {
        // guessing: is whatever matches the size OK?
        dev->heads     = 2;
        dev->sectors   = dev->size / 4;
        dev->cylinders = dev->size / ( dev->sectors * dev->heads * 512 );
    }

    dev->devType = adfDevType ( dev );
    dev->nVol    = 0;
    dev->volList = NULL;
    dev->mounted = FALSE;
    dev->name    = strdup ( name );
    dev->drv     = &adfDeviceDriverNative;

    return RC_OK;
}


/*
 * adfLinuxReleaseDevice
 *
 * free native device
 */
static RETCODE adfLinuxReleaseDevice ( struct AdfDevice * const dev )
{
    close ( ( (struct AdfNativeDevice *) dev->drvData )->fd );
    free ( dev->drvData );
    free ( dev->name );
    free ( dev );
    return RC_OK;
}


/*
 * adfLinuxReadSector
 *
 */
static RETCODE adfLinuxReadSector ( struct AdfDevice * const dev,
                                    const uint32_t           n,
                                    const unsigned           size,
                                    uint8_t * const          buf )
{
    const int fd = ( (struct AdfNativeDevice *) dev->drvData )->fd;

    off_t offset = n * 512;
    if ( lseek ( fd, offset, SEEK_SET ) != offset ) {
        return RC_ERROR;
    }

    if ( read ( fd, buf, (size_t) size ) != (ssize_t) size )
        return RC_ERROR;

    return RC_OK;   
}


/*
 * adfLinuxWriteSector
 *
 */
static RETCODE adfLinuxWriteSector ( struct AdfDevice * const dev,
                                     const uint32_t           n,
                                     const unsigned           size,
                                     const uint8_t * const    buf )
{
    const int fd = ( (struct AdfNativeDevice *) dev->drvData )->fd;

    off_t offset = n * 512;
    if ( lseek ( fd, offset, SEEK_SET ) != offset ) {
        return RC_ERROR;
    }

    if ( write ( fd, (void *) buf, (size_t) size ) != size )
        return RC_ERROR;

    return RC_OK;
}


/*
 * adfLinuxIsDevNative
 *
 */
static BOOL adfLinuxIsDevNative ( void )
{
    return TRUE;
}


/*
 * adfLinuxIsBlockDevice
 *
 */
static BOOL adfLinuxIsBlockDevice ( const char * const devName )
{
    //return ( strncmp ( devName, "/dev/", 5 ) == 0 );

    struct stat sb;
    if ( lstat ( devName, &sb ) == -1 ) {
        adfEnv.eFct ( "adfLinuxIsBlockDevice : lstat '%s' failed", devName );
        return FALSE;
    }

    return ( ( sb.st_mode & S_IFMT ) == S_IFBLK );
}


const struct AdfDeviceDriver adfDeviceDriverNative = {
    .name        = "native linux",
    .data        = NULL,
    .createDev   = NULL,
    .openDev     = adfLinuxInitDevice,
    .closeDev    = adfLinuxReleaseDevice,
    .readSector  = adfLinuxReadSector,
    .writeSector = adfLinuxWriteSector,
    .isNative    = adfLinuxIsDevNative,
    .isDevice    = adfLinuxIsBlockDevice
};
