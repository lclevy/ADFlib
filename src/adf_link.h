/*
 *  ADF Library. (C) 1997-2002 Laurent Clevy
 *
 *  adf_link.h
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

#ifndef ADF_LINK_H
#define ADF_LINK_H

#include "adf_prefix.h"
#include "adf_types.h"
#include "adf_vol.h"

#if defined (__ANY_IDEA_WHAT_IS_THIS_FOR__)
// the code below is not used anywhere and seems unfinished
// (not clear what it was meant for...)
// -> disabling it for now
PREFIX ADF_RETCODE adfBlockPtr2EntryName ( struct AdfVolume * vol,
                                           SECTNUM            nSect,
                                           SECTNUM            lPar,
                                           char **            name,
                                           int32_t *          size );
#endif

#endif  /* ADF_LINK_H */
