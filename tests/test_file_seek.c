#include <check.h>
#include <stdlib.h>

#ifndef _WIN32
#include <unistd.h>   // for unlink()
#endif

#include "adflib.h"
//#include "adf_util.h"
#include "test_util.h"


typedef struct test_data_s {
    struct AdfDevice * device;
//    struct AdfVolume * vol;
    char *             adfname;
    char *             volname;
    uint8_t            fstype;   // 0 - OFS, 1 - FFS
    unsigned           nVolumeBlocks;
    AdfFileMode        openMode;
    unsigned char *    buffer;
    unsigned           bufsize;
} test_data_t;


void setup ( test_data_t * const tdata );
void teardown ( test_data_t * const tdata );


START_TEST ( test_check_framework )
{
    ck_assert ( 1 );
}
END_TEST


void test_file_seek_eof ( test_data_t * const tdata )
{
    struct AdfDevice * const device = tdata->device;
    ck_assert_ptr_nonnull ( device );
    unsigned char * const buffer = tdata->buffer;
    const unsigned bufsize = tdata->bufsize;

    ///
    /// mount the test volume
    ///

    // mount the test volume
    struct AdfVolume * vol = // tdata->vol =
        adfVolMount ( device, 0, ADF_ACCESS_MODE_READWRITE );
    ck_assert_ptr_nonnull ( vol );

    // check it is an empty floppy disk
    unsigned free_blocks_before = adfCountFreeBlocks ( vol );
    ck_assert_int_eq ( tdata->nVolumeBlocks, free_blocks_before );
    int nentries = adfDirCountEntries ( vol, vol->curDirPtr );
    ck_assert_int_eq ( 0, nentries ); 

    // put random data in the buffer
    pattern_random ( buffer, bufsize );

    ///
    /// write the data to the file
    ///

    const char filename[] = "testfile.tmp";
    struct AdfFile * file = adfFileOpen ( vol, filename, tdata->openMode );
    ck_assert_ptr_nonnull ( file );
    unsigned bytes_written = (unsigned) adfFileWrite ( file, bufsize, buffer );
    ck_assert_uint_eq ( bufsize, bytes_written );
    adfFileClose ( file );

    // reset volume state (remount)
    adfVolUnMount ( vol );
    vol = // tdata->vol =
        adfVolMount ( device, 0, ADF_ACCESS_MODE_READWRITE );
    
    // reopen the file
    file = adfFileOpen ( vol, filename, ADF_FILE_MODE_READ );
    ck_assert_ptr_nonnull ( file );
    ck_assert_uint_eq ( file->fileHdr->byteSize, bufsize );

    const unsigned seek_offsets_from_eof[] = {
        0, 1, 2, 3, 4,
        255, 256, 257,
        487, 488, 489,
        511, 512, 513,
        970, 974, 975, 976, 977, 978,
        1022, 1023, 1024, 1025,
        2047, 2048, 2049, 2050,
        4095, 4096, 4097,
        //10000, 20000, 35000, 35130,
        35136,
        35137,          // min. file size requiring an ext block (OFS)
        35138,
        //36000,
        36864,
        36865,          // min. file size requiring an ext block (FFS)
        37250, 37370, 37380,
        38000,
        60000, 69784, 69785, 69796, 69800, 70000,
        100000, 200000,
        512000, 800000
    };
    const unsigned n_seek_offsets_from_eof = sizeof(seek_offsets_from_eof) / sizeof(unsigned);

    for ( unsigned i = 0 ; i < n_seek_offsets_from_eof ; ++i ) {

        // checking: eof pos. - offset
        unsigned offset = file->fileHdr->byteSize >= seek_offsets_from_eof[i] ?
            file->fileHdr->byteSize - seek_offsets_from_eof[i] : 0;
        ck_assert_int_eq ( adfFileSeek ( file, offset ), ADF_RC_OK );
        ck_assert_uint_eq ( file->fileHdr->byteSize, bufsize );
        ck_assert_uint_eq ( file->pos, offset );

        // checking: eof pos. + offset
        offset = file->fileHdr->byteSize + seek_offsets_from_eof[i];
        ck_assert_int_eq ( adfFileSeek ( file, offset ), ADF_RC_OK );
        ck_assert_uint_eq ( file->fileHdr->byteSize, bufsize );
        ck_assert_uint_eq ( file->pos, file->fileHdr->byteSize );
    }
    ck_assert_uint_eq ( file->fileHdr->byteSize, bufsize );
    adfFileClose ( file );

    // verify file data
    const bool data_valid =
        ( verify_file_data ( vol, filename, buffer, bufsize, 10 ) == 0 );
    ck_assert_msg ( data_valid,
                    "Data verification failed: "
                    "bufsize %u, vol type %s, dblock size %u\n",
                    bufsize,
                    ( tdata->fstype & 1 ) == 0 ? "OFS" : "FFS",
                    vol->datablockSize );

    // umount volume
    adfVolUnMount ( vol );
}


