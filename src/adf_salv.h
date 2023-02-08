#ifndef _ADF_SALV_H
#define _ADF_SALV_H 1

/*
 *  ADF Library. (C) 1997-2002 Laurent Clevy
 *
 *  adf_salv.h
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

#include"prefix.h"

#include "adf_defs.h"
#include "adf_vol.h"

struct GenBlock {
    SECTNUM sect;
    SECTNUM parent;
    int type;
    int secType;
    char *name;	/* if (type == 2 and (secType==2 or secType==-3)) */
};


RETCODE adfReadGenBlock ( struct AdfVolume * vol,
                          SECTNUM            nSect,
                          struct GenBlock *  block );

PREFIX RETCODE adfCheckEntry ( struct AdfVolume * vol,
                               SECTNUM            nSect,
                               int                level );

PREFIX RETCODE adfUndelEntry ( struct AdfVolume * vol,
                               SECTNUM            parent,
                               SECTNUM            nSect );

PREFIX struct AdfList * adfGetDelEnt ( struct AdfVolume * vol );
PREFIX void adfFreeDelList ( struct AdfList * list );


/*##########################################################################*/
#endif /* _ADF_SALV_H */

