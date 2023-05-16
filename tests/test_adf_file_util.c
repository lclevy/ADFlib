#include <check.h>
#include <stdio.h>
#include <stdlib.h>

//#include "adflib.h"
#include "adf_file_util.h"


START_TEST ( test_check_framework )
{
    ck_assert ( 1 );
}
END_TEST


START_TEST ( test_adfFilePos2datablockIndex )
{
    typedef struct test_data_s {
      // input
        unsigned pos,
                 blocksize;
      //output
        unsigned datablock_index;
    } test_data_t;

    const test_data_t test_data[] = {
        { 0,    488, 0 }, { 0,    512, 0 },
        { 487,  488, 0 }, { 487,  512, 0 },
        { 488,  488, 1 }, { 488,  512, 0 },
        { 511,  488, 1 }, { 511,  512, 0 },
        { 512,  488, 1 }, { 512,  512, 1 },
        { 975,  488, 1 }, { 975,  512, 1 },
        { 976,  488, 2 }, { 976,  512, 1 },
        { 1023, 488, 2 }, { 1023, 512, 1 },
        { 1024, 488, 2 }, { 1024, 512, 2 }
    };
    const unsigned NTESTS = sizeof ( test_data ) / sizeof ( test_data_t );

    for ( unsigned i = 0; i < NTESTS ; ++i ) {
        unsigned dblock_i = adfFilePos2datablockIndex ( test_data[i].pos,
                                                        test_data[i].blocksize );
        ck_assert_uint_eq ( test_data[i].datablock_index, dblock_i );
    }
}


START_TEST ( test_adfFileSize2Datablocks )
{
    typedef struct test_data_s {
      // input
        unsigned fsize,
                 blocksize;
      //output
        unsigned ndatablocks;
    } test_data_t;

    const test_data_t test_data[] = {
        { 0,    488, 0 }, { 0,    512, 0 },
        { 1,    488, 1 }, { 1,    512, 1 },        
        { 487,  488, 1 }, { 487,  512, 1 },
        { 488,  488, 1 }, { 488,  512, 1 },
        { 489,  488, 2 }, { 489,  512, 1 },
        { 511,  488, 2 }, { 511,  512, 1 },
        { 512,  488, 2 }, { 512,  512, 1 },
        { 513,  488, 2 }, { 513,  512, 2 },
        { 975,  488, 2 }, { 975,  512, 2 },
        { 976,  488, 2 }, { 976,  512, 2 },
        { 977,  488, 3 }, { 977,  512, 2 },
        { 1023, 488, 3 }, { 1023, 512, 2 },
        { 1024, 488, 3 }, { 1024, 512, 2 },
        { 1025, 488, 3 }, { 1025, 512, 3 }
    };
    const unsigned NTESTS = sizeof ( test_data ) / sizeof ( test_data_t );

    for ( unsigned i = 0; i < NTESTS ; ++i ) {
        unsigned ndblocks = adfFileSize2Datablocks ( test_data[i].fsize,
                                                     test_data[i].blocksize );
        //ck_assert_uint_eq ( test_data[i].ndatablocks, ndblocks );
        ck_assert_msg ( test_data[i].ndatablocks == ndblocks,
                        "test_data[i].ndatablocks %u != ndblocks %u, fsize %u, blocksize %u",
                        test_data[i].ndatablocks, ndblocks,
                        test_data[i].fsize,
                        test_data[i].blocksize );
    }
}


START_TEST ( test_adfFileDatablocks2Extblocks )
{
    typedef struct test_data_s {
      // input
        unsigned ndatablocks;
      //output
        unsigned nextblocks;
    } test_data_t;

    const test_data_t test_data[] = {
        { 0,   0 },
        { 1,   0 },
        { 71,  0 },
        { 72,  0 },
        { 73,  1 },
        { 144, 1 },
        { 145, 2 },
        { 216, 2 },
        { 217, 3 }
    };
    const unsigned NTESTS = sizeof ( test_data ) / sizeof ( test_data_t );

    for ( unsigned i = 0; i < NTESTS ; ++i ) {
        unsigned nExtBlocks = adfFileDatablocks2Extblocks ( test_data[i].ndatablocks );
        //ck_assert_uint_eq ( test_data[i].ndatablocks, ndblocks );
        ck_assert_msg ( test_data[i].nextblocks == nExtBlocks,
                        "test_data[i].nextblocks %u != nExtBlocks %u, ndatablocks %u",
                        test_data[i].nextblocks, nExtBlocks,
                        test_data[i].ndatablocks );
    }
}



