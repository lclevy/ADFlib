
//
// regr. test for the issue:
//   https://github.com/lclevy/ADFlib/issues/9
//

#include "adflib.h"

#define TEST_VERBOSITY 1

int test_floppy_overfilling ( char * const          adfname,
                              char * const          filename,
                              unsigned char * const buffer,
                              const int             blocksize );

int main (void)
{
    adfEnvInitDefault();

    const int BUF_SIZE = 4096;
    unsigned char buf [ BUF_SIZE ];
    for ( int i = 0 ; i < BUF_SIZE ; i += 4 ) {
        buf[i]   = 'A';
        buf[i+1] = 'M';
        buf[i+2] = 'I';
        buf[i+3] = 'G';
    }

    int status = 0;

    int test_bufsize[] = { 4096, 4095, 2049, 2048, 1025, 1024, 1023,
        513, 512, 511, 257, 256, 128, 32, 16, 8, 4, 2, 1 };

    for ( unsigned i = 0 ; i < sizeof ( test_bufsize ) / sizeof ( int ) ; ++i ) {
        status += test_floppy_overfilling (
            "test.adf", "testfile1.dat", buf, test_bufsize[i] );
    }

    adfEnvCleanUp();
    return status;
}


int test_floppy_overfilling ( char * const          adfname,
                              char * const          filename,
                              unsigned char * const buffer,
                              const int             blocksize )
{
#if TEST_VERBOSITY > 0
    printf ("Test floppy overfilling, blocksize: %d", blocksize );
#endif

    struct Device * device = adfCreateDumpDevice ( adfname, 80, 2, 11 );
    if ( ! device )
        return 1;
    adfCreateFlop ( device, "Floppy2ADF", 0 );

    struct Volume * vol = adfMount ( device, 0, FALSE );
    if ( ! vol )
        return 1;

#if TEST_VERBOSITY > 1
    printf ( "\nFree blocks: %d\n", adfCountFreeBlocks ( vol ) );
#endif

    struct File * output = adfOpenFile ( vol, filename, "w" );
    if ( ! output )
        return 1;

    int status = 1;
    for ( int i = 0; i < 1024 * 1024 / blocksize ; ++i ) {
        if ( adfWriteFile ( output, blocksize, buffer ) != blocksize ) {
            // OK, not all bytes written
            status = 0;   
            break;
        }
        //printf ( "\nFree blocks: %d\n", adfCountFreeBlocks ( vol ) );
    }
    //adfFlushFile ( output );
    adfCloseFile ( output );

#if TEST_VERBOSITY > 1
    printf ( "\nFree blocks: %d\n", adfCountFreeBlocks ( vol ) );
#endif

    adfUnMount ( vol );
    adfUnMountDev ( device );

#if TEST_VERBOSITY > 0
    printf (" -> %s\n", status ? "ERROR" : "OK" );
#endif

    return status;
}
