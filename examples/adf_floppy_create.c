
#include <adflib.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>


void usage ( void );


int main ( int     argc,
           char ** argv )
{
    if ( argc < 3 ) {
        usage();
        return 1;
    }

    char * const adfname = argv[1];

    FILE * const f = fopen ( adfname, "rb" );
    if ( f ) {
        fclose ( f );
        fprintf ( stderr, "'%s' already exists - aborting...\n", adfname );
        return 1;
    }
    
    const char * const floppy_type = argv[2];
    unsigned       tracks          = 80;
    const unsigned HEADS           = 2;

    unsigned sectors_per_track;
    if ( strncmp ( floppy_type, "dd", 2 ) == 0 )
        sectors_per_track = 11;
    else if ( strncmp ( floppy_type, "hd", 2 ) == 0 ) {
        sectors_per_track = 22;
    } else {
        fprintf ( stderr, "Incorrect fdtype '%s'.\n", floppy_type );
        return 1;        
    }

    // postfix to fdtype can specifie non-standard 80+ tracks
    if ( floppy_type[2] ) {
        if ( strcmp ( floppy_type + 2, "81" ) == 0 )
            tracks = 81;
        else if ( strcmp ( floppy_type + 2, "82" ) == 0 )
            tracks = 82;
        else if ( strcmp ( floppy_type + 2, "83" ) == 0 )
            tracks = 83;
        else {
            fprintf ( stderr, "Incorrect fdtype '%s'.\n", floppy_type );
            return 1;        
        }
    }
    
    printf ( "Creating floppy disk image: '%s'\n", adfname );

    adfEnvInitDefault();

    struct AdfDevice * device = adfDevCreate ( "dump", adfname, tracks, HEADS,
                                               sectors_per_track );
    if ( ! device ) {
        fprintf ( stderr, "Error creating floppy (%s) disk image %s.\n",
                  floppy_type, adfname );
        return 1;
    }
    adfDevInfo ( device );
    printf ( "Done!\n" );

    adfDevUnMount ( device );
    adfDevClose ( device );
    adfEnvCleanUp();

    return 0;
}


void usage ( void )
{
    printf ( "Usage:  adf_floppy_create filename fdtype\n\n"
             "   fdtype can be:\n"
             "     'dd' -> 880K\n"
             "     'hd' -> 1760K\n"
             "     'hd81', 'hd82', 'hd83' (non-standard 81-83 tracks)\n\n" );
}
