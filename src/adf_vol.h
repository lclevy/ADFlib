/*
 *  ADF Library. (C) 1997-2002 Laurent Clevy
 *
 *  adf_vol.h
 *
 *  $Id$
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

#ifndef ADF_VOL_H
#define ADF_VOL_H

#include "adf_blk.h"
#include "adf_types.h"
#include "adf_err.h"
#include "adf_prefix.h"
#include "adf_str.h"

#include <string.h>

/* ----- VOLUME ----- */

struct AdfBitmap {
    uint32_t                 size;         /* in blocks */
    SECTNUM *                blocks;       /* bitmap blocks pointers */
    struct AdfBitmapBlock ** table;
    bool *                   blocksChg;
};

struct AdfVolume {
    struct AdfDevice *dev;

    SECTNUM firstBlock;     /* first block of data area (from beginning of device) */
    SECTNUM lastBlock;      /* last block of data area  (from beginning of device) */
    SECTNUM rootBlock;      /* root block (from firstBlock) */

    struct fs {
        char    id[4];          /* "DOS", "PFS", ... */
        uint8_t type;           /* DOS: FFS/OFS, DIRCACHE, INTERNATIONAL;
                                   PFS: ... */
    } fs;
    bool bootCode;
    bool readOnly;
    unsigned datablockSize;      /* 488 or 512 */
    unsigned blockSize;			/* 512 */

    char *volName;

    bool mounted;

    struct AdfBitmap bitmap;

    SECTNUM curDirPtr;
};

static inline uint32_t adfVolGetBlockNumWithoutBootblock (
    const struct AdfVolume * const vol )
{
    return (uint32_t) ( vol->lastBlock - vol->firstBlock + 1 - 2 );
}

static inline uint32_t adfVolGetBlockNum ( const struct AdfVolume * const vol )
{
    return (uint32_t) ( vol->lastBlock - vol->firstBlock + 1 );
}

static inline SECTNUM adfVolCalcRootBlk ( const struct AdfVolume * const vol )
{
    return ( vol->lastBlock - vol->firstBlock + 1 ) / 2;
}


static inline bool adfVolIsDosFS ( const struct AdfVolume * const vol ) {
    return ( strncmp ( vol->fs.id, "DOS", 3 ) == 0 );
}

static inline bool adfVolIsOFS ( const struct AdfVolume * const vol ) {
    return adfVolIsDosFS ( vol ) && adfDosFsIsOFS ( vol->fs.type );
}

static inline bool adfVolIsFFS ( const struct AdfVolume * const vol ) {
    return adfVolIsDosFS ( vol ) && adfDosFsIsFFS ( vol->fs.type );
}

static inline bool adfVolHasINTL ( const struct AdfVolume * const vol ) {
    return adfVolIsDosFS ( vol ) && adfDosFsHasINTL ( vol->fs.type );
}

static inline bool adfVolHasDIRCACHE ( const struct AdfVolume * const vol ) {
    return adfVolIsDosFS ( vol ) && adfDosFsHasDIRCACHE ( vol->fs.type );
}

static inline bool adfVolIsPFS ( const struct AdfVolume * const vol ) {
    return ( strncmp ( vol->fs.id, "PFS", 3 ) == 0 );
}

static inline bool adfVolIsFsValid (  const struct AdfVolume * const vol )
{
    return (
        ( adfVolIsOFS ( vol ) &&
          ! adfVolHasINTL ( vol ) &&
          ! adfVolHasDIRCACHE ( vol ) ) ||
        adfVolIsFFS ( vol ) ||
        adfVolIsPFS ( vol ) );
}

PREFIX char * adfVolGetFsStr ( const struct AdfVolume * const vol );


PREFIX RETCODE adfVolInstallBootBlock ( struct AdfVolume * const vol,
                                        const uint8_t * const    code );

PREFIX bool adfVolIsSectNumValid ( const struct AdfVolume * const vol,
                                   const SECTNUM                  nSect );

PREFIX struct AdfVolume * adfVolMount ( struct AdfDevice * const dev,
                                        const int                nPart,
                                        const AdfAccessMode      mode );

PREFIX RETCODE adfVolRemount ( struct AdfVolume *  vol,
                               const AdfAccessMode mode );

PREFIX void adfVolUnMount ( struct AdfVolume * const vol );

PREFIX void adfVolInfo ( struct AdfVolume * const vol );

PREFIX struct AdfVolume * adfVolCreate ( struct AdfDevice * const dev,
                                         const uint32_t           start,
                                         const uint32_t           len,
                                         const char * const       volName,
                                         const uint8_t            volType );

PREFIX RETCODE adfVolReadBlock ( struct AdfVolume * const vol,
                                 const uint32_t           nSect,
                                 uint8_t * const          buf );

PREFIX RETCODE adfVolWriteBlock ( struct AdfVolume * const vol,
                                  const uint32_t           nSect,
                                  const uint8_t * const    buf );

#endif  /* ADF_VOL_H */
