/*
 *  ADF Library. (C) 1997-2002 Laurent Clevy
 *
 *  adf_dir.h
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
 *  along with ADFLib; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef ADF_DIR_H
#define ADF_DIR_H

#include "adf_blk.h"
#include "adf_err.h"
#include "adf_prefix.h"
#include "adf_vol.h"


/* ----- ENTRY ---- */

struct AdfEntry {
    int type;
    char* name;
    ADF_SECTNUM sector;
    ADF_SECTNUM real;
    ADF_SECTNUM parent;
    char* comment;
    uint32_t size;
    int32_t access;
    int year, month, days;
    int hour, mins, secs;
};


ADF_PREFIX ADF_RETCODE adfToRootDir ( struct AdfVolume * const vol );
bool isDirEmpty ( const struct AdfDirBlock * const dir );

ADF_PREFIX ADF_RETCODE adfRemoveEntry ( struct AdfVolume * const vol,
                                        const ADF_SECTNUM        pSect,
                                        const char * const       name );

ADF_PREFIX struct AdfList * adfGetDirEnt ( struct AdfVolume * const vol,
                                           const ADF_SECTNUM        nSect );

ADF_PREFIX struct AdfList * adfGetRDirEnt ( struct AdfVolume * const vol,
                                            const ADF_SECTNUM        nSect,
                                            const bool               recurs );

ADF_PREFIX void adfFreeDirList ( struct AdfList * const list );

ADF_PREFIX int adfDirCountEntries ( struct AdfVolume * const vol,
                                    const ADF_SECTNUM        dirPtr );

ADF_RETCODE adfEntBlock2Entry ( const struct AdfEntryBlock * const entryBlk,
                                struct AdfEntry * const            entry );

ADF_PREFIX void adfFreeEntry ( struct AdfEntry * const entry );

ADF_RETCODE adfCreateFile ( struct AdfVolume * const          vol,
                            const ADF_SECTNUM                 parent,
                            const char * const                name,
                            struct AdfFileHeaderBlock * const fhdr );

ADF_PREFIX ADF_RETCODE adfCreateDir ( struct AdfVolume * const vol,
                                      const ADF_SECTNUM        parent,
                                      const char * const       name );

ADF_SECTNUM adfCreateEntry ( struct AdfVolume * const     vol,
                             struct AdfEntryBlock * const dir,
                             const char * const           name,
                             const ADF_SECTNUM            thisSect );

ADF_PREFIX ADF_RETCODE adfRenameEntry ( struct AdfVolume * const vol,
                                        const ADF_SECTNUM        pSect,
                                        const char * const       oldName,
                                        const ADF_SECTNUM        nPSect,
                                        const char * const       newName );

ADF_PREFIX ADF_RETCODE adfReadEntryBlock ( struct AdfVolume * const     vol,
                                           const ADF_SECTNUM            nSect,
                                           struct AdfEntryBlock * const ent );

ADF_RETCODE adfWriteDirBlock ( struct AdfVolume * const   vol,
                               const ADF_SECTNUM          nSect,
                               struct AdfDirBlock * const dir );

ADF_RETCODE adfWriteEntryBlock ( struct AdfVolume * const           vol,
                                 const ADF_SECTNUM                  nSect,
                                 const struct AdfEntryBlock * const ent );

void adfAccess2String ( int32_t acc, char accStr[ 8 + 1 ] );
uint8_t adfIntlToUpper ( const uint8_t c );
unsigned adfGetHashValue( const uint8_t * const name,
                          const bool            intl );

void adfStrToUpper ( uint8_t * const       nstr,
                     const uint8_t * const ostr,
                     const unsigned        nlen,
                     const bool            intl );

ADF_PREFIX ADF_RETCODE adfChangeDir ( struct AdfVolume * const vol,
                                      const char * const       name );
ADF_PREFIX ADF_RETCODE adfParentDir ( struct AdfVolume * const vol );

ADF_PREFIX ADF_RETCODE adfSetEntryAccess ( struct AdfVolume * const vol,
                                           const ADF_SECTNUM        parSect,
                                           const char * const       name,
                                           const int32_t            newAcc );

ADF_PREFIX ADF_RETCODE adfSetEntryComment ( struct AdfVolume * const vol,
                                            const ADF_SECTNUM        parSect,
                                            const char * const       name,
                                            const char * const       newCmt );

ADF_PREFIX ADF_SECTNUM adfGetEntryByName ( struct AdfVolume * const     vol,
                                           const ADF_SECTNUM            dirPtr,
                                           const char * const           name,
                                           struct AdfEntryBlock * const entry );

ADF_SECTNUM adfNameToEntryBlk ( struct AdfVolume * const     vol,
                                const int32_t                ht[],
                                const char * const           name,
                                struct AdfEntryBlock * const entry,
                                ADF_SECTNUM * const          nUpdSect );

ADF_PREFIX void adfEntryPrint ( const struct AdfEntry * const entry );

#endif  /* ADF_DIR_H */
