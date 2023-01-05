#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(adflib, 0.7.12, lclevy_AT_club-internet.fr)
AC_CONFIG_SRCDIR([src/adf_env.c])
AC_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE

AC_ARG_ENABLE([examples],
              [  --enable-examples    Build exmples],
              [case "${enableval}" in
                yes) examples=true ;;
                no)  examples=false ;;
                *) AC_MSG_ERROR([bad value ${enableval} for --enable-examples]) ;;
               esac],
              [examples=true])

AM_CONDITIONAL([EXAMPLES], [test x$examples = xtrue])

AC_ARG_ENABLE([regtests],
              [  --enable-regtests    Build regression tests],
              [case "${enableval}" in
                yes) regtests=true ;;
                no)  regtests=false ;;
                *) AC_MSG_ERROR([bad value ${enableval} for --enable-regtests]) ;;
               esac],
              [regtests=true])

AM_CONDITIONAL([REGTESTS], [test x$regtests = xtrue])



# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

# Checks for libraries.
PKG_CHECK_MODULES([CHECK], [check >= 0.9.6])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([limits.h stdint.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_INT32_T
AC_TYPE_SIZE_T
AC_STRUCT_TM
AC_HEADER_STDBOOL
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_STAT
AC_CHECK_FUNCS([memset strchr strdup strerror strrchr])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 doc/Makefile
                 examples/Makefile
                 regtests/Test/Makefile
                 tests/Makefile
                 adflib.pc])
AC_OUTPUT
