/*
 *  ADF Library. (C) 1997-2002 Laurent Clevy
 *
 *  adf_dev_hd.c
 *
 *  $Id$
 *
 *  harddisk / device code
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


#include "adf_dev_hd.h"

#include "adf_byteorder.h"
#include "adf_dev_driver_dump.h"
#include "adf_env.h"
#include "adf_raw.h"
#include "adf_util.h"
#include "adf_vol.h"

#include <stdlib.h>
#include <string.h>


/*
 * adfFreeTmpVolList
 *
 */
static void adfFreeTmpVolList ( struct AdfList * const root )
{
    struct AdfList *cell;
    struct AdfVolume *vol;

    cell = root;
    while(cell!=NULL) {
        vol = (struct AdfVolume *) cell->content;
        if (vol->volName!=NULL)
            free(vol->volName);  
        cell = cell->next;
    }
    freeList(root);

}


/*
 * adfMountHdFile
 *
 */
RETCODE adfMountHdFile ( struct AdfDevice * const dev )
{
    struct AdfVolume * vol;

    dev->nVol = 0;
    dev->volList = (struct AdfVolume **) malloc (sizeof(struct AdfVolume *));
    if ( dev->volList == NULL ) {
        (*adfEnv.eFct)("adfMountHdFile : malloc");
        return RC_MALLOC;
    }

    vol = (struct AdfVolume *) malloc (sizeof(struct AdfVolume));
    if ( vol == NULL ) {
        free ( dev->volList );
        dev->volList = NULL;
        (*adfEnv.eFct)("adfMountHdFile : malloc");
        return RC_MALLOC;
    }
    dev->volList[0] = vol;
    dev->nVol++;      /* fixed by Dan, ... and by Gary */

    vol->dev = dev;
    vol->volName=NULL;
    
    vol->firstBlock = 0;

    unsigned size = dev->size + 512 - ( dev->size % 512 );
/*printf("size=%ld\n",size);*/

    /* set filesystem info (read from bootblock) */
    struct AdfBootBlock boot;
    if ( adfDevReadBlock ( dev, (uint32_t)vol->firstBlock, 512, (uint8_t *)&boot ) != RC_OK ) {
        adfEnv.eFct ( "adfMountHdFile : error reading BootBlock, device %s, volume %d",
                      dev->name, 0 );
        free ( dev->volList );
        dev->volList = NULL;
        return RC_ERROR;
    }
    memcpy ( vol->fs.id, boot.dosType, 3 );
    vol->fs.id[3] = '\0';
    vol->fs.type = (uint8_t) boot.dosType[3];
    vol->datablockSize = adfVolIsOFS ( vol ) ? 488 : 512;

    if ( adfVolIsDosFS ( vol ) ) {
        vol->rootBlock = (int32_t) ( ( size / 512 ) / 2 );
/*printf("root=%ld\n",vol->rootBlock);*/
        uint8_t buf[512];
        bool found = false;
        do {
            dev->drv->readSector ( dev, (uint32_t) vol->rootBlock, 512, buf );
            found = swapLong(buf) == ADF_T_HEADER &&
                    swapLong(buf + 508) == ADF_ST_ROOT;
            if (!found)
                (vol->rootBlock)--;
        } while (vol->rootBlock>1 && !found);

        if (vol->rootBlock==1) {
            (*adfEnv.eFct)("adfMountHdFile : rootblock not found");
            free ( dev->volList );
            dev->volList = NULL;
            free ( vol );
            dev->nVol = 0;
            return RC_ERROR;
        }
        vol->lastBlock = vol->rootBlock * 2 - 1;
    } else { // if ( adfVolIsPFS ( vol ) ) {
        vol->datablockSize = 0; //512;
        vol->volName = NULL;
        vol->rootBlock = -1;
        vol->lastBlock = (int32_t) ( dev->cylinders * dev->heads * dev->sectors - 1 );
    }

    return RC_OK;
}


/*
 * adfMountHd
 *
 * normal not used directly : called by adfDevMount()
 *
 * fills geometry fields and volumes list (dev->nVol and dev->volList[])
 */
