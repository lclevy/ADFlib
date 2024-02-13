#include <check.h>
#include <stdlib.h>

#ifndef _WIN32
#include <unistd.h>   // for unlink()
#endif

#include "adflib.h"
//#include "adf_util.h"
#include "adf_file_util.h"
#include "test_util.h"

#define TEST_VERBOSITY 0

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


void test_file_truncate ( test_data_t * const tdata )
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
    ck_assert_ptr_nonnull ( file );
    ck_assert_uint_eq ( 0, file->fileHdr->byteSize );
    ck_assert_uint_eq ( 0, file->pos );
    ck_assert_int_eq ( 0, file->posInExtBlk );
    //ck_assert_int_eq ( 0, file->posInDataBlk );
    ck_assert_int_eq ( 0, file->nDataBlock );
    ck_assert_int_eq ( adfEndOfFile ( file ), TRUE );
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
    ck_assert_int_eq ( adfEndOfFile ( file ), TRUE );

    // write data buffer to the file
    const unsigned bufsize = tdata->bufsize;
    const unsigned char * const buffer = tdata->buffer;
    unsigned bytes_written = adfFileWrite ( file, bufsize, buffer );
    ck_assert_int_eq ( bufsize, bytes_written );
    adfFileClose ( file );

    // reset volume state (remount)
    adfVolUnMount ( vol );
    vol = //tdata->vol =
        adfVolMount ( device, 0, ADF_ACCESS_MODE_READWRITE );

    // verify free blocks
    //ck_assert_int_eq ( free_blocks_before - file_blocks_used_by_empty_file - 1,
    //                   adfCountFreeBlocks ( vol ) );
    //int expected_free_blocks = free_blocks_before - file_blocks_used_by_empty_file - 1;
    const unsigned free_blocks_with_file_expected =
        free_blocks_before - filesize2blocks ( bufsize, vol->datablockSize );
    const unsigned free_blocks_with_file = adfCountFreeBlocks ( vol );
    ck_assert_msg (
        free_blocks_with_file == free_blocks_with_file_expected,
        "Free blocks after writing the file incorrect: %u (should be %u), bufsize %u",
        free_blocks_with_file, free_blocks_with_file_expected, bufsize );

    // verify the number of entries
    ck_assert_int_eq ( 1, adfDirCountEntries ( vol, vol->curDirPtr ) );

    // verify file information (meta-data)
    file = adfFileOpen ( vol, filename, ADF_FILE_MODE_READ );
    ck_assert_uint_eq ( bufsize, file->fileHdr->byteSize );
    ck_assert_int_gt ( file->fileHdr->firstData, 0 );
    ck_assert_uint_eq ( 0, file->pos );
    ck_assert_int_eq ( 0, file->posInExtBlk );
    //ck_assert_int_eq ( 0, file->posInDataBlk );
    ck_assert_int_eq ( 1, file->nDataBlock );
    ck_assert_int_eq ( adfEndOfFile ( file ), FALSE );    
    adfFileClose ( file );

    //
    // test truncating the file
    // 
    const unsigned truncsize = tdata->truncsize;
    
    // truncate the file
    file = adfFileOpen ( vol, filename, ADF_FILE_MODE_WRITE );
    ck_assert_ptr_nonnull ( file );

#if ( TEST_VERBOSITY >= 1 )
    printf ( "Testing with: bufsize %u, truncsize %u, dblock size %u\n",
             bufsize, truncsize, file->volume->datablockSize );
    fflush(stdout);
