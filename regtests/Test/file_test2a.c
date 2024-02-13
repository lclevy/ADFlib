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
    struct AdfFile *file;
    unsigned char buf[600];
    long n;
    FILE *out;
    long len;
    struct AdfList *list;
 
    adfEnvInitDefault();

//	adfSetEnvFct(0,0,MyVer,0);

    /* mount existing device : FFS */
    hd = adfMountDev ( "hd.adf", ADF_ACCESS_MODE_READWRITE );
    if (!hd) {
        fprintf(stderr, "can't mount device\n");
        adfEnvCleanUp(); exit(1);
    }
    adfDeviceInfo(hd);

    vol = adfMount ( hd, 1, ADF_ACCESS_MODE_READWRITE );
    if (!vol) {
        adfUnMountDev(hd);
        adfCloseDev(hd);
        fprintf(stderr, "can't mount volume\n");
        adfEnvCleanUp(); exit(1);
    }

    adfVolumeInfo(vol);


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
/*    list = adfGetDirEnt(vol,vol->curDirPtr);
    while(list) {
        adfEntryPrint(list->content);
        adfFreeEntry(list->content);
        list = list->next;
    }
    freeList(list);
*/

    /* re read this file */
    file = adfOpenFile(vol, "moon_gif","r");
    if (!file) return 1;
    out = fopen("moon__gif","wb");
    if (!out) return 1;

    len = 300;
    n = adfReadFile(file, len, buf);
    while(!adfEndOfFile(file)) {
        fwrite(buf,sizeof(unsigned char),n,out);
        n = adfReadFile(file, len, buf);
    }
    if (n>0)
        fwrite(buf,sizeof(unsigned char),n,out);

    fclose(out);

    adfCloseFile(file);

    adfUnMount(vol);
    adfUnMountDev(hd);
    adfCloseDev(hd);

    adfEnvCleanUp();

    return 0;
}
