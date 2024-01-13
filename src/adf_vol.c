/*
 *  ADF Library. (C) 1997-2002 Laurent Clevy
 *
 *  adf_vol.c
 *
 *  $Id$
 *
 * logical disk/volume code
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


#include "adf_vol.h"

#include "adf_bitm.h"
#include "adf_cache.h"
#include "adf_env.h"
#include "adf_nativ.h"
#include "adf_raw.h"
#include "adf_util.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>


uint32_t bitMask[32] = { 
    0x1, 0x2, 0x4, 0x8,
    0x10, 0x20, 0x40, 0x80,
    0x100, 0x200, 0x400, 0x800,
    0x1000, 0x2000, 0x4000, 0x8000,
    0x10000, 0x20000, 0x40000, 0x80000,
    0x100000, 0x200000, 0x400000, 0x800000,
    0x1000000, 0x2000000, 0x4000000, 0x8000000,
    0x10000000, 0x20000000, 0x40000000, 0x80000000 };


RETCODE adfInstallBootBlock ( struct AdfVolume * const vol,
                              const uint8_t * const    code )
{
    int i;
    struct bBootBlock boot;

    if ( vol->dev->devType != DEVTYPE_FLOPDD &&
         vol->dev->devType != DEVTYPE_FLOPHD )
    {
        return RC_ERROR;
    }

    RETCODE rc = adfReadBootBlock ( vol, &boot );
    if ( rc != RC_OK )
        return rc;

    boot.rootBlock = 880;
    for(i=0; i<1024-12; i++)         /* bootcode */
        boot.data[i] = code[i+12];

    rc = adfWriteBootBlock ( vol, &boot );
    if ( rc != RC_OK )
        return rc;

    vol->bootCode = TRUE;

    return RC_OK;
}


/*
 * isSectNumValid
 *
 */
BOOL isSectNumValid ( const struct AdfVolume * const vol,
                      const SECTNUM                  nSect )
{
    return ( nSect >= 0 &&
             nSect <= (vol->lastBlock - vol->firstBlock) );
}	
	


/*
 * adfVolumeInfo
 *
 */
void adfVolumeInfo ( struct AdfVolume * const vol )
{
    struct bRootBlock root;
    char diskName[35];
    int days,month,year;
	
    if ( adfReadRootBlock(vol, (uint32_t) vol->rootBlock, &root) != RC_OK )
        return;
	
    memset(diskName, 0, 35);
    memcpy(diskName, root.diskName, root.nameLen);
	
    printf ( "\nADF volume info:\n  Name:\t\t%-30s\n", vol->volName );
    printf ("  Type:\t\t");
    switch ( vol->dev->devType) {
    case DEVTYPE_FLOPDD:
        printf ("Floppy Double Density, 880 KBytes\n");
        break;
    case DEVTYPE_FLOPHD:
        printf ("Floppy High Density, 1760 KBytes\n");
        break;
    case DEVTYPE_HARDDISK:
        printf ("Hard Disk partition, %3.1f KBytes\n",
                ( vol->lastBlock - vol->firstBlock + 1 ) * 512.0 / 1024.0 );
        break;
    case DEVTYPE_HARDFILE:
        printf ("HardFile : %3.1f KBytes\n",
                ( vol->lastBlock - vol->firstBlock + 1 ) * 512.0 / 1024.0 );
        break;
    default:
        printf ("Unknown devType!\n");
    }
    printf ("  Filesystem:\t%s %s %s\n",
            isFFS(vol->dosType) ? "FFS" : "OFS",
            isINTL(vol->dosType) ? "INTL " : "",
            isDIRCACHE(vol->dosType) ? "DIRCACHE " : "");

    printf("  Free blocks:\t%d\n", adfCountFreeBlocks(vol));
    printf ("  R/W:\t\t%s\n", vol->readOnly ? "Read only" : "Read/Write");
 	
    /* created */
    adfDays2Date(root.coDays, &year, &month, &days);
    printf ( "  Created:\t%d/%02d/%02d %d:%02d:%02d\n",
             days, month, year,
             root.coMins / 60,
             root.coMins % 60,
             root.coTicks / 50 );

    adfDays2Date(root.days, &year, &month, &days);
    printf ( "  Last access:\t%d/%02d/%02d %d:%02d:%02d",
             days, month, year,
             root.mins / 60,
             root.mins % 60,
             root.ticks / 50 );

    adfDays2Date(root.cDays, &year, &month, &days);
    printf ( "\n\t\t%d/%02d/%02d %d:%02d:%02d\n",
             days, month, year,
             root.cMins / 60,
             root.cMins % 60,
             root.cTicks / 50 );
}



