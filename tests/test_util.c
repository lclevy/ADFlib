

#include <stdio.h>
#include <stdlib.h>

#include "test_util.h"


unsigned verify_file_data ( struct AdfVolume * const    vol,
                            const char * const          filename,
                            const unsigned char * const buffer,
                            const unsigned              bytes_written,  // == bufsize (!)
                            const unsigned              errors_max )
{
    struct AdfFile * output = adfFileOpen ( vol, filename, "r" );
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
        block_bytes_read = (unsigned) adfFileRead ( output, READ_BUFSIZE, readbuf );
        bytes_read += block_bytes_read;
        for ( unsigned i = 0 ; i < block_bytes_read ; ++i ) {
            if ( readbuf [ offset % READ_BUFSIZE ] != buffer [ offset ] ) {
                fprintf ( stderr, "Data differ at %u ( 0x%x ): orig. 0x%x, read 0x%x\n",
                          offset, offset,
                          buffer [ offset ],
                          readbuf [ offset % READ_BUFSIZE ] );
                fflush ( stderr );
                nerrors++;
                if ( nerrors > errors_max ) {
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

// bufsize must be divisible by 4
void pattern_AMIGAMIG ( unsigned char * buf,
                        const unsigned  bufsize )
{
    for ( unsigned i = 0 ; i < bufsize ; i += 4 ) {
        buf[i]   = 'A';
        buf[i+1] = 'M';
        buf[i+2] = 'I';
        buf[i+3] = 'G';
     }
}


void pattern_random ( unsigned char * buf,
                      const unsigned  bufsize )
{
    for ( unsigned i = 0 ; i < bufsize ; ++i ) {
        buf[i]   = (unsigned char) ( rand() & 0xff );
    }
}


unsigned pos2datablockIndex ( unsigned pos,
                              unsigned blocksize )
{
    return ( pos / blocksize );
}



unsigned filesize2datablocks ( unsigned fsize,
                               unsigned blocksize )
{
    return ( fsize / blocksize +
             ( ( fsize % blocksize > 0 ) ? 1 : 0 ) );
}


unsigned datablocks2extblocks ( unsigned data_blocks )
{
    //return max ( ( data_blocks - 1 ) / MAX_DATABLK, 0 );
    if ( data_blocks < 1 )
        return 0;
    return ( data_blocks - 1 ) / ( MAX_DATABLK );
}


unsigned filesize2blocks ( unsigned fsize,
                           unsigned blocksize )
{
    //assert ( fsize >= 0 );
    //assert ( blocksize >= 0 );
    unsigned data_blocks = filesize2datablocks ( fsize, blocksize );
    unsigned ext_blocks = datablocks2extblocks ( data_blocks );
    return data_blocks + ext_blocks + 1;   // +1 is the file header block
}


// datablock index ( 0, ...  , nblocks - 1 ) to block's position in ExtBlock
unsigned datablock2posInExtBlk ( unsigned datablock_idx )
{
    //return max ( ( datablock_idx - MAX_DATABLK ) % ( MAX_DATABLK + 1 ), 0 );

    if ( datablock_idx < MAX_DATABLK )
        // block pointer in file header (not in an ext. block)
        //return -1;
        return 0;
    return ( datablock_idx - MAX_DATABLK ) % ( MAX_DATABLK );
}
