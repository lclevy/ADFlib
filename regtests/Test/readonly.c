/*
 * readonly.c
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
    if ( argc < 2 ) {
        fprintf ( stderr,
                  "required parameter (image/device) absent - aborting...\n");
        return 1;
    }

    struct AdfVolume *vol;
    struct AdfList *list;
 
    adfEnvInitDefault();

    /* open and mount existing device */
    struct AdfDevice * hd = adfDevOpen ( argv[1], ADF_ACCESS_MODE_READWRITE );
    if ( ! hd ) {
        fprintf ( stderr, "Cannot open file/device '%s' - aborting...\n",
                  argv[1] );
        adfEnvCleanUp();
        exit(1);
    }

    RETCODE rc = adfDevMount ( hd );
    if ( rc != RC_OK ) {
        fprintf(stderr, "can't mount device\n");
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

    adfVolumeInfo(vol);

    list = adfGetDirEnt(vol,vol->curDirPtr);
    while(list) {
        adfEntryPrint ( list->content );
        adfFreeEntry(list->content);
        list = list->next;
    }
    freeList(list);

    putchar('\n');

    adfCreateDir(vol,vol->curDirPtr,"newdir");

    /* cd dir_2 */
    //SECTNUM nSect = adfChangeDir(vol, "same_hash");
    adfChangeDir(vol, "same_hash");

    list = adfGetDirEnt(vol,vol->curDirPtr);
    while(list) {
        adfEntryPrint ( list->content );
        adfFreeEntry(list->content);
        list = list->next;
    }
    freeList(list);

    putchar('\n');

    /* not empty */
    adfRemoveEntry(vol, vol->curDirPtr, "mon.paradox");

    /* first in same hash linked list */
    adfRemoveEntry(vol, vol->curDirPtr, "file_3a");
    /* second */
    adfRemoveEntry(vol, vol->curDirPtr, "dir_3");
    /* last */
    adfRemoveEntry(vol, vol->curDirPtr, "dir_1a");

    list = adfGetDirEnt(vol,vol->curDirPtr);
    while(list) {
        adfEntryPrint ( list->content );
        adfFreeEntry(list->content);
        list = list->next;
    }
    freeList(list);

    putchar('\n');

    adfParentDir(vol);

    adfRemoveEntry(vol, vol->curDirPtr, "mod.And.DistantCall");

    list = adfGetDirEnt(vol,vol->curDirPtr);
    while(list) {
        adfEntryPrint ( list->content );
        adfFreeEntry(list->content);
        list = list->next;
    }
    freeList(list);

    putchar('\n');

    adfVolumeInfo(vol);

    adfUnMount(vol);
    adfDevUnMount ( hd );
    adfDevClose ( hd );

    adfEnvCleanUp();

    return 0;
}
