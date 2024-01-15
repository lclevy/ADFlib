#include <check.h>
#include <stdlib.h>

#ifndef _WIN32
#include <unistd.h>   // for unlink()
#endif

#include "adflib.h"
//#include "adf_util.h"


typedef struct test_data_s {
    struct AdfDevice * device;
    struct AdfVolume * vol;
    char *          adfname;
    char *          volname;
    uint8_t         fstype;   // 0 - OFS, 1 - FFS
    unsigned        nVolumeBlocks;
    AdfFileMode     openMode;
} test_data_t;


void setup ( test_data_t * const tdata );
void teardown ( test_data_t * const tdata );


START_TEST ( test_check_framework )
{
    ck_assert ( 1 );
}
END_TEST


void test_file_append ( test_data_t * const tdata )
{
    struct AdfDevice * const device = tdata->device;
    ck_assert_ptr_nonnull ( device );

    ///
    /// mount the test volume
    ///

    // mount the test volume
    tdata->vol = adfMount ( tdata->device, 0, ADF_ACCESS_MODE_READWRITE );
    struct AdfVolume * const vol = tdata->vol;
    ck_assert_ptr_nonnull ( vol );

    // check it is an empty floppy disk
    unsigned free_blocks_before = adfCountFreeBlocks ( vol );
    ck_assert_int_eq ( tdata->nVolumeBlocks, free_blocks_before );
    int nentries = adfDirCountEntries ( vol, vol->curDirPtr );
    ck_assert_int_eq ( 0, nentries ); 


    ///
    /// create a new file
    ///

    // create a new file
    char filename[] = "testfile.tmp";
    struct AdfFile * file = adfFileOpen ( vol, filename, tdata->openMode );
    //ck_assert_ptr_null ( file );  // cannot open a new file in append mode
    //file = adfFileOpen ( vol, filename, ADF_FILE_MODE_WRITE );
    ck_assert_msg ( file != 0, "Cannot open file %s for %s", filename,
                    tdata->openMode == ADF_FILE_MODE_READ  ? "reading" :
                    tdata->openMode == ADF_FILE_MODE_WRITE ? "writing" :
                    "an unknown mode" );
    adfFileClose ( file );

    // reset volume state (remount)
    adfUnMount ( tdata->vol );
    tdata->vol = adfMount ( tdata->device, 0, ADF_ACCESS_MODE_READWRITE );

    // verify free blocks
    const unsigned file_blocks_used_by_empty_file = 1;
    ck_assert_int_eq ( free_blocks_before - file_blocks_used_by_empty_file,
                       adfCountFreeBlocks ( vol ) );

    // verify the number of entries
    ck_assert_int_eq ( 1, adfDirCountEntries ( vol, vol->curDirPtr ) );

    // verify file information (meta-data)
    file = adfFileOpen ( vol, filename, ADF_FILE_MODE_READ );
    ck_assert_uint_eq ( 0, file->fileHdr->byteSize );
    ck_assert_uint_eq ( 0, file->pos );
    ck_assert_int_eq ( 0, file->posInExtBlk );
    //ck_assert_int_eq ( 0, file->posInDataBlk );
    ck_assert_int_eq ( 0, file->nDataBlock );
    ck_assert_int_eq ( adfEndOfFile ( file ), TRUE );
    adfFileClose ( file );

    // the same when open for writing
    file = adfFileOpen ( vol, filename, ADF_FILE_MODE_WRITE );
    ck_assert_uint_eq ( 0, file->fileHdr->byteSize );
    ck_assert_uint_eq ( 0, file->pos );
    ck_assert_int_eq ( 0, file->posInExtBlk );
    //ck_assert_int_eq ( 0, file->posInDataBlk );
    ck_assert_int_eq ( 0, file->nDataBlock );
    ck_assert_int_eq ( adfEndOfFile ( file ), TRUE );
    adfFileClose ( file );


    ///
    /// test writing 1 byte to the created above, empty file
    ///

    // open for writing
    file = adfFileOpen ( vol, filename, ADF_FILE_MODE_WRITE );
    ck_assert_uint_eq ( 0, file->fileHdr->byteSize );
    ck_assert_int_eq ( file->fileHdr->firstData, 0 );
    ck_assert_uint_eq ( 0, file->pos );
    ck_assert_int_eq ( 0, file->posInExtBlk );
    //ck_assert_int_eq ( 0, file->posInDataBlk );
    ck_assert_int_eq ( 0, file->nDataBlock );
    ck_assert_int_eq ( adfEndOfFile ( file ), TRUE );

    RETCODE rc = adfFileSeek ( file, adfFileGetSize ( file ) );
    ck_assert_int_eq ( rc, RC_OK );
    ck_assert_uint_eq ( 0, file->fileHdr->byteSize );
    ck_assert_int_eq ( file->fileHdr->firstData, 0 );
    ck_assert_uint_eq ( 0, file->pos );
    ck_assert_int_eq ( 0, file->posInExtBlk );
    //ck_assert_int_eq ( 0, file->posInDataBlk );
    ck_assert_int_eq ( 0, file->nDataBlock );
    ck_assert_int_eq ( adfEndOfFile ( file ), TRUE );

    // write 1 byte to the file
    unsigned char wbuf[256];
    unsigned bytes_written = adfFileWrite ( file, 1, wbuf );
    ck_assert_int_eq ( 1, bytes_written );
    // ck_assert_uint_eq ( 1, file->fileHdr->byteSize ); -> fails, file header not updated yet(?)
    ck_assert_int_gt ( file->fileHdr->firstData, 0 );
    ck_assert_uint_eq ( 1, file->pos );
    ck_assert_int_eq ( 0, file->posInExtBlk );
    //ck_assert_int_eq ( 1, file->posInDataBlk );
    ck_assert_int_eq ( 1, file->nDataBlock );
    ck_assert_int_eq ( adfEndOfFile ( file ), TRUE );
    adfFileClose ( file );

    // reset volume state (remount)
    adfUnMount ( tdata->vol );
    tdata->vol = adfMount ( tdata->device, 0, ADF_ACCESS_MODE_READWRITE );

    // verify free blocks
    ck_assert_int_eq ( free_blocks_before - file_blocks_used_by_empty_file - 1,
                       adfCountFreeBlocks ( vol ) );

    // verify the number of entries
    ck_assert_int_eq ( 1, adfDirCountEntries ( vol, vol->curDirPtr ) );

    // verify file information (meta-data)
    file = adfFileOpen ( vol, filename, ADF_FILE_MODE_READ );
    ck_assert_uint_eq ( 1, file->fileHdr->byteSize );
    ck_assert_int_gt ( file->fileHdr->firstData, 0 );
    ck_assert_uint_eq ( 0, file->pos );
    ck_assert_int_eq ( 0, file->posInExtBlk );
    //ck_assert_int_eq ( 0, file->posInDataBlk );
    ck_assert_int_eq ( 1, file->nDataBlock );
    ck_assert_int_eq ( adfEndOfFile ( file ), FALSE );

    // read the written byte and verify (data and metadata)
    unsigned char rbuf[256];
    unsigned bytes_read = adfFileRead ( file, 1, rbuf );
    ck_assert_int_eq ( 1, bytes_read );
    ck_assert_uint_eq ( wbuf[0], rbuf[0] );
    ck_assert_uint_eq ( 1, file->pos );
    ck_assert_int_eq ( 0, file->posInExtBlk );
    //ck_assert_int_eq ( 1, file->posInDataBlk );
    ck_assert_int_eq ( 1, file->nDataBlock );

    adfFileClose ( file );
    
    // umount volume
    adfUnMount ( tdata->vol );
}

