
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


int show_block_allocation_bitmap ( struct AdfVolume * const vol );

char * num32_to_bit_str ( uint32_t num,
                          char     str[33] );


void usage ( void )
{
    printf ( "adf_bitmap - show block allocation bitmap\n\n"
             "Usage:  adf_bitmap adf_device\n\n"
             "where:\n  adf_device - an adf file (image) or a native (real) device\n\n"
             "(using adflib version %s)\n", adfGetVersionNumber() );
}


int main ( int     argc,
           char ** argv )
{
    if ( argc < 2 ) {
        usage();
        return 1;
    }
    char * const adfname = argv[1];
    //char * const path = ( argc == 3 ) ? argv[2] : NULL;

    int status = 0;

    adfEnvInitDefault();

    printf ( "\nOpening image/device:\t'%s'\n", adfname );
    struct AdfDevice * const dev = adfMountDev ( adfname, TRUE );
    if ( ! dev ) {
        fprintf ( stderr, "Cannot open file/device '%s' - aborting...\n",
                  adfname );
        status = 1;
        goto env_cleanup;
    }

    int vol_id = 0;
    struct AdfVolume * const vol = adfMount ( dev, vol_id, 1 );
    if ( ! vol ) {
        fprintf ( stderr, "Cannot mount volume %d - aborting...\n",
                  vol_id );
        status = 1;
        goto dev_cleanup;
    }

    unsigned volSizeBlocks = (unsigned) (vol->lastBlock - vol->firstBlock) + 1;
    printf ( "\nMounted volume:\t\t%d, '%s'\n"
             "\nVolume size in blocks:   \n"
             "   total (including boot block)   %u\n"
             "   filesystem blocks (in bitmap)  %u\n"
             "\nBlock allocation bitmap blocks:   %u\n",
             vol_id, vol->volName,
             volSizeBlocks, volSizeBlocks - 2,
             1 + ( volSizeBlocks - 2 ) / ( BM_MAP_SIZE * 4 * 8 ) );

    status = show_block_allocation_bitmap ( vol );

    adfUnMount ( vol );

dev_cleanup:
    adfUnMountDev ( dev );
env_cleanup:
    adfEnvCleanUp();

    return status;
}


int show_block_allocation_bitmap ( struct AdfVolume * const vol )
{
    struct bRootBlock   rb;
    struct bBitmapBlock bm;

    if ( adfReadRootBlock ( vol, (uint32_t) vol->rootBlock, &rb ) != RC_OK ) {
        fprintf ( stderr, "invalid RootBlock on orig. volume, sector %u\n",
                  vol->rootBlock );
        return 1;
    }
    
    /* Check root bm pages  */
    unsigned nerrors = 0;
    char bitStr[33];
    for ( unsigned i = 0 ; i < BM_PAGES_ROOT_SIZE ; i++ ) {
        SECTNUM bmPage = rb.bmPages[i];

        if ( bmPage == 0 )
            continue;

        RETCODE rc = adfReadBitmapBlock ( vol, bmPage, &bm );
        if ( rc != RC_OK ) {
            fprintf ( stderr, "error reading bitmap block on vol. orig, block %u\n",
                      bmPage );
            nerrors++;
            continue;
        }

        printf ( "\nBitmap allocation block - rootblock.bmPages[ %u ], block %d\n\n"
                     "index  block  -> hex       value   bitmap ('.' = free, 'o' = used)\n",
                 i, bmPage );

        for ( unsigned i = 0 ; i < BM_MAP_SIZE ; i++ ) {
            uint32_t val = bm.map[i];
            unsigned blockNum = 2 + i * 32;
            printf ( "%5u  %5u  0x%04x  0x%08x   %s\n",
                     i, blockNum, blockNum,
                     val,  num32_to_bit_str ( val, bitStr ) );
        }
    }

    /* add showing bmExt blocks! */

    return ( nerrors > 0 ) ? 1 : 0;    
}

char * num32_to_bit_str ( uint32_t num,
                          char     str[33] )
{
    for (unsigned i = 0 ; i <= 31 ; i++ ) {
        uint32_t mask = 1 << i;        // check endian!
        uint32_t bitValue = ( num & mask ) >> i;
        //str[i] = bitValue ? 'o' : '.';
        str[i] = bitValue ?
            '.' :  // 1 - free
            'o';   // 0 - allocated
    }
    str[32] = '\0';
    return str;
}


#if defined(_WIN32) && !defined(_CYGWIN)

// custom impl. of POSIX's dirname
// (note that it modifies buffer pointed by path)
char* dirname(char* path)
{
    if (!path)
        return NULL;

    int len = strlen(path);
    if (len < 1)
        return NULL;

    char* last_slash = strrchr(path, '/');
    if (!last_slash) {
        // no slash - no directory in path (only basename)
        path[0] = '.';
        path[1] = '\0';
        return path;
    }

    if (path + len - 1 == last_slash) {
        // slash at the end of the path - remove it
        path[len - 1] = '\0';
    }

    last_slash = strrchr(path, '/');
    if (!last_slash) {
        // no directory before basename
        path[0] = '.';
        path[1] = '\0';
        return path;
    }
    else {
        // cut the last slash and the basename from path
        *last_slash = '\0';
        return path;
    }
}

// custom impl. of POSIX's basename
// (note that it modifies buffer pointed by path)
char* basename(char* path)
{
    if (!path)
        return NULL;

    char* last_slash = strrchr(path, '/');
    if (!last_slash) {
        // no slash - no directory in path (only basename)
        return path;
    }

    int len = strlen(path);
    if (path + len - 1 == last_slash) {
        // slash at the end of the path - remove it
        path[len - 1] = '\0';
    }

    last_slash = strrchr(path, '/');
    if (!last_slash) {
        // no directory before basename
        return path;
    }
    else {
        // the basename starts after the last slash
        return last_slash + 1;
    }
}
#endif
