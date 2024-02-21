/*
 * hd_test.c
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
    struct AdfFile *file;
    unsigned char buf[600];
    long n;
    FILE *out;
    long len;
    struct AdfList *list;

    adfEnvInitDefault();


    /* create and mount one device */
    hd = adfCreateDumpDevice("flfile_test-newdev", 80, 11, 2);
    if (!hd) {
        fprintf(stderr, "can't mount device\n");
        adfEnvCleanUp(); exit(1);
    }
    adfCreateFlop ( hd, "empty", ADF_DOSFS_FFS |
                                 ADF_DOSFS_DIRCACHE );

    vol = adfVolMount ( hd, 0, ADF_ACCESS_MODE_READWRITE );
    if (!vol) {
        adfUnMountDev(hd);
        adfCloseDev(hd);
        fprintf(stderr, "can't mount volume\n");
        adfEnvCleanUp(); exit(1);
    }

    adfVolInfo(vol);

    /* the directory */
    list = adfGetDirEnt(vol,vol->curDirPtr);
    while(list) {
        adfEntryPrint ( list->content );
        adfFreeEntry(list->content);
        list = list->next;
    }
    freeList(list);

    /* write one file */
    file = adfOpenFile(vol, "moon_gif","w");
    if (!file) return 1;
    out = fopen("Check/MOON.GIF","rb");
    if (!out) return 1;
    
    len = 600;
    n = fread(buf,sizeof(unsigned char),len,out);
    while(!feof(out)) {
        adfWriteFile(file, n, buf);
        n = fread(buf,sizeof(unsigned char),len,out);
    }
    if (n>0)
        adfWriteFile(file, n, buf);

    fclose(out);

    adfCloseFile(file);

    /* the directory */
    list = adfGetDirEnt(vol,vol->curDirPtr);
    while(list) {
        adfEntryPrint ( list->content );
        adfFreeEntry(list->content);
        list = list->next;
    }
    freeList(list);








    adfVolUnMount(vol);
    adfUnMountDev(hd);
    adfCloseDev(hd);

    adfEnvCleanUp();

    return 0;
}
