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

struct bEntryBlock {
/*000*/	int32_t	type;		/* T_HEADER == 2 */
/*004*/	int32_t	headerKey;	/* current block number */
        int32_t	r1[3];
/*014*/	uint32_t	checkSum;
/*018*/	int32_t	hashTable[HT_SIZE];
        int32_t	r2[2];
/*140*/	int32_t	access;	/* bit0=del, 1=modif, 2=write, 3=read */
/*144*/	int32_t	byteSize;
/*148*/	char	commLen;
/*149*/	char	comment[MAXCMMTLEN+1];
        char	r3[91-(MAXCMMTLEN+1)];
/*1a4*/	int32_t	days;
/*1a8*/	int32_t	mins;
/*1ac*/	int32_t	ticks;
/*1b0*/	char	nameLen;
/*1b1*/	char	name[MAXNAMELEN+1];
        int32_t	r4;
/*1d4*/	int32_t	realEntry;
/*1d8*/	int32_t	nextLink;
        int32_t	r5[5];
/*1f0*/	int32_t	nextSameHash;
/*1f4*/	int32_t	parent;
/*1f8*/	int32_t	extension;
/*1fc*/	int32_t	secType;
	};


#define ENV_DECLARATION struct Env adfEnv


#endif /* _ADF_STR_H */
/*##########################################################################*/
