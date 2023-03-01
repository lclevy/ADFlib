
#ifndef __TEST_UTIL_H__
#define __TEST_UTIL_H__

#include "adflib.h"


unsigned verify_file_data ( struct AdfVolume * const vol,
                            char * const             filename,
                            unsigned char * const    buffer,
                            const unsigned           bytes_written,
                            const unsigned           errors_max );

void pattern_AMIGAMIG ( unsigned char * buf,
                        const unsigned  BUFSIZE );

void pattern_random ( unsigned char * buf,
                      const unsigned  BUFSIZE );


int pos2datablockIndex ( int pos,
                         int blocksize );

int filesize2datablocks ( int fsize,
                          int blocksize );

int datablocks2extblocks ( int data_blocks );

int filesize2blocks ( int fsize,
                      int blocksize );

int datablock2posInExtBlk ( int datablock_idx );

#endif
