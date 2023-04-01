#ifndef ADF_DUMP_H
#define ADF_DUMP_H 1

/*
 *  ADF Library. (C) 1997-2002 Laurent Clevy
 *
 *  adf_dump.h
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

#include "adf_dev.h"

PREFIX struct AdfDevice * adfCreateDumpDevice ( const char * const filename,
                                                const int32_t      cyl,
                                                const int32_t      heads,
                                                const int32_t      sec );

PREFIX RETCODE adfCreateHdFile ( struct AdfDevice * const dev,
                                 const char * const       volName,
                                 const uint8_t            volType );

BOOL adfInitDumpDevice ( struct AdfDevice * const dev,
                         const char * const       name,
                         const BOOL               ro );

BOOL adfReadDumpSector ( struct AdfDevice * const dev,
                         const uint32_t           n,
                         const unsigned           size,
                         uint8_t * const          buf );

BOOL adfWriteDumpSector ( struct AdfDevice * const dev,
                          const uint32_t           n,
                          const unsigned           size,
                          const uint8_t * const    buf );

RETCODE adfReleaseDumpDevice ( struct AdfDevice * const dev );

#endif /* ADF_DUMP_H */
/*##########################################################################*/
