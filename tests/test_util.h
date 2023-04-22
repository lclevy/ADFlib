
#ifndef __TEST_UTIL_H__
#define __TEST_UTIL_H__

#include "adflib.h"


unsigned verify_file_data ( struct AdfVolume * const    vol,
                            const char * const          filename,
                            const unsigned char * const buffer,
                            const unsigned              bytes_written,
                            const unsigned              errors_max );

void pattern_AMIGAMIG ( unsigned char * buf,
                        const unsigned  BUFSIZE );

void pattern_random ( unsigned char * buf,
                      const unsigned  BUFSIZE );


unsigned pos2datablockIndex ( unsigned pos,
                              unsigned blocksize );

unsigned filesize2datablocks ( unsigned fsize,
                               unsigned blocksize );

unsigned datablocks2extblocks ( unsigned data_blocks );

unsigned filesize2blocks ( unsigned fsize,
                           unsigned blocksize );

unsigned datablock2posInExtBlk ( unsigned datablock_idx );

#endif