RETCODE adfMountHd ( struct AdfDevice * const dev )
{
    struct AdfRSDKblock rdsk;
    struct AdfPARTblock part;
    int32_t next;
    struct AdfList *vList, *listRoot;
    int i;
    struct AdfVolume * vol;
    unsigned len;

    RETCODE rc = adfReadRDSKblock ( dev, &rdsk );
    if ( rc != RC_OK )
        return rc;

    /* PART blocks */
    listRoot = NULL;
    next = rdsk.partitionList;
    dev->nVol=0;
    vList = NULL;
    while( next!=-1 ) {
        rc = adfReadPARTblock ( dev, next, &part );
        if ( rc != RC_OK ) {
            adfFreeTmpVolList(listRoot);
            (*adfEnv.eFct)("adfMountHd : malloc");
            return rc;
        }

        vol = (struct AdfVolume *) malloc (sizeof(struct AdfVolume));
        if ( vol == NULL ) {
            adfFreeTmpVolList(listRoot);
            (*adfEnv.eFct)("adfMountHd : malloc");
            return RC_MALLOC;
        }
        vol->dev = dev;
        vol->volName=NULL;
        dev->nVol++;

        vol->firstBlock = (int32_t) rdsk.cylBlocks * part.lowCyl;
        vol->lastBlock = ( part.highCyl + 1 ) * (int32_t) rdsk.cylBlocks - 1;
        vol->blockSize = part.blockSize*4;

        /* set filesystem info (read from bootblock) */
        struct AdfBootBlock boot;
        if ( adfDevReadBlock ( dev, (uint32_t)vol->firstBlock, 512, (uint8_t *)&boot ) != RC_OK ) {
            adfEnv.eFct ( "adfMountHd : error reading BootBlock, device %s, volume %d",
                          dev->name, dev->nVol - 1 );
            adfFreeTmpVolList ( listRoot );
            free ( vol );
            return RC_ERROR;
        }
        memcpy ( vol->fs.id, boot.dosType, 3 );
        vol->fs.id[3] = '\0';
        vol->fs.type = (uint8_t) boot.dosType[3];
        vol->datablockSize = adfVolIsOFS ( vol ) ? 488 : 512;

        /* set volume name (from partition info) */
        len = (unsigned) min ( 31, part.nameLen );
        vol->volName = (char*)malloc(len+1);
        if ( vol->volName == NULL ) { 
            adfFreeTmpVolList(listRoot);
            free ( vol );
            (*adfEnv.eFct)("adfMount : malloc");
            return RC_MALLOC;
        }
        memcpy(vol->volName,part.name,len);
        vol->volName[len] = '\0';

        vol->mounted = false;

        /* stores temporaly the volumes in a linked list */
        if (listRoot==NULL)
            vList = listRoot = newCell(NULL, (void*)vol);
        else
            vList = newCell(vList, (void*)vol);

        if (vList==NULL) {
            adfFreeTmpVolList(listRoot);
            (*adfEnv.eFct)("adfMount : newCell() malloc");
            return RC_MALLOC;
        }

        vol->rootBlock = adfVolIsDosFS ( vol ) ? adfVolCalcRootBlk ( vol ) : -1;

        next = part.next;
    }

    /* stores the list in an array */
    dev->volList = (struct AdfVolume **) malloc (
        sizeof(struct AdfVolume *) * (unsigned) dev->nVol );
    if ( dev->volList == NULL ) {
        adfFreeTmpVolList(listRoot);
        (*adfEnv.eFct)("adfMount : malloc");
        return RC_MALLOC;
    }
    vList = listRoot;
    for(i=0; i<dev->nVol; i++) {
        dev->volList[i] = (struct AdfVolume *) vList->content;
        vList = vList->next;
    }
    freeList(listRoot);

    /* The code below seems to only check if the FSHD and LSEG blocks can be
       read. These blocks are not required to access partitions/volumes:
       http://lclevy.free.fr/adflib/adf_info.html#p64 */

    struct AdfFSHDblock fshd;
    next = rdsk.fileSysHdrList;
    while( next!=-1 ) {
        rc = adfReadFSHDblock ( dev, next, &fshd ); 
        if ( rc != RC_OK ) {
            /*
            for ( i = 0 ; i < dev->nVol ; i++ )
                free ( dev->volList[i] );
            free(dev->volList); */
            adfEnv.wFct ("adfMountHd : adfReadFSHDblock error, device %s, sector %d",
                         dev->name, next );
            //return rc;
            break;
        }
        next = fshd.next;
    }

    struct AdfLSEGblock lseg;
    next = fshd.segListBlock;
    while( next!=-1 ) {
        rc = adfReadLSEGblock ( dev, next, &lseg ); 
        if ( rc != RC_OK ) {
            /*for ( i = 0 ; i < dev->nVol ; i++ )
                free ( dev->volList[i] );
            free(dev->volList); */
            adfEnv.wFct ("adfMountHd : adfReadLSEGblock error, device %s, sector %s",
                         dev->name, next );
            //return rc;
            break;
        }
        next = lseg.next;
    }

    return RC_OK;
}


