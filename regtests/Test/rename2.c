/*
 * rename.c
 */


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
    struct AdfList *list, *cell;
 
    adfEnvInitDefault();

    /* create and mount one device */
    struct AdfDevice * const hd = adfDevCreate ( "dump", "rename2-newdev", 80, 2, 11 );
    if (!hd) {
        fprintf(stderr, "can't mount device\n");
        adfEnvCleanUp(); exit(1);
    }

    if ( adfCreateFlop ( hd, "empty", ADF_DOSFS_FFS |
                                      ADF_DOSFS_DIRCACHE ) != ADF_RC_OK )
    {
		fprintf(stderr, "can't create floppy\n");
        adfDevUnMount ( hd );
        adfDevClose ( hd );
        adfEnvCleanUp(); exit(1);
    }

    vol = adfVolMount ( hd, 0, ADF_ACCESS_MODE_READWRITE );
    if (!vol) {
        adfDevUnMount ( hd );
        adfDevClose ( hd );
        fprintf(stderr, "can't mount volume\n");
        adfEnvCleanUp(); exit(1);
    }

    adfDevInfo(hd);

    adfVolInfo(vol);


    cell = list = adfGetDirEnt(vol,vol->curDirPtr);
    while(cell) {
        adfEntryPrint ( cell->content );
        cell = cell->next;
    }
    adfFreeDirList(list);

    putchar('\n');

    adfCreateDir(vol,vol->curDirPtr,"dir_5u");

    adfCreateDir(vol,883,"dir_51");

    adfCreateDir(vol,vol->curDirPtr,"toto");
printf("[dir = %d]\n",vol->curDirPtr);
    cell = list = adfGetDirEnt(vol, vol->curDirPtr);
    while(cell) {
        adfEntryPrint ( cell->content );
        cell = cell->next;
    }
    adfFreeDirList(list);
printf("[dir = %ld]\n",883L);
    cell = list = adfGetDirEnt(vol,883);
    while(cell) {
        adfEntryPrint ( cell->content );
        cell = cell->next;
    }
    adfFreeDirList(list);

    adfRenameEntry(vol, 883,"dir_51", vol->curDirPtr,"dir_55");
putchar('\n');

printf("[dir = %d]\n",vol->curDirPtr);
    cell = list = adfGetDirEnt(vol, vol->curDirPtr);
    while(cell) {
        adfEntryPrint ( cell->content );
        cell = cell->next;
    }
    adfFreeDirList(list);
printf("[dir = %ld]\n",883L);
    cell = list = adfGetDirEnt(vol,883);
    while(cell) {
        adfEntryPrint ( cell->content );
        cell = cell->next;
    }
    adfFreeDirList(list);

    adfRenameEntry(vol, vol->curDirPtr,"toto", 883,"moved_dir");

putchar('\n');

printf("[dir = %d]\n",vol->curDirPtr);
    cell = list = adfGetDirEnt(vol, vol->curDirPtr);
    while(cell) {
        adfEntryPrint ( cell->content );
        cell = cell->next;
    }
    adfFreeDirList(list);
printf("[dir = %ld]\n",883L);
    cell = list = adfGetDirEnt(vol,883);
    while(cell) {
        adfEntryPrint ( cell->content );
        cell = cell->next;
    }
    adfFreeDirList(list);

	    adfVolUnMount(vol);
    adfDevUnMount ( hd );
    adfDevClose ( hd );

    adfEnvCleanUp();

    return 0;
}
