
#include <stdio.h>
#include <stdlib.h>
#include"adflib.h"

#define TEST_VERBOSITY 0

#define MAX_CHECKS 10

typedef struct check_s {
    char * dir_names[10];
    int    nentries;
} check_t;

typedef struct chdir_test_s {
    char *       info;
    char *       image;
    unsigned int nchecks;
    check_t      checks[MAX_CHECKS];
} chdir_test_t;


int run_chdir_tests ( chdir_test_t * test_data );

int test_chdir ( struct AdfVolume * vol,
                 check_t *          check );

int count_dir_entries ( struct AdfVolume * vol );


int main ( int argc, char * argv[] )
{ 
    (void) argc;
    adfEnvInitDefault();

//	adfSetEnvFct(0,0,MyVer,0);
    int status = 0;

    // setup
    chdir_test_t test_chdir_testffs = {
        .info       = "chdir testffs",
        .image      = NULL,
        .nchecks    = 6,
        .checks     = {
            //{ { "/", NULL }, 14 },
            //{ { ".", NULL }, 14 },
            { { "dir_1", NULL }, 1 },
            { { "dir_2", NULL }, 2 },
            { { "dir_2", "dir_21", NULL }, 0 },
            { { "empty_dir", NULL }, 0 },
            { { "hlink_dir1", NULL }, 1 },
            { { "hlink_dir2", NULL }, 2 }
            //{ { "slink_dir1", NULL }, 1 }   // -> softlinks do not work with adfChangeDir!
        }
    };

    chdir_test_t test_chdir_link_chains = {
        .info       = "chdir link chains",
        .image      = NULL,
        .nchecks    = 4,
        .checks     = {
            //{ { "/", NULL }, 14 },
            //{ { ".", NULL }, 14 },
            { { "Trashcan", NULL }, 0 },
            { { "dir1", NULL }, 1 },
            { { "dir1", "dir1_1", NULL }, 4 },
            { { "hardlinks_dir", "hl2hl2hl2dir1", "dir1_1", NULL }, 4 }
        }
    };
    
    test_chdir_testffs.image = argv[1];
    test_chdir_link_chains.image = argv[2];

    // run tests
    status += run_chdir_tests ( &test_chdir_testffs );
    status += run_chdir_tests ( &test_chdir_link_chains );
    // clean-up
    adfEnvCleanUp();

    return status;
}



int run_chdir_tests ( chdir_test_t * test_data )
{
//#if TEST_VERBOSITY > 0
    printf ( "\n*** %s  ( image file: %s )\n",
             test_data->info,
             test_data->image );
//#endif

    struct AdfDevice * const dev = adfDevOpen ( test_data->image,
                                                ADF_ACCESS_MODE_READONLY );
    if ( ! dev ) {
        fprintf ( stderr, "Cannot open file/device '%s' - aborting...\n",
                  test_data->image );
        adfEnvCleanUp();
        exit(1);
    }

    RETCODE rc = adfDevMount ( dev );
    if ( rc != RC_OK ) {
        fprintf ( stderr, "Cannot mount image %s - aborting the test...\n",
                  test_data->image );
        adfDevClose ( dev );
        return 1;
    }

    struct AdfVolume * const vol = adfVolMount ( dev, 0, ADF_ACCESS_MODE_READONLY );
    if ( ! vol ) {
        fprintf ( stderr, "Cannot mount volume 0 from image %s - aborting the test...\n",
                  test_data->image );
        adfDevUnMount ( dev );
        adfDevClose ( dev );
        return 1;
    }

    int nerrors = 0;
    for ( unsigned i = 0 ; i < test_data->nchecks ; ++i ) {
#if TEST_VERBOSITY > 0
        printf ("Executing check %d\n", i );
#endif
        nerrors += test_chdir ( vol, &test_data->checks[i] );
    }

    if ( nerrors ) {
        fprintf ( stderr, " -> Error chdir on image '%s'!\n", test_data->image );
    }
    
    // clean-up
//clean_up:
    //adfToRootDir ( vol );
    adfVolUnMount ( vol );
    adfDevUnMount ( dev );
    adfDevClose ( dev );

    return nerrors;
}


int test_chdir ( struct AdfVolume * vol,
                 check_t *          check )
{
//#if TEST_VERBOSITY > 0
    printf ("Test entering: ");
    for ( char ** dir = check->dir_names ; *dir ; dir++ ) {
        printf ("%s/", *dir );
    }
    printf ("\n");
//#endif
    adfToRootDir ( vol );

    for ( unsigned i = 0 ; check->dir_names[i] != NULL && i < 10; ++i ) {
        char * dir = check->dir_names[i];
#if TEST_VERBOSITY > 0
        printf ( "\nchdir to %s\n", dir );
#endif
        if ( adfChangeDir ( vol, dir ) != RC_OK ) {
            fprintf ( stderr, " -> Cannot chdir to %s\n", dir );
            adfToRootDir ( vol );
            return 1;
        }

        // add checking the name of the current dir. pointer block
/*        struct AdfEntryBlock ...
        if ( strcmp ( dir, ) != 0 ) {
            fprintf ( stderr, "Cannot chdir to %s\n", dir );
            adfToRootDir ( vol );
            return 1;
            }*/
    }

    int nentries = count_dir_entries ( vol );
    if ( nentries != check->nentries ) {
        fprintf ( stderr, " -> Incorrect number of entries: counted %d != expected %d\n",
                  nentries, check->nentries );
        adfToRootDir ( vol );
        return 1;            
    }
    adfToRootDir ( vol );
    printf (" -> OK.\n");
    return 0;
}


int count_dir_entries ( struct AdfVolume * vol )
{
    struct AdfList *list, *cell;
    
    int nentries = 0;
    cell = list = adfGetDirEnt ( vol, vol->curDirPtr );
    while ( cell ) {
        //adfEntryPrint ( cell->content );
        cell = cell->next;
        nentries++;
    }
    adfFreeDirList ( list );
    return nentries;
}
