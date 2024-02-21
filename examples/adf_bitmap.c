
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32) && !defined(_CYGWIN)
char* dirname(char* path);
char* basename(char* path);
#else
#include <libgen.h>
#endif

#include <adflib.h>


RETCODE rebuild_bitmap ( struct AdfVolume * const vol );
int show_block_allocation_bitmap ( struct AdfVolume * const vol );

typedef char bitstr32_t [36];
char * num32_to_bit_str ( const uint32_t num,
                          bitstr32_t     str );

unsigned num32_count_bits ( const uint32_t num,
                            const unsigned nskip_bits );

typedef enum {
    COMMAND_SHOW,
    COMMAND_REBUILD,
    COMMAND_HELP
} command_t;


void usage ( void )
{
    printf ( "\nadf_bitmap - show or rebuild block allocation bitmap\n\n"
             "Usage:  adf_bitmap <command> adf_device\n\n"
             "where:\n  command      - 'show', 'rebuild' or 'help'\n"
             "  adf_device   - an adf file (image) or a native (real) device\n\n"
             "(using adflib version %s)\n", adfGetVersionNumber() );
}


int main ( int     argc,
           char ** argv )
{
    if ( argc < 3 ) {
        usage();
        return 1;
    }

    command_t command;
    if ( strcmp ( argv[1], "rebuild" ) == 0 )
        command = COMMAND_REBUILD;
    else if ( strcmp ( argv[1], "show" ) == 0 )
        command = COMMAND_SHOW;
    else if ( strcmp ( argv[1], "help" ) == 0 ) {
        usage();
        return 0;
    }
    else {
        fprintf ( stderr, "\nUnknown command '%s'  "
                  "(use 'adf_bitmap help' for usage info)\n\n",
                  argv[1] );
        return 1;
    }

    char * const adfname = argv[2];

    int status = 0;

    adfEnvInitDefault();

    AdfAccessMode devMode = ( command == COMMAND_REBUILD ? ADF_ACCESS_MODE_READWRITE :
                                                           ADF_ACCESS_MODE_READONLY );

    printf ( "\nOpening image/device:\t'%s' (%s)\n",
             adfname, devMode ? "read-only" : "read-write" );

    struct AdfDevice * const dev = adfDevOpen ( adfname, devMode );
    if ( ! dev ) {
        fprintf ( stderr, "Cannot open file/device '%s' - aborting...\n",
                  adfname );
        status = 1;
        goto env_cleanup;
    }

    RETCODE rc = adfDevMount ( dev );
    if ( rc != RC_OK ) {
        fprintf ( stderr, "Cannot get volume info for file/device '%s' - aborting...\n",
                  adfname );
        goto dev_cleanup;
    }

    int vol_id = 0;
    struct AdfVolume * const vol = adfVolMount ( dev, vol_id, ADF_ACCESS_MODE_READONLY );
    if ( ! vol ) {
        fprintf ( stderr, "Cannot mount volume %d - aborting...\n",
                  vol_id );
        status = 1;
        goto dev_mount_cleanup;
    }

    unsigned volSizeBlocks = adfVolGetBlockNum ( vol );
    printf ( "\nMounted volume:\t\t%d, '%s'\n"
             "\nVolume size in blocks:   \n"
             "   total (including boot block)   %u\n"
             "   filesystem blocks (in bitmap)  %u\n"
             "\nBlock allocation bitmap blocks:   %u\n",
             vol_id, vol->volName,
             volSizeBlocks, volSizeBlocks - 2,
             1 + ( volSizeBlocks - 2 ) / ( BM_MAP_SIZE * 4 * 8 ) );

    if ( command == COMMAND_REBUILD ) {
        RETCODE rc = rebuild_bitmap ( vol );
        status = ( rc == RC_OK ? 0 : 1 );
    } else {  // COMMAND_SHOW
        status = show_block_allocation_bitmap ( vol );
    }

    adfVolUnMount ( vol );

dev_mount_cleanup:
    adfDevUnMount ( dev );
dev_cleanup:
    adfDevClose ( dev );
env_cleanup:
    adfEnvCleanUp();

    return status;
}


