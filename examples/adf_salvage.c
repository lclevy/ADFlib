
#include <stdio.h>

#include "adflib.h"

void showDeletedEntries ( struct AdfVolume * const     vol,
                          const struct AdfList * const list );
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

    struct AdfList * list = adfGetDelEnt ( vol );
    showDeletedEntries ( vol, list );
    //if ( list == NULL )
    //    goto clean_up_volume;
    adfFreeDelList ( list );

//clean_up_volume:
    adfVolUnMount ( vol );

clean_up_dev_unmount:
    adfDevUnMount ( dev );

clean_up_dev_close:
    adfDevClose ( dev );

clean_up_env:
    adfEnvCleanUp();

    return status;
}


void showDeletedEntries (  struct AdfVolume * const     vol,
                           const struct AdfList * const list )
{
    if ( list == NULL ) {
        printf ( "No deleted entries found.\n" );
        return;
    }
    const char * const line =
        "-----------------------------------------------------------||-------------------------------\n";
    printf (
        //"\nDeleted entries found:\n%s"
        "%s"
        "                     deleted entry                         ||          parent\n"
        "%s"
        "               name               |     type      | sector || sector |     type      | name\n"
        "%s", line, line, line );
    unsigned nDeletedEntries = 0;
    for ( const struct AdfList * cell = list ; cell != NULL ; cell = cell->next ) {
        struct GenBlock * const block = (struct GenBlock *) cell->content;

        struct AdfEntryBlock parent;
        ADF_RETCODE rc = adfReadEntryBlock ( vol, block->parent, &parent );
        const char * const parentName =
            ( rc == ADF_RC_OK ) ? parent.name : "error getting entry name";

        printf ( "%33s | %9s,%3d | %6d || %6d | %9s,%3d | %s\n",
                 block->name,
                 //block->type,
                 getBlockTypeStr ( block->secType ),
                 block->secType,
                 block->sect,
                 block->parent,
                 getBlockTypeStr ( parent.secType ),
                 parent.secType,
                 parentName );
        nDeletedEntries++;
    }
    printf ( "%s\nNumber of deleted entries: %u\n", line, nDeletedEntries );
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
