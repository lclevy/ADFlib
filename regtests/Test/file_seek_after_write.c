
#include "adflib.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#ifndef _WIN32
#include <unistd.h>   // for unlink()
#endif


typedef struct test_data_s {
    //struct AdfDevice * device;
    //struct AdfVolume * vol;
    char *          testname;
    char *          adfname;
    char *          volname;
    uint8_t         fstype;   // 0 - OFS, 1 - FFS
    unsigned        dblocksize;
    unsigned        chunksize;
    unsigned        offset;
    unsigned        bufsize;
} test_data_t;


unsigned test_seek_after_write ( const test_data_t * const test_data );


unsigned verify_overwritten_data ( struct AdfVolume * const vol,
                                   const char * const       filename,
                                   const uint8_t * const    buffer,
                                   const unsigned           offset,
                                   const unsigned           chunksize );

void pattern_random ( unsigned char * buf,
                      const unsigned  bufsize );

uint32_t checksum ( const unsigned char * dataptr,
                    size_t                size );

// from tests/test_util.c
unsigned verify_file_data ( struct AdfVolume * const    vol,
                            const char * const          filename,
                            const unsigned char * const buffer,
                            const unsigned              bytes_written,  // == bufsize (!)
                            const unsigned              errors_max );


int main(void)
{
    /* this test will try to overwrite the following offsets (OFS):
           +-----+-----+-----+
       ... | 486 | 487 | 488 |
           +-----+-----+-----+
                       |
        (data block 0) | (data block 1)
    */
    test_data_t test_dblock_1_overlap_ofs = {
        .testname   = "test_dblock_1_overlap_ofs",
        .adfname    = "test_seek_after_write_ofs.adf",
        .volname    = "TestSeekAfterWriteOFS",
        .fstype     = 0,    // OFS
        .dblocksize = 488,
        .chunksize  = 3,
        .bufsize    = 40000
    };

    test_dblock_1_overlap_ofs.offset =
        test_dblock_1_overlap_ofs.dblocksize - (
        test_dblock_1_overlap_ofs.chunksize - 1 );


    /* this test will try to overwrite the following offsets (FFS):
           +-----+-----+-----+
       ... | 510 | 511 | 512 |
           +-----+-----+-----+
                       |
        (data block 0) | (data block 1)
    */
    test_data_t test_dblock_1_overlap_ffs = {
        .testname  = " test_dblock_1_overlap_ffs",
        .adfname    = "test_seek_after_write_ffs.adf",
        .volname    = "TestSeekAfterWriteFFS",
        .fstype     = 1,    // FFS
        .dblocksize = 512,
        .chunksize  = 3,
        .bufsize    = 40000
    };

    test_dblock_1_overlap_ffs.offset =
        test_dblock_1_overlap_ffs.dblocksize - (
        test_dblock_1_overlap_ffs.chunksize - 1 );


    /* this test will try to overwrite the following offsets (OFS):
           +-------+-------+-------+
       ... | 35134 | 35135 | 35136 |
           +-------+-------+-------+
                           |
           (data block 71) | (data block 72)
    */
    test_data_t test_dblock_72_overlap_ofs = {
        .testname  = "test_dblock_72_overlap_ofs",
        .adfname    = "test_seek_after_write_ofs.adf",
        .volname    = "TestSeekAfterWriteOFS",
        .fstype     = 0,    // OFS
        .dblocksize = 488,
        .chunksize  = 3,
        .bufsize    = 40000
    };
    test_dblock_72_overlap_ofs.offset =
        35136         // the 1st offset addressed with an ext block (OFS)
        - ( test_dblock_72_overlap_ofs.chunksize - 1 );


    /* this test will try to overwrite the following offsets (FFS):
           +-------+-------+-------+
       ... | 36862 | 36863 | 36864 |
           +-------+-------+-------+
                           |
           (data block 71) | (data block 72)
    */
    test_data_t test_dblock_72_overlap_ffs = {
        .testname  = "test_dblock_72_overlap_ffs",
        .adfname    = "test_seek_after_write_ffs.adf",
        .volname    = "TestSeekAfterWriteFFS",
        .fstype     = 1,    // FFS
        .dblocksize = 512,
        .chunksize  = 3,
        .bufsize    = 40000
    };

    test_dblock_72_overlap_ffs.offset =
        36864         // the 1st offset addressed with an ext block (FFS)
        - ( test_dblock_72_overlap_ffs.chunksize - 1 );


    /* this test will try to overwrite the following offsets (OFS):
           +-------+-------+-------+
       ... | 35622 | 35623 | 35624 |
           +-------+-------+-------+
                           |
           (data block 72) | (data block 73)
    */
    test_data_t test_dblock_73_overlap_ofs = {
        .testname  = "test_dblock_73_overlap_ofs",
        .adfname    = "test_seek_after_write_ofs.adf",
        .volname    = "TestSeekAfterWriteOFS",
        .fstype     = 0,    // OFS
        .dblocksize = 488,
        .chunksize  = 3,
        .bufsize    = 40000
    };

    test_dblock_73_overlap_ofs.offset =
        35136 + 512    // the 2nd offset addressed with an ext block (OFS)
        - ( test_dblock_73_overlap_ofs.chunksize - 1 );


    /* this test will try to overwrite the following offsets (FFS):
           +-------+-------+-------+
       ... | 37374 | 37375 | 37376 |
           +-------+-------+-------+
                           |
           (data block 72) | (data block 73)
    */
    test_data_t test_dblock_73_overlap_ffs = {
        .testname  = "test_dblock_73_overlap_ffs",
        .adfname    = "test_seek_after_write_ffs.adf",
        .volname    = "TestSeekAfterWriteFFS",
        .fstype     = 1,    // FFS
        .dblocksize = 512,
        .chunksize  = 3,
        .bufsize    = 40000
    };

    test_dblock_73_overlap_ffs.offset =
        36864 + 512    // the 2nd offset addressed with an ext block (FFS)
        - ( test_dblock_73_overlap_ffs.chunksize - 1 );


    /* this test will try to overwrite the following offsets (FFS):
           +-------+-------+-------+
       ... | 37374 | 37375 | 37376 |
           +-------+-------+-------+
                           |
           (data block 72) | (data block 73)
    */
    test_data_t test_37380_3_overlap_ffs = {
        .testname   = "test_37380_3_overlap_ffs",
        .adfname    = "test_seek_after_write_ffs.adf",
        .volname    = "TestSeekAfterWriteFFS",
        .fstype     = 1,    // FFS
        .dblocksize = 512,
        .chunksize  = 3,
        .bufsize    = 37380,
        .offset     = 37374   // bufsize - chunksize - 1 (counting from 0)
    };


    /* this test will try to overwrite the following offsets (FFS):
           +-------+-------+-------+-------+-------+-------+-------+
       ... | 37373 | 37374 | 37375 | 37376 | 37377 | 37378 | 37379 |
           +-------+-------+-------+-------+-------+-------+-------+
                                   |
                   (data block 72) | (data block 73)
    */
    test_data_t test_37380_7_overlap_ffs = {
        .testname   = "test_37380_7_overlap_ffs",
        .adfname    = "test_seek_after_write_ffs.adf",
        .volname    = "TestSeekAfterWriteFFS",
        .fstype     = 1,    // FFS
        .dblocksize = 512,
        .chunksize  = 7,
        .bufsize    = 37380,
        .offset     = 37373   // bufsize - chunksize - 1 (counting from 0)
    };


    adfEnvInitDefault();

    unsigned errors = test_seek_after_write ( &test_dblock_1_overlap_ofs );
    errors         += test_seek_after_write ( &test_dblock_1_overlap_ffs );
    errors         += test_seek_after_write ( &test_dblock_72_overlap_ofs );
    errors         += test_seek_after_write ( &test_dblock_72_overlap_ffs );
    errors         += test_seek_after_write ( &test_dblock_73_overlap_ofs );
    errors         += test_seek_after_write ( &test_dblock_73_overlap_ffs );
    errors         += test_seek_after_write ( &test_37380_3_overlap_ffs );
    errors         += test_seek_after_write ( &test_37380_7_overlap_ffs );

    adfEnvCleanUp();

    return (int) errors;
}
    

