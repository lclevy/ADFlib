/*
 *  ADF Library. (C) 1997-2002 Laurent Clevy
 *
 *  adf_dev.c
 *
 *  $Id$
 *
 *  device code
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

#include <stdlib.h>
#include <string.h>

#include "adf_dev.h"

#include "adf_dev_flop.h"
#include "adf_dev_hd.h"
#include "adf_dump.h"
#include "adf_env.h"
#include "adf_err.h"
#include "adf_nativ.h"


/*
 * adfOpenDev
 *
 * open a device without mounting it, used by adfMountDev() or
 * for partitioning/formatting with adfCreateFlop/Hd
 *
 * Note that:
 * - an opened device must be closed with adfCloseDev()
 *   before mounting it with adfMountDev()
 *
 * WARNING: IT IS NOT CHECKING WHETHER THERE IS ANY EXISTING FILESYSTEM
 *          ON THE DEVICE, IT DOES NOT LOAD ROOTBLOCK ETC.
 *          IF UNSURE USE adfMountDev() FIRST TO CHECK IF FILESYSTEM STRUCTURES
 *          EXIST ALREADY ON THE DEVICE(!)
 */
struct adfDevice * adfOpenDev ( char * filename, BOOL ro )
{
    struct adfDevice * dev = ( struct adfDevice * )
        malloc ( sizeof ( struct adfDevice ) );
    if ( ! dev ) {
        (*adfEnv.eFct)("adfOpenDev : malloc error");
        return NULL;
    }

    dev->readOnly = ro;

    /* switch between dump files and real devices */
    struct nativeFunctions * nFct = adfEnv.nativeFct;
    dev->isNativeDev = ( *nFct->adfIsDevNative )( filename );

    RETCODE rc;
    if ( dev->isNativeDev )
        rc = ( *nFct->adfInitDevice )( dev, filename, ro );
    else
        rc = adfInitDumpDevice ( dev, filename, ro );
    if ( rc != RC_OK ) {
        ( *adfEnv.eFct )( "adfOpenDev : device init error" );
        free ( dev );
        return NULL;
    }

    dev->devType = adfDevType ( dev );
    dev->nVol    = 0;
    dev->volList = NULL;

    /*
    if ( dev->devType == DEVTYPE_FLOPDD ) {
        device->sectors = 11;
        device->heads = 2;
        fdtype = "DD";
    } else if ( dev->devType == DEVTYPE_FLOPHD ) {
        device->sectors = 22;
        device->heads = 2;
        fdtype = "HD";
    } else if ( dev->devType == DEVTYPE_HARDDISK ) {
        fprintf ( stderr, "adfOpenDev(): harddisk devices not implemented - aborting...\n" );
        return 1;
    } else {
        fprintf ( stderr, "adfOpenDev(): unknown device type - aborting...\n" );
        return 1;
    }
    device->cylinders = device->size / ( device->sectors * device->heads * 512 );
    */

    return dev;
}


/*
 * adfCloseDev
 *
 * Closes/releases an opened device.
 * Called by adfUnMountDev()
 */
void adfCloseDev ( struct adfDevice * dev )
{
    if ( ! dev )
        return;

    // free volume list
    //if ( dev->volList ) {
    if ( dev->nVol > 0 ) {
        for ( int i = 0 ; i < dev->nVol ; i++ ) {
            free ( dev->volList[i]->volName );
            free ( dev->volList[i] );
        }
        free ( dev->volList );
        dev->nVol = 0;
    }

    if ( dev->isNativeDev ) {
        struct nativeFunctions * const nFct = adfEnv.nativeFct;
        ( *nFct->adfReleaseDevice )( dev );
    } else
        adfReleaseDumpDevice ( dev );

    free ( dev );
}


/*
 * adfDevType
 *
 * returns the type of a device
 * only based of the field 'dev->size'
 */
int adfDevType ( struct adfDevice * dev )
{
    if( (dev->size==512*11*2*80) ||		/* BV */
        (dev->size==512*11*2*81) ||		/* BV */
        (dev->size==512*11*2*82) || 	/* BV */
        (dev->size==512*11*2*83) )		/* BV */
        return(DEVTYPE_FLOPDD);
    else if (dev->size==512*22*2*80)
        return(DEVTYPE_FLOPHD);
    else if (dev->size>512*22*2*80)
        return(DEVTYPE_HARDDISK);
    else {
        (*adfEnv.eFct)("adfDevType : unknown device type");
        return(-1);
    }
}


/*
 * adfDeviceInfo
 *
 * display information about the device and its volumes
 * for demonstration purpose only since the output is stdout !
 *
 * can be used before adfCreateVol() or adfMount()
 */
