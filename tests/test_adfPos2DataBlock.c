#include <check.h>
#include <stdlib.h>

//#include "adflib.h"
#include "adf_file.h"


START_TEST ( test_check_framework )
{
    ck_assert ( 1 );
}
END_TEST


START_TEST ( test_adfPos2DataBlock )
{
    int32_t
        dataBlockIndexInExtBlock,
        posInDataBlock,
        dataBlockNumber,
        extBlockNumber;

    typedef struct test_data_s {
      // input
        int32_t offset;
        int     blocksize;

      //output
        int     dataBlockIndexInExtBlock,
                posInDataBlock,
                dataBlockNumber;
        int32_t extBlockNumber;
    } test_data_t;

    test_data_t test_data[] = {
      // ffs

        { 0x8fff,                      // the last offset without ext block (ffs)
                            512,  0, 511,  71, -1 },
        
        { 0x9000 /*36864*/,  // 1st offset in datablock addressed with ext. block (ffs)
                            512,  0,   0,  72, 0 },

        { 0x9001 /*36865*/, 512,  0,   1,  72, 0 },

        { 0x9180,           512,  0, 384,  72, 0 },
        
        { 0x91ff,           512,  0, 511,  72, 0 },
        { 0x9200,           512,  1,   0,  73, 0 },
        { 0x9201,           512,  1,   1,  73, 0 },

        { 0x9bc0,           512,  5, 448,  77, 0 },
        
        { 0x11fff,          512, 71, 511, 143, 0 },
        
        { 0x12000,            // 1st offset in datablock addressed
                              // by the 2nd ext. block
                            512,  0,   0, 144, 1 },
        { 0x12001,          512,  0,   1, 144, 1 },

        { 0x237bf,          512, 67, 447, 283, 2 },
        { 0x237c0,          512, 67, 448, 283, 2 },
        { 0x237ce,          512, 67, 462, 283, 2 },
        { 0x237cf,          512, 67, 463, 283, 2 },

      // ofs
        { 0x893f,           488,  0, 487,  71, -1 },
        { 0x8940,      // the 1st offset in datablock addressed by ext. block (ofs)
                            488,  0,   0,  72, 0 },
        { 0x8941,           488,  0,   1,  72, 0 },
        { 0x2a708,          488, 68, 104, 356, 3 },
        { 0x2a716,          488, 68, 118, 356, 3 }
    };

    const int NTESTS = sizeof ( test_data ) / sizeof ( test_data_t );
    
    for ( int i = 0; i < NTESTS ; ++i ) {
        extBlockNumber = adfPos2DataBlock (
            // input
            test_data[i].offset,
            test_data[i].blocksize, 
            // output
            &dataBlockIndexInExtBlock,
            &posInDataBlock,
            &dataBlockNumber );

        printf ( "offset 0x%x ( %d )\n", test_data[i].offset, test_data[i].offset );
        ck_assert_int_eq ( test_data[i].offset,
                           test_data[i].offset );

        ck_assert_int_eq ( test_data[i].dataBlockIndexInExtBlock,
                           dataBlockIndexInExtBlock );

        ck_assert_int_eq ( test_data[i].posInDataBlock,
                           posInDataBlock );

        ck_assert_int_eq ( test_data[i].dataBlockNumber,
                           dataBlockNumber );

        ck_assert_int_eq ( test_data[i].extBlockNumber,
                           extBlockNumber );
    }

}

Suite * adflib_suite ( void )
{
    Suite * s = suite_create ( "adflib" );
    
    TCase * tc = tcase_create ( "check framework" );
    tcase_add_test ( tc, test_check_framework );
    suite_add_tcase ( s, tc );

    tc = tcase_create ( "adflib adfPos2DataBlock" );
    tcase_add_test ( tc, test_adfPos2DataBlock );
    suite_add_tcase ( s, tc );

    return s;
}


int main ( void )
{
    Suite * s = adflib_suite();
    SRunner * sr = srunner_create ( s );

    srunner_run_all ( sr, CK_VERBOSE ); //CK_NORMAL );
    int number_failed = srunner_ntests_failed ( sr );
    srunner_free ( sr );
    return ( number_failed == 0 ) ?
        EXIT_SUCCESS :
        EXIT_FAILURE;
}