/*
 * adfCreateHdHeader
 *
 * create PARTIALLY the sectors of the header of one harddisk : can not be mounted
 * back on a real Amiga ! It's because some device dependant values can't be guessed...
 *
 * do not use dev->volList[], but partList for partitions information : start and len are cylinders,
 *  not blocks
 * do not fill dev->volList[]
 * called by adfCreateHd()
 */
RETCODE adfCreateHdHeader ( struct AdfDevice * const               dev,
                            const int                              n,
                            const struct Partition * const * const partList )
{
    (void) n;
    int i;
    struct AdfRSDKblock rdsk;
    struct AdfPARTblock part;
    SECTNUM j;
    unsigned len;

    /* RDSK */ 
 
    memset ( (uint8_t *) &rdsk, 0, sizeof(struct AdfRSDKblock) );

    rdsk.rdbBlockLo = 0;
    rdsk.rdbBlockHi = (dev->sectors*dev->heads*2)-1;
    rdsk.loCylinder = 2;
    rdsk.hiCylinder = dev->cylinders-1;
    rdsk.cylBlocks  = dev->sectors*dev->heads;

    rdsk.cylinders = dev->cylinders;
    rdsk.sectors   = dev->sectors;
    rdsk.heads     = dev->heads;
	
    rdsk.badBlockList = -1;
    rdsk.partitionList = 1;
    rdsk.fileSysHdrList = 1 + dev->nVol;

    RETCODE rc = adfWriteRDSKblock ( dev, &rdsk );
    if ( rc != RC_OK )
        return rc;

    /* PART */

    j=1;
    for(i=0; i<dev->nVol; i++) {
        memset ( &part, 0, sizeof(struct AdfPARTblock) );

        if (i<dev->nVol-1)
            part.next = j+1;
        else
            part.next = -1;

        len = min ( (unsigned) ADF_MAX_NAME_LEN,
                    (unsigned) strlen ( partList[i]->volName ) );
        part.nameLen = (char) len;
        strncpy(part.name, partList[i]->volName, len);

        part.surfaces       = (int32_t) dev->heads;
        part.blocksPerTrack = (int32_t) dev->sectors;
        part.lowCyl = partList[i]->startCyl;
        part.highCyl = partList[i]->startCyl + partList[i]->lenCyl -1;
        memcpy ( part.dosType, "DOS", 3 );

        part.dosType[3] = partList[i]->volType & 0x01;

        rc = adfWritePARTblock ( dev, j, &part );
        if ( rc != RC_OK )
            return rc;
        j++;
    }

    /* FSHD */
    struct AdfFSHDblock fshd;
    memcpy ( fshd.dosType, "DOS", 3 );
    fshd.dosType[3] = (char) partList[0]->volType;
    fshd.next = -1;
    fshd.segListBlock = j+1;
    rc = adfWriteFSHDblock ( dev, j, &fshd );
    if ( rc != RC_OK )
        return rc;
    j++;
	
    /* LSEG */
    struct AdfLSEGblock lseg;
    lseg.next = -1;

    return adfWriteLSEGblock ( dev, j, &lseg );
}


/*
 * adfCreateHd
 *
 * create a filesystem one an harddisk device (partitions==volumes, and the header)
 *
 * fills dev->volList[]
 *
 */
