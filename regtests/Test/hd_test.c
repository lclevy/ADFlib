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
    if ( argc < 2 ) {
        fprintf ( stderr,
                  "required parameter (image/device) absent - aborting...\n");
        return 1;
    }

    struct AdfVolume *vol, *vol2;

    /* initialisation */
    adfEnvInitDefault();

    /*** a real harddisk ***/
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
    adfDeviceInfo(hd);

    /* mount the 2 partitions */
    vol = adfMount ( hd, 0, ADF_ACCESS_MODE_READWRITE );
    if (!vol) {
        adfUnMountDev(hd);
        adfCloseDev(hd);
        fprintf(stderr, "can't mount volume 0\n");
        adfEnvCleanUp(); exit(1);
    }
    adfVolumeInfo(vol);

    vol2 = adfMount(hd, 1, ADF_ACCESS_MODE_READWRITE );
    if (!vol2) {
        adfUnMountDev(hd);
        adfCloseDev(hd);
        fprintf(stderr, "can't mount volume 1\n");
        adfEnvCleanUp(); exit(1);
    }
    adfVolumeInfo(vol2);

    /* unmounts */
    adfUnMount(vol);
    adfUnMount(vol2);
    adfUnMountDev(hd);
    adfCloseDev(hd);


    /*** a dump of a zip disk ***/
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
    adfDeviceInfo(hd);

    vol = adfMount(hd, 0, ADF_ACCESS_MODE_READWRITE );
    if (!vol) {
        adfUnMountDev(hd);
        adfCloseDev(hd);
        fprintf(stderr, "can't mount volume\n");
        adfEnvCleanUp(); exit(1);
    }
    adfVolumeInfo(vol);

    adfUnMount(vol);
    adfUnMountDev(hd);
    adfCloseDev(hd);

    /* clean up */
    adfEnvCleanUp();

    return 0;
}