/*
 * adfMount
 *
 * 
 */
PREFIX struct AdfVolume * adfMount ( struct AdfDevice * const dev,
                                     const int                nPart,
                                     const AdfAccessMode      mode )
{
    struct bRootBlock root;
    struct bBootBlock boot;
    struct AdfVolume * vol;

    if (dev==NULL || nPart >= dev->nVol) {
        (*adfEnv.eFct)("adfMount : invalid parameter(s)");
        return NULL;
    }

    vol = dev->volList[nPart];
    vol->dev = dev;
    vol->mounted = TRUE;

/*printf("first=%ld last=%ld root=%ld\n",vol->firstBlock,
 vol->lastBlock, vol->rootBlock);
*/
    if (adfReadBootBlock(vol, &boot)!=RC_OK) {
        adfEnv.eFct ( "adfMount : invalid BootBlock" );
        vol->mounted = FALSE;
        return NULL;
    }       
    
    vol->dosType = (uint8_t) boot.dosType[3];
    if (isFFS(vol->dosType))
        vol->datablockSize = 512;
    else
        vol->datablockSize = 488;

    if (dev->readOnly /*|| isDIRCACHE(vol->dosType)*/)
        vol->readOnly = TRUE;
    else
        vol->readOnly = ( mode != ADF_ACCESS_MODE_READWRITE );
	   	
    if ( adfReadRootBlock ( vol, (uint32_t) vol->rootBlock, &root ) != RC_OK ) {
        adfEnv.eFct ( "adfMount : invalid RootBlock, sector %u", vol->rootBlock );
        vol->mounted = FALSE;
        return NULL;
    }

    RETCODE rc = adfBitmapAllocate ( vol );
    if ( rc != RC_OK ) {
            adfEnv.eFct ( "adfMount : adfBitmapAllocate() returned error %d, "
                          "mounting volume %s failed", rc, vol->volName );
            adfUnMount ( vol );
            return NULL;
    }

    rc = adfReadBitmap ( vol, &root );
    if ( rc != RC_OK ) {
        adfEnv.eFct ( "adfMount : adfReadBitmap() returned error %d, "
                      "mounting volume %s failed", rc, vol->volName );
        adfUnMount ( vol );
        return NULL;
    }

    /*
    if ( root.bmFlag != BM_VALID ) {
        if ( vol->readOnly == TRUE ) {
            rc = adfReconstructBitmap ( vol, &root );
            if ( rc != RC_OK ) {
                adfEnv.eFct ( "adfMount : adfReconstructBitmap() returned error %d, "
                              "mounting volume %s failed", rc, vol->volName );
                adfUnMount ( vol );
                return NULL;
            }
        } else {
            adfEnv.eFct ( "adfMount : block allocation bitmap marked invalid in root block, "
                          "mounting the volume %s read-write not possible", vol->volName );
            adfUnMount ( vol );
            return NULL;
        }
    }
    */

    vol->curDirPtr = vol->rootBlock;

/*printf("blockSize=%d\n",vol->blockSize);*/

    return( vol );
}


/*
 * adfRemountReadWrite
 *
 *
 */
PREFIX RETCODE adfRemountReadWrite ( struct AdfVolume * vol )
{
    if ( vol == NULL )
        return RC_ERROR;

    if ( vol->readOnly == FALSE ) {
        adfEnv.wFct ( "adfRemountReadWrite : volume %s already mounted read-write",
                      vol->volName );
        return RC_OK;
    }

    if ( vol->dev->readOnly ) {
        adfEnv.eFct ( "adfRemountReadWrite : device read-only, "
                      "cannot mount %s read-write", vol->volName );
        return RC_ERROR;
    }

    // the volume's bitmap could have been rebuilt in memory,
    // write it to the volume when entering read-write mode
    vol->readOnly = FALSE;
    RETCODE rc = adfUpdateBitmap ( vol );
    if ( rc != RC_OK ) {
        vol->readOnly = TRUE;
        return rc;
    }

    vol->readOnly = FALSE;
    return RC_OK;
}



