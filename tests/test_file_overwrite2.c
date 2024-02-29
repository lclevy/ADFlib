#include <check.h>
#include <stdlib.h>

#ifndef _WIN32
#include <unistd.h>   // for unlink()
#endif

#include "test_util.h"

#define TEST_VERBOSITY 0


typedef struct test_data_s {
    struct AdfDevice * device;
    //struct AdfVolume * vol;
    char *          adfname;
    char *          volname;
    uint8_t         fstype;   // 0 - OFS, 1 - FFS
    unsigned        nVolumeBlocks;
//    AdfFileMode          openMode;
    unsigned char * buffer;
    unsigned        bufsize;    // at least 2 bytes
} test_data_t;


void setup ( test_data_t * const tdata );
void teardown ( test_data_t * const tdata );


START_TEST ( test_check_framework )
{
    ck_assert ( 1 );
}
END_TEST


void test_file_overwrite ( test_data_t * const tdata )
{
    struct AdfDevice * const device = tdata->device;
    ck_assert_ptr_nonnull ( device );


    ///
    /// mount the test volume
    ///

    // mount the test volume
    struct AdfVolume * vol = //tdata->vol =
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

    // create a new file
    char filename[] = "testfile.tmp";
    struct AdfFile * file = adfFileOpen ( vol, filename, ADF_FILE_MODE_WRITE );
    ck_assert_ptr_nonnull ( file );
    adfFileClose ( file );

    // reset volume state (remount)
    adfVolUnMount ( vol );
    vol = //tdata->vol =
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
    ck_assert_int_eq ( 0, file->posInDataBlk );
    ck_assert_int_eq ( 0, file->nDataBlock );
    ck_assert_int_eq ( adfEndOfFile ( file ), true );
    adfFileClose ( file );

    // the same when open for appending
/*    file = adfFileOpen ( vol, filename, "a" );
    ck_assert_uint_eq ( 0, file->fileHdr->byteSize );
    ck_assert_uint_eq ( 0, file->pos );
    ck_assert_int_eq ( 0, file->posInExtBlk );
    ck_assert_int_eq ( 0, file->posInDataBlk );
    ck_assert_int_eq ( 0, file->nDataBlock );
    ck_assert_int_eq ( adfEndOfFile ( file ), true );
    adfFileClose ( file );
*/
    // the same when open for writing
    file = adfFileOpen ( vol, filename, ADF_FILE_MODE_WRITE );
    ck_assert_uint_eq ( 0, file->fileHdr->byteSize );
    ck_assert_uint_eq ( 0, file->pos );
    ck_assert_int_eq ( 0, file->posInExtBlk );
    ck_assert_int_eq ( 0, file->posInDataBlk );
    ck_assert_int_eq ( 0, file->nDataBlock );
    ck_assert_int_eq ( adfEndOfFile ( file ), true );
    adfFileClose ( file );


    ///
    /// test writing 1st part of the data buffer to the created
    /// above, empty file
    ///

    // open for writing
    file = adfFileOpen ( vol, filename, ADF_FILE_MODE_WRITE );
    ck_assert_uint_eq ( 0, file->fileHdr->byteSize );
    ck_assert_int_eq ( file->fileHdr->firstData, 0 );
    ck_assert_uint_eq ( 0, file->pos );
    ck_assert_int_eq ( 0, file->posInExtBlk );
    ck_assert_int_eq ( 0, file->posInDataBlk );
    ck_assert_int_eq ( 0, file->nDataBlock );
    ck_assert_int_eq ( adfEndOfFile ( file ), true );

    // write the first part of the buffer to the file
    unsigned buf1size = tdata->bufsize / 2;
#if ( TEST_VERBOSITY > 0 )
//    printf (" buf1size = %u, d. block size %u\n", buf1size, vol->datablockSize );
//    fflush (stdout);
#endif
    unsigned bytes_written = adfFileWrite ( file, buf1size, tdata->buffer );
    ck_assert_int_eq ( buf1size, bytes_written );
    ck_assert_uint_eq ( buf1size, file->fileHdr->byteSize );
    ck_assert_int_gt ( file->fileHdr->firstData, 0 );
    ck_assert_uint_eq ( buf1size, file->pos );
    //ck_assert_int_eq ( 0, file->posInExtBlk );
    //ck_assert_int_eq ( datablock2posInExtBlk (
    //                       pos2datablockIndex ( file->pos, vol->datablockSize ) ),
    //                   file->posInExtBlk );    
    //ck_assert_int_eq ( buf1size % vol->datablockSize, file->posInDataBlk );
    ck_assert_msg ( ( file->posInDataBlk == buf1size % vol->datablockSize ) ||
                    ( file->posInDataBlk == vol->datablockSize ),
                    "posInDataBlk incorrect: %d, expected %d, bufsize %u",
                    file->posInDataBlk, buf1size % vol->datablockSize, buf1size );
    ck_assert_int_eq ( 1 + ( buf1size - 1 ) / vol->datablockSize, file->nDataBlock );
    ck_assert_int_eq ( adfEndOfFile ( file ), true );
    adfFileClose ( file );

    // reset volume state (remount)
    adfVolUnMount ( vol );
    vol = //tdata->vol =
        adfVolMount ( device, 0, ADF_ACCESS_MODE_READWRITE );

    // verify free blocks
    //ck_assert_int_eq ( free_blocks_before - file_blocks_used_by_empty_file -
    //                   ( 1 + ( buf1size - 1 ) / vol->datablockSize ),
    //                   adfCountFreeBlocks ( vol ) );

    // verify the number of entries
    ck_assert_int_eq ( 1, adfDirCountEntries ( vol, vol->curDirPtr ) );

    // verify file information (meta-data)
    file = adfFileOpen ( vol, filename, ADF_FILE_MODE_READ );
    ck_assert_uint_eq ( buf1size, file->fileHdr->byteSize );
    ck_assert_int_gt ( file->fileHdr->firstData, 0 );
    ck_assert_uint_eq ( 0, file->pos );
    ck_assert_int_eq ( 0, file->posInExtBlk );
    ck_assert_int_eq ( 0, file->posInDataBlk );
    ck_assert_int_eq ( 1, file->nDataBlock );
    ck_assert_int_eq ( adfEndOfFile ( file ), false );

    unsigned char * const rbuf = malloc ( tdata->bufsize );
    if ( ! rbuf )
        exit ( 1 );

    unsigned bytes_read = adfFileRead ( file, buf1size, rbuf );
    ck_assert_int_eq ( buf1size, bytes_read );
    ck_assert_mem_eq ( tdata->buffer, rbuf, buf1size );
    ck_assert_uint_eq ( buf1size, file->pos );
//    ck_assert_int_eq ( 0, file->posInExtBlk ); // to check later (for larger buffer)
    //ck_assert_int_eq ( buf1size % vol->datablockSize, file->posInDataBlk );
    ck_assert_msg ( ( file->posInDataBlk == buf1size % vol->datablockSize ) ||
                    ( file->posInDataBlk == vol->datablockSize ),
                    "posInDataBlk incorrect: %d, expected %d, bufsize %u",
                    file->posInDataBlk, buf1size % vol->datablockSize, buf1size );

    ck_assert_int_eq ( 1 + ( buf1size - 1 ) / vol->datablockSize, file->nDataBlock );
    ck_assert_int_eq ( adfEndOfFile ( file ), true );

    adfFileClose ( file );

    ck_assert ( verify_file_data ( vol, filename, tdata->buffer, buf1size, 100 ) == 0 );

    // reset volume state (remount)
    adfVolUnMount ( vol );
    vol = //tdata->vol =
        adfVolMount ( device, 0, ADF_ACCESS_MODE_READWRITE );


    ///
    /// test writing 2nd part of the data buffer, overwriting the first written data
    ///

    // open the file for writing
    file = adfFileOpen ( vol, filename, ADF_FILE_MODE_WRITE );

    // verify metadata after opening
    ck_assert_uint_eq ( buf1size, file->fileHdr->byteSize );
    ck_assert_int_gt ( file->fileHdr->firstData, 0 ); // 0 is bootblock
    ck_assert_uint_eq ( 0, file->pos );
    ck_assert_int_eq ( 0, file->posInExtBlk );
    ck_assert_int_eq ( 0, file->posInDataBlk );
    ck_assert_int_eq ( 1, file->nDataBlock );
    ck_assert_int_eq ( adfEndOfFile ( file ), false );

    // overwrite the data with another part of the write buffer
    unsigned buf2size =  tdata->bufsize - buf1size;
    //ck_assert ( buf2size, buf1size ); -> not necessarily
#if ( TEST_VERBOSITY > 0 )
    printf ( " buf1size = %u,\n buf2size = %u,\n buflen   = %u. block size %u\n",
             buf1size, buf2size, tdata->bufsize, vol->datablockSize );
    fflush ( stdout );
#endif
    unsigned char * wbuf = & tdata->buffer [ buf1size ];
    bytes_written = adfFileWrite ( file, buf2size, wbuf );
    ck_assert_int_eq ( buf2size, bytes_written );
    ck_assert_uint_eq ( buf2size, file->fileHdr->byteSize );
    ck_assert_int_gt ( file->fileHdr->firstData, 0 );  // 0 is bootblock
    ck_assert_uint_eq ( buf2size, file->pos );
//    ck_assert_int_eq ( 0, file->posInExtBlk );
    //ck_assert_int_eq ( buf2size % ( vol->datablockSize + 1 ), file->posInDataBlk );
    //ck_assert_int_eq ( ( buf2size + 1 ) % ( vol->datablockSize + 1 ), file->posInDataBlk );
    ck_assert_msg ( ( file->posInDataBlk == buf2size % vol->datablockSize ) ||
                    ( file->posInDataBlk == vol->datablockSize ),
                    "posInDataBlk incorrect: %d, expected %d, bufsize %u",
                    file->posInDataBlk, buf2size % vol->datablockSize, buf2size );
    ck_assert_uint_eq ( ( ( buf2size - 1 ) / vol->datablockSize ) + 1, file->nDataBlock );
    ck_assert_int_eq ( adfEndOfFile ( file ), true );  // sure ? always 2nd part is bigger?
    adfFileClose ( file );

    // reset volume state (remount)
    adfVolUnMount ( vol );
    vol = //tdata->vol =
        adfVolMount ( device, 0, ADF_ACCESS_MODE_READWRITE );
    
    // verify file information (meta-data)
    file = adfFileOpen ( vol, filename, ADF_FILE_MODE_READ );
    ck_assert_uint_eq ( buf2size, file->fileHdr->byteSize );
    ck_assert_int_gt ( file->fileHdr->firstData, 0 );   // 0 is bootblock
    ck_assert_uint_eq ( 0, file->pos );
    ck_assert_int_eq ( 0, file->posInExtBlk );
    ck_assert_int_eq ( 0, file->posInDataBlk );
    ck_assert_int_eq ( 1, file->nDataBlock );
    ck_assert_int_eq ( adfEndOfFile ( file ), false );

    // verify file data (and metadata)
    bytes_read = adfFileRead ( file, buf2size, rbuf );
    ck_assert_int_eq ( buf2size, bytes_read );
    ck_assert_mem_eq ( wbuf, rbuf, buf2size );
    ck_assert_uint_eq ( buf2size, file->pos );
//    ck_assert_int_eq ( 0, file->posInExtBlk );
    //ck_assert_int_eq ( buf2size % ( vol->datablockSize + 1 ), file->posInDataBlk );
    ck_assert_msg ( ( file->posInDataBlk == buf2size % vol->datablockSize ) ||
                    ( file->posInDataBlk == vol->datablockSize ),
                    "posInDataBlk incorrect: %d, expected %d, bufsize %u",
                    file->posInDataBlk, buf2size % vol->datablockSize, buf2size );
    ck_assert_int_eq ( ( ( buf2size - 1 ) / vol->datablockSize ) + 1, file->nDataBlock );
    ck_assert_int_eq ( adfEndOfFile ( file ), true );
    adfFileClose ( file );

    free (rbuf);

    ck_assert ( verify_file_data ( vol, filename, wbuf, buf2size, 100 ) == 0 );
    
    // umount volume
    adfVolUnMount ( vol );
}

