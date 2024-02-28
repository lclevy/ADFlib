
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32) && !defined(_CYGWIN)
char* dirname(char* path);
char* basename(char* path);
#else
#include <libgen.h>
#endif

#include "adf_show_metadata_volume.h"
#include "adf_show_metadata_dir.h"
#include "adf_show_metadata_file.h"


struct args {
    const char * adfname;
    int          vol_id;
    const char * path;
};

int parse_args ( const int                  argc,
                 const char * const * const argv,
                 struct args * const        args );

void show_device_metadata ( struct AdfDevice * const dev );

void show_dentry_metadata ( struct AdfVolume * const vol,
                            const char * const       path );

void usage ( void )
{
    printf ( "\nadf_show_metadata - show metadata of an adf device or a file/directory\n\n"
             "Usage:  adf_show_metadata adf_device [vol] [path]\n\n"
             "where:\n  adf_device - an adf file (image) or a native (real) device\n"
             "  vol        - (optional) partition/volume number\n"
             "  path       - (optional) a file/directory inside the ADF device\n\n"
             "(using adflib version %s)\n", adfGetVersionNumber() );
}


int main ( const int                  argc,
           const char * const * const argv )
{
    int status = 0;
    struct args args;
    if ( ( status = parse_args (argc, argv, &args ) ) != 0 )
        return status;

    adfEnvInitDefault();

    printf ( "\nOpening image/device:\t'%s'\n", args.adfname );
    struct AdfDevice * const dev = adfDevOpen ( args.adfname, ADF_ACCESS_MODE_READONLY );
    if ( ! dev ) {
        fprintf ( stderr, "Cannot open file/device '%s' - aborting...\n",
                  args.adfname );
        status = 1;
        goto env_cleanup;
    }

    ADF_RETCODE rc = adfDevMount ( dev );
    if ( rc != RC_OK ) {
        fprintf ( stderr, "Cannot get volume info for file/device '%s' - aborting...\n",
                  args.adfname );
        goto dev_cleanup;
    }

    if ( args.vol_id < 0 ) {
        show_device_metadata ( dev );
        goto dev_cleanup;
    }

    struct AdfVolume * const vol = adfVolMount ( dev, args.vol_id,
                                                 ADF_ACCESS_MODE_READONLY );
    if ( ! vol ) {
        fprintf ( stderr, "Cannot mount volume %d - aborting...\n",
                  args.vol_id );
        status = 1;
        goto dev_mount_cleanup;
    }
    printf ( "Mounted volume:\t\t%d\n", args.vol_id );

    if ( args.path != NULL ) {
        //printf ( "Show file / dirtory info\n" );
        show_dentry_metadata ( vol, args.path );
    } else {
        show_volume_metadata ( vol );
    }

    adfVolUnMount ( vol );

dev_mount_cleanup:
    adfDevUnMount ( dev );
dev_cleanup:
    adfDevClose ( dev );
env_cleanup:
    adfEnvCleanUp();

    return status;
}


int parse_args ( const int                  argc,
                 const char * const * const argv,
                 struct args * const        args )
{
    if ( argc < 2 ) {
        usage();
        return 1;
    }

    args->adfname = argv[1];

    if ( argc < 3 ) {
        args->vol_id = -1;
        return 0;
    }

    char * endptr;
    long vol_id_l = strtol (argv[2], &endptr, 10);
    if ( *endptr != 0 ) {
        fprintf ( stderr, "Invalid volume '%s'\n", argv[2] );
        return 1;
    }
    args->vol_id = (int) vol_id_l;

    args->path = ( argc >= 4 ) ? argv[3] : NULL;
    return 0;
}


void show_device_metadata ( struct AdfDevice * const dev )
{
    adfDevInfo ( dev );
}


void show_dentry_metadata ( struct AdfVolume * const vol,
                            const char * const       path )
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

    //printf ( "Directory:\t%s\n", dir_path );
    if ( strcmp ( dir_path, "." ) != 0 ) {
        if ( adfChangeDir ( vol, dir_path ) != RC_OK ) {
            fprintf ( stderr, "Invalid dir: '%s'\n", dir_path );
            goto show_entry_cleanup;
        }
    }

    // get entry
    struct AdfEntryBlock entry;
    SECTNUM sectNum = adfGetEntryByName ( vol, vol->curDirPtr,
                                          entry_name, &entry );
    if ( sectNum == -1 ) {
        fprintf (stderr, "'%s' not found.\n", entry_name );
        goto show_entry_cleanup;
    }

    switch ( entry.secType ) {
    case ADF_ST_ROOT:
        fprintf ( stderr, "Querying root directory?\n" );
        break;
    case ADF_ST_DIR:
        show_directory_metadata ( vol, sectNum );
        break;
    case ADF_ST_FILE:
        show_file_metadata ( vol, sectNum );
        break;
    case ADF_ST_LFILE:
        //show_hardlink_metadata ( vol, sectNum );
        break;
    case ADF_ST_LDIR:
        //show_hardlink_metadata ( vol, sectNum );
        break;
    case ADF_ST_LSOFT:
        break;
    default:
        fprintf ( stderr, "unknown entry type %d\n", entry.secType );
        goto show_entry_cleanup;
    }

show_entry_cleanup:
    free ( entryname_buf );
    free ( dirpath_buf );
}


#if defined(_WIN32) && !defined(_CYGWIN)

// custom impl. of POSIX's dirname
// (note that it modifies buffer pointed by path)
char* dirname(char* path)
{
    if (!path)
        return NULL;

    int len = strlen(path);
    if (len < 1)
        return NULL;

    char* last_slash = strrchr(path, '/');
    if (!last_slash) {
        // no slash - no directory in path (only basename)
        path[0] = '.';
        path[1] = '\0';
        return path;
    }

    if (path + len - 1 == last_slash) {
        // slash at the end of the path - remove it
        path[len - 1] = '\0';
    }

    last_slash = strrchr(path, '/');
    if (!last_slash) {
        // no directory before basename
        path[0] = '.';
        path[1] = '\0';
        return path;
    }
    else {
        // cut the last slash and the basename from path
        *last_slash = '\0';
        return path;
    }
}

// custom impl. of POSIX's basename
// (note that it modifies buffer pointed by path)
char* basename(char* path)
{
    if (!path)
        return NULL;

    char* last_slash = strrchr(path, '/');
    if (!last_slash) {
        // no slash - no directory in path (only basename)
        return path;
    }

    int len = strlen(path);
    if (path + len - 1 == last_slash) {
        // slash at the end of the path - remove it
        path[len - 1] = '\0';
    }

    last_slash = strrchr(path, '/');
    if (!last_slash) {
        // no directory before basename
        return path;
    }
    else {
        // the basename starts after the last slash
        return last_slash + 1;
    }
}
#endif