static const unsigned buflen[] = {
    0, 1, 2,
    255, 256, 257,
    487, 488, 489,
    511, 512, 513,
    970, 974, 975, 976, 977, 978,
    1022, 1023, 1024, 1025,
    2047, 2048, 2049, 2050,
    4095, 4096, 4097,
    //10000, 20000, 35000, 35130,
    35136,
    35137,          // min. file requiring an ext block (OFS)
    35138,
    //36000,
    36864,
    36865,          // min. file requiring an ext block (FFS)
    37250, 37370, 37380,
    38000,
    //40000, 50000,
    60000, 69784, 69785, 69796, 69800, 70000,
    //100000, 200000,
    512000, 800000
};
static const unsigned buflensize = sizeof ( buflen ) / sizeof (unsigned);


START_TEST ( test_file_seek_eof_ofs )
{
    test_data_t test_data = {
        .adfname = "test_file_seek_eof_ofs.adf",
        .volname = "Test_file_seek_eof_ofs",
        .fstype  = 0,          // OFS
        .openMode = ADF_FILE_MODE_WRITE,
        .nVolumeBlocks = 1756
    };
    for ( unsigned i = 0 ; i < buflensize ; ++i )  {
        test_data.bufsize = buflen[i];
        setup ( &test_data );
        test_file_seek_eof ( &test_data );
        teardown ( &test_data );
    }
}
END_TEST


START_TEST ( test_file_seek_eof_ffs )
{
    test_data_t test_data = {
        .adfname = "test_file_seek_eof_ffs.adf",
        .volname = "Test_file_seek_eof_ffs",
        .fstype  = 1,          // FFS
        .openMode = ADF_FILE_MODE_WRITE,
        .nVolumeBlocks = 1756
    };
    for ( unsigned i = 0 ; i < buflensize ; ++i )  {
        test_data.bufsize = buflen[i];
        setup ( &test_data );
        test_file_seek_eof ( &test_data );
        teardown ( &test_data );
    }
}
END_TEST


Suite * adflib_suite ( void )
{
    Suite * s = suite_create ( "adflib" );

    TCase * tc = tcase_create ( "check framework" );
    tcase_add_test ( tc, test_check_framework );
    tcase_set_timeout ( tc, 30 );    
    suite_add_tcase ( s, tc );

    tc = tcase_create ( "adflib test_file_seek_eof_ofs" );
    tcase_add_test ( tc, test_file_seek_eof_ofs );
    tcase_set_timeout ( tc, 30 );    
    suite_add_tcase ( s, tc );

    tc = tcase_create ( "adflib test_file_seek_eof_ffs" );
    //tcase_add_checked_fixture ( tc, setup_ffs, teardown_ffs );
    tcase_add_test ( tc, test_file_seek_eof_ffs );
    tcase_set_timeout ( tc, 30 );    
    suite_add_tcase ( s, tc );

    return s;
}


int main ( void )
{
    Suite * s = adflib_suite();
    SRunner * sr = srunner_create ( s );

    adfEnvInitDefault();
    srunner_run_all ( sr, CK_VERBOSE ); //CK_NORMAL );
    adfEnvCleanUp();

    int number_failed = srunner_ntests_failed ( sr );
    srunner_free ( sr );
    return ( number_failed == 0 ) ?
        EXIT_SUCCESS :
        EXIT_FAILURE;
}


void setup ( test_data_t * const tdata )
{
    tdata->device = adfDevCreate ( "dump", tdata->adfname, 80, 2, 11 );
    if ( tdata->device == NULL ) {
        //return;
        exit(1);
    }
    if ( adfCreateFlop ( tdata->device, tdata->volname, tdata->fstype ) != ADF_RC_OK ) {
        fprintf ( stderr, "adfCreateFlop error creating volume: %s\n",
                  tdata->volname );
        exit(1);
    }

    //tdata->vol = adfVolMount ( tdata->device, 0, ADF_ACCESS_MODE_READWRITE );
    //if ( ! tdata->vol )
    //    return;
    //    exit(1);
    tdata->buffer = malloc ( tdata->bufsize );
    if ( tdata->buffer == NULL )
        exit(1);
    //pattern_AMIGAMIG ( tdata->buffer, tdata->bufsize );
    //pattern_random ( tdata->buffer, tdata->bufsize );
}


void teardown ( test_data_t * const tdata )
{
    free ( tdata->buffer );
    tdata->buffer = NULL;

    //adfVolUnMount ( tdata->vol );
    adfDevUnMount ( tdata->device );
    adfDevClose ( tdata->device );
    if ( unlink ( tdata->adfname ) != 0 )
        perror("error deleting the image");
}
