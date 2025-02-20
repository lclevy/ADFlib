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

    struct AdfVolume *vol;
    struct AdfList *list, *head;
 
    adfEnvInitDefault();

    /* open and mount existing device */
    struct AdfDevice * hd = adfDevOpen ( argv[1], ADF_ACCESS_MODE_READWRITE );
    if ( ! hd ) {
        fprintf ( stderr, "Cannot open file/device '%s' - aborting...\n",
                  argv[1] );
        adfEnvCleanUp();
        exit(1);
    }

    ADF_RETCODE rc = adfDevMount ( hd );
    if ( rc != ADF_RC_OK ) {
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
	
    adfVolInfo(vol);

    head = list = adfGetDirEnt(vol,vol->curDirPtr);
    while(list) {
        adfEntryPrint ( list->content );
        //adfFreeEntry(list->content);
        list = list->next;
    }
    adfFreeDirList(head);

    putchar('\n');


    /* cd dir_2 */
    //ADF_SECTNUM nSect =
    adfChangeDir(vol, "same_hash");

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

    adfVolInfo(vol);

    adfVolUnMount(vol);
    adfDevUnMount ( hd );
    adfDevClose ( hd );

    adfEnvCleanUp();

    return 0;
}