void adfDeviceInfo ( struct adfDevice * dev )
{
    const char * devTypeInfo = NULL;
    switch ( dev->devType ) {
    case DEVTYPE_FLOPDD:
        devTypeInfo = "floppy dd";
        break;
    case DEVTYPE_FLOPHD:
        devTypeInfo = "floppy hd";
        break;
    case DEVTYPE_HARDDISK:
        devTypeInfo = "harddisk";
        break;
    case DEVTYPE_HARDFILE:
        devTypeInfo = "hardfile";
        break;
    default:
        devTypeInfo = "unknown device type!";
    }

    printf ( "\nADF device info:\n  Type:\t\t%s, %s\n", devTypeInfo,
             dev->isNativeDev ? "real (native device!)" : "file (image)" );

    printf ( "  Geometry:\n"
             "    Cylinders\t%d\n"
             "    Heads\t%d\n"
             "    Sectors\t%d\n\n",
             dev->cylinders, dev->heads, dev->sectors );

    printf ( "  Volumes (%d):\n"
             "   idx  first bl.     last bl.    name\n",
             dev->nVol );

    for ( int i = 0 ; i < dev->nVol ; i++ ) {
        if ( dev->volList[i]->volName )
            printf("    %2d    %7d      %7d    \"%s\"", i,
                   dev->volList[i]->firstBlock,
                   dev->volList[i]->lastBlock,
                   dev->volList[i]->volName);
        else
            printf("    %2d    %7d      %7d\n", i,
                   dev->volList[i]->firstBlock,
                   dev->volList[i]->lastBlock);
        if ( dev->volList[i]->mounted )
            printf("    mounted");
        putchar('\n');
    }
    putchar('\n');
}


/*
 * adfMountDev
 *
 * mount a dump file (.adf) or a real device (uses adf_nativ.c and .h)
 *
 * adfInitDevice() must fill dev->size !
 */
struct adfDevice * adfMountDev ( char * filename,
                                 BOOL   ro )
{
    RETCODE rc;
    uint8_t buf[512];

    struct adfDevice * dev = adfOpenDev ( filename, ro );
    if ( ! dev ) {
        //(*adfEnv.eFct)("adfMountDev : malloc error");
        return NULL;
    }

    switch( dev->devType ) {

    case DEVTYPE_FLOPDD:
    case DEVTYPE_FLOPHD:
        if ( adfMountFlop ( dev ) != RC_OK ) {
            adfCloseDev ( dev );
            return NULL;
        }
        break;

    case DEVTYPE_HARDDISK:
        rc = adfReadBlockDev ( dev, 0, 512, buf );

       /* BV ...from here*/
        if( rc != RC_OK ) {
            adfCloseDev ( dev );
            (*adfEnv.eFct)("adfMountDev : adfReadDumpSector failed");
            return NULL;
        }

        /* a file with the first three bytes equal to 'DOS' */
    	if (!dev->isNativeDev && strncmp("DOS",(char*)buf,3)==0) {
            if ( adfMountHdFile ( dev ) != RC_OK ) {
                adfCloseDev ( dev );
                return NULL;
            }
        }
        else if ( adfMountHd ( dev ) != RC_OK ) {
            adfCloseDev ( dev );
            return NULL;								/* BV ...to here.*/
        }
	    break;

    default:
        (*adfEnv.eFct)("adfMountDev : unknown device type");
        adfCloseDev ( dev );
        return NULL;								/* BV */
    }

	return dev;
}


/*
 * adfUnMountDev
 *
 */
void adfUnMountDev ( struct adfDevice * dev )
{
    adfCloseDev ( dev );
}


RETCODE adfReadBlockDev ( struct adfDevice * dev,
                          int32_t            pSect,
                          int32_t            size,
                          uint8_t *          buf )
{
    RETCODE rc;

/*printf("pSect R =%ld\n",pSect);*/
    if ( dev->isNativeDev ) {
        struct nativeFunctions * const nFct = adfEnv.nativeFct;
        rc = (*nFct->adfNativeReadSector)( dev, pSect, size, buf );
    } else
        rc = adfReadDumpSector( dev, pSect, size, buf );
/*printf("rc=%ld\n",rc);*/
    return rc;
}


RETCODE adfWriteBlockDev ( struct adfDevice * dev,
                           int32_t            pSect,
                           int32_t            size,
                           uint8_t *          buf )
{
    RETCODE rc;

/*printf("nativ=%d\n",dev->isNativeDev);*/
    if ( dev->isNativeDev ) {
        struct nativeFunctions * const nFct = adfEnv.nativeFct;
        rc = (*nFct->adfNativeWriteSector)( dev, pSect, size, buf );
    } else
        rc = adfWriteDumpSector ( dev, pSect, size, buf );

    return rc;
}
