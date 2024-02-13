/*
 * ADF Library
 *
 * adf_dev_driver_dump.c
 *
 *  $Id$
 *
 * Amiga Dump File specific routines
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

#include "adf_dev_driver_dump.h"

#include "adf_blk.h"
#include "adf_env.h"
#include "adf_err.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct DevDumpData {
    FILE * fd;
};


/*
 * adfDevDumpOpen
 *
 */
static struct AdfDevice * adfDevDumpOpen ( const char * const  name,
                                           const AdfAccessMode mode )
{
    struct AdfDevice * dev = ( struct AdfDevice * )
        malloc ( sizeof ( struct AdfDevice ) );
    if ( dev == NULL ) {
        adfEnv.eFct ( "adfDevDumpOpen : malloc error" );
        return NULL;
    }

    dev->readOnly = ( mode != ADF_ACCESS_MODE_READWRITE );

    dev->drvData = malloc ( sizeof ( struct DevDumpData ) );
    if ( dev->drvData == NULL ) {
        adfEnv.eFct("adfDevDumpOpen : malloc data error");
        free ( dev );
        return NULL;
    }

    FILE ** const fd = &( ( (struct DevDumpData *) dev->drvData )->fd );

    errno = 0;
    if ( ! dev->readOnly ) {
        *fd = fopen ( name, "rb+" );
        /* force read only */
        if ( *fd == NULL && ( errno == EACCES || errno == EROFS ) ) {
            *fd = fopen ( name, "rb" );
            dev->readOnly = TRUE;
            if ( *fd != NULL )
                adfEnv.wFct("adfDevDumpOpen : fopen, read-only mode forced");
        }
    }
    else
        /* read only requested */
        *fd = fopen ( name, "rb" );

    if ( *fd == NULL ) {
        adfEnv.eFct ( "adfDevDumpOpen : fopen" );
        free ( dev->drvData );
        free ( dev );
        return NULL;
    }

    /* determines size */
    fseek ( *fd, 0, SEEK_END );
    dev->size = (uint32_t) ftell ( *fd );
    fseek ( *fd, 0, SEEK_SET );

    dev->devType = adfDevType ( dev );

    dev->nVol    = 0;
    dev->volList = NULL;
    dev->mounted = FALSE;

    dev->drv = &adfDeviceDriverDump;

    dev->name = strdup ( name );

    return dev;
}


/*
 * adfReadDumpSector
 *
 */
static RETCODE adfReadDumpSector ( struct AdfDevice * const dev,
                                   const uint32_t           n,
                                   const unsigned           size,
                                   uint8_t * const          buf )
{
/*puts("adfReadDumpSector");*/
    FILE * const fd = ( (struct DevDumpData *) dev->drvData )->fd;
    int pos = fseek ( fd, 512 * n, SEEK_SET );
/*printf("nnn=%ld size=%d\n",n,size);*/
    if ( pos == -1 )
        return RC_ERROR;

    size_t bytes_read = fread ( buf, 1, size, fd );
/*puts("123");*/
    if ( bytes_read != size ) {
/*printf("rr=%d\n",r);*/
        return RC_ERROR;
    }
/*puts("1234");*/

    return RC_OK;
}


/*
 * adfWriteDumpSector
 *
 */
static RETCODE adfWriteDumpSector ( struct AdfDevice * const dev,
                                    const uint32_t           n,
                                    const unsigned           size,
                                    const uint8_t * const    buf )
{
    FILE * const fd = ( (struct DevDumpData *) dev->drvData )->fd;
    int r = fseek ( fd, 512 * n, SEEK_SET );
    if (r==-1)
        return RC_ERROR;

    if ( fwrite ( buf, 1, size, fd ) != (unsigned int) size )
        return RC_ERROR;
/*puts("adfWriteDumpSector");*/
    return RC_OK;
}


/*
 * adfReleaseDumpDevice
 *
 */
static RETCODE adfReleaseDumpDevice ( struct AdfDevice * const dev )
{
    fclose ( ( (struct DevDumpData *) dev->drvData )->fd );

    if ( dev->mounted )
        adfDevUnMount ( dev );

    free ( dev->drvData );
    free ( dev->name );
    free ( dev );

    return RC_OK;
}



