/*
 *  ADF Library. (C) 1997-2002 Laurent Clevy
 *
 *  adf_bitm.h
 *
 *  $Id$
 *
 *  bitmap code
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

#ifndef ADF_BITM_H
#define ADF_BITM_H

#include "adf_blk.h"
#include "adf_err.h"
#include "adf_prefix.h"
#include "adf_types.h"
#include "adf_vol.h"

ADF_PREFIX bool adfVolBitmapIsMarkedValid ( struct AdfVolume * const vol );

ADF_PREFIX ADF_RETCODE adfReadBitmapBlock (
    struct AdfVolume * const      vol,
    const ADF_SECTNUM             nSect,
    struct AdfBitmapBlock * const bitm );

ADF_PREFIX ADF_RETCODE adfWriteBitmapBlock (
    struct AdfVolume * const            vol,
    const ADF_SECTNUM                   nSect,
    const struct AdfBitmapBlock * const bitm );

ADF_PREFIX ADF_RETCODE adfReadBitmapExtBlock (
    struct AdfVolume * const         vol,
    const ADF_SECTNUM                nSect,
    struct AdfBitmapExtBlock * const bitme );

ADF_PREFIX ADF_RETCODE adfWriteBitmapExtBlock (
    struct AdfVolume * const               vol,
    const ADF_SECTNUM                      nSect,
    const struct AdfBitmapExtBlock * const bitme );

ADF_SECTNUM adfGet1FreeBlock ( struct AdfVolume * const vol );
ADF_PREFIX ADF_RETCODE adfUpdateBitmap ( struct AdfVolume * const vol );
ADF_PREFIX uint32_t adfCountFreeBlocks ( const struct AdfVolume * const vol );

ADF_RETCODE adfReadBitmap ( struct AdfVolume * const          vol,
                            const struct AdfRootBlock * const root );

ADF_PREFIX ADF_RETCODE adfReconstructBitmap (
    struct AdfVolume * const          vol,
    const struct AdfRootBlock * const root );

bool adfIsBlockFree ( const struct AdfVolume * const vol,
                      const ADF_SECTNUM              nSect );

void adfSetBlockFree ( struct AdfVolume * const vol,
                       const ADF_SECTNUM        nSect );

void adfSetBlockUsed ( struct AdfVolume * const vol,
                       const ADF_SECTNUM        nSect );

bool adfGetFreeBlocks ( struct AdfVolume * const vol,
                        const int                nbSect,
                        ADF_SECTNUM * const      sectList );

ADF_RETCODE adfCreateBitmap ( struct AdfVolume * const vol );
ADF_RETCODE adfWriteNewBitmap ( struct AdfVolume * const vol );

ADF_RETCODE adfBitmapAllocate ( struct AdfVolume * const vol );
ADF_PREFIX void adfFreeBitmap ( struct AdfVolume * const vol );

#endif  /* ADF_BITM_H */