RETCODE rebuild_bitmap ( struct AdfVolume * const vol )
{
    RETCODE rc = adfVolRemount ( vol, ADF_ACCESS_MODE_READWRITE );
    if ( rc != RC_OK ) {
        fprintf ( stderr, "Remounting the volume read-write has failed -"
                  " aborting...\n" );
        return rc;
    }

    struct bRootBlock root;
    //printf ("reading root block from %u\n", vol->rootBlock );
    rc = adfReadRootBlock ( vol, (uint32_t) vol->rootBlock, &root );
    if ( rc != RC_OK ) {
        adfEnv.eFct ( "Invalid RootBlock, sector %u - aborting...",
                      vol->rootBlock );
        return rc;
    }
    //printf ("root block read, name %s\n", root.diskName );

    rc = adfReconstructBitmap ( vol, &root );
    if ( rc != RC_OK ) {
        adfEnv.eFct ( "Error rebuilding the bitmap (%d)", rc );
        return rc;
    }

    rc = adfUpdateBitmap ( vol );
    if ( rc != RC_OK ) {
        adfEnv.eFct ( "Error writing updated bitmap (%d)", rc );
        return rc;
    }

    printf ("Bitmap reconstruction complete.\n");

    return RC_OK;
}


int show_block_allocation_bitmap ( struct AdfVolume * const vol )
{
    const char
        separatorLine1[] =
        "----------------------------------------------------------------------",
        separatorLine2[] =
        "======================================================================";
    struct bRootBlock   rb;
    struct bBitmapBlock bm;

    if ( adfReadRootBlock ( vol, (uint32_t) vol->rootBlock, &rb ) != RC_OK ) {
        fprintf ( stderr, "invalid RootBlock on orig. volume, sector %u\n",
                  vol->rootBlock );
        return 1;
    }

    printf ( "\nBlock allocation bitmap valid:    %s\n",
             rb.bmFlag == ADF_BM_VALID ? "yes" : "No!" );

    /* Check root bm pages  */
    unsigned nerrors = 0,
        nblocks_free = 0,
        nblocks_used = 0;
    bitstr32_t bitStr;
    unsigned filesystem_blocks_num = adfVolGetBlockNumWithoutBootblock ( vol );
    unsigned last_uint32_bits_unused =
        filesystem_blocks_num % 32 == 0 ? 0 : 32 - filesystem_blocks_num % 32;

    for ( unsigned i = 0 ; i < BM_PAGES_ROOT_SIZE ; i++ ) {
        SECTNUM bmPage = rb.bmPages[i];

        if ( bmPage == 0 )
            continue;

        RETCODE rc = adfReadBitmapBlock ( vol, bmPage, &bm );
        if ( rc != RC_OK ) {
            fprintf ( stderr, "error reading bitmap block on vol. %s, block %u\n",
                      vol->volName, bmPage );
            nerrors++;
            continue;
        }

        printf ( "\n%s\nBitmap allocation block - rootblock.bmPages[ %u ], block %d\n\n"
                 "index  block  -> hex       value     bitmap ('.' = free, 'o' = used)\n%s\n",
                 separatorLine2, i, bmPage, separatorLine1 );

        for ( unsigned j = 0 ; j < BM_MAP_SIZE ; j++ ) {
            uint32_t val = bm.map[j];
            unsigned blockNum = ( i * BM_MAP_SIZE + j ) * 32;
            printf ( "%5u  %5u  0x%04x  0x%08x   %s\n",
                     j, blockNum, blockNum,
                     val,  num32_to_bit_str ( val, bitStr ) );
            if ( blockNum < filesystem_blocks_num ) {
                unsigned nbits_unused =
                    ( blockNum + 32 < filesystem_blocks_num ) ?
                    0 : last_uint32_bits_unused;
                unsigned nbits_true = num32_count_bits ( val, nbits_unused );
                nblocks_free += nbits_true;
                nblocks_used += 32 - nbits_unused - nbits_true;
            }
        }
    }

    /* show bmExt blocks */
    SECTNUM bmExtBlkPtr = rb.bmExt;
    while ( bmExtBlkPtr != 0 ) {
        struct bBitmapExtBlock bmExtBlk;
        RETCODE rc = adfReadBitmapExtBlock ( vol, bmExtBlkPtr, &bmExtBlk );
        if ( rc != RC_OK ) {
            adfFreeBitmap ( vol );
            return 1;
        }

        unsigned i = 0;
        while ( i < BM_PAGES_EXT_SIZE ) {
            SECTNUM bmBlkPtr = bmExtBlk.bmPages[i];
            if ( ! adfVolIsSectNumValid ( vol, bmBlkPtr ) ) {
                adfEnv.eFct ( "adfReadBitmap : sector %d out of range", bmBlkPtr );
                adfFreeBitmap ( vol );
                return 1;
            }

            rc = adfReadBitmapBlock ( vol, bmBlkPtr, &bm );
            if ( rc != RC_OK ) {
                fprintf ( stderr, "error reading bitmap block on vol. %s, block %u\n",
                      vol->volName, bmBlkPtr );
                nerrors++;
                i++;
                continue;
            }

            printf ( "\n%s\nBitmap allocation block - bmExt(%u).bmPages[ %u ], block %d\n\n"
                     "index  block  -> hex       value     bitmap ('.' = free, 'o' = used)\n%s\n",
                     separatorLine2, bmExtBlkPtr, i, bmBlkPtr, separatorLine1 );

            for ( unsigned j = 0 ; j < BM_MAP_SIZE ; j++ ) {
                uint32_t val = bm.map[j];
                unsigned blockNum = ( BM_PAGES_ROOT_SIZE * BM_MAP_SIZE +
                                      i * BM_MAP_SIZE + j ) * 32;
                printf ( "%5u  %5u  0x%04x  0x%08x   %s\n",
                         j, blockNum, blockNum,
                         val,  num32_to_bit_str ( val, bitStr ) );
                if ( blockNum < filesystem_blocks_num ) {
                    unsigned nbits_unused =
                        ( blockNum + 32 < filesystem_blocks_num ) ?
                        0 : last_uint32_bits_unused;
                    unsigned nbits_true = num32_count_bits ( val, nbits_unused );
                    nblocks_free += nbits_true;
                    nblocks_used += 32 - nbits_unused - nbits_true;
                }
            }

            i++;
        }

        bmExtBlkPtr = bmExtBlk.nextBlock;
    }

    /* show block statistics */
    printf ( "\nBlocks used  %6u (%u bytes)"
             "\n       free  %6u (%u bytes)"
             "\n       total %6u (%u bytes)\n",
             nblocks_used, nblocks_used * 512,
             nblocks_free, nblocks_free * 512,
             filesystem_blocks_num, filesystem_blocks_num * 512 );
    if ( nerrors > 0 )
        printf ("\nWARNING: %u errors occured meaning that some bitmap "
                "(or bitmap ext.) blocks could not be read\n"
                " so the numbers above are not fully accurate "
                "(exclude data from the unchecked bitmap blocks!)\n", nerrors );
    return ( nerrors > 0 ) ? 1 : 0;
}


char * num32_to_bit_str ( const uint32_t num,
                          bitstr32_t     str )
{
    for (unsigned i = 0, j = 0 ; i <= 31 ; i++, j++ ) {
        uint32_t mask = 1u << i;
        uint32_t bitValue = ( num & mask ) >> i;
        //str[i] = bitValue ? 'o' : '.';

        // byte separator
        if ( i % 8 == 0 && i > 0 ) {
            str[j] = ' ';
            j++;
        }

        str[j] = bitValue ?
            '.' :  // 1 -> free block
            'o';   // 0 -> allocated block
    }
    str[35] = '\0';
    return str;
}


unsigned num32_count_bits ( const uint32_t num,
                            const unsigned nskip_bits )
{
    unsigned ntrue = 0;
    for ( unsigned i = 0 ; i <= 31u - nskip_bits ; i++ ) {
        uint32_t mask = 1u << i;
        uint32_t bitValue = ( num & mask ) >> i;
        ntrue  += bitValue;
    }
    return ntrue;
}
