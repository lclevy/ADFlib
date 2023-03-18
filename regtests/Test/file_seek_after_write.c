
#include "adflib.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


typedef struct test_data_s {
    //struct AdfDevice * device;
    //struct AdfVolume * vol;
    char *          adfname;
    char *          volname;
    int             fstype;   // 0 - OFS, 1 - FFS
    unsigned        chunksize;
} test_data_t;


int test_seek_after_write ( const test_data_t * const test_data );
void pattern_random ( unsigned char * buf,
                      const unsigned  bufsize );

int main(void)
{
    const test_data_t test_data_ofs = {
        .adfname   = "test_seek_after_write_ofs.adf",
        .volname   = "TestSeekAfterWriteOFS",
        .fstype    = 0,    // OFS
        .chunksize = 3
    };

    const test_data_t test_data_ffs = {
        .adfname   = "test_seek_after_write_ffs.adf",
        .volname   = "TestSeekAfterWriteFFS",
        .fstype    = 1,    // FFS
        .chunksize = 3
    };

    adfEnvInitDefault();

    int errors = test_seek_after_write ( &test_data_ofs );
    errors += test_seek_after_write ( &test_data_ffs );

    adfEnvCleanUp();

    return errors;
}
    

int test_seek_after_write ( const test_data_t * const test_data )
{
    // create device
    const char * const adfname = test_data->adfname;
    struct AdfDevice * const device = adfCreateDumpDevice ( adfname, 80, 2, 11 );
    if ( ! device )
        return 1;
    adfCreateFlop ( device, test_data->volname, test_data->fstype );

    int errors = 0;

    // mount volume
    struct AdfVolume * vol = adfMount ( device, 0, FALSE );
    if ( vol == NULL ) {
        errors += 1;
        goto umount_device;
    }
    const unsigned blocksize =
        //( isOFS ( vol->dosType ) ? 488u : 512u );
        vol->datablockSize;
    
    // allocate buffer (size of 4 data blocks)
    const unsigned bufsize = blocksize * 4;
    uint8_t * buffer = malloc ( bufsize );
    if ( buffer == NULL )  {
        errors += 1;
        goto umount_volume;
    }

    // fill buffer with 0s
    memset ( buffer, 0, bufsize );

    // create a new file in th ADF volume
    const char filename[] = "testfile.tmp";
    struct AdfFile * file = adfFileOpen ( vol, filename, "w" );
    if ( file == NULL )  {
        errors += 1;
        goto cleanup;
    }

    // ... and write it with buffer fille with 0s
    unsigned bytes_written = (unsigned) adfFileWrite ( file, (int) bufsize, buffer );
    adfFileClose ( file );


    // fill the buffer with random bytes
    pattern_random ( buffer, bufsize );

    // write file without seek (for off-line comparison / double-check)
    const char filename2[] = "testfile_wo_seek.tmp";
    file = adfFileOpen ( vol, filename2, "w" );
    if ( file == NULL )  {
        errors += 1;
        goto cleanup;
    }
    bytes_written = (unsigned) adfFileWrite ( file, (int) bufsize, buffer );
    adfFileClose ( file );
    
    // reopen the test file
    file = adfFileOpen ( vol, filename, "w" );
    if ( file == NULL )  {
        errors += 1;
        goto cleanup;
    }

    // write chunk being end of the first data block + 1 byte (so that the last byte is
    // in the next data block)
    const unsigned chunksize = test_data->chunksize;
    assert ( chunksize < blocksize );
    const unsigned offset = blocksize - ( chunksize - 1 );
    RETCODE rc = adfFileSeek ( file, offset );
    if ( rc != RC_OK ) {
        adfFileClose ( file );
        fprintf ( stderr, "seeking to offset 0x%x (0x%u) failed\n",
                offset, offset );
        errors += 1;
        goto cleanup;
    }
    bytes_written = (unsigned) adfFileWrite ( file, chunksize, buffer + offset );

    // seek to inside another (3rd) data block (to trigger file flush by seek)
    adfFileSeek ( file, blocksize * 3 + 10 );

    adfFileClose ( file );


    // verify overwritten data
    file = adfFileOpen ( vol, filename, "w" );
    if ( file == NULL )  {
        errors += 1;
        goto cleanup;
    }
    uint8_t * const rbuf = malloc ( chunksize );
    adfFileSeek ( file, offset );
    bytes_written = (unsigned) adfFileRead ( file, chunksize, rbuf );
    adfFileClose ( file );
    
    for ( unsigned i = 0 ; i < chunksize ; ++i )  {
        const unsigned offset_i = offset + i;
        //printf ("offset 0x%x (0x%u), buffer: %x, file %x\n",
        //        offset_i, offset_i, buffer[offset_i], rbuf[i] );
        if ( rbuf[i] != buffer[offset_i] ) {
            errors += 1;
            printf ("error on offset 0x%x (0x%u)\n", offset_i, offset_i );
        }
    }
    free ( rbuf );

cleanup:
    free ( buffer );
umount_volume:
    adfUnMount ( vol );
umount_device:
    adfUnMountDev ( device );
    if ( unlink ( adfname ) != 0 )
        perror ("error deleting the image");
    return errors;
}


void pattern_random ( unsigned char * buf,
                      const unsigned  bufsize )
{
    for ( unsigned i = 0 ; i < bufsize ; ++i )  {
        buf[i] = (unsigned char) ( rand() & 0xff );
    }
}