RETCODE adfCreateHd ( struct AdfDevice * const               dev,
                      const unsigned                         n,
                      const struct Partition * const * const partList )
{
    unsigned i, j;

/*struct AdfVolume *vol;*/

    if ( dev == NULL || partList == NULL ) {
        (*adfEnv.eFct)("adfCreateHd : illegal parameter(s)");
        return RC_ERROR;
    }

    dev->volList = (struct AdfVolume **) malloc (
        sizeof(struct AdfVolume *) * n );
    if (!dev->volList) {
        (*adfEnv.eFct)("adfCreateFlop : malloc");
        return RC_MALLOC;
    }
    for(i=0; i<n; i++) {
        dev->volList[i] = adfVolCreate( dev,
					(uint32_t) partList[i]->startCyl,
					(uint32_t) partList[i]->lenCyl,
					partList[i]->volName, 
					partList[i]->volType );
        if (dev->volList[i]==NULL) {
           for(j=0; j<i; j++) {
               free( dev->volList[i] );
/* pas fini */
           }
           free(dev->volList);
           adfEnv.eFct ( "adfCreateHd : adfVolCreate() failed" );
        }
    }
    dev->nVol = (int) n;
/*
vol=dev->volList[0];
printf("0first=%ld last=%ld root=%ld\n",vol->firstBlock,
 vol->lastBlock, vol->rootBlock);
*/
    dev->mounted = true;

    return adfCreateHdHeader ( dev, (int) n, partList );
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
    dev->volList = (struct AdfVolume **) malloc (sizeof(struct AdfVolume *));
    if ( dev->volList == NULL ) {
        adfEnv.eFct ( "adfCreateHdFile : malloc" );
        return RC_ERROR;
    }

    dev->volList[0] = adfVolCreate( dev, 0L, dev->cylinders, volName, volType );
    if (dev->volList[0]==NULL) {
        free(dev->volList);
        return RC_ERROR;
    }

    dev->nVol = 1;
    dev->devType = DEVTYPE_HARDFILE;

    return RC_OK;
}


/*
 * ReadRDSKblock
 *
 */
RETCODE adfReadRDSKblock ( struct AdfDevice * const    dev,
                           struct AdfRSDKblock * const blk )
{
    uint8_t buf[256];

    RETCODE rc = adfDevReadBlock ( dev, 0, 256, buf );
    if ( rc != RC_OK )
       return rc;

    memcpy(blk, buf, 256);
#ifdef LITT_ENDIAN
    /* big to little = 68000 to x86 */
    adfSwapEndian ( (uint8_t *) blk, ADF_SWBL_RDSK );
#endif

    if ( strncmp(blk->id,"RDSK",4)!=0 ) {
        (*adfEnv.eFct)("ReadRDSKblock : RDSK id not found");
        return RC_ERROR;
    }

    if ( blk->size != 64 )
        (*adfEnv.wFct)("ReadRDSKBlock : size != 64");				/* BV */

    if ( blk->checksum != adfNormalSum(buf,8,256) ) {
         (*adfEnv.wFct)("ReadRDSKBlock : incorrect checksum");
         /* BV FIX: Due to malicious Win98 write to sector
         rc|=RC_BLOCKSUM;*/
    }
	
    if ( blk->blockSize != 512 )
         (*adfEnv.wFct)("ReadRDSKBlock : blockSize != 512");		/* BV */

    if ( blk->cylBlocks !=  blk->sectors*blk->heads )
        (*adfEnv.wFct)( "ReadRDSKBlock : cylBlocks != sectors*heads");

    return rc;
}


/*
 * adfWriteRDSKblock
 *
 */
RETCODE adfWriteRDSKblock ( struct AdfDevice * const    dev,
                            struct AdfRSDKblock * const rdsk )
{
    uint8_t buf[ADF_LOGICAL_BLOCK_SIZE];
    uint32_t newSum;

    if (dev->readOnly) {
        (*adfEnv.wFct)("adfWriteRDSKblock : can't write block, read only device");
        return RC_ERROR;
    }

    memset ( buf, 0, ADF_LOGICAL_BLOCK_SIZE );

    memcpy ( rdsk->id, "RDSK", 4 );
    rdsk->size = sizeof(struct AdfRSDKblock) / sizeof(int32_t);
    rdsk->blockSize = ADF_LOGICAL_BLOCK_SIZE;
    rdsk->badBlockList = -1;

    memcpy ( rdsk->diskVendor, "ADFlib  ", 8 );
    memcpy ( rdsk->diskProduct, "harddisk.adf    ", 16 );
    memcpy ( rdsk->diskRevision, "v1.0", 4 );

