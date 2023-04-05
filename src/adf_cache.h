#ifndef _ADF_CACHE_H
#define _ADF_CACHE_H 1
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
 *  along with Foobar; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#include "adf_blk.h"
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
    char name[MAXNAMELEN+1],
         comm[MAXCMMTLEN+1];
/*    char *name, *comm;*/
};


void adfGetCacheEntry ( const struct bDirCacheBlock * const dirc,
                        int * const                   p,
                        struct AdfCacheEntry * const  cEntry );

int adfPutCacheEntry ( struct bDirCacheBlock * dirc,
                       int *                   p,
                       struct AdfCacheEntry *  cEntry );

struct AdfList * adfGetDirEntCache ( struct AdfVolume * const vol,
                                     const SECTNUM            dir,
                                     const BOOL               recurs );

RETCODE adfCreateEmptyCache ( struct AdfVolume *   vol,
                              struct bEntryBlock * parent,
                              SECTNUM              nSect );

RETCODE adfAddInCache ( struct AdfVolume *   vol,
                        struct bEntryBlock * parent,
                        struct bEntryBlock * entry );

RETCODE adfUpdateCache ( struct AdfVolume *   vol,
                         struct bEntryBlock * parent,
                         struct bEntryBlock * entry,
                         BOOL                 entryLenChg );

RETCODE adfDelFromCache ( struct AdfVolume *   vol,
                          struct bEntryBlock * parent,
                          SECTNUM              headerKey );

RETCODE adfReadDirCBlock ( struct AdfVolume *      vol,
                           SECTNUM                 nSect,
                           struct bDirCacheBlock * dirc );

RETCODE adfWriteDirCBlock ( struct AdfVolume *      vol,
                            int32_t                 nSect,
                            struct bDirCacheBlock * dirc );

#endif /* _ADF_CACHE_H */

/*##########################################################################*/
