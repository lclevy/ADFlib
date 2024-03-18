
#include <adflib.h>
#include "adf_dev.h"
#include "adf_dev_flop.h"
#include <errno.h>

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


#ifdef WIN32
#include "getopt.h"
#else
#include <libgen.h>
#include <unistd.h>
#endif

typedef struct CmdlineOptions {
    char     *adfName,
             *label;
    unsigned  volidx;
    uint8_t   fsType;
    bool      force,
              verbose,
              help,
              version;
} CmdlineOptions;


bool parse_args ( const int * const    argc,
                  char * const * const argv,
                  CmdlineOptions *     options );


void usage ( void )
{
    printf ( "\nUsage:  adf_format [-f] [-l label] [-p volume] [-t fstype] adf_device\n\n"
             "Quick-format an ADF (Amiga Disk File) or an HDF (Hard Disk File) volume.\n\n"
             "Options:\n"
             "  -f         force formatting even if a filesystem already present\n"
             "             (WARNING: know what you're doing, irreversible data loss!)\n"
             "  -l label   set volume name/label, default: ""\n"
             "  -p volume  volume/partition index, counting from 0, default: 0\n"
             "  -t fstype  set A. DOS filesystem type (OFS/FFS + INTL, DIR_CACHE)\n"
             "  -v         be more verbose\n\n"
             "  -h         show help\n"
             "  -V         show version\n\n"

             "  fstype can be 0-7: flags = 3 least significant bits\n"
             "         bit  set         clr\n"
             "         0    FFS         OFS\n"
             "         1    INTL ONLY   NO_INTL ONLY\n"
             "         2    DIRC&INTL   NO_DIRC&INTL\n\n" );
}


int main ( const int            argc,
           char * const * const argv )
{
    CmdlineOptions options;
    if ( ! parse_args ( &argc, argv, &options ) ) {
        fprintf ( stderr, "Usage info:  adf_format -h\n" );
        exit ( EXIT_FAILURE );
    }

    if ( options.help ) {
        usage();
        exit ( EXIT_SUCCESS );
    }

    if ( options.version ) {
        printf ( "\n%s (%s), powered by ADFlib %s (%s)\n\n",
                 ADFLIB_VERSION, ADFLIB_DATE,
                 adfGetVersionNumber(), adfGetVersionDate() );
        exit ( EXIT_SUCCESS );
    }

    adfEnvInitDefault();

    struct AdfDevice * const device = adfDevOpen ( options.adfName,
                                                   ADF_ACCESS_MODE_READWRITE );
    if ( device == NULL ) {
        fprintf ( stderr, "Cannot open '%s' - aborting...\n", options.adfName );
        return 1;
    }

    ADF_RETCODE rc = adfDevMount ( device );
    if ( rc != ADF_RC_OK ) {
        fprintf ( stderr, "adfDevMount failed on %s - aborting...\n",
                  options.adfName );
        adfDevClose ( device );
        exit ( EXIT_FAILURE );
    }

    if ( options.verbose )
        adfDevInfo ( device );

    if ( device->nVol <= (int) options.volidx ) {
        fprintf ( stderr, "Invalid volume index %u, %s contains %d volume%s.\n",
                  options.volidx, options.adfName, device->nVol,
                  device->nVol > 1 ? "s" : "");
        adfDevClose ( device );
        exit ( EXIT_FAILURE );
    }

    if ( ! options.force ) {
        if ( adfVolIsFsValid ( device->volList[ options.volidx ] ) ) {
            fprintf ( stderr, "Volume %u of %s already contains a filesystem (%s)"
                      " - aborting... (use -f to enforce formatting anyway)\n",
                      options.volidx, options.adfName,
                      adfVolGetFsStr ( device->volList[ options.volidx ] ) );
            adfDevClose ( device );
            exit ( EXIT_FAILURE );
        }
    }
    adfDevUnMount ( device );

    char *devtype_str;
    if ( device->devType == ADF_DEVTYPE_FLOPDD ) {
        devtype_str     = "DD floppy (880k)";
    } else if ( device->devType == ADF_DEVTYPE_FLOPHD ) {
        devtype_str     = "HD floppy (1760k)";
    } else if ( device->devType == ADF_DEVTYPE_HARDFILE ) {
        devtype_str     = "Hardfile (hdf)";
    } else { //if ( devtype == DEVTYPE_HARDDISK ) {
        fprintf ( stderr, "Devices with RDB (partitioned) are not supported "
                  "(yet...) - aborting...\n" );
        return 1;
    }

    printf ( "Formatting %s '%s', volume %d, DOS fstype %d, label '%s'... ",
             devtype_str, options.adfName, options.volidx, options.fsType, options.label );
    fflush ( stdout );

    rc = ( device->devType == ADF_DEVTYPE_HARDFILE ) ?
        adfCreateHdFile ( device, options.label, options.fsType ) :
        adfCreateFlop   ( device, options.label, options.fsType );
    if ( rc != ADF_RC_OK ) {
        fprintf ( stderr, "Error formatting '%s'!", options.adfName );
        adfDevClose ( device );
        adfEnvCleanUp();
        return 1;
    }
    printf ( "Done!\n" );

    if ( options.verbose )
        adfDevInfo ( device );

    adfDevClose ( device );
    adfEnvCleanUp();

    return 0;
}



/* return value: true - valid, false - invalid */
bool parse_args ( const int * const    argc,
                  char * const * const argv,
                  CmdlineOptions *     options )
{
    // set default options
    memset ( options, 0, sizeof ( CmdlineOptions ) );
    options->fsType  = 1;
    options->volidx  = 0;
    options->label   = "";
    options->force   =
    options->verbose =
    options->help    =
    options->version = false;

    const char * valid_options = "l:p:t:fhvV";
    int opt;
    while ( ( opt = getopt ( *argc, (char * const *) argv, valid_options ) ) != -1 ) {
//        printf ( "optind %d, opt %c, optarg %s\n", optind, ( char ) opt, optarg );
        switch ( opt ) {
        case 'f': {
            options->force = true;
            continue;
        }
        case 'l': {
            options->label = optarg;
            size_t labelLen = strlen ( options->label );
            if ( labelLen > ADF_MAX_NAME_LEN ) {
                fprintf ( stderr, "Label '%s' is too long (%ld > max. %d characters).\n",
                          options->label, labelLen, ADF_MAX_NAME_LEN );
                return false;
            }
            continue;
        }

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

        case 't': {
            // filesystem type (subtype - just the number id in 'DOSn' signature)
            char * endptr = NULL;
            options->fsType = ( uint8_t ) strtoul ( optarg, &endptr, 10 );
            if ( endptr == optarg ||
                 options->fsType > 7 ||
                 ( options->fsType > 0 &&
                   options->fsType % 2 == 0 ) )
            {
                fprintf ( stderr, "Invalid filesystem type %u.\n", options->fsType );
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

    options->adfName = argv [ optind ];
    return true;
}
