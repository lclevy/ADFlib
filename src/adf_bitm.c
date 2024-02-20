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
 *  along with ADFLib; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "adf_bitm.h"

#include "adf_cache.h"
#include "adf_dir.h"
#include "adf_env.h"
#include "adf_file_block.h"
#include "adf_raw.h"
#include "adf_util.h"
#include "defendian.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>


#define CHECK_NONZERO_BMPAGES_BEYOND_BMSIZE 0

extern uint32_t bitMask[32];

static RETCODE adfBitmapListSetUsed ( struct AdfVolume * const     vol,
                                      const struct AdfList * const list );

static RETCODE adfBitmapFileBlocksSetUsed (
    struct AdfVolume * const              vol,
    const struct bFileHeaderBlock * const fhBlock );

static RETCODE adfBitmapDirCacheSetUsed ( struct AdfVolume * const vol,
                                          SECTNUM                  dCacheBlockNum );


static uint32_t nBlock2bitmapSize ( uint32_t nBlock )
{
    uint32_t mapSize = (uint32_t) nBlock / ( BM_MAP_SIZE * 32 );
    if ( ( nBlock % ( BM_MAP_SIZE * 32 ) ) != 0 )
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

    for ( unsigned i = 0 ; i < vol->bitmap.size ; i++ )
        if ( vol->bitmap.blocksChg[i] ) {
            rc = adfWriteBitmapBlock ( vol, vol->bitmap.blocks[i],
                                       vol->bitmap.table[i] );
            if ( rc != RC_OK )
                return rc;
            vol->bitmap.blocksChg[i] = FALSE;
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
                        const struct bRootBlock * const root )
{
    RETCODE rc = RC_OK;

    for ( unsigned i = 0 ; i < vol->bitmap.size ; i++ ) {
        vol->bitmap.blocksChg[i] = FALSE;
    }

    uint32_t j = 0,
             i = 0;
    /* bitmap pointers in rootblock : 0 <= i < BM_PAGES_ROOT_SIZE */
    SECTNUM bmSect;
    while ( i < vol->bitmap.size &&
            i < BM_PAGES_ROOT_SIZE &&
            root->bmPages[i] != 0 )
    {
        vol->bitmap.blocks[j] = bmSect = root->bmPages[i];
        if ( ! isSectNumValid ( vol, bmSect ) ) {
            adfEnv.wFct ( "adfReadBitmap: sector %d out of range, root bm[%u]",
                          bmSect, i );
            adfFreeBitmap ( vol );
            return RC_ERROR;
        }

        rc = adfReadBitmapBlock ( vol, bmSect, vol->bitmap.table[j] );
        if ( rc != RC_OK ) {
            adfFreeBitmap(vol);
            return rc;
        }
        j++;
        i++;
    }

    /* state validity checks */
    assert ( i <= BM_PAGES_ROOT_SIZE );

    assert ( ( i < vol->bitmap.size &&
               root->bmPages[i] != 0 ) ||
             ( i == vol->bitmap.size ) );

    // some images fail on this, https://github.com/lclevy/ADFlib/issues/63
    //   assert ( ( i == vol->bitmap.size && root->bmPages[i] == 0 ) ||
    //            ( i < vol->bitmap.size ) );

    assert ( ( i < vol->bitmap.size &&
               root->bmExt != 0 ) ||
             ( i == vol->bitmap.size ) );

    assert ( ( i == vol->bitmap.size &&
               root->bmExt == 0 ) ||
             ( i < vol->bitmap.size ) );

    if  ( i < vol->bitmap.size  &&  root->bmPages[i] == 0 ) {
        adfEnv.eFct ( "adfReadBitmap: root bmpages[%u] == 0, "
                      "but vol. %s should have %u bm sectors",
                      i, vol->volName, vol->bitmap.size );
        adfFreeBitmap ( vol );
        return RC_ERROR;
    }

    if ( i < vol->bitmap.size  &&  root->bmExt == 0 ) {
        adfEnv.eFct ( "adfReadBitmap: read %u of %u from root bm sectors of "
                      "%u total to read, but root->bmExt is 0",
                      i, BM_PAGES_ROOT_SIZE, vol->bitmap.size );
        adfFreeBitmap ( vol );
        return RC_ERROR;
    }

    /* check for erratic (?) (non-zero) entries in bmpages beyond the expected size,
       more info:  https://github.com/lclevy/ADFlib/issues/63 */
#if CHECK_NONZERO_BMPAGES_BEYOND_BMSIZE == 1
    for ( uint32_t i2 = i ; i2 < BM_PAGES_ROOT_SIZE ; i2++ ) {
        if ( root->bmPages[i2] != 0 )
            adfEnv.wFct ( "adfReadBitmap: a non-zero (%u, 0x%02x) entry in rootblock "
                          "bmpage[%u] in a volume with bmpage size %d",
                          root->bmPages[i2],
                          root->bmPages[i2],
                          i2, vol->bitmap.size );
    }
#endif

    if ( root->bmExt == 0 )
        return rc;

    struct bBitmapExtBlock bmExt;
    SECTNUM bmExtSect = root->bmExt;
#if CHECK_NONZERO_BMPAGES_BEYOND_BMSIZE == 1
    unsigned bmExt_i = 0;
#endif
    while ( bmExtSect != 0 ) {
        /* bitmap pointers in bitmapExtBlock, j <= mapSize */
        rc = adfReadBitmapExtBlock ( vol, bmExtSect, &bmExt );
        if ( rc != RC_OK ) {
            adfFreeBitmap(vol);
            return rc;
        }
        i=0;
        while ( i < BM_PAGES_EXT_SIZE && j < vol->bitmap.size ) {
            bmSect = bmExt.bmPages[i];
            if ( ! isSectNumValid ( vol, bmSect ) ) {
                adfEnv.wFct ( "adfReadBitmap : sector %d out of range, "
                              "bmext %d bmpages[%u]", bmSect, bmExtSect, i );
                return RC_ERROR;
            }
            vol->bitmap.blocks[j] = bmSect;

            rc = adfReadBitmapBlock ( vol, bmSect, vol->bitmap.table[j] );
            if ( rc != RC_OK ) {
                adfFreeBitmap(vol);
                return rc;
            }
            i++; j++;
        }

        /* check for erratic (?) (non-zero) entries in bmpages beyond
           the expected size,
           more info:  https://github.com/lclevy/ADFlib/issues/63 */
#if CHECK_NONZERO_BMPAGES_BEYOND_BMSIZE == 1
        for ( uint32_t i2 = i ; i2 < BM_PAGES_EXT_SIZE ; i2++ ) {
            if ( bmExt.bmPages[i2] != 0 )
                adfEnv.wFct (
                    "adfReadBitmap: a non-zero (%u, 0x%02x) entry in bm ext. %u (%d) "
                    "bmext.bmpage[%u] (bmpage %u), volume bm blocks %d",
                    bmExt.bmPages[i2],
                    bmExt.bmPages[i2],
                    bmExt_i, bmExtSect,
                    i2,
                    i2 + BM_PAGES_ROOT_SIZE + bmExt_i * BM_PAGES_EXT_SIZE,
                    vol->bitmap.size );
        }
        bmExt_i++;
#endif

        bmExtSect = bmExt.nextBlock;
    }

    return rc;
}


/*
 * adfRecreateBitmap
 *
 */
RETCODE adfReconstructBitmap ( struct AdfVolume * const        vol,
                               const struct bRootBlock * const root )
{
    RETCODE rc = RC_OK;

    // all bitmap blocks are to update (to improve/optimize, ie. compare with existing)
    for ( unsigned i = 0 ; i < vol->bitmap.size ; i++ ) {
        vol->bitmap.blocksChg[i] = TRUE;
    }

    uint32_t i = 0,
             j = 0;
    /* bitmap pointers in rootblock : 0 <= i < BM_PAGES_ROOT_SIZE */
    SECTNUM bmSect;
    while ( i < vol->bitmap.size &&
            i < BM_PAGES_ROOT_SIZE &&
            root->bmPages[i] != 0 )
    {
        vol->bitmap.blocks[j] = bmSect = root->bmPages[i];
        if ( ! isSectNumValid ( vol, bmSect ) ) {
            adfEnv.wFct ( "adfReconstructBitmap: sector %d out of range, root bm[%u]",
                          bmSect, i );
            adfFreeBitmap ( vol );
            return RC_ERROR;
        }

        //rc = adfReadBitmapBlock ( vol, nSect, vol->bitmapTable[j] );
        //if ( rc != RC_OK ) {
        //    adfFreeBitmap(vol);
        //    return rc;
        //}
        j++;
        i++;
    }

    /* state validity checks */
    assert ( i <= BM_PAGES_ROOT_SIZE );

    assert ( ( i < vol->bitmap.size &&
               root->bmPages[i] != 0 ) ||
             ( i == vol->bitmap.size ) );

    // some images fail on this, https://github.com/lclevy/ADFlib/issues/63
    //   assert ( ( i == vol->bitmap.size && root->bmPages[i] == 0 ) ||
    //            ( i < vol->bitmap.size ) );

    assert ( ( i < vol->bitmap.size &&
               root->bmExt != 0 ) ||
             ( i == vol->bitmap.size ) );

    assert ( ( i == vol->bitmap.size &&
               root->bmExt == 0 ) ||
             ( i < vol->bitmap.size ) );

    if  ( i < vol->bitmap.size  &&  root->bmPages[i] == 0 ) {
        adfEnv.eFct ( "adfReconstructBitmap: root bmpages[%u] == 0, "
                      "but vol. %s should have %u bm sectors",
                      i, vol->volName, vol->bitmap.size );
        adfFreeBitmap ( vol );
        return RC_ERROR;
    }

    if ( i < vol->bitmap.size  &&  root->bmExt == 0 ) {
        adfEnv.eFct ( "adfReconstructBitmap: read %u of %u from root bm sectors of "
                      "%u total to read, but root->bmExt is 0",
                      i, BM_PAGES_ROOT_SIZE, vol->bitmap.size );
        adfFreeBitmap ( vol );
        return RC_ERROR;
    }

    /* check for erratic (?) (non-zero) entries in bmpages beyond the expected size,
       more info:  https://github.com/lclevy/ADFlib/issues/63 */
#if CHECK_NONZERO_BMPAGES_BEYOND_BMSIZE == 1
    for ( uint32_t i2 = i ; i2 < BM_PAGES_ROOT_SIZE ; i2++ ) {
        if ( root->bmPages[i2] != 0 )
            adfEnv.wFct ( "adfReconstructBitmap: a non-zero (%u, 0x%02x) entry in rootblock "
                          "bmpage[%u] in a volume with bmpage size %d",
                          root->bmPages[i2],
                          root->bmPages[i2],
                          i2, vol->bitmap.size );
    }
    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    //     -> REPAIR INSTEAD (?) -> JUST SET TO 0
    //
#endif

    SECTNUM bmExtSect = root->bmExt;
#if CHECK_NONZERO_BMPAGES_BEYOND_BMSIZE == 1
    unsigned bmExt_i = 0;
#endif
    while ( bmExtSect != 0 ) {
        struct bBitmapExtBlock bmExtBlock;

        /* bitmap pointers in bitmapExtBlock, j <= mapSize */
        rc = adfReadBitmapExtBlock ( vol, bmExtSect, &bmExtBlock );
        if ( rc != RC_OK ) {
            adfFreeBitmap(vol);
            return rc;
        }
        i=0;
        while ( i < BM_PAGES_EXT_SIZE && j < vol->bitmap.size ) {
            SECTNUM bmBlkPtr = bmExtBlock.bmPages[i];
            if ( ! isSectNumValid ( vol, bmBlkPtr ) ) {
                adfEnv.eFct ( "adfReconstructBitmap : sector %d out of range, "
                              "bmext %d bmpages[%u]", bmBlkPtr, bmExtSect, i );
                adfFreeBitmap ( vol );
                return RC_ERROR;
            }
            vol->bitmap.blocks[j] = bmBlkPtr;

            //rc = adfReadBitmapBlock ( vol, nSect, vol->bitmapTable[j] );
            //if ( rc != RC_OK ) {
            //    adfFreeBitmap(vol);
            //    return rc;
            //}
            i++; j++;
        }

        /* check for erratic (?) (non-zero) entries in bmpages beyond the expected size,
           more info:  https://github.com/lclevy/ADFlib/issues/63 */
#if CHECK_NONZERO_BMPAGES_BEYOND_BMSIZE == 1
        for ( uint32_t i2 = i ; i2 < BM_PAGES_EXT_SIZE ; i2++ ) {
            if ( bmExtBlock.bmPages[i2] != 0 )
                adfEnv.wFct (
                    "adfReconstructBitmap: a non-zero (%u, 0x%02x) entry in bm ext. %u (%d) "
                    "bmext.bmpage[%u] (bmpage %u), volume bmpage blocks %d",
                    bmExtBlock.bmPages[i2],
                    bmExtBlock.bmPages[i2],
                    bmExt_i, bmExtSect,
                    i2,
                    i2 + BM_PAGES_ROOT_SIZE + bmExt_i * BM_PAGES_EXT_SIZE,
                    vol->bitmap.size );
        }
        bmExt_i++;
#endif

        bmExtSect = bmExtBlock.nextBlock;
    }

    // initially - set all free
    for ( int32_t i = 2 ;
          i < vol->lastBlock - vol->firstBlock ;   // CHECK UPPER COND
          i++ )
    {
        adfSetBlockFree(vol, i);
    }

    // then - mark all used blocks

    // mark rootblock
    adfSetBlockUsed(vol, vol->rootBlock);

    // mark bitmap blocks (all - from rootblock and bitmap ext. blocks)
    for ( uint32_t i = 0 ; i < vol->bitmap.size ; i++ ) {
        //if ( vol->bitmapBlocks[i] > 0 )  -- this should always be TRUE
        adfSetBlockUsed ( vol, vol->bitmap.blocks[i] );
    }

    // traverse all files and directories

    struct bDirBlock //bEntryBlock
        rootDirBlock;
    rc = adfReadEntryBlock ( vol, vol->rootBlock,
                             ( struct bEntryBlock * ) &rootDirBlock );
    if ( rc != RC_OK ) {
        adfEnv.eFct ( "Error reading directory entry block (%d)\n",
                      vol->rootBlock );
        return rc;
    }

    if ( ! isDirEmpty ( (const struct bDirBlock * const) &rootDirBlock) ) {
        // note: for a large volume (a hard disk) getting all entries can become big
        // - it may need to be optimized
        struct AdfList * const entries = adfGetRDirEnt ( vol, vol->rootBlock, TRUE );
        if ( entries == NULL ) {
            return RC_ERROR;
        }

        adfBitmapListSetUsed ( vol, entries );
        adfFreeDirList ( entries );
    }

    // directory cache blocks? (TO CHECK!)
    //  -> done above (in adfBitmapListSetUsed() )

    // any other blocks to check????

    return rc;
}


static RETCODE adfBitmapListSetUsed ( struct AdfVolume * const     vol,
                                      const struct AdfList * const list )
{
    RETCODE rc = RC_OK;

    const struct AdfList * cell = list;
    while ( cell != NULL ) {
        const struct AdfEntry * const entry =
            (const struct AdfEntry * const) cell->content;

        // mark entry block
        // (all header blocks (file, dir, links) are done with this)
        adfSetBlockUsed ( vol, entry->sector );

        // mark file blocks
        if ( entry->type == ST_FILE ) {
            struct bFileHeaderBlock fhBlock;
            RETCODE rc2 = adfReadEntryBlock ( vol, entry->sector,
                                              (struct bEntryBlock *) &fhBlock );
            if ( rc2 != RC_OK ) {
                //showerror ?
                rc = rc2;
                break;
            }

            rc2 = adfBitmapFileBlocksSetUsed ( vol, &fhBlock );
            if ( rc2 != RC_OK ) {
                //showerror ?
                rc = rc2;
                break;
            }
        }

        // mark directory and directory cache blocks
        else if ( entry->type == ST_DIR ) {
            struct bDirBlock dirBlock;
            RETCODE rc = adfReadEntryBlock ( vol, entry->sector,
                                             (struct bEntryBlock *) &dirBlock );
            if ( rc != RC_OK )
                return rc;
            rc = adfBitmapDirCacheSetUsed ( vol, dirBlock.extension );
        }

        // if any subdirectory present - process recursively
        if ( cell->subdir != NULL ) {
            RETCODE rc2 = adfBitmapListSetUsed ( vol,
                ( const struct AdfList * const ) cell->subdir );
            if ( rc2 != RC_OK )
                rc = rc2;
        }
        cell = cell->next;
    }
    return rc;
}


static RETCODE adfBitmapFileBlocksSetUsed (
    struct AdfVolume * const              vol,
    const struct bFileHeaderBlock * const fhBlock )
{
    // mark blocks from the header
    for ( uint32_t block = 0 ; block < MAX_DATABLK ; block++ ) {
        if ( fhBlock->dataBlocks[block] > 1 )
            adfSetBlockUsed ( vol, fhBlock->dataBlocks[block] );
    }

    // mark blocks from ext blocks

    //const int nExtBlocks = (int) adfFileSize2Extblocks ( fhBlock->byteSize,
    //                                                     vol->datablockSize );
    SECTNUM extBlockPtr = fhBlock->extension;
    struct bFileExtBlock fext;
    while ( extBlockPtr != 0 ) {
        adfSetBlockUsed ( vol, extBlockPtr );
        if ( adfReadFileExtBlock ( vol, extBlockPtr, &fext ) != RC_OK ) {
            adfEnv.eFct ( "adfBitmapMarkFileBlocks: "
                          "error reading ext block %d, file '%s'",
                          extBlockPtr, fhBlock->fileName );
            return RC_BLOCKREAD;
        }
        for ( uint32_t block = 0 ; block < MAX_DATABLK ; block++ ) {
            if ( fext.dataBlocks[block] > 1 )
                adfSetBlockUsed ( vol, fext.dataBlocks[block] );
        }
        extBlockPtr = fext.extension;
    }
    return RC_OK;
}


static RETCODE adfBitmapDirCacheSetUsed ( struct AdfVolume * const vol,
                                          SECTNUM                  dCacheBlockNum )
{
    RETCODE rc = RC_OK;
    while ( dCacheBlockNum != 0 ) {
        adfSetBlockUsed ( vol, dCacheBlockNum );

        struct bDirCacheBlock dirCacheBlock;
        rc = adfReadDirCBlock ( vol, dCacheBlockNum, &dirCacheBlock );
        if ( rc != RC_OK )
            break;

        dCacheBlockNum = dirCacheBlock.nextDirC;
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
    int block      = sectOfMap / ( BM_MAP_SIZE * 32 );
    int indexInMap = ( sectOfMap / 32 ) % BM_MAP_SIZE;

/*printf("sect=%d block=%d ind=%d,  ",sectOfMap,block,indexInMap);
printf("bit=%d,  ",sectOfMap%32);
printf("bitm=%x,  ",bitMask[ sectOfMap%32]);
printf("res=%x,  ",vol->bitmapTable[ block ]->map[ indexInMap ]
        & bitMask[ sectOfMap%32 ]);
*/
    return ( (vol->bitmap.table[ block ]->map[ indexInMap ]
        & bitMask[ sectOfMap%32 ])!=0 );
}


/*
 * adfSetBlockFree OK
 *
 */
void adfSetBlockFree ( struct AdfVolume * const vol,
                       const SECTNUM            nSect )
{
    assert ( nSect >= 2 );
    assert ( nSect <= vol->lastBlock - vol->firstBlock );

    uint32_t oldValue;
    int sectOfMap = nSect-2;
    int block      = sectOfMap / ( BM_MAP_SIZE * 32 );
    int indexInMap = ( sectOfMap / 32 ) % BM_MAP_SIZE;

/*printf("sect=%d block=%d ind=%d,  ",sectOfMap,block,indexInMap);
printf("bit=%d,  ",sectOfMap%32);
*printf("bitm=%x,  ",bitMask[ sectOfMap%32]);*/

    oldValue = vol->bitmap.table[ block ]->map[ indexInMap ];
/*printf("old=%x,  ",oldValue);*/
    vol->bitmap.table[ block ]->map[ indexInMap ]
	    = oldValue | bitMask[ sectOfMap%32 ];
/*printf("new=%x,  ",vol->bitmapTable[ block ]->map[ indexInMap ]);*/

    vol->bitmap.blocksChg[ block ] = TRUE;
}


/*
 * adfSetBlockUsed
 *
 */
void adfSetBlockUsed ( struct AdfVolume * const vol,
                       const SECTNUM            nSect )
{
    assert ( nSect >= 2 );
    assert ( nSect <= vol->lastBlock - vol->firstBlock );

    uint32_t oldValue;
    int sectOfMap = nSect-2;
    int block      = sectOfMap / ( BM_MAP_SIZE * 32 );
    int indexInMap = ( sectOfMap / 32 ) % BM_MAP_SIZE;

    oldValue = vol->bitmap.table[ block ]->map[ indexInMap ];

    vol->bitmap.table[ block ]->map[ indexInMap ]
	    = oldValue & (~bitMask[ sectOfMap%32 ]);
    vol->bitmap.blocksChg[ block ] = TRUE;
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
    SECTNUM nBlock = (SECTNUM) adfVolGetBlockNumWithoutBootblock ( vol );

    vol->bitmap.size = nBlock2bitmapSize ( (uint32_t) nBlock );
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

    sectList = (SECTNUM *) malloc ( sizeof(SECTNUM) * (unsigned) vol->bitmap.size );
    if (!sectList) {
		(*adfEnv.eFct)("adfCreateBitmap : sectList");
        return RC_MALLOC;
    }

    if ( ! adfGetFreeBlocks ( vol, (int) vol->bitmap.size, sectList ) ) {
        free(sectList);
        return RC_VOLFULL;
    }

    RETCODE rc = adfReadRootBlock ( vol, (uint32_t) vol->rootBlock, &root );
    if ( rc != RC_OK ) {
        free(sectList);
        return rc;
    }

    unsigned n = min( vol->bitmap.size, (uint32_t) BM_PAGES_ROOT_SIZE );
    for ( unsigned i = 0 ; i < n ; i++ ) {
        root.bmPages[i] = vol->bitmap.blocks[i] = sectList[i];
    }
    unsigned nBlock = n;

    /* for devices with more than 25 * BM_MAP_SIZE(127) blocks == hards disks */
    if ( vol->bitmap.size > BM_PAGES_ROOT_SIZE ) {

        unsigned nExtBlock = (vol->bitmap.size - BM_PAGES_ROOT_SIZE ) / BM_MAP_SIZE;
        if ( ( vol->bitmap.size - BM_PAGES_ROOT_SIZE ) % BM_MAP_SIZE )
            nExtBlock++;

        bitExtBlock = (SECTNUM *) malloc ( sizeof(SECTNUM) * (unsigned) nExtBlock );
        if (!bitExtBlock) {
            free(sectList);
			adfEnv.eFct("adfWriteNewBitmap : malloc failed");
            return RC_MALLOC;
        }

        if ( ! adfGetFreeBlocks ( vol, (int) nExtBlock, bitExtBlock ) ) {
           free(sectList); free(bitExtBlock);
           return RC_VOLFULL;
        }

        unsigned k = 0;
        root.bmExt = bitExtBlock[ k ];
        while( nBlock<vol->bitmap.size ) {
            int i = 0;
            while( i < BM_PAGES_EXT_SIZE && nBlock < vol->bitmap.size ) {
                bitme.bmPages[i] = vol->bitmap.blocks[nBlock] = sectList[i];
                i++;
                nBlock++;
            }
            if ( k + 1 < nExtBlock )
                bitme.nextBlock = bitExtBlock[ k+1 ];
            else
                bitme.nextBlock = 0;

            rc = adfWriteBitmapExtBlock ( vol, bitExtBlock[ k ], &bitme );
            if ( rc != RC_OK ) {
                free(sectList); free(bitExtBlock);
                return rc;
            }

            k++;
        }
        free( bitExtBlock );

    }
    free( sectList);

    return adfWriteRootBlock ( vol, (uint32_t) vol->rootBlock, &root );
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
    RETCODE rc = adfReadBlock ( vol, (uint32_t) nSect, buf );
    if ( rc != RC_OK )
        return rc;

    memcpy ( bitm, buf, LOGICAL_BLOCK_SIZE );
#ifdef LITT_ENDIAN
    /* big to little = 68000 to x86 */
    swapEndian((uint8_t*)bitm, SWBL_BITMAP);
#endif

    if ( bitm->checkSum != adfNormalSum ( buf, 0, LOGICAL_BLOCK_SIZE ) ) {
        adfEnv.wFct ( "adfReadBitmapBlock : invalid checksum, volume '%s', block %u",
                      vol->volName, nSect );
        // return error here?
    }

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

    memcpy ( buf, bitm, LOGICAL_BLOCK_SIZE );

#ifdef LITT_ENDIAN
    /* little to big */
    swapEndian ( buf, SWBL_BITMAP );
#endif

    uint32_t newSum = adfNormalSum ( buf, 0, LOGICAL_BLOCK_SIZE );
    swLong ( buf, newSum );

/*	dumpBlock((uint8_t*)buf);*/

    return adfWriteBlock ( vol, (uint32_t) nSect, buf );
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

    RETCODE rc = adfReadBlock ( vol, (uint32_t) nSect, buf );
    if ( rc != RC_OK )
        return rc;

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
    return adfWriteBlock ( vol, (uint32_t) nSect, buf );
}


/*
 * adfFreeBitmap
 *
 */
void adfFreeBitmap ( struct AdfVolume * const vol )
{
    for ( unsigned i = 0 ; i < vol->bitmap.size ; i++ )
        free ( vol->bitmap.table[i] );
    vol->bitmap.size = 0;

    free ( vol->bitmap.table );
    vol->bitmap.table = NULL;

    free ( vol->bitmap.blocks );
    vol->bitmap.blocks = NULL;

    free ( vol->bitmap.blocksChg );
    vol->bitmap.blocksChg = NULL;
}


/*#######################################################################################*/

/*
 * adfBitmapAllocate
 *
 * vol->bitmapSize must be set properly before calling
 */
RETCODE adfBitmapAllocate ( struct AdfVolume * const vol )
{
    vol->bitmap.size = nBlock2bitmapSize (
        adfVolGetBlockNumWithoutBootblock ( vol ) );

    vol->bitmap.table = (struct bBitmapBlock**)
        malloc ( sizeof(struct bBitmapBlock *) * vol->bitmap.size );
    if ( vol->bitmap.table == NULL ) {
        adfEnv.eFct("adfBitmapAllocate : malloc, vol->bitmapTable");
        return RC_MALLOC;
    }

    vol->bitmap.blocks = (SECTNUM *) malloc ( sizeof(SECTNUM) * vol->bitmap.size );
    if ( vol->bitmap.blocks == NULL ) {
        free ( vol->bitmap.table );
        vol->bitmap.table = NULL;
        adfEnv.eFct("adfBitmapAllocate : malloc, vol->bitmapBlocks");
        return RC_MALLOC;
    }

    vol->bitmap.blocksChg = (BOOL*) malloc ( sizeof(BOOL) * vol->bitmap.size );
    if ( vol->bitmap.blocksChg == NULL ) {
        free ( vol->bitmap.table );
        vol->bitmap.table = NULL;
        free ( vol->bitmap.blocks );
        vol->bitmap.blocks = NULL;
        adfEnv.eFct("adfBitmapAllocate : malloc, vol->bitmapBlocksChg");
        return RC_MALLOC;
    }

    for ( unsigned i = 0 ; i < vol->bitmap.size ; i++ ) {
        vol->bitmap.table[i] = (struct bBitmapBlock *)
            malloc ( sizeof(struct bBitmapBlock) );

        if ( vol->bitmap.table[i] == NULL) {
            free ( vol->bitmap.blocksChg );
            vol->bitmap.blocksChg = NULL;
            free ( vol->bitmap.blocks );
            vol->bitmap.blocks = NULL;
            for ( unsigned j = 0 ; j < i ; j++ )
                free ( vol->bitmap.table[j] );
            free ( vol->bitmap.table );
            vol->bitmap.table = NULL;
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
