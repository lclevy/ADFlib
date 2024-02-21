/*
 *  ADF Library. (C) 1997-2002 Laurent Clevy
 *
 *  adf_salv.c
 *
 *  $Id$
 *
 * undelete and salvage code : EXPERIMENTAL !
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


#include "adf_salv.h"

#include "adf_bitm.h"
#include "adf_cache.h"
#include "adf_dir.h"
#include "adf_env.h"
#include "adf_file_block.h"
#include "adf_util.h"

#include <string.h>
#include <stdlib.h>


/*
 * adfFreeGenBlock
 *
 */
static void adfFreeGenBlock ( struct GenBlock * const block )
{
    if ( block != NULL ) {
        if ( block->name != NULL )
            free ( block->name );
        free ( block );
    }
}


/*
 * adfFreeDelList
 *
 */
void adfFreeDelList ( struct AdfList * const list )
{
    struct AdfList *cell;

    cell = list;
    while(cell!=NULL) {
        adfFreeGenBlock((struct GenBlock*)cell->content);
        cell = cell->next;
    }
    freeList(list);
}


/*
 * adfGetDelEnt
 *
 */
struct AdfList * adfGetDelEnt ( struct AdfVolume * const vol )
{
    struct GenBlock *block;
    int32_t i;
    struct AdfList *list, *head;
    BOOL delEnt;

    list = head = NULL;
    block = NULL;
    delEnt = TRUE;
    for(i=vol->firstBlock + 2 ; i<=vol->lastBlock; i++) {
        if (adfIsBlockFree(vol, i)) {
            if (delEnt) {
                block = (struct GenBlock*)malloc(sizeof(struct GenBlock));
                if ( block == NULL ) {
                    adfFreeDelList ( head );
                    return NULL;
                }
/*printf("%p\n",block);*/
            }

            if ( adfReadGenBlock ( vol, i, block ) != RC_OK ) {
                adfFreeDelList ( head );
                return NULL;
            }

            delEnt = (block->type==T_HEADER 
                && (block->secType==ST_DIR || block->secType==ST_FILE) );

            if (delEnt) {
                if (head==NULL)
                    list = head = newCell(NULL, (void*)block);
                else
                    list = newCell(list, (void*)block);
            }
        }
    }

    if (block!=NULL && list!=NULL && block!=list->content) {
        free(block);
/*        printf("%p\n",block);*/
    }
    return head;
}


/*
 * adfReadGenBlock
 *
 */
RETCODE adfReadGenBlock ( struct AdfVolume * const vol,
                          const SECTNUM            nSect,
                          struct GenBlock * const  block )
{
    uint8_t buf[LOGICAL_BLOCK_SIZE];
    unsigned len;
    char name[MAXNAMELEN+1];

    RETCODE rc = adfVolReadBlock ( vol, (unsigned) nSect, buf );
    if ( rc != RC_OK )
        return rc;

    block->type =(int) swapLong(buf);
    block->secType =(int) swapLong(buf+vol->blockSize-4);
    block->sect = nSect;
    block->name = NULL;

    if (block->type==T_HEADER) {
        switch(block->secType) {
        case ST_FILE:
        case ST_DIR:
        case ST_LFILE:
        case ST_LDIR:
            len = min( (unsigned) MAXNAMELEN, buf [ vol->blockSize - 80 ] );
            strncpy(name, (char*)buf+vol->blockSize-79, len);
            name[len] = '\0';
            block->name = strdup(name);
            block->parent = (int32_t) swapLong ( buf + vol->blockSize - 12 );
            break;
        case ST_ROOT:
            break;
        default: 
            ;
        }
    }
    return RC_OK;
}


/*
 * adfCheckParent
 *
 */
RETCODE adfCheckParent ( struct AdfVolume * vol,
                         SECTNUM            pSect )
{
    struct GenBlock block;

    if (adfIsBlockFree(vol, pSect)) {
        (*adfEnv.wFct)("adfCheckParent : parent doesn't exists");
        return RC_ERROR;
    }

    /* verify if parent is a DIR or ROOT */
    RETCODE rc = adfReadGenBlock ( vol, pSect, &block );
    if ( rc != RC_OK )
        return rc;

    if ( block.type!=T_HEADER 
        || (block.secType!=ST_DIR && block.secType!=ST_ROOT) ) {
        (*adfEnv.wFct)("adfCheckParent : parent secType is incorrect");
        return RC_ERROR;
    }

    return RC_OK;
}


