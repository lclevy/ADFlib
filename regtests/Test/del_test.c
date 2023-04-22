/*
 * del_test.c
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include"adflib.h"
#include "adf_dir.h"

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
    if ( argc < 2 ) {
        fprintf ( stderr,
                  "required parameter (image/device) absent - aborting...\n");
        return 1;
    }

    struct AdfDevice *hd;
    struct AdfVolume *vol;
    struct AdfList *list, *head;
    SECTNUM nSect;
 
    adfEnvInitDefault();

    /* mount existing device */
    hd = adfMountDev( argv[1],FALSE );
    if (!hd) {
        fprintf(stderr, "can't mount device\n");
        adfEnvCleanUp(); exit(1);
    }

    vol = adfMount(hd, 0, FALSE);
    if (!vol) {
        adfUnMountDev(hd);
        fprintf(stderr, "can't mount volume\n");
        adfEnvCleanUp(); exit(1);
    }
	
    adfVolumeInfo(vol);

    head = list = adfGetDirEnt(vol,vol->curDirPtr);
    while(list) {
        adfEntryPrint ( list->content );
        //adfFreeEntry(list->content);
        list = list->next;
    }
    adfFreeDirList(head);

    putchar('\n');


    /* cd dir_2 */
    nSect = adfChangeDir(vol, "same_hash");

    head = list = adfGetDirEnt(vol,vol->curDirPtr);
    while(list) {
        adfEntryPrint ( list->content );
        //adfFreeEntry(list->content);
        list = list->next;
    }
    adfFreeDirList(head);

    putchar('\n');

    /* not empty */
    adfRemoveEntry(vol, vol->curDirPtr, "dir_2");

    /* first in same hash linked list */
    adfRemoveEntry(vol, vol->curDirPtr, "file_3a");
    /* second */
    adfRemoveEntry(vol, vol->curDirPtr, "dir_3");
    /* last */
    adfRemoveEntry(vol, vol->curDirPtr, "dir_1a");

    head = list = adfGetDirEnt(vol,vol->curDirPtr);
    while(list) {
        adfEntryPrint ( list->content );
        //adfFreeEntry(list->content);
        list = list->next;
    }
    adfFreeDirList(head);

    putchar('\n');

    adfParentDir(vol);

    adfRemoveEntry(vol, vol->curDirPtr, "mod.And.DistantCall");

    head = list = adfGetDirEnt(vol,vol->curDirPtr);
    while(list) {
        adfEntryPrint ( list->content );
        //adfFreeEntry(list->content);
        list = list->next;
    }
    adfFreeDirList(head);

    putchar('\n');

    adfVolumeInfo(vol);

    adfUnMount(vol);
    adfUnMountDev(hd);


    adfEnvCleanUp();

    return 0;
}
