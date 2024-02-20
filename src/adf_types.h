/*
 *  ADF Library. (C) 1997-2002 Laurent Clevy
 *
 *  adf_types.h
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
 *  along with ADFLib; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef _ADF_TYPES_H
#define _ADF_TYPES_H 1

#include <stdint.h>

typedef int32_t SECTNUM;

typedef int      BOOL;

#define TRUE    1
#define FALSE   0

typedef enum {
    ADF_ACCESS_MODE_READWRITE = 0,
    ADF_ACCESS_MODE_READONLY  = 1
} AdfAccessMode;

#endif