#endif
    //ck_assert_int_eq ( RC_OK, adfFileTruncate ( file, truncsize ) );
    ck_assert_msg ( adfFileTruncate ( file, truncsize ) == RC_OK,
        "adfFileTruncate failed, bufsize %u, truncsize %u",
        bufsize, truncsize );

    // check file metadata before closing the file
    //ck_assert_uint_eq ( truncsize, file->pos );
    ck_assert_msg (  truncsize == file->pos,
                     " truncsize %u != file->pos %u, bufsize %u, truncsize %u",
                     truncsize, file->pos, bufsize, truncsize );
    ck_assert_uint_eq ( truncsize, file->fileHdr->byteSize );
    adfFileClose ( file );

    // reset volume state (remount)
    adfVolUnMount ( vol );
    vol = adfVolMount ( device, 0, ADF_ACCESS_MODE_READWRITE );
    
    // check volume metadata after truncating
    ck_assert_int_eq ( 1, adfDirCountEntries ( vol, vol->curDirPtr ) );

    const unsigned free_blocks_file_truncated_expected =
        free_blocks_before - filesize2blocks ( truncsize, vol->datablockSize );
    const unsigned free_blocks_file_truncated = adfCountFreeBlocks ( vol );
    ck_assert_msg (
        free_blocks_file_truncated == free_blocks_file_truncated_expected,
        "Free blocks after truncating incorrect: %u (should be %u), bufsize %u, truncsize %u",
        free_blocks_file_truncated, free_blocks_file_truncated_expected, bufsize, truncsize );
    
    // reread the truncated file
    file = adfFileOpen ( vol, filename, ADF_FILE_MODE_READ );
    ck_assert_ptr_nonnull ( file );
    unsigned char * rbuf = malloc ( truncsize + 1 );
    ck_assert_ptr_nonnull ( rbuf );
    unsigned bytes_read = adfFileRead ( file, truncsize + 1, rbuf );
    free ( rbuf );
    rbuf = NULL;

    ck_assert_int_eq ( truncsize, bytes_read );
    ck_assert_uint_eq ( truncsize, file->pos );

    //ck_assert_int_eq ( 0, file->posInExtBlk );  // TODO
    //ck_assert_int_eq ( 1, file->posInDataBlk );

    /*unsigned expected_nDataBlock =
        //( ( bufsize - 1 ) / vol->datablockSize ) + 1;
        //filesize2datablocks ( bufsize, vol->datablockSize ) + 1;
        pos2datablockIndex ( (unsigned) max ( (int) truncsize - 1, 0 ), vol->datablockSize ) + 1;
    //ck_assert_int_eq ( expected_nDataBlock, file->nDataBlock );
   ck_assert_msg ( file->nDataBlock == expected_nDataBlock,
                    "file->nDataBlock %d == expected %d, truncsize %u",
                    file->nDataBlock, expected_nDataBlock, truncsize );
    */
    ck_assert_int_eq ( adfEndOfFile ( file ), TRUE );

    //printf ("file->pos %u\n", file->pos);
    //fflush(stdout);

    unsigned nDataBlocks = adfFileSize2Datablocks ( file->fileHdr->byteSize, vol->datablockSize );
    unsigned nExtBlocks  = adfFileDatablocks2Extblocks ( nDataBlocks );
#if ( TEST_VERBOSITY >= 1 )
    printf ("nDataBlocks %u, nExtBlocks %u, bufsize %u, truncsize %u\n",
            nDataBlocks, nExtBlocks, bufsize, truncsize );
    fflush(stdout);
