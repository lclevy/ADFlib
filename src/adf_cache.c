/*
 *  ADF Library. (C) 1997-2002 Laurent Clevy
 *
 *  adf_cache.c
 *
 *  $Id$
 *
 *  directory cache code
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

#include "adf_cache.h"

#include "adf_bitm.h"
#include "adf_byteorder.h"
#include "adf_dir.h"
#include "adf_env.h"
#include "adf_raw.h"
#include "adf_util.h"

#include <stdlib.h>
#include <string.h>


/*
freeEntCache(struct AdfCacheEntry *cEntry)
{
    if (cEntry->name!=NULL)
        free(cEntry->name);
    if (cEntry->comm!=NULL)
        free(cEntry->comm);
}
*/

/*
 * adfGetDirEntCache
 *
 * replace 'adfGetDirEnt'. returns a the dir contents based on the dircache list
 */
struct AdfList * adfGetDirEntCache ( struct AdfVolume * const vol,
                                     const SECTNUM            dir,
                                     const BOOL               recurs )
{
    struct AdfEntryBlock parent;
    struct AdfDirCacheBlock dirc;
    int offset, n;
    struct AdfList *cell, *head;
    struct AdfCacheEntry caEntry;
    struct AdfEntry *entry;
    SECTNUM nSect;

    if (adfReadEntryBlock(vol,dir,&parent)!=RC_OK)
        return NULL;

    nSect = parent.extension;

    cell = head = NULL;
    do {
        /* one loop per cache block */
        n = offset = 0;
	    if (adfReadDirCBlock(vol, nSect, &dirc)!=RC_OK)
            return NULL;
        while (n<dirc.recordsNb) {
            /* one loop per record */
            entry = ( struct AdfEntry * ) malloc ( sizeof ( struct AdfEntry ) );
            if (!entry) {
                adfFreeDirList(head);
                return NULL;
            }
            if (adfGetCacheEntry(&dirc, &offset, &caEntry) != RC_OK) {
                free(entry); adfFreeDirList(head);
                return NULL;
            }

            /* converts a cache entry into a dir entry */
            entry->type = (int)caEntry.type;
            entry->name = strdup(caEntry.name);
            if (entry->name==NULL) {
                free(entry); adfFreeDirList(head);
                return NULL;
            }
            entry->sector = (int32_t) caEntry.header;
            entry->comment = strdup(caEntry.comm);
            if (entry->comment==NULL) {
                free(entry->name); adfFreeDirList(head);
                return NULL;
            }
            entry->size = (uint32_t) caEntry.size;
            entry->access = (int32_t) caEntry.protect;
            adfDays2Date( caEntry.days, &(entry->year), &(entry->month),
                &(entry->days) );
            entry->hour = caEntry.mins/60;
            entry->mins = caEntry.mins%60;
            entry->secs = caEntry.ticks/50;

            /* add it into the linked list */
            if (head==NULL)
                head = cell = newCell(NULL, (void*)entry);
            else
                cell = newCell(cell, (void*)entry);

            if (cell==NULL) {
                adfFreeEntry(entry);
                adfFreeDirList(head);
                return NULL;
            }

            if ( recurs && entry->type == ADF_ST_DIR )
                 cell->subdir = adfGetDirEntCache(vol,entry->sector,recurs);

            n++;
        }
        nSect = dirc.nextDirC;
    }while (nSect!=0);

    return head;
}



/*
 * adfGetCacheEntry
 *
 * Returns a cache entry, starting from the offset p (the index into records[])
 * This offset is updated to the end of the returned entry.
 */
RETCODE adfGetCacheEntry ( const struct AdfDirCacheBlock * const dirc,
                           int * const                           p,
                           struct AdfCacheEntry * const          cEntry )
{
    int ptr;

    ptr = *p;
    if ( ptr > ADF_LOGICAL_BLOCK_SIZE - 26 )  /* minimum cache entry length */
        return RC_ERROR;

/*printf("p=%d\n",ptr);*/

#ifdef LITT_ENDIAN
    cEntry->header = swapLong(dirc->records+ptr);
    cEntry->size = swapLong(dirc->records+ptr+4);
    cEntry->protect = swapLong(dirc->records+ptr+8);
    cEntry->days = swapShort(dirc->records+ptr+16);
    cEntry->mins = swapShort(dirc->records+ptr+18);
    cEntry->ticks = swapShort(dirc->records+ptr+20);
#else
    cEntry->header = Long(dirc->records+ptr);
    cEntry->size = Long(dirc->records+ptr+4);
    cEntry->protect = Long(dirc->records+ptr+8);
    cEntry->days = Short(dirc->records+ptr+16);
    cEntry->mins = Short(dirc->records+ptr+18);
    cEntry->ticks = Short(dirc->records+ptr+20);
#endif
    cEntry->type =(signed char) dirc->records[ptr+22];

