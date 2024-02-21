/*
 *  ADF Library. (C) 1997-2002 Laurent Clevy
 *
 *  adf_blk.h
 *
 *  $Id$
 *
 *  general blocks structures
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


#ifndef ADF_BLK_H
#define ADF_BLK_H 1

#include "adf_types.h"


#define ADF_LOGICAL_BLOCK_SIZE 512

/* ----- FILE SYSTEM ----- */

/*
 * Filesystem type, defined in volume's bootblock as "DOSn"
 * where bitmasked n has the meaning as below.
 * ( see also: http://lclevy.free.fr/adflib/adf_info.html#p41 )
 */
#define ADF_DOSFS_OFS         0   /* 000 */
#define ADF_DOSFS_FFS         1   /* 001 */
#define ADF_DOSFS_INTL        2   /* 010 */
#define ADF_DOSFS_DIRCACHE    4   /* 100 */

static inline BOOL adfDosFsIsFFS ( const uint8_t c ) {
    return c & ADF_DOSFS_FFS;
}

static inline BOOL adfDosFsIsOFS ( const uint8_t c ) {
    return ! adfDosFsIsFFS ( c );
}

static inline BOOL adfDosFsHasINTL ( const uint8_t c ) {
    return c & ADF_DOSFS_INTL;
}

static inline BOOL adfDosFsHasDIRCACHE ( const uint8_t c ) {
    return c & ADF_DOSFS_DIRCACHE;
}


/* ----- ENTRIES ----- */

/* access constants */

#define ADF_ACCMASK_D	(1<<0)
#define ADF_ACCMASK_E	(1<<1)
#define ADF_ACCMASK_W	(1<<2)
#define ADF_ACCMASK_R	(1<<3)
#define ADF_ACCMASK_A	(1<<4)
#define ADF_ACCMASK_P	(1<<5)
#define ADF_ACCMASK_S	(1<<6)
#define ADF_ACCMASK_H	(1<<7)

static inline BOOL adfAccHasD ( const int32_t c )  { return c & ADF_ACCMASK_D; }
static inline BOOL adfAccHasE ( const int32_t c )  { return c & ADF_ACCMASK_E; }
static inline BOOL adfAccHasW ( const int32_t c )  { return c & ADF_ACCMASK_W; }
static inline BOOL adfAccHasR ( const int32_t c )  { return c & ADF_ACCMASK_R; }
static inline BOOL adfAccHasA ( const int32_t c )  { return c & ADF_ACCMASK_A; }
static inline BOOL adfAccHasP ( const int32_t c )  { return c & ADF_ACCMASK_P; }
static inline BOOL adfAccHasS ( const int32_t c )  { return c & ADF_ACCMASK_S; }
static inline BOOL adfAccHasH ( const int32_t c )  { return c & ADF_ACCMASK_H; }


/* ----- BLOCKS ----- */

/* block constants */

#define ADF_BM_VALID	-1
#define ADF_BM_INVALID	0

#define ADF_HT_SIZE            72
#define ADF_BM_PAGES_ROOT_SIZE 25
#define ADF_BM_PAGES_EXT_SIZE  127
#define ADF_BM_MAP_SIZE        127
#define ADF_MAX_DATABLK        72

#define ADF_MAX_NAME_LEN	30
#define ADF_MAX_COMMENT_LEN	79


/* block primary and secondary types */

#define ADF_T_HEADER    2
#define ADF_ST_ROOT     1
#define ADF_ST_DIR      2
#define ADF_ST_FILE    -3
#define ADF_ST_LFILE   -4
#define ADF_ST_LDIR     4
#define ADF_ST_LSOFT    3
#define ADF_T_LIST     16
#define ADF_T_DATA      8
#define ADF_T_DIRC     33


/*--- blocks structures --- */


struct AdfBootBlock {
/*000*/	char	dosType[4];
/*004*/	uint32_t checkSum;
/*008*/	int32_t	rootBlock;
/*00c*/	uint8_t data[500+512];
};