unsigned test_seek_after_write ( const test_data_t * const test_data )
{
    // create device
    const char * const adfname = test_data->adfname;
    struct AdfDevice * const device = adfDevCreate ( "dump", adfname, 80, 2, 11 );
    if ( ! device )
        return 1;
    adfCreateFlop ( device, test_data->volname, test_data->fstype );

    unsigned errors = 0;

    // mount volume
    struct AdfVolume * vol = adfMount ( device, 0, ADF_ACCESS_MODE_READWRITE );
    if ( vol == NULL ) {
        errors += 1;
        goto umount_device;
    }
    const unsigned blocksize =
        //( isOFS ( vol->dosType ) ? 488u : 512u );
        (unsigned) vol->datablockSize;
    assert ( (unsigned) vol->datablockSize == test_data->dblocksize );
    
    // allocate buffers
    const unsigned bufsize = test_data->bufsize;
    uint8_t * const buffer_0 = malloc ( bufsize );
    if ( buffer_0 == NULL )  {
        errors += 1;
        goto umount_volume;
    }
    uint8_t * const buffer_random = malloc ( bufsize );
    if ( buffer_random == NULL )  {
        errors += 1;
        goto cleanup_0;
    }

    // fill buffer_0 with 0s
    memset ( buffer_0, 0, bufsize );

    // fill the buffer with random bytes and make a checksum
    pattern_random ( buffer_random, bufsize );
    uint32_t buf_checksum = checksum ( buffer_random, bufsize );

    // create a new file in th ADF volume
    const char filename[] = "testfile_chunk_overwrite.tmp";
    struct AdfFile * file = adfFileOpen ( vol, filename, ADF_FILE_MODE_WRITE );
    if ( file == NULL )  {
        errors += 1;
        goto cleanup;
    }

    // ... and write it with buffer filled with 0s
    unsigned bytes_written = adfFileWrite ( file, bufsize, buffer_0 );
    assert ( bytes_written == bufsize );
    assert ( file->fileHdr->byteSize == bufsize );
    adfFileClose ( file );

    // write file without seek (for off-line comparison / double-check)
    const char filename2[] = "testfile_wo_seek.tmp";
    file = adfFileOpen ( vol, filename2, ADF_FILE_MODE_WRITE );
    if ( file == NULL )  {
        errors += 1;
        goto cleanup;
    }
    bytes_written = adfFileWrite ( file, bufsize, buffer_random );
    assert ( bytes_written == bufsize );
    assert ( file->fileHdr->byteSize == bufsize );
    adfFileClose ( file );

    const unsigned chunksize = test_data->chunksize;
    assert ( chunksize < blocksize );
    const unsigned offset = test_data->offset;

    // check data in "normally" written file (without any seek)
    errors += verify_file_data ( vol, filename2, buffer_random, bufsize, 10 );
    
    // reopen the test file
    file = adfFileOpen ( vol, filename, ADF_FILE_MODE_WRITE );
    if ( file == NULL )  {
        errors += 1;
        goto cleanup;
    }

    // write chunk being end of a data block + 1 byte (so that the last byte is
    // in the next/following data block) from buffer with random data
    RETCODE rc = adfFileSeek ( file, offset );
    if ( rc != RC_OK ) {
        adfFileClose ( file );
        fprintf ( stderr, "seeking to offset 0x%x (0x%u) failed\n",
                offset, offset );
        errors += 1;
        goto cleanup;
    }
    bytes_written = adfFileWrite ( file, chunksize,
                                   buffer_random + offset );
    assert ( bytes_written == chunksize );
    assert ( file->fileHdr->byteSize == bufsize );

    // seek to inside another data block (to trigger file flush by seek)
    adfFileSeek ( file, bufsize / 2 );

    adfFileClose ( file );

    uint32_t buf_checksum2 = checksum ( buffer_random, bufsize );
    assert ( buf_checksum == buf_checksum2 );

    // verify overwritten data
    unsigned nerrors_overwriting =
        verify_overwritten_data ( vol, filename, buffer_random, offset, chunksize );
    if ( nerrors_overwriting > 0 ) {
            fprintf ( stderr, "+%u errors, file %s\n",
                      nerrors_overwriting, filename );
            fflush ( stderr );
    }
    
    // check also the same data in "normally" written file (without any seek)
    unsigned nerrors_wo_seek = //verify_data ( vol, filename2, buffer, offset, chunksize );
        verify_file_data ( vol, filename2, buffer_random, bufsize, 10 );
    if ( nerrors_wo_seek > 0 )  {
            fprintf ( stderr, "+%u errors, file %s\n",
                      nerrors_wo_seek, filename2 );
            fflush ( stderr );
    }

    errors += nerrors_overwriting + nerrors_wo_seek;
    if ( errors > 0 )  {
        fprintf ( stderr, "%s failed with %u errors ( fs %s, dblock %u,"
                  " filesize %u, chunk offset %u, chunksize %u )\n",
                  test_data->testname, errors,
                  ( test_data->fstype == 0 ) ? "OFS" : "FFS",
                  test_data->dblocksize,
                  bufsize, offset, chunksize );
        fflush ( stderr );
    }

cleanup:
    free ( buffer_random );
cleanup_0:
    free ( buffer_0 );
umount_volume:
    adfUnMount ( vol );
umount_device:
    adfDevUnMount ( device );
    adfDevClose ( device );
    if ( unlink ( adfname ) != 0 )
        perror ("error deleting the image");
    return errors;
}


