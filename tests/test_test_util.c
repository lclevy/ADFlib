#include <check.h>
#include <stdlib.h>

#include "test_util.h"


START_TEST ( test_check_framework )
{
    ck_assert ( 1 );
}
END_TEST


START_TEST ( test_filesize2datablocks )
{
    static const struct testdata_s {
        int fsize,
            result488,
            result512;
    } testdata [] = {
        { 0,      0,  0 },
        { 1,      1,  1 },
        { 487,    1,  1 },
        { 488,    1,  1 },
        { 489,    2,  1 },
        { 512,    2,  1 },
        { 513,    2,  2 },
        { 976,    2,  2 },
        { 977,    3,  2 },
        { 1024,   3,  2 },
        { 1025,   3,  3 },
        { 35136, 72, 69 },
        { 35137, 73, 69 }
    };
    static const unsigned ntests = sizeof ( testdata ) / sizeof ( struct testdata_s );
    for ( unsigned i = 0 ; i < ntests ; ++i ) {
        //ck_assert_int_eq ( filesize2datablocks ( testdata[i].fsize, 488 ),
        //                   testdata[i].result488 );
        int dblocks = filesize2datablocks ( testdata[i].fsize, 488 );
        ck_assert_msg ( dblocks == testdata[i].result488,
                        "datablocks incorrect: %d, expected %d, blocksize %d",
                        dblocks, testdata[i].result488, testdata[i].fsize );

        //ck_assert_int_eq ( filesize2datablocks ( testdata[i].fsize, 512 ),
        //                   testdata[i].result512 );
        dblocks = filesize2datablocks ( testdata[i].fsize, 512 );
        ck_assert_msg ( dblocks == testdata[i].result512,
                        "datablocks incorrect: %d, expected %d, blocksize %d",
                        dblocks, testdata[i].result512, testdata[i].fsize );
    }
}
END_TEST


START_TEST ( test_datablocks2extblocks )
{
    static const struct testdata_s {
        int datablocks,
            result;
    } testdata [] = {
        { 0,    0 },
        { 1,    0 },
        { 72,   0 },
        { 73,   1 },
        { 144,  1 },
        { 145,  2 },
        { 720,  9 },
        { 721, 10 }
    };
    static const unsigned ntests = sizeof ( testdata ) / sizeof ( struct testdata_s );
    for ( unsigned i = 0 ; i < ntests ; ++i ) {
        int extblocks = datablocks2extblocks ( testdata[i].datablocks );
        ck_assert_msg ( extblocks == testdata[i].result,
                        "ext. blocks incorrect: %d, expected %d, datablocks %d",
                        extblocks, testdata[i].result, testdata[i].datablocks );
    }
}
END_TEST


START_TEST ( test_filesize2blocks )
{
    static const struct testdata_s {
        int fsize,
            result488,
            result512;
    } testdata [] = {
        { 0,      1,  1 },
        { 1,      2,  2 },
        { 487,    2,  2 },
        { 488,    2,  2 },
        { 489,    3,  2 },
        { 512,    3,  2 },
        { 513,    3,  3 },
        { 976,    3,  3 },
        { 977,    4,  3 },
        { 1024,   4,  3 },
        { 1025,   4,  4 },

        { 35136, 72 + 1,        // 72 data blocks + file header
                         70 },  
        { 35137, 75,            // the first ext. block for 488 (+ 1 data + 1 ext block)
                         70 },
        { 36864, 78,     73 },
        { 36865, 78,     75 },  // the first ext. block for 512

        { 70272, ( 72 * 2 )     + 1 + 1, 138 + 1 + 1 },
        { 70273, ( 72 * 2 + 1 ) + 2 + 1,                 /* the 2nd ext. block for 488
                                                            (+1 data block, +1 ext block) */
                                         138 + 1 + 1 },

        { 73728, ( 72 * 2 + 8 ) + 2 + 1, ( 72 * 2 )     + 1 + 1 },
        { 73729, ( 72 * 2 + 8 ) + 2 + 1, ( 72 * 2 + 1 ) + 2 + 1 }  /* the 2nd ext block for 512
                                                                      (+1 data block, +1 ext block) */
    };
    static const unsigned ntests = sizeof ( testdata ) / sizeof ( struct testdata_s );
    for ( unsigned i = 0 ; i < ntests ; ++i ) {
        int nblocks = filesize2blocks ( testdata[i].fsize, 488 );
        ck_assert_msg ( nblocks == testdata[i].result488,
                        "blocks incorrect: %d, expected %d, blocksize %d",
                        nblocks, testdata[i].result488, testdata[i].fsize );

        nblocks = filesize2blocks ( testdata[i].fsize, 512 );
        ck_assert_msg ( nblocks == testdata[i].result512,
                        "blocks incorrect: %d, expected %d, blocksize %d",
                        nblocks, testdata[i].result512, testdata[i].fsize );
    }
}
END_TEST


START_TEST ( test_datablocks2posInExtBlk )
{
    static const struct testdata_s {
        int datablock_idx,
            result;
    } testdata [] = {
        //{ 0,    -1 },
        //{ 1,    -1 },
        //{ 2,    -1 },

        //{ 71,   -1 },
        { 0,    0 },
        { 1,    0 },
        { 2,    0 },

        { 71,   0 },
        { 72,   0 },
        { 73,   1 },
        { 74,   2 },
        { 75,   3 },

        { 143,  71 },
        { 144,   0 },
        { 145,   1 },
        { 146,   2 },
        { 147,   3 },

        { 719,  71 },
        { 720,   0 },
        { 721,   1 },
        { 722,   2 }
    };
    static const unsigned ntests = sizeof ( testdata ) / sizeof ( struct testdata_s );
    for ( unsigned i = 0 ; i < ntests ; ++i ) {
        int posInExtBlk = datablock2posInExtBlk ( testdata[i].datablock_idx );
        ck_assert_msg ( posInExtBlk == testdata[i].result,
                        "pos in ext. block incorrect: %d, expected %d, datablocks %d",
                        posInExtBlk, testdata[i].result, testdata[i].datablock_idx );
    }
}
END_TEST


Suite * adflib_suite ( void )
{
    Suite * s = suite_create ( "test_util" );
    
    TCase * tc = tcase_create ( "check framework" );
    tcase_add_test ( tc, test_check_framework );
    suite_add_tcase ( s, tc );

    tc = tcase_create ( "test_filesize2datablocks" );
    //tcase_add_checked_fixture ( tc, setup_ffs, teardown_ffs );
    tcase_add_test ( tc, test_filesize2datablocks );
    suite_add_tcase ( s, tc );

    tc = tcase_create ( "test_datablocks2extblocks" );
    tcase_add_test ( tc, test_datablocks2extblocks );
    suite_add_tcase ( s, tc );

    tc = tcase_create ( "test_filesize2blocks" );
    tcase_add_test ( tc, test_filesize2blocks );
    suite_add_tcase ( s, tc );

    tc = tcase_create ( "test_datablocks2posInExtBlk" );
    tcase_add_test ( tc, test_datablocks2posInExtBlk );
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
