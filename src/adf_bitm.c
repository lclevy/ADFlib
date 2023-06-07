/*
 *  ADF Library. (C) 1997-2002 Laurent Clevy
 *
 *  adf_bitm.c
 *
 *  $Id$
 *
 *  bitmap code
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

#include "adf_bitm.h"

#include "adf_env.h"
#include "adf_raw.h"
#include "adf_util.h"
#include "defendian.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>


extern uint32_t bitMask[32];

static RETCODE adfBitmapAllocate ( struct AdfVolume * const vol );

static uint32_t nBlock2bitmapSize ( uint32_t nBlock )
{
    uint32_t mapSize = (uint32_t) nBlock / ( 127 * 32 );
    if ( ( nBlock % ( 127 * 32 ) ) != 0 )
        mapSize++;
    return mapSize;
}

/*
 * adfUpdateBitmap
 *
 */
RETCODE adfUpdateBitmap ( struct AdfVolume * const vol )
{
    struct bRootBlock root;

/*printf("adfUpdateBitmap\n");*/

    RETCODE rc = adfReadRootBlock ( vol, (uint32_t) vol->rootBlock, &root );
    if ( rc != RC_OK )
        return rc;

    root.bmFlag = BM_INVALID;

    rc = adfWriteRootBlock ( vol, (uint32_t) vol->rootBlock, &root );
    if ( rc != RC_OK )
        return rc;

    for ( unsigned i = 0 ; i < vol->bitmapSize ; i++ )
        if ( vol->bitmapBlocksChg[i] ) {
            rc = adfWriteBitmapBlock ( vol, vol->bitmapBlocks[i],
                                       vol->bitmapTable[i] );
            if ( rc != RC_OK )
                return rc;
            vol->bitmapBlocksChg[i] = FALSE;
    }

    root.bmFlag = BM_VALID;
    adfTime2AmigaTime(adfGiveCurrentTime(),&(root.days),&(root.mins),&(root.ticks));
    return adfWriteRootBlock ( vol, (uint32_t) vol->rootBlock, &root );
}


/*
 * adfCountFreeBlocks
 *
 */
uint32_t adfCountFreeBlocks ( const struct AdfVolume * const vol )
{
    int j;

    uint32_t freeBlocks = 0L;
    for(j=vol->firstBlock+2; j<=(vol->lastBlock - vol->firstBlock); j++)
        if ( adfIsBlockFree(vol,j) )
            freeBlocks++;

    return freeBlocks;
}


/*
 * adfReadBitmap
 *
 */
RETCODE adfReadBitmap ( struct AdfVolume * const        vol,
                        const uint32_t                  nBlock,
                        const struct bRootBlock * const root )
{
    uint32_t i, j;
    struct bBitmapExtBlock bmExt;

    vol->bitmapSize = nBlock2bitmapSize ( nBlock );
    RETCODE rc = adfBitmapAllocate ( vol );
    if ( rc != RC_OK )
        return rc;

    for ( unsigned i = 0 ; i < vol->bitmapSize ; i++ ) {
        vol->bitmapBlocksChg[i] = FALSE;
    }

    j=0; i=0;
    /* bitmap pointers in rootblock : 0 <= i <BM_SIZE */
    SECTNUM nSect;
    while ( i < BM_SIZE && root->bmPages[i] != 0 ) {
            vol->bitmapBlocks[j] = nSect = root->bmPages[i];
        if ( ! isSectNumValid ( vol, nSect ) ) {
            adfEnv.wFct ( "adfReadBitmap : sector %d out of range", nSect );
            // abort here?
        }

        rc = adfReadBitmapBlock ( vol, nSect, vol->bitmapTable[j] );
        if ( rc != RC_OK ) {
            adfFreeBitmap(vol);
            return rc;
        }
        j++; i++;
    }
    nSect = root->bmExt;
    while ( nSect != 0 ) {
        /* bitmap pointers in bitmapExtBlock, j <= mapSize */
        rc = adfReadBitmapExtBlock ( vol, nSect, &bmExt );
        if ( rc != RC_OK ) {
            adfFreeBitmap(vol);
            return rc;
        }
        i=0;
        while ( i < 127 && j < vol->bitmapSize ) {
            nSect = bmExt.bmPages[i];
            if ( ! isSectNumValid ( vol, nSect ) ) {
                adfEnv.wFct ( "adfReadBitmap : sector %d out of range", nSect );
                // abort here?
            }
            vol->bitmapBlocks[j] = nSect;

            rc = adfReadBitmapBlock ( vol, nSect, vol->bitmapTable[j] );
            if ( rc != RC_OK ) {
                adfFreeBitmap(vol);
                return rc;
            }
            i++; j++;
        }
        nSect = bmExt.nextBlock;
    }

    return rc;
}


