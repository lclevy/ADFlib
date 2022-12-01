/*
 * dir_test.c
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include"adflib.h"


int test_chdir_hlink ( struct Volume * vol,
                       char *          hlink,
                       int             num_entries );


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
    struct Device *hd;
    struct Volume *vol;
    struct List *list, *cell;
    SECTNUM nSect;
 
    adfEnvInitDefault();

//	adfSetEnvFct(0,0,MyVer,0);

    /* mount existing device */
/* testffs.adf */
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

    cell = list = adfGetDirEnt(vol,vol->curDirPtr);
    while(cell) {
        printEntry(cell->content);
        cell = cell->next;
    }
    adfFreeDirList(list);

    putchar('\n');

    /* cd dir_2 */
    nSect = adfChangeDir(vol, "dir_2");

    cell = list = adfGetDirEnt(vol,vol->curDirPtr);
    while(cell) {
        printEntry(cell->content);
        cell = cell->next;
    }
    adfFreeDirList(list);

    putchar('\n');

    /* cd .. */
    adfParentDir(vol);

    list = adfGetDirEnt(vol,vol->curDirPtr);
    while(list) {
        printEntry(list->content);
        adfFreeEntry(list->content);
        list = list->next;
    }
    freeList(list);

    int status = 0;

    /* cd hlink_dir1 (hardlink to dir_1) */
    status += test_chdir_hlink ( vol, "hlink_dir1", 1 );

    /* cd hlink_dir2 (hardlink to dir_2) */
    status += test_chdir_hlink ( vol, "hlink_dir2", 2 );


    adfUnMount(vol);
    adfUnMountDev(hd);


    adfEnvCleanUp();

    return status;
}


int test_chdir_hlink ( struct Volume * vol,
                       char *          hlink,
                       int             num_entries )
{
    int status = 0;

    adfToRootDir ( vol );

    printf ("*** Test entering hard link %s\n", hlink );
    RETCODE rc = adfChangeDir ( vol, hlink );
    if ( rc != RC_OK ) {
        fprintf ( stderr, "adfChangeDir error entering hard link %s.\n",
                  hlink );
        status++;
    }

    struct List * list = adfGetDirEnt ( vol, vol->curDirPtr );
    int count = 0;
    while ( list ) {
        //printEntry ( list->content );
        list = list->next;
        count++;
    }

    if ( count != num_entries ) {
        fprintf ( stderr, "Incorrect number of entries (%d) after chdir to hard link %s.\n",
                  count, hlink );
        status++;
    }

    adfToRootDir ( vol );

    return status;
}
