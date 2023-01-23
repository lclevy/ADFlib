#ifndef _ADF_STR_H
#define _ADF_STR_H 1

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
 *  aint32_t with Foobar; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include<stdio.h>

#include"adf_defs.h"
#include"adf_blk.h"
#include"adf_err.h"


struct List{         /* generic linked tree */
    void *content;
    struct List* subdir;
    struct List* next;
};


#define ENV_DECLARATION struct Env adfEnv


#endif /* _ADF_STR_H */
/*##########################################################################*/
