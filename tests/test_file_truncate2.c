#include <check.h>
#include <stdlib.h>

#ifndef _WIN32
#include <unistd.h>   // for unlink()
#endif

#include "adflib.h"
//#include "adf_util.h"
#include "adf_file_util.h"

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
    unsigned           truncsize;
} test_data_t;


void setup ( test_data_t * const tdata );
void teardown ( test_data_t * const tdata );


START_TEST ( test_check_framework )
{
    ck_assert ( 1 );
}
END_TEST


void test_adfFileTruncateGetBlocksToRemove ( test_data_t * const tdata )
{
    struct AdfDevice * const device = tdata->device;
    ck_assert_ptr_nonnull ( device );


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


    ///
    /// create a new file
    ///

    char filename[] = "testfile.tmp";
    struct AdfFile * file = adfFileOpen ( vol, filename, tdata->openMode );
    ck_assert_ptr_nonnull ( file );
    adfFileClose ( file );

    // reset volume state (remount)
    adfVolUnMount ( vol );
    vol = // tdata->vol =
        adfVolMount ( device, 0, ADF_ACCESS_MODE_READWRITE );

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
    ck_assert_int_eq ( adfEndOfFile ( file ), true );
    adfFileClose ( file );

    // the same when open for appending
/*    file = adfFileOpen ( vol, filename, "a" );
    ck_assert_uint_eq ( 0, file->fileHdr->byteSize );
    ck_assert_uint_eq ( 0, file->pos );
    ck_assert_int_eq ( 0, file->posInExtBlk );
    //ck_assert_int_eq ( 0, file->posInDataBlk );
    ck_assert_int_eq ( 0, file->nDataBlock );
    ck_assert_int_eq ( adfEndOfFile ( file ), true );
    adfFileClose ( file );
*/
    // the same when open for writing
    file = adfFileOpen ( vol, filename, ADF_FILE_MODE_WRITE );
    ck_assert_uint_eq ( 0, file->fileHdr->byteSize );
    ck_assert_uint_eq ( 0, file->pos );
    ck_assert_int_eq ( 0, file->posInExtBlk );
    //ck_assert_int_eq ( 0, file->posInDataBlk );
    ck_assert_int_eq ( 0, file->nDataBlock );
    ck_assert_int_eq ( adfEndOfFile ( file ), true );
    adfFileClose ( file );


    ///
    /// write data to the created above, empty file
    ///
    
    // open for writing
    file = adfFileOpen ( vol, filename, ADF_FILE_MODE_WRITE );
    ck_assert_uint_eq ( 0, file->fileHdr->byteSize );
    ck_assert_int_eq ( file->fileHdr->firstData, 0 );
    ck_assert_uint_eq ( 0, file->pos );
    ck_assert_int_eq ( 0, file->posInExtBlk );
    //ck_assert_int_eq ( 0, file->posInDataBlk );
    ck_assert_int_eq ( 0, file->nDataBlock );
    ck_assert_int_eq ( adfEndOfFile ( file ), true );

    // write data buffer to the file
    const unsigned bufsize = tdata->bufsize;
    const unsigned char * const buffer = tdata->buffer;
    unsigned bytes_written = adfFileWrite ( file, bufsize, buffer );
    ck_assert_int_eq ( bufsize, bytes_written );
    ck_assert_uint_eq ( bufsize, file->fileHdr->byteSize );
    ck_assert_int_gt ( file->fileHdr->firstData, 0 );
    ck_assert_uint_eq ( bufsize, file->pos );
    //ck_assert_int_eq ( 0, file->posInExtBlk );
    //ck_assert_int_eq ( datablock2posInExtBlk (
    //                       filesize2datablocks ( bufsize, vol->datablockSize ) ),
    //                   file->posInExtBlk );
    unsigned expected_posInExtBlk = datablock2posInExtBlk (
        filesize2datablocks ( bufsize, vol->datablockSize ) );
    //int expected_posInExtBlk = datablock2posInExtBlk (
    //    pos2datablockIndex ( bufsize, vol->datablockSize ) );
    ck_assert_msg ( expected_posInExtBlk == file->posInExtBlk ||
                    ( file->posInExtBlk == 72 && expected_posInExtBlk == 0 ),
                    "file->posInExtBlk %d != %d, bufsize %u",
                    file->posInExtBlk, expected_posInExtBlk, tdata->bufsize );
    //ck_assert_int_eq ( 1, file->posInDataBlk );
    ck_assert_int_eq ( ( ( bufsize - 1 ) / vol->datablockSize ) + 1, file->nDataBlock );
    ck_assert_int_eq ( adfEndOfFile ( file ), true );
    adfFileClose ( file );


    // reset volume state (remount)
    adfVolUnMount ( vol );
    vol = //tdata->vol =
        adfVolMount ( device, 0, ADF_ACCESS_MODE_READWRITE );

    file = adfFileOpen ( vol, filename, ADF_FILE_MODE_READ );
    ck_assert_ptr_nonnull ( file );

    const unsigned truncsize = tdata->truncsize;
    //printf ( "testing with: bufsize %u, truncsize %u\n",
    //         bufsize, truncsize );
    //fflush(stdout);

    AdfVectorSectors blocks_to_remove;
    ADF_RETCODE rc = adfFileTruncateGetBlocksToRemove ( file, truncsize, &blocks_to_remove );
    //printf ( "testing 2 with : bufsize %u, truncsize %u\n",
    //         bufsize, truncsize );
    //fflush(stdout);
    ck_assert_int_eq ( rc, ADF_RC_OK );

    adfFileClose ( file );

    //const unsigned free_blocks_file_truncated_expected =
    //    free_blocks_before - filesize2blocks ( truncsize, vol->datablockSize );
    const unsigned n_blocks_to_remove_expected =
        adfFileSize2Blocks ( bufsize, vol->datablockSize ) -
        adfFileSize2Blocks ( truncsize, vol->datablockSize );
    
    //ck_assert_uint_eq ( n_blocks_to_remove_expected, blocks_to_remove.len );
    ck_assert_msg ( n_blocks_to_remove_expected == blocks_to_remove.len,
                    "n_blocks_to_remove_expected %d == blocks_to_remove.len %d, "
                    "bufsize %u, truncsize %u",
                    n_blocks_to_remove_expected, blocks_to_remove.len,
                    bufsize, truncsize );
    free ( blocks_to_remove.sectors );

    // umount volume
    adfVolUnMount ( vol );
}

