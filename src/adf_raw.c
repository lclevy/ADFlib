/*
 *  ADF Library. (C) 1997-2002 Laurent Clevy
 *
 *  adf_raw.c
 *
 *  $Id$
 *
 * logical disk/volume code
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

#include "adf_raw.h"

#include "adf_byteorder.h"
#include "adf_env.h"
#include "adf_util.h"

#include <string.h>

#ifndef NDEBUG
#define NDEBUG
#endif
#include <assert.h>

#define SW_LONG  4
#define SW_SHORT 2
#define SW_CHAR  1

#define MAX_SWTYPE 11

static const int swapTable [ MAX_SWTYPE + 1 ][ 15 ] = {
    { 4, SW_CHAR, 2, SW_LONG, 1012, SW_CHAR, 0, 1024 },     /* first bytes of boot */
    { 108, SW_LONG, 40, SW_CHAR, 10, SW_LONG, 0, 512 },        /* root */
    { 6, SW_LONG, 488, SW_CHAR, 0, 512 },                      /* data */
                                                            /* file, dir, entry */
    { 82, SW_LONG, 92, SW_CHAR, 3, SW_LONG, 36, SW_CHAR, 11, SW_LONG, 0, 512 },
    { 6, SW_LONG, 0, 24 },                                       /* cache */
    { 128, SW_LONG, 0, 512 },                                /* bitmap, fext */
		                                                    /* link */                                        
    { 6, SW_LONG, 64, SW_CHAR, 86, SW_LONG, 32, SW_CHAR, 12, SW_LONG, 0, 512 },
    { 4, SW_CHAR, 39, SW_LONG, 56, SW_CHAR, 10, SW_LONG, 0, 256 }, /* RDSK */
    { 4, SW_CHAR, 127, SW_LONG, 0, 512 },                          /* BADB */
    { 4, SW_CHAR, 8, SW_LONG, 32, SW_CHAR, 31, SW_LONG, 4, SW_CHAR, /* PART */
      15, SW_LONG, 0, 256 },
    { 4, SW_CHAR, 7, SW_LONG, 4, SW_CHAR, 55, SW_LONG, 0, 256 }, /* FSHD */
    { 4, SW_CHAR, 4, SW_LONG, 492, SW_CHAR, 0, 512 }             /* LSEG */
    };


/*
 * adfSwapEndian
 *
 * magic :-) endian swap function (big -> little for read, little to big for write)
 */

void adfSwapEndian ( uint8_t * const buf,
                     const int       type )
{
    int i = 0,
        p = 0;

    assert ( type >= 0 );
    assert ( type <= MAX_SWTYPE );
    if ( type > MAX_SWTYPE || type < 0 ) {
        /* this should never happen */
        adfEnv.eFct ( "adfSwapEndian: type %d do not exist", type );
        return;
    }

    while ( swapTable[type][i] != 0 ) {
        for ( int j = 0 ; j < swapTable[type][i] ; j++ )  {
            switch ( swapTable[type][i + 1] ) {
            case SW_LONG:
                *(uint32_t*)(buf + p) = Long(buf + p);
                p += 4;
                break;
            case SW_SHORT:
                *(uint32_t*)(buf + p) = Short(buf + p);
                p += 2;
                break;
            case SW_CHAR:
                p++;
                break;
            default:
                ;
            }
        }
        i += 2;
    }
    if ( p != swapTable[type][i + 1] )
        (*adfEnv.wFct)("Warning: Endian Swapping length");		/* BV */
}





/*
 * adfReadRootBlock
 *
 * ENDIAN DEPENDENT
 */
ADF_RETCODE adfReadRootBlock ( struct AdfVolume * const    vol,
                               const uint32_t              nSect,
                               struct AdfRootBlock * const root )
{
    uint8_t buf[ADF_LOGICAL_BLOCK_SIZE];

    ADF_RETCODE rc = adfVolReadBlock ( vol, nSect, buf );
    if ( rc != RC_OK )
        return rc;

