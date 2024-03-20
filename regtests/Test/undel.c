/*
 *  undel.c */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include"adflib.h"


void MyVer(char *msg)
{
    fprintf(stderr,"Verbose [%s]\n",msg);
}


/*
 *
 *
 */
int main(int argc, char *argv[])
{
    (void) argc, (void) argv;
    int status = 0;
    struct AdfList *list, *cell;
 
    adfEnvInitDefault();

    adfEnvSetProperty ( ADF_PR_USEDIRC, true );
 
    /* create and mount one device */
    struct AdfDevice * const hd = adfDevCreate ( "dump", "undel-newdev", 80, 2, 11 );
    if (!hd) {
        fprintf(stderr, "can't mount device\n");
        status = 1;
        goto clean_up_env;
    }

    adfDevInfo ( hd );

    if ( adfCreateFlop ( hd, "empty", ADF_DOSFS_FFS |
                                      ADF_DOSFS_DIRCACHE ) != ADF_RC_OK )
    {
		fprintf(stderr, "can't create floppy\n");
        status = 2;
	goto clean_up_dev_close;
    }

    struct AdfVolume * const vol = adfVolMount ( hd, 0, ADF_ACCESS_MODE_READWRITE );
    if (!vol) {
        fprintf(stderr, "can't mount volume\n");
        status = 3;
	goto clean_up_dev_unmount;
    }

    struct AdfFile * const fic = adfFileOpen ( vol, "file_1a", ADF_FILE_MODE_WRITE );
    if (!fic) {
        status = 4;
        goto clean_up_volume;
    }
    const unsigned char buf[1];
    adfFileWrite ( fic, 1, buf );
    adfFileClose ( fic );

    puts("\ncreate file_1a");
    adfVolInfo(vol);

    adfCreateDir(vol,vol->curDirPtr,"dir_5u");
    puts("\ncreate dir_5u");
    adfVolInfo(vol);

    cell = list = adfGetDirEnt(vol, vol->curDirPtr);
    while(cell) {
        adfEntryPrint ( cell->content );
        cell = cell->next;
    }
    adfFreeDirList(list);

    puts("\nremove file_1a");
    adfRemoveEntry(vol,vol->curDirPtr,"file_1a");
    adfVolInfo(vol);

    adfRemoveEntry(vol,vol->curDirPtr,"dir_5u");
    puts("\nremove dir_5u");
    adfVolInfo(vol);

    cell = list = adfGetDelEnt(vol);
    if (cell)
        puts ( "Found deleted entries:" );
    else {
        fprintf ( stderr, "No deleted entries found! -> ERROR.\n" );
        status = 5;
        goto clean_up_volume;
    }
    while(cell) {
        struct GenBlock * const block = (struct GenBlock *) cell->content;
        printf ( "name %s, block type %d, 2nd type %d, sector %d\n",
                 block->name,
                 block->type,
                 block->secType,
                 block->sect );
        cell = cell->next;
    }
    adfFreeDelList(list);

    adfUndelEntry(vol,vol->curDirPtr,883); // file_1a
    puts("\nundel file_1a");
    adfVolInfo(vol);

    adfUndelEntry(vol,vol->curDirPtr,885); // dir_5u
    puts("\nundel dir_5u");
    adfVolInfo(vol);

    cell = list = adfGetDirEnt(vol, vol->curDirPtr);
    while(cell) {
        adfEntryPrint ( cell->content );
        cell = cell->next;
    }
    adfFreeDirList(list);

clean_up_volume:
    adfVolUnMount(vol);

clean_up_dev_unmount:
    adfDevUnMount ( hd );

clean_up_dev_close:
    adfDevClose ( hd );

clean_up_env:
    adfEnvCleanUp();

    return status;
}