unsigned verify_overwritten_data ( struct AdfVolume * const vol,
                                   const char * const       filename,
                                   const uint8_t * const    buffer,
                                   const unsigned           offset,
                                   const unsigned           chunksize )
{
    unsigned nerrors = 0;
    struct AdfFile * const file = adfFileOpen ( vol, filename, ADF_FILE_MODE_READ );
    if ( file == NULL )
        return 1;

    const unsigned filesize = file->fileHdr->byteSize;
    uint8_t * const rbuf = //malloc ( chunksize );
        malloc ( filesize );
    assert ( rbuf != NULL );

    //RETCODE rc = adfFileSeek ( file, offset );
    //assert ( rc == RC_OK );

    // verify part filled with 0
    unsigned chunk0_size = offset;
    unsigned bytes_read = adfFileRead ( file, chunk0_size, rbuf );
    assert ( bytes_read == chunk0_size );
    for ( unsigned i = 0 ; i < chunk0_size ; ++i ) {
        if ( rbuf[i] != 0 ) {
            nerrors += 1;
            fprintf ( stderr, "data error: offset 0x%x (0x%u), buf 0x%x, file 0x%x,"
                      " fname %s, filesize %u\n",
                      i, i, 0, rbuf[i], filename, filesize );
            fflush ( stderr );
        }
    }

    // verify overwritten data chunk
    bytes_read = adfFileRead ( file, chunksize, rbuf );
    assert ( bytes_read == chunksize );
    for ( unsigned i = 0 ; i < chunksize ; ++i )  {
        const unsigned offset_i = offset + i;
        //printf ("offset 0x%x (0x%u), buffer: %x, file %x\n",
        //        offset_i, offset_i, buffer[offset_i], rbuf[i] );
        if ( rbuf[i] != buffer[offset_i] ) {
            nerrors += 1;
            fprintf ( stderr, "data error: offset 0x%x (0x%u), buf 0x%x, file 0x%x,"
                      " fname %s, chunk offset %u, chunksize %u, filesize %u\n",
                      offset_i, offset_i, buffer[offset_i], rbuf[i], filename,
                      offset, chunksize, filesize );
            fflush ( stderr );
        }
    }

    // verify the remaining part filled with 0
    chunk0_size = filesize - chunk0_size - chunksize;
    bytes_read = adfFileRead ( file, chunk0_size, rbuf );
    assert ( bytes_read == chunk0_size );
    for ( unsigned i = 0 ; i < chunk0_size ; ++i ) {
        if ( rbuf[i] != 0 ) {
            nerrors += 1;
            fprintf ( stderr, "data error: offset 0x%x (0x%u), buf 0x%x, file 0x%x,"
                      " fname %s, filesize %u\n",
                      i, i, 0, rbuf[i], filename, filesize );
            fflush ( stderr );
        }
    }

    adfFileClose ( file );
    free ( rbuf );
    return nerrors;
}


