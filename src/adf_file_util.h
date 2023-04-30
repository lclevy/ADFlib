
#ifndef __ADF_FILE_UTIL__
#define __ADF_FILE_UTIL__

#include "adf_blk.h"

#define NDEBUG
#include <assert.h>

static inline unsigned adfFilePos2datablockIndex ( unsigned pos,
                                                   unsigned blocksize )
{
    assert ( blocksize > 0 );
    return ( pos / blocksize );
}

static inline unsigned adfFileSize2Datablocks ( unsigned fsize,
                                                unsigned blocksize )
{
    assert ( blocksize > 0 );
    return ( fsize / blocksize +
             ( ( fsize % blocksize > 0 ) ? 1 : 0 ) );
}

static inline unsigned adfFileDatablocks2Extblocks ( unsigned ndatablocks )
{
    //return max ( ( ndata_blocks - 1 ) / MAX_DATABLK, 0 );
    if ( ndatablocks < 1 )
        return 0;
    return ( ndatablocks - 1 ) / ( MAX_DATABLK );
}

static inline unsigned adfFileSize2Blocks ( unsigned fsize,
                                            unsigned blocksize )
{
    //assert ( blocksize > 0 );
    unsigned data_blocks = adfFileSize2Datablocks ( fsize, blocksize );
    unsigned ext_blocks  = adfFileDatablocks2Extblocks ( data_blocks );
    return data_blocks + ext_blocks + 1;   // +1 for the file header block
}

#endif