static const unsigned buflen[] = {
    1, 2, 256,
    487, 488, 489,
    511, 512, 513,
    970, 974, 975, 976, 977, 978,
    1022, 1023, 1024, 1025,
    2047, 2048, 2049, 2050,
    4095, 4096, 4097,
    10000, 20000, 35000, 35130,
    35136,
    35137,    // the 1st requiring an ext. block (OFS)
    35138,
    36000, 36864,
    36865,    // the 1st requiring an ext. block (FFS)
    37000, 37380, 40000, 50000,
    60000, 69784, 69785, 69796, 69800, 70000,
    100000, 200000, 512000,
    800000
};
static const unsigned buflensize = sizeof ( buflen ) / sizeof ( unsigned );


static const unsigned truncsizes[] = {
    0, 1, 2, 256,
    487, 488, 489,
    511, 512, 513,
    970, 974, 975, 976, 977, 978,
    1022, 1023, 1024, 1025,
    2047, 2048, 2049, 2050,
    4095, 4096, 4097,
    10000, 20000, 35000, 35130,
    35136,
    35137,    // the 1st requiring an ext. block (OFS)
    35138,
    36000, 36864,
    36865,    // the 1st requiring an ext. block (FFS)
    37000, 37380, 40000, 50000,
    60000, 69784, 69785, 69796, 69800, 70000,
    100000, 200000, 512000,
    800000, 820000
};
static const unsigned ntruncsizes = sizeof ( truncsizes ) / sizeof ( unsigned );


//static const unsigned trunc


START_TEST ( test_file_truncate2_ofs )
{
    test_data_t test_data = {
        .adfname = "test_file_truncate2_ofs.adf",
        .volname = "Test_file_truncate2_ofs",
        .fstype  = 0,          // OFS
        .openMode = ADF_FILE_MODE_WRITE,
        .nVolumeBlocks = 1756
    };
    for ( unsigned i = 0 ; i < buflensize ; ++i ) {
        test_data.bufsize = buflen[i];
        for ( unsigned j = 0 ; j < ntruncsizes ; ++j ) {
            test_data.truncsize = truncsizes[j];
            if ( test_data.bufsize < test_data.truncsize )
                continue;
            setup ( &test_data );
            test_adfFileTruncateGetBlocksToRemove ( &test_data );
            teardown ( &test_data );
        }
    }
}
END_TEST


START_TEST ( test_file_truncate2_ffs )
{
    test_data_t test_data = {
        .adfname = "test_file_truncate2_ffs.adf",
        .volname = "Test_file_truncate2_ffs",
        .fstype  = 1,          // FFS
        .openMode = ADF_FILE_MODE_WRITE,
        .nVolumeBlocks = 1756
    };
    for ( unsigned i = 0 ; i < buflensize ; ++i ) {
        test_data.bufsize = buflen[i];
        for ( unsigned j = 0 ; j < ntruncsizes ; ++j ) {
            test_data.truncsize = truncsizes[j];
            if ( test_data.bufsize < test_data.truncsize )
                continue;
            setup ( &test_data );
            test_adfFileTruncateGetBlocksToRemove ( &test_data );
            teardown ( &test_data );
        }
    }
}
END_TEST


Suite * adflib_suite ( void )
{
    Suite * s = suite_create ( "adflib" );
    
    TCase * tc = tcase_create ( "check framework" );
    tcase_add_test ( tc, test_check_framework );
    suite_add_tcase ( s, tc );

    tc = tcase_create ( "adflib test_file_truncate2_ofs" );
    tcase_add_test ( tc, test_file_truncate2_ofs );
    tcase_set_timeout ( tc, 60 );
    suite_add_tcase ( s, tc );

    tc = tcase_create ( "adflib test_file_truncate2_ffs" );
    //tcase_add_checked_fixture ( tc, setup_ffs, teardown_ffs );
    tcase_add_test ( tc, test_file_truncate2_ffs );
    tcase_set_timeout ( tc, 60 );
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
    tdata->device = adfDevCreate ( "ramdisk", tdata->adfname, 80, 2, 11 );
    if ( ! tdata->device ) {       
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
    if ( ! tdata->buffer )
        exit(1);
    //pattern_AMIGAMIG ( tdata->buffer, tdata->bufsize );
    pattern_random ( tdata->buffer, tdata->bufsize );
}


void teardown ( test_data_t * const tdata )
{
    free ( tdata->buffer );
    tdata->buffer = NULL;

    //adfVolUnMount ( tdata->vol );
    adfDevUnMount ( tdata->device );
    adfDevClose ( tdata->device );
    //if ( unlink ( tdata->adfname ) != 0 )
    //    perror("error deleting the image");
}
