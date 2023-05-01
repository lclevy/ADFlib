
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>

#include "debug_util.h"


/*
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

    int size = backtrace ( buffer, BUFSIZE );
    const char * const * const strings = backtrace_symbols ( buffer, size );
    if ( strings == NULL ) {
        perror ( "error getting symbols" );
        return;
    }

    printf ( "Obtained %d stack frames.\n", size );
    for ( unsigned i = 0 ; i < size ; i++ )
        printf ( "%s\n", strings[i] );

    free ( strings );
}