/*
 * adfUndelDir
 *
 */
RETCODE adfUndelDir ( struct AdfVolume * vol,
                      SECTNUM            pSect,
                      SECTNUM            nSect,
                      struct bDirBlock * entry )
{
    (void) nSect;
    RETCODE rc;
    struct bEntryBlock parent;
    char name[MAXNAMELEN+1];

    /* check if the given parent sector pointer seems OK */
    rc = adfCheckParent ( vol, pSect );
    if ( rc != RC_OK)
        return rc;

    if (pSect!=entry->parent) {
        (*adfEnv.wFct)("adfUndelDir : the given parent sector isn't the entry parent");
        return RC_ERROR;
    }

    if (!adfIsBlockFree(vol, entry->headerKey))
        return RC_ERROR;
    if ( isDIRCACHE ( vol->fs.type ) &&
         ! adfIsBlockFree ( vol, entry->extension ) )
    {
        return RC_ERROR;
    }

    rc = adfReadEntryBlock ( vol, pSect, &parent );
    if ( rc != RC_OK )
        return rc;

    strncpy(name, entry->dirName, entry->nameLen);
    name[(int)entry->nameLen] = '\0';
    /* insert the entry in the parent hashTable, with the headerKey sector pointer */
    adfSetBlockUsed(vol,entry->headerKey);
    if ( adfCreateEntry ( vol, &parent, name, entry->headerKey ) == -1 )
        return RC_ERROR;

    if ( isDIRCACHE ( vol->fs.type ) ) {
        rc = adfAddInCache ( vol, &parent, (struct bEntryBlock *) entry );
        if ( rc != RC_OK )
            return rc;

        adfSetBlockUsed(vol,entry->extension);
    }

    return adfUpdateBitmap ( vol );
}


/*
 * adfUndelFile
 *
 */
RETCODE adfUndelFile ( struct AdfVolume *        vol,
                       SECTNUM                   pSect,
                       SECTNUM                   nSect,
                       struct bFileHeaderBlock * entry )
{
    (void) nSect;
    int32_t i;
    char name[MAXNAMELEN+1];
    struct bEntryBlock parent;
    RETCODE rc;
    struct AdfFileBlocks fileBlocks;

    /* check if the given parent sector pointer seems OK */
    rc = adfCheckParent ( vol, pSect );
    if ( rc != RC_OK )
        return rc;

    if (pSect!=entry->parent) {
        (*adfEnv.wFct)("adfUndelFile : the given parent sector isn't the entry parent");
        return RC_ERROR;
    }

    rc = adfGetFileBlocks ( vol, entry, &fileBlocks );
    if ( rc != RC_OK )
        return rc;

    for(i=0; i<fileBlocks.nbData; i++)
        if ( !adfIsBlockFree(vol,fileBlocks.data[i]) )
            return RC_ERROR;
        else
            adfSetBlockUsed(vol, fileBlocks.data[i]);
    for(i=0; i<fileBlocks.nbExtens; i++)
        if ( !adfIsBlockFree(vol,fileBlocks.extens[i]) )
            return RC_ERROR;
        else
            adfSetBlockUsed(vol, fileBlocks.extens[i]);

    free(fileBlocks.data);
    free(fileBlocks.extens);

    rc = adfReadEntryBlock ( vol, pSect, &parent );
    if ( rc != RC_OK )
        return rc;

    strncpy(name, entry->fileName, entry->nameLen);
    name[(int)entry->nameLen] = '\0';
    /* insert the entry in the parent hashTable, with the headerKey sector pointer */
    if ( adfCreateEntry(vol, &parent, name, entry->headerKey) == -1 )
        return RC_ERROR;

    if ( isDIRCACHE ( vol->fs.type ) ) {
        rc = adfAddInCache ( vol, &parent, (struct bEntryBlock *) entry );
        if ( rc != RC_OK )
            return rc;
    }

    return adfUpdateBitmap ( vol );
}


/*
 * adfUndelEntry
 *
 */
