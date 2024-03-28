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
 *  along with ADFLib; if not, write to the Free Software
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
    adfListFree ( list );
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

    list = head = NULL;
    block = NULL;
    bool delEnt = true;
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

            if ( adfReadGenBlock ( vol, i, block ) != ADF_RC_OK ) {
                free(block);
                adfFreeDelList ( head );
                return NULL;
            }

            delEnt = block->type == ADF_T_HEADER &&
                     ( block->secType == ADF_ST_DIR ||
                       block->secType == ADF_ST_FILE );

            if (delEnt) {
                if (head==NULL)
                    list = head = adfListNewCell ( NULL, (void *) block );
                else
                    list = adfListNewCell ( list, (void *) block );

                block = NULL;
            }
        }
    }

    if (block!=NULL) {
        free(block);
/*        printf("%p\n",block);*/
    }
    return head;
}


/*
 * adfReadGenBlock
 *
 */
ADF_RETCODE adfReadGenBlock ( struct AdfVolume * const vol,
                              const ADF_SECTNUM        nSect,
                              struct GenBlock * const  block )
{
    uint8_t buf[ADF_LOGICAL_BLOCK_SIZE];
    unsigned len;
    char name[ ADF_MAX_NAME_LEN + 1 ];

    ADF_RETCODE rc = adfVolReadBlock ( vol, (unsigned) nSect, buf );
    if ( rc != ADF_RC_OK )
        return rc;

    block->type =(int) swapLong(buf);
    block->secType =(int) swapLong(buf+vol->blockSize-4);
    block->sect = nSect;
    block->name = NULL;

    if ( block->type == ADF_T_HEADER ) {
        switch(block->secType) {
        case ADF_ST_FILE:
        case ADF_ST_DIR:
        case ADF_ST_LFILE:
        case ADF_ST_LDIR:
            len = min( (unsigned) ADF_MAX_NAME_LEN, buf [ vol->blockSize - 80 ] );
            strncpy(name, (char*)buf+vol->blockSize-79, len);
            name[len] = '\0';
            block->name = strdup(name);
            block->parent = (int32_t) swapLong ( buf + vol->blockSize - 12 );
            break;
        case ADF_ST_ROOT:
            break;
        default: 
            ;
        }
    }
    return ADF_RC_OK;
}


/*
 * adfCheckParent
 *
 */
ADF_RETCODE adfCheckParent ( struct AdfVolume * vol,
                             ADF_SECTNUM        pSect )
{
    if (adfIsBlockFree(vol, pSect)) {
        (*adfEnv.wFct)("adfCheckParent : parent doesn't exists");
        return ADF_RC_ERROR;
    }

    struct GenBlock * const block =
        (struct GenBlock *) malloc (sizeof(struct GenBlock));
    if ( block == NULL) {
        (*adfEnv.wFct)("adfCheckParent : malloc failed");
        return ADF_RC_ERROR;
    }
    block->name = NULL;

    /* verify if parent is a DIR or ROOT */
    ADF_RETCODE rc = adfReadGenBlock ( vol, pSect, block );
    if ( rc == ADF_RC_OK ) {
        if ( block->type != ADF_T_HEADER ||
           ( block->secType != ADF_ST_DIR &&
             block->secType != ADF_ST_ROOT ) ) {
            (*adfEnv.wFct)("adfCheckParent : parent secType is incorrect");
            rc = ADF_RC_ERROR;
        }
    }

    adfFreeGenBlock(block);

    return rc;
}


/*
 * adfUndelDir
 *
 */
ADF_RETCODE adfUndelDir ( struct AdfVolume *   vol,
                          ADF_SECTNUM          pSect,
                          ADF_SECTNUM          nSect,
                          struct AdfDirBlock * entry )
{
    (void) nSect;
    char name[ ADF_MAX_NAME_LEN + 1 ];

    /* check if the given parent sector pointer seems OK */
    ADF_RETCODE rc = adfCheckParent ( vol, pSect );
    if ( rc != ADF_RC_OK)
        return rc;

    if (pSect!=entry->parent) {
        (*adfEnv.wFct)("adfUndelDir : the given parent sector isn't the entry parent");
        return ADF_RC_ERROR;
    }

    if (!adfIsBlockFree(vol, entry->headerKey))
        return ADF_RC_ERROR;
    if ( adfVolHasDIRCACHE ( vol ) &&
         ! adfIsBlockFree ( vol, entry->extension ) )
    {
        return ADF_RC_ERROR;
    }

