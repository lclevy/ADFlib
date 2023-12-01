/*
 * The program performs a test of updating the block allocation bitmap
 * on the given image.
 *
 * It does the following:
 * 1. makes a copy of the original images
 * 2. mounts and enforces block allocation bitmap update
 * 3. compares if the updated bitmap is the same as the original
 *    (every bit different counts as an error)
 * Obviously, the test can be performed only on images with already correct
 * block allocation bitmap (which should not be changed by the enforced update).
 *
 * More details on bitmap issues:
 * https://github.com/lclevy/ADFlib/issues/63
 */


#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "adflib.h"

#define TEST_VERBOSITY 1


void log_error ( FILE * const       file,
                 const char * const format, ... );

void log_warning ( FILE * const       file,
                   const char * const format, ... );

void log_info ( FILE * const       file,
                const char * const format, ... );

RETCODE copy_file ( const char * const dst_fname,
                    const char * const src_fname );

unsigned compare_bitmaps ( struct AdfVolume * const volOrig,
                           struct AdfVolume * const volUpdate );

char * num32_to_bit_str ( uint32_t num,
                          char     str[33] );


int main ( const int          argc,
           const char * const argv[] )
{
    if ( argc < 2 ) {
        fprintf (stderr, "Usage: %s <ADF_IMAGE>\n", argv[0] );
        return 1;
    }

    /* set filenames */
    const char * const adfOrig         = argv[1];
    char               adfUpdate[1024] = "\0";
    strcat ( adfUpdate, adfOrig );
    strcat ( adfUpdate, "-bitmap_updated.adf" );

    BOOL error_status = FALSE;

    /* make a copy for updating */
    if ( copy_file ( adfUpdate, adfOrig ) != RC_OK ) {
        error_status = TRUE;
        goto delete_adf_copy;
    }

    /* init adflib */
    adfEnvInitDefault();
//	adfSetEnvFct(0,0,MyVer,0);

    /* mount the original image */
    struct AdfDevice * const devOrig = adfMountDev ( adfOrig, FALSE );
    if ( devOrig == NULL ) {
        log_error ( stderr, "can't mount device %s\n", adfOrig );
        error_status = TRUE;
        goto clean_up;
    }

    struct AdfVolume * const volOrig = adfMount ( devOrig, 0, FALSE );
    if ( volOrig == NULL ) {
        log_error ( stderr, "can't mount volume %d\n", 0 );
        error_status = TRUE;
        goto umount_dev_orig;
    }

    
    /* mount the image copy (for updating) */
    struct AdfDevice * const devUpdate = adfMountDev ( adfUpdate, FALSE );
    if ( devUpdate == NULL ) {
        log_error ( stderr, "can't mount device %s\n", adfUpdate );
        error_status = TRUE;
        goto umount_vol_orig;
    }

    struct AdfVolume * const volUpdate = adfMount ( devUpdate, 0, FALSE );
    if ( volUpdate == NULL ) {
        log_error ( stderr, "can't mount volume %d\n", 0 );
        error_status = TRUE;
        goto umount_dev_updated;
    }


    /* update the block allocation bitmap */
    RETCODE rc = adfVolReconstructBitmap ( volUpdate );
    if ( rc != RC_OK ) {
        log_error (
            stderr, "error reconstructing block allocation bitmap, volume %d\n", 0 );
        error_status = TRUE;
        goto umount_vol_updated;
    }

    /* compare the original and reconstructed */
    unsigned nerrors = compare_bitmaps ( volOrig, volUpdate );
    if ( nerrors != 0 )
        log_error ( stderr, "Bitmap update of %s: %u errors\n", adfOrig, nerrors );
    
    error_status = ( nerrors != 0 ? TRUE : FALSE );
    
    //adfVolumeInfo(vol);
    //putchar('\n');

    /* cleanup */

umount_vol_updated:
    adfUnMount ( volUpdate );

umount_dev_updated:
    adfUnMountDev ( devUpdate );

umount_vol_orig:
    adfUnMount ( volOrig );

umount_dev_orig:
    adfUnMountDev ( devOrig );

clean_up:
    adfEnvCleanUp();
    
delete_adf_copy:
    log_info ( stdout, "Removing %s\n", adfUpdate );
    unlink ( adfUpdate );
    
    return ( error_status == TRUE ? 1 : 0 );
}



#define BUFSIZE 1024

RETCODE copy_file ( const char * const dst_fname,
                    const char * const src_fname )
{
    log_info ( stdout, "Copying %s to %s... ", src_fname, dst_fname );
    FILE * const src = fopen ( src_fname, "rb" );
    if ( src == NULL )
        return RC_ERROR;

    FILE * const dst = fopen ( dst_fname, "wb" );
    if ( src == NULL ) {
        fclose ( src );
        return RC_ERROR;
    }

    RETCODE status = RC_OK;
    char buffer[BUFSIZE];
    size_t bytes_read;
    while ( ( bytes_read = fread ( buffer, 1, BUFSIZE, src ) ) > 0 ) {
        size_t bytes_written = fwrite ( buffer, 1, bytes_read, dst );
        if ( bytes_written != bytes_read ) {
            log_error ( stderr, "error writing copy to %s\n", dst_fname );
            status = RC_ERROR;
            break;
        }
    }
    fclose ( src );
    fclose ( dst );
    log_info ( stdout, "Done!\n" );
    return status;
}


