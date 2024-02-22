
#ifndef _ADF_SHOW_METADATA_FILE_H__
#define _ADF_SHOW_METADATA_FILE_H__

#include <adflib.h>

void show_file_metadata ( struct AdfVolume * const vol,
                          SECTNUM                  fheader_sector );

void show_file_header_block ( const struct AdfFileHeaderBlock * const block );

void show_file_ext_blocks ( struct AdfVolume * const                vol,
                            const struct AdfFileHeaderBlock * const fheader_block );

void show_ext_block ( const struct AdfFileExtBlock * const extblock );
void show_file_data_blocks_array ( const int32_t datablocks[ADF_MAX_DATABLK] );

#endif
