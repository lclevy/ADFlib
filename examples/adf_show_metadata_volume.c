
#include <adf_raw.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "adf_show_metadata_volume.h"

#include "adf_show_metadata_common.h"


static void show_bmpages ( struct AdfVolume * const vol,
                           const struct bRootBlock * const rblock );

static void show_bmpages_array ( const int32_t * const bmpages,
                                 const unsigned        size );


void show_volume_metadata ( struct AdfVolume * const vol )
{
    adfVolInfo ( vol );

    struct bBootBlock bblock;
    if ( adfReadBootBlock ( vol, &bblock ) != RC_OK ) {
        fprintf ( stderr, "Error reading rootblock\n");
        return;
    }
    show_bootblock ( &bblock, false );

    struct bRootBlock rblock;
    SECTNUM root_block_sector = adfVolCalcRootBlk ( vol );
    printf ("\nRoot block sector:\t%u\n", root_block_sector );

    if ( adfReadRootBlock ( vol, (uint32_t)root_block_sector, &rblock ) != RC_OK ) {
        fprintf ( stderr, "Error reading rootblock at sector %u.\n", root_block_sector );
        return;
    }
    show_rootblock ( &rblock );

    show_bmpages ( vol, &rblock );
}


// replace non-printable with a dot '.'
static inline char printable ( char c )
{
    return ( isalnum ( c ) ? c : '.' );
}


void show_bootblock ( const struct bBootBlock * const bblock,
                      bool                            show_data )
{
    printf ("\nBootblock:\n  dosType:\t");
    for ( unsigned i = 0 ; i < 3 ; i++ )
        putchar ( printable ( bblock->dosType[i] ) );
    printf ( "%c (0x%x)\n", printable ( bblock->dosType[3] ), bblock->dosType[3] );

    uint8_t bblock_copy[sizeof(struct bBootBlock)];
    memcpy ( bblock_copy, bblock, sizeof (struct bBootBlock) );
    adfSwapEndian ( bblock_copy, ADF_SWBL_BOOT );
    uint32_t checksum_calculated = adfBootSum ( bblock_copy );
    printf ( "  checkSum:\t0x%x\n"
             "  - calculated:\t0x%x%s\n"
             "  rootBlock:\t0x%x (%u)\n",
             bblock->checkSum,
             checksum_calculated,
             bblock->checkSum == checksum_calculated ? " -> OK" : "  -> different(!)",
             bblock->rootBlock, bblock->rootBlock );

    if ( show_data )
        show_bootblock_data ( bblock );
}


void show_bootblock_data ( const struct bBootBlock * const bblock )
{
    puts ( "\nbootblock data / code (non-zero bytes):\n" );
    for ( unsigned i = 0 ; i < 500 + 512 ; ++i ) {
        uint32_t byte_i = bblock->data[i];
        if ( byte_i )
            printf ( "  data[ %d ]:  0x%x (%d)\n",
                     i, byte_i, byte_i );
    }
}


