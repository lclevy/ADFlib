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
    struct AdfVolume *volume;

    struct bFileHeaderBlock* fileHdr;
    void *currentData;
    struct bFileExtBlock* currentExt;

    int32_t nDataBlock;  /* current data block number */
    SECTNUM curDataPtr;  /* sector number of current data block */
    uint32_t pos;

    int posInDataBlk;
    int posInExtBlk;
    BOOL eof, writeMode;
};

struct AdfFileBlocks {
    SECTNUM header;
    int32_t nbExtens;
    SECTNUM* extens;
    int32_t nbData;
    SECTNUM* data;
};

RETCODE adfGetFileBlocks ( struct AdfVolume *        vol,
                           struct bFileHeaderBlock * entry,
                           struct AdfFileBlocks *    fileBlocks );

RETCODE adfFreeFileBlocks ( struct AdfVolume *        vol,
                            struct bFileHeaderBlock * entry );

PREFIX int32_t adfFileRealSize(uint32_t size, int blockSize, int32_t *dataN, int32_t *extN);

PREFIX int32_t adfPos2DataBlock(int32_t pos, int blockSize, int *posInExtBlk, int *posInDataBlk, int32_t *curDataN );

RETCODE adfWriteFileHdrBlock ( struct AdfVolume *        vol,
                               SECTNUM                   nSect,
                               struct bFileHeaderBlock * fhdr );

RETCODE adfReadDataBlock ( struct AdfVolume * vol,
                           SECTNUM            nSect,
                           void *             data );

RETCODE adfWriteDataBlock ( struct AdfVolume * vol,
                            SECTNUM            nSect,
                            void *             data );

PREFIX RETCODE adfReadFileExtBlock ( struct AdfVolume *     vol,
                                     SECTNUM                nSect,
                                     struct bFileExtBlock * fext );

PREFIX RETCODE adfWriteFileExtBlock ( struct AdfVolume *     vol,
                                      SECTNUM                nSect,
                                      struct bFileExtBlock * fext );

PREFIX struct AdfFile * adfOpenFile ( struct AdfVolume * vol,
                                      char *             name,
                                      char *             mode );

PREFIX void adfCloseFile ( struct AdfFile * file );

PREFIX int32_t adfReadFile ( struct AdfFile * file,
                             int32_t          n,
                             uint8_t *        buffer );

PREFIX BOOL adfEndOfFile ( struct AdfFile * file );

PREFIX void adfFileSeek ( struct AdfFile * file,
                          uint32_t         pos );		/* BV */

RETCODE adfReadNextFileBlock ( struct AdfFile * file );

PREFIX int32_t adfWriteFile ( struct AdfFile * file,
                              int32_t          n,
                              uint8_t *        buffer );

SECTNUM adfCreateNextFileBlock ( struct AdfFile * file );

PREFIX void adfFlushFile ( struct AdfFile * file );

RETCODE adfReadFileExtBlockN ( struct AdfFile *       file,
                               int32_t                extBlock,
                               struct bFileExtBlock * fext );
#endif /* ADF_FILE_H */

