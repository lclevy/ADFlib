/*
 * file_seek_test2.c
 */

#include <stdio.h>
#include <stdlib.h>
#include"adflib.h"

#define TEST_VERBOSITY 0

typedef struct check_s {
    unsigned int  offset;
    unsigned char value;
} check_t;

typedef struct reading_test_s {
    char *       image_filename;
    char *       hlink_name;
    char *       file_name;

    unsigned int nchecks;
    check_t      checks[];
} reading_test_t;


reading_test_t test_hlink = {
    .hlink_name = "hlink_blue",
    .file_name  = "dir_2/blue2c.gif",
    .nchecks    = 8,
    .checks     = {
        { 0, 0x47 },
        { 1, 0x49 },
        { 2, 0x46 },
        { 3, 0x38 },
                    
        // the last 4 bytes of the file
        { 0xcfe, 0x42 },
        { 0xcff, 0x04 },
        { 0xd00, 0x00 },
        { 0xd01, 0x3B }
    }
};


int test_simple_hlink_read ( struct Volume * const vol,
                             reading_test_t * test_data );

int test_single_read ( struct File * const file_adf,
                       unsigned int        offset,
                       unsigned char       expected_value );


int main ( int argc, char * argv[] )
{ 
    (void) argc;
    adfEnvInitDefault();

//	adfSetEnvFct(0,0,MyVer,0);
    int status = 0;

    // setup
    test_hlink.image_filename = argv[1];

    struct Device * const dev = adfMountDev ( test_hlink.image_filename, TRUE );
    if ( ! dev ) {
        printf ( "Cannot mount image %s - aborting the test...\n",
                 test_hlink.image_filename );
        return 1;
    }

    struct Volume * const vol = adfMount ( dev, 0, TRUE );
    if ( ! vol ) {
        printf ( "Cannot mount volume 0 from image %s - aborting the test...\n",
                 test_hlink.image_filename );
        adfUnMountDev ( dev );
        return 1;
    }
#if TEST_VERBOSITY > 0
    adfVolumeInfo ( vol );
#endif

    // run tests
    status += test_simple_hlink_read ( vol, &test_hlink );

    // clean-up
    adfUnMount ( vol );
    adfUnMountDev ( dev );
    
    adfEnvCleanUp();

    return status;
}


int test_simple_hlink_read ( struct Volume * const vol,
                             reading_test_t * test_data )
{
#if TEST_VERBOSITY > 0
    printf ( "\n*** Testing single hard link file reads"
             "\n\timage file:\t%s\n\thlink:\t%s\n\treal file:\t%s\n",
             test_data->image_filename,
             test_data->hlink_name,
             test_data->file_name );
#endif

    int status = 0;
    struct File * const file_adf = adfOpenFile ( vol, test_data->hlink_name, "r" );
    if ( ! file_adf ) {
        fprintf ( stderr, "Cannot open hard link file %s - aborting...\n",
                  test_data->hlink_name );
        return 1;
    }

    for ( unsigned int i = 0 ; i < test_data->nchecks ; ++i ) {
        status += test_single_read ( file_adf,
                                     test_data->checks[i].offset,
                                     test_data->checks[i].value );
    }

    adfCloseFile ( file_adf );

    return status;
}


int test_single_read ( struct File * const file_adf,
                       unsigned int        offset,
                       unsigned char       expected_value )
{
#if TEST_VERBOSITY > 0
    printf ( "  Reading data after seek to position 0x%x (%d)...",
             offset, offset );
#endif

    adfFileSeek ( file_adf, offset );

    unsigned char c;
    int n = adfReadFile ( file_adf, 1, &c );

    if ( n != 1 ) {
        fprintf ( stderr, " -> Reading data failed!!!\n" );
        return 1;
    }
    
    if ( c != expected_value ) {
        fprintf ( stderr, " -> Incorrect data read:  expected 0x%x != read 0x%x\n",
                 (int) expected_value, (int) c );
        return 1;
    }

#if TEST_VERBOSITY > 0
    printf ( " -> OK.\n" );
#endif

    return 0;
}
