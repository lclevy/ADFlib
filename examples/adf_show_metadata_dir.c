
#include <adf_dir.h>
#include <stdio.h>

#include "adf_show_metadata_dir.h"

#include "adf_show_metadata_common.h"


void show_directory_metadata ( struct adfVolume * const vol,
                               SECTNUM                  dir_sector )
{
    struct bDirBlock //bEntryBlock
        dir_block;
    if ( adfReadEntryBlock ( vol, dir_sector,
                             ( struct bEntryBlock * ) &dir_block ) != RC_OK )
    {
        fprintf ( stderr, "Error reading directory entry block (%d)\n",
                  dir_sector );
        return;
    }

    printf ( "\nDirectory block:\n"
             //"  Offset  field\t\tvalue\n"
             "  0x000  type\t\t0x%x\t\t%u\n"
             "  0x004  headerKey\t0x%x\t\t%u\n"
             "  0x008  highSeq\t0x%x\t\t%u\n"
             "  0x00c  hashTableSize\t0x%x\t\t%u\n"
             "  0x010  r1\t\t0x%x\n"
             "  0x014  checkSum\t0x%x\n"
             "  0x018  hashTable[%u]:\t(see below)\n"
             "  0x138  r2[2]:\t\t(see below)\n"
             "  0x140  access\t\t0x%x\n"
             "  0x144  r4\t\t0x%x\n"
             "  0x148  commLen\t0x%x\t\t%u\n"
             "  0x149  comment [ %u ]:\t%s\n"
             "  0x199  r5[%u]:\t(see below)\n"
             "  0x1a4  days\t\t0x%x\t\t%u\n"
             "  0x1a8  mins\t\t0x%x\t\t%u\n"
             "  0x1ac  ticks\t\t0x%x\t\t%u\n"
             "  0x1b0  nameLen\t0x%x\t\t%u\n"
             "  0x1b1  dirName:\t%s\n"
             "  0x1d0  r6\t\t0x%x\n"
             "  0x1d4  real\t\t0x%x\t\t%u\n"
             "  0x1d8  nextLink\t0x%x\t\t%u\n"
             "  0x1dc  r7[5]:\t\t(see below)\n"
             "  0x1f0  nextSameHash\t0x%x\t\t%u\n"
             "  0x1f4  parent\t\t0x%x\t\t%u\n"
             "  0x1f8  extension\t0x%x\t\t%u\n"
             "  0x1fc  secType\t0x%x\t\t%d\n",
             dir_block.type, dir_block.type,
             dir_block.headerKey, dir_block.headerKey,
             dir_block.highSeq, dir_block.highSeq,
             dir_block.hashTableSize, dir_block.hashTableSize,
             dir_block.r1,
             dir_block.checkSum,
             HT_SIZE, //hashTable[HT_SIZE],
             //dir_block.r2[2],
             dir_block.access,
             dir_block.r4,
             dir_block.commLen, dir_block.commLen,
             MAXCMMTLEN + 1, dir_block.comment,
             91 - ( MAXCMMTLEN + 1 ), //dir_block.r5[91-(MAXCMMTLEN+1)],
             dir_block.days, dir_block.days,
             dir_block.mins, dir_block.mins,
             dir_block.ticks, dir_block.ticks,
             dir_block.nameLen, dir_block.nameLen,
             dir_block.dirName,
             dir_block.r6,
             dir_block.real, dir_block.real,
             dir_block.nextLink, dir_block.nextLink,
             //dir_block.r7[5],
             dir_block.nextSameHash, dir_block.nextSameHash,
             dir_block.parent, dir_block.parent,
             dir_block.extension, dir_block.extension,
             dir_block.secType, dir_block.secType
        );

    show_hashtable ( ( const uint32_t * const ) dir_block.hashTable );
}

