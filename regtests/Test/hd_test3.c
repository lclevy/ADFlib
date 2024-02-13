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
    struct AdfVolume *vol, *vol2;
    struct Partition part1;
    struct Partition part2;
    struct Partition **partList;

    adfEnvInitDefault();

    const char tmpdevname[] = "hd_test3-newdev";

    /* an harddisk, "b"=7.5Mb, "h"=74.5mb */

    struct AdfDevice * hd = adfDevCreate ( "dump", tmpdevname, 980, 10, 17 );
    if (!hd) {
        fprintf(stderr, "can't mount device\n");
        adfEnvCleanUp(); exit(1);
    }

    adfDevInfo(hd);

    partList = (struct Partition**)malloc(sizeof(struct Partition*)*2);
    if (!partList) exit(1);

    part1.startCyl =2;
	part1.lenCyl = 100;
	part1.volName = strdup("b");
    part1.volType = FSMASK_FFS|FSMASK_DIRCACHE;
	
    part2.startCyl =101;
	part2.lenCyl = 878;
	part2.volName = strdup("h");
    part2.volType = FSMASK_FFS;

    partList[0] = &part1;
    partList[1] = &part2;

    RETCODE rc = adfCreateHd ( hd, 2, (const struct Partition * const * const) partList );
    free(partList);
    free(part1.volName);
    free(part2.volName);
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
    vol2 = adfVolMount ( hd, 1, ADF_ACCESS_MODE_READWRITE );
    if (!vol2) {
        adfUnMount(vol);
        adfDevUnMount( hd );
        adfDevClose ( hd );
        fprintf(stderr, "can't mount volume\n");
        adfEnvCleanUp(); exit(1);
    }

    adfVolumeInfo(vol);
    adfVolumeInfo(vol2);

    adfUnMount(vol);
    adfUnMount(vol2);
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

    adfDevUnMount ( hd );
    adfDevClose ( hd );

    adfEnvCleanUp();
    return 0;
}
