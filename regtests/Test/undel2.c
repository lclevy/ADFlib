/*
 *  undel2.c
 */

#include <errno.h>
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
    int status = 0;

    if ( argc < 4 )
        exit(10);
    const char * const adfDevName    = argv[1];
    const char * const fileToRecover = argv[2]; // "mod.and.distantcall";

    char *endptr;
    errno = 0;
    const ADF_SECTNUM fileHeaderSector = (ADF_SECTNUM) strtol ( argv[3], &endptr, 10 );
    if ( errno != 0 ) {
        perror("strtol");
        exit(11);
    }
    if ( fileHeaderSector < 2 )
        exit(12);

    adfEnvInitDefault();

    adfEnvSetProperty ( ADF_PR_USEDIRC, true );
 
    struct AdfDevice * const hd = adfDevOpen ( adfDevName, ADF_ACCESS_MODE_READWRITE );
    if ( ! hd ) {
        fprintf ( stderr, "Cannot open file/device '%s' - aborting...\n",
                  adfDevName );
        status = 1;
        goto clean_up_env;
    }

    if ( (unsigned) fileHeaderSector > hd->cylinders * hd->heads * hd->sectors )
        exit(12);

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

    struct AdfList *list, *cell;
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
    if (cell)
        puts ( "Found deleted entries:" );
    else {
        fprintf ( stderr, "No deleted entries found! -> ERROR.\n" );
        status = 4;
        goto clean_up_volume;
    }
    while(cell) {
        struct GenBlock * const block = (struct GenBlock *) cell->content;
        printf ( "name %s, block type %d, 2nd type %d, sector %d\n",
                 block->name,
                 block->type,
                 block->secType,
                 block->sect );
        cell = cell->next;
    }
    adfFreeDelList(list);

    printf ( "\nundel %s at %d", fileToRecover, fileHeaderSector );
    rc = adfCheckEntry ( vol, fileHeaderSector, 0 );
    if ( rc != ADF_RC_OK ) {
        fprintf (stderr, "adfCheckEntry error %d\n", rc );
        status = 5;
        goto clean_up_volume;
    }
    rc = adfUndelEntry ( vol, vol->curDirPtr, fileHeaderSector );
    if ( rc != ADF_RC_OK ) {
        fprintf (stderr, "adfUndelEntry error %d\n", rc );
        status = 6;
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
        status = 7;
        goto clean_up_volume;
    }

    FILE * const out = fopen ( fileToRecover, "wb" );
    if ( out == NULL ) {
        status = 8;
        goto clean_up_file_adf;
    }

    unsigned char buf[600];
    const unsigned len = sizeof(buf) / sizeof(unsigned char);

    unsigned
        fileSizeInHeader   = file->fileHdr->byteSize,
        fileSizeCalculated = 0;

    while(!adfEndOfFile(file)) {
        unsigned n = adfFileRead ( file, len, buf );
        if ( n != len && ! adfEndOfFile ( file ) ) {
            fprintf ( stderr, "adfFileRead: error reading %s at %u (device: %s)\n",
                      fileToRecover, adfFileGetPos ( file ), adfDevName );
            status = 9;
            goto clean_up_file_local;
        }
        fwrite(buf,sizeof(unsigned char),n,out);
        fileSizeCalculated += n;
    }

    if ( fileSizeInHeader != fileSizeCalculated ) {
        fprintf ( stderr, "file size error: size in file header block %u != size read %u"
                  " (device '%s', file '%s')\n",
                  fileSizeInHeader, fileSizeCalculated, adfDevName,
                  file->fileHdr->fileName );
        status = 10;
    }

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