    cEntry->nLen = dirc->records[ptr+23];
/*    cEntry->name = (char*)malloc(sizeof(char)*(cEntry->nLen+1));
    if (!cEntry->name)
         return;
*/
    if (cEntry->nLen < 1 || cEntry->nLen > ADF_MAX_NAME_LEN) return RC_ERROR;
    if ( ( ptr + 24 + cEntry->nLen ) > ADF_LOGICAL_BLOCK_SIZE )
        return RC_ERROR;
    memcpy(cEntry->name, dirc->records+ptr+24, cEntry->nLen);
    cEntry->name[(int)(cEntry->nLen)]='\0';

    cEntry->cLen = dirc->records[ptr+24+cEntry->nLen];
    if ( cEntry->cLen > ADF_MAX_COMMENT_LEN )
        return RC_ERROR;
    if ( ptr + 24 + cEntry->nLen + 1 + cEntry->cLen > ADF_LOGICAL_BLOCK_SIZE )
        return RC_ERROR;
    if (cEntry->cLen>0) {
/*        cEntry->comm =(char*)malloc(sizeof(char)*(cEntry->cLen+1));
        if (!cEntry->comm) {
            free( cEntry->name ); cEntry->name=NULL;
            return;
        }
*/        memcpy(cEntry->comm,dirc->records+ptr+24+cEntry->nLen+1,cEntry->cLen);
    }
        cEntry->comm[(int)(cEntry->cLen)]='\0';
/*printf("cEntry->nLen %d cEntry->cLen %d %s\n",cEntry->nLen,cEntry->cLen,cEntry->name);*/
    *p  = ptr+24+cEntry->nLen+1+cEntry->cLen;

    /* the starting offset of each record must be even (68000 constraint) */
    if ((*p%2)!=0)
        *p=(*p)+1;

    return RC_OK;
}


/*
 * adfPutCacheEntry
 *
 * remplaces one cache entry at the p offset, and returns its length
 */
int adfPutCacheEntry ( struct AdfDirCacheBlock * const    dirc,
                       const int * const                  p,
                       const struct AdfCacheEntry * const cEntry )
{
    int ptr, l;

    ptr = *p;

#ifdef LITT_ENDIAN
    swLong(dirc->records+ptr, cEntry->header);
    swLong(dirc->records+ptr+4, cEntry->size);
    swLong(dirc->records+ptr+8, cEntry->protect);
    swShort(dirc->records+ptr+16, cEntry->days);
    swShort(dirc->records+ptr+18, cEntry->mins);
    swShort(dirc->records+ptr+20, cEntry->ticks);
#else
    memcpy(dirc->records+ptr,&(cEntry->header),4);
    memcpy(dirc->records+ptr+4,&(cEntry->size),4);
    memcpy(dirc->records+ptr+8,&(cEntry->protect),4);
    memcpy(dirc->records+ptr+16,&(cEntry->days),2);
    memcpy(dirc->records+ptr+18,&(cEntry->mins),2);
    memcpy(dirc->records+ptr+20,&(cEntry->ticks),2);
#endif
    dirc->records[ptr+22] =(uint8_t)cEntry->type;

    dirc->records[ptr+23] = cEntry->nLen;
    memcpy(dirc->records+ptr+24, cEntry->name, cEntry->nLen);

    dirc->records[ptr+24+cEntry->nLen] = cEntry->cLen;
    memcpy(dirc->records+ptr+24+cEntry->nLen+1, cEntry->comm, cEntry->cLen);

/*puts("adfPutCacheEntry");*/

    l = 25+cEntry->nLen+cEntry->cLen;
    if ((l%2)==0)
        return l;
    else {
        dirc->records[ptr+l] =(char)0;
        return l+1;
    }

    /* ptr%2 must be == 0, if l%2==0, (ptr+l)%2==0 */
}


/*
 * adfEntry2CacheEntry
 *
 * converts one dir entry into a cache entry, and return its future length in records[]
 */
int adfEntry2CacheEntry ( const struct AdfEntryBlock * const entry,
                          struct AdfCacheEntry * const       newEntry )
{
    int entryLen;

