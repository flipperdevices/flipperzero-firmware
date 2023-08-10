#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([usercypto], [0.1], [])
AC_CONFIG_SRCDIR([src/rsa.c])

AM_INIT_AUTOMAKE([1.11 -Wall -Werror -Wno-portability foreign tar-ustar subdir-objects no-define color-tests])

LT_PREREQ([2.2])
LT_INIT([disable-static])
LT_LANG([C++])
LT_LANG([C])

# Checks for programs.
AC_PROG_CC
AC_CONFIG_MACRO_DIR([m4])

# Checks for libraries.
AM_LDFLAGS=$LDFLAGS
LDFLAGS="$LDFLAGS -L/opt/intel/ipp/lib -lippcp -lippcore"

# Path to find wolfssl/options and other includes
AM_CPPFLAGS=$CPPFLAGS
CPPFLAGS="$CPPFLAGS -I../../ -I/opt/intel/ipp/include"
AC_CHECK_LIB([ippcore], [ippGetStatusString], [], [AC_MSG_ERROR([ippcore library needed ./configure LDFLAGS=/path/to/ipp/lib])])
AC_CHECK_LIB([ippcp], [ippsRSA_InitPublicKey], [], [AC_MSG_ERROR([ippcp library needed ./configure LDFLAGS=/path/to/ipp/lib])])

# check headers
AC_CHECK_HEADER([ippcp.h], [], [AC_MSG_ERROR([ippcp.h not found ./configure CPPFLAGS=-I/ipp/headers])])
AC_CHECK_HEADER([ipp.h], [], [AC_MSG_ERROR([ipp.h not found ./configure CPPFLAGS=-I/ipp/headers])])

LDFLAGS=$AM_LDFLAGS
CPPFLAGS=$AM_CPPFLAGS

AM_LDFLAGS="-L/opt/intel/ipp/lib -lippcp -lippcore"
AM_CPPFLAGS="-I/opt/intel/ipp/include"

AC_SUBST([AM_CPPFLAGS])
AC_SUBST([AM_LDFLAGS])
AC_C_INLINE

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
