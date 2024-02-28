/*
 *  ADF Library. (C) 1997-2002 Laurent Clevy
 *
 *  adf_cache.h
 *
 *  $Id$
 *
 *  directory cache code
 *
 *  This file is part of ADFLib.
 *
 *  ADFLib is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  ADFLib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with ADFLib; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef ADF_CACHE_H
#define ADF_CACHE_H

#include "adf_blk.h"
#include "adf_err.h"
#include "adf_vol.h"


struct AdfCacheEntry {
    uint32_t header,
            size,
            protect;
    uint16_t days,
             mins,
             ticks;
    signed char type;
    uint8_t nLen,
            cLen;
    char name[ ADF_MAX_NAME_LEN + 1 ],
         comm[ ADF_MAX_COMMENT_LEN + 1 ];
/*    char *name, *comm;*/
};


ADF_RETCODE adfGetCacheEntry ( const struct AdfDirCacheBlock * const dirc,
                               int * const                           p,
                               struct AdfCacheEntry * const          cEntry );

int adfPutCacheEntry ( struct AdfDirCacheBlock * const    dirc,
                       const int * const                  p,
                       const struct AdfCacheEntry * const cEntry );

struct AdfList * adfGetDirEntCache ( struct AdfVolume * const vol,
                                     const SECTNUM            dir,
                                     const bool               recurs );

ADF_RETCODE adfCreateEmptyCache ( struct AdfVolume * const     vol,
                                  struct AdfEntryBlock * const parent,
                                  const SECTNUM                nSect );

ADF_RETCODE adfAddInCache ( struct AdfVolume * const           vol,
                            const struct AdfEntryBlock * const parent,
                            const struct AdfEntryBlock * const entry );

ADF_RETCODE adfUpdateCache ( struct AdfVolume * const           vol,
                             const struct AdfEntryBlock * const parent,
                             const struct AdfEntryBlock * const entry,
                             const bool                         entryLenChg );

ADF_RETCODE adfDelFromCache ( struct AdfVolume * const           vol,
                              const struct AdfEntryBlock * const parent,
                              const SECTNUM                      headerKey );

ADF_RETCODE adfReadDirCBlock ( struct AdfVolume * const        vol,
                               const SECTNUM                   nSect,
                               struct AdfDirCacheBlock * const dirc );

ADF_RETCODE adfWriteDirCBlock ( struct AdfVolume * const        vol,
                                const int32_t                   nSect,
                                struct AdfDirCacheBlock * const dirc );

#endif  /* ADF_CACHE_H */