    /* new entry */
    newEntry->header = (uint32_t) entry->headerKey;
    if ( entry->secType == ADF_ST_FILE )
        newEntry->size = entry->byteSize;
    else
        newEntry->size = 0L;
    newEntry->protect = (uint32_t) entry->access;
    newEntry->days    = (uint16_t) entry->days;
    newEntry->mins    = (uint16_t) entry->mins;
    newEntry->ticks   = (uint16_t) entry->ticks;
    newEntry->type = (signed char)entry->secType;
    newEntry->nLen = entry->nameLen;
    memcpy(newEntry->name, entry->name, newEntry->nLen);
    newEntry->name[(int)(newEntry->nLen)] = '\0';
    newEntry->cLen = entry->commLen;
    if (newEntry->cLen>0)
        memcpy(newEntry->comm, entry->comment, newEntry->cLen);

    entryLen = 24+newEntry->nLen+1+newEntry->cLen;

/*printf("entry->name %d entry->comment %d\n",entry->nameLen,entry->commLen);
printf("newEntry->nLen %d newEntry->cLen %d\n",newEntry->nLen,newEntry->cLen);
*/    if ((entryLen%2)==0)
        return entryLen;
    else
        return entryLen+1;
}


/*
 * adfDelFromCache
 *
 * delete one cache entry from its block. don't do 'records garbage collecting'
 */
RETCODE adfDelFromCache ( struct AdfVolume * const           vol,
                          const struct AdfEntryBlock * const parent,
                          const SECTNUM                      headerKey )
{
    struct AdfDirCacheBlock dirc;
    SECTNUM nSect, prevSect;
    struct AdfCacheEntry caEntry;
    int offset, oldOffset, n;
    BOOL found;
    int entryLen;
    int i;
    RETCODE rc = RC_OK;

    prevSect = -1;
	nSect = parent->extension;
    found = FALSE;
    do {
        rc = adfReadDirCBlock ( vol, nSect, &dirc );
        if ( rc != RC_OK )
            return rc;

        offset = 0; n = 0;
        while(n < dirc.recordsNb && !found) {
            oldOffset = offset;

            rc = adfGetCacheEntry ( &dirc, &offset, &caEntry );
            if ( rc != RC_OK)
                return rc;

            found = ( caEntry.header == (uint32_t) headerKey );
            if (found) {
                entryLen = offset-oldOffset;
                if (dirc.recordsNb>1 || prevSect==-1) {
                    if (n<dirc.recordsNb-1) {
                        /* not the last of the block : switch the following records */
                        for(i=oldOffset; i<(488-entryLen); i++)
                            dirc.records[i] = dirc.records[i+entryLen];
                        /* and clear the following bytes */
                        for(i=488-entryLen; i<488; i++)
                            dirc.records[i] = 0;
                    }
                    else {
                        /* the last record of this cache block */
                        for(i=oldOffset; i<offset; i++)
                            dirc.records[i] = 0;
                    }
                    dirc.recordsNb--;

                    rc = adfWriteDirCBlock ( vol, dirc.headerKey, &dirc );
                    if ( rc != RC_OK )
                        return rc;
                }
                else {
                    /* dirc.recordsNb ==1 or == 0 , prevSect!=-1 :
                    * the only record in this dirc block and a previous dirc block exists
                    */
                    adfSetBlockFree(vol, dirc.headerKey);

                    rc = adfReadDirCBlock ( vol, prevSect, &dirc );
                    if ( rc != RC_OK )
                        return rc;

                    dirc.nextDirC = 0L;

                    rc = adfWriteDirCBlock ( vol, prevSect, &dirc );
                    if ( rc != RC_OK )
                        return rc;

                    rc = adfUpdateBitmap ( vol );
                    if ( rc != RC_OK )
                        return rc;
                }
            }
            n++;
        }
        prevSect = nSect;
        nSect = dirc.nextDirC;
    }while(nSect!=0 && !found);

    if (!found)
        (*adfEnv.wFct)("adfUpdateCache : entry not found");

    return rc;
}


/*
 * adfAddInCache
 *
 */
