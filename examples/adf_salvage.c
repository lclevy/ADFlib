/*
 * adf_salvage
 *
 * show and restore deleted files on Amiga disk images (ADF)
 *
 *  This file is part of ADFLib.
 *
 *  ADFLib is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  ADFLib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Foobar; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <adflib.h>
#include "adf_dev.h"
#include "adf_dev_flop.h"
#include <errno.h>

#include <stdbool.h>
#include <stdio.h>

#ifdef WIN32
#include "getopt.h"
#else
#include <libgen.h>
#include <unistd.h>
#endif

typedef struct CmdlineOptions {
    char     *adfDevName;
    unsigned  volidx,
             *entrySectors;
    bool      verbose,
              help,
              version;
} CmdlineOptions;


bool parse_args ( const int * const    argc,
                  char * const * const argv,
                  CmdlineOptions *     options );

void showDeletedEntries ( struct AdfVolume * const     vol,
                          const struct AdfList * const list );
char * getBlockTypeStr ( const int block2ndaryType );


void usage ( void )
{
    printf ( "\nUsage:  adf_salvage  [-p volume] adf_device\n\n"
             "Salvage files from an ADF (Amiga Disk File) or an HDF (Hard Disk File) volume.\n\n"
             "Options:\n"
             "  -p volume  volume/partition index, counting from 0, default: 0\n"
             "  -v         be more verbose\n\n"
             "  -h         show help\n"
             "  -V         show version\n\n" );
}


int main ( const int     argc,
           char * const argv[] )
{
    CmdlineOptions options;
    if ( ! parse_args ( &argc, argv, &options ) ) {
        fprintf ( stderr, "Usage info:  adf_salvage -h\n" );
        exit ( EXIT_FAILURE );
    }

    if ( options.help ) {
        usage();
        exit ( EXIT_SUCCESS );
    }

    if ( options.version ) {
        printf ( "%s (%s), powered by ADFlib %s (%s)\n",
                 ADFLIB_VERSION, ADFLIB_DATE,
                 adfGetVersionNumber(), adfGetVersionDate() );
        exit ( EXIT_SUCCESS );
    }

    int status = 0;

    adfEnvInitDefault();
    adfEnvSetProperty ( ADF_PR_USEDIRC, true );
 
    struct AdfDevice * const dev = adfDevOpen ( options.adfDevName, ADF_ACCESS_MODE_READONLY );
    if ( dev == NULL ) {
        fprintf ( stderr, "Error opening device '%s' - aborting...\n",
                  options.adfDevName );
        status = 1;
        goto clean_up_env;
    }

    ADF_RETCODE rc = adfDevMount ( dev );
    if ( rc != ADF_RC_OK ) {
        fprintf ( stderr, "Error mounting device '%s' - aborting...\n",
                  options.adfDevName );
        status = 2;
        goto clean_up_dev_close;
    }

    //adfDevInfo ( dev );

    struct AdfVolume * const vol = adfVolMount ( dev, (int) options.volidx,
                                                 ADF_ACCESS_MODE_READONLY );
    if ( vol == NULL ) {
        fprintf ( stderr, "Error mounting volume %d of '%s' - aborting...\n",
                  options.volidx, options.adfDevName );
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


/* return value: true - valid, false - invalid */
bool parse_args ( const int * const    argc,
                  char * const * const argv,
                  CmdlineOptions *     options )
{
    // set default options
    memset ( options, 0, sizeof ( CmdlineOptions ) );
    options->volidx       = 0;
    options->entrySectors = NULL;

    options->verbose =
    options->help    =
    options->version = false;

    const char * valid_options = "p:hvV";
    int opt;
    while ( ( opt = getopt ( *argc, (char * const *) argv, valid_options ) ) != -1 ) {
//        printf ( "optind %d, opt %c, optarg %s\n", optind, ( char ) opt, optarg );
        switch ( opt ) {
        case 'p': {
            // partition (volume) number (index)
            char * endptr = NULL;
            options->volidx = ( unsigned int ) strtoul ( optarg, &endptr, 10 );
            if ( endptr == optarg ||
                 options->volidx > 255 )  // is there a limit for max. number of partitions???
            {
                fprintf ( stderr, "Invalid volume/partition %u.\n", options->volidx );
                return false;
            }
            continue;
        }

        case 'v':
            options->verbose = true;
            continue;

        case 'h':
            options->help = true;
            return true;

        case 'V':
            options->version = true;
            return true;

        default:
            return false;
        }
    }

    if ( optind != *argc - 1 ) {
        fprintf ( stderr, "Missing the name of an adf file/device.\n" );
        return false;
    }

    options->adfDevName = argv [ optind ];
    return true;
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
