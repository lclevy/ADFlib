/*
 * dir_test.c
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include"adflib.h"
#include "adf_dir.h"

int test_chdir_hlink ( struct AdfVolume * vol,
                       char *             hlink,
                       int                num_entries );

int test_softlink_realname ( struct AdfVolume * vol,
                             char *             slink,
                             char *             expected_dest_name );


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
    struct AdfList *list, *cell;
 
    adfEnvInitDefault();

//	adfEnvSetFct(0,0,MyVer,0);

    /* open and mount existing device */
/* testffs.adf */
    struct AdfDevice * hd = adfDevOpen ( argv[1], ADF_ACCESS_MODE_READWRITE );
    if ( ! hd ) {
        fprintf ( stderr, "Cannot open file/device '%s' - aborting...\n",
                  argv[1] );
        adfEnvCleanUp();
        exit(1);
    }

    ADF_RETCODE rc = adfDevMount ( hd );
    if ( rc != RC_OK ) {
        fprintf(stderr, "can't mount device\n");
        adfDevClose ( hd );
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

    cell = list = adfGetDirEnt(vol,vol->curDirPtr);
    while(cell) {
        adfEntryPrint ( cell->content );
        cell = cell->next;
    }
    adfFreeDirList(list);

    putchar('\n');

    /* cd dir_2 */
    //SECTNUM nSect = adfChangeDir(vol, "dir_2");
    adfChangeDir(vol, "dir_2");

    cell = list = adfGetDirEnt(vol,vol->curDirPtr);
    while(cell) {
        adfEntryPrint ( cell->content );
        cell = cell->next;
    }
    adfFreeDirList(list);

    putchar('\n');

    /* cd .. */
    adfParentDir(vol);

    cell = list = adfGetDirEnt(vol,vol->curDirPtr);
    while(cell) {
        adfEntryPrint ( cell->content );
        adfFreeEntry(cell->content);
        cell = cell->next;
    }
    freeList(list);

    int status = 0;

    /* cd hlink_dir1 (hardlink to dir_1) */
    status += test_chdir_hlink ( vol, "hlink_dir1", 1 );

    /* cd hlink_dir2 (hardlink to dir_2) */
    status += test_chdir_hlink ( vol, "hlink_dir2", 2 );

    /* test getting real name from a softlink */
    status += test_softlink_realname ( vol, "slink_dir1", "dir_1" );

    adfVolUnMount(vol);
    adfDevUnMount ( hd );
    adfDevClose ( hd );

    adfEnvCleanUp();

    return status;
}


int test_chdir_hlink ( struct AdfVolume * vol,
                       char *             hlink,
                       int                num_entries )
{
    int status = 0;

    adfToRootDir ( vol );

    printf ("*** Test entering hard link %s\n", hlink );
    ADF_RETCODE rc = adfChangeDir ( vol, hlink );
    if ( rc != RC_OK ) {
        fprintf ( stderr, "adfChangeDir error entering hard link %s.\n",
                  hlink );
        status++;
    }

    struct AdfList * list, * cell;
    list = cell = adfGetDirEnt ( vol, vol->curDirPtr );
    int count = 0;
    while ( cell ) {
        //adfEntryPrint ( list->content );
        cell = cell->next;
        count++;
    }
    adfFreeDirList ( list );

    if ( count != num_entries ) {
        fprintf ( stderr, "Incorrect number of entries (%d) after chdir to hard link %s.\n",
                  count, hlink );
        status++;
    }

    adfToRootDir ( vol );

    return status;
}


int test_softlink_realname ( struct AdfVolume * vol,
                             char *             slink,
                             char *             expected_dest_name )
{
    adfToRootDir ( vol );

    printf ("*** Test getting destination name for soft link %s\n", slink );

    struct AdfLinkBlock entry;
    SECTNUM sectNum = adfGetEntryByName ( vol, vol->curDirPtr, slink,
                                          (struct AdfEntryBlock *) &entry );
    if ( sectNum == -1 ) {
        return 1;
    }

    if ( entry.secType != ADF_ST_LSOFT ) {
        return 1;
    }

    if ( strncmp ( expected_dest_name, entry.realName, 6 ) != 0 ) {
        fprintf ( stderr,
                  "Name of the softlink %s incorrect: read '%s' != expected '%s'\n",
                  slink, entry.realName, expected_dest_name );
        return 1;
    }
    printf (" -> OK!\n");
    return 0;
}