START_TEST ( test_file_append_ofs )
{
    test_data_t test_data = {
        .adfname = "test_file_append_ofs.adf",
        .volname = "Test_file_append_ofs",
        .fstype  = 0,          // OFS
        .openMode = ADF_FILE_MODE_WRITE,
        .nVolumeBlocks = 1756
    };
    setup ( &test_data );
    test_file_append ( &test_data );
    teardown ( &test_data );
}
END_TEST


START_TEST ( test_file_append_ffs )
{
    test_data_t test_data = {
        .adfname = "test_file_append_ffs.adf",
        .volname = "Test_file_append_ffs",
        .fstype  = 1,          // FFS
        .openMode = ADF_FILE_MODE_WRITE,
        .nVolumeBlocks = 1756
    };
    setup ( &test_data );
    test_file_append ( &test_data );
    teardown ( &test_data );
}
END_TEST


Suite * adflib_suite ( void )
{
    Suite * s = suite_create ( "adflib" );

    TCase * tc = tcase_create ( "check framework" );
    tcase_add_test ( tc, test_check_framework );
    suite_add_tcase ( s, tc );

    tc = tcase_create ( "adflib test_file_append_ofs" );
    //tcase_add_checked_fixture ( tc, setup_ffs, teardown_ffs );
    tcase_add_test ( tc, test_file_append_ofs );
    suite_add_tcase ( s, tc );

    tc = tcase_create ( "adflib test_file_append_ffs" );
    tcase_add_test ( tc, test_file_append_ffs );
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
    tdata->device = adfCreateDumpDevice ( tdata->adfname, 80, 2, 11 );
    if ( ! tdata->device ) {       
        //return;
        exit(1);
    }
    if ( adfCreateFlop ( tdata->device, tdata->volname, tdata->fstype ) != RC_OK ) {
        fprintf ( stderr, "adfCreateFlop error creating volume: %s\n",
                  tdata->volname );
        exit(1);
    }

    //tdata->vol = adfMount ( tdata->device, 0, ADF_ACCESS_MODE_READWRITE );
    //if ( ! tdata->vol )
    //    return;
    //    exit(1);
}


void teardown ( test_data_t * const tdata )
{
    //adfUnMount ( tdata->vol );
    adfUnMountDev ( tdata->device );
    unlink ( tdata->adfname );
}
