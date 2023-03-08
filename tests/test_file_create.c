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
    int             fstype;   // 0 - OFS, 1 - FFS
    unsigned        nVolumeBlocks;
    char *          openMode;  // "w" or "a"
} test_data_t;


void setup ( test_data_t * const tdata );
void teardown ( test_data_t * const tdata );


START_TEST ( test_check_framework )
{
    ck_assert ( 1 );
}
END_TEST


void test_file_create ( test_data_t * const tdata )
{
    struct AdfDevice * const device = tdata->device;
    ck_assert_ptr_nonnull ( device );

    // mount the test volume
    tdata->vol = adfMount ( tdata->device, 0, FALSE );
    struct AdfVolume * const vol    = tdata->vol;
    ck_assert_ptr_nonnull ( vol );

    // check it is an empty floppy disk
    int free_blocks_before = adfCountFreeBlocks ( vol );
    ck_assert_int_eq ( tdata->nVolumeBlocks, free_blocks_before );
    int nentries = adfDirCountEntries ( vol, vol->curDirPtr );
    ck_assert_int_eq ( 0, nentries ); 

    // create a new file
    char filename[] = "testfile.tmp";
    struct AdfFile * file = adfFileOpen ( vol, filename, tdata->openMode );
    ck_assert_ptr_nonnull ( file );
    adfFileClose ( file );

    // reset volume state (remount)
    adfUnMount ( tdata->vol );
    tdata->vol = adfMount ( tdata->device, 0, FALSE );

    // verify free blocks
    const int file_blocks_used_by_empty_file = 1;
    ck_assert_int_eq ( free_blocks_before - file_blocks_used_by_empty_file,
                       adfCountFreeBlocks ( vol ) );

    // verify the number of entries
    ck_assert_int_eq ( 1, adfDirCountEntries ( vol, vol->curDirPtr ) );

    // verify file information (meta-data)
    file = adfFileOpen ( vol, filename, "r" );
    ck_assert_uint_eq ( 0, file->fileHdr->byteSize );
    ck_assert_uint_eq ( 0, file->pos );
    ck_assert_int_eq ( 0, file->posInExtBlk );
    //ck_assert_int_eq ( 0, file->posInDataBlk );
    ck_assert_int_eq ( 0, file->nDataBlock );
    ck_assert ( adfEndOfFile ( file ) != 0 );
    adfFileClose ( file );

    // the same when open for appending
    file = adfFileOpen ( vol, filename, "a" );
    ck_assert_uint_eq ( 0, file->fileHdr->byteSize );
    ck_assert_uint_eq ( 0, file->pos );
    ck_assert_int_eq ( 0, file->posInExtBlk );
    //ck_assert_int_eq ( 0, file->posInDataBlk );
    ck_assert_int_eq ( 0, file->nDataBlock );
    ck_assert ( adfEndOfFile ( file ) != 0 );
    adfFileClose ( file );

    // the same when open for writing
    file = adfFileOpen ( vol, filename, "w" );
    ck_assert_uint_eq ( 0, file->fileHdr->byteSize );
    ck_assert_uint_eq ( 0, file->pos );
    ck_assert_int_eq ( 0, file->posInExtBlk );
    //ck_assert_int_eq ( 0, file->posInDataBlk );
    ck_assert_int_eq ( 0, file->nDataBlock );
    ck_assert ( adfEndOfFile ( file ) != 0 );
    adfFileClose ( file );
    
    // umount volume
    adfUnMount ( tdata->vol );
}


