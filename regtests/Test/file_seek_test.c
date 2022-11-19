/*
 * file_seek_test.c
 */

#include <stdio.h>
#include"adflib.h"


int main ( int argc, char * argv[] )
{ 
    adfEnvInitDefault();

//	adfSetEnvFct(0,0,MyVer,0);

    /* mount an existing device : OFS */
    struct Device * const dev = adfMountDev ( argv[1], TRUE );
    struct Volume * const vol = adfMount ( dev, 0, TRUE );
    adfVolumeInfo ( vol );

    /* test file seek OK.*/
    //const unsigned int highest_not_failing_pos = 36864;
    const unsigned int highest_not_failing_pos = 35135;
    char large_file[] = "moon.gif";
    struct File * file = adfOpenFile ( vol, large_file, "r" );
    if ( ! file )  return 1;
    printf ( "Seeking to a non-failing position (%d)\n",
             highest_not_failing_pos );
    adfFileSeek ( file, highest_not_failing_pos );
    adfCloseFile ( file );

    // test file seek that fail
    file = adfOpenFile ( vol, large_file, "r" );
    if ( ! file )  return 1;
    printf ( "Seeking to the first failing position (%d)\n",
             highest_not_failing_pos + 1 );
    adfFileSeek ( file, highest_not_failing_pos + 1 ); // SEGFAULT(!)
    adfCloseFile ( file );

    // clean-up
    adfUnMount ( vol );
    adfUnMountDev ( dev );

    adfEnvCleanUp();

    return 0;
}