    memcpy ( root, buf, ADF_LOGICAL_BLOCK_SIZE );
#ifdef LITT_ENDIAN
    adfSwapEndian ( (uint8_t*) root, ADF_SWBL_ROOT );
#endif

    if ( root->type != ADF_T_HEADER ||
         root->secType != ADF_ST_ROOT )
    {
        (*adfEnv.wFct)("adfReadRootBlock : id not found");
        return RC_BLOCKTYPE;
    }

    const uint32_t checksumCalculated = adfNormalSum ( buf, 0x14, ADF_LOGICAL_BLOCK_SIZE );
    if ( root->checkSum != checksumCalculated ) {
        const char msg[] = "adfReadRootBlock : invalid checksum 0x%x != 0x%x (calculated)"
            ", block %d, volume '%s'";
        if ( adfEnv.ignoreChecksumErrors ) {
            adfEnv.wFct ( msg, root->checkSum, checksumCalculated, nSect, vol->volName );
        } else {
            adfEnv.eFct ( msg, root->checkSum, checksumCalculated, nSect, vol->volName );
            return RC_BLOCKSUM;
        }
    }
		
    return RC_OK;
}


/*
 * adfWriteRootBlock
 *
 * 
 */
ADF_RETCODE adfWriteRootBlock ( struct AdfVolume * const    vol,
                                const uint32_t              nSect,
                                struct AdfRootBlock * const root )
{
/*printf("adfWriteRootBlock %ld\n",nSect);*/
    uint8_t buf[ADF_LOGICAL_BLOCK_SIZE];

    root->type = ADF_T_HEADER;
    root->headerKey = 0L;
    root->highSeq = 0L;
    root->hashTableSize = ADF_HT_SIZE;
    root->firstData = 0L;
    /* checkSum, hashTable */
    /* bmflag */
    /* bmPages, bmExt */
    root->nextSameHash = 0L;
    root->parent = 0L;
    root->secType = ADF_ST_ROOT;

    memcpy(buf, root, ADF_LOGICAL_BLOCK_SIZE);

#ifdef LITT_ENDIAN
    adfSwapEndian ( buf, ADF_SWBL_ROOT );
#endif
    uint32_t newSum = adfNormalSum ( buf, 20, ADF_LOGICAL_BLOCK_SIZE );
    swLong(buf+20, newSum);
/*	*(uint32_t*)(buf+20) = swapLong((uint8_t*)&newSum);*/
/* 	dumpBlock(buf);*/
    return adfVolWriteBlock ( vol, nSect, buf );
}


/*
 * adfReadBootBlock
 *
 * ENDIAN DEPENDENT
 */
ADF_RETCODE adfReadBootBlock ( struct AdfVolume * const    vol,
                               struct AdfBootBlock * const boot )
{
    uint8_t buf[1024];
	
/*puts("22");*/
    ADF_RETCODE rc = adfVolReadBlock ( vol, 0, buf );
    if ( rc != RC_OK )
        return rc;
/*puts("11");*/
    rc = adfVolReadBlock ( vol, 1, buf + ADF_LOGICAL_BLOCK_SIZE );
    if ( rc != RC_OK )
        return rc;

    memcpy ( boot, buf, ADF_LOGICAL_BLOCK_SIZE * 2 );
#ifdef LITT_ENDIAN
    adfSwapEndian ( (uint8_t *) boot, ADF_SWBL_BOOT );
#endif
    if ( strncmp ( "DOS", boot->dosType, 3 ) != 0 ) {
        if ( strncmp ( "PFS", boot->dosType, 3 ) == 0 ) {
            adfEnv.wFct("adfReadBootBlock : PFS volume found - not supported...");
            return RC_ERROR;
        }
        adfEnv.wFct("adfReadBootBlock : DOS id not found");
        return RC_ERROR;
    }