void pattern_random ( unsigned char * buf,
                      const unsigned  bufsize )
{
    for ( unsigned i = 0 ; i < bufsize ; ++i )  {
        buf[i] = (unsigned char) ( rand() & 0xff );
    }
}


uint32_t checksum ( const unsigned char * dataptr,
                    size_t                size )
{
    uint32_t chksum = 0;
    while ( size-- != 0 )
        //chksum -= *dataptr++;
        //chksum += *dataptr++;
        chksum ^= (uint32_t) (*dataptr++);
    return chksum;
}


// from tests/test_util.c
unsigned verify_file_data ( struct AdfVolume * const    vol,
                            const char * const          filename,
                            const unsigned char * const buffer,
                            const unsigned              bytes_written,  // == bufsize (!)
                            const unsigned              errors_max )
{
    struct AdfFile * output = adfFileOpen ( vol, filename, ADF_FILE_MODE_READ );
    if ( ! output )
        return 1;

#ifdef _MSC_VER   // visual studio do not understand that const is const...
#define READ_BUFSIZE 1024
#else
    const unsigned READ_BUFSIZE = 1024;
#endif
    unsigned char readbuf [ READ_BUFSIZE ];

    unsigned bytes_read = 0,
             block_bytes_read,
             offset = 0,
             nerrors = 0;
    do {
        block_bytes_read = adfFileRead ( output, READ_BUFSIZE, readbuf );
        bytes_read += block_bytes_read;

        for ( unsigned i = 0 ; i < block_bytes_read ; ++i ) {
            if ( readbuf [ offset % READ_BUFSIZE ] != buffer [ offset ] ) {
                fprintf ( stderr, "Data differ at %u ( 0x%x ): orig. 0x%x, read 0x%x\n",
                          offset, offset,
                          buffer [ offset ],
                          readbuf [ offset % READ_BUFSIZE ] );
                fflush ( stderr );
                nerrors++;
                if ( nerrors >= errors_max ) {
                    adfFileClose ( output );
                    return nerrors;
                }
            }
            offset++;
        }
    } while ( block_bytes_read == READ_BUFSIZE );

    adfFileClose ( output );

    if ( bytes_read != bytes_written ) {
        fprintf ( stderr, "bytes read (%u) != bytes written (%u) -> ERROR!!!\n",
                  bytes_read, bytes_written );
        fflush ( stderr );
        nerrors++;
    }

    return nerrors;
}
