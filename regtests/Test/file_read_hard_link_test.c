
#include <stdio.h>
#include <stdlib.h>
#include"adflib.h"

#define TEST_VERBOSITY 1

typedef struct check_s {
    unsigned int  offset;
    unsigned char value;
} check_t;

typedef struct reading_test_s {
    char *       info;
    char *       image_filename;

    char *       hlink_dir;
    char *       hlink_name;

    char *       real_file;

    unsigned int nchecks;
    check_t      checks[100];
} reading_test_t;


int test_hlink_read ( reading_test_t * test_data );

int test_single_read ( struct AdfFile * const file_adf,
                       unsigned int           offset,
                       unsigned char          expected_value );


int main ( int argc, char * argv[] )
{ 
    (void) argc;

    adfEnvInitDefault();

//	adfSetEnvFct(0,0,MyVer,0);
    int status = 0;

    // setup
    reading_test_t test_hlink = {
        .info       = "hard link file reads",
        .hlink_dir  = NULL, //"dir_1",
        .hlink_name = "hlink_blue",
        .real_file  = "dir_2/blue2c.gif",
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

    reading_test_t test_chained_hlink = {
        .info       = "chained hard link file reads",
        .hlink_dir  = "hardlinks_file",
        .hlink_name = "hl2hl2hl2testfile1",
        .real_file  = "dir1/dir1_1/testfile.txt",
        .nchecks    = 8,
        .checks     = {
            { 0, 'T' },
            { 1, 'h' },
            { 2, 'i' },
            { 3, 's' },

            // the last 4 bytes of the file
            { 0x3e, '.' },  // 0x2e
            { 0x3f, '.' },
            { 0x40, '\n' },  // 0x0a
            { 0x41, '\n' }
        }
    };

    test_hlink.image_filename = argv[1];
    test_chained_hlink.image_filename = argv[2];

    // run tests
    printf ("*** Test reading a file opened using a hardlink\n" );
    status += test_hlink_read ( &test_hlink );
    status += test_hlink_read ( &test_chained_hlink );
    printf ( status ? " -> ERROR\n" : " -> PASSED\n" );

    // clean-up
    adfEnvCleanUp();

    return status;
}


int test_hlink_read ( reading_test_t * test_data )
{
#if TEST_VERBOSITY > 0
    printf ( "\n*** Testing %s"
             "\n\timage file:\t%s\n\tdirectory:\t%s\n\thlink:\t\t%s\n\treal file:\t%s\n",
             test_data->info,
             test_data->image_filename,
             test_data->hlink_dir,
             test_data->hlink_name,
             test_data->real_file );
#endif

    struct AdfDevice * const dev = adfMountDev ( test_data->image_filename, TRUE );
    if ( ! dev ) {
        fprintf ( stderr, "Cannot mount image %s - aborting the test...\n",
                  test_data->image_filename );
        return 1;
    }

    struct AdfVolume * const vol = adfMount ( dev, 0, TRUE );
    if ( ! vol ) {
        fprintf ( stderr, "Cannot mount volume 0 from image %s - aborting the test...\n",
                  test_data->image_filename );
        adfUnMountDev ( dev );
        return 1;
    }

#if TEST_VERBOSITY > 0
    //adfVolumeInfo ( vol );
#endif

    int status = 0;
    adfToRootDir ( vol );
    char * dir = test_data->hlink_dir;
    if ( dir ) {
#if TEST_VERBOSITY > 0
        printf ("Entering directory %s...\n", dir );
#endif
        int chdir_st = adfChangeDir ( vol, dir );
        if ( chdir_st != RC_OK ) {
            fprintf ( stderr, " -> Cannot chdir to %s, status %d - aborting...\n",
                      dir, chdir_st );
            adfToRootDir ( vol );
            status = 1;
            goto clean_up;
        }
    }

    struct AdfFile * const file_adf = adfOpenFile ( vol, test_data->hlink_name, "r" );
    if ( ! file_adf ) {
        fprintf ( stderr, " -> Cannot open hard link file %s - aborting...\n",
                  test_data->hlink_name );
        status = 1;
        goto clean_up;
    }

    for ( unsigned int i = 0 ; i < test_data->nchecks ; ++i ) {
        status += test_single_read ( file_adf,
                                     test_data->checks[i].offset,
                                     test_data->checks[i].value );
    }

    adfCloseFile ( file_adf );

    // clean-up
clean_up:
    //adfToRootDir ( vol );
    adfUnMount ( vol );
    adfUnMountDev ( dev );

    return status;
}


int test_single_read ( struct AdfFile * const file_adf,
                       unsigned int           offset,
                       unsigned char          expected_value )
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
