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


PREFIX RETCODE adfReadBitmapBlock ( struct AdfVolume * const      vol,
                                    const SECTNUM                 nSect,
                                    struct AdfBitmapBlock * const bitm );

PREFIX RETCODE adfWriteBitmapBlock ( struct AdfVolume * const            vol,
                                     const SECTNUM                       nSect,
                                     const struct AdfBitmapBlock * const bitm );

PREFIX RETCODE adfReadBitmapExtBlock ( struct AdfVolume * const         vol,
                                       const SECTNUM                    nSect,
                                       struct AdfBitmapExtBlock * const bitme );

PREFIX RETCODE adfWriteBitmapExtBlock ( struct AdfVolume * const               vol,
                                        const SECTNUM                          nSect,
                                        const struct AdfBitmapExtBlock * const bitme );

SECTNUM adfGet1FreeBlock ( struct AdfVolume * const vol );
PREFIX RETCODE adfUpdateBitmap ( struct AdfVolume * const vol );
PREFIX uint32_t adfCountFreeBlocks ( const struct AdfVolume * const vol );

RETCODE adfReadBitmap ( struct AdfVolume * const          vol,
                        const struct AdfRootBlock * const root );

PREFIX RETCODE adfReconstructBitmap ( struct AdfVolume * const          vol,
                                      const struct AdfRootBlock * const root );

bool adfIsBlockFree ( const struct AdfVolume * const vol,
                      const SECTNUM            nSect );

void adfSetBlockFree ( struct AdfVolume * const vol,
                       const SECTNUM            nSect );

void adfSetBlockUsed ( struct AdfVolume * const vol,
                       const SECTNUM            nSect );

bool adfGetFreeBlocks ( struct AdfVolume * const vol,
                        const int                nbSect,
                        SECTNUM * const          sectList );

RETCODE adfCreateBitmap ( struct AdfVolume * const vol );
RETCODE adfWriteNewBitmap ( struct AdfVolume * const vol );

RETCODE adfBitmapAllocate ( struct AdfVolume * const vol );
PREFIX void adfFreeBitmap ( struct AdfVolume * const vol );

#endif  /* ADF_BITM_H */