RETCODE adfUndelEntry ( struct AdfVolume * const vol,
                        const SECTNUM            parent,
                        const SECTNUM            nSect )
{
    struct bEntryBlock entry;

    RETCODE rc = adfReadEntryBlock ( vol, nSect, &entry );
    if ( rc != RC_OK )
        return rc;

    switch(entry.secType) {
    case ST_FILE:
        rc = adfUndelFile ( vol, parent, nSect, (struct bFileHeaderBlock*) &entry );
        break;
    case ST_DIR:
        rc = adfUndelDir ( vol, parent, nSect, (struct bDirBlock*) &entry );
        break;
    default:
        ;
    }

    return rc;
}


/*
 * adfCheckFile
 *
 */
RETCODE adfCheckFile ( struct AdfVolume * const              vol,
                       const SECTNUM                         nSect,
                       const struct bFileHeaderBlock * const file,
                       const int                             level )
{
    (void) nSect, (void) level;
    struct bFileExtBlock extBlock;
    struct bOFSDataBlock dataBlock;
    struct AdfFileBlocks fileBlocks;
    int n;
 
    RETCODE rc = adfGetFileBlocks ( vol, file, &fileBlocks );
    if ( rc != RC_OK )
        return rc;

/*printf("data %ld ext %ld\n",fileBlocks.nbData,fileBlocks.nbExtens);*/
    if ( adfVolIsOFS ( vol ) ) {
        /* checks OFS datablocks */
        for(n=0; n<fileBlocks.nbData; n++) {
/*printf("%ld\n",fileBlocks.data[n]);*/
            rc = adfReadDataBlock ( vol, fileBlocks.data[n], &dataBlock );
            if ( rc != RC_OK )
                goto adfCheckFile_free;

            if (dataBlock.headerKey!=fileBlocks.header)
                (*adfEnv.wFct)("adfCheckFile : headerKey incorrect");
            if ( dataBlock.seqNum != (unsigned) n + 1 )
                (*adfEnv.wFct)("adfCheckFile : seqNum incorrect");
            if (n<fileBlocks.nbData-1) {
                if (dataBlock.nextData!=fileBlocks.data[n+1])
                    (*adfEnv.wFct)("adfCheckFile : nextData incorrect");
                if (dataBlock.dataSize!=vol->datablockSize)
                    (*adfEnv.wFct)("adfCheckFile : dataSize incorrect");
            }
            else { /* last datablock */
                if (dataBlock.nextData!=0)
                    (*adfEnv.wFct)("adfCheckFile : nextData incorrect");
            }
        }
    }

    for(n=0; n<fileBlocks.nbExtens; n++) {
        rc = adfReadFileExtBlock ( vol, fileBlocks.extens[n], &extBlock );
        if ( rc != RC_OK )
            goto adfCheckFile_free;

        if (extBlock.parent!=file->headerKey)
            (*adfEnv.wFct)("adfCheckFile : extBlock parent incorrect");
        if (n<fileBlocks.nbExtens-1) {
            if (extBlock.extension!=fileBlocks.extens[n+1])
                (*adfEnv.wFct)("adfCheckFile : nextData incorrect");
        }
        else
            if (extBlock.extension!=0)
                (*adfEnv.wFct)("adfCheckFile : nextData incorrect");
    }

adfCheckFile_free:
    free(fileBlocks.data);
    free(fileBlocks.extens);

    return rc;
}


/*
 * adfCheckDir
 *
 */
RETCODE adfCheckDir ( const struct AdfVolume * const vol,
                      const SECTNUM                  nSect,
                      const struct bDirBlock * const dir,
                      const int                      level )
{
    // function to implement???
    // for now - suppressing warnings about unused parameters
    (void) vol, (void) nSect, (void) dir, (void) level;

    return RC_OK;
}


/*
 * adfCheckEntry
 *
 */
RETCODE adfCheckEntry ( struct AdfVolume * const vol,
                        const SECTNUM            nSect,
                        const int                level )
{
    struct bEntryBlock entry;

    RETCODE rc = adfReadEntryBlock ( vol, nSect, &entry );
    if ( rc != RC_OK )
        return rc;    

    switch(entry.secType) {
    case ST_FILE:
        rc = adfCheckFile(vol, nSect, (struct bFileHeaderBlock*)&entry, level);
        break;
    case ST_DIR:
        rc = adfCheckDir(vol, nSect, (struct bDirBlock*)&entry, level);
        break;
    default:
/*        printf("adfCheckEntry : not supported\n");*/					/* BV */
        rc = RC_ERROR;
    }

    return rc;
}


/*#############################################################################*/