void test_file_create_with_append ( test_data_t * const tdata )
{
    struct AdfDevice * const device = tdata->device;
    ck_assert_ptr_nonnull ( device );

    // mount the test volume
    tdata->vol = adfMount ( tdata->device, 0, FALSE );
    struct AdfVolume * const vol    = tdata->vol;
    ck_assert_ptr_nonnull ( vol );

    // check it is an empty floppy disk
    int free_blocks_before = adfCountFreeBlocks ( vol );
    ck_assert_int_eq ( tdata->nVolumeBlocks, free_blocks_before );
    int nentries = adfDirCountEntries ( vol, vol->curDirPtr );
    ck_assert_int_eq ( 0, nentries ); 

    // create a new file
    char filename[] = "testfile.tmp";
    struct AdfFile * file = adfFileOpen ( vol, filename, tdata->openMode );
    ck_assert_ptr_null ( file );
    adfFileClose ( file );   // should not be needed (but should not fail either)

    // reset volume state (remount)
    adfUnMount ( tdata->vol );
    tdata->vol = adfMount ( tdata->device, 0, FALSE );

    // verify free blocks
    ck_assert_int_eq ( free_blocks_before,
                       adfCountFreeBlocks ( vol ) );

    // verify the number of entries
    ck_assert_int_eq ( 0, adfDirCountEntries ( vol, vol->curDirPtr ) );
    
    // umount volume
    adfUnMount ( tdata->vol );
}



START_TEST ( test_file_create_empty_ofs_write )
{
    test_data_t test_data = {
        .adfname = "test_file_create_ofs_write.adf",
        .volname = "Test_file_create_ofs",
        .fstype  = 0,          // OFS
        .openMode = "w",
        .nVolumeBlocks = 1756
    };
    setup ( &test_data );
    test_file_create ( &test_data );
    teardown ( &test_data );
}

START_TEST ( test_file_create_empty_ffs_write )
{
    test_data_t test_data = {
        .adfname = "test_file_create_ffs_write.adf",
        .volname = "Test_file_create_ffs",
        .fstype  = 1,          // FFS
        .openMode = "w",
        .nVolumeBlocks = 1756
    };
    setup ( &test_data );
    test_file_create ( &test_data );
    teardown ( &test_data );
}


START_TEST ( test_file_create_empty_ofs_append )
{
    test_data_t test_data = {
        .adfname = "test_file_create_ofs_append.adf",
        .volname = "Test_file_create_ofs",
        .fstype  = 0,          // OFS
        .openMode = "a",
        .nVolumeBlocks = 1756
    };
    setup ( &test_data );
    test_file_create_with_append ( &test_data );
    teardown ( &test_data );
}

START_TEST ( test_file_create_empty_ffs_append)
{
    test_data_t test_data = {
        .adfname = "test_file_create_ffs_append.adf",
        .volname = "Test_file_create_ffs",
        .fstype  = 1,          // FFS
        .openMode = "a",
        .nVolumeBlocks = 1756
    };
    setup ( &test_data );
    test_file_create_with_append ( &test_data );
    teardown ( &test_data );
}


Suite * adflib_suite ( void )
{
    Suite * s = suite_create ( "adflib" );
    
    TCase * tc = tcase_create ( "check framework" );
    tcase_add_test ( tc, test_check_framework );
    suite_add_tcase ( s, tc );

    tc = tcase_create ( "adflib test_file_create_empty_ofs_write" );
    //tcase_add_checked_fixture ( tc, setup_ffs, teardown_ffs );
    tcase_add_test ( tc, test_file_create_empty_ofs_write );
    suite_add_tcase ( s, tc );

    tc = tcase_create ( "adflib test_file_create_empty_ffs_write" );
    tcase_add_test ( tc, test_file_create_empty_ffs_write );
    suite_add_tcase ( s, tc );

    tc = tcase_create ( "adflib test_file_create_empty_ofs_append" );
    tcase_add_test ( tc, test_file_create_empty_ofs_append );
    suite_add_tcase ( s, tc );

    tc = tcase_create ( "adflib test_file_create_empty_ffs_append" );
    tcase_add_test ( tc, test_file_create_empty_ffs_append );
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
    adfCreateFlop ( tdata->device, tdata->volname, 0 );

    //tdata->vol = adfMount ( tdata->device, 0, FALSE );
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
