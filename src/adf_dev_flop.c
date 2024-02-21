/*
 *  ADF Library. (C) 1997-2002 Laurent Clevy
 *
 *  adf_dev_flop.c
 *
 *  $Id$
 *
 *  device code / floppy
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

#include "adf_dev_flop.h"

#include "adf_env.h"
#include "adf_raw.h"
#include "adf_vol.h"

#include <stdlib.h>
#include <string.h>


/*
 * adfMountFlop
 *
 * normaly not used directly, called directly by adfDevMount()
 *
 * use dev->devType to choose between DD and HD
 * fills the volume list with one volume
 */
RETCODE adfMountFlop ( struct AdfDevice * const dev )
{
    struct AdfVolume *vol;

    vol = (struct AdfVolume *) malloc (sizeof(struct AdfVolume));
    if (!vol) { 
        (*adfEnv.eFct)("adfMount : malloc");
        return RC_ERROR;
    }

    vol->firstBlock = 0;
    vol->lastBlock = (int32_t) ( dev->cylinders * dev->heads * dev->sectors - 1 );
    vol->blockSize = 512;
    vol->dev = dev;

    /* set filesystem info (read from bootblock) */
    struct AdfBootBlock boot;
    if ( adfDevReadBlock ( dev, vol->firstBlock, 512, &boot ) != RC_OK ) {
        adfEnv.eFct ( "adfMountFlop : error reading BootBlock, device %s, volume %d",
                      dev->name, 0 );
        free ( vol );
        return RC_ERROR;
    }
    memcpy ( vol->fs.id, boot.dosType, 3 );
    vol->fs.id[3] = '\0';
    vol->fs.type = (uint8_t) boot.dosType[3];

    if ( adfVolIsDosFS ( vol ) ) {
        vol->datablockSize = adfVolIsOFS ( vol ) ? 488 : 512;

        vol->rootBlock = adfVolCalcRootBlk ( vol );
        struct AdfRootBlock root;
        vol->mounted = TRUE;    // must be set to read the root block
        if ( adfReadRootBlock ( vol, (uint32_t) vol->rootBlock, &root ) != RC_OK ) {
            free ( vol );
            return RC_ERROR;
        }
        vol->mounted = FALSE;

        char diskName[35];
        memset(diskName, 0, 35);
        memcpy(diskName, root.diskName, root.nameLen);

        vol->volName = strdup(diskName);
    } else { // if ( adfVolIsPFS ( vol ) ) {
        vol->datablockSize = 0; //512;
        vol->volName = NULL;
        vol->rootBlock = -1;
    }

    dev->volList = (struct AdfVolume **) malloc (sizeof(struct AdfVolume *));
    if (!dev->volList) {
        free(vol->volName);
        free(vol);
        (*adfEnv.eFct)("adfMount : malloc");
        return RC_ERROR;
    }
    dev->volList[0] = vol;
    dev->nVol = 1;

/*printf("root=%d\n",vol->rootBlock);	    */
    return RC_OK;
}


/*
 * adfCreateFlop
 *
 * create a filesystem on a floppy device
 * fills dev->volList[]
 */
RETCODE adfCreateFlop ( struct AdfDevice * const dev,
                        const char * const       volName,
                        const uint8_t            volType )
{
    if (dev==NULL) {
        (*adfEnv.eFct)("adfCreateFlop : dev==NULL");
        return RC_ERROR;
    }
    if ( volName == NULL ) {
        (*adfEnv.eFct)("adfCreateFlop : volName == NULL");
        return RC_ERROR;
    }
    dev->volList = (struct AdfVolume **) malloc (sizeof(struct AdfVolume *));
    if (!dev->volList) { 
        (*adfEnv.eFct)("adfCreateFlop : malloc");
        return RC_ERROR;
    }
    dev->volList[0] = adfVolCreate ( dev, 0L, 80L, volName, volType );
    if (dev->volList[0]==NULL) {
        free(dev->volList);
        return RC_ERROR;
    }
    dev->nVol = 1;
    if (dev->sectors==11)
        dev->devType=DEVTYPE_FLOPDD;
    else
        dev->devType=DEVTYPE_FLOPHD;

    dev->mounted = TRUE;

    return RC_OK;
}
