/*
 *  ADF Library. (C) 1997-2002 Laurent Clevy
 *
 *  adf_util.h
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

#ifndef ADF_UTIL_H
#define ADF_UTIL_H

#include "adf_prefix.h"
#include "adf_types.h"

#include <stdlib.h>   // for min(), max() on Windows/MSVC

struct DateTime {
    int year, mon, day, hour, min, sec;
};


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

//#define Short(p) ((p)[0]<<8 | (p)[1])
//#define Long(p) (Short(p)<<16 | Short(p+2))

static inline uint16_t Short ( const uint8_t * const p ) {
    return (uint16_t) ( ( p[0] << 8u ) | p[1] );
}

static inline uint32_t Long ( const uint8_t * const p ) {
    return (uint32_t) ( Short( p ) << 16 |
                        Short( p + 2 ) );
}


/* swap short and swap long macros for little endian machines */

//#define swapShort(p) ((p)[0]<<8 | (p)[1])
//#define swapLong(p) (swapShort(p)<<16 | swapShort(p+2))

static inline uint16_t swapShort ( const uint8_t * const p ) {
    return (uint16_t) ( ( p[0] << 8 ) | p[1] );
}

static inline uint32_t swapLong ( const uint8_t * const p ) {
    return (uint32_t) ( ( swapShort(p) << 16 ) |
                        swapShort( p + 2 ) );
}


void swLong ( uint8_t * const buf,
              const uint32_t  val );

void swShort ( uint8_t * const buf,
               const uint16_t  val );

PREFIX void adfDays2Date( int32_t       days,
                          int * const   yy,
                          int * const   mm,
                          int * const   dd );

bool adfIsLeap ( const int y );

void adfTime2AmigaTime ( struct DateTime dt,
                         int32_t * const day,
                         int32_t * const min,
                         int32_t * const ticks );

struct DateTime adfGiveCurrentTime ( void );

void dumpBlock ( const uint8_t * const buf );

#endif  /* ADF_UTIL_H */