#endif

    // make sure we have available current ext. block (if needed)
    /*ck_assert_int_eq ( adfFileSeek ( file, 0 ), RC_OK );
    ck_assert_int_eq ( adfFileSeek ( file, truncsize + 1 ), RC_OK );
    ck_assert_int_eq ( adfEndOfFile ( file ), TRUE );
    int32_t * const dataBlocks = ( nExtBlocks < 1 ) ? file->fileHdr->dataBlocks :
                                                      file->currentExt->dataBlocks;
    */
    struct bFileExtBlock * fext = NULL;
    int32_t * dataBlocks = NULL;
    if ( nExtBlocks < 1 ) {
        dataBlocks = file->fileHdr->dataBlocks;
    } else {  // ( nExtBlocks >= 1 )
        if  ( vol->datablockSize != 488 ) {
            // FFS
            ck_assert_ptr_nonnull ( file->currentExt );
            dataBlocks = file->currentExt->dataBlocks;
        } else {
            // for OFS - we must read the current ext.(!)
            fext =  malloc ( sizeof (struct bFileExtBlock) );
            ck_assert_ptr_nonnull ( fext );
            ck_assert_int_eq ( adfFileReadExtBlockN ( file, (int) nExtBlocks - 1, fext ),
                               RC_OK );
            dataBlocks = fext->dataBlocks;
        }
    }

    // check the number of non-zero blocks in the array of the last metadata block (header or ext)
    unsigned nonZeroCount = 0;
    for ( unsigned i = 0 ; i < MAX_DATABLK ; ++i ) {
        if ( dataBlocks[i] != 0 ) {
            //printf ("A non-zero block %u: %d\n", i, dataBlocks[i] );
            nonZeroCount++;
        }
    }
    free(fext);

    if ( file->fileHdr->byteSize == 0 )
        ck_assert_uint_eq ( nonZeroCount, 0 );
    else {
        unsigned nonZeroExpected = ( nDataBlocks % MAX_DATABLK != 0 ?
                                     nDataBlocks % MAX_DATABLK :
                                     MAX_DATABLK );
        //ck_assert_uint_eq ( nonZeroCount, nonZeroExpected );
        ck_assert_msg ( nonZeroCount == nonZeroExpected,
                        "Incorrect number of non-zero blocks in the last metadata block:"
                        "nonZeroCount %u != nonZeroExpected %u, bufsize %u, truncsize %u",
                        nonZeroCount, nonZeroExpected, bufsize, truncsize );
    }
    adfFileClose ( file );

    // verify data of the truncated file

    // this verifies up to the size of the buffer
    unsigned commonSize = min ( bufsize, truncsize );
    ck_assert_msg ( verify_file_data ( vol, filename, buffer, commonSize, 10 ) == 0,
                    "Data verification failed, bufsize %u (0x%x), "
                    "truncsize %u (0x%x), commonsize %u (0x%x)",
                    bufsize, bufsize, truncsize, truncsize, commonSize, commonSize );

    // if enlarging the file... 
    if ( truncsize > bufsize ) {
        // ...  must also check the zeroed part

#if ( TEST_VERBOSITY >= 1 )
        printf ("Checking the enlarged (zero-filled) part\n");
        fflush (stdout);
#endif
        file = adfFileOpen ( vol, filename, ADF_FILE_MODE_READ );
        ck_assert_ptr_nonnull ( file );
        adfFileSeek ( file, bufsize );
        size_t extrasize = truncsize - bufsize;
        uint8_t * ebuf = malloc ( extrasize );
        ck_assert_ptr_nonnull ( ebuf );
        ck_assert_uint_eq ( adfFileRead (file, (uint32_t) extrasize, ebuf ), extrasize );
        for ( unsigned i = 0 ; i < extrasize ; ++i ) {
            if ( ebuf[i] != 0 ) {
                ck_assert_msg ( 0,
                                "Non-zero data in enlarged/zeroed part, offset 0x%x (%u),"
                                " bufsize %u (0x%x), truncsize %u (0x%x)\n",
                                bufsize + i, bufsize + i,
                                bufsize, bufsize, truncsize, truncsize );
            }
        }
        free ( ebuf );
        adfFileClose ( file ); 
    }
    
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
    37000, 37380, 40000,
    60000, 69784, 69785, 69796, 69800, 70000,
    100000,
    //200000, 512000, 800000,
    820000
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
    //10000, 20000, 35000, 35130,
    35136,
    35137,    // the 1st requiring an ext. block (OFS)
    35138,
    36000, 36864,
    36865,    // the 1st requiring an ext. block (FFS)
    37000, 37380, 40000,
    60000, 69784, 69785, 69796, 69800, 70000,
    100000,
    //200000, 512000, 800000,
    //820000
};
static const unsigned ntruncsizes = sizeof ( truncsizes ) / sizeof ( unsigned );


START_TEST ( test_file_truncate_ofs )
{
    test_data_t test_data = {
        .adfname = "test_file_truncate_ofs.adf",
        .volname = "Test_file_truncate_ofs",
        .fstype  = 0,          // OFS
        .openMode = ADF_FILE_MODE_WRITE,
        .nVolumeBlocks = 1756
    };
    for ( unsigned i = 0 ; i < buflensize ; ++i ) {
        test_data.bufsize = buflen[i];
        for ( unsigned j = 0 ; j < ntruncsizes ; ++j ) {
            test_data.truncsize = truncsizes[j];
            //if ( test_data.bufsize < test_data.truncsize )
            //    continue;
            setup ( &test_data );
            test_file_truncate ( &test_data );
            teardown ( &test_data );
        }
    }
}
END_TEST


START_TEST ( test_file_truncate_ffs )
{
    test_data_t test_data = {
        .adfname = "test_file_truncate_ffs.adf",
        .volname = "Test_file_truncate_ffs",
        .fstype  = 1,          // FFS
        .openMode = ADF_FILE_MODE_WRITE,
        .nVolumeBlocks = 1756
    };
    for ( unsigned i = 0 ; i < buflensize ; ++i ) {
        test_data.bufsize = buflen[i];
        for ( unsigned j = 0 ; j < ntruncsizes ; ++j ) {
            test_data.truncsize = truncsizes[j];
            //if ( test_data.bufsize < test_data.truncsize )
            //    continue;
            setup ( &test_data );
            test_file_truncate ( &test_data );
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

    tc = tcase_create ( "adflib test_file_truncate_ofs" );
    tcase_add_test ( tc, test_file_truncate_ofs );
    tcase_set_timeout ( tc, 300 );
    suite_add_tcase ( s, tc );

    tc = tcase_create ( "adflib test_file_truncate_ffs" );
    //tcase_add_checked_fixture ( tc, setup_ffs, teardown_ffs );
    tcase_add_test ( tc, test_file_truncate_ffs );
    tcase_set_timeout ( tc, 300 );
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
    if ( adfCreateFlop ( tdata->device, tdata->volname, tdata->fstype ) != RC_OK ) {
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