    memcpy ( buf, rdsk, sizeof(struct AdfRSDKblock) );
#ifdef LITT_ENDIAN
    adfSwapEndian ( buf, ADF_SWBL_RDSK );
#endif

    newSum = adfNormalSum ( buf, 8, ADF_LOGICAL_BLOCK_SIZE );
    swLong(buf+8, newSum);

    return adfDevWriteBlock ( dev, 0, ADF_LOGICAL_BLOCK_SIZE, buf );
}


/*
 * ReadPARTblock
 *
 */
RETCODE adfReadPARTblock ( struct AdfDevice * const    dev,
                           const int32_t               nSect,
                           struct AdfPARTblock * const blk )
{
    uint8_t buf[ sizeof(struct AdfPARTblock) ];

    RETCODE rc = adfDevReadBlock ( dev, (uint32_t) nSect,
                                   sizeof(struct AdfPARTblock), buf );
    if ( rc != RC_OK )
       return rc;

    memcpy ( blk, buf, sizeof(struct AdfPARTblock) );
#ifdef LITT_ENDIAN
    /* big to little = 68000 to x86 */
    adfSwapEndian ( (uint8_t *) blk, ADF_SWBL_PART);
#endif

    if ( strncmp(blk->id,"PART",4)!=0 ) {
    	(*adfEnv.eFct)("ReadPARTblock : PART id not found");
        return RC_ERROR;
    }

    if ( blk->size != 64 )
        (*adfEnv.wFct)("ReadPARTBlock : size != 64");

    if ( blk->blockSize!=128 ) {
    	(*adfEnv.eFct)("ReadPARTblock : blockSize!=512, not supported (yet)");
        return RC_ERROR;
    }

    if ( blk->checksum != adfNormalSum(buf,8,256) )
        (*adfEnv.wFct)( "ReadPARTBlock : incorrect checksum");

    return rc;
}


/*
 * adfWritePARTblock
 *
 */
RETCODE adfWritePARTblock ( struct AdfDevice * const    dev,
                            const int32_t               nSect,
                            struct AdfPARTblock * const part )
{
    uint8_t buf[ADF_LOGICAL_BLOCK_SIZE];
    uint32_t newSum;
	
    if (dev->readOnly) {
        (*adfEnv.wFct)("adfWritePARTblock : can't write block, read only device");
        return RC_ERROR;
    }

    memset ( buf, 0, ADF_LOGICAL_BLOCK_SIZE );

    memcpy ( part->id, "PART", 4 );
    part->size = sizeof(struct AdfPARTblock) / sizeof(int32_t);
    part->blockSize = ADF_LOGICAL_BLOCK_SIZE;
    part->vectorSize = 16;
    part->blockSize = 128;
    part->sectorsPerBlock = 1;
    part->dosReserved = 2;

    memcpy ( buf, part, sizeof(struct AdfPARTblock) );
#ifdef LITT_ENDIAN
    adfSwapEndian ( buf, ADF_SWBL_PART );
#endif

    newSum = adfNormalSum ( buf, 8, ADF_LOGICAL_BLOCK_SIZE );
    swLong(buf+8, newSum);
/*    *(int32_t*)(buf+8) = swapLong((uint8_t*)&newSum);*/

    return adfDevWriteBlock ( dev, (uint32_t) nSect, ADF_LOGICAL_BLOCK_SIZE, buf );
}

/*
 * ReadFSHDblock
 *
 */
RETCODE adfReadFSHDblock ( struct AdfDevice * const    dev,
                           const int32_t               nSect,
                           struct AdfFSHDblock * const blk )
{
    uint8_t buf[ sizeof(struct AdfFSHDblock) ];

    RETCODE rc = adfDevReadBlock ( dev, (uint32_t) nSect,
                                   sizeof(struct AdfFSHDblock), buf );
    if ( rc != RC_OK )
        return rc;
		
    memcpy ( blk, buf, sizeof(struct AdfFSHDblock) );
#ifdef LITT_ENDIAN
    /* big to little = 68000 to x86 */
    adfSwapEndian ( (uint8_t *) blk, ADF_SWBL_FSHD );
#endif

    if ( strncmp(blk->id,"FSHD",4)!=0 ) {
    	(*adfEnv.eFct)("ReadFSHDblock : FSHD id not found");
        return RC_ERROR;
    }

