
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "adf_show_metadata_volume.h"
#include "adf_show_metadata_dir.h"
#include "adf_show_metadata_file.h"


static void show_device_metadata ( struct Device * const dev );    

static void show_dentry_metadata ( struct Volume * const vol,
                                   char * const          path );

void usage ( void )
{
    printf ( "Usage:  adf_show_metadata filename.adf/device filename/dirname\n\n" );
}


int main ( int     argc,
           char ** argv )
{
    if ( argc < 2 ) {
        usage();
        return 1;
    }
    char * const adfname = argv[1];
    char * const path = ( argc == 3 ) ? argv[2] : NULL;

    int status = 0;

    adfEnvInitDefault();

    printf ( "\nOpening image/device:\t'%s'\n", adfname );
    struct Device * const dev = adfMountDev ( adfname, TRUE );
    if ( ! dev ) {
        fprintf ( stderr, "Cannot open file/device '%s' - aborting...\n",
                  adfname );
        status = 1;
        goto env_cleanup;
    }

    int vol_id = 0;
    struct Volume * const vol = adfMount ( dev, vol_id, 1 );
    if ( ! vol ) {
        fprintf ( stderr, "Cannot mount volume %d - aborting...\n",
                  vol_id );
        status = 1;
        goto dev_cleanup;
    }
    printf ( "Mounted volume:\t\t%d\n", vol_id );

    if ( path ) {
        //printf ( "Show file / dirtory info\n" );
        show_dentry_metadata ( vol, path );
    } else {
        show_device_metadata ( dev );
        show_volume_metadata ( vol );
    }

    adfUnMount ( vol );

dev_cleanup:
    adfUnMountDev ( dev );
env_cleanup:
    adfEnvCleanUp();

    return status;
}


static void show_device_metadata ( struct Device * const dev )
{
    adfDeviceInfo ( dev );
}


static void show_dentry_metadata ( struct Volume * const vol,
                                   char * const          path )
{
    printf ( "\nPath:\t\t%s\n", path );
    const char * path_relative = path;

    // skip all leading '/' from the path
    while ( *path_relative == '/' )
        path_relative++;

    if ( *path_relative == '\0' ) { // root directory
        printf ("\nVolume's root directory.\n");
        show_directory_metadata ( vol, vol->curDirPtr );
        return;
    }
    char * dirpath_buf = strdup ( path_relative );
    char * dir_path = dirname ( dirpath_buf );
    char * entryname_buf = strdup ( path );
    char * entry_name = basename ( entryname_buf );

    printf ( "Directory:\t%s\n", dir_path );
    if ( strcmp ( dir_path, "." ) != 0 ) {
        if ( adfChangeDir ( vol, dir_path ) != RC_OK ) {
            fprintf ( stderr, "Invalid dir: '%s'\n", dir_path );
            goto show_entry_cleanup;
        }
    }

    // get parent
    struct bEntryBlock parent;
    if ( adfReadEntryBlock ( vol, vol->curDirPtr, &parent ) != RC_OK ) {
        fprintf ( stderr, "Error reading parent entry block (%d)\n",
                  vol->curDirPtr );
        goto show_entry_cleanup;
    }

    // get entry
    struct bEntryBlock entry;
    SECTNUM nUpdSect;
    SECTNUM sectNum = adfNameToEntryBlk ( vol, parent.hashTable, entry_name,
                                          ( struct bEntryBlock * ) &entry,
                                          &nUpdSect );
    if ( sectNum == -1 ) {
        fprintf (stderr, "'%s' not found.\n", entry_name );
        goto show_entry_cleanup;
    }

    switch ( entry.secType ) {
    case ST_ROOT:
        fprintf ( stderr, "Querying root directory?\n" );
        break;
    case ST_DIR:
        show_directory_metadata ( vol, sectNum );
        break;
    case ST_FILE:
        show_file_metadata ( vol, sectNum );
        break;
    case ST_LFILE:
        //show_hardlink_metadata ( vol, sectNum );
        break;
    case ST_LDIR:
        //show_hardlink_metadata ( vol, sectNum );
        break;
    case ST_LSOFT:
        break;
    default:
        fprintf ( stderr, "unknown entry type %d\n", entry.secType );
        goto show_entry_cleanup;
    }

show_entry_cleanup:
    free ( entryname_buf );
    free ( dirpath_buf );
}
