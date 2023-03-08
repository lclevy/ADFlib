#ifndef ADF_FILE_BLOCK_H
#define ADF_FILE_BLOCK_H 1

/*
 *  ADF Library. (C) 1997-2002 Laurent Clevy
 *
 *  adf_file_block.h
 *
 *  $Id$
 *
 *  Volume block-level code for files
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

struct AdfFileBlocks {
    SECTNUM   header;
    int32_t   nbExtens;
    SECTNUM * extens;
    int32_t   nbData;
    SECTNUM * data;
};


RETCODE adfGetFileBlocks ( struct AdfVolume * const        vol,
                           struct bFileHeaderBlock * const entry,
                           struct AdfFileBlocks * const    fileBlocks );

RETCODE adfFreeFileBlocks ( struct AdfVolume * const        vol,
                            struct bFileHeaderBlock * const entry );

PREFIX int32_t adfFileRealSize ( uint32_t        size,
                                 int             blockSize,
                                 int32_t * const dataN,
                                 int32_t * const extN );

RETCODE adfWriteFileHdrBlock ( struct AdfVolume * const        vol,
                               const SECTNUM                   nSect,
                               struct bFileHeaderBlock * const fhdr );

RETCODE adfReadDataBlock ( struct AdfVolume * const vol,
                           const SECTNUM            nSect,
                           void * const             data );

RETCODE adfWriteDataBlock ( struct AdfVolume * const vol,
                            const SECTNUM            nSect,
                            void * const             data );

PREFIX RETCODE adfReadFileExtBlock ( struct AdfVolume * const     vol,
                                     const SECTNUM                nSect,
                                     struct bFileExtBlock * const fext );

PREFIX RETCODE adfWriteFileExtBlock ( struct AdfVolume * const     vol,
                                      const SECTNUM                nSect,
                                      struct bFileExtBlock * const fext );
#endif
