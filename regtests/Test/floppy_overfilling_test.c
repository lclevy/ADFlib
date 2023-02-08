
//
// regr. test for the issue:
//   https://github.com/lclevy/ADFlib/issues/9
//

#include "adflib.h"

#include <stdio.h>
#include <stdlib.h>

#define TEST_VERBOSITY 1

int test_floppy_overfilling ( char * const          adfname,
                              char * const          filename,
                              unsigned char * const buffer,
                              const unsigned        blocksize,
                              const unsigned char   fstype );

int verify_file_data ( struct AdfVolume * const vol,
                       char * const             filename,
                       unsigned char * const buffer,
                       const unsigned        bytes_written );

void pattern_AMIGAMIG ( unsigned char * buf,
                        const unsigned  BUFSIZE );

void pattern_random ( unsigned char * buf,
                      const unsigned  BUFSIZE );


int main (void)
{
    adfEnvInitDefault();

#ifdef _MSC_VER   // visual studio do not understand that const is const...
#define BUF_SIZE 1024 * 1024
#else
    const unsigned BUF_SIZE = 1024 * 1024;
#endif
    unsigned char buf [ BUF_SIZE ];

    //pattern_AMIGAMIG ( buf, BUFSIZE );
    pattern_random ( buf, BUF_SIZE );

    int status = 0;

    unsigned test_bufsize[] = { 4096, 4095, 2049, 2048, 1025, 1024, 1023,
        513, 512, 511, 257, 256, 128, 32, 16, 8, 4, 2, 1 };

    for ( unsigned i = 0 ; i < sizeof ( test_bufsize ) / sizeof ( unsigned ) ; ++i ) {
        status += test_floppy_overfilling (
            "test.adf", "testfile1.dat", buf, test_bufsize[i], 0 );  // OFS
        status += test_floppy_overfilling (
            "test.adf", "testfile1.dat", buf, test_bufsize[i], 1 );  // FFS
    }

    adfEnvCleanUp();
    return status;
}


int test_floppy_overfilling ( char * const          adfname,
                              char * const          filename,
                              unsigned char * const buffer,
                              const unsigned        blocksize,
                              const unsigned char   fstype )
{
    const char * const fstype_info[] = { "OFS", "FFS" };
#if TEST_VERBOSITY > 0
    printf ("Test floppy overfilling, filesystem: %s, blocksize: %d",
            fstype_info [fstype], blocksize );
#endif

    struct AdfDevice * device = adfCreateDumpDevice ( adfname, 80, 2, 11 );
    if ( ! device )
        return 1;
    adfCreateFlop ( device, "OverfillTest", fstype );

    struct AdfVolume * vol = adfMount ( device, 0, FALSE );
    if ( ! vol )
        return 1;

#if TEST_VERBOSITY > 1
    printf ( "\nFree blocks: %d\n", adfCountFreeBlocks ( vol ) );
#endif

    struct adfFile * output = adfOpenFile ( vol, filename, "w" );
    if ( ! output )
        return 1;

    int status = 1;
    unsigned iterations = 1024u * 1024u / blocksize +
        ( 1024 * 1024 % blocksize > 0 ? 1 : 0 );
    unsigned char * bufferp = buffer;
    unsigned bytes_written = 0;
    for ( unsigned i = 0; i < iterations ; ++i ) {
        int block_bytes_written = adfWriteFile ( output, (int) blocksize, bufferp );
        bytes_written += (unsigned) block_bytes_written;
        if ( (unsigned) block_bytes_written != blocksize ) {
            // OK, not all bytes written
            status = 0;   
            break;
        }
        bufferp += blocksize;
        //printf ( "\nFree blocks: %d\n", adfCountFreeBlocks ( vol ) );
    }

    if ( output->pos != bytes_written ) {
        fprintf ( stderr, "written_file->pos (%u) != bytes_written (%u) - ERROR!\n",
                  output->pos, bytes_written );
        status++;
    }

    //adfFlushFile ( output );
    adfCloseFile ( output );

#if TEST_VERBOSITY > 1
    printf ( "\nFree blocks: %d\n", adfCountFreeBlocks ( vol ) );
#endif
    int free_blocks = adfCountFreeBlocks ( vol );
    if ( free_blocks != 0 ) {
        fprintf ( stderr, "\n%d blocks still free after 'overfilling'!\n",
                  free_blocks );
        status++;
    }

    status += verify_file_data ( vol, filename, buffer, bytes_written );

    adfUnMount ( vol );
    adfUnMountDev ( device );

#if TEST_VERBOSITY > 0
    printf (" -> %s\n", status ? "ERROR" : "OK" );
#endif

    return status;
}


int verify_file_data ( struct AdfVolume * const vol,
                       char * const             filename,
                       unsigned char * const    buffer,
                       const unsigned           bytes_written )
{
    struct adfFile * output = adfOpenFile ( vol, filename, "r" );
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
             offset = 0;
    int status = 0;
    do {
        block_bytes_read = (unsigned) adfReadFile ( output, (int) READ_BUFSIZE, readbuf );
        bytes_read += block_bytes_read;
        for ( unsigned i = 0 ; i < block_bytes_read ; ++i ) {
            if ( readbuf [ offset % READ_BUFSIZE ] != buffer [ offset ] ) {
                fprintf ( stderr, "Data differ at %u ( 0x%x ): orig. 0x%x, read 0x%x\n",
                          offset, offset,
                          buffer [ offset ],
                          readbuf [ offset % READ_BUFSIZE ] );
                status++;
            }
            offset++;
        }
    } while ( block_bytes_read == READ_BUFSIZE );

    adfCloseFile ( output );

    if ( bytes_read != bytes_written ) {
        fprintf ( stderr, "bytes read (%u) != bytes written (%u) -> ERROR!!!\n",
                  bytes_read, bytes_written );
        status++;
    }

    return status;
}


void pattern_AMIGAMIG ( unsigned char * buf,
                        const unsigned  BUFSIZE )
{
    for ( unsigned i = 0 ; i < BUFSIZE ; i += 4 ) {
        buf[i]   = 'A';
        buf[i+1] = 'M';
        buf[i+2] = 'I';
        buf[i+3] = 'G';
     }
}


void pattern_random ( unsigned char * buf,
                      const unsigned  BUFSIZE )
{
    for ( unsigned i = 0 ; i < BUFSIZE ; ++i ) {
        buf[i]   = (unsigned char) ( rand() & 0xff );
    }
}
