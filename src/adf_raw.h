/*
 *  ADF Library. (C) 1997-2002 Laurent Clevy
 *
 *  adf_raw.h
 *
 *  $Id$
 *
 *  blocks level code
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

#ifndef _ADF_RAW_H
#define _ADF_RAW_H 1

#include "adf_blk.h"
#include "adf_err.h"
#include "adf_vol.h"

#define ADF_SWBL_BOOT         0
#define ADF_SWBL_ROOT         1
#define ADF_SWBL_DATA         2
#define ADF_SWBL_FILE         3
#define ADF_SWBL_ENTRY        3
#define ADF_SWBL_DIR          3
#define ADF_SWBL_CACHE        4
#define ADF_SWBL_BITMAP       5
#define ADF_SWBL_FEXT         5
#define ADF_SWBL_LINK         6
#define ADF_SWBL_BITMAPE      5
#define ADF_SWBL_RDSK         7
#define ADF_SWBL_BADB         8
#define ADF_SWBL_PART         9
#define ADF_SWBL_FSHD         10
#define ADF_SWBL_LSEG         11

PREFIX RETCODE adfReadRootBlock ( struct AdfVolume * const  vol,
                                  const uint32_t            nSect,
                                  struct bRootBlock * const root );

PREFIX RETCODE adfWriteRootBlock ( struct AdfVolume * const  vol,
                                   const uint32_t            nSect,
                                   struct bRootBlock * const root );

PREFIX RETCODE adfReadBootBlock ( struct AdfVolume * const  vol,
                                  struct bBootBlock * const boot );

PREFIX RETCODE adfWriteBootBlock ( struct AdfVolume * const  vol,
                                   struct bBootBlock * const boot );

PREFIX uint32_t adfBootSum ( const uint8_t * const buf );

PREFIX uint32_t adfNormalSum ( const uint8_t * const buf,
			       const int offset,
			       const int bufLen );

PREFIX void adfSwapEndian ( uint8_t * const buf,
                            const int       type );

#endif /* _ADF_RAW_H */

/*##########################################################################*/
