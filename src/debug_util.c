
#include "debug_util.h"

#if ( defined HAVE_BACKTRACE && defined HAVE_BACKTRACE_SYMBOLS )
#include <execinfo.h>    /* required for backtrace() */
#endif

#include <stdio.h>
#include <stdlib.h>

#if ( defined HAVE_BACKTRACE && defined HAVE_BACKTRACE_SYMBOLS )
/*
  backtrace(), backtrace_symbols are available only in glibc

  Require link option: -rdynamic

  More info:
  https://www.gnu.org/software/libc/manual/html_node/Backtraces.html
  https://stackoverflow.com/questions/5945775/how-to-get-more-detailed-backtrace
  https://stackoverflow.com/questions/3899870/print-call-stack-in-c-or-c/54365144#54365144
  https://stackoverflow.com/questions/6934659/how-to-make-backtrace-backtrace-symbols-print-the-function-names
 */
void adfPrintBacktrace ( void )
{
    const unsigned BUFSIZE = 100;
    void *buffer [ BUFSIZE ];

    int size = backtrace ( buffer, (int) BUFSIZE );
    const char * const * const strings =
        ( const char * const* const ) backtrace_symbols ( buffer, size );

    if ( strings == NULL ) {
        perror ( "error getting symbols" );
        return;
    }

    printf ( "Obtained %d stack frames.\n", size );
    for ( int i = 0 ; i < size ; i++ )
        printf ( "%s\n", strings[i] );

    free ( (void *) strings );

}
#else
/* no backtrace without glibc

   ( for Windows, if ever needed, this might be helpful:
     https://stackoverflow.com/questions/26398064/counterpart-to-glibcs-backtrace-and-backtrace-symbols-on-windows )
 */
void adfPrintBacktrace ( void )
{
    fprintf ( stderr, "Sorry, no backtrace without glibc...\n" );
}
#endif