START_TEST ( test_adfFileSize2Blocks )
{
    typedef struct test_data_s {
      // input
        unsigned fsize,
                 blocksize;
      //output
        unsigned nblocks;
    } test_data_t;

    const test_data_t test_data[] = {
        { 0,     488, 1 },  { 0,     512, 1 },     // only file header block
        { 1,     488, 2 },  { 1,     512, 2 },     // +1 data block
        { 487,   488, 2 },  { 487,   512, 2 },
        { 488,   488, 2 },  { 488,   512, 2 },
        { 489,   488, 3 },  { 489,   512, 2 },     // +1 data block (OFS)
        { 511,   488, 3 },  { 511,   512, 2 },
        { 512,   488, 3 },  { 512,   512, 2 },
        { 513,   488, 3 },  { 513,   512, 3 },     // +1 data block (FFS)
        { 35136, 488, 73 }, { 35136, 512, 70 },
        { 35137, 488, 75 }, { 35137, 512, 70 },    // +1 d. block, +1 (first) ext. (OFS)
        { 36864, 488, 78 }, { 36864, 512, 73 },
        { 36865, 488, 78 }, { 36865, 512, 75 },    // +1 d. block, +1 (first) ext. (FFS)

        /*            data blocks    ext  hdr  */
        { 70272, 488, ( 72 * 2 )     + 1 + 1 },  { 70272, 512, 138 + 1 + 1 },
        { 70273, 488, ( 72 * 2 + 1 ) + 2 + 1 },  { 70273, 512, 138 + 1 + 1 },
        /*                     ^^^   ^^^
         * the 2nd ext. block for 488 (+1 data block, +1 ext block)
         */

        { 73728, 488, ( 72 * 2 + 8 ) + 2 + 1 },  { 73728, 512, ( 72 * 2 )     + 1 + 1 },
        { 73729, 488, ( 72 * 2 + 8 ) + 2 + 1 },  { 73729, 512, ( 72 * 2 + 1 ) + 2 + 1 }
        /*                                                              ^^^   ^^^
         *                       the 2nd ext block for 512 (+1 data block, +1 ext block)
         */
    };
    const unsigned NTESTS = sizeof ( test_data ) / sizeof ( test_data_t );

    for ( unsigned i = 0; i < NTESTS ; ++i ) {
        unsigned nblocks = adfFileSize2Blocks ( test_data[i].fsize,
                                                test_data[i].blocksize );
        //ck_assert_uint_eq ( test_data[i].ndatablocks, ndblocks );
        ck_assert_msg ( test_data[i].nblocks == nblocks,
                        "test_data[i].nblocks %u != nblocks %u, fsize %u, blocksize %u",
                        test_data[i].nblocks, nblocks,
                        test_data[i].fsize,
                        test_data[i].blocksize );
    }
}


Suite * adflib_suite ( void )
{
    Suite * s = suite_create ( "adf file util" );
    
    TCase * tc = tcase_create ( "check framework" );
    tcase_add_test ( tc, test_check_framework );
    suite_add_tcase ( s, tc );

    tc = tcase_create ( "adflib adfFilePos2datablockIndex" );
    tcase_add_test ( tc, test_adfFilePos2datablockIndex );
    suite_add_tcase ( s, tc );

    tc = tcase_create ( "adflib adfFileSize2Datablocks" );
    tcase_add_test ( tc, test_adfFileSize2Datablocks );
    suite_add_tcase ( s, tc );

    tc = tcase_create ( "adflib adfFileDatablocks2Extblocks" );
    tcase_add_test ( tc, test_adfFileDatablocks2Extblocks );
    suite_add_tcase ( s, tc );

    tc = tcase_create ( "adflib adfFileSize2Blocks" );
    tcase_add_test ( tc, test_adfFileSize2Blocks );
    suite_add_tcase ( s, tc );

    return s;
}


int main ( void )
{
    Suite * suite = adflib_suite();
    SRunner * sr = srunner_create ( suite );

    srunner_run_all ( sr, CK_VERBOSE ); //CK_NORMAL );
    int number_failed = srunner_ntests_failed ( sr );
    srunner_free ( sr );
    return ( number_failed == 0 ) ?
        EXIT_SUCCESS :
        EXIT_FAILURE;
}