struct AdfRootBlock {
/*000*/	int32_t	type;
        int32_t	headerKey;
        int32_t	highSeq;
/*00c*/	int32_t	hashTableSize;
        int32_t	firstData;
/*014*/	uint32_t checkSum;
/*018*/	int32_t	hashTable[ADF_HT_SIZE];		/* hash table */
/*138*/	int32_t	bmFlag;				/* bitmap flag, -1 means VALID */
/*13c*/	int32_t	bmPages[ADF_BM_PAGES_ROOT_SIZE];
/*1a0*/	int32_t	bmExt;
/*1a4*/	int32_t	cDays; 	/* creation date FFS and OFS */
/*1a8*/	int32_t	cMins;
/*1ac*/	int32_t	cTicks;
/*1b0*/	uint8_t	nameLen;
/*1b1*/	char 	diskName[ ADF_MAX_NAME_LEN + 1 ];
        char	r2[8];
/*1d8*/	int32_t	days;		/* last access : days after 1 jan 1978 */
/*1dc*/	int32_t	mins;		/* hours and minutes in minutes */
/*1e0*/	int32_t	ticks;		/* 1/50 seconds */
/*1e4*/	int32_t	coDays;	/* creation date OFS */
/*1e8*/	int32_t	coMins;
/*1ec*/	int32_t	coTicks;
        int32_t	nextSameHash;	/* == 0 */
        int32_t	parent;		/* == 0 */
/*1f8*/	int32_t	extension;		/* FFS: first directory cache block */
/*1fc*/	int32_t	secType;	/* == 1 */
};


struct bEntryBlock {
/*000*/	int32_t	 type;		/* ADF_T_HEADER == 2 */
/*004*/	int32_t	 headerKey;	/* current block number */
        int32_t	 r1[3];
/*014*/	uint32_t checkSum;
/*018*/	int32_t	 hashTable[ADF_HT_SIZE];
        int32_t	 r2[2];
/*140*/	int32_t	 access;	/* bit0=del, 1=modif, 2=write, 3=read */
/*144*/	uint32_t byteSize;
/*148*/	uint8_t	 commLen;
/*149*/	char	 comment[ ADF_MAX_COMMENT_LEN + 1 ];
        char	 r3[ 91 - ( ADF_MAX_COMMENT_LEN + 1 ) ];
/*1a4*/	int32_t	 days;
/*1a8*/	int32_t	 mins;
/*1ac*/	int32_t	 ticks;
/*1b0*/	uint8_t	 nameLen;
/*1b1*/	char	 name[ ADF_MAX_NAME_LEN + 1 ];
        int32_t	 r4;
/*1d4*/	int32_t	 realEntry;
/*1d8*/	int32_t	 nextLink;
        int32_t	 r5[5];
/*1f0*/	int32_t	 nextSameHash;
/*1f4*/	int32_t	 parent;
/*1f8*/	int32_t	 extension;
/*1fc*/	int32_t	 secType;
};


struct bFileHeaderBlock {
/*000*/	int32_t	type;		/* == 2 */
/*004*/	int32_t	headerKey;	/* current block number */
/*008*/	int32_t	highSeq;	/* number of data block in this hdr block */
/*00c*/	int32_t	dataSize;	/* == 0 */
/*010*/	int32_t	firstData;
/*014*/	uint32_t checkSum;
/*018*/	int32_t	dataBlocks[ADF_MAX_DATABLK];
/*138*/	int32_t	r1;
/*13c*/	int32_t	r2;
/*140*/	int32_t	access;	/* bit0=del, 1=modif, 2=write, 3=read */
/*144*/	uint32_t	byteSize;
/*148*/	uint8_t	commLen;
/*149*/	char	comment[ ADF_MAX_COMMENT_LEN + 1 ];
        char	r3[ 91 - ( ADF_MAX_COMMENT_LEN + 1 ) ];
/*1a4*/	int32_t	days;
/*1a8*/	int32_t	mins;
/*1ac*/	int32_t	ticks;
/*1b0*/	uint8_t	nameLen;
/*1b1*/	char	fileName[ADF_MAX_NAME_LEN+1];
        int32_t	r4;
/*1d4*/	int32_t	real;		/* unused == 0 */
/*1d8*/	int32_t	nextLink;	/* link chain */
        int32_t	r5[5];
/*1f0*/	int32_t	nextSameHash;	/* next entry with sane hash */
/*1f4*/	int32_t	parent;		/* parent directory */
/*1f8*/	int32_t	extension;	/* pointer to extension block */
/*1fc*/	int32_t	secType;	/* == -3 */
};


/*--- file header extension block structure ---*/