/*
 * adfCreateDumpDevice
 *
 * returns NULL if failed
 */ 
static struct AdfDevice * adfCreateDumpDevice ( const char * const filename,
                                                const uint32_t     cylinders,
                                                const uint32_t     heads,
                                                const uint32_t     sectors )
{
    uint8_t buf[LOGICAL_BLOCK_SIZE];
/*    int32_t i;*/
    int r;
	
    struct AdfDevice * dev = (struct AdfDevice *)
        malloc (sizeof(struct AdfDevice));
    if (!dev) { 
        (*adfEnv.eFct)("adfCreateDumpDevice : malloc dev");
        return NULL;
    }

    dev->drvData = malloc ( sizeof ( struct DevDumpData ) );
    if ( dev->drvData == NULL ) {
        adfEnv.eFct("adfDevDumpOpen : malloc data error");
        free ( dev );
        return NULL;
    }

    FILE ** fd = &( ( (struct DevDumpData *) dev->drvData )->fd );

    *fd = fopen ( filename, "wb" );
    if ( *fd == NULL ) {
        free ( dev->drvData );
        free ( dev );
        (*adfEnv.eFct)("adfCreateDumpDevice : fopen");
        return NULL;
    }

/*    for(i=0; i<cylinders*heads*sectors; i++)
        fwrite(buf, sizeof(uint8_t), 512 , nDev->fd);
*/
    long lastBlockOffset = ( ( cylinders * heads * sectors ) - 1 ) *
        LOGICAL_BLOCK_SIZE;
    r = fseek ( *fd, lastBlockOffset, SEEK_SET );
    if (r==-1) {
        fclose ( *fd );
        free ( dev->drvData );
        free ( dev );
        (*adfEnv.eFct)("adfCreateDumpDevice : fseek");
        return NULL;
    }

    memset ( buf, 0, LOGICAL_BLOCK_SIZE );
    size_t blocksWritten = fwrite ( buf, LOGICAL_BLOCK_SIZE, 1, *fd );
    if ( blocksWritten != 1 ) {
        fclose ( *fd );
        free ( dev->drvData );
        free ( dev );
        (*adfEnv.eFct)("adfCreateDumpDevice : fwrite");
        return NULL;
    }

    fclose ( *fd );

    *fd = fopen ( filename, "rb+" );
    if ( ! *fd ) {
        free ( dev->drvData );
        free ( dev );
        (*adfEnv.eFct)("adfCreateDumpDevice : fopen");
        return NULL;
    }
    dev->cylinders = cylinders;
    dev->heads = heads;
    dev->sectors = sectors;
    dev->size = cylinders*heads*sectors* LOGICAL_BLOCK_SIZE;	

    if (dev->size==80*11*2*LOGICAL_BLOCK_SIZE)
        dev->devType = DEVTYPE_FLOPDD;
    else if (dev->size==80*22*2*LOGICAL_BLOCK_SIZE)
        dev->devType = DEVTYPE_FLOPHD;
	else 	
        dev->devType = DEVTYPE_HARDDISK;
		
    dev->nVol = 0;
    dev->readOnly = FALSE;
    dev->mounted = FALSE;

    dev->drv = &adfDeviceDriverDump;

    dev->name = strdup ( filename );

    return(dev);
}

static BOOL adfDevDumpIsNativeDevice ( void )
{
    return FALSE;
}

const struct AdfDeviceDriver adfDeviceDriverDump = {
    .name        = "dump",
    .data        = NULL,
    .createDev   = adfCreateDumpDevice,
    .openDev     = adfDevDumpOpen,       // adfOpenDev + adfInitDumpDevice
    .closeDev    = adfReleaseDumpDevice,
    .readSector  = adfReadDumpSector,
    .writeSector = adfWriteDumpSector,
    .isNative    = adfDevDumpIsNativeDevice,
    .isDevice    = NULL
};

/*##################################################################################*/
