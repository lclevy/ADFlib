/*
 * file_test.c
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
    struct AdfFile *file;
    unsigned char buf[600];
    FILE *out;
 
    adfEnvInitDefault();

//	adfSetEnvFct(0,0,MyVer,0);

    /* open and mount existing device : FFS */
    struct AdfDevice * hd = adfOpenDev ( argv[1], ADF_ACCESS_MODE_READWRITE );
    if ( ! hd ) {
        fprintf ( stderr, "Cannot open file/device '%s' - aborting...\n",
                  argv[1] );
        adfEnvCleanUp();
        exit(1);
    }

    RETCODE rc = adfMountDev ( hd );
    if ( rc != RC_OK ) {
        fprintf(stderr, "can't mount device\n");
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

    adfVolumeInfo(vol);

    file = adfFileOpen ( vol, "mod.and.distantcall", ADF_FILE_MODE_READ );
    if (!file) return 1;
    out = fopen("mod.distant","wb");
    if (!out) return 1;
    
    unsigned len = 600;
    unsigned n = adfFileRead ( file, len, buf );
    while(!adfEndOfFile(file)) {
        fwrite(buf,sizeof(unsigned char),n,out);
        n = adfFileRead ( file, len, buf );
    }
    if (n>0)
        fwrite(buf,sizeof(unsigned char),n,out);

    fclose(out);

    adfFileClose ( file );

    file = adfFileOpen ( vol, "emptyfile", ADF_FILE_MODE_READ );
    if (!file) { 
        adfUnMount(vol);
        adfUnMountDev(hd);
        adfCloseDev(hd);
        fprintf(stderr, "can't open file\n");
        exit(1);
    }
 
    n = adfFileRead ( file, 2, buf );

    adfFileClose ( file );

    adfUnMount(vol);
    adfUnMountDev(hd);
    adfCloseDev(hd);


    /* ofs */

    hd = adfOpenDev ( argv[2], ADF_ACCESS_MODE_READWRITE );
    if ( ! hd ) {
        fprintf ( stderr, "Cannot open file/device '%s' - aborting...\n",
                  argv[2] );
        adfEnvCleanUp();
        exit(1);
    }

    rc = adfMountDev ( hd );
    if ( rc != RC_OK ) {
        fprintf(stderr, "can't mount device\n");
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

    adfVolumeInfo(vol);

    file = adfFileOpen ( vol, "moon.gif", ADF_FILE_MODE_READ );
    if (!file) return 1;
    out = fopen("moon_gif","wb");
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
    adfCloseDev(hd);

    adfEnvCleanUp();

    return 0;
}
