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
    struct AdfVolume *vol;
    struct AdfFile *fic;
    unsigned char buf[1];
    struct AdfList *list, *cell;
    struct GenBlock *block;
    BOOL true = TRUE;
 
    adfEnvInitDefault();

    adfChgEnvProp(PR_USEDIRC,&true);
 
    /* create and mount one device */
    struct AdfDevice * const hd = adfCreateDev ( "dump", "undel-newdev", 80, 2, 11 );
    if (!hd) {
        fprintf(stderr, "can't mount device\n");
        adfEnvCleanUp(); exit(1);
    }

    adfDeviceInfo(hd);

    if (adfCreateFlop( hd, "empty", FSMASK_FFS|FSMASK_DIRCACHE )!=RC_OK) {
		fprintf(stderr, "can't create floppy\n");
        adfUnMountDev(hd);
        adfCloseDev(hd);
        adfEnvCleanUp(); exit(1);
    }

    vol = adfMount ( hd, 0, ADF_ACCESS_MODE_READWRITE );
    if (!vol) {
        adfUnMountDev(hd);
        adfCloseDev(hd);
        fprintf(stderr, "can't mount volume\n");
        adfEnvCleanUp(); exit(1);
    }

    fic = adfFileOpen ( vol, "file_1a", ADF_FILE_MODE_WRITE );
    if (!fic) {
        adfUnMount(vol);
        adfUnMountDev(hd);
        adfCloseDev(hd);
        adfEnvCleanUp();
        exit(1);
    }
    adfFileWrite ( fic, 1, buf );
    adfFileClose ( fic );

    puts("\ncreate file_1a");
    adfVolumeInfo(vol);

    adfCreateDir(vol,vol->curDirPtr,"dir_5u");
    puts("\ncreate dir_5u");
    adfVolumeInfo(vol);

    cell = list = adfGetDirEnt(vol, vol->curDirPtr);
    while(cell) {
        adfEntryPrint ( cell->content );
        cell = cell->next;
    }
    adfFreeDirList(list);

    puts("\nremove file_1a");
    adfRemoveEntry(vol,vol->curDirPtr,"file_1a");
    adfVolumeInfo(vol);

    adfRemoveEntry(vol,vol->curDirPtr,"dir_5u");
    puts("\nremove dir_5u");
    adfVolumeInfo(vol);

    cell = list = adfGetDelEnt(vol);
    while(cell) {
        block =(struct GenBlock*) cell->content;
        printf ( "%s %d %d %d\n",
                 block->name,
                 block->type,
                 block->secType,
                 block->sect );
        cell = cell->next;
    }
    adfFreeDelList(list);

    adfUndelEntry(vol,vol->curDirPtr,883); // file_1a
    puts("\nundel file_1a");
    adfVolumeInfo(vol);

    adfUndelEntry(vol,vol->curDirPtr,885); // dir_5u
    puts("\nundel dir_5u");
    adfVolumeInfo(vol);

    cell = list = adfGetDirEnt(vol, vol->curDirPtr);
    while(cell) {
        adfEntryPrint ( cell->content );
        cell = cell->next;
    }
    adfFreeDirList(list);


    adfUnMount(vol);
    adfUnMountDev(hd);
    adfCloseDev(hd);

    adfEnvCleanUp();

    return 0;
}
