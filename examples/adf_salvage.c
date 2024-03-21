
#include <stdio.h>

#include "adflib.h"

char * getBlockTypeStr ( const int block2ndaryType );


int main ( const int          argc,
           const char * const argv[] )
{

    if ( argc < 2 )
          exit(1); 
    const char * const adfDevName    = argv[1];

    int status = 0;

    adfEnvInitDefault();
    adfEnvSetProperty ( ADF_PR_USEDIRC, true );
 
    struct AdfDevice * const dev = adfDevOpen ( adfDevName, ADF_ACCESS_MODE_READONLY );
    if ( dev == NULL ) {
        fprintf ( stderr, "Error opening device '%s' - aborting...\n",
                  adfDevName );
        status = 1;
        goto clean_up_env;
    }

    ADF_RETCODE rc = adfDevMount ( dev );
    if ( rc != ADF_RC_OK ) {
        fprintf ( stderr, "Error mounting device '%s' - aborting...\n",
                  adfDevName );
        status = 2;
        goto clean_up_dev_close;
    }

    //adfDevInfo ( dev );

    struct AdfVolume * const vol = adfVolMount ( dev, 0, ADF_ACCESS_MODE_READONLY );
    if ( vol == NULL ) {
        fprintf ( stderr, "Error mounting volume %d of '%s' - aborting...\n",
                  0, adfDevName );
        status = 3;
        goto clean_up_dev_unmount;
    }

    struct AdfList *list, *cell;
    cell = list = adfGetDelEnt(vol);
    if ( cell != NULL ) {
        printf ( "\nDeleted entries found:\n"
                 "%26sname     block type       2nd type   sector\n", "" );
    } else {
        printf ("No deleted entries found.\n" );
        goto clean_up_volume;
    }
    unsigned nDeletedEntries = 0;
    while ( cell != NULL ) {
        struct GenBlock * const block = (struct GenBlock *) cell->content;
        printf ( "%30s%15d%12s%3d%9d\n",
                 block->name,
                 block->type,
                 getBlockTypeStr ( block->secType ),
                 block->secType,
                 block->sect );
        cell = cell->next;
        nDeletedEntries++;
    }
    adfFreeDelList ( list );
    printf ( "\nNumber of deleted entries: %u\n", nDeletedEntries );


clean_up_volume:
    adfVolUnMount ( vol );

clean_up_dev_unmount:
    adfDevUnMount ( dev );

clean_up_dev_close:
    adfDevClose ( dev );

clean_up_env:
    adfEnvCleanUp();

    return status;
}


char * getBlockTypeStr ( const int block2ndaryType )
{
    switch ( block2ndaryType ) {
    case ADF_ST_ROOT:       return "root";        
    case ADF_ST_DIR:        return "dir";
    case ADF_ST_FILE:       return "file";
    case ADF_ST_LFILE:      return "file link";
    case ADF_ST_LDIR:       return "dir link";
    case ADF_ST_LSOFT:      return "soft link";
    case ADF_T_LIST:        return "t list";
    case ADF_T_DATA:        return "t data";
    case ADF_T_DIRC:        return "t dirc";
    }
    return "unknown";
}
