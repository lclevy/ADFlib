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
    (void) argc, (void) argv;
    struct AdfVolume *vol;
    struct Partition part1;
    struct Partition **partList;

    adfEnvInitDefault();

    const char tmpdevname[] = "hd_test2-newdev";

    /* a zip disk */
    struct AdfDevice * hd = adfDevCreate ( "dump", tmpdevname, 2891, 1, 68 );
    if (!hd) {
        fprintf(stderr, "can't mount device\n");
        adfEnvCleanUp(); exit(1);
    }

    adfDevInfo(hd);

    partList = (struct Partition**)malloc(sizeof(struct Partition*));
    if (!partList) exit(1);

    part1.startCyl = 2;
	part1.lenCyl = 2889;
	part1.volName = strdup("zip");
    part1.volType = FSMASK_FFS|FSMASK_DIRCACHE;

    partList[0] = &part1;
    RETCODE rc = adfCreateHd ( hd, 1, (const struct Partition * const * const) partList );
    free(partList);
    free(part1.volName);
    if ( rc != RC_OK ) {
        adfDevUnMount ( hd );
        adfDevClose ( hd );
        fprintf ( stderr, "adfCreateHd returned error %d\n", rc );
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
    adfVolUnMount(vol);
    adfDevUnMount ( hd );
    adfDevClose ( hd );

    /* mount the created device */
    hd = adfDevOpen ( tmpdevname, ADF_ACCESS_MODE_READWRITE );
    if ( ! hd ) {
        fprintf ( stderr, "Cannot open file/device '%s' - aborting...\n",
                  tmpdevname );
        adfEnvCleanUp();
        exit(1);
    }

    rc = adfDevMount ( hd );
    if ( rc != RC_OK ) {
        adfDevClose ( hd );
        fprintf(stderr, "can't mount device\n");
        adfEnvCleanUp(); exit(1);
    }

    adfDevInfo(hd);

    vol = adfVolMount ( hd, 0, ADF_ACCESS_MODE_READWRITE );
    if (!vol) {
        adfDevUnMount ( hd );
        adfDevClose ( hd );
        fprintf(stderr, "can't mount volume\n");
        adfEnvCleanUp(); exit(1);
    }

    adfVolInfo(vol);

    adfVolUnMount(vol);
    adfDevUnMount ( hd );
    adfDevClose ( hd );

    adfEnvCleanUp();

    return 0;
}
