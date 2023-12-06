#ifndef _ADF_VOL_H
#define _ADF_VOL_H 1

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

#include "prefix.h"

#include "adf_types.h"
#include "adf_err.h"
#include "adf_str.h"

/* ----- VOLUME ----- */

struct AdfBitmap {
    uint32_t               size;         /* in blocks */
    SECTNUM *              blocks;       /* bitmap blocks pointers */
    struct bBitmapBlock ** table;
    BOOL *                 blocksChg;
};

struct AdfVolume {
    struct AdfDevice *dev;

    SECTNUM firstBlock;     /* first block of data area (from beginning of device) */
    SECTNUM lastBlock;      /* last block of data area  (from beginning of device) */
    SECTNUM rootBlock;      /* root block (from firstBlock) */

    uint8_t dosType;           /* FFS/OFS, DIRCACHE, INTERNATIONAL */
    BOOL bootCode;
    BOOL readOnly;
    unsigned datablockSize;      /* 488 or 512 */
    unsigned blockSize;			/* 512 */

    char *volName;

    BOOL mounted;

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


PREFIX RETCODE adfInstallBootBlock ( struct AdfVolume * const vol,
                                     const uint8_t * const    code );

PREFIX BOOL isSectNumValid ( const struct AdfVolume * const vol,
                             const SECTNUM                  nSect );

PREFIX struct AdfVolume * adfMount ( struct AdfDevice * const dev,
                                     const int                nPart,
                                     const AdfAccessMode      mode );

PREFIX void adfUnMount ( struct AdfVolume * const vol );

PREFIX void adfVolumeInfo ( struct AdfVolume * const vol );

struct AdfVolume * adfCreateVol ( struct AdfDevice * const dev,
                                  const uint32_t           start,
                                  const uint32_t           len,
                                  const char * const       volName,
                                  const uint8_t            volType );

RETCODE adfVolReconstructBitmap ( struct AdfVolume * const vol );

/*void adfReadBitmap(struct AdfVolume* , int32_t nBlock, struct bRootBlock* root);
void adfUpdateBitmap(struct AdfVolume*);
*/
PREFIX RETCODE adfReadBlock ( struct AdfVolume * const vol,
                              const uint32_t           nSect,
                              uint8_t * const          buf );

PREFIX RETCODE adfWriteBlock ( struct AdfVolume * const vol,
                               const uint32_t           nSect,
                               const uint8_t * const    buf );

#endif /* _ADF_VOL_H */

/*##########################################################################*/