/*
*
* adfUnMount
*
* free bitmap structures
* free current dir
*/
void adfUnMount ( struct AdfVolume * const vol )
{
    if (!vol) {
        (*adfEnv.eFct)("adfUnMount : vol is null");
        return;
    }

    adfFreeBitmap(vol);

    vol->mounted = FALSE;
	
}



/*
 * adfCreateVol
 *
 * 
 */
struct AdfVolume * adfCreateVol ( struct AdfDevice * const dev,
                                  const uint32_t           start,
                                  const uint32_t           len,
                                  const char * const       volName,
                                  const uint8_t            volType )
{
    struct bBootBlock boot;
    struct bRootBlock root;
/*    struct bDirCacheBlock dirc;*/
    SECTNUM blkList[2];
    struct AdfVolume* vol;

    if (adfEnv.useProgressBar)
        (*adfEnv.progressBar)(0);

    vol = (struct AdfVolume *) malloc (sizeof(struct AdfVolume));
    if (!vol) { 
        (*adfEnv.eFct)("adfCreateVol : malloc vol");
        return NULL;
    }
	
    vol->dev = dev;
    vol->firstBlock = (int32_t) ( dev->heads * dev->sectors * start );
    vol->lastBlock = vol->firstBlock + (int32_t) ( dev->heads * dev->sectors * len ) - 1;
    vol->rootBlock = adfVolCalcRootBlk ( vol );

/*printf("first=%ld last=%ld root=%ld\n",vol->firstBlock,
 vol->lastBlock, vol->rootBlock);
*/
    vol->curDirPtr = vol->rootBlock;

    vol->readOnly = dev->readOnly;

    vol->mounted = TRUE;

    unsigned nlen = min ( (unsigned) MAXNAMELEN,
                          (unsigned) strlen ( volName ) );
    vol->volName = (char*)malloc(nlen+1);
    if (!vol->volName) { 
        (*adfEnv.eFct)("adfCreateVol : malloc");
        free(vol); return NULL;
    }
    memcpy(vol->volName, volName, nlen);
    vol->volName[nlen]='\0';

    if (adfEnv.useProgressBar)
        (*adfEnv.progressBar)(25);

    memset(&boot, 0, 1024);
    boot.dosType[3] = (char) volType;
/*printf("first=%d last=%d\n", vol->firstBlock, vol->lastBlock);
printf("name=%s root=%d\n", vol->volName, vol->rootBlock);
*/
    if (adfWriteBootBlock(vol, &boot)!=RC_OK) {
        free(vol->volName); free(vol);
        return NULL;
    }

    if (adfEnv.useProgressBar)
        (*adfEnv.progressBar)(20);

    if (adfCreateBitmap( vol )!=RC_OK) {
        free(vol->volName); free(vol);
        return NULL;
    }

    if (adfEnv.useProgressBar)
        (*adfEnv.progressBar)(40);


/*for(i=0; i<127; i++)
printf("%3d %x, ",i,vol->bitmapTable[0]->map[i]);
*/
    if ( isDIRCACHE(volType) )
        adfGetFreeBlocks( vol, 2, blkList );
    else
        adfGetFreeBlocks( vol, 1, blkList );


/*printf("[0]=%d [1]=%d\n",blkList[0],blkList[1]);*/

    memset(&root, 0, LOGICAL_BLOCK_SIZE);

    root.nameLen = (uint8_t) strlen ( vol->volName );
    memcpy(root.diskName,volName,root.nameLen);
    adfTime2AmigaTime(adfGiveCurrentTime(),&(root.coDays),&(root.coMins),&(root.coTicks));

    /* dircache block */
    if ( isDIRCACHE(volType) ) {
        root.extension = 0L;
        root.secType = ST_ROOT; /* needed by adfCreateEmptyCache() */
        adfCreateEmptyCache(vol, (struct bEntryBlock*)&root, blkList[1]);
    }

    if (adfEnv.useProgressBar)
        (*adfEnv.progressBar)(60);

    if ( adfWriteRootBlock ( vol, (uint32_t) blkList[0], &root ) != RC_OK ) {
        free(vol->volName); free(vol);
        return NULL;
    }

   /* fills root->bmPages[] and writes filled bitmapExtBlocks */
    if (adfWriteNewBitmap(vol)!=RC_OK)
        return NULL;

    if (adfEnv.useProgressBar)
        (*adfEnv.progressBar)(80);

    if (adfUpdateBitmap(vol)!=RC_OK)
        return NULL;

    if (adfEnv.useProgressBar)
        (*adfEnv.progressBar)(100);
/*printf("free blocks %ld\n",adfCountFreeBlocks(vol));*/

    /* will be managed by adfMount() later */
    adfFreeBitmap(vol);

    vol->mounted = FALSE;

    return(vol);
}

