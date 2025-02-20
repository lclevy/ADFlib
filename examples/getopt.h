/* This custom implementation of getopt() comes from:
 *   https://gist.github.com/superwills/5815344
 * see also the discussion on missing getopt on Windows:
 *   https://stackoverflow.com/questions/10404448/getopt-h-compiling-linux-c-code-in-windows
 *
 * For the copyright and license, see getopt.c
 */

#ifndef GETOPT_H
#define GETOPT_H

extern int
    opterr,                 /* if error message should be printed */
    optind,                 /* index into parent argv vector */
    optopt,                 /* character checked for validity */
    optreset;               /* reset getopt */

extern char *optarg;        /* argument associated with option */

int getopt(int nargc, char * const nargv[], const char *ostr);

#endif