struct bFileExtBlock {
/*000*/	int32_t	type;		/* == 0x10 */
/*004*/	int32_t	headerKey;
/*008*/	int32_t	highSeq;
/*00c*/	int32_t	dataSize;	/* == 0 */
/*010*/	int32_t	firstData;	/* == 0 */
/*014*/	uint32_t checkSum;
/*018*/	int32_t	dataBlocks[ADF_MAX_DATABLK];
        int32_t	r[45];
        int32_t	info;		/* == 0 */
        int32_t	nextSameHash;	/* == 0 */
/*1f4*/	int32_t	parent;		/* header block */
/*1f8*/	int32_t	extension;	/* next header extension block */
/*1fc*/	int32_t	secType;	/* -3 */	
};



struct bDirBlock {
/*000*/	int32_t	type;		/* == 2 */
/*004*/	int32_t	headerKey;
/*008*/	int32_t	highSeq;	/* == 0 */
/*00c*/	int32_t	hashTableSize;	/* == 0 */
        int32_t	r1;		/* == 0 */
/*014*/	uint32_t checkSum;
/*018*/	int32_t	hashTable[ADF_HT_SIZE];		/* hash table */
        int32_t	r2[2];
/*140*/	int32_t	access;
        int32_t	r4;		/* == 0 */
/*148*/	uint8_t	commLen;
/*149*/	char	comment[ ADF_MAX_COMMENT_LEN + 1 ];
        char	r5[ 91 - ( ADF_MAX_COMMENT_LEN + 1 ) ];
/*1a4*/	int32_t	days;		/* last access */
/*1a8*/	int32_t	mins;
/*1ac*/	int32_t	ticks;
/*1b0*/	uint8_t	nameLen;
/*1b1*/	char 	dirName[ADF_MAX_NAME_LEN+1];
        int32_t	r6;
/*1d4*/	int32_t	real;		/* ==0 */
/*1d8*/	int32_t	nextLink;	/* link list */
        int32_t	r7[5];
/*1f0*/	int32_t	nextSameHash;
/*1f4*/	int32_t	parent;
/*1f8*/	int32_t	extension;		/* FFS : first directory cache */
/*1fc*/	int32_t	secType;	/* == 2 */
};



struct bOFSDataBlock{
/*000*/	int32_t	type;		/* == 8 */
/*004*/	int32_t	headerKey;	/* pointer to file_hdr block */
/*008*/	uint32_t seqNum;	/* file data block number */
/*00c*/	uint32_t dataSize;	/* <= 0x1e8 */
/*010*/	int32_t	nextData;	/* next data block */
/*014*/	uint32_t checkSum;
/*018*/	uint8_t data[488];
/*200*/	};


/* --- bitmap --- */

struct bBitmapBlock {
/*000*/	uint32_t checkSum;
/*004*/	uint32_t map[ADF_BM_MAP_SIZE];
	};


struct bBitmapExtBlock {
/*000*/	int32_t	bmPages[ADF_BM_PAGES_EXT_SIZE];
/*1fc*/	int32_t	nextBlock;
	};


struct bLinkBlock {
/*000*/	int32_t	type;		/* == 2 */
/*004*/	int32_t	headerKey;	/* self pointer */
        int32_t	r1[3];
/*014*/	uint32_t checkSum;
/*018*/	char	realName[64];
        int32_t	r2[83];
/*1a4*/	int32_t	days;		/* last access */
/*1a8*/	int32_t	mins;
/*1ac*/	int32_t	ticks;
/*1b0*/	uint8_t	nameLen;
/*1b1*/	char 	name[ADF_MAX_NAME_LEN+1];
        int32_t	r3;
/*1d4*/	int32_t	realEntry;
/*1d8*/	int32_t	nextLink;
        int32_t	r4[5];
/*1f0*/	int32_t	nextSameHash;
/*1f4*/	int32_t	parent;	
        int32_t	r5;
/*1fc*/	int32_t	secType;	/* == -4, 4, 3 */
	};



/*--- directory cache block structure ---*/

struct bDirCacheBlock {
/*000*/	int32_t	type;		/* == 33 */
/*004*/	int32_t	headerKey;
/*008*/	int32_t	parent;
/*00c*/	int32_t	recordsNb;
/*010*/	int32_t	nextDirC;
/*014*/	uint32_t checkSum;
/*018*/	uint8_t records[488];
	};


#endif /* ADF_BLK_H */
/*##########################################################################*/
