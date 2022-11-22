/*
 * file_seek_test.c
 */

#include <stdio.h>
#include"adflib.h"


typedef struct check_s {
    unsigned int  offset;
    unsigned char value;
} check_t;

typedef struct reading_test_s {
    char * image_filename;
    char * filename;

    unsigned int nchecks;
    check_t      checks[];
} reading_test_t;


const unsigned int ofs_highest_pos_not_using_extension = 35135;  // 0x893f

reading_test_t test_ofs = {
    .filename = "moon.gif",
    .nchecks = 7,
    .checks = { { 0, 0x47 },
                { 1, 0x49 },
                { ofs_highest_pos_not_using_extension, 5 },
                { ofs_highest_pos_not_using_extension + 1, 0x2d },
                { ofs_highest_pos_not_using_extension + 2, 0x6c },
                { 0x2a708, 3 },
                { 0x2a716, 0x3b }  // the last byte of the file
                // add checking beyond the file ?
    }
};


const unsigned int ffs_highest_pos_not_using_extension = //39935;  // 0x9bff   ( 512 * 72) ?
    36863;  // 0x9bff

reading_test_t test_ffs = {
    .filename = "mod.And.DistantCall",
    .nchecks = 11,
    .checks = { { 0, 0x64 },
                { 1, 0x69 },
                { ofs_highest_pos_not_using_extension, 0x28 },
                { ofs_highest_pos_not_using_extension + 1, 0x24 },
                { ofs_highest_pos_not_using_extension + 2, 0x1b },
                { ffs_highest_pos_not_using_extension, 0xff },
                { ffs_highest_pos_not_using_extension + 1, 0x07 },
                { ffs_highest_pos_not_using_extension + 2, 0x0c },
                { 0x237c0, 0xfc },
                { 0x237ce, 0xea },
                { 0x237cf, 0x00 },  // the last byte of the file
                // add checking beyond the file ?
    }
};


int run_single_seek_tests ( reading_test_t * test_data );
int test_single_seek ( struct File *       file,
                       unsigned int        offset,
                       const unsigned char expected_value );


int main ( int argc, char * argv[] )
{ 
    adfEnvInitDefault();

//	adfSetEnvFct(0,0,MyVer,0);
    int status = 0;

    test_ofs.image_filename = argv[1];
    status += run_single_seek_tests ( &test_ofs );

    test_ffs.image_filename = argv[2];
    status += run_single_seek_tests ( &test_ffs );

    adfEnvCleanUp();

    return status;
}



int run_single_seek_tests ( reading_test_t * test_data )
{
    printf ( "\n*** Test file seeking on image: %s, filename: %s\n",
             test_data->image_filename, test_data->filename );

    struct Device * const dev = adfMountDev ( test_data->image_filename, TRUE );
    if ( ! dev ) {
        printf ( "Cannot mount image %s - aborting the test...\n",
                 test_data->image_filename );
        return 1;
    }

    struct Volume * const vol = adfMount ( dev, 0, TRUE );
    if ( ! vol ) {
        printf ( "Cannot mount volume 0 from image %s - aborting the test...\n",
                 test_data->image_filename );
        adfUnMountDev ( dev );
        return 1;
    }
    adfVolumeInfo ( vol );

    int status = 0;
    for ( int i = 0 ; i < test_data->nchecks ; ++i ) {

        struct File * file = adfOpenFile ( vol, test_data->filename, "r" );
        if ( ! file ) {
            printf ("Cannot open file %s - aborting...\n", test_data->filename );
            status = 1;
            goto cleanup;
        }

        status += test_single_seek ( file,
                                     test_data->checks[i].offset,
                                     test_data->checks[i].value );
        adfCloseFile ( file );
    }

cleanup:
    adfUnMount ( vol );
    adfUnMountDev ( dev );

    return status;
}


int test_single_seek ( struct File *       file,
                       unsigned int        offset,
                       const unsigned char expected_value )
{
    printf ( "  Reading data after seek to position 0x%x (%d)...",
             offset, offset );

    adfFileSeek ( file, offset );

    unsigned char c;
    int n = adfReadFile ( file, 1, &c );

    if ( n != 1 ) {
        printf ( " -> Reading data failed!!!\n" );
        return 1;
    }

    if ( c != expected_value ) {
        printf ( " -> Incorrect data read:  expected 0x%x != read 0x%x\n",
                 (int) expected_value, (int) c );
        return 1;
    }

    printf ( " -> OK.\n" );
    return 0;
}