// returns number of errors
unsigned compare_bitmaps ( struct AdfVolume * const volOrig,
                           struct AdfVolume * const volUpdate )
{
    struct bRootBlock   rbOrig, rbUpdate;
    struct bBitmapBlock bmOrig, bmUpdate;

    if ( adfReadRootBlock ( volOrig, (uint32_t) volOrig->rootBlock,
                            &rbOrig ) != RC_OK )
    {
        log_error ( stderr, "invalid RootBlock on orig. volume, sector %u\n",
                    volOrig->rootBlock );

        return 1;
    }

    if ( adfReadRootBlock ( volUpdate, (uint32_t) volUpdate->rootBlock,
                            &rbUpdate ) != RC_OK )
    {
        log_error ( stderr, "invalid RootBlock on orig. volume, sector %u\n",
                    volOrig->rootBlock );

        return 1;
    } 
    
    /* Check root bm pages  */
    unsigned nerrors = 0;
    for ( unsigned i = 0 ; i < BM_PAGES_ROOT_SIZE ; i++ ) {
        SECTNUM bmPageOrig   = rbOrig.bmPages[i],
                bmPageUpdate = rbUpdate.bmPages[i];

        if ( bmPageOrig == 0 && bmPageUpdate == 0 )
            continue;
             
        if ( ( bmPageOrig == 0 && bmPageUpdate != 0 ) ||
             ( bmPageOrig != 0 && bmPageUpdate == 0 ) )
        {
            log_error ( stderr, "bmPages[%u]: orig (%u) != ... update (%u)"
                        " - and one of them is 0 -> error -> skipping!\n",
                        i, bmPageOrig, bmPageUpdate );
            nerrors++;
            continue;
        }
        
        if ( bmPageOrig != bmPageUpdate ) {
            // in case of relocation, this could be OK - but return a warning
            log_warning ( stderr, "bmPages[%u] differ: orig (%u) != update (%u)\n",
                          i, bmPageOrig, bmPageUpdate );
        }

        RETCODE rc = adfReadBitmapBlock ( volOrig, bmPageOrig, &bmOrig );
        if ( rc != RC_OK ) {
            log_error ( stderr, "error reading bitmap block on vol. orig, block %u\n",
                        bmPageOrig );
            nerrors++;
            continue;
        }

        rc = adfReadBitmapBlock ( volUpdate, bmPageUpdate, &bmUpdate );
        if ( rc != RC_OK ) {
            log_error ( stderr, "error reading bitmap block on vol. update, block %u\n",
                        bmPageOrig );
            nerrors++;
            continue;
        }

        char bitStrOrig[33];
        char bitStrUpdate[33];
        for ( unsigned i = 0 ; i < BM_MAP_SIZE ; i++ ) {
            if ( bmOrig.map[i] != bmUpdate.map[i] ) {
                uint32_t
                    valOrig   = bmOrig.map[i],
                    valUpdate = bmUpdate.map[i];
                log_error ( stderr,
                            "bm differ at %u:\n"
                            "  orig   %10u  %s\n"
                            "  update %10u  %s\n",
                            i,
                            valOrig,   num32_to_bit_str ( valOrig, bitStrOrig ),
                            valUpdate, num32_to_bit_str ( valUpdate, bitStrUpdate ) );
                nerrors++;
            }
        }
    }

    /* add checking bmExt blocks! */

    return nerrors;
}



void log_error ( FILE * const       file,
                 const char * const format, ... )
{
#if TEST_VERBOSITY > 0
    va_list ap;
    va_start ( ap, format );
    fprintf ( stderr, "Error: " );
    vfprintf ( file, format, ap );
    va_end ( ap );
#else
    (void) file, (void) format;
#endif
}


void log_warning ( FILE * const       file,
                   const char * const format, ... )
{
#if TEST_VERBOSITY > 0
    va_list ap;
    va_start ( ap, format );
    fprintf ( stderr, "Warning: " );
    vfprintf ( file, format, ap );
    va_end ( ap );
#else
    (void) file, (void) format;
#endif
}


void log_info ( FILE * const       file,
                const char * const format, ... )
{
#if TEST_VERBOSITY > 1
    va_list ap;
    va_start ( ap, format );
    //fprintf ( stderr, "Warning: " );
    vfprintf ( file, format, ap );
    va_end ( ap );
#else
    (void) file, (void) format;
#endif
}




char * num32_to_bit_str ( uint32_t num,
                          char     str[33] )
{
    for (unsigned i = 0 ; i <= 31 ; i++ ) {
        uint32_t mask = 1 << i;
        uint32_t bitValue = ( num & mask ) >> i;
        str[i] = bitValue ?
            '.' :  // block free
            'o';   // block allocated
    }
    str[32] = '\0';
    return str;
}
