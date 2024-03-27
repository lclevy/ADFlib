/*
 *  ADF Library. (C) 1997-2002 Laurent Clevy
 *
 *  adf_str.h
 *
 *  $Id$
 *
 *  structures/constants definitions
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

#ifndef ADF_STR_H
#define ADF_STR_H

#include "adf_err.h"
#include "adf_prefix.h"
#include "adf_types.h"

struct AdfList {         /* generic linked tree */
    void *content;
    struct AdfList *subdir;
    struct AdfList *next;
};

/* shorter, less clutter but type-unsafe version:
struct AdfVector {
    unsigned len;
    union {
        void *         contents;
        ADF_SECTNUM *  sectors;
        // ...
    };
};
*/

struct AdfVector {
    unsigned  nItems,
              itemSize;
    void *    items;
};

struct AdfVectorSectors {
    unsigned       nItems,
                   itemSize;
    ADF_SECTNUM *  sectors;
};


PREFIX struct AdfList * adfListNewCell ( struct AdfList * const list,
                                         void * const           content );

PREFIX void adfListFree ( struct AdfList * const list );

PREFIX ADF_RETCODE adfVectorAllocate ( struct AdfVector * const vector );
PREFIX void adfVectorFree ( struct AdfVector * const vector );

#endif  /* ADF_STR_H */
