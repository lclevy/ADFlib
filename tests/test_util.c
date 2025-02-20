

#include <stdio.h>
#include <stdlib.h>

#include "test_util.h"
#include "adf_file_util.h"

unsigned verify_file_data ( struct AdfVolume * const    vol,
                            const char * const          filename,
                            const unsigned char * const buffer,
                            const unsigned              fsize,
                            const unsigned              errors_max )
{
    struct AdfFile * const output = adfFileOpen ( vol, filename, ADF_FILE_MODE_READ );
    if ( ! output )
        return 1;

#ifdef _MSC_VER   // visual studio do not understand that const is const...
#define READ_BUFSIZE 1024
#else
    const unsigned READ_BUFSIZE = 1024;
#endif
    unsigned char readbuf [ READ_BUFSIZE ];

    unsigned bytes_read = 0,
             bytes_to_read = fsize,
             offset = 0,
             nerrors = 0,
             chunk_bytes_to_read,
             chunk_bytes_read;
    while ( bytes_to_read > 0 ) {
        chunk_bytes_to_read = ( READ_BUFSIZE < bytes_to_read ? READ_BUFSIZE : bytes_to_read );
        chunk_bytes_read = (unsigned) adfFileRead ( output, chunk_bytes_to_read, readbuf );
        bytes_read    += chunk_bytes_read;
        bytes_to_read -= chunk_bytes_read;

        for ( unsigned i = 0 ; i < chunk_bytes_read ; ++i ) {
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
    }

    adfFileClose ( output );

    if ( bytes_read != fsize ) {
        fprintf ( stderr, "bytes read (%u) != bytes written (%u) -> ERROR!!!\n",
                  bytes_read, fsize );
        fflush ( stderr );
        nerrors++;
    }

    return nerrors;
}


static unsigned validate_file_metadata_last_ext ( struct AdfFile * const file )
{
    unsigned fsize = file->fileHdr->byteSize;

    ADF_RETCODE rc = adfFileSeek ( file, fsize + 1 );
    if ( rc != ADF_RC_OK || ! adfEndOfFile ( file ) ) {
        return 1;
    }
    //unsigned nerrors = 0;
    unsigned nDataBlocks = adfFileSize2Datablocks ( file->fileHdr->byteSize,
                                                    file->volume->datablockSize );
    unsigned nExtBlocks  = adfFileDatablocks2Extblocks ( nDataBlocks );
    //printf ("nDataBlocks %u, nExtBlocks %u, bufsize %u, truncsize %u\n",
    //        nDataBlocks, nExtBlocks, bufsize, truncsize );
    //fflush(stdout);

    // make sure we have available current ext. block (if needed)
    //int32_t * const dataBlocks = ( nExtBlocks < 1 ) ? file->fileHdr->dataBlocks :
    //                                                  file->currentExt->dataBlocks;
    struct AdfFileExtBlock * fext = NULL;
    int32_t * dataBlocks = NULL;
    if ( nExtBlocks < 1 ) {
        dataBlocks = file->fileHdr->dataBlocks;
    } else {  // ( nExtBlocks >= 1 )
        if  ( file->volume->datablockSize != 488 ) {
            // FFS
            //ck_assert_ptr_nonnull ( file->currentExt );
            if ( file->currentExt == NULL )
                return 2;
            dataBlocks = file->currentExt->dataBlocks;
        } else {
            // for OFS - we must read the current ext.(!)
            fext = (struct AdfFileExtBlock *) malloc ( sizeof (struct AdfFileExtBlock) );
            if ( fext == NULL )
                return 3;
            if ( adfFileReadExtBlockN ( file, (int) nExtBlocks - 1, fext ) !=  ADF_RC_OK )
                return 4;
            dataBlocks = fext->dataBlocks;
        }
    }

    // check the number of non-zero blocks in the array of the last metadata block (header or ext)
    unsigned nonZeroCount = 0;
    for ( unsigned i = 0 ; i < ADF_MAX_DATABLK ; ++i ) {
        if ( dataBlocks[i] != 0 )
            nonZeroCount++;
    }
    free(fext);

    if ( file->fileHdr->byteSize == 0 ) {
        //ck_assert_uint_eq ( nonZeroCount, 0 );
        if ( nonZeroCount != 0 )
            return 5;
    } else {
        unsigned nonZeroExpected = ( nDataBlocks % ADF_MAX_DATABLK != 0 ?
                                     nDataBlocks % ADF_MAX_DATABLK :
                                     ADF_MAX_DATABLK );
        //ck_assert_uint_eq ( nonZeroCount, nonZeroExpected );
        if ( nonZeroCount != nonZeroExpected ) {
            printf ("Incorrect number of non-zero blocks in the last metadata block:"
                    "nonZeroCount %u != nonZeroExpected %u, filesize %u",
                    nonZeroCount, nonZeroExpected, file->fileHdr->byteSize );
            return 6;
        }
    }
    return 0;
}


unsigned validate_file_metadata ( struct AdfVolume * const vol,
                                  const char * const       filename,
                                  const unsigned           errors_max )
{
    (void) errors_max;
    struct AdfFile * const file = adfFileOpen ( vol, filename, ADF_FILE_MODE_READ );
    if ( ! file )
        return 1;
    unsigned nerrors = validate_file_metadata_last_ext ( file );
    adfFileClose ( file );
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
        buf[i] = (unsigned char) ( rand() & 0xff );
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
    //return max ( ( data_blocks - 1 ) / ADF_MAX_DATABLK, 0 );
    if ( data_blocks < 1 )
        return 0;
    return ( data_blocks - 1 ) / ( ADF_MAX_DATABLK );
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
    //return max ( ( datablock_idx - ADF_MAX_DATABLK ) % ( ADF_MAX_DATABLK + 1 ), 0 );

    if ( datablock_idx < ADF_MAX_DATABLK )
        // block pointer in file header (not in an ext. block)
        //return -1;
        return 0;
    return ( datablock_idx - ADF_MAX_DATABLK ) % ( ADF_MAX_DATABLK );
}
