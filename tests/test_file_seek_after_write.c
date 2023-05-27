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
    char *             openMode;  // "w" or "a"
    unsigned char *    buffer;
    unsigned           bufsize;
    unsigned           chunksize;
} test_data_t;


void setup ( test_data_t * const tdata );
void teardown ( test_data_t * const tdata );


START_TEST ( test_check_framework )
{
    ck_assert ( 1 );
}
END_TEST


void test_file_seek_after_write ( test_data_t * const tdata )
{
    struct AdfDevice * const device = tdata->device;
    ck_assert_ptr_nonnull ( device );
    unsigned char * const buffer = tdata->buffer;
    const unsigned bufsize = tdata->bufsize;
    const unsigned chunksize = tdata->chunksize;
    ck_assert_uint_gt ( bufsize, chunksize );

    ///
    /// mount the test volume
    ///

    // mount the test volume
    struct AdfVolume * vol = // tdata->vol =
        adfMount ( device, 0, FALSE );
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

    /// ... and write it with zeroes
    memset ( buffer, 0, bufsize );
    unsigned bytes_written = (unsigned) adfFileWrite ( file, bufsize, buffer );
    ck_assert_uint_eq ( bufsize, bytes_written );
    adfFileClose ( file );

    // reset volume state (remount)
    adfUnMount ( vol );
    vol = // tdata->vol =
        adfMount ( device, 0, FALSE );

    // put random data in the buffer
    pattern_random ( buffer, bufsize );

    // reopen the file
    file = adfFileOpen ( vol, filename, tdata->openMode );
    ck_assert_ptr_nonnull ( file );
    ck_assert_uint_eq ( file->fileHdr->byteSize, bufsize );

    // create and initialize an array with data chunk status (TRUE means written)
    const unsigned nchunks = bufsize / chunksize +
        ( bufsize % chunksize > 0 ? 1 : 0 );
    ck_assert_uint_gt ( nchunks, 0 );
    BOOL * const chunks = malloc ( nchunks * sizeof(BOOL) );
    ck_assert_ptr_nonnull ( chunks );
    for ( unsigned i = 0 ; i < nchunks ; ++i )
        chunks[i] = FALSE;

    // write the first half of file data chunks in a random order
    //const unsigned nrandom_chunks = nchunks / 2;
    //for ( unsigned i = 0 ; i < nrandom_chunks ; ++i )  {
    for ( unsigned i = 0 ; i < nchunks ; ++i )  {
        unsigned chunk_i;
        do {
            chunk_i = (unsigned) rand() % nchunks;
        } while ( chunks[ chunk_i ] );
        const unsigned offset = chunk_i * chunksize;
        const uint8_t * const chunk = &buffer[ offset ];
        unsigned const wsize =
            ( chunk_i == nchunks - 1 ) ?               // the last chunk?
            //bufsize - ( nchunks - 1 ) * chunksize + bufsize % chunksize :
            ( bufsize % chunksize == 0 ? chunksize :   // the last chunk is chunksize
              bufsize % chunksize ) :                  // or what is left
            chunksize;                                // not the last chunk (-> chunksize)
        //if ( offset < 488 && offset + wsize >= 488 )  {
        /*if ( ( ( offset < 36864 && offset + wsize >= 36864 ) ||
               ( offset < 37376 && offset + wsize >= 37376 ) ) &&
             ( chunksize == 7 ) &&
             ( bufsize == 37380 ) &&
             ( tdata->fstype == 1 ) )
        {
            printf ("wrinting chunk %u (nchunks %u), chunksize %u, wsize %u, "
                    "offset %u, bufsize %u, vol data block size %u\n",
                    chunk_i, nchunks, chunksize, wsize, offset, bufsize,
                    vol->datablockSize );
            fflush (stdout);
        }*/

        RETCODE rc = adfFileSeek ( file, offset );
        ck_assert_int_eq ( rc, RC_OK );
        bytes_written = (unsigned) adfFileWrite ( file, wsize, chunk );
        ck_assert_uint_eq ( wsize, bytes_written );
        chunks[ chunk_i ] = TRUE;
    }

    // write the second half of file data chunks in order (just to speed-up)
/*    for ( unsigned i = 0 ; i < nchunks ; ++i )  {
        if ( ! chunks[i] ) {
            const unsigned offset = i * chunksize;
            const uint8_t * const chunk = &buffer[ offset ];
            unsigned const wsize = ( i == nchunks - 1 ) ?
                //bufsize - ( nchunks - 1 ) * chunksize + bufsize % chunksize :
                ( bufsize % chunksize == 0 ? chunksize :
                  bufsize % chunksize ) :
                chunksize;
            RETCODE rc = adfFileSeek ( file, offset );
            ck_assert_int_eq ( rc, RC_OK );
            bytes_written = (unsigned) adfFileWrite ( file, (int) wsize, chunk );
            ck_assert_uint_eq ( wsize, bytes_written );
            //chunks[ i ] = TRUE;   // not necessary
        }
        }
*/
    free ( chunks );
    ck_assert_uint_eq ( file->fileHdr->byteSize, bufsize );
    adfFileClose ( file );

    // verify written data
    const BOOL data_valid =
        ( verify_file_data ( vol, filename, buffer, bufsize, 10 ) == 0 );
    if ( ! data_valid ) {
        fprintf ( stderr,
                  "Data verification failed: nchunks %u, chunksize %u, "
                  "bufsize %u, vol type %s, dblock size %u\n",
                  nchunks, chunksize, bufsize,
                  ( tdata->fstype & 1 ) == 0 ? "OFS" : "FFS",
                  vol->datablockSize );
        fflush (stderr);
    }

    ck_assert_msg ( data_valid,
                    "Data verification failed for bufsize %u (0x%x), chunksize %u (0x%x)",
                    bufsize, bufsize, chunksize, chunksize );

    // umount volume
    adfUnMount ( vol );
}