    if ( boot->data[0] != 0 ) {
        const uint32_t checksumCalculated = adfBootSum ( buf );
/*printf("compsum=%lx sum=%lx\n",	adfBootSum(buf),boot->checkSum );*/		/* BV */
        if ( boot->checkSum != checksumCalculated ) {
            const char msg[] = "adfReadBootBlock : invalid checksum 0x%x != 0x%x (calculated)"
                ", block %d, volume '%s'";
            if ( adfEnv.ignoreChecksumErrors ) {
                adfEnv.wFct ( msg, boot->checkSum, checksumCalculated, 0, vol->volName );
            } else {
                adfEnv.eFct ( msg, boot->checkSum, checksumCalculated, 0, vol->volName );
                return RC_BLOCKSUM;
            }
        }
    }

    return RC_OK;
}

/*
 * adfWriteBootBlock
 *
 *
 *     write bootcode ?
 */
ADF_RETCODE adfWriteBootBlock ( struct AdfVolume * const    vol,
                                struct AdfBootBlock * const boot )
{
    uint8_t buf[ ADF_LOGICAL_BLOCK_SIZE * 2 ];

    boot->dosType[0] = 'D';
    boot->dosType[1] = 'O';
    boot->dosType[2] = 'S';
    memcpy ( buf, boot, ADF_LOGICAL_BLOCK_SIZE * 2 );
#ifdef LITT_ENDIAN
    adfSwapEndian ( buf, ADF_SWBL_BOOT );
#endif

    if (boot->rootBlock==880 || boot->data[0]!=0) {
        uint32_t newSum = adfBootSum ( buf );
/*fprintf(stderr,"sum %x %x\n",newSum,adfBootSum2(buf));*/
        swLong(buf+4,newSum);
/*        *(uint32_t*)(buf+4) = swapLong((uint8_t*)&newSum);*/
    }

/*	dumpBlock(buf);
	dumpBlock(buf+512);
*/

    ADF_RETCODE rc = adfVolWriteBlock ( vol, 0, buf );
    if ( rc != RC_OK )
        return rc;

    rc = adfVolWriteBlock ( vol, 1, buf + 512 );
    if (rc != RC_OK )
        return rc;

/*puts("adfWriteBootBlock");*/
    return RC_OK;
}


/*
 * NormalSum
 *
 * buf = where the block is stored
 * offset = checksum place (in bytes)
 * bufLen = buffer length (in bytes)
 */
uint32_t adfNormalSum ( const uint8_t * const buf,
                        const int             offset,
                        const int             bufLen )
{
    uint32_t newsum;
    int i;

    newsum=0L;
    for(i=0; i < (bufLen/4); i++)
        if ( i != (offset/4) )       /* old chksum */
            newsum+=Long(buf+i*4);
    newsum = (uint32_t) ( - (int32_t) newsum );	/* WARNING */

    return(newsum);
}

/*
 * adfBitmapSum
 *
 */
uint32_t adfBitmapSum ( const uint8_t * const buf )
{
	uint32_t newSum;
	int i;
	
	newSum = 0L;
	for(i=1; i<128; i++)
		newSum-=Long(buf+i*4);
	return(newSum);
}


/*
 * adfBootSum
 *
 */
uint32_t adfBootSum ( const uint8_t * const buf )
{
    uint32_t d, newSum;
    int i;
	
    newSum=0L;
    for(i=0; i<256; i++) {
        if (i!=1) {
            d = Long(buf+i*4);
            if ( (0xffffffffU-newSum)<d )
                newSum++;
            newSum+=d;
        }
    }
    newSum = ~newSum;	/* not */

    return(newSum);
}

uint32_t adfBootSum2 ( const uint8_t * const buf )
{
    uint32_t prevsum, newSum;

    newSum = 0L;
    for ( unsigned i = 0; i < 1024 / sizeof(uint32_t) ; i++ ) {
        if (i!=1) {
            prevsum = newSum;
            newSum += Long(buf+i*4);
            if (newSum < prevsum)
                newSum++;
        }
    }
    newSum = ~newSum;	/* not */

    return(newSum);
}


/*#######################################################################################*/
