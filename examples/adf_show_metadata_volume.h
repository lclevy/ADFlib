
#ifndef _ADF_SHOW_METADATA_VOLUME_H__
#define _ADF_SHOW_METADATA_VOLUME_H__

#include <adflib.h>
#include <adf_blk.h>
#include <stdbool.h>

void show_volume_metadata ( struct AdfVolume * const vol );

void show_bootblock ( const struct AdfBootBlock * const bblock,
                      bool                              show_data );

void show_bootblock_data ( const struct AdfBootBlock * const bblock );

void show_rootblock ( const struct AdfRootBlock * const rblock );

#endif
