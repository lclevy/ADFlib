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
    if ( argc < 2 ) {
        fprintf ( stderr,
                  "required parameter (image/device) absent - aborting...\n");
        return 1;
    }

    struct AdfDevice *hd;
    struct AdfVolume *vol;
    struct AdfFile *file;
    unsigned char buf[600];
    FILE *out;
    struct AdfList *list;
 
    adfEnvInitDefault();

//	adfSetEnvFct(0,0,MyVer,0);

    /* mount existing device : FFS */
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


    /* write one file */
    file = adfFileOpen ( vol, "moon_gif", "w" );
    if (!file) return 1;
    out = fopen( argv[2],"rb");
    if (!out) return 1;
    
    unsigned len = 600;
    unsigned n = (unsigned) fread ( buf, sizeof(unsigned char), len, out );
    while(!feof(out)) {
        adfFileWrite ( file, n, buf );
        n = (unsigned) fread ( buf, sizeof(unsigned char), len, out );
    }
    if (n>0)
        adfFileWrite ( file, n, buf );

    fclose(out);

    adfFileClose ( file );

    /* the directory */
    list = adfGetDirEnt(vol,vol->curDirPtr);
    while(list) {
        adfEntryPrint ( list->content );
        adfFreeEntry(list->content);
        list = list->next;
    }
    freeList(list);


    /* re read this file */
    file = adfFileOpen ( vol, "moon_gif", "r" );
    if (!file) return 1;
    out = fopen("moon__gif","wb");
    if (!out) return 1;

    len = 300;
    n = adfFileRead ( file, len, buf );
    while(!adfEndOfFile(file)) {
        fwrite(buf,sizeof(unsigned char),n,out);
        n = adfFileRead ( file, len, buf );
    }
    if (n>0)
        fwrite(buf,sizeof(unsigned char),n,out);

    fclose(out);

    adfFileClose ( file );

    adfUnMount(vol);
    adfUnMountDev(hd);


    adfEnvCleanUp();

    return 0;
}
