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
 *  along with ADFLib; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "adf_dev_flop.h"

#include "adf_env.h"
#include "adf_raw.h"
#include "adf_util.h"
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
ADF_RETCODE adfMountFlop ( struct AdfDevice * const dev )
{
    struct AdfVolume *vol;

    vol = (struct AdfVolume *) malloc (sizeof(struct AdfVolume));
    if (!vol) { 
        (*adfEnv.eFct)("adfMount : malloc");
        return ADF_RC_MALLOC;
    }

    vol->firstBlock = 0;
    vol->lastBlock = (int32_t) ( dev->cylinders * dev->heads * dev->sectors - 1 );
    vol->blockSize = 512;
    vol->dev = dev;
    vol->volName = NULL;
    vol->mounted = false;

    /* set filesystem info (read from bootblock) */
    struct AdfBootBlock boot;
    ADF_RETCODE rc = adfDevReadBlock (
        dev, (uint32_t)vol->firstBlock, 512, (uint8_t *)&boot );
    if ( rc != ADF_RC_OK ) {
        adfEnv.eFct ( "adfMountFlop : error reading BootBlock, device %s, volume %d",
                      dev->name, 0 );
        free ( vol );
        return rc;
    }
    memcpy ( vol->fs.id, boot.dosType, 3 );
    vol->fs.id[3] = '\0';
    vol->fs.type = (uint8_t) boot.dosType[3];

    if ( adfVolIsDosFS ( vol ) ) {
        vol->datablockSize = adfVolIsOFS ( vol ) ? 488 : 512;

        vol->rootBlock = adfVolCalcRootBlk ( vol );
        struct AdfRootBlock root;
        vol->mounted = true;    // must be set to read the root block
        rc = adfReadRootBlock ( vol, (uint32_t) vol->rootBlock, &root );
        vol->mounted = false;
        if ( rc != ADF_RC_OK ) {
            free ( vol );
            return rc;
        }

        vol->volName = strndup ( root.diskName,
                                 min ( root.nameLen,
                                       (unsigned) ADF_MAX_NAME_LEN ) );
    } else { // if ( adfVolIsPFS ( vol ) ) {
        vol->datablockSize = 0; //512;
        vol->volName = NULL;
        vol->rootBlock = -1;
    }

    dev->volList = (struct AdfVolume **) malloc (sizeof(struct AdfVolume *));
    if (!dev->volList) {
        free(vol->volName);
        free(vol);
        adfEnv.eFct ( "adfMountFlop : malloc, device '%s'", dev->name );
        return ADF_RC_MALLOC;
    }
    dev->volList[0] = vol;
    dev->nVol = 1;

/*printf("root=%d\n",vol->rootBlock);	    */
    return ADF_RC_OK;
}


/*
 * adfCreateFlop
 *
 * create a filesystem on a floppy device
 * fills dev->volList[]
 */
ADF_RETCODE adfCreateFlop ( struct AdfDevice * const dev,
                            const char * const       volName,
                            const uint8_t            volType )
{
    if (dev==NULL) {
        (*adfEnv.eFct)("adfCreateFlop : dev==NULL");
        return ADF_RC_NULLPTR;
    }
    if ( volName == NULL ) {
        (*adfEnv.eFct)("adfCreateFlop : volName == NULL");
        return ADF_RC_NULLPTR;
    }
    dev->volList = (struct AdfVolume **) malloc (sizeof(struct AdfVolume *));
    if (!dev->volList) { 
        (*adfEnv.eFct)("adfCreateFlop : malloc");
        return ADF_RC_MALLOC;
    }
    dev->volList[0] = adfVolCreate ( dev, 0L, 80L, volName, volType );
    if (dev->volList[0]==NULL) {
        free(dev->volList);
        return ADF_RC_ERROR;
    }
    dev->nVol = 1;
    dev->devType = ( dev->sectors == 11 ) ? ADF_DEVTYPE_FLOPDD :
                                            ADF_DEVTYPE_FLOPHD;
    dev->mounted = true;

    return ADF_RC_OK;
}