    struct AdfEntryBlock parent;
    rc = adfReadEntryBlock ( vol, pSect, &parent );
    if ( rc != ADF_RC_OK )
        return rc;

    strncpy(name, entry->dirName, entry->nameLen);
    name[(int)entry->nameLen] = '\0';
    /* insert the entry in the parent hashTable, with the headerKey sector pointer */
    adfSetBlockUsed(vol,entry->headerKey);
    if ( adfCreateEntry ( vol, &parent, name, entry->headerKey ) == -1 )
        return ADF_RC_ERROR;

    if ( adfVolHasDIRCACHE ( vol ) ) {
        rc = adfAddInCache ( vol, &parent, (struct AdfEntryBlock *) entry );
        if ( rc != ADF_RC_OK )
            return rc;

        adfSetBlockUsed(vol,entry->extension);
    }

    return adfUpdateBitmap ( vol );
}


/*
 * adfUndelFile
 *
 */
ADF_RETCODE adfUndelFile ( struct AdfVolume *          vol,
                           ADF_SECTNUM                 pSect,
                           ADF_SECTNUM                 nSect,
                           struct AdfFileHeaderBlock * entry )
{
    (void) nSect;
    char name[ ADF_MAX_NAME_LEN + 1 ];
    struct AdfFileBlocks fileBlocks;

    /* check if the given parent sector pointer seems OK */
    ADF_RETCODE rc = adfCheckParent ( vol, pSect );
    if ( rc != ADF_RC_OK )
        return rc;

    if (pSect!=entry->parent) {
        (*adfEnv.wFct)("adfUndelFile : the given parent sector isn't the entry parent");
        return ADF_RC_ERROR;
    }

    rc = adfGetFileBlocks ( vol, entry, &fileBlocks );
    if ( rc != ADF_RC_OK )
        return rc;

    /* mark file data blocks as used */
    for ( unsigned i = 0 ; i < fileBlocks.data.nItems ; i++ ) {
        if ( ! adfIsBlockFree ( vol, fileBlocks.data.sectors[i] ) ) {
            rc = ADF_RC_ERROR;
            goto adfUndelFile_error;
        } else
            adfSetBlockUsed ( vol, fileBlocks.data.sectors[i] );
    }

    /* mark file extension blocks as used */
    for ( unsigned i = 0 ; i < fileBlocks.extens.nItems ; i++ ) {
        if ( ! adfIsBlockFree ( vol, fileBlocks.extens.sectors[i] ) ) {
            rc = ADF_RC_ERROR;
            goto adfUndelFile_error;
        }
        else
            adfSetBlockUsed ( vol, fileBlocks.extens.sectors[i] );
    }

    adfVectorFree ( (struct AdfVector *) &fileBlocks.data );
    adfVectorFree ( (struct AdfVector *) &fileBlocks.extens );

    /* mark file header block as used */
    adfSetBlockUsed ( vol, nSect );

    struct AdfEntryBlock parent;
    rc = adfReadEntryBlock ( vol, pSect, &parent );
    if ( rc != ADF_RC_OK )
        goto adfUndelFile_error;

    strncpy(name, entry->fileName, entry->nameLen);
    name[(int)entry->nameLen] = '\0';
    /* insert the entry in the parent hashTable, with the headerKey sector pointer */
    if ( adfCreateEntry(vol, &parent, name, entry->headerKey) == -1 ) {
        rc = ADF_RC_ERROR;
        goto adfUndelFile_error;
    }
    if ( adfVolHasDIRCACHE ( vol ) ) {
        rc = adfAddInCache ( vol, &parent, (struct AdfEntryBlock *) entry );
        if ( rc != ADF_RC_OK )
            goto adfUndelFile_error;
    }

    return adfUpdateBitmap ( vol );

adfUndelFile_error:
    adfVectorFree ( (struct AdfVector *) &fileBlocks.data );
    adfVectorFree ( (struct AdfVector *) &fileBlocks.extens );
    return rc;
}


/*
 * adfUndelEntry
 *
 */