    if ( blk->size != 64 )
         (*adfEnv.wFct)("ReadFSHDblock : size != 64");

    if ( blk->checksum != adfNormalSum(buf,8,256) )
        (*adfEnv.wFct)( "ReadFSHDblock : incorrect checksum");

    return rc;
}


/*
 *  adfWriteFSHDblock
 *
 */
RETCODE adfWriteFSHDblock ( struct AdfDevice * const    dev,
                            const int32_t               nSect,
                            struct AdfFSHDblock * const fshd )
{
    uint8_t buf[ADF_LOGICAL_BLOCK_SIZE];
    uint32_t newSum;

    if (dev->readOnly) {
        (*adfEnv.wFct)("adfWriteFSHDblock : can't write block, read only device");
        return RC_ERROR;
    }

    memset ( buf, 0, ADF_LOGICAL_BLOCK_SIZE );

    memcpy ( fshd->id, "FSHD", 4 );
    fshd->size = sizeof(struct AdfFSHDblock) / sizeof(int32_t);

    memcpy ( buf, fshd, sizeof(struct AdfFSHDblock) );
#ifdef LITT_ENDIAN
    adfSwapEndian ( buf, ADF_SWBL_FSHD );
#endif

    newSum = adfNormalSum ( buf, 8, ADF_LOGICAL_BLOCK_SIZE );
    swLong(buf+8, newSum);
/*    *(int32_t*)(buf+8) = swapLong((uint8_t*)&newSum);*/

    return adfDevWriteBlock ( dev, (uint32_t) nSect, ADF_LOGICAL_BLOCK_SIZE, buf );
}


/*
 * ReadLSEGblock
 *
 */
RETCODE adfReadLSEGblock ( struct AdfDevice * const    dev,
                           const int32_t               nSect,
                           struct AdfLSEGblock * const blk )
{
    uint8_t buf[ sizeof(struct AdfLSEGblock) ];

    RETCODE rc = adfDevReadBlock ( dev, (uint32_t) nSect,
                                   sizeof(struct AdfLSEGblock), buf );
    if ( rc != RC_OK )
        return rc;
		
    memcpy ( blk, buf, sizeof(struct AdfLSEGblock) );
#ifdef LITT_ENDIAN
    /* big to little = 68000 to x86 */
    adfSwapEndian ( (uint8_t *) blk, ADF_SWBL_LSEG );
#endif

    if ( strncmp(blk->id,"LSEG",4)!=0 ) {
    	(*adfEnv.eFct)("ReadLSEGblock : LSEG id not found");
        return RC_ERROR;
    }

    if ( blk->checksum != adfNormalSum ( buf, 8, sizeof(struct AdfLSEGblock) ) )
        (*adfEnv.wFct)("ReadLSEGBlock : incorrect checksum");

    if ( blk->next!=-1 && blk->size != 128 )
        (*adfEnv.wFct)("ReadLSEGBlock : size != 128");

    return RC_OK;
}


/*
 * adfWriteLSEGblock
 *
 */
RETCODE adfWriteLSEGblock ( struct AdfDevice * const    dev,
                            const int32_t               nSect,
                            struct AdfLSEGblock * const lseg )
{
    uint8_t buf[ADF_LOGICAL_BLOCK_SIZE];
    uint32_t newSum;

    if (dev->readOnly) {
        (*adfEnv.wFct)("adfWriteLSEGblock : can't write block, read only device");
        return RC_ERROR;
    }

    memset ( buf, 0, ADF_LOGICAL_BLOCK_SIZE );

    memcpy ( lseg->id, "LSEG", 4 );
    lseg->size = sizeof(struct AdfLSEGblock) / sizeof(int32_t);

    memcpy ( buf, lseg, sizeof(struct AdfLSEGblock) );
#ifdef LITT_ENDIAN
    adfSwapEndian ( buf, ADF_SWBL_LSEG );
#endif

    newSum = adfNormalSum ( buf, 8, ADF_LOGICAL_BLOCK_SIZE );
    swLong(buf+8,newSum);
/*    *(int32_t*)(buf+8) = swapLong((uint8_t*)&newSum);*/

    return adfDevWriteBlock ( dev, (uint32_t) nSect, ADF_LOGICAL_BLOCK_SIZE, buf );
}

/*##########################################################################*/
