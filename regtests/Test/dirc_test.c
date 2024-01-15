/*
 * dir_test.c
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
    struct AdfDevice *hd;
    struct AdfVolume *vol;
    struct AdfList *list, *cell;
    SECTNUM nSect;
 
    adfEnvInitDefault();

//	adfSetEnvFct(0,0,MyVer,0);

    /* mount existing device */
    hd = adfMountDev ( "testdirc.adf", ADF_ACCESS_MODE_READWRITE );
    if (!hd) {
        fprintf(stderr, "can't mount device\n");
        adfEnvCleanUp(); exit(1);
    }

    vol = adfMount ( hd, 0, ADF_ACCESS_MODE_READWRITE );
    if (!vol) {
        adfUnMountDev(hd);
        fprintf(stderr, "can't mount volume\n");
        adfEnvCleanUp(); exit(1);
    }

    adfVolumeInfo(vol);

    cell = list = adfGetDirEnt(vol,vol->curDirPtr);
    while(cell) {
        adfEntryPrint ( cell->content );
        cell = cell->next;
    }
    adfFreeDirList(list);

    putchar('\n');


    adfUnMount(vol);
    adfUnMountDev(hd);


    adfEnvCleanUp();

    return 0;
}