/*
 * adfIsBlockFree
 *
 */
BOOL adfIsBlockFree ( const struct AdfVolume * const vol,
                      const SECTNUM nSect )
{
    assert ( nSect >= 2 );
    int sectOfMap = nSect-2;
    int block = sectOfMap/(127*32);
    int indexInMap = (sectOfMap/32)%127;

/*printf("sect=%d block=%d ind=%d,  ",sectOfMap,block,indexInMap);
printf("bit=%d,  ",sectOfMap%32);
printf("bitm=%x,  ",bitMask[ sectOfMap%32]);
printf("res=%x,  ",vol->bitmapTable[ block ]->map[ indexInMap ]
        & bitMask[ sectOfMap%32 ]);
*/
    return ( (vol->bitmapTable[ block ]->map[ indexInMap ]
        & bitMask[ sectOfMap%32 ])!=0 );
}


/*
 * adfSetBlockFree OK
 *
 */
void adfSetBlockFree ( struct AdfVolume * const vol,
                       const SECTNUM            nSect )
{
    uint32_t oldValue;
    int sectOfMap = nSect-2;
    int block = sectOfMap/(127*32);
    int indexInMap = (sectOfMap/32)%127;

/*printf("sect=%d block=%d ind=%d,  ",sectOfMap,block,indexInMap);
printf("bit=%d,  ",sectOfMap%32);
*printf("bitm=%x,  ",bitMask[ sectOfMap%32]);*/

    oldValue = vol->bitmapTable[ block ]->map[ indexInMap ];
/*printf("old=%x,  ",oldValue);*/
    vol->bitmapTable[ block ]->map[ indexInMap ]
	    = oldValue | bitMask[ sectOfMap%32 ];
/*printf("new=%x,  ",vol->bitmapTable[ block ]->map[ indexInMap ]);*/

    vol->bitmapBlocksChg[ block ] = TRUE;
}


/*
 * adfSetBlockUsed
 *
 */
void adfSetBlockUsed ( struct AdfVolume * const vol,
                       const SECTNUM            nSect )
{
    uint32_t oldValue;
    int sectOfMap = nSect-2;
    int block = sectOfMap/(127*32);
    int indexInMap = (sectOfMap/32)%127;

    oldValue = vol->bitmapTable[ block ]->map[ indexInMap ];

    vol->bitmapTable[ block ]->map[ indexInMap ]
	    = oldValue & (~bitMask[ sectOfMap%32 ]);
    vol->bitmapBlocksChg[ block ] = TRUE;
}


/*
 * adfGet1FreeBlock
 *
 */
SECTNUM adfGet1FreeBlock ( struct AdfVolume * const vol )
{
    SECTNUM block[1];
    if (!adfGetFreeBlocks(vol,1,block))
        return(-1);
    else
        return(block[0]);
}

/*
 * adfGetFreeBlocks
 *
 */
BOOL adfGetFreeBlocks ( struct AdfVolume * const vol,
                        const int                nbSect,
                        SECTNUM * const          sectList )
{
    int i, j;
    BOOL diskFull;
    int32_t block = vol->rootBlock;

    i = 0;
    diskFull = FALSE;
/*printf("lastblock=%ld\n",vol->lastBlock);*/
    while ( i < nbSect && !diskFull ) {
        if ( adfIsBlockFree(vol, block) ) {
            sectList[i] = block;
            i++;
        }
/*        if ( block==vol->lastBlock )
            block = vol->firstBlock+2;*/
        if ( ( block + vol->firstBlock ) == vol->lastBlock ) {
            block = 2;
        } else {
            block++;
            if ( block == vol->rootBlock )
                diskFull = TRUE;
        }
    }

    BOOL gotAllBlocks = ( i == nbSect );
    if ( gotAllBlocks )
        for(j=0; j<nbSect; j++)
            adfSetBlockUsed( vol, sectList[j] );

    return gotAllBlocks;
}


/*
 * adfCreateBitmap
 *
 * create bitmap structure in vol
 */
RETCODE adfCreateBitmap ( struct AdfVolume * const vol )
{
    SECTNUM nBlock = vol->lastBlock - vol->firstBlock + 1 - 2;

    vol->bitmapSize = nBlock2bitmapSize ( (uint32_t) nBlock );
    RETCODE rc = adfBitmapAllocate ( vol );
    if ( rc != RC_OK )
        return rc;

    for ( int i = vol->firstBlock + 2 ; i <= (vol->lastBlock - vol->firstBlock) ; i++ )
        adfSetBlockFree(vol, i);

    return rc;
}


