#ifndef _ADF_DEV_HD_H
#define _ADF_DEV_HD_H 1

/*
 *  ADF Library. (C) 1997-2002 Laurent Clevy
 *
 *  adf_dev_hd.h
 *
 *  $Id$
 *
 *  Harddisk devices code
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


#include "adf_dev.h"
#include "adf_err.h"
#include "hd_blk.h"
#include "prefix.h"

RETCODE adfMountHd ( struct AdfDevice * const dev );
RETCODE adfMountHdFile ( struct AdfDevice * const dev );

RETCODE adfCreateHdHeader ( struct AdfDevice * const               dev,
                            const int                              n,
                            const struct Partition * const * const partList );

PREFIX RETCODE adfCreateHd ( struct AdfDevice * const               dev,
                             const unsigned                         n,
                             const struct Partition * const * const partList );

PREFIX RETCODE adfCreateHdFile ( struct AdfDevice * const dev,
                                 const char * const       volName,
                                 const uint8_t            volType );

RETCODE adfReadRDSKblock ( struct AdfDevice * const  dev,
                           struct bRDSKblock * const blk );

RETCODE adfWriteRDSKblock ( struct AdfDevice * const  dev,
                            struct bRDSKblock * const rdsk );

RETCODE adfReadPARTblock ( struct AdfDevice * const  dev,
                           const int32_t             nSect,
                           struct bPARTblock * const blk );

RETCODE adfWritePARTblock ( struct AdfDevice * const  dev,
                            const int32_t             nSect,
                            struct bPARTblock * const part );

RETCODE adfReadFSHDblock ( struct AdfDevice * const  dev,
                           const int32_t             nSect,
                           struct bFSHDblock * const blk );

RETCODE adfWriteFSHDblock ( struct AdfDevice * const  dev,
                            const int32_t             nSect,
                            struct bFSHDblock * const fshd );

RETCODE adfReadLSEGblock ( struct AdfDevice * const  dev,
                           const int32_t             nSect,
                           struct bLSEGblock * const blk );

RETCODE adfWriteLSEGblock ( struct AdfDevice * const  dev,
                            const int32_t             nSect,
                            struct bLSEGblock * const lseg );
#endif /* _ADF_HD_H */

/*##########################################################################*/
