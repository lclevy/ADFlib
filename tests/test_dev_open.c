#include <check.h>
#include <stdlib.h>

#ifndef _WIN32
#include <unistd.h>   // for unlink()
#endif

#include "adflib.h"
//#include "adf_util.h"


typedef struct test_data_s {
    struct AdfDevice * device;
    char *          adfname;
    char *          volname;
    uint8_t         fstype;   // 0 - OFS, 1 - FFS
    AdfAccessMode   openMode;
    bool            ignoreChecksumErrors;
} test_data_t;


void setup ( test_data_t * const tdata );
void teardown ( test_data_t * const tdata );


START_TEST ( test_check_framework )
{
    ck_assert ( 1 );
}
END_TEST


START_TEST ( test_dev_open_nonexistent )
{
    test_data_t test_data[] = {
        {   .adfname = "nonexistent.adf",
            .openMode = ADF_ACCESS_MODE_READONLY,
            .ignoreChecksumErrors = false
        },
        {   .adfname = "nonexistent.adf",
            .openMode = ADF_ACCESS_MODE_READONLY,
            .ignoreChecksumErrors = true
        },
        {   .adfname = "nonexistent.adf",
            .openMode = ADF_ACCESS_MODE_READWRITE,
            .ignoreChecksumErrors = false
        },
        {   .adfname = "nonexistent.adf",
            .openMode = ADF_ACCESS_MODE_READWRITE,
            .ignoreChecksumErrors = true
        }
    };
    unsigned ntest_data = sizeof ( test_data ) / sizeof (  test_data_t );
    
    for ( unsigned i = 0 ; i < ntest_data ; i++ ) {
        adfEnvSetProperty ( ADF_PR_IGNORE_CHECKSUM_ERRORS,
                            test_data[i].ignoreChecksumErrors );

        struct AdfDevice * const dev = adfDevOpen ( test_data[i].adfname,
                                                    test_data[i].openMode );
        ck_assert_ptr_null ( dev );
    }
}
END_TEST


START_TEST ( test_dev_open_existent )
{
    test_data_t test_data[] = {
        {   .adfname = "anexistent1.adf",
            .volname = "AnExitentVolume",
            .openMode = ADF_ACCESS_MODE_READONLY,
            .ignoreChecksumErrors = false
        },
        {   .adfname = "anexistent2.adf",
            .volname = "AnExitentVolume",
            .openMode = ADF_ACCESS_MODE_READONLY,
            .ignoreChecksumErrors = true
        },
        {   .adfname = "anexistent3.adf",
            .volname = "AnExitentVolume",
            .openMode = ADF_ACCESS_MODE_READWRITE,
            .ignoreChecksumErrors = false
        },
        {   .adfname = "anexistent4.adf",
            .volname = "AnExitentVolume",
            .openMode = ADF_ACCESS_MODE_READWRITE,
            .ignoreChecksumErrors = true
        }
    };
    unsigned ntest_data = sizeof ( test_data ) / sizeof (  test_data_t );

    for ( unsigned i = 0 ; i < ntest_data ; i++ ) {
        test_data_t * const tdata = &test_data[i];

        setup ( tdata );

        //fprintf (stderr, "Testing: %u, open mode %d, ignore checksums %d\n",
        //         i, (int) tdata->openMode, tdata->ignoreChecksumErrors );

        adfEnvSetProperty ( ADF_PR_IGNORE_CHECKSUM_ERRORS,
                            tdata->ignoreChecksumErrors );

        struct AdfDevice * const dev = adfDevOpen ( tdata->adfname,
                                                    tdata->openMode );
        ck_assert_ptr_nonnull ( dev );
        adfDevClose ( dev );

        teardown ( tdata );
    }
}
END_TEST


Suite * adflib_suite ( void )
{
    Suite * const s = suite_create ( "adflib" );
    
    TCase * tc = tcase_create ( "check framework" );
    tcase_add_test ( tc, test_check_framework );
    suite_add_tcase ( s, tc );

    tc = tcase_create ( "adflib test_dev_open_nonexistent" );
    tcase_add_test ( tc, test_dev_open_nonexistent );
    suite_add_tcase ( s, tc );

    tc = tcase_create ( "adflib test_dev_open_existent" );
    tcase_add_test ( tc, test_dev_open_existent );
    suite_add_tcase ( s, tc );

    return s;
}


int main ( void )
{
    Suite * s = adflib_suite();
    SRunner * sr = srunner_create ( s );

    adfEnvInitDefault();
    srunner_run_all ( sr, CK_VERBOSE ); //CK_NORMAL );
    adfEnvCleanUp();

    int number_failed = srunner_ntests_failed ( sr );
    srunner_free ( sr );
    return ( number_failed == 0 ) ?
        EXIT_SUCCESS :
        EXIT_FAILURE;
}


void setup ( test_data_t * const tdata )
{
    tdata->device = adfDevCreate ( "dump", tdata->adfname, 80, 2, 11 );
    if ( ! tdata->device ) {       
        //return;
        exit(1);
    }
    if ( adfCreateFlop ( tdata->device, tdata->volname, tdata->fstype ) != ADF_RC_OK ) {
        fprintf ( stderr, "adfCreateFlop error creating volume: %s\n",
                  tdata->volname );
        exit(1);
    }

    adfDevUnMount ( tdata->device );
    adfDevClose ( tdata->device );
}


void teardown ( test_data_t * const tdata )
{
    unlink ( tdata->adfname );
}

