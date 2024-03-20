/*
 *  undel2.c
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
int main ( const int          argc,
           const char * const argv[] )
{
    struct AdfList *list, *cell;
    struct GenBlock *block;
    unsigned char buf[600];
    int status = 0;

    if ( argc < 3 )
        exit(10);
    const char * const adfDevName    = argv[1];
    const char * const fileToRecover = argv[2]; // "mod.and.distantcall";

    adfEnvInitDefault();

    adfEnvSetProperty ( ADF_PR_USEDIRC, true );
 
    struct AdfDevice * const hd = adfDevOpen ( adfDevName, ADF_ACCESS_MODE_READWRITE );
    if ( ! hd ) {
        fprintf ( stderr, "Cannot open file/device '%s' - aborting...\n",
                  adfDevName );
        status = 1;
        goto clean_up_env;
    }

    ADF_RETCODE rc = adfDevMount ( hd );
    if ( rc != ADF_RC_OK ) {
        fprintf(stderr, "can't mount device\n");
        status = 2;
        goto clean_up_dev_close;
    }

    adfDevInfo ( hd );

    struct AdfVolume * const vol = adfVolMount ( hd, 0, ADF_ACCESS_MODE_READWRITE );
    if (!vol) {
        fprintf(stderr, "can't mount volume\n");
        status = 3;
        goto clean_up_dev_unmount;
    }

    cell = list = adfGetDirEnt(vol, vol->curDirPtr);
    while(cell) {
        adfEntryPrint ( cell->content );
        cell = cell->next;
    }
    adfFreeDirList(list);
    adfVolInfo(vol);

    printf ( "\nremove %s", fileToRecover );
    adfRemoveEntry(vol,vol->curDirPtr, fileToRecover );
    adfVolInfo(vol);

    cell = list = adfGetDelEnt(vol);
    while(cell) {
        block =(struct GenBlock*) cell->content;
        printf ( "%s %d %d %d\n",
                 block->name,
                 block->type,
                 block->secType,
                 block->sect );
        cell = cell->next;
    }
    adfFreeDelList(list);

    printf ( "\nundel %s", fileToRecover );
    rc = adfCheckEntry ( vol, 886, 0 );
    if ( rc != ADF_RC_OK ) {
        fprintf (stderr, "adfCheckEntry error %d\n", rc );
        status = 4;
        goto clean_up_volume;
    }
    rc = adfUndelEntry ( vol, vol->curDirPtr, 886 );
    if ( rc != ADF_RC_OK ) {
        fprintf (stderr, "adfUndelEntry error %d\n", rc );
        status = 5;
        goto clean_up_volume;
    }

    adfVolInfo(vol);

    cell = list = adfGetDirEnt(vol, vol->curDirPtr);
    while(cell) {
        adfEntryPrint ( cell->content );
        cell = cell->next;
    }
    adfFreeDirList(list);

    struct AdfFile * const file = adfFileOpen ( vol, fileToRecover,
                                                ADF_FILE_MODE_READ );
    if ( file == NULL ) {
        status = 6;
        goto clean_up_volume;
    }

    FILE * const out = fopen ( fileToRecover, "wb" );
    if ( out == NULL ) {
        status = 7;
        goto clean_up_file_adf;
    }

    unsigned len = 600;
    unsigned n = adfFileRead ( file, len, buf );
    while(!adfEndOfFile(file)) {
        fwrite(buf,sizeof(unsigned char),n,out);
        n = adfFileRead ( file, len, buf );
        if ( n != len && ! adfEndOfFile ( file ) ) {
            fprintf ( stderr, "adfFileRead: error reading %s at %u (device: %s)\n",
                      fileToRecover, adfFileGetPos ( file ), adfDevName );
            status = 8;
            goto clean_up_file_local;
        }
    }
    if (n>0)
        fwrite(buf,sizeof(unsigned char),n,out);

clean_up_file_local:
    fclose(out);

clean_up_file_adf:
    adfFileClose ( file );

clean_up_volume:
    adfVolUnMount(vol);

clean_up_dev_unmount:
    adfDevUnMount ( hd );

clean_up_dev_close:
    adfDevClose ( hd );

clean_up_env:
    adfEnvCleanUp();

    return status;
}