/*
 * adfWriteNewBitmap
 *
 * write ext blocks and bitmap
 *
 * uses vol->bitmapSize, 
 */
RETCODE adfWriteNewBitmap ( struct AdfVolume * const vol )
{
    struct bBitmapExtBlock bitme;
    SECTNUM *bitExtBlock;
    SECTNUM *sectList;
    struct bRootBlock root;

    sectList = (SECTNUM *) malloc ( sizeof(SECTNUM) * (unsigned) vol->bitmapSize );
    if (!sectList) {
		(*adfEnv.eFct)("adfCreateBitmap : sectList");
        return RC_MALLOC;
    }

    if ( ! adfGetFreeBlocks ( vol, (int) vol->bitmapSize, sectList ) ) {
        free(sectList);
		return RC_ERROR;
    }
	
    if ( adfReadRootBlock ( vol, (uint32_t) vol->rootBlock, &root ) != RC_OK ) {
        free(sectList);
		return RC_ERROR;
    }

    unsigned n = min( vol->bitmapSize, (uint32_t) BM_SIZE );
    for ( unsigned i = 0 ; i < n ; i++ ) {
        root.bmPages[i] = vol->bitmapBlocks[i] = sectList[i];
    }
    unsigned nBlock = n;

    /* for devices with more than 25*127 blocks == hards disks */
    if (vol->bitmapSize>BM_SIZE) {

        unsigned nExtBlock = (vol->bitmapSize - BM_SIZE ) / 127;
        if ((vol->bitmapSize-BM_SIZE)%127)
            nExtBlock++;

        bitExtBlock = (SECTNUM *) malloc ( sizeof(SECTNUM) * (unsigned) nExtBlock );
        if (!bitExtBlock) {
            free(sectList);
			adfEnv.eFct("adfWriteNewBitmap : malloc failed");
            return RC_MALLOC;
        }

        if ( ! adfGetFreeBlocks ( vol, (int) nExtBlock, bitExtBlock ) ) {
           free(sectList); free(bitExtBlock);
           return RC_MALLOC;
        }

        unsigned k = 0;
        root.bmExt = bitExtBlock[ k ];
        while( nBlock<vol->bitmapSize ) {
            int i = 0;
            while( i<127 && nBlock<vol->bitmapSize ) {
                bitme.bmPages[i] = vol->bitmapBlocks[nBlock] = sectList[i];
                i++;
                nBlock++;
            }
            if ( k + 1 < nExtBlock )
                bitme.nextBlock = bitExtBlock[ k+1 ];
            else
                bitme.nextBlock = 0;
            if (adfWriteBitmapExtBlock(vol, bitExtBlock[ k ], &bitme)!=RC_OK) {
                free(sectList); free(bitExtBlock);
				return RC_ERROR;
            }
            k++;
        }
        free( bitExtBlock );

    }
    free( sectList);

    if ( adfWriteRootBlock ( vol, (uint32_t) vol->rootBlock, &root ) != RC_OK )
        return RC_ERROR;
    
    return RC_OK;
}

/*
 * adfReadBitmapBlock
 *
 * ENDIAN DEPENDENT
 */
RETCODE adfReadBitmapBlock ( struct AdfVolume *    vol,
                             SECTNUM               nSect,
                             struct bBitmapBlock * bitm )
{
    uint8_t buf[LOGICAL_BLOCK_SIZE];

/*printf("bitmap %ld\n",nSect);*/
    if ( adfReadBlock ( vol, (uint32_t) nSect, buf ) != RC_OK )
        return RC_ERROR;

    memcpy ( bitm, buf, LOGICAL_BLOCK_SIZE );
#ifdef LITT_ENDIAN
    /* big to little = 68000 to x86 */
    swapEndian((uint8_t*)bitm, SWBL_BITMAP);
#endif

    if ( bitm->checkSum != adfNormalSum ( buf, 0, LOGICAL_BLOCK_SIZE ) )
        adfEnv.wFct("adfReadBitmapBlock : invalid checksum");

    return RC_OK;
}


/*
 * adfWriteBitmapBlock
 *
 * OK
 */
RETCODE adfWriteBitmapBlock ( struct AdfVolume * const          vol,
                              const SECTNUM                     nSect,
                              const struct bBitmapBlock * const bitm )
{
    uint8_t buf[LOGICAL_BLOCK_SIZE];
	uint32_t newSum;
	
	memcpy(buf,bitm,LOGICAL_BLOCK_SIZE);
#ifdef LITT_ENDIAN
    /* little to big */
    swapEndian(buf, SWBL_BITMAP);
#endif

	newSum = adfNormalSum(buf, 0, LOGICAL_BLOCK_SIZE);
    swLong(buf,newSum);

/*	dumpBlock((uint8_t*)buf);*/
    if ( adfWriteBlock ( vol, (uint32_t) nSect, buf ) != RC_OK )
		return RC_ERROR;

    return RC_OK;
}


