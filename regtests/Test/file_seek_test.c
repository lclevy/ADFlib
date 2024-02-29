/*
 * file_seek_test.c
 */

#include <stdio.h>
#include"adflib.h"

#define TEST_VERBOSITY 0

typedef struct check_s {
    unsigned int  offset;
    unsigned char value;
} check_t;

typedef struct reading_test_s {
    char * image_filename;
    char * filename;

    unsigned int nchecks;
    check_t *    checks;
} reading_test_t;


#ifdef _MSC_VER   // visual studio do not understand that const is const...
#define ofs_first_pos_using_extension 0x8940
#else
const unsigned int ofs_first_pos_using_extension = 0x8940;  // 35136
#endif


check_t checks_ofs[] = {
    { 0, 0x47 },
    { 1, 0x49 },
    { ofs_first_pos_using_extension - 1,    5 },
    { ofs_first_pos_using_extension,     0x2d },
    { ofs_first_pos_using_extension + 1, 0x6c },
    { 0x2a708, 3 },
    { 0x2a716, 0x3b },  // the last byte of the test file
    { 0x2a717, 0x00 }   // EOF
};

reading_test_t test_ofs = {
    .filename = "moon.gif",
    .nchecks = sizeof ( checks_ofs ) / sizeof ( check_t ),
    .checks = checks_ofs
};


#ifdef _MSC_VER   // visual studio do not understand that const is const...
#define ffs_first_pos_using_extension     0x9000   // 36863
#define ffs_first_pos_using_2nd_extension 0x12000  // 73728
#else
const unsigned int
    ffs_first_pos_using_extension     = 0x9000,   // 36863
    ffs_first_pos_using_2nd_extension = 0x12000;  // 73728
#endif

check_t checks_ffs[] = {
    { 0, 0x64 },
    { 1, 0x69 },

    { ofs_first_pos_using_extension - 1, 0x28 },
    { ofs_first_pos_using_extension,     0x24 },
    { ofs_first_pos_using_extension + 1, 0x1b },

    { ffs_first_pos_using_extension - 1, 0xff },
    { ffs_first_pos_using_extension,     0x07 },
    { ffs_first_pos_using_extension + 1, 0x0c },

    { 0x9100, 0x12 },
    { 0x9180, 0x0d },
    { 0x91ff, 0x06 },
    { 0x9200, 0x00 },
    { 0x9201, 0xf6 },
    { 0x9bc0, 0x09 },

    { ffs_first_pos_using_2nd_extension - 1, 0x18 },
    { ffs_first_pos_using_2nd_extension,     0x1a },
    { ffs_first_pos_using_2nd_extension + 1, 0x1d },

    { 0x237bf, 0xfa },
    { 0x237c0, 0xfc },
    { 0x237ce, 0xea },
    { 0x237cf, 0x00 },  // the last byte of the file
    { 0x237d0, 0x00 }   // EOF
};

reading_test_t test_ffs = {
    .filename = "mod.And.DistantCall",
    .nchecks =  sizeof ( checks_ffs ) / sizeof ( check_t ),
    .checks  = checks_ffs
};


int run_single_seek_tests ( reading_test_t * test_data );
int test_single_seek ( struct AdfFile *    file,
                       unsigned int        offset,
                       const unsigned char expected_value );
int test_seek_eof ( struct AdfFile * file,
                    unsigned int     offset );


