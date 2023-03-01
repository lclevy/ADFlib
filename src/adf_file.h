#ifndef ADF_FILE_H
#define ADF_FILE_H 1

/*
 *  ADF Library. (C) 1997-2002 Laurent Clevy
 *
 *  adf_file.h
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


#include "adf_defs.h"
#include "adf_vol.h"
#include "prefix.h"


/* ----- FILE ----- */

struct AdfFile {
    struct AdfVolume *        volume;

    struct bFileHeaderBlock * fileHdr;
    void *                    currentData;
    struct bFileExtBlock *    currentExt;

    int32_t  nDataBlock;  /* current data block number */
    SECTNUM  curDataPtr;  /* sector number of current data block;
                             if == 0 -> data in the buffer (currentData) is
                                        invalid (eg. block not read correctly) */
    uint32_t pos;

    int posInDataBlk;
    int posInExtBlk;
    BOOL writeMode;
};

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

PREFIX int32_t adfPos2DataBlock ( int32_t   pos,
                                  int       blockSize,
                                  int *     posInExtBlk,
                                  int *     posInDataBlk,
                                  int32_t * curDataN );

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

PREFIX struct AdfFile * adfOpenFile ( struct AdfVolume * const vol,
                                      const char * const       name,
                                      const char * const       mode );

PREFIX void adfCloseFile ( struct AdfFile * const file );

PREFIX int32_t adfReadFile ( struct AdfFile * const file,
                             const int32_t          n,
                             uint8_t * const        buffer );

PREFIX BOOL adfEndOfFile ( const struct AdfFile * const file );

PREFIX RETCODE adfFileSeek ( struct AdfFile * const file,
                             const uint32_t         pos );		/* BV */

RETCODE adfReadNextFileBlock ( struct AdfFile * const file );

PREFIX int32_t adfWriteFile ( struct AdfFile * const file,
                              const int32_t          n,
                              const uint8_t * const  buffer );

SECTNUM adfCreateNextFileBlock ( struct AdfFile * const file );

PREFIX void adfFlushFile ( struct AdfFile * const file );

RETCODE adfReadFileExtBlockN ( struct AdfFile * const       file,
                               const int32_t                extBlock,
                               struct bFileExtBlock * const fext );
#endif /* ADF_FILE_H */

