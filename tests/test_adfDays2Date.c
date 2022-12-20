#include <check.h>
#include <stdlib.h>

//#include "adflib.h"
#include "adf_util.h"


START_TEST ( test_check_framework )
{
    ck_assert ( 1 );
}
END_TEST


START_TEST ( test_adfDays2Date  )
{

    typedef struct test_data_s {
      // input
        int32_t days;

      //output
        int     yy,
                mm,
                dd;
    } test_data_t;

    test_data_t test_data[] = {
        // doesn't work for negative days
        // -> cannot represent dates before 1/1/1978 (!)
        //{ -1,   1977, 12, 31 },

        { 0,     1978,  1,  1 },
        { 6988,  1997,  2, 18 },
        { 16412, 2022, 12,  8 }
    };

    const int NTESTS = sizeof ( test_data ) / sizeof ( test_data_t );

    int yy, mm, dd;
    for ( int i = 0; i < NTESTS ; ++i ) {
        adfDays2Date (
            // input
            test_data[i].days,
            // output
            &yy, &mm, &dd );

        printf ( "days 0x%x ( %d )\n", test_data[i].days,
                                       test_data[i].days );
        ck_assert_int_eq ( test_data[i].yy, yy );
        ck_assert_int_eq ( test_data[i].mm, mm );
        ck_assert_int_eq ( test_data[i].dd, dd );
    }

}

Suite * adflib_suite ( void )
{
    Suite * s = suite_create ( "adflib" );
    
    TCase * tc = tcase_create ( "check framework" );
    tcase_add_test ( tc, test_check_framework );
    suite_add_tcase ( s, tc );

    tc = tcase_create ( "adflib adfDays2Date" );
    tcase_add_test ( tc, test_adfDays2Date  );
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
