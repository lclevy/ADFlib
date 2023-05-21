#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])

# version details - use utils/bump-version to update it
m4_define([ADFLIB_VERSION], [0.7.13])
m4_define([ADFLIB_LT_VERSION], [0:12:0])
m4_define([ADFLIB_DATE], [2022-01-25])

AC_INIT([adflib],[ADFLIB_VERSION],[lclevy_AT_club-internet.fr])

AC_CONFIG_SRCDIR([src/adf_env.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIRS([m4])
AM_INIT_AUTOMAKE
AM_SILENT_RULES([yes])

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

#
# native
#
AC_CANONICAL_HOST

# detect first
echo "Host OS: ${host_os}"
case "${host_os}" in
     linux*)
	NATIVE_DIR=linux
	;;
     win32*)
	NATIVE_DIR=win32
	;;
     cygwin*)
	NATIVE_DIR=win32
	;;
     darwin*)
	NATIVE_DIR=generic
	;;
     *)
        NATIVE_DIR=generic
	;;
esac

# ... then check if enforced
AC_ARG_ENABLE([native_generic],
              [  --enable-native-generic    Enable generic native devices],
              [case "${enableval}" in
                yes) NATIVE_DIR=generic ;;
                no)  ;;
                *) AC_MSG_ERROR([bad value ${enableval} for --enable-native-generic]) ;;
               esac],
              [])

AC_ARG_ENABLE([native_linux],
              [  --enable-native-linux    Enable Linux native devices],
              [case "${enableval}" in
                yes) NATIVE_DIR=linux ;;
                no)  ;;
                *) AC_MSG_ERROR([bad value ${enableval} for --enable-native-linux]) ;;
               esac],
              [])

AC_ARG_ENABLE([native_win32],
              [  --enable-native-win32    Enable Win32 native devices],
              [case "${enableval}" in
                yes) NATIVE_DIR=win32 ;;
                no)  ;;
                *) AC_MSG_ERROR([bad value ${enableval} for --enable-native-win32]) ;;
               esac],
              [])

AM_CONDITIONAL([NATIVE_GENERIC], [test x${NATIVE_DIR} = xgeneric])
AM_CONDITIONAL([NATIVE_LINUX],   [test x${NATIVE_DIR} = xlinux])
AM_CONDITIONAL([NATIVE_WIN32],   [test x${NATIVE_DIR} = xwin32])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AM_PROG_AR
LT_INIT

# Checks for libraries.
PKG_CHECK_MODULES([CHECK], [check >= 0.9.6], [tests=yes], [tests=no])
AM_CONDITIONAL([TESTS], [test x${tests} = xyes])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_INT32_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Version
AC_SUBST([ADFLIB_VERSION], [ADFLIB_VERSION])
AC_SUBST([ADFLIB_LT_VERSION], [ADFLIB_LT_VERSION])
AC_SUBST([ADFLIB_DATE], [ADFLIB_DATE])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 doc/Makefile
                 examples/Makefile
                 regtests/Test/Makefile
                 tests/Makefile
                 adflib.pc])
AC_OUTPUT
