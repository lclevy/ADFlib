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

#ifndef ADF_DEV_HD_H
#define ADF_DEV_HD_H

#include "adf_dev.h"
#include "adf_err.h"
#include "adf_blk_hd.h"
#include "adf_prefix.h"

ADF_RETCODE adfMountHd ( struct AdfDevice * const dev );
ADF_RETCODE adfMountHdFile ( struct AdfDevice * const dev );

ADF_RETCODE adfCreateHdHeader ( struct AdfDevice * const               dev,
                                const int                              n,
                                const struct Partition * const * const partList );

ADF_PREFIX ADF_RETCODE adfCreateHd (
    struct AdfDevice * const               dev,
    const unsigned                         n,
    const struct Partition * const * const partList );

ADF_PREFIX ADF_RETCODE adfCreateHdFile ( struct AdfDevice * const dev,
                                         const char * const       volName,
                                         const uint8_t            volType );

ADF_RETCODE adfReadRDSKblock ( struct AdfDevice * const    dev,
                               struct AdfRDSKblock * const blk );

ADF_RETCODE adfWriteRDSKblock ( struct AdfDevice * const    dev,
                                struct AdfRDSKblock * const rdsk );

ADF_RETCODE adfReadPARTblock ( struct AdfDevice * const    dev,
                               const int32_t               nSect,
                               struct AdfPARTblock * const blk );

ADF_RETCODE adfWritePARTblock ( struct AdfDevice * const    dev,
                                const int32_t               nSect,
                                struct AdfPARTblock * const part );

ADF_RETCODE adfReadFSHDblock ( struct AdfDevice * const    dev,
                               const int32_t               nSect,
                               struct AdfFSHDblock * const blk );

ADF_RETCODE adfWriteFSHDblock ( struct AdfDevice * const    dev,
                                const int32_t               nSect,
                                struct AdfFSHDblock * const fshd );

ADF_RETCODE adfReadLSEGblock ( struct AdfDevice * const    dev,
                               const int32_t               nSect,
                               struct AdfLSEGblock * const blk );

ADF_RETCODE adfWriteLSEGblock ( struct AdfDevice * const    dev,
                                const int32_t               nSect,
                                struct AdfLSEGblock * const lseg );
#endif /* ADF_DEV_HD_H */