RETCODE adfAddInCache ( struct AdfVolume * const           vol,
                        const struct AdfEntryBlock * const parent,
                        const struct AdfEntryBlock * const entry )
{
    struct AdfDirCacheBlock dirc, newDirc;
    SECTNUM nSect, nCache;
    struct AdfCacheEntry caEntry, newEntry;
    int offset, n;
    int entryLen;
    RETCODE rc = RC_OK;

    entryLen = adfEntry2CacheEntry(entry, &newEntry);
/*printf("adfAddInCache--%4ld %2d %6ld %8lx %4d %2d:%02d:%02d %30s %22s\n",
    newEntry.header, newEntry.type, newEntry.size, newEntry.protect,
    newEntry.days, newEntry.mins/60, newEntry.mins%60,
	newEntry.ticks/50,
	newEntry.name, newEntry.comm);
*/
    nSect = parent->extension;
    do {
        rc = adfReadDirCBlock ( vol, nSect, &dirc );
        if ( rc != RC_OK )
            return rc;

        offset = 0; n = 0;
/*printf("parent=%4ld\n",dirc.parent);*/
        while(n < dirc.recordsNb) {
            rc = adfGetCacheEntry ( &dirc, &offset, &caEntry );
            if ( rc != RC_OK )
                return rc;

/*printf("*%4ld %2d %6ld %8lx %4d %2d:%02d:%02d %30s %22s\n",
    caEntry.header, caEntry.type, caEntry.size, caEntry.protect,
    caEntry.days, caEntry.mins/60, caEntry.mins%60,
	caEntry.ticks/50,
	caEntry.name, caEntry.comm);
*/
            n++;
        }

/*        if (offset+entryLen<=488) {
            adfPutCacheEntry(&dirc, &offset, &newEntry);
            dirc.recordsNb++;
            adfWriteDirCBlock(vol, dirc.headerKey, &dirc);
            return rc;
        }*/
        nSect = dirc.nextDirC;
    }while(nSect!=0);

    /* in the last block */
    if (offset+entryLen<=488) {
        adfPutCacheEntry(&dirc, &offset, &newEntry);
        dirc.recordsNb++;
/*printf("entry name=%s\n",newEntry.name);*/
    }
    else {
        /* request one new block free */
        nCache = adfGet1FreeBlock(vol);
        if (nCache==-1) {
           (*adfEnv.wFct)("adfCreateDir : nCache==-1");
           return RC_VOLFULL;
        }

        /* create a new dircache block */
        memset(&newDirc,0,512);
        if ( parent->secType == ADF_ST_ROOT )
            newDirc.parent = vol->rootBlock;
        else if ( parent->secType == ADF_ST_DIR )
            newDirc.parent = parent->headerKey;
        else
            (*adfEnv.wFct)("adfAddInCache : unknown secType");
        newDirc.recordsNb = 0L;
        newDirc.nextDirC = 0L;

        adfPutCacheEntry(&dirc, &offset, &newEntry);
        newDirc.recordsNb++;

        rc = adfWriteDirCBlock ( vol, nCache, &newDirc );
        if ( rc != RC_OK )
            return rc;

        dirc.nextDirC = nCache;
    }
/*printf("dirc.headerKey=%ld\n",dirc.headerKey);*/

/*if (strcmp(entry->name,"file_5u")==0)
dumpBlock(&dirc);
*/
    return adfWriteDirCBlock ( vol, dirc.headerKey, &dirc );
}


/*
 * adfUpdateCache
 *
 */
RETCODE adfUpdateCache ( struct AdfVolume * const           vol,
                         const struct AdfEntryBlock * const parent,
                         const struct AdfEntryBlock * const entry,
                         const BOOL                         entryLenChg )
{
    struct AdfDirCacheBlock dirc;
    SECTNUM nSect;
    struct AdfCacheEntry caEntry, newEntry;
    int offset, oldOffset, n;
    BOOL found;
    int i, oLen, nLen;
    int sLen; /* shift length */
    RETCODE rc = RC_OK;

    nLen = adfEntry2CacheEntry(entry, &newEntry);

    nSect = parent->extension;
    found = FALSE;
    do {
/*printf("dirc=%ld\n",nSect);*/
        rc = adfReadDirCBlock ( vol, nSect, &dirc );
        if ( rc != RC_OK )
            return rc;

        offset = 0; n = 0;
        /* search entry to update with its header_key */
        while(n < dirc.recordsNb && !found) {
            oldOffset = offset;
            /* offset is updated */
            rc = adfGetCacheEntry ( &dirc, &offset, &caEntry );
            if ( rc != RC_OK )
                return rc;
            oLen = offset-oldOffset;
            sLen = oLen-nLen;
/*printf("olen=%d nlen=%d\n",oLen,nLen);*/
            found = (caEntry.header==newEntry.header);
            if (found) {
                if (!entryLenChg || oLen==nLen) {
                    /* same length : remplace the old values */
                    adfPutCacheEntry(&dirc, &oldOffset, &newEntry);
/*if (entryLenChg) puts("oLen==nLen");*/
                    rc = adfWriteDirCBlock ( vol, dirc.headerKey, &dirc );
                    if ( rc != RC_OK )
                        return rc;
                }
                else if (oLen>nLen) {
/*puts("oLen>nLen");*/
                    /* the new record is shorter, write it,
                     * then shift down the following records
                     */
                    adfPutCacheEntry(&dirc, &oldOffset, &newEntry);
                    for(i=oldOffset+nLen; i<(488-sLen); i++)
                        dirc.records[i] = dirc.records[i+sLen];
                    /* then clear the following bytes */
                    for(i=488-sLen; i<488; i++)
                        dirc.records[i] = (char)0;

                    rc = adfWriteDirCBlock ( vol, dirc.headerKey, &dirc );
                    if ( rc != RC_OK )
                        return rc;
                }
                else {
                    /* the new record is larger */
/*puts("oLen<nLen");*/
                    rc = adfDelFromCache ( vol, parent, entry->headerKey );
                    if ( rc != RC_OK )
                        return rc;

                    rc = adfAddInCache ( vol, parent, entry );
                    if ( rc != RC_OK )
                        return rc;
/*puts("oLen<nLen end");*/

                }
            }
            n++;
        }
        nSect = dirc.nextDirC;
    }while(nSect!=0 && !found);

    if (found) {
        return adfUpdateBitmap ( vol );
    }
    else
        (*adfEnv.wFct)("adfUpdateCache : entry not found");

    return RC_OK;
}


