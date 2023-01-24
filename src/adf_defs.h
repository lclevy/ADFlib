/*
 *  ADF Library. (C) 1997-2002 Laurent Clevy
 *
 *  adf_defs.h
 *
 *  $Id$
 *
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


#ifndef _ADF_DEFS_H
#define _ADF_DEFS_H 1

#include <stdint.h>

#include "config.h"
/* autotools defines this */
#ifdef PACKAGE_VERSION
#define ADFLIB_VERSION PACKAGE_VERSION
#endif

#define ADFLIB_DATE "January 24th, 2022"


#define SECTNUM int32_t
#define RETCODE int32_t

#define TRUE    1
#define FALSE   0

#define ULONG   uint32_t
#define USHORT  uint16_t
#define UCHAR   uint8_t
#define BOOL    int


/* defines max and min */
#ifndef max
#if defined(__clang__) || defined(__GNUC__)
#define max(a,b)             \
({                           \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a > _b ? _a : _b;       \
})
#else
#define max(a,b)        ((a)>(b) ? (a) : (b))
#endif
#endif

#ifndef min
#if defined(__clang__) || defined(__GNUC__)
#define min(a,b)             \
({                           \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b;       \
})
#else
#define min(a,b)        ((a)<(b) ? (a) : (b))
#endif
#endif


/* (*byte) to (*short) and (*byte) to (*long) conversion */

#define Short(p) ((p)[0]<<8 | (p)[1])
#define Long(p) (Short(p)<<16 | Short(p+2))


/* swap short and swap long macros for little endian machines */

#define swapShort(p) ((p)[0]<<8 | (p)[1])
#define swapLong(p) (swapShort(p)<<16 | swapShort(p+2))



#endif /* _ADF_DEFS_H */
/*##########################################################################*/
