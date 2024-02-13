/*
 *  undel3.c
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
    (void) argc;
    struct AdfVolume *vol;
    struct AdfList *list, *cell;
    struct GenBlock *block;
    BOOL true = TRUE;
    struct AdfFile *file;
    unsigned char buf[600];
    FILE *out;
  
    adfEnvInitDefault();

    adfChgEnvProp(PR_USEDIRC,&true);
 
    struct AdfDevice * hd = adfDevOpen ( argv[1], ADF_ACCESS_MODE_READWRITE );
    if ( ! hd ) {
        fprintf ( stderr, "Cannot open file/device '%s' - aborting...\n",
                  argv[1] );
        adfEnvCleanUp();
        exit(1);
    }

    RETCODE rc = adfDevMount ( hd );
    if ( rc != RC_OK ) {
        fprintf(stderr, "can't mount device\n");
        adfDevClose ( hd );
        adfEnvCleanUp(); exit(1);
    }

    adfDevInfo ( hd );

    vol = adfVolMount ( hd, 0, ADF_ACCESS_MODE_READWRITE );
    if (!vol) {
        adfDevUnMount ( hd );
        adfDevClose ( hd );
        fprintf(stderr, "can't mount volume\n");
        adfEnvCleanUp(); exit(1);
    }

    cell = list = adfGetDirEnt(vol, vol->curDirPtr);
    while(cell) {
        adfEntryPrint ( cell->content );
        cell = cell->next;
    }
    adfFreeDirList(list);
    adfVolumeInfo(vol);

    puts("\nremove MOON.GIF");
    adfRemoveEntry(vol,vol->curDirPtr,"MOON.GIF");
    adfVolumeInfo(vol);

    cell = list = adfGetDelEnt(vol);
    while(cell) {
        block =(struct GenBlock*) cell->content;
       printf("%s %d %d %d\n",block->name,block->type,block->secType,
            block->sect);
        cell = cell->next;
    }
    adfFreeDelList(list);

    adfCheckEntry(vol,884,0);
    adfUndelEntry(vol,vol->curDirPtr,884);
    puts("\nundel MOON.GIF");
    adfVolumeInfo(vol);

    cell = list = adfGetDirEnt(vol, vol->curDirPtr);
    while(cell) {
        adfEntryPrint ( cell->content );
        cell = cell->next;
    }
    adfFreeDirList(list);

    file = adfFileOpen ( vol, "MOON.GIF", ADF_FILE_MODE_READ );
    if (!file) return 1;
    out = fopen("moon_gif","wb");
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

    adfUnMount(vol);
    adfDevUnMount ( hd );
    adfDevClose ( hd );

    adfEnvCleanUp();

    return 0;
}