/*
 * adfReadBitmapExtBlock
 *
 * ENDIAN DEPENDENT
 */
RETCODE adfReadBitmapExtBlock ( struct AdfVolume * const       vol,
                                const SECTNUM                  nSect,
                                struct bBitmapExtBlock * const bitme )
{
    uint8_t buf[LOGICAL_BLOCK_SIZE];

    if ( adfReadBlock ( vol, (uint32_t) nSect, buf ) != RC_OK )
        return RC_ERROR;

    memcpy ( bitme, buf, LOGICAL_BLOCK_SIZE );
#ifdef LITT_ENDIAN
    swapEndian((uint8_t*)bitme, SWBL_BITMAP);
#endif

    return RC_OK;
}


/*
 * adfWriteBitmapExtBlock
 *
 */
RETCODE adfWriteBitmapExtBlock ( struct AdfVolume * const             vol,
                                 const SECTNUM                        nSect,
                                 const struct bBitmapExtBlock * const bitme )
{
    uint8_t buf[LOGICAL_BLOCK_SIZE];
	
    memcpy ( buf, bitme, LOGICAL_BLOCK_SIZE );
#ifdef LITT_ENDIAN
    /* little to big */
    swapEndian(buf, SWBL_BITMAPE);
#endif

/*	dumpBlock((uint8_t*)buf);*/
    if ( adfWriteBlock ( vol, (uint32_t) nSect, buf ) != RC_OK )
        return RC_ERROR;

    return RC_OK;
}


/*
 * adfFreeBitmap
 *
 */
void adfFreeBitmap ( struct AdfVolume * const vol )
{
    for( unsigned i = 0 ; i < vol->bitmapSize ; i++ )
        free(vol->bitmapTable[i]);
    vol->bitmapSize = 0;

    free(vol->bitmapTable);
	vol->bitmapTable = 0;

    free(vol->bitmapBlocks);
	vol->bitmapBlocks = 0;

    free(vol->bitmapBlocksChg);
	vol->bitmapBlocksChg = 0;
}


/*#######################################################################################*/

/*
 * adfBitmapAllocate
 *
 * vol->bitmapSize must be set properly before calling
 */
static RETCODE adfBitmapAllocate ( struct AdfVolume * const vol )
{
    vol->bitmapTable = (struct bBitmapBlock**)
        malloc ( sizeof(struct bBitmapBlock*) * vol->bitmapSize );
    if ( vol->bitmapTable == NULL ) {
        adfEnv.eFct("adfBitmapAllocate : malloc, vol->bitmapTable");
        return RC_MALLOC;
    }

    vol->bitmapBlocks = (SECTNUM*) malloc ( sizeof(SECTNUM) * vol->bitmapSize );
    if ( vol->bitmapBlocks == NULL ) {
        free ( vol->bitmapTable );
        adfEnv.eFct("adfBitmapAllocate : malloc, vol->bitmapBlocks");
        return RC_MALLOC;
    }

    vol->bitmapBlocksChg = (BOOL*) malloc ( sizeof(BOOL) * vol->bitmapSize );
    if ( vol->bitmapBlocksChg == NULL ) {
        free ( vol->bitmapTable );
        free ( vol->bitmapBlocks );
        adfEnv.eFct("adfBitmapAllocate : malloc, vol->bitmapBlocksChg");
        return RC_MALLOC;
    }

    for ( unsigned i = 0 ; i < vol->bitmapSize ; i++ ) {
        vol->bitmapTable[i] = (struct bBitmapBlock*)
            malloc ( sizeof(struct bBitmapBlock) );

        if ( vol->bitmapTable[i] == NULL) {
            free ( vol->bitmapBlocksChg );
            free ( vol->bitmapBlocks );
            for ( unsigned j = 0 ; j < i ; j++ )
                free ( vol->bitmapTable[j] );
            free ( vol->bitmapTable );
            adfEnv.eFct("adfBitmapAllocate : malloc");
            return RC_MALLOC;
        }
    }
    return RC_OK;
}

/*
static void adfBitmapRelease ( struct AdfVolume * const vol )
{
    if ( vol->bitmapTable != NULL ) {
        for ( unsigned j = 0 ; j < i; j++ )
                free(vol->bitmapTable[j]);
        free(vol->bitmapTable);
    }
    free(vol->bitmapBlocks);
    free(vol->bitmapBlocksChg);
}
*/
