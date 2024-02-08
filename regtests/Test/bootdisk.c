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
    if ( argc < 2 ) {
        fprintf ( stderr,
                  "required parameter (bootcode file) absent - aborting...\n");
        return 1;
    }
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

    /* create and mount one device */
    hd = adfCreateDev ( "dump", "bootdisk-newdev", 80, 2, 11 );
    if (!hd) {
        fprintf(stderr, "can't mount device\n");
        adfEnvCleanUp(); exit(1);
    }

    adfDeviceInfo(hd);

    if (adfCreateFlop( hd, "empty", FSMASK_FFS|FSMASK_DIRCACHE )!=RC_OK) {
		fprintf(stderr, "can't create floppy\n");
        adfUnMountDev(hd);
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

	adfInstallBootBlock(vol, bootcode);

    adfVolumeInfo(vol);

    adfUnMount(vol);
    adfUnMountDev(hd);
    adfCloseDev(hd);

    adfEnvCleanUp();

    return 0;
}
