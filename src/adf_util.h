#ifndef _ADF_UTIL_H
#define _ADF_UTIL_H 1

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

#include "adf_defs.h"
#include "prefix.h"


struct DateTime {
    int year, mon, day, hour, min, sec;
};

void swLong ( uint8_t * const buf,
              const uint32_t  val );

void swShort ( uint8_t * const buf,
               const uint16_t  val );

PREFIX struct AdfList * newCell ( struct AdfList * list,
                                  void *           content );
PREFIX void freeList ( struct AdfList * list );
PREFIX void adfDays2Date(int32_t days, int *yy, int *mm, int *dd);
BOOL adfIsLeap(int y);
    void
adfTime2AmigaTime(struct DateTime dt, int32_t *day, int32_t *min, int32_t *ticks );
    struct DateTime
adfGiveCurrentTime( void );

void dumpBlock(uint8_t *buf);

/*##########################################################################*/
#endif /* _ADF_UTIL_H */