/*
 * adfCreateEmptyCache
 *
 */
RETCODE adfCreateEmptyCache ( struct AdfVolume * const     vol,
                              struct AdfEntryBlock * const parent,
                              const SECTNUM                nSect )
{
    SECTNUM nCache;

    if (nSect==-1) {
        nCache = adfGet1FreeBlock(vol);
        if (nCache==-1) {
           (*adfEnv.wFct)("adfCreateDir : nCache==-1");
           return RC_VOLFULL;
        }
    }
    else
        nCache = nSect;

    if (parent->extension==0)
		parent->extension = nCache;

    struct AdfDirCacheBlock dirc;
    memset ( &dirc, 0, sizeof(struct AdfDirCacheBlock) );

    if ( parent->secType == ADF_ST_ROOT )
        dirc.parent = vol->rootBlock;
    else if ( parent->secType == ADF_ST_DIR )
        dirc.parent = parent->headerKey;
    else {
        (*adfEnv.wFct)("adfCreateEmptyCache : unknown secType");
/*printf("secType=%ld\n",parent->secType);*/
    }

    dirc.recordsNb = 0;
    dirc.nextDirC = 0;

    return adfWriteDirCBlock ( vol, nCache, &dirc );
}


/*
 * adfReadDirCBlock
 *
 */
RETCODE adfReadDirCBlock ( struct AdfVolume * const        vol,
                           const SECTNUM                   nSect,
                           struct AdfDirCacheBlock * const dirc )
{
    uint8_t buf[512];

    RETCODE rc = adfVolReadBlock ( vol, (uint32_t) nSect, buf );
    if ( rc != RC_OK )
        return rc;

    memcpy(dirc,buf,512);
#ifdef LITT_ENDIAN
    adfSwapEndian ( (uint8_t *) dirc, ADF_SWBL_CACHE );
#endif
    if (dirc->checkSum!=adfNormalSum(buf,20,512))
        adfEnv.wFct ( "adfReadDirCBlock : invalid checksum, volume '%s', block %u",
                      vol->volName, nSect );
    if ( dirc->type != ADF_T_DIRC )
        adfEnv.wFct ( "adfReadDirCBlock : ADF_T_DIRC not found, volume '%s', block %u",
                      vol->volName, nSect );
    if (dirc->headerKey!=nSect)
        adfEnv.wFct ( "adfReadDirCBlock : headerKey (%u) != nSect (%u), volume '%s', block %u",
            dirc->headerKey, nSect, vol->volName, nSect );

    return RC_OK;
}


/*
 * adfWriteDirCblock
 *
 */
RETCODE adfWriteDirCBlock ( struct AdfVolume * const        vol,
                            const int32_t                   nSect,
                            struct AdfDirCacheBlock * const dirc )
{
    uint8_t buf[ADF_LOGICAL_BLOCK_SIZE];
    uint32_t newSum;

    dirc->type = ADF_T_DIRC;
    dirc->headerKey = nSect;

    memcpy(buf, dirc, ADF_LOGICAL_BLOCK_SIZE);
#ifdef LITT_ENDIAN
    adfSwapEndian ( buf, ADF_SWBL_CACHE );
#endif

    newSum = adfNormalSum(buf, 20, ADF_LOGICAL_BLOCK_SIZE);
    swLong(buf+20,newSum);
/*    *(int32_t*)(buf+20) = swapLong((uint8_t*)&newSum);*/

/*puts("adfWriteDirCBlock");*/
    return adfVolWriteBlock ( vol, (uint32_t) nSect, buf );
}

/*################################################################################*/