int main ( int argc, char * argv[] )
{
    if ( argc < 3 ) {
        fprintf ( stderr,
                  "%s requires 2 parameters\n", argv[0] );
        return 1;
    }

    adfEnvInitDefault();

//	adfEnvSetFct(0,0,MyVer,0);
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
#if TEST_VERBOSITY > 0
    printf ( "\n*** Test file seeking on image: %s, filename: %s\n",
             test_data->image_filename, test_data->filename );
    fflush ( stdout );
#endif

    struct AdfDevice * const dev = adfDevOpen ( test_data->image_filename,
                                                ADF_ACCESS_MODE_READONLY );
    if ( ! dev ) {
        fprintf ( stderr, "Cannot open file/device '%s' - aborting...\n",
                  test_data->image_filename );
        adfEnvCleanUp();
        exit(1);
    }

    ADF_RETCODE rc = adfDevMount ( dev );
    if ( rc != ADF_RC_OK ) {
        fprintf ( stderr, "Cannot mount image %s - aborting the test...\n",
                  test_data->image_filename );
        adfDevClose ( dev );
        return 1;
    }

    struct AdfVolume * const vol = adfVolMount ( dev, 0, ADF_ACCESS_MODE_READONLY );
    if ( ! vol ) {
        fprintf ( stderr, "Cannot mount volume 0 from image %s - aborting the test...\n",
                 test_data->image_filename );
        adfDevUnMount ( dev );
        adfDevClose ( dev );
        return 1;
    }
#if TEST_VERBOSITY > 0
    adfVolInfo ( vol );
#endif

    int status = 0;
    for ( unsigned int i = 0 ; i < test_data->nchecks - 1; ++i ) {

        struct AdfFile * file = adfFileOpen ( vol, test_data->filename, ADF_FILE_MODE_READ );
        if ( ! file ) {
            printf ("Cannot open file %s - aborting...\n", test_data->filename );
            status = 1;
            goto cleanup;
        }

        status += test_single_seek ( file,
                                     test_data->checks[i].offset,
                                     test_data->checks[i].value );
        adfFileClose ( file );
    }

    // test EOF
    struct AdfFile * file = adfFileOpen ( vol, test_data->filename, ADF_FILE_MODE_READ );
    if ( ! file ) {
        printf ("Cannot open file %s - aborting...\n", test_data->filename );
        status = 1;
        goto cleanup;
    }

    unsigned int check_eof = test_data->nchecks - 1;
    status += test_seek_eof ( file, test_data->checks[ check_eof ].offset );
    adfFileClose ( file );

cleanup:
    adfVolUnMount ( vol );
    adfDevUnMount ( dev );
    adfDevClose ( dev );

    return status;
}


int test_single_seek ( struct AdfFile *    file,
                       unsigned int        offset,
                       const unsigned char expected_value )
{
#if TEST_VERBOSITY > 0
    printf ( "  Reading data after seek to position 0x%x (%d)...",
             offset, offset );
    fflush ( stdout );
#endif
    ADF_RETCODE rc = adfFileSeek ( file, offset );
    if ( rc != ADF_RC_OK ) {
        fprintf ( stderr, "Seeking to position 0x%x (%d) failed!!!\n",
                  offset, offset );
        return 1;
    }

    unsigned char c;
    unsigned n = adfFileRead ( file, 1, &c );

    if ( n != 1 ) {
        fprintf ( stderr, "Reading data failed after seeking to position 0x%x (%d)!!!\n",
                  offset, offset );
        return 1;
    }

    if ( c != expected_value ) {
        fprintf ( stderr, "Incorrect data read after seeking to position 0x%x (%d):\n"
                  "\texpected 0x%x != read 0x%x\n",
                  offset, offset, (int) expected_value, (int) c );
        return 1;
    }

#if TEST_VERBOSITY > 0
    printf ( " -> OK.\n" );
#endif
    return 0;
}


int test_seek_eof ( struct AdfFile * file,
                    unsigned int     offset )
{
#if TEST_VERBOSITY > 0
    printf ( "  Seeking to EOF position 0x%x (%d)...", offset, offset );
    fflush ( stdout );
    fflush ( stderr );
#endif

    // seek to end and check EOF
    ADF_RETCODE rc = adfFileSeek ( file, offset );
    if ( rc != ADF_RC_OK ) {
        fprintf ( stderr, " -> seeking to 0x%x (%d) failed!!!\n",
                  offset, offset );
        return 1;
    }

    if ( ! adfEndOfFile ( file ) ) {
        fprintf ( stderr, " -> EOF false after seeking to EOF ( 0x%x / %d )!!!\n",
                  offset, offset );
        return 1;
    }

    uint32_t fsize = file->fileHdr->byteSize;
    if ( file->pos != fsize ) {
        fprintf ( stderr, " -> Incorrect file position at EOF: pos 0x%x (%d), size 0x%x (%d)\n",
                  file->pos, file->pos, fsize, fsize );
        return 1;
    }
#if TEST_VERBOSITY > 0
    printf ( " -> OK.\n" );
#endif

    // try to read at EOF
#if TEST_VERBOSITY > 0
    printf ( "  Reading at EOF position 0x%x (%d)...", file->pos, file->pos );
#endif
    unsigned char c;
    unsigned n = adfFileRead ( file, 1, &c );
    if ( n != 0 ) {
        fprintf ( stderr, " -> Length of data read at EOF not zero (%d)!!!\n", n );
        return 1;
    }

    if ( ! adfEndOfFile ( file ) ) {
        fprintf ( stderr, " -> no EOF after reading at EOF!!!\n" );
        return 1;
    }

    if ( file->pos != fsize ) {
        fprintf ( stderr, " -> Incorrect file position at EOF: pos 0x%x (%d), size 0x%x (%d)\n",
                  file->pos, file->pos, fsize, fsize );
        return 1;
    }

#if TEST_VERBOSITY > 0
    printf ( " -> OK.\n" );
#endif
    return 0;
}