static const unsigned buflen[] = { 2,

    512, 513, 970, 974,
    975,   // problem with OFS!
    976, 977, 978,
    1022,
    1023,  // problem with FFS!
    1024,
    1025,
      2047, 2048,
      2049,  // problem with FFS! for 1025
    2050,
    4095, 4096, 4097,
    70000, 72000,72001,
    200000, 600000, 800000,
    1200000, 1600000,
    1689000,    /* nearly max size for an 880k floppy with OFS (max is 844728)
                  (FFS a bit more - no ext blocks) */
    1689456,
    //1689457, 1690000  // too big for OFS
};
static const unsigned buflensize = sizeof ( buflen ) / sizeof (int);

START_TEST ( test_file_overwrite_ofs )
{
    test_data_t test_data = {
        .adfname = "test_file_overwrite2_ofs.adf",
        .volname = "Test_file_overwrite2_ofs",
        .fstype  = 0,          // OFS
        //.openMode = "a",
        .nVolumeBlocks = 1756
    };
    for ( unsigned i = 0 ; i < buflensize ; ++i ) {
        test_data.bufsize = buflen[i];
        setup ( &test_data );
        test_file_overwrite ( &test_data );
        teardown ( &test_data );
    }
}
END_TEST

START_TEST ( test_file_overwrite_ffs )
{
    test_data_t test_data = {
        .adfname = "test_file_overwrite2_ffs.adf",
        .volname = "Test_file_overwrite2_ffs",
        .fstype  = 1,          // FFS
        //.openMode = "a",
        .nVolumeBlocks = 1756
    };
    for ( unsigned i = 0 ; i < buflensize ; ++i ) {
        test_data.bufsize = buflen[i];
        setup ( &test_data );
        test_file_overwrite ( &test_data );
        teardown ( &test_data );
    }
}
END_TEST