void show_rootblock ( const struct bRootBlock * const rblock )
{
    uint8_t rblock_orig_endian[512];
    memcpy ( rblock_orig_endian, rblock, 512 );
    adfSwapEndian ( rblock_orig_endian, ADF_SWBL_ROOT );
    uint32_t checksum_calculated = adfNormalSum ( rblock_orig_endian, 0x14,
                                                  sizeof (struct bRootBlock ) );
    printf ( "\nRootblock:\n"
             //"  offset field\t\tvalue\n"
             "  0x000  type:\t\t0x%x\t\t%u\n"
             "  0x004  headerKey:\t0x%x\t\t%u\n"
             "  0x008  highSeq:\t0x%x\t\t%u\n"
             "  0x00c  hashTableSize:\t0x%x\t\t%u\n"
             "  0x010  firstData:\t0x%x\t\t%u\n"
             "  0x014  checkSum:\t0x%x\n"
             "     ->  calculated:\t0x%x%s\n"
             "  0x018  hashTable [ %u ]:\t(see below)\n"
             "  0x138  bmFlag:\t0x%x\n"
             "  0x13c  bmPages[ %u ]:\t\t(see below)\n"
             "  0x1a0  bmExt:\t\t0x%x\n"
             "  0x1a4  cDays:\t\t0x%x\t\t%u\n"
             "  0x1a8  cMins:\t\t0x%x\t\t%u\n"
             "  0x1ac  cTicks:\t0x%x\t\t%u\n"
             "  0x1b0  nameLen:\t0x%x\t\t%u\n"
             "  0x1b1  diskName:\t%s\n"
             "  0x1d0  r2[8]:\t\t\t(see below)\n"
             "  0x1d8  days:\t\t0x%x\t\t%u\n"
             "  0x1dc  mins:\t\t0x%x\t\t%u\n"
             "  0x1e0  ticks:\t\t0x%x\t\t%u\n"
             "  0x1e4  coDays:\t0x%x\t\t%u\n"
             "  0x1e8  coMins:\t0x%x\t\t%u\n"
             "  0x1ec  coTicks:\t0x%x\t\t%u\n"
             "  0x1f0  nextSameHash:\t0x%x\t\t%u\n"
             "  0x1f4  parent:\t0x%x\t\t%u\n"
             "  0x1f8  extension:\t0x%x\t\t%u\n"
             "  0x1fc  secType:\t0x%x\t\t%d\n",
             rblock->type, rblock->type,
             rblock->headerKey, rblock->headerKey,
             rblock->highSeq, rblock->highSeq,
             rblock->hashTableSize, rblock->hashTableSize,
             rblock->firstData, rblock->firstData,
             rblock->checkSum,
             checksum_calculated,
             rblock->checkSum == checksum_calculated ? " -> OK" : " -> different(!)",
             HT_SIZE, //rblock->hashTable[HT_SIZE],
             rblock->bmFlag,
             BM_PAGES_ROOT_SIZE, //rblock->bmPages[BM_PAGES_ROOT_SIZE],
             rblock->bmExt,
             rblock->cDays, rblock->cDays,
             rblock->cMins, rblock->cMins,
             rblock->cTicks, rblock->cTicks,
             rblock->nameLen, rblock->nameLen,
             rblock->diskName,
             //rblock->r2[8],
             rblock->days, rblock->days,
             rblock->mins, rblock->mins,
             rblock->ticks, rblock->ticks,
             rblock->coDays, rblock->coDays,
             rblock->coMins, rblock->coMins,
             rblock->coTicks, rblock->coTicks,
             rblock->nextSameHash, rblock->nextSameHash,
             rblock->parent, rblock->parent,
             rblock->extension, rblock->extension,
             rblock->secType, rblock->secType
        );

    show_hashtable ( ( const uint32_t * const ) rblock->hashTable );
}


static void show_bmpages ( struct AdfVolume * const        vol,
                           const struct bRootBlock * const rblock )
{
    // show bmpages from the root block
    show_bmpages_array ( rblock->bmPages, BM_PAGES_ROOT_SIZE );

    // show bm ext block pages
    SECTNUM nSect = rblock->bmExt;
    while ( nSect != 0 ) {
        struct bBitmapExtBlock bmExtBlock;
        RETCODE rc = adfReadBitmapExtBlock ( vol, nSect, &bmExtBlock );
        if ( rc == RC_OK ) {
            show_bmpages_array ( (const int32_t * const) &bmExtBlock.bmPages,
                                 BM_PAGES_EXT_SIZE );
        } else {
            fprintf ( stderr, "Error reading bitmap allocation block, sector %u.\n",
                      nSect );
        }
        nSect = bmExtBlock.nextBlock;
    }
}


static void show_bmpages_array ( const int32_t * const bmpages,
                                 const unsigned        size )
{
    printf ( "\nBitmap block pointers (bmPages) (non-zero):\n" );
    for ( unsigned i = 0 ; i < size ; ++i ) {
        uint32_t bmpage_i = (uint32_t) bmpages [ i ];
        if ( bmpage_i )
            printf ( "  bmpages [ %2u ]:\t\t0x%x\t\t%u\n",
                     i, bmpage_i, bmpage_i );
    }
}
