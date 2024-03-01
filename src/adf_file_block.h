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
 *  along with ADFLib; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef ADF_FILE_BLOCK_H
#define ADF_FILE_BLOCK_H

#include "adf_blk.h"
#include "adf_err.h"
#include "adf_vol.h"

struct AdfFileBlocks {
    ADF_SECTNUM  header;

    int32_t      nbExtens;
    ADF_SECTNUM *extens;

    int32_t      nbData;
    ADF_SECTNUM *data;
};


ADF_RETCODE adfGetFileBlocks ( struct AdfVolume * const                vol,
                               const struct AdfFileHeaderBlock * const entry,
                               struct AdfFileBlocks * const            fileBlocks );

ADF_RETCODE adfFreeFileBlocks ( struct AdfVolume * const          vol,
                                struct AdfFileHeaderBlock * const entry );

PREFIX uint32_t adfFileRealSize ( const uint32_t  size,
                                  const unsigned  blockSize,
                                  int32_t * const dataN,
                                  int32_t * const extN );

ADF_RETCODE adfWriteFileHdrBlock ( struct AdfVolume * const          vol,
                                   const ADF_SECTNUM                 nSect,
                                   struct AdfFileHeaderBlock * const fhdr );

ADF_RETCODE adfReadDataBlock ( struct AdfVolume * const vol,
                               const ADF_SECTNUM        nSect,
                               void * const             data );

ADF_RETCODE adfWriteDataBlock ( struct AdfVolume * const vol,
                                const ADF_SECTNUM        nSect,
                                void * const             data );

PREFIX ADF_RETCODE adfReadFileExtBlock ( struct AdfVolume * const       vol,
                                         const ADF_SECTNUM              nSect,
                                         struct AdfFileExtBlock * const fext );

PREFIX ADF_RETCODE adfWriteFileExtBlock ( struct AdfVolume * const       vol,
                                          const ADF_SECTNUM              nSect,
                                          struct AdfFileExtBlock * const fext );
#endif  /* ADF_FILE_BLOCK_H */