static const unsigned buflen[] = {
    //1, 2,
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
    //40000,
    //50000,
    //60000, 69784, 69785, 69796, 69800, 70000,
    //100000, 200000,
    //512000, 800000
};
static const unsigned buflensize = sizeof ( buflen ) / sizeof (unsigned);

static const unsigned chunklen[] = {
    //1, 2,
    3, 7, 10, 15, 16, 64,
    256,
    487, 488, 489,       // 488 -> ofs data block size
    511, 512, 513,       // 512 -> ffs data block size
    975, 976, 977,
    1022, 1023, 1024, 1025,
    2047, 2048, 2049, 2050,
    4095, 4096,
    35136,
    35137,          // min. file size requiring an ext block (OFS)
    35138,
    36864,
    36865,          // min. file size requiring an ext block (FFS)
    36866
};
static const unsigned chunklensize = sizeof ( chunklen ) / sizeof (unsigned);


START_TEST ( test_file_seek_after_write_ofs )
{
    test_data_t test_data = {
        .adfname = "test_file_seek_after_write_ofs.adf",
        .volname = "Test_file_seek_after_write_ofs",
        .fstype  = 0,          // OFS
        .openMode = "w",
        .nVolumeBlocks = 1756
    };
    for ( unsigned i = 0 ; i < buflensize ; ++i )  {
        test_data.bufsize = buflen[i];
        for ( unsigned j = 0 ; j < chunklensize ; ++j )  {
            if ( chunklen[j] >= test_data.bufsize )
                break;
            test_data.chunksize = chunklen[j];
            setup ( &test_data );
            test_file_seek_after_write ( &test_data );
            teardown ( &test_data );
        }
    }
}
END_TEST


START_TEST ( test_file_seek_after_write_ffs )
{
    test_data_t test_data = {
        .adfname = "test_file_seek_after_write_ffs.adf",
        .volname = "Test_file_seek_after_write_ffs",
        .fstype  = 1,          // FFS
        .openMode = "w",
        .nVolumeBlocks = 1756
    };
    for ( unsigned i = 0 ; i < buflensize ; ++i )  {
        test_data.bufsize = buflen[i];
        for ( unsigned j = 0 ; j < chunklensize ; ++j )  {
            if ( chunklen[j] >= test_data.bufsize )
                break;
            test_data.chunksize = chunklen[j];
            setup ( &test_data );
            test_file_seek_after_write ( &test_data );
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

    tc = tcase_create ( "adflib test_file_seek_after_write_ofs" );
    tcase_add_test ( tc, test_file_seek_after_write_ofs );
    tcase_set_timeout ( tc, 120 );
    suite_add_tcase ( s, tc );

    tc = tcase_create ( "adflib test_file_seek_after_write_ffs" );
    //tcase_add_checked_fixture ( tc, setup_ffs, teardown_ffs );
    tcase_add_test ( tc, test_file_seek_after_write_ffs );
    tcase_set_timeout ( tc, 120 );
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
    if ( tdata->device == NULL ) {
        //return;
        exit(1);
    }
    if ( adfCreateFlop ( tdata->device, tdata->volname, tdata->fstype ) != RC_OK ) {
        fprintf ( stderr, "adfCreateFlop error creating volume: %s\n",
                  tdata->volname );
        exit(1);
    }

    //tdata->vol = adfMount ( tdata->device, 0, FALSE );
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

    //adfUnMount ( tdata->vol );
    adfUnMountDev ( tdata->device );
    if ( unlink ( tdata->adfname ) != 0 )
        perror("error deleting the image");
}
