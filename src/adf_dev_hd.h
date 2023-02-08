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
 *  along with Foobar; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include"prefix.h"

#include "adf_dev.h"
#include "hd_blk.h"

RETCODE adfMountHd ( struct adfDevice * dev );
RETCODE adfMountHdFile ( struct adfDevice * dev );

RETCODE adfCreateHdHeader ( struct adfDevice *  dev,
                            int                 n,
                            struct Partition ** partList );

PREFIX RETCODE adfCreateHd ( struct adfDevice *  dev,
                             int                 n,
                             struct Partition ** partList );

PREFIX RETCODE adfCreateHdFile ( struct adfDevice * dev,
                                 char *             volName,
                                 int                volType );

RETCODE adfReadRDSKblock ( struct adfDevice *  dev,
                           struct bRDSKblock * blk );

RETCODE adfWriteRDSKblock ( struct adfDevice *  dev,
                            struct bRDSKblock * rdsk );

RETCODE adfReadPARTblock ( struct adfDevice *  dev,
                           int32_t             nSect,
                           struct bPARTblock * blk );

RETCODE adfWritePARTblock ( struct adfDevice *  dev,
                            int32_t             nSect,
                            struct bPARTblock * part );

RETCODE adfReadFSHDblock ( struct adfDevice *  dev,
                           int32_t             nSect,
                           struct bFSHDblock * blk );

RETCODE adfWriteFSHDblock ( struct adfDevice *  dev,
                            int32_t             nSect,
                            struct bFSHDblock * fshd );

RETCODE adfReadLSEGblock ( struct adfDevice *  dev,
                           int32_t             nSect,
                           struct bLSEGblock * blk );

RETCODE adfWriteLSEGblock ( struct adfDevice *  dev,
                            int32_t             nSect,
                            struct bLSEGblock * lseg );
#endif /* _ADF_HD_H */

/*##########################################################################*/
