#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)

# Version info
m4_define([adflib_version_major],[0])
m4_define([adflib_version_minor],[7])
m4_define([adflib_version_micro],[13])
m4_define([adflib_version],[adflib_version_major.adflib_version_minor.adflib_version_micro])

AC_INIT(adflib,[adflib_version],lclevy_AT_club-internet.fr)

AC_CONFIG_SRCDIR([src/adf_env.c])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIRS([m4])
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

#
# native
#
AC_CANONICAL_HOST

# detect first
echo "Host OS: ${host_os}"
case "${host_os}" in
     linux*)
	NATIVE_DIR=linux
	ADFLIB_OS=linux
	;;
     win32*)
	NATIVE_DIR=win32
	ADFLIB_OS=win32
	;;
     cygwin*)
	NATIVE_DIR=win32
	ADFLIB_OS=cygwin
	;;
     darwin*)
	NATIVE_DIR=generic
	ADFLIB_OS=darwin
	;;
     *)
        NATIVE_DIR=generic
	ADFLIB_OS=generic
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
echo "NATIVE_DIR: ${NATIVE_DIR}"
echo "ADFLIB_OS: ${ADFLIB_OS}"

#AM_CONDITIONAL([NATIVE_GENERIC], [test x${devnative} = xgeneric])
#AM_CONDITIONAL([NATIVE_LINUX],   [test x${devnative} = xlinux])
#AM_CONDITIONAL([NATIVE_WIN32],   [test x${devnative} = xwin32])

AM_CONDITIONAL([ADFLIB_OS_CYGWIN], [test x${ADFLIB_OS} = xcygwin])
AM_CONDITIONAL([ADFLIB_OS_DARWIN], [test x${ADFLIB_OS} = xdarwin])
AM_CONDITIONAL([ADFLIB_OS_LINUX],  [test x${ADFLIB_OS} = xlinux])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AM_PROG_AR
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


AC_SUBST(NATIVE_DIR)

# Version
AC_SUBST(ADFLIB_MAJOR_VERSION, [adflib_version_major])
AC_SUBST(ADFLIB_MINOR_VERSION, [adflib_version_minor])
AC_SUBST(ADFLIB_MICRO_VERSION, [adflib_version_micro])

# Libtool version info
# ( https://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html )
#AC_SUBST(LT_RELEASE, [adflib_version_major.adflib_version_minor])
#LT_CURRENT=adflib_version_major
LT_CURRENT=`expr 100 '*' adflib_version_major + adflib_version_minor`
LT_REVISION=adflib_version_micro
#LT_AGE=adflib_version_minor
LT_AGE=0
AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/generic/Makefile
                 src/linux/Makefile
                 src/win32/Makefile
                 doc/Makefile
                 examples/Makefile
                 regtests/Test/Makefile
                 tests/Makefile
                 adflib.pc])
AC_OUTPUT