Suite * adflib_suite ( void )
{
    Suite * s = suite_create ( "adflib" );
    
    TCase * tc = tcase_create ( "check framework" );
    tcase_add_test ( tc, test_check_framework );
    suite_add_tcase ( s, tc );

    tc = tcase_create ( "adflib test_file_overwrite_ofs" );
    tcase_add_test ( tc, test_file_overwrite_ofs );
    tcase_set_timeout ( tc, 60 );
    suite_add_tcase ( s, tc );

    tc = tcase_create ( "adflib test_file_overwrite_ffs" );
    //tcase_add_checked_fixture ( tc, setup_ffs, teardown_ffs );
    tcase_add_test ( tc, test_file_overwrite_ffs );
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
    tdata->device = adfDevCreate ( "dump", tdata->adfname, 80, 2, 11 );
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
    pattern_random ( tdata->buffer,
                     tdata->bufsize );
}


void teardown ( test_data_t * const tdata )
{
    free ( tdata->buffer );
    tdata->buffer = NULL;

    //adfVolUnMount ( tdata->vol );
    adfDevUnMount ( tdata->device );
    adfDevClose ( tdata->device );

    //printf ("unlinkuing the file %s\n", tdata->adfname );
    //fflush(stdout);
    if ( unlink ( tdata->adfname ) != 0 )
        perror("error deleting the image");
}
