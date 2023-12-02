/*
 * bootdisk.c
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
    FILE* boot;
    unsigned char bootcode[1024];
 
    boot=fopen(argv[1],"rb");
    if (!boot) {
        fprintf(stderr, "can't mount volume\n");
        exit(1);
    }
    fread(bootcode, sizeof(unsigned char), 1024, boot);
    fclose(boot);

    adfEnvInitDefault();

    hd = adfMountDev ( argv[2], ADF_ACCESS_MODE_READWRITE );
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

	adfInstallBootBlock(vol, bootcode);

    adfVolumeInfo(vol);

    adfUnMount(vol);
    adfUnMountDev(hd);

    adfEnvCleanUp();

    return 0;
}