ADF_RETCODE adfUndelEntry ( struct AdfVolume * const vol,
                            const ADF_SECTNUM        parent,
                            const ADF_SECTNUM        nSect )
{
    struct AdfEntryBlock entry;

    ADF_RETCODE rc = adfReadEntryBlock ( vol, nSect, &entry );
    if ( rc != ADF_RC_OK )
        return rc;

    switch(entry.secType) {
    case ADF_ST_FILE:
        rc = adfUndelFile ( vol, parent, nSect, (struct AdfFileHeaderBlock *) &entry );
        break;
    case ADF_ST_DIR:
        rc = adfUndelDir ( vol, parent, nSect, (struct AdfDirBlock *) &entry );
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
ADF_RETCODE adfCheckFile ( struct AdfVolume * const                vol,
                           const ADF_SECTNUM                       nSect,
                           const struct AdfFileHeaderBlock * const file,
                           const int                               level )
{
    (void) nSect, (void) level;

    struct AdfFileBlocks fileBlocks;
    ADF_RETCODE rc = adfGetFileBlocks ( vol, file, &fileBlocks );
    if ( rc != ADF_RC_OK )
        return rc;

/*printf("data %ld ext %ld\n",fileBlocks.nbData,fileBlocks.nbExtens);*/
    if ( adfVolIsOFS ( vol ) ) {
        /* checks OFS datablocks */
        struct AdfOFSDataBlock dataBlock;
        for ( unsigned n = 0 ; n < fileBlocks.data.nItems ; n++ ) {
/*printf("%ld\n",fileBlocks.data[n]);*/
            rc = adfReadDataBlock ( vol, fileBlocks.data.sectors[n], &dataBlock );
            if ( rc != ADF_RC_OK )
                goto adfCheckFile_free;

            if (dataBlock.headerKey!=fileBlocks.header)
                (*adfEnv.wFct)("adfCheckFile : headerKey incorrect");
            if ( dataBlock.seqNum != (unsigned) n + 1 )
                (*adfEnv.wFct)("adfCheckFile : seqNum incorrect");
            if (n < fileBlocks.data.nItems - 1 ) {
                if ( dataBlock.nextData != fileBlocks.data.sectors[ n + 1 ] )
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

    struct AdfFileExtBlock extBlock;
    for ( unsigned n = 0 ; n < fileBlocks.extens.nItems ; n++ ) {
        rc = adfReadFileExtBlock ( vol, fileBlocks.extens.sectors[n], &extBlock );
        if ( rc != ADF_RC_OK )
            goto adfCheckFile_free;

        if (extBlock.parent!=file->headerKey)
            (*adfEnv.wFct)("adfCheckFile : extBlock parent incorrect");
        if ( n < fileBlocks.extens.nItems - 1 ) {
            if ( extBlock.extension != fileBlocks.extens.sectors[ n + 1 ] )
                (*adfEnv.wFct)("adfCheckFile : nextData incorrect");
        }
        else
            if (extBlock.extension!=0)
                (*adfEnv.wFct)("adfCheckFile : nextData incorrect");
    }

adfCheckFile_free:
    adfVectorFree ( (struct AdfVector *) &fileBlocks.data );
    adfVectorFree ( (struct AdfVector *) &fileBlocks.extens );

    return rc;
}


/*
 * adfCheckDir
 *
 */
ADF_RETCODE adfCheckDir ( const struct AdfVolume * const   vol,
                          const ADF_SECTNUM                nSect,
                          const struct AdfDirBlock * const dir,
                          const int                        level )
{
    // function to implement???
    // for now - suppressing warnings about unused parameters
    (void) vol, (void) nSect, (void) dir, (void) level;

    return ADF_RC_ERROR;
}


/*
 * adfCheckEntry
 *
 */
ADF_RETCODE adfCheckEntry ( struct AdfVolume * const vol,
                            const ADF_SECTNUM        nSect,
                            const int                level )
{
    struct AdfEntryBlock entry;

    ADF_RETCODE rc = adfReadEntryBlock ( vol, nSect, &entry );
    if ( rc != ADF_RC_OK )
        return rc;    

    switch(entry.secType) {
    case ADF_ST_FILE:
        rc = adfCheckFile ( vol, nSect, (struct AdfFileHeaderBlock *) &entry, level );
        break;
    case ADF_ST_DIR:
        rc = adfCheckDir ( vol, nSect, (struct AdfDirBlock *) &entry, level );
        break;
    default:
/*        printf("adfCheckEntry : not supported\n");*/					/* BV */
        rc = ADF_RC_ERROR;
    }

    return rc;
}


/*#############################################################################*/