/*
RETCODE adfVolReconstructBitmap ( struct AdfVolume * const vol )
{
    struct bRootBlock root;
    //printf ("reading root block from %u\n", vol->rootBlock );
    RETCODE rc = adfReadRootBlock ( vol, (uint32_t) vol->rootBlock, &root );
    if ( rc != RC_OK ) {
        adfEnv.eFct ( "adfVolReconstructBitmap : invalid RootBlock, sector %u",
                      vol->rootBlock );
        return rc;
    }
    //printf ("root block read, name %s\n", root.diskName );
    return adfReconstructBitmap ( vol, &root );
}
*/

/*-----*/

/*
 * adfReadBlock
 *
 * read logical block
 */
RETCODE adfReadBlock ( struct AdfVolume * const vol,
                       const uint32_t           nSect,
                       uint8_t * const          buf )
{
    if (!vol->mounted) {
        (*adfEnv.eFct)("the volume isn't mounted, adfReadBlock not possible");
        return RC_ERROR;
    }

    /* translate logical sect to physical sect */
    unsigned pSect = nSect + (unsigned) vol->firstBlock;

    if (adfEnv.useRWAccess)
        (*adfEnv.rwhAccess)( (SECTNUM) pSect, (SECTNUM) nSect, FALSE );

/*  char strBuf[80];
    printf("psect=%ld nsect=%ld\n",pSect,nSect);
    sprintf(strBuf,"ReadBlock : accessing logical block #%ld", nSect);
    adfEnv.vFct(strBuf);
*/
    if ( pSect < (unsigned) vol->firstBlock ||
         pSect > (unsigned) vol->lastBlock )
    {
        adfEnv.wFct("adfReadBlock : nSect %u out of range", nSect );
        return RC_BLOCKOUTOFRANGE;
    }

    RETCODE rc = adfReadBlockDev ( vol->dev, pSect, 512, buf );
    if ( rc != RC_OK ) {
        adfEnv.eFct ( "adfReadBlock: error reading block %d, volume '%s'",
                      nSect, vol->volName );
    }
    return rc;
}


/*
 * adfWriteBlock
 *
 */
RETCODE adfWriteBlock ( struct AdfVolume * const vol,
                        const uint32_t           nSect,
                        const uint8_t * const    buf )
{
    if (!vol->mounted) {
        (*adfEnv.eFct)("the volume isn't mounted, adfWriteBlock not possible");
        return RC_ERROR;
    }

    if (vol->readOnly) {
        (*adfEnv.wFct)("adfWriteBlock : can't write block, read only volume");
        return RC_ERROR;
    }

    unsigned pSect = nSect + (unsigned) vol->firstBlock;
/*printf("write nsect=%ld psect=%ld\n",nSect,pSect);*/

    if (adfEnv.useRWAccess)
        adfEnv.rwhAccess ( (SECTNUM) pSect, (SECTNUM) nSect, TRUE );
 
    if ( pSect < (unsigned) vol->firstBlock ||
         pSect > (unsigned) vol->lastBlock )
    {
        adfEnv.wFct ( "adfWriteBlock : nSect %u out of range", nSect );
        return RC_BLOCKOUTOFRANGE;
    }

    RETCODE rc = adfWriteBlockDev ( vol->dev, pSect, 512, buf );
    if ( rc != RC_OK ) {
        adfEnv.eFct ( "adfWriteBlock: error writing block %d, volume '%s'",
                      nSect, vol->volName );
    }
    return rc;
}



/*#######################################################################################*/
