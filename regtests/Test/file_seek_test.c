/*
 * file_seek_test.c
 */

#include <stdio.h>
#include"adflib.h"


int test_reading_data_after_seek ( struct Volume * const vol,
                                   char *                filename,
                                   unsigned int          offset,
                                   const unsigned  char  expected_value );

int test_reading_data_after_seek_ofs (  char * adf_image_fname );
int test_reading_data_after_seek_ffs (  char * adf_image_fname );


int main ( int argc, char * argv[] )
{ 
    adfEnvInitDefault();

//	adfSetEnvFct(0,0,MyVer,0);
    int status = 0;
    status += test_reading_data_after_seek_ofs ( argv[1] );
    status += test_reading_data_after_seek_ffs ( argv[2] );

    adfEnvCleanUp();

    return status;
}


int test_reading_data_after_seek_ofs ( char * adf_image_fname )
{
    int status = 0;

    printf ( "\n\n*********** TEST FILE SEEKING ON OFS\n" );

    /* mount an existing device : OFS */
    struct Device * const dev = adfMountDev ( adf_image_fname, TRUE );
    struct Volume * const vol = adfMount ( dev, 0, TRUE );
    adfVolumeInfo ( vol );


    /* test file seek OK.*/
    //const unsigned int highest_not_failing_pos = 36864;
    const unsigned int highest_not_failing_pos = 35135;  // 0x893f
    char large_file[] = "moon.gif";
    struct File * file = adfOpenFile ( vol, large_file, "r" );
    if ( ! file )  {
        status = 1;
        goto cleanup;
    }
    printf ( "Seeking to a non-failing position (%d)...\n",
             highest_not_failing_pos );
    adfFileSeek ( file, highest_not_failing_pos );
    adfCloseFile ( file );
    printf ("OK.\n");

    // test file seek that fail
    file = adfOpenFile ( vol, large_file, "r" );
    if ( ! file ) {
        status = 1;
        goto cleanup;
    }
    printf ( "Seeking to the first failing position (%d)...\n",
             highest_not_failing_pos + 1 );
    adfFileSeek ( file, highest_not_failing_pos + 1 ); // SEGFAULT(!)
    adfCloseFile ( file );
    printf ("OK.\n");


// test reading data after a seek
    status += test_reading_data_after_seek ( vol, large_file,
                                             0,
                                             0x47 );

    status += test_reading_data_after_seek ( vol, large_file,
                                             1,
                                             0x49 );

    status += test_reading_data_after_seek ( vol, large_file,
                                             highest_not_failing_pos,
                                             0x5 );

    status += test_reading_data_after_seek ( vol, large_file,
                                             highest_not_failing_pos + 1,
                                             0x2d );

    status += test_reading_data_after_seek ( vol, large_file,
                                             highest_not_failing_pos + 2,
                                             0x6c );


    status += test_reading_data_after_seek ( vol, large_file,
                                             0x2a708, 3 );

    status += test_reading_data_after_seek ( vol, large_file,
                                             0x2a716, 0x3b );  // the last byte of the file

cleanup:
    adfUnMount ( vol );
    adfUnMountDev ( dev );

    return status;
}


int test_reading_data_after_seek_ffs ( char * adf_image_fname )
{
    int status = 0;

    printf ( "\n\n*********** TEST FILE SEEKING ON FFS\n" );

    /* mount an existing device : OFS */
    struct Device * const dev = adfMountDev ( adf_image_fname, TRUE );
    struct Volume * const vol = adfMount ( dev, 0, TRUE );
    adfVolumeInfo ( vol );


    /* test file seek OK.*/
    //const unsigned int highest_not_failing_pos = 36864;
    const unsigned int highest_not_failing_pos_ofs = 35135;  // 0x893f
    //const unsigned int highest_not_failing_pos_ffs = 39935;  // 0x9bff   ( 512 * 72) ?
    const unsigned int highest_not_failing_pos_ffs = 36863;  // 0x9bff
    char large_file[] = "mod.And.DistantCall";
    struct File * file = adfOpenFile ( vol, large_file, "r" );
    if ( ! file )  {
        status = 1;
        goto cleanup;
    }
    printf ( "Seeking to a non-failing position (%d)...\n",
             highest_not_failing_pos_ffs );
    adfFileSeek ( file, highest_not_failing_pos_ffs );
    adfCloseFile ( file );
    printf ("OK.\n");


    // test file seek that fail
    file = adfOpenFile ( vol, large_file, "r" );
    if ( ! file ) {
        status = 1;
        goto cleanup;
    }
    printf ( "Seeking to the first failing position (%d)...\n",
             highest_not_failing_pos_ffs + 1 );
    adfFileSeek ( file, highest_not_failing_pos_ffs + 1 ); // SEGFAULT(!)
    adfCloseFile ( file );
    printf ("OK.\n");


// test reading data after a seek
    status += test_reading_data_after_seek ( vol, large_file,
                                             0,
                                             0x64 );

    status += test_reading_data_after_seek ( vol, large_file,
                                             1,
                                             0x69 );

    status += test_reading_data_after_seek ( vol, large_file,
                                             highest_not_failing_pos_ofs,
                                             0x28 );

    status += test_reading_data_after_seek ( vol, large_file,
                                             highest_not_failing_pos_ofs + 1,
                                             0x24 );

    status += test_reading_data_after_seek ( vol, large_file,
                                             highest_not_failing_pos_ofs + 2,
                                             0x1b );

    status += test_reading_data_after_seek ( vol, large_file,
                                             highest_not_failing_pos_ffs,
                                             0xff );

    status += test_reading_data_after_seek ( vol, large_file,
                                             highest_not_failing_pos_ffs + 1,
                                             0x07 );

    status += test_reading_data_after_seek ( vol, large_file,
                                             highest_not_failing_pos_ffs + 2,
                                             0x0c );

    status += test_reading_data_after_seek ( vol, large_file,
                                             0x237c0, 0xfc );

    status += test_reading_data_after_seek ( vol, large_file,
                                             0x237ce, 0xea );

    status += test_reading_data_after_seek ( vol, large_file,
                                             0x237cf, 0x00 );  // the last byte of the file

cleanup:
    adfUnMount ( vol );
    adfUnMountDev ( dev );

    return status;
}



// test reading data after a seek
int test_reading_data_after_seek ( struct Volume * const vol,
                                   char *                filename,
                                   unsigned int          offset,
                                   const unsigned char   expected_value )
{
    printf ( "\n  ************************************************************"
             "\n  *** Test reading data after seek to position 0x%x (%d)...\n",
             offset, offset );

    struct File * file = adfOpenFile ( vol, filename, "r" );
    if ( ! file )
        return 1;

    adfFileSeek ( file, offset );

    unsigned char c;
    int n = adfReadFile ( file, 1, &c );
    adfCloseFile ( file );

    if ( n != 1 ) {
        printf ( "Reading data failed!\n" );
        return 1;
    }

    if ( c != expected_value ) {
        printf ( "      Incorrect data read:  expected 0x%x != read 0x%x\n",
                 (int) expected_value, (int) c );
        return 1;
    }

    printf ( "      OK.\n" );
    return 0;
}
