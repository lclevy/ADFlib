/*
 * ADF Library
 *
 * adf_dump.c
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

#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include<errno.h>

#include "adf_blk.h"
#include"adf_defs.h"
#include"adf_str.h"
#include "adf_vol.h"
#include"adf_nativ.h"
#include"adf_err.h"
#include"adf_dump.h"
#include "adf_env.h"


/*
 * adfInitDumpDevice
 *
 */
RETCODE adfInitDumpDevice ( struct AdfDevice * const dev,
                            const char * const       name,
                            const BOOL               ro )
{
    dev->readOnly = ro;
    errno = 0;
    if (!ro) {
        dev->fd = fopen ( name, "rb+" );
        /* force read only */
        if ( ! dev->fd && ( errno == EACCES || errno == EROFS ) ) {
            dev->fd = fopen ( name, "rb" );
            dev->readOnly = TRUE;
            if ( dev->fd )
                (*adfEnv.wFct)("myInitDevice : fopen, read-only mode forced");
        }
    }
    else
        /* read only requested */
        dev->fd = fopen ( name, "rb" );

    if ( ! dev->fd ) {
        (*adfEnv.eFct)("myInitDevice : fopen");
        return RC_ERROR;
    }

    /* determines size */
    fseek ( dev->fd, 0, SEEK_END );
    dev->size = ftell ( dev->fd );
    fseek ( dev->fd, 0, SEEK_SET );

    return RC_OK;
}


/*
 * adfReadDumpSector
 *
 */
RETCODE adfReadDumpSector ( struct AdfDevice * const dev,
                            const uint32_t           n,
                            const unsigned           size,
                            uint8_t * const          buf )
{
    int r;
/*puts("adfReadDumpSector");*/
    r = fseek ( dev->fd, 512 * n, SEEK_SET );
/*printf("nnn=%ld size=%d\n",n,size);*/
    if (r==-1)
        return RC_ERROR;
/*puts("123");*/
    if ( ( r = fread ( buf, 1, size, dev->fd ) ) != size ) {
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
RETCODE adfWriteDumpSector ( struct AdfDevice * const dev,
                             const uint32_t           n,
                             const unsigned           size,
                             const uint8_t * const    buf )
{
    int r = fseek ( dev->fd, 512 * n, SEEK_SET );
    if (r==-1)
        return RC_ERROR;

    if ( fwrite ( buf, 1, size, dev->fd ) != (unsigned int) size )
        return RC_ERROR;
/*puts("adfWriteDumpSector");*/
    return RC_OK;
}


/*
 * adfReleaseDumpDevice
 *
 */
RETCODE adfReleaseDumpDevice ( struct AdfDevice * dev )
{
    fclose ( dev->fd );

    //free ( dev );  // this is done externally - maybe should be moved here?

    return RC_OK;
}


/*
 * adfCreateHdFile
 *
 */
RETCODE adfCreateHdFile ( struct AdfDevice * const dev,
                          const char * const       volName,
                          const uint8_t            volType )
{
    if (dev==NULL) {
        (*adfEnv.eFct)("adfCreateHdFile : dev==NULL");
        return RC_ERROR;
    }
    dev->volList = (struct AdfVolume **) malloc (sizeof(struct Volume *));
    if (!dev->volList) { 
                (*adfEnv.eFct)("adfCreateHdFile : unknown device type");
        return RC_ERROR;
    }

    dev->volList[0] = adfCreateVol( dev, 0L, (int32_t)dev->cylinders, volName, volType );
    if (dev->volList[0]==NULL) {
        free(dev->volList);
        return RC_ERROR;
    }

    dev->nVol = 1;
    dev->devType = DEVTYPE_HARDFILE;

    return RC_OK;
}


/*
 * adfCreateDumpDevice
 *
 * returns NULL if failed
 */ 
struct AdfDevice * adfCreateDumpDevice ( const char * const filename,
                                         const int32_t      cylinders,
                                         const int32_t      heads,
                                         const int32_t      sectors )
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

    dev->fd = fopen ( filename, "wb" );
    if ( ! dev->fd ) {
        free ( dev );
        (*adfEnv.eFct)("adfCreateDumpDevice : fopen");
        return NULL;
    }

/*    for(i=0; i<cylinders*heads*sectors; i++)
        fwrite(buf, sizeof(uint8_t), 512 , nDev->fd);
*/
    long lastBlockOffset = ( ( cylinders * heads * sectors ) - 1 ) *
        LOGICAL_BLOCK_SIZE;
    r = fseek ( dev->fd, lastBlockOffset, SEEK_SET );
    if (r==-1) {
        fclose ( dev->fd );
        free ( dev );
        (*adfEnv.eFct)("adfCreateDumpDevice : fseek");
        return NULL;
    }

    memset ( buf, 0, LOGICAL_BLOCK_SIZE );
    size_t blocksWritten = fwrite ( buf, LOGICAL_BLOCK_SIZE, 1, dev->fd );
    if ( blocksWritten != 1 ) {
        fclose ( dev->fd );
        free ( dev );
        (*adfEnv.eFct)("adfCreateDumpDevice : fwrite");
        return NULL;
    }

    fclose ( dev->fd );

    dev->fd = fopen ( filename, "rb+" );
    if ( ! dev->fd ) {
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
    dev->isNativeDev = FALSE;
    dev->readOnly = FALSE;

    return(dev);
}

/*##################################################################################*/
