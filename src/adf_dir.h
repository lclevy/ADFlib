#ifndef ADF_DIR_H
#define ADF_DIR_H 1

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
 *  along with Foobar; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "adf_blk.h"
#include "adf_err.h"
#include "adf_prefix.h"
#include "adf_vol.h"


/* ----- ENTRY ---- */

struct AdfEntry {
    int type;
    char* name;
    SECTNUM sector;
    SECTNUM real;
    SECTNUM parent;
    char* comment;
    uint32_t size;
    int32_t access;
    int year, month, days;
    int hour, mins, secs;
};


PREFIX RETCODE adfToRootDir ( struct AdfVolume * const vol );
BOOL isDirEmpty ( const struct bDirBlock * const dir );

PREFIX RETCODE adfRemoveEntry ( struct AdfVolume * const vol,
                                const SECTNUM            pSect,
                                const char * const       name );

PREFIX struct AdfList * adfGetDirEnt ( struct AdfVolume * const vol,
                                       const SECTNUM            nSect );

PREFIX struct AdfList * adfGetRDirEnt ( struct AdfVolume * const vol,
                                        const SECTNUM            nSect,
                                        const BOOL               recurs );

PREFIX void adfFreeDirList ( struct AdfList * const list );

PREFIX int adfDirCountEntries ( struct AdfVolume * const vol,
                                const SECTNUM            dirPtr );

RETCODE adfEntBlock2Entry ( const struct bEntryBlock * const entryBlk,
                            struct AdfEntry * const          entry );

PREFIX void adfFreeEntry (struct AdfEntry * const entry );

RETCODE adfCreateFile ( struct AdfVolume * const        vol,
                        const SECTNUM                   parent,
                        const char * const              name,
                        struct bFileHeaderBlock * const fhdr );

PREFIX RETCODE adfCreateDir ( struct AdfVolume * const vol,
                              const SECTNUM            parent,
                              const char * const       name );

SECTNUM adfCreateEntry ( struct AdfVolume * const   vol,
                         struct bEntryBlock * const dir,
                         const char * const         name,
                         const SECTNUM              thisSect );

PREFIX RETCODE adfRenameEntry ( struct AdfVolume * const vol,
                                const SECTNUM            pSect,
                                const char * const       oldName,
                                const SECTNUM            nPSect,
                                const char * const       newName );

PREFIX RETCODE adfReadEntryBlock ( struct AdfVolume * const   vol,
                                   const SECTNUM              nSect,
                                   struct bEntryBlock * const ent );

RETCODE adfWriteDirBlock ( struct AdfVolume * const vol,
                           const SECTNUM            nSect,
                           struct bDirBlock * const dir );

RETCODE adfWriteEntryBlock ( struct AdfVolume * const         vol,
                             const SECTNUM                    nSect,
                             const struct bEntryBlock * const ent );

char* adfAccess2String(int32_t acc);
uint8_t adfIntlToUpper ( const uint8_t c );
unsigned adfGetHashValue( const uint8_t * const name,
                          const BOOL            intl );

void adfStrToUpper ( uint8_t * const       nstr,
                     const uint8_t * const ostr,
                     const unsigned        nlen,
                     const BOOL            intl );

PREFIX RETCODE adfChangeDir ( struct AdfVolume * const vol,
                              const char * const       name );
PREFIX RETCODE adfParentDir ( struct AdfVolume * const vol );

PREFIX RETCODE adfSetEntryAccess ( struct AdfVolume * const vol,
                                   const SECTNUM            parSect,
                                   const char * const       name,
                                   const int32_t            newAcc );

PREFIX RETCODE adfSetEntryComment ( struct AdfVolume * const vol,
                                    const SECTNUM            parSect,
                                    const char * const       name,
                                    const char * const       newCmt );

PREFIX SECTNUM adfGetEntryByName ( struct AdfVolume * const   vol,
                                   const SECTNUM              dirPtr,
                                   const char * const         name,
                                   struct bEntryBlock * const entry );

SECTNUM adfNameToEntryBlk ( struct AdfVolume * const   vol,
                            const int32_t              ht[],
                            const char * const         name,
                            struct bEntryBlock * const entry,
                            SECTNUM * const            nUpdSect );

PREFIX void adfEntryPrint ( const struct AdfEntry * const entry );

#endif /* ADF_DIR_H */

