# configure.ac
#
# Copyright (C) 2006-2021 wolfSSL Inc.
#
# This file is part of wolfSSL. (formerly known as CyaSSL)
#
#
AC_COPYRIGHT([Copyright (C) 2006-2020 wolfSSL Inc.])
AC_PREREQ([2.69])
AC_INIT([wolfssl],[5.6.3],[https://github.com/wolfssl/wolfssl/issues],[wolfssl],[https://www.wolfssl.com])
AC_CONFIG_AUX_DIR([build-aux])

# The following sets CFLAGS to empty if unset on command line.  We do not
# want the default "-g -O2" that AC_PROG_CC sets automatically.
: ${CFLAGS=""}

# Capture user C_EXTRA_FLAGS from configure line.
# Use of C_EXTRA_FLAGS is deprecated because CFLAGS was fixed but someone
# might still be using it.
CFLAGS="$CFLAGS $C_EXTRA_FLAGS $C_FLAGS"

AC_PROG_CC
AM_PROG_CC_C_O
AC_CANONICAL_HOST
AC_CANONICAL_TARGET
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([1.14.1 -Wall -Werror -Wno-portability foreign tar-ustar subdir-objects no-define color-tests])
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AC_ARG_PROGRAM

AC_CONFIG_HEADERS([config.h:config.in])

LT_PREREQ([2.4.2])
LT_INIT([disable-static win32-dll])

AC_ARG_VAR(EXTRA_CPPFLAGS, [Extra CPPFLAGS to add to end of autoconf-computed arg list.  Can also supply directly to make.])
AC_ARG_VAR(EXTRA_CFLAGS, [Extra CFLAGS to add to end of autoconf-computed arg list.  Can also supply directly to make.])
AC_ARG_VAR(EXTRA_CCASFLAGS, [Extra CCASFLAGS to add to end of autoconf-computed arg list.  Can also supply directly to make.])
AC_ARG_VAR(EXTRA_LDFLAGS, [Extra LDFLAGS to add to end of autoconf-computed arg list.  Can also supply directly to make.])

WOLFSSL_CONFIG_ARGS=$ac_configure_args
AC_SUBST([WOLFSSL_CONFIG_ARGS])

# shared library versioning
# The three numbers in the libwolfssl.so.*.*.* file name. Unfortunately
# these numbers don't always line up nicely with the library version.
WOLFSSL_LIBRARY_VERSION_FIRST=40
WOLFSSL_LIBRARY_VERSION_SECOND=0
WOLFSSL_LIBRARY_VERSION_THIRD=0
WOLFSSL_LIBRARY_VERSION=40:0:0
#                        | | |
#                 +------+ | +---+
#                 |        |     |
#                current:revision:age
#                 |        |     |
#                 |        |     +- increment if interfaces have been added
#                 |        |        set to zero if interfaces have been removed
#                 |        |        or changed
#                 |        +- increment if source code has changed
#                 |           set to zero if current is incremented
#                 +- increment if interfaces have been added, removed or changed
AC_SUBST([WOLFSSL_LIBRARY_VERSION_FIRST])
AC_SUBST([WOLFSSL_LIBRARY_VERSION_SECOND])
AC_SUBST([WOLFSSL_LIBRARY_VERSION_THIRD])
AC_SUBST([WOLFSSL_LIBRARY_VERSION])

gl_VISIBILITY
AS_IF([ test -n "$CFLAG_VISIBILITY" ], [
       AM_CFLAGS="$AM_CFLAGS $CFLAG_VISIBILITY"
       ])


# Moved these size of and type checks before the library checks.
# The library checks add the library to subsequent test compiles
# and in some rare cases, the networking check causes these sizeof
# checks to fail.
AC_CHECK_SIZEOF([long long])
AC_CHECK_SIZEOF([long])
AC_CHECK_SIZEOF([time_t])
AC_CHECK_TYPES([__uint128_t])


# Distro build feature subset (Debian, Ubuntu, etc.)
AC_ARG_ENABLE([distro],
    [AS_HELP_STRING([--enable-distro],[Enable wolfSSL distro build (default: disabled)])],
    [ ENABLED_DISTRO=$enableval ],
    [ ENABLED_DISTRO=no ]
    )
if test "$ENABLED_DISTRO" = "yes"
then
    enable_shared=yes
    enable_static=yes
    enable_all=yes
    enable_earlydata=no
    REPRODUCIBLE_BUILD_DEFAULT=yes
else
    REPRODUCIBLE_BUILD_DEFAULT=no
fi


AC_CHECK_HEADERS([arpa/inet.h fcntl.h limits.h netdb.h netinet/in.h stddef.h time.h sys/ioctl.h sys/socket.h sys/time.h errno.h sys/un.h])
AC_CHECK_LIB([network],[socket])
AC_C_BIGENDIAN
AC_C___ATOMIC
AC_CHECK_HEADER(stdatomic.h, [AM_CPPFLAGS="$AM_CPPFLAGS -DWOLFSSL_HAVE_ATOMIC_H"],[])

# check if functions of interest are linkable, but also check if
# they're declared by the expected headers, and if not, supersede the
# unusable positive from AC_CHECK_FUNCS().
AC_CHECK_FUNCS([gethostbyname getaddrinfo gettimeofday gmtime_r gmtime_s inet_ntoa memset socket strftime atexit])
AC_CHECK_DECLS([gethostbyname, getaddrinfo, gettimeofday, gmtime_r, gmtime_s, inet_ntoa, memset, socket, strftime, atexit], [], [
if test "$(eval echo \$"$(eval 'echo ac_cv_func_${as_decl_name}')")" = "yes"
then
    AC_MSG_NOTICE([    note: earlier check for $(eval 'echo ${as_decl_name}') superseded.])
    eval "ac_cv_func_${as_decl_name}=no"
    _mask_varname=HAVE_`eval "echo '${as_decl_name}'" | tr 'a-z' 'A-Z'`
    sed --in-place "s~^#define ${_mask_varname} 1$~~" confdefs.h
fi
], [[
#ifdef HAVE_SYS_SOCKET_H
    #include <sys/socket.h>
#endif
#ifdef HAVE_STRING_H
    #include <string.h>
#endif
#ifdef HAVE_NETDB_H
    #include <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
    #include <arpa/inet.h>
#endif
#ifdef HAVE_SYS_TIME_H
    #include <sys/time.h>
#endif
#ifdef HAVE_TIME_H
    #include <time.h>
#endif
#ifdef HAVE_STDLIB_H
    #include <stdlib.h>
#endif
]])

AC_PROG_INSTALL
AC_TYPE_SIZE_T
AC_TYPE_UINT8_T
AC_TYPE_UINTPTR_T
AM_PROG_AS

OPTIMIZE_CFLAGS="-Os"
OPTIMIZE_FAST_CFLAGS="-O2"
OPTIMIZE_HUGE_CFLAGS="-funroll-loops -DTFM_SMALL_SET -DTFM_HUGE_SET"
DEBUG_CFLAGS="-g -DDEBUG -DDEBUG_WOLFSSL"
LIB_ADD=
LIB_STATIC_ADD=

if test "$output_objdir" = ""
then
    output_objdir=.
fi


# Thread local storage
thread_ls_on="no"
AC_ARG_ENABLE([threadlocal],
    [AS_HELP_STRING([--enable-threadlocal],[Enable thread local support (default: enabled)])],
    [ ENABLED_THREADLOCAL=$enableval ],
    [ ENABLED_THREADLOCAL=yes ]
    )
if test "$ENABLED_THREADLOCAL" = "yes"
then
    AX_TLS([thread_ls_on=yes],[thread_ls_on=no])
    AS_IF([test "x$thread_ls_on" = "xyes"],[AM_CFLAGS="$AM_CFLAGS -DHAVE_THREAD_LS"])
fi


# DEBUG
AX_DEBUG
AS_IF([test "$ax_enable_debug" = "yes"],
      [AM_CFLAGS="$AM_CFLAGS $DEBUG_CFLAGS"],
      [AM_CFLAGS="$AM_CFLAGS -DNDEBUG"])
AS_IF([test "$ax_enable_debug" = "yes"],
      [AM_CCASFLAGS="$DEBUG_CFLAGS $AM_CCASFLAGS"],
      [AM_CCASFLAGS="$AM_CCASFLAGS -DNDEBUG"])

# Start without certificates enabled and enable if a certificate algorithm is
# enabled
ENABLED_CERTS="no"

# Implements requirements from RFC9325
AC_ARG_ENABLE([harden-tls],
    [AS_HELP_STRING([--enable-harden-tls],[Enable requirements from RFC9325. Possible values are <yes>, <112>, or <128>. <yes> is equivalent to <112>. (default: disabled)])],
    [ ENABLED_HARDEN_TLS=$enableval ],
    [ ENABLED_HARDEN_TLS=no ]
    )

if test "x$ENABLED_HARDEN_TLS" != "xno"
then
    if test "x$ENABLED_HARDEN_TLS" = "xyes" || test "x$ENABLED_HARDEN_TLS" = "x112"
    then
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_HARDEN_TLS=112"
    elif test "x$ENABLED_HARDEN_TLS" = "x128"
    then
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_HARDEN_TLS=128"
    else
        AC_MSG_ERROR([Invalid value for --enable-harden-tls])
    fi
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_EXTRA_ALERTS -DWOLFSSL_CHECK_ALERT_ON_ERR"
fi

# Support for forcing 32-bit mode
# To force 32-bit instructions use:
#   ./configure CFLAGS="-m32" LDFLAGS="-m32" && make
# The checks for sizeof long and long/long are run at the top of configure and require "-m32" to be set directly in the ./configure statement.
AC_ARG_ENABLE([32bit],
    [AS_HELP_STRING([--enable-32bit],[Enables 32-bit support (default: disabled)])],
    [ ENABLED_32BIT=$enableval ],
    [ ENABLED_32BIT=no ]
    )

# 16-bit compiler support
AC_ARG_ENABLE([16bit],
    [AS_HELP_STRING([--enable-16bit],[Enables 16-bit support (default: disabled)])],
    [ ENABLED_16BIT=$enableval ],
    [ ENABLED_16BIT=no ]
    )
if test "$ENABLED_16BIT" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWC_16BIT_CPU"
fi

AC_ARG_ENABLE([64bit],
    [AS_HELP_STRING([--enable-64bit],[Enables 64-bit support (default: disabled)])],
    [ ENABLED_64BIT=$enableval ],
    [ ENABLED_64BIT=no ]
    )

AC_ARG_ENABLE([kdf],
    [AS_HELP_STRING([--enable-kdf],[Enables kdf support (default: enabled)])],
    [ ENABLED_KDF=$enableval ],
    [ ENABLED_KDF=yes ]
    )

AC_ARG_ENABLE([hmac],
    [AS_HELP_STRING([--enable-hmac],[Enables HMAC support (default: enabled)])],
    [ ENABLED_HMAC=$enableval ],
    [ ENABLED_HMAC=yes ]
    )

AC_ARG_ENABLE([do178],
    [AS_HELP_STRING([--enable-do178],[Enable DO-178, Will NOT work w/o DO178 license (default: disabled)])],
    [ENABLED_DO178=$enableval],
    [ENABLED_DO178="no"])

if test "$ENABLED_DO178" = "no"
then
    AM_CFLAGS="$AM_CFLAGS -DNO_DO178"
else
    AM_CFLAGS="$AM_CFLAGS -DHAVE_DO178"
fi
# Support for disabling all ASM
AC_ARG_ENABLE([asm],
    [AS_HELP_STRING([--enable-asm],[Enables option for assembly (default: enabled)])],
    [ ENABLED_ASM=$enableval ],
    [ ENABLED_ASM=yes ]
    )

if test "$ENABLED_ASM" = "no"
then
    AM_CFLAGS="$AM_CFLAGS -DTFM_NO_ASM -DWOLFSSL_NO_ASM"
fi
AC_SUBST([ENABLED_ASM])


# Default math is SP Math all and not fast math
# FIPS v1 and v2 must use fast math
DEF_SP_MATH="yes"
DEF_FAST_MATH="no"

# FIPS 140
AC_ARG_ENABLE([fips],
    [AS_HELP_STRING([--enable-fips],[Enable FIPS 140-2, Will NOT work w/o FIPS license (default: disabled)])],
    [ENABLED_FIPS=$enableval],
    [ENABLED_FIPS="no"])

# wolfEngine Options
AC_ARG_ENABLE([engine],
    [AS_HELP_STRING([--enable-engine],[Enable wolfEngine options (default: disabled)])],
    [ ENABLED_WOLFENGINE=$enableval ],
    [ ENABLED_WOLFENGINE=no ]
    )

AS_CASE([$ENABLED_WOLFENGINE],
    [no],[
        ENABLED_WOLFENGINE="no"
    ],
    [disabled],[
        ENABLED_WOLFENGINE="no"
    ],
    [yes|fips-v2|cert3389],[
        ENABLED_WOLFENGINE="yes"
        ENABLED_FIPS="v2"
    ],
    [fips-v5],[
        ENABLED_WOLFENGINE="yes"
        ENABLED_FIPS="v5"
    ],
    [fips-ready],[
        ENABLED_WOLFENGINE="yes"
        ENABLED_FIPS="ready"
    ],
    [no-fips],[
        ENABLED_WOLFENGINE="yes"
        ENABLED_FIPS="no"
    ],
    [
        AC_MSG_ERROR([Invalid value for --enable-engine "$ENABLED_WOLFENGINE" (options: fips-v2, fips-ready, no-fips, no, disabled)])
    ])

# The FIPS options are:
#   no - FIPS build disabled, FIPS sources forbidden in build tree
#   disabled - FIPS build disabled, FIPS sources ignored in build tree
#   v1 - FIPS 140-2 Cert 2425
#   default - same as v1
#   v2 - FIPS 140-2 Cert 3389
#   cert3389 - alias for v2
#   rand - wolfRand
#   v5-RC12 - FIPS 140-3, wolfCrypt/fips WCv5.0-RC12
#   v5 - currently, alias for v5-RC12
#   ready - FIPS 140-3 settings with in-tree wolfcrypt sources, feature locked
#   dev - FIPS 140-3 settings with in-tree wolfcrypt sources, features freely adjustable
#   v5-ready - Alias for ready.
#   v5-dev - Alias for dev.
#
# These options have been retired, but are listed here for historical reference:
#   v5-RC8 - historical FIPS 140-3 (wolfCrypt WCv5.0-RC8).
#            HAVE_FIPS_VERSION = 5, HAVE_FIPS_VERSION_MINOR = 0.
#   v5-RC9 - historical FIPS 140-3 (wolfCrypt WCv5.0-RC9)
#            HAVE_FIPS_VERSION = 5, HAVE_FIPS_VERSION_MINOR = 1.
#   v5-RC10 - historical FIPS 140-3, wolfCrypt/fips WCv5.0-RC10
#             HAVE_FIPS_VERSION = 5, HAVE_FIPS_VERSION_MINOR = 2.
#   v5-RC11 - historical FIPS 140-3, wolfCrypt/fips WCv5.0-RC11
#             HAVE_FIPS_VERSION = 5, HAVE_FIPS_VERSION_MINOR = 2.
AS_CASE([$ENABLED_FIPS],
    [no],[
        FIPS_VERSION="none"
    ],
    [disabled],[
        FIPS_VERSION="disabled"
        ENABLED_FIPS="no"
    ],
    [v1|yes|cert2425],[
        FIPS_VERSION="v1"
        HAVE_FIPS_VERSION=1
        ENABLED_FIPS="yes"
        DEF_SP_MATH="no"
        DEF_FAST_MATH="yes"
    ],
    [v2|cert3389],[
        FIPS_VERSION="v2"
        HAVE_FIPS_VERSION=2
        HAVE_FIPS_VERSION_MINOR=0
        ENABLED_FIPS="yes"
        DEF_SP_MATH="no"
        DEF_FAST_MATH="yes"
    ],
    [rand],[
        FIPS_VERSION="rand"
        HAVE_FIPS_VERSION=2
        HAVE_FIPS_VERSION_MINOR=1
        ENABLED_FIPS="yes"
        DEF_SP_MATH="no"
        DEF_FAST_MATH="no"
    ],
    [v5|v5-RC12],[
        FIPS_VERSION="v5-RC12"
        HAVE_FIPS_VERSION=5
        HAVE_FIPS_VERSION_MINOR=2
        ENABLED_FIPS="yes"
        DEF_SP_MATH="no"
        DEF_FAST_MATH="yes"
    ],
    [ready|v5-ready],[
        FIPS_VERSION="ready"
        HAVE_FIPS_VERSION=5
        HAVE_FIPS_VERSION_MINOR=3
        ENABLED_FIPS="yes"
        DEF_SP_MATH="no"
        DEF_FAST_MATH="yes"
    ],
    [dev|v5-dev],[
        FIPS_VERSION="dev"
        HAVE_FIPS_VERSION=5
        HAVE_FIPS_VERSION_MINOR=3
        ENABLED_FIPS="yes"
        # for dev, DEF_SP_MATH and DEF_FAST_MATH follow non-FIPS defaults (currently sp-math-all)
    ],
    [
        AC_MSG_ERROR([Invalid value for --enable-fips "$ENABLED_FIPS" (main options: v1, v2, v5, ready, dev, rand, no, disabled)])
    ])

if test -z "$HAVE_FIPS_VERSION_MINOR"
then
    HAVE_FIPS_VERSION_MINOR=0
fi
if test -z "$HAVE_FIPS_VERSION"
then
    HAVE_FIPS_VERSION=0
fi

if test "$ENABLED_FIPS" != "no"
then
    REPRODUCIBLE_BUILD_DEFAULT=yes
fi

AS_CASE([$FIPS_VERSION],
    [none],
    [
      AS_IF([ test -s $srcdir/wolfcrypt/src/fips.c ],
            [AC_MSG_ERROR([FIPS source tree is incompatible with non-FIPS build (requires --enable-fips)])])
    ],
    [disabled],
    [],
    [
      AS_IF([ ! test -s $srcdir/wolfcrypt/src/fips.c],
            [AC_MSG_ERROR([non-FIPS source tree is incompatible with --enable-fips=$enableval])])
    ]
)

# For reproducible build, gate out from the build anything that might
# introduce semantically frivolous jitter, maximizing chance of
# identical object files.
AC_ARG_ENABLE([reproducible-build],
    [AS_HELP_STRING([--enable-reproducible-build],[Enable maximally reproducible build (default: disabled)])],
    [ ENABLED_REPRODUCIBLE_BUILD=$enableval ],
    [ ENABLED_REPRODUCIBLE_BUILD=$REPRODUCIBLE_BUILD_DEFAULT ]
    )

if test "$ENABLED_REPRODUCIBLE_BUILD" = "yes"
then
    # Test ar for the "D" option. Should be checked before the libtool macros.
    if test -z "$AR"; then
        AR=ar
    fi
    xxx_ar_flags=$(${AR} --help 2>&1)
    if test -z "$RANLIB"; then
        RANLIB=ranlib
    fi
    xxx_ranlib_flags=$(${RANLIB} --help 2>&1)
    AM_CFLAGS="$AM_CFLAGS -DHAVE_REPRODUCIBLE_BUILD"
    AS_CASE([$xxx_ar_flags],[*'use zero for timestamps and uids/gids'*],[AR_FLAGS="Dcr" lt_ar_flags="Dcr"])
    AS_CASE([$xxx_ranlib_flags],[*'Use zero for symbol map timestamp'*],[RANLIB="${RANLIB} -D"])
fi


AC_ARG_ENABLE([benchmark],
    [AS_HELP_STRING([--enable-benchmark],[Build benchmark when building crypttests (default: enabled)])],
    [ENABLED_BENCHMARK=$enableval],
    [ENABLED_BENCHMARK=yes]
    )


# Linux Kernel Module
AC_ARG_ENABLE([linuxkm],
    [AS_HELP_STRING([--enable-linuxkm],[Enable Linux Kernel Module (default: disabled)])],
    [ENABLED_LINUXKM=$enableval],
    [ENABLED_LINUXKM=no]
    )

AC_ARG_ENABLE([linuxkm-defaults],
    [AS_HELP_STRING([--enable-linuxkm-defaults],[Enable feature defaults for Linux Kernel Module (default: disabled)])],
    [ENABLED_LINUXKM_DEFAULTS=$enableval],
    [ENABLED_LINUXKM_DEFAULTS=$ENABLED_LINUXKM]
    )

AC_ARG_ENABLE([linuxkm-pie],
    [AS_HELP_STRING([--enable-linuxkm-pie],[Enable relocatable object build of Linux kernel module (default: disabled)])],
    [ENABLED_LINUXKM_PIE=$enableval],
    [ENABLED_LINUXKM_PIE=$ENABLED_FIPS]
    )
if test "$ENABLED_LINUXKM_PIE" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_LINUXKM_PIE_SUPPORT"
elif test "$ENABLED_FIPS" = yes && test "$ENABLED_LINUXKM" = yes
then
    AC_MSG_ERROR([FIPS linuxkm requires linuxkm-pie.])
fi
AC_SUBST([ENABLED_LINUXKM_PIE])

AC_ARG_ENABLE([linuxkm-benchmarks],
    [AS_HELP_STRING([--enable-linuxkm-benchmarks],[Enable crypto benchmarking autorun at module load time for Linux kernel module (default: disabled)])],
    [ENABLED_LINUXKM_BENCHMARKS=$enableval],
    [ENABLED_LINUXKM_BENCHMARKS=no]
    )
if test "$ENABLED_LINUXKM_BENCHMARKS" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_LINUXKM_BENCHMARKS"
fi
AC_SUBST([ENABLED_LINUXKM_BENCHMARKS])

if test "$ENABLED_LINUXKM_DEFAULTS" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_DH_CONST -DWOLFSSL_SP_MOD_WORD_RP -DWOLFSSL_SP_DIV_64 -DWOLFSSL_SP_DIV_WORD_HALF -DWOLFSSL_SMALL_STACK_STATIC -DWOLFSSL_TEST_SUBROUTINE=static"
    if test "$ENABLED_LINUXKM_PIE" = "yes"; then
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_NO_OCSP_ISSUER_CHECK"
    fi
    if test "$ENABLED_FIPS" = "no"; then
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_OLD_PRIME_CHECK"
    fi
    DEF_SP_MATH="yes"
    DEF_FAST_MATH="no"
fi

AC_ARG_WITH([linux-source],
    [AS_HELP_STRING([--with-linux-source=PATH],[PATH to root of Linux kernel build tree])],
    [KERNEL_ROOT=$withval],
    [KERNEL_ROOT=""])

AC_ARG_WITH([linux-arch],
    [AS_HELP_STRING([--with-linux-arch=arch],[built arch (SRCARCH) of Linux kernel build tree])],
    [KERNEL_ARCH=$withval],
    [KERNEL_ARCH=""])

if test "x$ENABLED_LINUXKM" = "xyes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_LINUXKM"
    ENABLED_NO_LIBRARY=yes
    ENABLED_BENCHMARK=no
    output_objdir="$(realpath "$output_objdir")/linuxkm"

    if test "$KERNEL_ROOT" = ""; then
        AC_PATH_DEFAULT_KERNEL_SOURCE
        KERNEL_ROOT="$DEFAULT_KERNEL_ROOT"
    fi
    AC_SUBST([KERNEL_ROOT])

    if test "$KERNEL_ARCH" = ""; then
        AC_DEFAULT_KERNEL_ARCH
        KERNEL_ARCH="$DEFAULT_KERNEL_ARCH"
    fi
    AC_SUBST([KERNEL_ARCH])

    if test "${KERNEL_ROOT}" = ""; then
        AC_MSG_ERROR([Linux kernel source root not found -- supply with --with-linux-source=PATH.])
    fi
    if test "${KERNEL_ARCH}" = ""; then
        AC_MSG_ERROR([Linux kernel target architecture for build tree ${KERNEL_ROOT} could not be determined.  Is target kernel configured?])
    fi

    AM_CFLAGS="$AM_CFLAGS -DNO_DEV_RANDOM -DNO_WRITEV -DNO_STDIO_FILESYSTEM -DWOLFSSL_NO_SOCK -DWOLFSSL_USER_IO"
fi


# MATH LIBRARY SELECTION

# Single Precision maths implementation
AC_ARG_ENABLE([sp],
    [AS_HELP_STRING([--enable-sp],[Enable Single Precision maths implementation (default: disabled)])],
    [ ENABLED_SP=$enableval ],
    [ ENABLED_SP=$ENABLED_SP_DEFAULT ],
    )

AC_ARG_ENABLE([sp-math-all],
    [AS_HELP_STRING([--enable-sp-math-all],[Enable Single Precision math implementation for full algorithm suite (default: enabled)])],
    [ ENABLED_SP_MATH_ALL=$enableval ],
    [ ENABLED_SP_MATH_ALL=$DEF_SP_MATH ],
    )

# Single Precision maths (acceleration for common key sizes and curves)
if test "$ENABLED_LINUXKM_DEFAULTS" = "yes" && test "$ENABLED_SP" != "no" && test "$ENABLED_SP_MATH_ALL" = "no"
then
    ENABLED_SP_MATH_DEFAULT=yes
else
    ENABLED_SP_MATH_DEFAULT=no
fi
AC_ARG_ENABLE([sp-math],
    [AS_HELP_STRING([--enable-sp-math],[Enable Single Precision math implementation with restricted algorithm suite (default: disabled)])],
    [ ENABLED_SP_MATH=$enableval ],
    [ ENABLED_SP_MATH=$ENABLED_SP_MATH_DEFAULT ],
    )

if test "$enable_sp_math" != ""
then
    # When the restricted SP Math is selected and not SP Math ALL, then disable
    # SP Math ALL.
    if test "$enable_sp_math" != "no" && test "$enable_sp_math_all" = ""
    then
        ENABLED_SP_MATH_ALL="no"
    else
        # Can't choose restricted and unrestricted SP Math
        if test "$enable_sp_math" != "no" && test "$enable_sp_math_all" != "no"
        then
            AC_MSG_ERROR([--enable-sp-math and --enable-sp-math-all are incompatible. Use --enable-sp-math-all only when all key sizes need to be supported.])
        fi
    fi
fi

# enable SP math assembly support automatically for x86_64 and aarch64 (except Linux kernel module)
SP_ASM_DEFAULT=no
if test "$ENABLED_SP_MATH" = "yes" && test "$ENABLED_LINUXKM_DEFAULTS" = "no"
then
    if test "$host_cpu" = "x86_64" || test "$host_cpu" = "aarch64" || test "$host_cpu" = "amd64"
    then
        SP_ASM_DEFAULT=yes
    fi
fi
AC_ARG_ENABLE([sp-asm],
    [AS_HELP_STRING([--enable-sp-asm],[Enable Single Precision assembly implementation (default: enabled on x86_64/aarch64/amd64)])],
    [ ENABLED_SP_ASM=$enableval ],
    [ ENABLED_SP_ASM=$SP_ASM_DEFAULT ],
    )

if test "$ENABLED_SP_ASM" != "no"
then
    if test "$ENABLED_SP" = "no"
    then
        AC_MSG_ERROR([--enable-sp-asm requires SP to be enabled.])
    fi
    if test "$ENABLED_SP" = ""
    then
        ENABLED_SP=yes
    fi
fi

# fastmath
AC_ARG_ENABLE([fastmath],
    [AS_HELP_STRING([--enable-fastmath],[Enable fast math ops (default: disabled)])],
    [ ENABLED_FASTMATH=$enableval ],
    [ ENABLED_FASTMATH=$DEF_FAST_MATH ]
    )

# fast HUGE math
AC_ARG_ENABLE([fasthugemath],
    [AS_HELP_STRING([--enable-fasthugemath],[Enable fast math + huge code (default: disabled)])],
    [ ENABLED_FASTHUGEMATH=$enableval ],
    [ ENABLED_FASTHUGEMATH=no ]
    )

if test "$ENABLED_BUMP" = "yes"
then
    ENABLED_FASTHUGEMATH="yes"
fi

if test "$ENABLED_FASTHUGEMATH" = "yes"
then
    ENABLED_FASTMATH="yes"
fi

if test "$host_cpu" = "x86_64" || test "$host_cpu" = "amd64"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_X86_64_BUILD"
fi
if test "$host_cpu" = "x86"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_X86_BUILD"
fi

# if sp-math-all is not set, then enable fast math
if test "x$ENABLED_FASTMATH" = "xyes" && test "$enable_sp_math_all" = "" && test "$enable_sp_math" = ""
then
    # turn off fastmath if leanpsk on or asn off (w/o DH and ECC)
    if test "$ENABLED_LEANPSK" = "yes" || test "$ENABLED_ASN" = "no"
    then
        if test "$ENABLED_DH" = "no" && test "$ENABLED_ECC" = "no" && test "$ENABLED_RSA" = "no"
        then
            ENABLED_FASTMATH="no"
        else
            AM_CFLAGS="$AM_CFLAGS -DUSE_FAST_MATH"
            ENABLED_HEAPMATH="no"
        fi
    else
        AM_CFLAGS="$AM_CFLAGS -DUSE_FAST_MATH"
        ENABLED_HEAPMATH="no"
        ENABLED_SP_MATH_ALL="no"
    fi
    AS_IF([test "x$host_cpu" = "xaarch64"],[AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_AARCH64_BUILD"])

    if test "$ENABLED_SAKKE" = "yes" && test "$ENABLED_SAKKE_SMALL" != "yes"
    then
        AM_CFLAGS="$AM_CFLAGS -funroll-loops -DTFM_SMALL_SET"
    fi
fi


# heap based integer.c math (not timing resistant)
AC_ARG_ENABLE([heapmath],
    [AS_HELP_STRING([--enable-heapmath],[Enable heap based integer.c math ops (default: disabled)])],
    [ ENABLED_HEAPMATH=$enableval ],
    [ ENABLED_HEAPMATH=no]
    )
if test "x$ENABLED_HEAPMATH" = "xyes"
then
    AM_CFLAGS="$AM_CFLAGS -DUSE_INTEGER_HEAP_MATH"
    ENABLED_HEAPMATH="yes"
    ENABLED_SP="no"
    ENABLED_SP_MATH_ALL="no"
fi



# ALL FEATURES
AC_ARG_ENABLE([all],
    [AS_HELP_STRING([--enable-all],[Enable all wolfSSL features, except SSLv3 (default: disabled)])],
    [ ENABLED_ALL=$enableval ],
    [ ENABLED_ALL=no ]
    )
if test "$ENABLED_ALL" = "yes"
then
    test "$enable_dtls" = "" && enable_dtls=yes
    if test "x$FIPS_VERSION" != "xv1"
    then
        test "$enable_tls13" = "" && enable_tls13=yes
        test "$enable_rsapss" = "" && enable_rsapss=yes
    fi

    # this set is also enabled by enable-all-crypto:
    test "$enable_atomicuser" = "" && enable_atomicuser=yes
    test "$enable_aesgcm" = "" && enable_aesgcm=yes
    test "$enable_aesgcm_stream" = "" && enable_aesgcm_stream=yes
    test "$enable_aesccm" = "" && enable_aesccm=yes
    test "$enable_aesctr" = "" && enable_aesctr=yes
    test "$enable_aesofb" = "" && enable_aesofb=yes
    test "$enable_aescfb" = "" && enable_aescfb=yes
    test "$enable_aescbc_length_checks" = "" && enable_aescbc_length_checks=yes
    test "$enable_camellia" = "" && enable_camellia=yes
    test "$enable_ripemd" = "" && enable_ripemd=yes
    test "$enable_sha224" = "" && enable_sha224=yes
    test "$enable_shake128" = "" && enable_shake128=yes
    test "$enable_shake256" = "" && enable_shake256=yes
    test "$enable_sessioncerts" = "" && enable_sessioncerts=yes
    test "$enable_keygen" = "" && enable_keygen=yes
    test "$enable_certgen" = "" && enable_certgen=yes
    test "$enable_certreq" = "" && enable_certreq=yes
    test "$enable_certext" = "" && enable_certext=yes
    test "$enable_sep" = "" && enable_sep=yes
    test "$enable_hkdf" = "" && enable_hkdf=yes
    test "$enable_curve25519" = "" && enable_curve25519=yes
    test "$enable_curve448" = "" && enable_curve448=yes
    test "$enable_fpecc" = "" && enable_fpecc=yes
    test "$enable_eccencrypt" = "" && enable_eccencrypt=yes
    test "$enable_psk" = "" && enable_psk=yes
    test "$enable_cmac" = "" && enable_cmac=yes
    test "$enable_siphash" = "" && enable_siphash=yes
    test "$enable_xts" = "" && enable_xts=yes
    test "$enable_ocsp" = "" && enable_ocsp=yes
    test "$enable_ocspstapling" = "" && enable_ocspstapling=yes
    test "$enable_ocspstapling2" = "" && enable_ocspstapling2=yes
    test "$enable_crl" = "" && enable_crl=yes
    test "$enable_supportedcurves" = "" && enable_supportedcurves=yes
    test "$enable_tlsx" = "" && enable_tlsx=yes
    test "$enable_pwdbased" = "" && enable_pwdbased=yes
    test "$enable_aeskeywrap" = "" && enable_aeskeywrap=yes
    test "$enable_x963kdf" = "" && enable_x963kdf=yes
    test "$enable_scrypt" = "" && enable_scrypt=yes
    test "$enable_indef" = "" && enable_indef=yes
    test "$enable_enckeys" = "" && enable_enckeys=yes
    test "$enable_hashflags" = "" && enable_hashflags=yes
    test "$enable_defaultdhparams" = "" && enable_defaultdhparams=yes
    test "$enable_base64encode" = "" && enable_base64encode=yes
    test "$enable_base16" = "" && enable_base16=yes
    test "$enable_arc4" = "" && enable_arc4=yes
    test "$enable_des3" = "" && enable_des3=yes
    test "$enable_blake2" = "" && enable_blake2=yes
    test "$enable_blake2s" = "" && enable_blake2s=yes
    test "$enable_md2" = "" && enable_md2=yes
    test "$enable_md4" = "" && enable_md4=yes
    test "$enable_cryptocb" = "" && enable_cryptocb=yes
    test "$enable_anon" = "" && enable_anon=yes
    test "$enable_ssh" = "" && enable_ssh=yes

    test "$enable_savesession" = "" && enable_savesession=yes
    test "$enable_savecert" = "" && enable_savecert=yes
    test "$enable_postauth" = "" && enable_postauth=yes
    test "$enable_hrrcookie" = "" && enable_hrrcookie=yes
    test "$enable_fallback_scsv" = "" && enable_fallback_scsv=yes
    test "$enable_webserver" = "" && enable_webserver=yes
    test "$enable_crl_monitor" = "" && enable_crl_monitor=yes
    test "$enable_sni" = "" && enable_sni=yes
    test "$enable_maxfragment" = "" && enable_maxfragment=yes
    test "$enable_alpn" = "" && enable_alpn=yes
    test "$enable_truncatedhmac" = "" && enable_truncatedhmac=yes
    test "$enable_trusted_ca" = "" && enable_trusted_ca=yes
    test "$enable_session_ticket" = "" && enable_session_ticket=yes
    test "$enable_earlydata" = "" && enable_earlydata=yes
    test "$enable_ech" = "" && enable_ech=yes

    if test "$ENABLED_32BIT" != "yes"
    then
        test "$enable_sha512" = "" && enable_sha512=yes
        test "$enable_sha3" = "" && enable_sha3=yes
    fi

    if test "$ENABLED_LINUXKM_DEFAULTS" != "yes"
    then
        test "$enable_compkey" = "" && enable_compkey=yes
        AM_CFLAGS="$AM_CFLAGS -DHAVE_CRL_IO -DHAVE_IO_TIMEOUT"
    fi

    # sp-math is incompatible with opensslextra, ECC custom curves, and DSA.
    if test "$ENABLED_SP_MATH" = "no"
    then
        test "$enable_dsa" = "" && enable_dsa=yes
        test "$enable_ecccustcurves" = "" && enable_ecccustcurves=yes
        test "$enable_brainpool" = "" && enable_brainpool=yes
        test "$enable_srp" = "" && enable_srp=yes
        # linuxkm is incompatible with opensslextra and its dependents.
        if test "$ENABLED_LINUXKM_DEFAULTS" != "yes"
        then
            if test "$ENABLED_FIPS" = "no"
            then
                if test "$ENABLED_32BIT" != "yes"
                then
                    test "$enable_openssh" = "" && enable_openssh=yes
                fi
                # S/MIME support requires PKCS7, which requires no FIPS.
                test "$enable_smime" = "" && enable_smime=yes
            fi
            test "$enable_opensslextra" = "" && enable_opensslextra=yes
            test "$enable_opensslall" = "" && enable_opensslall=yes
            test "$enable_certservice" = "" && enable_certservice=yes
            test "$enable_lighty" = "" && enable_lighty=yes
            test "$enable_nginx" = "" && enable_nginx=yes
            test "$enable_openvpn" = "" && enable_openvpn=yes
            test "$enable_asio" = "" && enable_asio=yes
            test "$enable_libwebsockets" = "" && enable_libwebsockets=yes
            test "$enable_qt" = "" && enable_qt=yes
        fi
    fi

    if test "$ENABLED_FIPS" = "no"
    then
        test "$enable_pkcallbacks" = "" && enable_pkcallbacks=yes
        test "$enable_xchacha" = "" && enable_xchacha=yes
        test "$enable_scep" = "" && enable_scep=yes
        test "$enable_pkcs7" = "" && enable_pkcs7=yes
        test "$enable_nullcipher" = "" && enable_nullcipher=yes
        test "$enable_mcast" = "" && enable_mcast=yes
        if test "$ENABLED_32BIT" != "yes"
        then
            test "$enable_ed25519" = "" && enable_ed25519=yes
            test "$enable_ed25519_stream" = "" && enable_ed25519_stream=yes
            test "$enable_ed448" = "" && enable_ed448=yes
            test "$enable_ed448_stream" = "" && enable_ed448_stream=yes
        fi

        if test "$ENABLED_LINUXKM_DEFAULTS" != "yes"
        then
            # these use DES3:
            test "$enable_stunnel" = "" && enable_stunnel=yes
            test "$enable_curl" = "" && enable_curl=yes
            test "$enable_tcpdump" = "" && enable_tcpdump=yes

            test "$enable_eccsi" = "" && enable_eccsi=yes
            test "$enable_sakke" = "" && enable_sakke=yes
        fi
    fi

    if test "$ENABLED_FIPS" = "no" || test "$ENABLED_FIPS" = "dev"; then
        test "$enable_aessiv" = "" && enable_aessiv=yes
    fi

    # Enable DH const table speedups (eliminates `-lm` math lib dependency)
    AM_CFLAGS="$AM_CFLAGS -DHAVE_FFDHE_2048 -DHAVE_FFDHE_3072"
    DEFAULT_MAX_CLASSIC_ASYM_KEY_BITS=4096

    # Enable multiple attribute additions such as DC
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_MULTI_ATTRIB"

    # Enable AES Decrypt, AES ECB
    AM_CFLAGS="$AM_CFLAGS -DHAVE_AES_DECRYPT -DHAVE_AES_ECB"

    # Enable Alt Names, DER Load, Keep Certs, CRL IO with Timeout
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_ALT_NAMES -DWOLFSSL_DER_LOAD -DKEEP_OUR_CERT -DKEEP_PEER_CERT"

    # Enable DH Extra
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_DH_EXTRA"

    # Enable deterministic ECC signing API with variant
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_ECDSA_DETERMINISTIC_K_VARIANT"

    # Store issuer name components when parsing certificates.
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_HAVE_ISSUER_NAMES"

    # Certificate extensions and alt. names for FPKI use
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SUBJ_DIR_ATTR -DWOLFSSL_FPKI -DWOLFSSL_SUBJ_INFO_ACC"

    # Handle as many subject/issuer name OIDs as possible
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_CERT_NAME_ALL"

    # More thorough error queue usage.
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_VERBOSE_ERRORS"
fi


# ALL CRYPTO FEATURES
AC_ARG_ENABLE([all-crypto],
    [AS_HELP_STRING([--enable-all-crypto],[Enable all wolfcrypt algorithms (default: disabled)])],
    [ ENABLED_ALL_CRYPT=$enableval ],
    [ ENABLED_ALL_CRYPT=no ]
    )
if test "$ENABLED_ALL_CRYPT" = "yes"
then
    test "$enable_atomicuser" = "" && enable_atomicuser=yes
    test "$enable_aesgcm" = "" && enable_aesgcm=yes
    test "$enable_aesgcm_stream" = "" && enable_aesgcm_stream=yes
    test "$enable_aesccm" = "" && enable_aesccm=yes
    test "$enable_aesctr" = "" && enable_aesctr=yes
    test "$enable_aesofb" = "" && enable_aesofb=yes
    test "$enable_aescfb" = "" && enable_aescfb=yes
    test "$enable_aescbc_length_checks" = "" && enable_aescbc_length_checks=yes
    test "$enable_camellia" = "" && enable_camellia=yes
    test "$enable_ripemd" = "" && enable_ripemd=yes
    test "$enable_sha224" = "" && enable_sha224=yes
    test "$enable_shake128" = "" && enable_shake128=yes
    test "$enable_shake256" = "" && enable_shake256=yes
    test "$enable_sessioncerts" = "" && enable_sessioncerts=yes
    test "$enable_keygen" = "" && enable_keygen=yes
    test "$enable_certgen" = "" && enable_certgen=yes
    test "$enable_certreq" = "" && enable_certreq=yes
    test "$enable_certext" = "" && enable_certext=yes
    test "$enable_sep" = "" && enable_sep=yes
    test "$enable_hkdf" = "" && enable_hkdf=yes
    test "$enable_curve25519" = "" && enable_curve25519=yes
    test "$enable_curve448" = "" && enable_curve448=yes
    test "$enable_fpecc" = "" && enable_fpecc=yes
    test "$enable_eccencrypt" = "" && enable_eccencrypt=yes
    test "$enable_psk" = "" && enable_psk=yes
    test "$enable_cmac" = "" && enable_cmac=yes
    test "$enable_siphash" = "" && enable_siphash=yes
    test "$enable_xts" = "" && enable_xts=yes
    test "$enable_ocsp" = "" && enable_ocsp=yes
    test "$enable_ocspstapling" = "" && enable_ocspstapling=yes
    test "$enable_ocspstapling2" = "" && enable_ocspstapling2=yes
    test "$enable_crl" = "" && enable_crl=yes
    test "$enable_supportedcurves" = "" && enable_supportedcurves=yes
    test "$enable_tlsx" = "" && enable_tlsx=yes
    test "$enable_pwdbased" = "" && enable_pwdbased=yes
    test "$enable_aeskeywrap" = "" && enable_aeskeywrap=yes
    test "$enable_x963kdf" = "" && enable_x963kdf=yes
    test "$enable_scrypt" = "" && enable_scrypt=yes
    test "$enable_indef" = "" && enable_indef=yes
    test "$enable_enckeys" = "" && enable_enckeys=yes
    test "$enable_hashflags" = "" && enable_hashflags=yes
    test "$enable_defaultdhparams" = "" && enable_defaultdhparams=yes
    test "$enable_base64encode" = "" && enable_base64encode=yes
    test "$enable_base16" = "" && enable_base16=yes
    test "$enable_arc4" = "" && enable_arc4=yes
    test "$enable_des3" = "" && enable_des3=yes
    test "$enable_blake2" = "" && enable_blake2=yes
    test "$enable_blake2s" = "" && enable_blake2s=yes
    test "$enable_md2" = "" && enable_md2=yes
    test "$enable_md4" = "" && enable_md4=yes
    test "$enable_cryptocb" = "" && enable_cryptocb=yes
    test "$enable_anon" = "" && enable_anon=yes
    test "$enable_ssh" = "" && enable_ssh=yes

    if test "$ENABLED_32BIT" != "yes"
    then
        test "$enable_sha512" = "" && enable_sha512=yes
        test "$enable_sha3" = "" && enable_sha3=yes
    fi

    if test "$ENABLED_LINUXKM_DEFAULTS" != "yes"
    then
        test "$enable_compkey" = "" && enable_compkey=yes
    fi

    if test "$ENABLED_SP_MATH" = "no"
    then
        test "$enable_dsa" = "" && enable_dsa=yes
        test "$enable_ecccustcurves" = "" && enable_ecccustcurves=yes
        test "$enable_brainpool" = "" && enable_brainpool=yes
        test "$enable_srp" = "" && enable_srp=yes
    fi

    if test "$ENABLED_FIPS" = "no"
    then
        test "$enable_pkcallbacks" = "" && enable_pkcallbacks=yes
        test "$enable_xchacha" = "" && enable_xchacha=yes
        test "$enable_pkcs7" = "" && enable_pkcs7=yes
        test "$enable_nullcipher" = "" && enable_nullcipher=yes
        if test "$ENABLED_32BIT" != "yes"
        then
            test "$enable_ed25519" = "" && enable_ed25519=yes
            test "$enable_ed25519_stream" = "" && enable_ed25519_stream=yes
            test "$enable_ed448" = "" && enable_ed448=yes
            test "$enable_ed448_stream" = "" && enable_ed448_stream=yes
        fi

        if test "$ENABLED_LINUXKM_DEFAULTS" != "yes"
        then
            test "$enable_eccsi" = "" && enable_eccsi=yes
            test "$enable_sakke" = "" && enable_sakke=yes
        fi
    fi

    if test "$ENABLED_FIPS" = "no" || test "$ENABLED_FIPS" = "dev"; then
        test "$enable_aessiv" = "" && enable_aessiv=yes
    fi

    # Enable AES Decrypt, AES ECB, Alt Names, DER Load
    AM_CFLAGS="$AM_CFLAGS -DHAVE_AES_DECRYPT -DHAVE_AES_ECB -DWOLFSSL_ALT_NAMES -DWOLFSSL_DER_LOAD"

    # Enable DH const table speedups (eliminates `-lm` math lib dependency)
    AM_CFLAGS="$AM_CFLAGS -DHAVE_FFDHE_2048 -DHAVE_FFDHE_3072"
    DEFAULT_MAX_CLASSIC_ASYM_KEY_BITS=4096

    # Enable multiple attribute additions such as DC
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_MULTI_ATTRIB"

    # Enable DH Extra
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_DH_EXTRA"

    # Enable deterministic ECC signing API with variant
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_ECDSA_DETERMINISTIC_K_VARIANT"

    # Store issuer name components when parsing certificates.
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_HAVE_ISSUER_NAMES"
fi

# liboqs
ENABLED_LIBOQS="no"
tryliboqsdir=""
AC_ARG_WITH([liboqs],
    [AS_HELP_STRING([--with-liboqs=PATH],[Path to liboqs install (default /usr/local) EXPERIMENTAL!])],
    [
        AC_MSG_CHECKING([for liboqs])
        LIBS="$LIBS -loqs"

        AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <oqs/common.h>]], [[ OQS_init(); ]])], [ liboqs_linked=yes ],[ liboqs_linked=no ])

        if test "x$liboqs_linked" = "xno" ; then
            if test "x$withval" != "xno" ; then
                tryliboqsdir=$withval
            fi
            if test "x$withval" = "xyes" ; then
                tryliboqsdir="/usr/local"
            fi

            CPPFLAGS="$AM_CPPFLAGS -DHAVE_LIBOQS -DHAVE_TLS_EXTENSIONS -I$tryliboqsdir/include"
            LDFLAGS="$AM_LDFLAGS $LDFLAGS -L$tryliboqsdir/lib"

            AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <oqs/common.h>]], [[ OQS_init(); ]])], [ liboqs_linked=yes ],[ liboqs_linked=no ])

            if test "x$liboqs_linked" = "xno" ; then
                AC_MSG_ERROR([liboqs isn't found.
                If it's already installed, specify its path using --with-liboqs=/dir/])
            fi
            AC_MSG_RESULT([yes])
            AM_CPPFLAGS="$CPPFLAGS"
            AM_LDFLAGS="$AM_LDFLAGS -L$tryliboqsdir/lib"
        else
            AC_MSG_RESULT([yes])
        fi

        if test "x$ENABLED_OPENSSLEXTRA" = "xno" && test "x$ENABLED_OPENSSLCOEXIST" = "xno"
        then
            ENABLED_OPENSSLEXTRA="yes"
            AM_CFLAGS="$AM_CFLAGS -DOPENSSL_EXTRA"
        fi

        AM_CFLAGS="$AM_CFLAGS -DHAVE_LIBOQS -DHAVE_TLS_EXTENSIONS"
        ENABLED_LIBOQS="yes"
    ]
)


# KYBER
# Used:
#  - SHA3, Shake128 and Shake256, or
#  - SHA256, SHA512, AES-CTR
AC_ARG_ENABLE([kyber],
    [AS_HELP_STRING([--enable-kyber],[Enable KYBER (default: disabled)])],
    [ ENABLED_KYBER=$enableval ],
    [ ENABLED_KYBER=no ]
    )

ENABLED_WC_KYBER=no
for v in `echo $ENABLED_KYBER | tr "," " "`
do
  case $v in
  yes | all)
    ENABLED_KYBER512=yes
    ENABLED_KYBER768=yes
    ENABLED_KYBER1024=yes
    ;;
  no)
    ;;
  wolfssl)
    ENABLED_WC_KYBER=yes
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_WC_KYBER"
    ;;
  small)
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_KYBER_SMALL"
    ;;
  512)
    ENABLED_KYBER512=yes
    ;;
  768)
    ENABLED_KYBER768=yes
    ;;
  1024)
    ENABLED_KYBER1024=yes
    ;;
  *)
    AC_MSG_ERROR([Invalid choice for KYBER []: $ENABLED_KYBER.])
    break;;
  esac
done

if test "$ENABLED_KYBER" != "no"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_HAVE_KYBER"

    if test "$ENABLED_KYBER512" = ""; then
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_NO_KYBER512"
    fi
    if test "$ENABLED_KYBER768" = ""; then
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_NO_KYBER768"
    fi
    if test "$ENABLED_KYBER1024" = ""; then
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_NO_KYBER1024"
    fi

    if test "$ENABLED_WC_KYBER" = "yes"
    then
        test "$enable_sha3" = "" && enable_sha3=yes
        test "$enable_shake128" = "" && enable_shake128=yes
        test "$enable_shake256" = "" && enable_shake256=yes
    else
        # Default is to use liboqs. Make sure its enabled.
        if test "$ENABLED_LIBOQS" = "no"; then
            AC_MSG_ERROR([The default implementation for kyber is liboqs.
            Please use --with-liboqs.])
        fi
    fi
fi


# liblms
# Get the path to the hash-sigs LMS HSS lib.
ENABLED_LIBLMS="no"
tryliblmsdir=""
AC_ARG_WITH([liblms],
    [AS_HELP_STRING([--with-liblms=PATH],[PATH to hash-sigs LMS/HSS install (default /usr/local) EXPERIMENTAL!])],
    [
        AC_MSG_CHECKING([for liblms])

        AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <hss.h>]], [[ param_set_t lm_type; param_set_t lm_ots_type; hss_get_public_key_len(4, &lm_type, &lm_ots_type); ]])], [ liblms_linked=yes ],[ liblms_linked=no ])

        if test "x$liblms_linked" = "xno" ; then
            if test "x$withval" != "xno" ; then
                tryliblmsdir=$withval
            fi
            if test "x$withval" = "xyes" ; then
                tryliblmsdir="/usr/local"
            fi

            # 1. By default use the hash-sigs single-threaded static library.
            # 2. If 1 not found, then use the multi-threaded static lib.
            # 3. If 2 not found, then use the multi-threaded dynamic lib.
            if test -e $tryliblmsdir/hss_lib.a; then
                CPPFLAGS="$AM_CPPFLAGS -DHAVE_LIBLMS -I$tryliblmsdir"
                LIB_STATIC_ADD="$LIB_STATIC_ADD $tryliblmsdir/hss_lib.a"
                enable_shared=no
                enable_static=yes
                liblms_linked=yes
            elif test -e $tryliblmsdir/hss_lib_thread.a; then
                CPPFLAGS="$AM_CPPFLAGS -DHAVE_LIBLMS -I$tryliblmsdir"
                LIB_STATIC_ADD="$LIB_STATIC_ADD $tryliblmsdir/hss_lib_thread.a"
                enable_shared=no
                enable_static=yes
                liblms_linked=yes
            elif test -e $tryliblmsdir/lib/libhss.so; then
                LIBS="$LIBS -lhss"
                CPPFLAGS="$AM_CPPFLAGS -DHAVE_LIBLMS -I$tryliblmsdir/include/hss"
                LDFLAGS="$AM_LDFLAGS $LDFLAGS -L$tryliblmsdir/lib"

                AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <hss.h>]], [[ param_set_t lm_type; param_set_t lm_ots_type; hss_get_public_key_len(4, &lm_type, &lm_ots_type); ]])], [ liblms_linked=yes ],[ liblms_linked=no ])
            else
                AC_MSG_ERROR([liblms isn't found.
                If it's already installed, specify its path using --with-liblms=/dir/])
            fi

            if test "x$liblms_linked" = "xno" ; then
                AC_MSG_ERROR([liblms isn't found.
                If it's already installed, specify its path using --with-liblms=/dir/])
            fi

            AC_MSG_RESULT([yes])
            AM_CPPFLAGS="$CPPFLAGS"
            AM_LDFLAGS="$LDFLAGS"
        else
            AC_MSG_RESULT([yes])
        fi

        AM_CFLAGS="$AM_CFLAGS -DHAVE_LIBLMS"
        ENABLED_LIBLMS="yes"
    ]
)


# LMS
AC_ARG_ENABLE([lms],
    [AS_HELP_STRING([--enable-lms],[Enable stateful LMS/HSS signatures (default: disabled)])],
    [ ENABLED_LMS=$enableval ],
    [ ENABLED_LMS=no ]
    )

ENABLED_WC_LMS=no
for v in `echo $ENABLED_LMS | tr "," " "`
do
  case $v in
  yes)
    ;;
  no)
    ;;
  wolfssl)
    ENABLED_WC_LMS=yes
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_WC_LMS"
    ;;
  *)
    AC_MSG_ERROR([Invalid choice for LMS []: $ENABLED_LMS.])
    break;;
  esac
done

if test "$ENABLED_LMS" != "no"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_HAVE_LMS"

    if test "$ENABLED_WC_LMS" = "no";
    then
        # Default is to use hash-sigs LMS lib. Make sure it's enabled.
        if test "$ENABLED_LIBLMS" = "no"; then
            AC_MSG_ERROR([The default implementation for LMS is the hash-sigs LMS/HSS lib.
            Please use --with-liblms.])
        fi
    fi
fi


# SINGLE THREADED
AC_ARG_ENABLE([singlethreaded],
    [AS_HELP_STRING([--enable-singlethreaded],[Enable wolfSSL single threaded (default: disabled)])],
    [ ENABLED_SINGLETHREADED=$enableval ],
    [ ENABLED_SINGLETHREADED=no ])

AS_IF([ test "x$ENABLED_SINGLETHREADED" = "xno" ],[
       AX_PTHREAD([
                   AC_DEFINE([HAVE_PTHREAD], [1], [Define if you have POSIX threads libraries and header files.])
                   # If AX_PTHREAD is adding -Qunused-arguments, need to prepend with -Xcompiler libtool will use it. Newer
                   # versions of clang don't need the -Q flag when using pthreads.
                   AS_CASE([$PTHREAD_CFLAGS],[-Qunused-arguments*],[PTHREAD_CFLAGS="-Xcompiler $PTHREAD_CFLAGS"])
                   AM_CFLAGS="$AM_CFLAGS $PTHREAD_CFLAGS"
                   LIBS="$LIBS $PTHREAD_LIBS"
                   ],[
                      ENABLED_SINGLETHREADED=yes
                      ])
      ])

AS_IF([ test "x$ENABLED_SINGLETHREADED" = "xyes" ],[ AM_CFLAGS="$AM_CFLAGS -DSINGLE_THREADED" ])

# Enable rwlock
AC_ARG_ENABLE([rwlock],
    [AS_HELP_STRING([--enable-rwlock],[Enable use of rwlock (default: disabled)])],
    [ENABLED_RWLOCK=$enableval],
    [ENABLED_RWLOCK=no])

if test "$ENABLED_RWLOCK" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_USE_RWLOCK"
fi

# wolfCrypt Only Build
AC_ARG_ENABLE([cryptonly],
    [AS_HELP_STRING([--enable-cryptonly],[Enable wolfCrypt Only build (default: disabled)])],
    [ENABLED_CRYPTONLY=$enableval],
    [ENABLED_CRYPTONLY=no])

AS_IF([test "x$FIPS_VERSION" = "xrand"],[ENABLED_CRYPTONLY="yes"])

# ECH
AC_ARG_ENABLE([ech],
    [AS_HELP_STRING([--enable-ech],[Enable ECH (default: disabled)])],
    [ ENABLED_ECH=$enableval ],
    [ ENABLED_ECH=no ]
    )
if test "$ENABLED_ECH" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_ECH"

    test "$enable_hpke" = "" && enable_hpke=yes
    test "$enable_ecc" = "" && enable_ecc=yes
    test "$enable_curve25519" = "" && enable_curve25519=yes
    test "$enable_sha256" = "" && enable_sha256=yes
    test "$enable_tlsx" = "" && enable_tlsx=yes
    test "$enable_sni" = "" && enable_sni=yes
    test "$enable_tls13" = "" && enable_tls13=yes
fi

# DTLS
# DTLS is a prereq for the options mcast, sctp, and jni. Enabling any of those
# without DTLS will also enable DTLS.
AC_ARG_ENABLE([dtls],
    [AS_HELP_STRING([--enable-dtls],[Enable wolfSSL DTLS (default: disabled)])],
    [ ENABLED_DTLS=$enableval ],
    [ ENABLED_DTLS=no ]
    )
if test "$ENABLED_DTLS" = "yes"
then
  AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_DTLS"
fi

# DTLS change MTU
AC_ARG_ENABLE([dtls-mtu],
    [AS_HELP_STRING([--enable-dtls-mtu],[Enable setting the MTU size for wolfSSL DTLS (default: disabled)])],
    [ ENABLED_DTLS_MTU=$enableval ],
    [ ENABLED_DTLS_MTU=no ]
    )
if test "$ENABLED_DTLS_MTU" = "yes"
then
  AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_DTLS_MTU"
fi

# TLS v1.3 Draft 18 (Note: only final TLS v1.3 supported, here for backwards build compatibility)
AC_ARG_ENABLE([tls13-draft18],
    [AS_HELP_STRING([--enable-tls13-draft18],[Enable wolfSSL TLS v1.3 Draft 18 (default: disabled)])],
    [ ENABLED_TLS13_DRAFT18=$enableval ],
    [ ENABLED_TLS13_DRAFT18=no ]
    )

# TLS v1.3
AC_ARG_ENABLE([tls13],
    [AS_HELP_STRING([--enable-tls13],[Enable wolfSSL TLS v1.3 (default: enabled)])],
    [ ENABLED_TLS13=$enableval ],
    [ ENABLED_TLS13=yes ]
    )
if test "x$FIPS_VERSION" = "xv1" ||
        ( test "$HAVE_FIPS_VERSION" = 2 && test "$HAVE_FIPS_VERSION_MINOR" = 1 ) ||
        test "$ENABLED_CRYPTONLY" = "yes"
then
    ENABLED_TLS13="no"
fi

# QUIC support
AC_ARG_ENABLE([quic],
    [AS_HELP_STRING([--enable-quic],[Enable QUIC API with wolfSSL TLS v1.3 (default: disabled)])],
    [ ENABLED_QUIC=$enableval ],
    [ ENABLED_QUIC=no ]
    )

if test "$ENABLED_QUIC" = "yes"
then
    if test "x$ENABLED_TLS13" = "xno"
    then
        AC_MSG_ERROR([TLS 1.3 is disabled - necessary for QUIC])
    fi
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_QUIC"
    # QUIC proto handlers need app_data at WOLFSSL*
    AM_CFLAGS="$AM_CFLAGS -DHAVE_EX_DATA"
fi


# Post-handshake Authentication
AC_ARG_ENABLE([postauth],
    [AS_HELP_STRING([--enable-postauth],[Enable wolfSSL Post-handshake Authentication (default: disabled)])],
    [ ENABLED_TLS13_POST_AUTH=$enableval ],
    [ ENABLED_TLS13_POST_AUTH=no ]
    )
if test "$ENABLED_TLS13_POST_AUTH" = "yes"
then
    if test "x$ENABLED_TLS13" = "xno"
    then
        AC_MSG_NOTICE([TLS 1.3 is disabled - disabling Post-handshake Authentication])
        ENABLED_TLS13_POST_AUTH="no"
    else
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_POST_HANDSHAKE_AUTH"
    fi
fi


# Hello Retry Request Cookie
AC_ARG_ENABLE([hrrcookie],
    [AS_HELP_STRING([--enable-hrrcookie],[Enable the server to send Cookie Extension in HRR with state (default: disabled)])],
    [ ENABLED_SEND_HRR_COOKIE=$enableval ],
    [ ENABLED_SEND_HRR_COOKIE=undefined ]
    )
if test "$ENABLED_SEND_HRR_COOKIE" = "yes"
then
    if test "x$ENABLED_TLS13" = "xno"
    then
        AC_MSG_NOTICE([TLS 1.3 is disabled - disabling HRR Cookie])
        ENABLED_SEND_HRR_COOKIE="no"
    else
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SEND_HRR_COOKIE"
    fi
fi


# RNG
AC_ARG_ENABLE([rng],
    [AS_HELP_STRING([--enable-rng],[Enable compiling and using RNG (default: enabled)])],
    [ ENABLED_RNG=$enableval ],
    [ ENABLED_RNG=yes ]
    )

if test "$ENABLED_RNG" = "no"
then
    AM_CFLAGS="$AM_CFLAGS -DWC_NO_RNG"
fi


# DTLS-SCTP
AC_ARG_ENABLE([sctp],
    [AS_HELP_STRING([--enable-sctp],[Enable wolfSSL DTLS-SCTP support (default: disabled)])],
    [ENABLED_SCTP=$enableval],
    [ENABLED_SCTP=no])

AS_IF([test "x$ENABLED_SCTP" = "xyes"],
        [AC_MSG_CHECKING([for SCTP])
         AC_RUN_IFELSE(
            [AC_LANG_PROGRAM(
[[
#include <sys/socket.h>
#include <arpa/inet.h>
]],
[[int s = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP); if (s == -1) return 1;]])],
[AC_MSG_RESULT(yes)],
[AC_MSG_RESULT(no)
 AC_MSG_ERROR([SCTP not available, remove enable-sctp from configure])],
: ,
: ,
: )])

# DTLS-SRTP
AC_ARG_ENABLE([srtp],
    [AS_HELP_STRING([--enable-srtp],[Enable wolfSSL DTLS-SRTP support (default: disabled)])],
    [ENABLED_SRTP=$enableval],
    [ENABLED_SRTP=no])

# DTLS-MULTICAST
AC_ARG_ENABLE([mcast],
    [AS_HELP_STRING([--enable-mcast],[Enable wolfSSL DTLS multicast support (default: disabled)])],
    [ENABLED_MCAST=$enableval],
    [ENABLED_MCAST=no])


# List of open source project defines using our openssl compatibility layer:
# bind dns (--enable-bind) WOLFSSL_BIND
# libssh2 (--enable-libssh2)
# openssh (--enable-openssh) WOLFSSL_OPENSSH
# openvpn (--enable-openvpn) WOLFSSL_OPENVPN
# nginx (--enable-nginx) WOLFSSL_NGINX
# ntp (--enable-ntp)
# openresty (--enable-openresty)
# haproxy (--enable-haproxy) WOLFSSL_HAPROXY
# wpa_supplicant (--enable-wpas) WOLFSSL_WPAS
# ssl fortress (--enable-fortress) FORTRESS
# ssl bump (--enable-bump)
# signal (--enable-signal)
# lighty (--enable-lighty) HAVE_LIGHTY
# rsyslog (--enable-rsyslog)
# stunnel (--enable-stunnel) HAVE_STUNNEL
# curl (--enable-curl) HAVE_CURL
# libest (--enable-libest) HAVE_LIBEST
# asio (--enable-asio) WOLFSSL_ASIO
# libwebsockets (--enable-libwebsockets) WOLFSSL_LIBWEBSOCKETS
# qt (--enable-qt) WOLFSSL_QT
# qt test (--enable-qt-test) WOLFSSL_QT_TEST
# HAVE_POCO_LIB
# WOLFSSL_MYSQL_COMPATIBLE
# web server (--enable-webserver) HAVE_WEBSERVER
# net-snmp (--enable-net-snmp)
# krb (--enable-krb) WOLFSSL_KRB
# FFmpeg (--enable-ffmpeg) WOLFSSL_FFMPEG
# strongSwan (--enable-strongswan)
# OpenLDAP (--enable-openldap)
# hitch (--enable-hitch)

# Bind DNS compatibility Build
AC_ARG_ENABLE([bind],
    [AS_HELP_STRING([--enable-bind],[Enable Bind DNS compatibility build (default: disabled)])],
    [ENABLED_BIND=$enableval],
    [ENABLED_BIND=no])

AC_ARG_ENABLE([libssh2],
    [AS_HELP_STRING([--enable-libssh2],[Enable libssh2 compatibility build (default: disabled)])],
    [ENABLED_LIBSSH2=$enableval],
    [ENABLED_LIBSSH2=no])

# OpenSSH compatibility Build
AC_ARG_ENABLE([openssh],
    [AS_HELP_STRING([--enable-openssh],[Enable OpenSSH compatibility build (default: disabled)])],
    [ENABLED_OPENSSH=$enableval],
    [ENABLED_OPENSSH=no])

# OpenVPN compatibility Build
AC_ARG_ENABLE([openvpn],
    [AS_HELP_STRING([--enable-openvpn],[Enable OpenVPN compatibility build (default: disabled)])],
    [ENABLED_OPENVPN=$enableval],
    [ENABLED_OPENVPN=no])

# openresty compatibility build
AC_ARG_ENABLE([openresty],
    [AS_HELP_STRING([--enable-openresty],[Enable openresty (default: disabled)])],
    [ ENABLED_OPENRESTY=$enableval ],
    [ ENABLED_OPENRESTY=no ]
    )

# nginx compatibility build
AC_ARG_ENABLE([nginx],
    [AS_HELP_STRING([--enable-nginx],[Enable nginx (default: disabled)])],
    [ ENABLED_NGINX=$enableval ],
    [ ENABLED_NGINX=no ]
    )

# chrony support. Needs the compatibility layer for SNI callback functionality,
# but otherwise uses pure wolfCrypt.
AC_ARG_ENABLE([chrony],
    [AS_HELP_STRING([--enable-chrony],[Enable chrony support (default: disabled)])],
    [ ENABLED_CHRONY=$enableval ],
    [ ENABLED_CHRONY=no ]
    )

if test "$ENABLED_CHRONY" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_ALWAYS_KEEP_SNI"
fi

if test "$ENABLED_OPENRESTY" = "yes"
then
    ENABLED_NGINX="yes"
fi

# OpenLDAP support
AC_ARG_ENABLE([openldap],
    [AS_HELP_STRING([--enable-openldap],[Enable OpenLDAP support (default: disabled)])],
    [ ENABLED_OPENLDAP=$enableval ],
    [ ENABLED_OPENLDAP=no ]
    )

# lighty Support
AC_ARG_ENABLE([lighty],
    [AS_HELP_STRING([--enable-lighty],[Enable lighttpd/lighty (default: disabled)])],
    [ ENABLED_LIGHTY=$enableval ],
    [ ENABLED_LIGHTY=no ]
    )
# rsyslog Support
AC_ARG_ENABLE([rsyslog],
    [AS_HELP_STRING([--enable-rsyslog],[Enable rsyslog (default: disabled)])],
    [ ENABLED_RSYSLOG=$enableval ],
    [ ENABLED_RSYSLOG=no ]
    )

# haproxy compatibility build
AC_ARG_ENABLE([haproxy],
    [AS_HELP_STRING([--enable-haproxy],[Enable haproxy (default: disabled)])],
    [ ENABLED_HAPROXY=$enableval ],
    [ ENABLED_HAPROXY=no ]
    )

# wpa_supplicant support
AC_ARG_ENABLE([wpas],
    [AS_HELP_STRING([--enable-wpas],[Enable wpa_supplicant support (default: disabled)])],
    [ ENABLED_WPAS=$enableval ],
    [ ENABLED_WPAS=no ]
    )

# wpa_supplicant support
AC_ARG_ENABLE([wpas-dpp],
    [AS_HELP_STRING([--enable-wpas-dpp],[Enable wpa_supplicant support with dpp (default: disabled)])],
    [ ENABLED_WPAS_DPP=$enableval ],
    [ ENABLED_WPAS_DPP=no ]
    )

if test "$ENABLED_WPAS_DPP" = "yes"
then
    ENABLED_WPAS="yes"
fi

# ntp support
AC_ARG_ENABLE([ntp],
    [AS_HELP_STRING([--enable-ntp],[Enable ntp support (default: disabled)])],
    [ ENABLED_NTP=$enableval ],
    [ ENABLED_NTP=no ]
    )

# Fortress build
AC_ARG_ENABLE([fortress],
    [AS_HELP_STRING([--enable-fortress],[Enable SSL fortress build (default: disabled)])],
    [ ENABLED_FORTRESS=$enableval ],
    [ ENABLED_FORTRESS=no ]
    )

# libwebsockets Support
AC_ARG_ENABLE([libwebsockets],
    [AS_HELP_STRING([--enable-libwebsockets],[Enable libwebsockets (default: disabled)])],
    [ ENABLED_LIBWEBSOCKETS=$enableval ],
    [ ENABLED_LIBWEBSOCKETS=no ]
    )
if test "$ENABLED_LIBWEBSOCKETS" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_LIBWEBSOCKETS -DHAVE_EX_DATA -DOPENSSL_NO_EC"
fi


if test "$ENABLED_OPENSSH" = "yes"
then
    ENABLED_FORTRESS="yes"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_OPENSSH -DHAVE_EX_DATA -DWOLFSSL_BASE16"
fi

# net-snmp Build
AC_ARG_ENABLE([net-snmp],
    [AS_HELP_STRING([--enable-net-snmp],[Enable net-snmp (default: disabled)])],
    [ ENABLED_NETSNMP=$enableval ],
    [ ENABLED_NETSNMP=no ]
    )

# kerberos 5 Build
AC_ARG_ENABLE([krb],
    [AS_HELP_STRING([--enable-krb],[Enable kerberos 5 support (default: disabled)])],
    [ ENABLED_KRB=$enableval ],
    [ ENABLED_KRB=no ]
    )

# FFmpeg Build
AC_ARG_ENABLE([ffmpeg],
    [AS_HELP_STRING([--enable-ffmpeg],[Enable FFmpeg support (default: disabled)])],
    [ ENABLED_FFMPEG=$enableval ],
    [ ENABLED_FFMPEG=no ]
    )


#IP alternative name Support
AC_ARG_ENABLE([ip-alt-name],
    [AS_HELP_STRING([--enable-ip-alt-name],[Enable IP subject alternative name (default: disabled)])],
    [ ENABLE_IP_ALT_NAME=$enableval ],
    [ ENABLE_IP_ALT_NAME=no ]
    )

if test "$ENABLE_IP_ALT_NAME" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_IP_ALT_NAME"
fi

#Qt Support
AC_ARG_ENABLE([qt],
    [AS_HELP_STRING([--enable-qt],[Enable qt (default: disabled)])],
    [ ENABLED_QT=$enableval ],
    [ ENABLED_QT=no ]
    )

# ssl bump build
AC_ARG_ENABLE([bump],
    [AS_HELP_STRING([--enable-bump],[Enable SSL Bump build (default: disabled)])],
    [ ENABLED_BUMP=$enableval ],
    [ ENABLED_BUMP=no ]
    )

# SNIFFER
AC_ARG_ENABLE([sniffer],
   [AS_HELP_STRING([--enable-sniffer],[Enable wolfSSL sniffer support (default: disabled)])],
   [ ENABLED_SNIFFER=$enableval ],
   [ ENABLED_SNIFFER=no ]
   )

# signal compatibility build
AC_ARG_ENABLE([signal],
    [AS_HELP_STRING([--enable-signal],[Enable signal (default: disabled)])],
    [ ENABLED_SIGNAL=$enableval ],
    [ ENABLED_SIGNAL=no ]
    )

# strongSwan support
AC_ARG_ENABLE([strongswan],
    [AS_HELP_STRING([--enable-strongswan],[Enable strongSwan support (default: disabled)])],
    [ ENABLED_STRONGSWAN=$enableval ],
    [ ENABLED_STRONGSWAN=no ]
    )

# hitch support
AC_ARG_ENABLE([hitch],
    [AS_HELP_STRING([--enable-hitch],[Enable hitch support (default: disabled)])],
    [ ENABLED_HITCH=$enableval ],
    [ ENABLED_HITCH=no ]
    )

# OpenSSL Coexist
AC_ARG_ENABLE([opensslcoexist],
    [AS_HELP_STRING([--enable-opensslcoexist],[Enable coexistence of wolfssl/openssl (default: disabled)])],
    [ ENABLED_OPENSSLCOEXIST=$enableval ],
    [ ENABLED_OPENSSLCOEXIST=no ]
    )

if test "x$ENABLED_OPENSSLCOEXIST" = "xyes" || test "$ENABLED_WOLFENGINE" = "yes"
then
    # make sure old names are disabled
    enable_oldnames=no

    AM_CFLAGS="$AM_CFLAGS -DOPENSSL_COEXIST"
fi

# S/MIME
AC_ARG_ENABLE([smime],
    [AS_HELP_STRING([--enable-smime],[Enable S/MIME (default: disabled)])],
    [ ENABLED_SMIME=$enableval ],
    [ ENABLED_SMIME=no ]
    )

# Platform Security Architecture (PSA)
AC_ARG_ENABLE([psa],
[AS_HELP_STRING([--enable-psa],[use Platform Security Architecture (PSA) interface (default: disabled)])],
[ ENABLED_PSA=$enableval ],
[ ENABLED_PSA=no ]
)

AC_ARG_WITH([psa-include],
    [AS_HELP_STRING([--with-psa-include=PATH],
       [PATH to directory with PSA header files])],
    [PSA_INCLUDE=$withval],
    [PSA_INCLUDE=""])

AC_ARG_WITH([psa-lib],
    [AS_HELP_STRING([--with-psa-lib=PATH],[PATH to directory with the PSA library])],
    [PSA_LIB=$withval],
    [PSA_LIB=""])

AC_ARG_WITH([psa-lib-name],
    [AS_HELP_STRING([--with-psa-lib-name=NAME],[NAME of PSA library])],
    [PSA_LIB_NAME=$withval],
    [PSA_LIB_NAME=""])

AC_ARG_ENABLE([psa-lib-static],
    [AS_HELP_STRING([--enable-psa-lib-static],[Link PSA as static library (default: disable)])],
    [ ENABLED_PSA_STATIC=$enableval ],
    [ ENABLED_PSA_STATIC=no ]
)

if test "x$ENABLED_PSA" = "xyes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_HAVE_PSA"
fi

if test "x$ENABLED_PSA" != "xyes" && \
    (test "x$PSA_LIB" != "x" || test "x$PSA_INCLUDE" != "x" || test "x$PSA_LIB_NAME" != "x" )
then
    AC_MSG_ERROR([to use PSA you need to enable it with --enable-psa])
fi

if test -n "$PSA_LIB"
then
    AC_MSG_CHECKING([for $PSA_LIB])
    if ! test -d "$PSA_LIB"
    then
        AC_MSG_ERROR([PSA lib dir $PSA_LIB not found.])
    fi
    AC_MSG_RESULT([yes])
    AM_LDFLAGS="$AM_LDFLAGS -L$PSA_LIB"
fi

if test -n "$PSA_LIB_NAME"
then
    if test "x$ENABLED_PSA_STATIC" = "xyes"
    then
        LIB_STATIC_ADD="$LIB_STATIC_ADD $PSA_LIB/$PSA_LIB_NAME"
    else
        LIB_ADD="$LIB_ADD -l$PSA_LIB_NAME"
    fi
fi

if test -n "$PSA_INCLUDE"
then
    AC_MSG_CHECKING([for $PSA_INCLUDE])
    if ! test -d "$PSA_INCLUDE"
    then
        AC_MSG_ERROR([psa include dir $PSA_INCLUDE not found.])
    fi
    AC_MSG_RESULT([yes])
    AM_CFLAGS="$AM_CFLAGS -I$PSA_INCLUDE"
fi

AC_SUBST([PSA_LIB])
AC_SUBST([PSA_LIB_NAME])
AC_SUBST([PSA_INCLUDE])

# OPENSSL Compatibility ALL
AC_ARG_ENABLE([opensslall],
[AS_HELP_STRING([--enable-opensslall],[Enable all OpenSSL API, size++ (default: disabled)])],
[ ENABLED_OPENSSLALL=$enableval ],
[ ENABLED_OPENSSLALL=no ]
)
if test "$ENABLED_LIBWEBSOCKETS" = "yes" || test "$ENABLED_OPENVPN" = "yes" || \
   test "$ENABLED_WPAS_DPP" = "yes" || test "$ENABLED_SMIME" = "yes" || \
   test "$ENABLED_HAPROXY" = "yes" || test "$ENABLED_BIND" = "yes" || \
   test "$ENABLED_NTP" = "yes" || test "$ENABLED_NETSNMP" = "yes" || \
   test "$ENABLED_OPENRESTY" = "yes" || test "$ENABLED_RSYSLOG" = "yes" || \
   test "$ENABLED_KRB" = "yes" || test "$ENABLED_CHRONY" = "yes" || \
   test "$ENABLED_FFMPEG" = "yes" || test "$ENABLED_STRONGSWAN" = "yes" || \
   test "$ENABLED_OPENLDAP" = "yes" || test "$ENABLED_HITCH" = "yes"
then
    ENABLED_OPENSSLALL="yes"
fi

# OPENSSL Extra Compatibility
AC_ARG_ENABLE([opensslextra],
    [AS_HELP_STRING([--enable-opensslextra],[Enable extra OpenSSL API, size+ (default: disabled)])],
    [ ENABLED_OPENSSLEXTRA=$enableval ],
    [ ENABLED_OPENSSLEXTRA=no ]
    )

if test "$ENABLED_QUIC" = "yes"
then
    ENABLED_OPENSSLEXTRA="yes"
fi


# One Error Queue per Thread
AC_ARG_ENABLE([error-queue-per-thread],
[AS_HELP_STRING([--enable-error-queue-per-thread],[Enable one error queue per thread. Requires thread local storage. (default: disabled)])],
[ ENABLED_ERRORQUEUEPERTHREAD=$enableval ],
[ ENABLED_ERRORQUEUEPERTHREAD=check ]
)

if test "$ENABLED_ERRORQUEUEPERTHREAD" = "check"
then
    AS_IF([test "$thread_ls_on" = "no" ||
           test "$ENABLED_SINGLETHREADED" = "yes"],
        [ENABLED_ERRORQUEUEPERTHREAD=no],
        [ENABLED_ERRORQUEUEPERTHREAD=yes])
fi

if test "$ENABLED_ERRORQUEUEPERTHREAD" = "yes"
then
    if test "$thread_ls_on" != "yes"
    then
        AC_MSG_ERROR(error-queue-per-thread needs thread-local storage.)
    fi
    AM_CFLAGS="$AM_CFLAGS -DERROR_QUEUE_PER_THREAD"
fi

# High Strength Build
AC_ARG_ENABLE([maxstrength],
    [AS_HELP_STRING([--enable-maxstrength],[Enable Max Strength build, allows TLSv1.2-AEAD-PFS ciphers only (default: disabled)])],
    [ENABLED_MAXSTRENGTH=$enableval],
    [ENABLED_MAXSTRENGTH=no])


# Harden, enable Timing Resistance and Blinding by default
AC_ARG_ENABLE([harden],
    [AS_HELP_STRING([--enable-harden],[Enable Hardened build, Enables Timing Resistance and Blinding (default: enabled)])],
    [ENABLED_HARDEN=$enableval],
    [ENABLED_HARDEN=yes])

if test "$ENABLED_HARDEN" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DTFM_TIMING_RESISTANT -DECC_TIMING_RESISTANT"
    if test "$ENABLED_RNG" = "yes"
    then
        AM_CFLAGS="$AM_CFLAGS -DWC_RSA_BLINDING"
    fi
else
    AM_CFLAGS="$AM_CFLAGS -DWC_NO_HARDEN -DWC_NO_CACHE_RESISTANT"
fi


# IPv6 Test Apps
AC_ARG_ENABLE([ipv6],
    [AS_HELP_STRING([--enable-ipv6],[Enable testing of IPV6 (default: disabled)])],
    [ ENABLED_IPV6=$enableval ],
    [ ENABLED_IPV6=no ]
    )

if test "$ENABLED_IPV6" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DTEST_IPV6 -DWOLFSSL_IPV6"
fi

if test "$ENABLED_WPAS" = "small"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_WPAS_SMALL"
fi
if test "$ENABLED_WPAS" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_WPAS"
    AM_CFLAGS="$AM_CFLAGS -DHAVE_SECRET_CALLBACK"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_PUBLIC_ECC_ADD_DBL"
fi
if test "$ENABLED_WPAS" != "no"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_ALWAYS_VERIFY_CB"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_ALWAYS_KEEP_SNI"
    AM_CFLAGS="$AM_CFLAGS -DHAVE_EX_DATA"
    AM_CFLAGS="$AM_CFLAGS -DHAVE_EXT_CACHE"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_EITHER_SIDE"
    AM_CFLAGS="$AM_CFLAGS -DOPENSSL_EXTRA_X509_SMALL"

    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_PUBLIC_MP"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_DER_LOAD"
    AM_CFLAGS="$AM_CFLAGS -DATOMIC_USER"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_KEY_GEN"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_DES_ECB"
    AM_CFLAGS="$AM_CFLAGS -DKEEP_OUR_CERT"
    AM_CFLAGS="$AM_CFLAGS -DKEEP_PEER_CERT"
    AM_CFLAGS="$AM_CFLAGS -DHAVE_KEYING_MATERIAL"
    AM_CFLAGS="$AM_CFLAGS -DNO_SESSION_CACHE_REF"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_VALIDATE_ECC_IMPORT"
    AM_CFLAGS="$AM_CFLAGS -DWC_CTC_NAME_SIZE=128"

    if test "$ENABLED_OPENSSLEXTRA" = "no"
    then
        ENABLED_OPENSSLEXTRA="yes"
        AM_CFLAGS="$AM_CFLAGS -DOPENSSL_EXTRA"
    fi
fi

if test "$ENABLED_FORTRESS" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DFORTRESS -DWOLFSSL_ALWAYS_VERIFY_CB -DWOLFSSL_AES_COUNTER -DWOLFSSL_AES_DIRECT -DWOLFSSL_DER_LOAD -DWOLFSSL_KEY_GEN"
fi


if test "$ENABLED_BUMP" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DLARGE_STATIC_BUFFERS -DWOLFSSL_CERT_GEN -DWOLFSSL_KEY_GEN -DHUGE_SESSION_CACHE -DWOLFSSL_DER_LOAD -DWOLFSSL_ALT_NAMES -DWOLFSSL_TEST_CERT"
    DEFAULT_MAX_CLASSIC_ASYM_KEY_BITS=4096
fi

# lean psk build
AC_ARG_ENABLE([leanpsk],
    [AS_HELP_STRING([--enable-leanpsk],[Enable Lean PSK build (default: disabled)])],
    [ ENABLED_LEANPSK=$enableval ],
    [ ENABLED_LEANPSK=no ]
    )

if test "$ENABLED_LEANPSK" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_LEANPSK -DWOLFSSL_STATIC_PSK -DHAVE_NULL_CIPHER -DSINGLE_THREADED -DNO_AES -DNO_FILESYSTEM -DNO_RSA -DNO_DSA -DNO_DH -DNO_PWDBASED -DNO_MD4 -DNO_MD5 -DNO_ERROR_STRINGS -DNO_OLD_TLS -DNO_RC4 -DNO_WRITEV -DNO_DEV_RANDOM -DWOLFSSL_USER_IO -DNO_SHA"
    ENABLED_SLOWMATH="no"
    ENABLED_SINGLETHREADED="yes"
    enable_lowresource=yes
fi


# lean TLS build (TLS 1.2 client only (no client auth), ECC256, AES128 and SHA256 w/o Shamir)
AC_ARG_ENABLE([leantls],
    [AS_HELP_STRING([--enable-leantls],[Enable Lean TLS build (default: disabled)])],
    [ ENABLED_LEANTLS=$enableval ],
    [ ENABLED_LEANTLS=no ]
    )

if test "$ENABLED_LEANTLS" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_LEANTLS -DNO_WRITEV -DHAVE_ECC -DTFM_ECC256 -DECC_USER_CURVES -DNO_WOLFSSL_SERVER -DNO_RSA -DNO_DSA -DNO_DH -DNO_PWDBASED -DNO_MD5 -DNO_ERROR_STRINGS -DNO_OLD_TLS -DNO_RC4 -DNO_SHA -DNO_PSK -DNO_WOLFSSL_MEMORY -DNO_WOLFSSL_CM_VERIFY"
    enable_lowresource=yes
fi


# low resource options to reduce flash and memory use
AC_ARG_ENABLE([lowresource],
    [AS_HELP_STRING([--enable-lowresource],[Enable low resource options for memory/flash (default: disabled)])],
    [ ENABLED_LOWRESOURCE=$enableval ],
    [ ENABLED_LOWRESOURCE=no ]
    )

if test "$ENABLED_LOWRESOURCE" = "yes"
then
    # low memory / flash flags
    AM_CFLAGS="$AM_CFLAGS -DNO_SESSION_CACHE -DRSA_LOW_MEM -DCURVE25519_SMALL -DED25519_SMALL -DWOLFSSL_SMALL_CERT_VERIFY -DWOLFSSL_NO_ASYNC_IO"

    # low flash flags
    AM_CFLAGS="$AM_CFLAGS -DUSE_SLOW_SHA -DUSE_SLOW_SHA256 -DUSE_SLOW_SHA512"

    # AES small
    AM_CFLAGS="$AM_CFLAGS -DGCM_SMALL -DWOLFSSL_AES_NO_UNROLL -DWOLFSSL_AES_SMALL_TABLES"
fi


# TITAN cache
AC_ARG_ENABLE([titancache],
    [AS_HELP_STRING([--enable-titancache],[Enable titan session cache (default: disabled)])],
    [ ENABLED_TITANCACHE=$enableval ],
    [ ENABLED_TITANCACHE=no ]
    )

if test "$ENABLED_TITANCACHE" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DTITAN_SESSION_CACHE"
fi


# HUGE cache
AC_ARG_ENABLE([hugecache],
    [AS_HELP_STRING([--enable-hugecache],[Enable huge session cache (default: disabled)])],
    [ ENABLED_HUGECACHE=$enableval ],
    [ ENABLED_HUGECACHE=no ]
    )

if test "$ENABLED_HUGECACHE" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DHUGE_SESSION_CACHE"
fi


# big cache
AC_ARG_ENABLE([bigcache],
    [AS_HELP_STRING([--enable-bigcache],[Enable big session cache (default: disabled)])],
    [ ENABLED_BIGCACHE=$enableval ],
    [ ENABLED_BIGCACHE=no ]
    )

if test "$ENABLED_BIGCACHE" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DBIG_SESSION_CACHE"
fi


# SMALL cache
AC_ARG_ENABLE([smallcache],
    [AS_HELP_STRING([--enable-smallcache],[Enable small session cache (default: disabled)])],
    [ ENABLED_SMALLCACHE=$enableval ],
    [ ENABLED_SMALLCACHE=no ]
    )

if test "$ENABLED_SMALLCACHE" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DSMALL_SESSION_CACHE"
fi


# Persistent session cache
AC_ARG_ENABLE([savesession],
    [AS_HELP_STRING([--enable-savesession],[Enable persistent session cache (default: disabled)])],
    [ ENABLED_SAVESESSION=$enableval ],
    [ ENABLED_SAVESESSION=no ]
    )

if test "$ENABLED_SAVESESSION" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DPERSIST_SESSION_CACHE"
fi


# Persistent cert cache
AC_ARG_ENABLE([savecert],
    [AS_HELP_STRING([--enable-savecert],[Enable persistent cert cache (default: disabled)])],
    [ ENABLED_SAVECERT=$enableval ],
    [ ENABLED_SAVECERT=no ]
    )

if test "$ENABLED_SAVECERT" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DPERSIST_CERT_CACHE"
fi


# Write duplicate WOLFSSL object
AC_ARG_ENABLE([writedup],
    [AS_HELP_STRING([--enable-writedup],[Enable write duplication of WOLFSSL objects (default: disabled)])],
    [ ENABLED_WRITEDUP=$enableval ],
    [ ENABLED_WRITEDUP=no ]
    )

if test "$ENABLED_WRITEDUP" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_WRITE_DUP"
fi


# Atomic User Record Layer
AC_ARG_ENABLE([atomicuser],
    [AS_HELP_STRING([--enable-atomicuser],[Enable Atomic User Record Layer (default: disabled)])],
    [ ENABLED_ATOMICUSER=$enableval ],
    [ ENABLED_ATOMICUSER=no ]
    )

if test "$ENABLED_ATOMICUSER" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DATOMIC_USER"
fi


# Public Key Callbacks
AC_ARG_ENABLE([pkcallbacks],
    [AS_HELP_STRING([--enable-pkcallbacks],[Enable Public Key Callbacks (default: disabled)])],
    [ ENABLED_PKCALLBACKS=$enableval ],
    [ ENABLED_PKCALLBACKS=no ]
    )

if test "$ENABLED_PKCALLBACKS" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_PK_CALLBACKS"
fi


# Maxim Integrated MAXQ10XX
ENABLED_MAXQ10XX="no"
maxqpartnumber=""
AC_ARG_WITH([maxq10xx],
    [AS_HELP_STRING([--with-maxq10xx=PART],[MAXQ10XX PART Number])],
    [
        AC_MSG_CHECKING([for maxq10xx])

        # Read the part number
        maxqpartnumber=$withval

        if test "$maxqpartnumber" = "MAXQ1065"; then
            LIB_STATIC_ADD="$LIB_STATIC_ADD lib/libmaxq1065_api.a"
            AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_MAXQ1065"
            ENABLED_MAXQ10XX="yes"
            AC_CHECK_LIB([rt], [clock_gettime])
        elif test "$maxqpartnumber" = "MAXQ108x"; then
            LIB_STATIC_ADD="$LIB_STATIC_ADD lib/libmaxq108x_api.a"
            AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_MAXQ108X"
            ENABLED_MAXQ10XX="yes"
            AC_CHECK_LIB([rt], [clock_gettime])
        else
            AC_MSG_ERROR([need a valid MAXQ part number])
        fi

        AC_MSG_RESULT([yes])
    ]
)

# Microchip/Atmel CryptoAuthLib
ENABLED_CRYPTOAUTHLIB="no"
trylibatcadir=""
AC_ARG_WITH([cryptoauthlib],
    [AS_HELP_STRING([--with-cryptoauthlib=PATH],[PATH to CryptoAuthLib install (default /usr/)])],
    [
        AC_MSG_CHECKING([for cryptoauthlib])
        CPPFLAGS="$CPPFLAGS -DWOLFSSL_ATECC508A"
        LIBS="$LIBS -lcryptoauth"

        AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <cryptoauthlib.h>]], [[ atcab_init(0); ]])],[ libatca_linked=yes ],[ libatca_linked=no ])

        if test "x$libatca_linked" = "xno" ; then
            if test "x$withval" != "xno" ; then
                trylibatcadir=$withval
            fi
            if test "x$withval" = "xyes" ; then
                trylibatcadir="/usr"
            fi

            LDFLAGS="$LDFLAGS -L$trylibatcadir/lib"
            CPPFLAGS="$CPPFLAGS -I$trylibatcadir/lib"

            AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <cryptoauthlib.h>]], [[ atcab_init(0); ]])],[ libatca_linked=yes ],[ libatca_linked=no ])

            if test "x$libatca_linked" = "xno" ; then
                AC_MSG_ERROR([cryptoauthlib isn't found.
                If it's already installed, specify its path using --with-cryptoauthlib=/dir/])
            fi

            AM_LDFLAGS="$AM_LDFLAGS -L$trylibatcadir/lib"
            AM_CFLAGS="$AM_CFLAGS -I$trylibatcadir/lib"
            AC_MSG_RESULT([yes])
        else
            AC_MSG_RESULT([yes])
        fi

        ENABLED_CRYPTOAUTHLIB="yes"
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_ATECC508A"
    ]
)

# NXP SE050
# Example: "./configure --with-se050=/home/pi/simw_top"
ENABLED_SE050="no"
trylibse050dir=""
AC_ARG_WITH([se050],
    [AS_HELP_STRING([--with-se050=PATH],[PATH to SE050 install (default /usr/local)])],
    [
        AC_MSG_CHECKING([for SE050])

        LIBS="$LIBS -lSSS_APIs"
        AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <fsl_sss_api.h>]], [[ sss_mac_init(0);]])],[ libse050_linked=yes ],[ libse050_linked=no ])

        if test "x$libse050_linked" = "xno" ; then
            if test "x$withval" != "xno" ; then
                trylibse050dir=$withval
            fi
            if test "x$withval" = "xyes" ; then
                trylibse050dir="/usr/local"
            fi
            LDFLAGS="$LDFLAGS -L$trylibse050dir/lib"
            LDFLAGS="$LDFLAGS -L$trylibse050dir/build/sss"
            CPPFLAGS="$CPPFLAGS -I$trylibse050dir/include/se05x"
            CPPFLAGS="$CPPFLAGS -I$trylibse050dir/build"
            CPPFLAGS="$CPPFLAGS -I$trylibse050dir/sss/inc"
            CPPFLAGS="$CPPFLAGS -I$trylibse050dir/sss/ex/inc"
            CPPFLAGS="$CPPFLAGS -I$trylibse050dir/sss/port/default"
            CPPFLAGS="$CPPFLAGS -I$trylibse050dir/hostlib/hostLib/inc"
            CPPFLAGS="$CPPFLAGS -I$trylibse050dir/hostlib/hostLib/libCommon/log"
            CPPFLAGS="$CPPFLAGS -I$trylibse050dir/hostlib/hostLib/libCommon/infra"
            CPPFLAGS="$CPPFLAGS -I$trylibse050dir/hostlib/hostLib/se05x_03_xx_xx"

            if test -e "$trylibse050dir/build/sss/libSSS_APIs.a"; then
                SE050_STATIC=yes
            else
                SE050_STATIC=no
            fi
            if test "x$SE050_STATIC" = "xyes"; then
                LIB_STATIC_ADD="$trylibse050dir/build/sss/ex/src/libex_common.a \
                                $trylibse050dir/build/sss/libSSS_APIs.a \
                                $trylibse050dir/build/hostlib/hostLib/se05x/libse05x.a \
                                $trylibse050dir/build/hostlib/hostLib/liba7x_utils.a \
                                $trylibse050dir/build/hostlib/hostLib/libCommon/log/libmwlog.a \
                                $trylibse050dir/build/hostlib/hostLib/libCommon/libsmCom.a $LIB_STATIC_ADD"
            else
                AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <fsl_sss_api.h>]], [[ sss_mac_init(0); ]])],[ libse050_linked=yes ],[ libse050_linked=no ])
                if test "x$libse050_linked" = "xno" ; then
                    AC_MSG_ERROR([SE050 isn't found.
                    If it's already installed, specify its path using --with-se050=/dir/])
                fi
            fi

            # Requires AES direct
            AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_AES_DIRECT -DHAVE_AES_ECB"

            # Does not support SHA2-512 224/256
            AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_NOSHA512_224 -DWOLFSSL_NOSHA512_256"

            AC_MSG_RESULT([yes])
        else
            AC_MSG_RESULT([yes])
        fi

        ENABLED_SE050="yes"
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SE050 -DSSS_USE_FTR_FILE"
    ]
)

ENABLED_SNIFFTEST=no
AS_IF([ test "x$ENABLED_SNIFFER" = "xyes" ],
      [
          AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SNIFFER -DWOLFSSL_STATIC_EPHEMERAL"
          AC_CHECK_HEADERS([pcap/pcap.h],
              [ ENABLED_SNIFFTEST=yes ],
              [ AC_MSG_WARN([cannot enable sniffer test without having libpcap available.]) ]
          )
      ])


# AES-CBC
AC_ARG_ENABLE([aescbc],
    [AS_HELP_STRING([--enable-aescbc],[Enable wolfSSL AES-CBC support (default: enabled)])],
    [ ENABLED_AESCBC=$enableval ],
    [ ENABLED_AESCBC=yes ]
    )

if test "$ENABLED_AESCBC" = "no"
then
    AM_CFLAGS="$AM_CFLAGS -DNO_AES_CBC"
    AM_CCASFLAGS="$AM_CCASFLAGS -DHAVE_AES_CBC"
fi

# AES-CBC length checks (checks that input lengths are multiples of block size)
AC_ARG_ENABLE([aescbc_length_checks],
    [AS_HELP_STRING([--enable-aescbc-length-checks],[Enable AES-CBC length validity checks (default: disabled)])],
    [ ENABLED_AESCBC_LENGTH_CHECKS=$enableval ],
    [ ENABLED_AESCBC_LENGTH_CHECKS=no ]
    )

if test "$ENABLED_AESCBC_LENGTH_CHECKS" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_AES_CBC_LENGTH_CHECKS"
fi

# leanpsk and leantls don't need gcm

# AES-GCM
AC_ARG_ENABLE([aesgcm],
    [AS_HELP_STRING([--enable-aesgcm],[Enable wolfSSL AES-GCM support (default: enabled)])],
    [ ENABLED_AESGCM=$enableval ],
    [ ENABLED_AESGCM=yes ]
    )
AC_ARG_ENABLE([aesgcm-stream],
    [AS_HELP_STRING([--enable-aesgcm-stream],[Enable wolfSSL AES-GCM support with streaming APIs (default: disabled)])],
    [ ENABLED_AESGCM_STREAM=$enableval ],
    [ ENABLED_AESGCM_STREAM=no ]
    )

# leanpsk and leantls don't need gcm
if test "$FIPS_VERSION" = "rand" || test "$ENABLED_LEANPSK" = "yes" ||
   (test "$ENABLED_LEANTLS" = "yes" && test "$ENABLED_TLS13" = "no")
then
    ENABLED_AESGCM=no
fi

if test "$ENABLED_AESGCM" = "yes" && test "$ac_cv_c_bigendian" != "yes"
then
    ENABLED_AESGCM="4bit"
fi


# AES-CCM
AC_ARG_ENABLE([aesccm],
    [AS_HELP_STRING([--enable-aesccm],[Enable wolfSSL AES-CCM support (default: disabled)])],
    [ ENABLED_AESCCM=$enableval ],
    [ ENABLED_AESCCM=no ]
    )

if test "$ENABLED_AESCCM" = "yes" || test "$ENABLED_WOLFENGINE" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_AESCCM"
    AM_CCASFLAGS="$AM_CCASFLAGS -DHAVE_AESCCM"
fi

# AES-SIV (RFC 5297)
AC_ARG_ENABLE([aessiv],
    [AS_HELP_STRING([--enable-aessiv],[Enable AES-SIV (RFC 5297) (default: disabled)])],
    [ ENABLED_AESSIV=$enableval ],
    [ ENABLED_AESSIV=no ]
    )

if test "$ENABLED_CHRONY" = "yes"
then
    ENABLED_AESSIV=yes
fi

# AES-CTRf
AC_ARG_ENABLE([aesctr],
    [AS_HELP_STRING([--enable-aesctr],[Enable wolfSSL AES-CTR support (default: disabled)])],
    [ ENABLED_AESCTR=$enableval ],
    [ ENABLED_AESCTR=no ]
    )
if test "$ENABLED_OPENVPN" = "yes" || test "$ENABLED_LIBSSH2" = "yes" || test "$ENABLED_AESSIV" = "yes" || test "$ENABLED_WOLFENGINE" = "yes"
then
    ENABLED_AESCTR=yes
fi

if test "$ENABLED_QUIC" = "yes"
then
    ENABLED_AESCTR=yes
fi

# AES-OFB
AC_ARG_ENABLE([aesofb],
    [AS_HELP_STRING([--enable-aesofb],[Enable wolfSSL AES-OFB support (default: disabled)])],
    [ ENABLED_AESOFB=$enableval ],
    [ ENABLED_AESOFB=no ]
    )

if test "$ENABLED_AESOFB" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_AES_OFB -DWOLFSSL_AES_DIRECT"
fi


# AES-CFB
AC_ARG_ENABLE([aescfb],
    [AS_HELP_STRING([--enable-aescfb],[Enable wolfSSL AES-CFB support (default: disabled)])],
    [ ENABLED_AESCFB=$enableval ],
    [ ENABLED_AESCFB=no ]
    )

if test "$ENABLED_AESCFB" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_AES_CFB"
fi


# SM4
ENABLED_SM4="no"
AC_ARG_ENABLE([sm4-ecb],
    [AS_HELP_STRING([--enable-sm4-ecb],[Enable wolfSSL SM4-ECB support (default: disabled)])],
    [ ENABLED_SM4_ECB=$enableval ],
    [ ENABLED_SM4_ECB=no ]
    )

if test "$ENABLED_SM4_ECB" = "small"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SM4_SMALL"
fi
if test "$ENABLED_SM4_ECB" != "no"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SM4_ECB"
    ENABLED_SM4="yes"
fi

AC_ARG_ENABLE([sm4-cbc],
    [AS_HELP_STRING([--enable-sm4-cbc],[Enable wolfSSL SM4-CBC support (default: disabled)])],
    [ ENABLED_SM4_CBC=$enableval ],
    [ ENABLED_SM4_CBC=no ]
    )

if test "$ENABLED_SM4_CBC" = "small"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SM4_SMALL"
fi
if test "$ENABLED_SM4_CBC" != "no"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SM4_CBC"
    ENABLED_SM4="yes"
fi

AC_ARG_ENABLE([sm4-ctr],
    [AS_HELP_STRING([--enable-sm4-ctr],[Enable wolfSSL SM4-CTR support (default: disabled)])],
    [ ENABLED_SM4_CTR=$enableval ],
    [ ENABLED_SM4_CTR=no ]
    )

if test "$ENABLED_SM4_CTR" = "small"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SM4_SMALL"
fi
if test "$ENABLED_SM4_CTR" != "no"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SM4_CTR"
    ENABLED_SM4="yes"
fi

AC_ARG_ENABLE([sm4-gcm],
    [AS_HELP_STRING([--enable-sm4-gcm],[Enable wolfSSL SM4-GCM support (default: disabled)])],
    [ ENABLED_SM4_GCM=$enableval ],
    [ ENABLED_SM4_GCM=no ]
    )

if test "$ENABLED_SM4_GCM" = "small"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SM4_SMALL"
fi
if test "$ENABLED_SM4_GCM" != "no"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SM4_GCM"
    ENABLED_SM4="yes"
fi

AC_ARG_ENABLE([sm4-ccm],
    [AS_HELP_STRING([--enable-sm4-ccm],[Enable wolfSSL SM4-CCM support (default: disabled)])],
    [ ENABLED_SM4_CCM=$enableval ],
    [ ENABLED_SM4_CCM=no ]
    )

if test "$ENABLED_SM4_CCM" = "small"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SM4_SMALL"
fi
if test "$ENABLED_SM4_CCM" != "no"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SM4_CCM"
    ENABLED_SM4="yes"
fi

if test "$ENABLED_SM4" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SM4"
fi


ENABLED_ARMASM_INLINE="no"
ENABLED_ARMASM_SHA3="no"
ENABLED_ARMASM_CRYPTO_SM4="no"
# ARM Assembly
# Both SHA3 and SHA512 instructions available with ARMV8.2-a
AC_ARG_ENABLE([armasm],
    [AS_HELP_STRING([--enable-armasm],[Enable wolfSSL ARMv8 ASM support (default: disabled). Set to sha512-crypto or sha3-crypto to use SHA512 and SHA3 instructions with Aarch64 CPU.])],
    [ ENABLED_ARMASM=$enableval ],
    [ ENABLED_ARMASM=no ]
    )
if test "$ENABLED_ARMASM" != "no" && test "$ENABLED_ASM" = "yes"
then

    for v in `echo $ENABLED_ARMASM | tr "," " "`
    do
        case $v in
        yes)
            ;;
        inline)
            ENABLED_ARMASM_INLINE=yes
            ;;
        sha512-crypto | sha3-crypto)
            case $host_cpu in
            *aarch64*)
                ;;
            *)
                AC_MSG_ERROR([SHA512/SHA3 instructions only available on Aarch64 CPU.])
                break;;
            esac
            ENABLED_ARMASM_SHA3=yes
            ENABLED_ARMASM_PLUS=yes
            ;;
        sm4)
            case $host_cpu in
            *aarch64*)
                ;;
            *)
                AC_MSG_ERROR([SM4 instructions only available on Aarch64 CPU.])
                break;;
            esac
            ENABLED_ARMASM_SM4=yes
            # gcc requires -march=...+sm4 to enable SM4 instructions
            ENABLED_ARMASM_CRYPTO_SM4=yes
            ENABLED_ARMASM_PLUS=yes
            ;;
        sm3)
            case $host_cpu in
            *aarch64*)
                ;;
            *)
                AC_MSG_ERROR([SM3 instructions only available on Aarch64 CPU.])
                break;;
            esac
            ENABLED_ARMASM_SM3=yes
            # gcc requires -march=...+sm4 to enable SM3 instructions
            ENABLED_ARMASM_CRYPTO_SM4=yes
            ENABLED_ARMASM_PLUS=yes
            ;;
        *)
            AC_MSG_ERROR([Invalid choice of ARM asm inclusions (yes, sha512-crypto, sha3-crypto): $ENABLED_ARMASM.])
            break;;
        esac
    done
    ENABLED_ARMASM="yes"

    AM_CCASFLAGS="$AM_CCASFLAGS -DWOLFSSL_ARMASM"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_ARMASM -DWOLFSSL_NO_HASH_RAW"
    #Check if mcpu and mfpu values already set if not use default
    case $CPPFLAGS in
    *mcpu* | *mfpu*)
        break;; #Do not override user set values
    *)
        case $host_cpu in
        *aarch64*)
            case $host_os in
            *darwin*)
                # All known Aarch64 Mac computers support SHA-512 instructions
                ENABLED_ARMASM_SHA3=yes
                ;;
            *)
                # +crypto needed for hardware acceleration
                if test "$ENABLED_ARMASM_PLUS" = "yes"; then
                    AM_CPPFLAGS="$AM_CPPFLAGS -march=armv8.2-a+crypto"
                    if test "$ENABLED_ARMASM_SHA3" = "yes"; then
                        AM_CPPFLAGS="$AM_CPPFLAGS+sha3"
                    fi
                    if test "$ENABLED_ARMASM_CRYPTO_SM4" = "yes"; then
                        AM_CPPFLAGS="$AM_CPPFLAGS+sm4"
                    fi
                else
                    AM_CPPFLAGS="$AM_CPPFLAGS -mcpu=generic+crypto"
                fi
                ;;
            esac
            # Include options.h
            AM_CCASFLAGS="$AM_CCASFLAGS -DEXTERNAL_OPTS_OPENVPN"
            ENABLED_ARMASM_CRYPTO=yes

            # Check for and set -mstrict-align compiler flag
            # Used to set assumption that Aarch64 systems will not handle
            #   unaligned memory references. The flag -mstrict-align is needed
            #   on some compiler versions to avoid an invalid addressing mode
            #   error with  "m" constraint variables in the inline assembly AES
            #   code. Even though unaligned load/store access is permitted on
            #   normal memory with Cortex-A series boards with the exception
            #   being exclusive and ordered access.
            case $CPPFLAGS in
                *mstrict-align*)
                    break;; # already set by user
                *)
                    AM_CPPFLAGS="$AM_CPPFLAGS -mstrict-align"
                    AC_MSG_NOTICE([64bit ARMv8, setting -mstrict-align]);;
            esac
            AC_MSG_NOTICE([64bit ARMv8 found, setting mcpu to generic+crypto])
            ;;
        armv7a*)
            AM_CPPFLAGS="$AM_CPPFLAGS -march=armv7-a -mfpu=neon -DWOLFSSL_ARMASM_NO_HW_CRYPTO -DWOLFSSL_ARM_ARCH=7"
            # Include options.h
            AM_CCASFLAGS="$AM_CCASFLAGS -DEXTERNAL_OPTS_OPENVPN"
            ENABLED_ARMASM_CRYPTO=no
            ENABLED_AESGCM_STREAM=no # not yet implemented
            AC_MSG_NOTICE([32bit ARMv7-a found, setting mfpu to neon])
            ;;
        *)
            AM_CPPFLAGS="$AM_CPPFLAGS -mfpu=crypto-neon-fp-armv8"
            # Include options.h
            AM_CCASFLAGS="$AM_CCASFLAGS -DEXTERNAL_OPTS_OPENVPN"
            ENABLED_ARMASM_CRYPTO=yes
            AC_MSG_NOTICE([32bit ARMv8 found, setting mfpu to crypto-neon-fp-armv8])
            ;;
        esac
    esac
fi

if test "$ENABLED_ARMASM_SHA3" = "yes"; then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_ARMASM_CRYPTO_SHA512 -DWOLFSSL_ARMASM_CRYPTO_SHA3"
    AM_CCASFLAGS="$AM_CCASFLAGS -DWOLFSSL_ARMASM_CRYPTO_SHA512 -DWOLFSSL_ARMASM_CRYPTO_SHA3"
fi
if test "$ENABLED_ARMASM_SM3" = "yes"; then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_ARMASM_CRYPTO_SM3"
fi
if test "$ENABLED_ARMASM_SM4" = "yes"; then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_ARMASM_CRYPTO_SM4"
fi

# Xilinx hardened crypto
AC_ARG_ENABLE([xilinx],
    [AS_HELP_STRING([--enable-xilinx],[Enable wolfSSL support for Xilinx hardened crypto(default: disabled)])],
    [ ENABLED_XILINX=$enableval ],
    [ ENABLED_XILINX=no ]
    )
if test "$ENABLED_XILINX" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_XILINX -DWOLFSSL_XILINX_CRYPT"
fi


# CAAM build
trylibsecodir="/usr"
AC_ARG_WITH([seco],
    [AS_HELP_STRING([--with-seco=PATH],[PATH to SECO install (default /usr/lib/)])],
    [
AC_MSG_CHECKING([for SECO])

if test "x$withval" != "xno" ; then
    trylibsecodir=$withval
fi
]
)

AC_ARG_ENABLE([aria],
    [AS_HELP_STRING([--enable-aria],[Enable wolfSSL support for ARIA (default: disabled)])],
    [ ENABLED_ARIA=$enableval ],
    [ ENABLED_ARIA=no ]
    )
if test "$ENABLED_ARIA" = "yes"
then
    ARIA_DIR=MagicCrypto
    # Enable dependency
    ENABLED_OPENSSLEXTRA="yes"
    CFLAGS="$CFLAGS -I$ARIA_DIR/include"
    AM_CFLAGS="$AM_CFLAGS -DHAVE_ARIA -DOPENSSL_EXTRA"
    AM_LDFLAGS="$AM_LDFLAGS -L$ARIA_DIR/lib -lMagicCrypto"
    build_pwd="$(pwd)"
    headers="mcapi_error.h mcapi_type.h mcapi.h"
    for header in $headers
    do
        AC_CHECK_HEADER([$header], [], [
                AC_MSG_ERROR([Error including $header. Please put the MagicCrypto library in $build_pwd.])
            ], [
            extern int dummy_int_to_make_compiler_happy;
        ])
    done
fi

AC_ARG_ENABLE([caam],
    [AS_HELP_STRING([--enable-caam],[Enable wolfSSL support for CAAM (default: disabled)])],
    [ ENABLED_CAAM=$enableval ],
    [ ENABLED_CAAM=no ]
    )

if test "$ENABLED_CAAM" != "no"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_CAAM"

    for v in `echo $ENABLED_CAAM | tr "," " "`
    do
      case $v in
        qnx)
            AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_QNX_CAAM"
            ENABLED_CAAM_QNX="yes"
            ;;

        imx6q)
            AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_IMX6Q_CAAM"
            ;;

        imx6ul)
            AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_IMX6UL_CAAM"
            ;;

        seco)
            SECO_DIR=$trylibsecodir
            AM_CPPFLAGS="$AM_CPPFLAGS -I$SECO_DIR/include"
            AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_CAAM -DWOLFSSL_SECO_CAAM"

            AC_CHECK_LIB([hsm_lib],[hsm_open_session])
            AC_CHECK_LIB([seco_nvm_manager],[seco_nvm_manager])
            LIB_STATIC_ADD="$LIB_STATIC_ADD $SECO_DIR/lib/hsm_lib.a $SECO_DIR/lib/seco_nvm_manager.a"
            LIB_ADD="$LIB_ADD -lz"
            ;;

      esac
    done
fi

# INTEL AES-NI
AC_ARG_ENABLE([aesni],
    [AS_HELP_STRING([--enable-aesni],[Enable wolfSSL AES-NI support (default: disabled)])],
    [ ENABLED_AESNI=$enableval ],
    [ ENABLED_AESNI=no ]
    )

# INTEL ASM
AC_ARG_ENABLE([intelasm],
    [AS_HELP_STRING([--enable-intelasm],[Enable All Intel ASM speedups (default: disabled)])],
    [ ENABLED_INTELASM=$enableval ],
    [ ENABLED_INTELASM=no ]
    )

if test "$ENABLED_ASM" = "yes"
then
    if test "$ENABLED_AESNI" = "small"
    then
        AM_CFLAGS="$AM_CFLAGS -DAES_GCM_AESNI_NO_UNROLL"
        ENABLED_AESNI=yes
    fi

    if test "$ENABLED_AESNI" = "yes" || test "$ENABLED_INTELASM" = "yes"
    then
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_AESNI"
        if test "$CC" != "icc"
        then
            case $host_os in
            mingw*)
                # Windows uses intrinsics for GCM which uses SSE4 instructions.
                # MSVC has own build files.
                AM_CFLAGS="$AM_CFLAGS -maes -msse4 -mpclmul"
                ;;
            *)
                # Intrinsics used in AES_set_decrypt_key (TODO: rework)
                AM_CFLAGS="$AM_CFLAGS -maes"
                ;;
            esac
        fi
        AS_IF([test "x$ENABLED_AESGCM" != "xno"],[AM_CCASFLAGS="$AM_CCASFLAGS -DHAVE_AESGCM"])
        AS_IF([test "x$ENABLED_SM3" != "xno"],[AM_CCASFLAGS="$AM_CCASFLAGS -DWOLFSSL_SM3"])
    fi

    if test "$ENABLED_INTELASM" = "yes"
    then
        AM_CFLAGS="$AM_CFLAGS -DUSE_INTEL_SPEEDUP"
        ENABLED_AESNI=yes
    fi

    if test "$host_cpu" = "x86_64" || test "$host_cpu" = "amd64"
    then
        AM_CCASFLAGS="$AM_CCASFLAGS -DWOLFSSL_X86_64_BUILD"
    fi
    if test "$host_cpu" = "x86"
    then
        AM_CCASFLAGS="$AM_CCASFLAGS -DWOLFSSL_X86_BUILD"
        ENABLED_X86_ASM=yes
    fi
fi

AC_ARG_ENABLE([aligndata],
    [AS_HELP_STRING([--enable-aligndata],[align data for ciphers (default: enabled)])],
    [ ENABLED_ALIGN_DATA=$enableval ],
    [ ENABLED_ALIGN_DATA=yes ]
    )

if test "$ENABLED_ALIGN_DATA" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_USE_ALIGN"
fi

# INTEL RDRAND
AC_ARG_ENABLE([intelrand],
    [AS_HELP_STRING([--enable-intelrand],[Enable Intel rdrand as preferred RNG source (default: disabled)])],
    [ ENABLED_INTELRDRAND=$enableval ],
    [ ENABLED_INTELRDRAND=no ]
    )

if test "$ENABLED_INTELRDRAND" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_INTEL_RDRAND"
fi

# AMD RDSEED
AC_ARG_ENABLE([amdrand],
    [AS_HELP_STRING([--enable-amdrand],[Enable AMD rdseed as preferred RNG seeding source (default: disabled)])],
    [ ENABLED_AMDRDSEED=$enableval ],
    [ ENABLED_AMDRDSEED=no ]
    )

if test "$ENABLED_AMDRDSEED" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_AMD_RDSEED"
fi


# Linux af_alg
AC_ARG_ENABLE([afalg],
    [AS_HELP_STRING([--enable-afalg],[Enable Linux af_alg use for crypto (default: disabled)])],
    [ ENABLED_AFALG=$enableval ],
    [ ENABLED_AFALG=no ]
    )

if test "$ENABLED_AFALG" = "yes"
then
    if test "$ENABLED_AESCCM" = "yes"
    then
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_AES_DIRECT"
    fi
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_AFALG"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_AFALG_HASH"
fi

if test "$ENABLED_AFALG" = "xilinx"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_AFALG_XILINX -DWOLFSSL_AFALG_XILINX_AES"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_AFALG_XILINX_SHA3 -DWOLFSSL_AFALG_XILINX_RSA"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_NOSHA3_224 -DWOLFSSL_NOSHA3_256 -DWOLFSSL_NOSHA3_512"
    ENABLED_AFALG="yes"
    ENABLED_XILINX="yes"
fi

if test "$ENABLED_AFALG" = "xilinx-aes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_AFALG_XILINX -DWOLFSSL_AFALG_XILINX_AES"
    ENABLED_AFALG="yes"
    ENABLED_XILINX="yes"
fi

if test "$ENABLED_AFALG" = "xilinx-sha3"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_AFALG_XILINX"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_AFALG_XILINX_SHA3"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_NOSHA3_224 -DWOLFSSL_NOSHA3_256 -DWOLFSSL_NOSHA3_512"
    ENABLED_AFALG="yes"
    ENABLED_XILINX="yes"
fi

if test "$ENABLED_AFALG" = "xilinx-rsa"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_AFALG_XILINX"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_AFALG_XILINX_RSA"
    ENABLED_AFALG="yes"
    ENABLED_XILINX="yes"
fi


# libkcapi

AC_ARG_ENABLE([kcapi-hash],
    [AS_HELP_STRING([--enable-kcapi-hash],[Enable libkcapi use for hashing (default: disabled)])],
    [ ENABLED_KCAPI_HASH=$enableval ],
    [ ENABLED_KCAPI_HASH=no ]
    )

AC_ARG_ENABLE([kcapi-hmac],
    [AS_HELP_STRING([--enable-kcapi-hmac],[Enable libkcapi use for HMAC (default: disabled)])],
    [ ENABLED_KCAPI_HMAC=$enableval ],
    [ ENABLED_KCAPI_HMAC=no ]
    )

AC_ARG_ENABLE([kcapi-aes],
    [AS_HELP_STRING([--enable-kcapi-aes],[Enable libkcapi use for AES (default: disabled)])],
    [ ENABLED_KCAPI_AES=$enableval ],
    [ ENABLED_KCAPI_AES=no ]
    )

AC_ARG_ENABLE([kcapi-rsa],
    [AS_HELP_STRING([--enable-kcapi-rsa],[Enable libkcapi use for RSA (default: disabled)])],
    [ ENABLED_KCAPI_RSA=$enableval ],
    [ ENABLED_KCAPI_RSA=no ]
    )

AC_ARG_ENABLE([kcapi-dh],
    [AS_HELP_STRING([--enable-kcapi-dh],[Enable libkcapi use for DH (default: disabled)])],
    [ ENABLED_KCAPI_DH=$enableval ],
    [ ENABLED_KCAPI_DH=no ]
    )

AC_ARG_ENABLE([kcapi-ecc],
    [AS_HELP_STRING([--enable-kcapi-ecc],[Enable libkcapi use for ECC (default: disabled)])],
    [ ENABLED_KCAPI_ECC=$enableval ],
    [ ENABLED_KCAPI_ECC=no ]
    )

AC_ARG_ENABLE([kcapi],
    [AS_HELP_STRING([--enable-kcapi],[Enable libkcapi use for crypto (default: disabled)])],
    [ ENABLED_KCAPI=$enableval ],
    [ ENABLED_KCAPI=no ]
    )

if test "$ENABLED_KCAPI" = "yes"
then
    AS_IF([test "$enable_kcapi_hash" != "no"], [ENABLED_KCAPI_HASH=yes])
    AS_IF([test "$enable_kcapi_hmac" != "no"], [ENABLED_KCAPI_HMAC=yes])
    AS_IF([test "$enable_kcapi_aes" != "no"], [ENABLED_KCAPI_AES=yes])
    AS_IF([test "$enable_kcapi_rsa" != "no"], [ENABLED_KCAPI_RSA=yes])
    AS_IF([test "$enable_kcapi_dh" != "no"], [ENABLED_KCAPI_DH=yes])
    AS_IF([test "$enable_kcapi_ecc" != "no"], [ENABLED_KCAPI_ECC=yes])
fi

if  test "$ENABLED_KCAPI_HASH" != "no" ||
    test "$ENABLED_KCAPI_HMAC" != "no" ||
    test "$ENABLED_KCAPI_AES" != "no" ||
    test "$ENABLED_KCAPI_RSA" != "no" ||
    test "$ENABLED_KCAPI_DH" != "no" ||
    test "$ENABLED_KCAPI_ECC" != "no"
then
    LIBS="$LIBS -lkcapi"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_KCAPI"
fi

if test "$ENABLED_KCAPI_HASH" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_KCAPI_HASH -DWOLFSSL_KCAPI_HASH_KEEP"
    # Linux Kernel doesn't support truncated SHA512 algorithms
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_NOSHA512_224 -DWOLFSSL_NOSHA512_256"
fi

if test "$ENABLED_KCAPI_HMAC" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_KCAPI_HMAC"
fi

if test "$ENABLED_KCAPI_AES" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_KCAPI_AES"
    HAVE_AESGCM_PORT=yes
    if test "$ENABLED_AESCCM" = "yes"
    then
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_AES_DIRECT"
    fi

    if test "$ENABLED_AESGCM_STREAM" = "yes"
    then
        AC_MSG_ERROR([--enable-aesgcm-stream is incompatible with --enable-kcapi.])
    fi
fi

if test "$ENABLED_KCAPI_RSA" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_KCAPI_RSA"
fi

if test "$ENABLED_KCAPI_DH" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_KCAPI_DH -DWOLFSSL_DH_EXTRA"
fi

if test "$ENABLED_KCAPI_ECC" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_KCAPI_ECC"
fi


# Support for Linux dev/crypto calls
AC_ARG_ENABLE([devcrypto],
    [AS_HELP_STRING([--enable-devcrypto],[Enable Linux dev crypto calls: all | aes (all aes support) | hash (all hash algos) | cbc (aes-cbc only) (default: disabled)])],
    [ ENABLED_DEVCRYPTO=$enableval ],
    [ ENABLED_DEVCRYPTO=no ]
    )

if test "$ENABLED_DEVCRYPTO" = "yes" || test "$ENABLED_DEVCRYPTO" = "all"
then
    #enable all devcrypto supported algorithms
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_DEVCRYPTO"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_DEVCRYPTO_CBC"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_DEVCRYPTO_AES"
    if test "$ENABLED_AESCCM" = "yes"
    then
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_AES_DIRECT"
    fi
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_DEVCRYPTO_HASH"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_NO_HASH_RAW"
    ENABLED_DEVCRYPTO=yes
fi
if test "$ENABLED_DEVCRYPTO" = "aes"
then
    #enable only AES-CBC algorithm support
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_DEVCRYPTO"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_DEVCRYPTO_AES"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_DEVCRYPTO_CBC"
    if test "$ENABLED_AESCCM" = "yes"
    then
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_AES_DIRECT"
    fi
    ENABLED_DEVCRYPTO=yes
fi
if test "$ENABLED_DEVCRYPTO" = "cbc"
then
    #enable only AES-CBC algorithm support
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_DEVCRYPTO"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_DEVCRYPTO_CBC"
    ENABLED_DEVCRYPTO=yes
fi
if test "$ENABLED_DEVCRYPTO" = "hash"
then
    #enable only hash algorithm support
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_DEVCRYPTO"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_DEVCRYPTO_HASH"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_NO_HASH_RAW"
    ENABLED_DEVCRYPTO=yes
fi
if test "$ENABLED_DEVCRYPTO" = "hmac"
then
    #enable only hmac algorithm support
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_DEVCRYPTO"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_DEVCRYPTO_HMAC"
    ENABLED_DEVCRYPTO=yes
fi
if test "$ENABLED_DEVCRYPTO" = "rsa"
then
    #enable only rsa algorithm support
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_DEVCRYPTO"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_DEVCRYPTO_RSA"
    ENABLED_DEVCRYPTO=yes
fi
if test "$ENABLED_DEVCRYPTO" = "seco"
then
    #enable support of devcrypto for algos not supported with seco
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_DEVCRYPTO"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_DEVCRYPTO_HMAC"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_DEVCRYPTO_RSA"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_DEVCRYPTO_CURVE25519"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_DEVCRYPTO_ECDSA"
    ENABLED_DEVCRYPTO=yes
fi


# Camellia
AC_ARG_ENABLE([camellia],
    [AS_HELP_STRING([--enable-camellia],[Enable wolfSSL Camellia support (default: disabled)])],
    [ ENABLED_CAMELLIA=$enableval ],
    [ ENABLED_CAMELLIA=no ]
    )

if test "$ENABLED_CAMELLIA" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_CAMELLIA"
fi


# MD2
AC_ARG_ENABLE([md2],
    [AS_HELP_STRING([--enable-md2],[Enable wolfSSL MD2 support (default: disabled)])],
    [ ENABLED_MD2=$enableval ],
    [ ENABLED_MD2=no ]
    )

if test "$ENABLED_BUMP" = "yes"
then
    ENABLED_MD2="yes"
fi

if test "$ENABLED_MD2" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_MD2"
fi


# NULL CIPHER
AC_ARG_ENABLE([nullcipher],
    [AS_HELP_STRING([--enable-nullcipher],[Enable wolfSSL NULL cipher support (default: disabled)])],
    [ ENABLED_NULL_CIPHER=$enableval ],
    [ ENABLED_NULL_CIPHER=no ]
    )

if test "$ENABLED_OPENSSH" = "yes"
then
    ENABLED_NULL_CIPHER="yes"
fi

if test "$ENABLED_NULL_CIPHER" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_NULL_CIPHER"
fi

# RIPEMD
AC_ARG_ENABLE([ripemd],
    [AS_HELP_STRING([--enable-ripemd],[Enable wolfSSL RIPEMD-160 support (default: disabled)])],
    [ ENABLED_RIPEMD=$enableval ],
    [ ENABLED_RIPEMD=no ]
    )

if test "$ENABLED_OPENSSH" = "yes" && test "x$ENABLED_FIPS" = "xno"
then
    ENABLED_RIPEMD="yes"
fi

if test "$ENABLED_RIPEMD" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_RIPEMD"
fi


# BLAKE2
AC_ARG_ENABLE([blake2],
    [AS_HELP_STRING([--enable-blake2],[Enable wolfSSL BLAKE2b support (default: disabled)])],
    [ ENABLED_BLAKE2=$enableval ],
    [ ENABLED_BLAKE2=no ]
    )

if test "$ENABLED_BLAKE2" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_BLAKE2 -DHAVE_BLAKE2B"
fi


AC_ARG_ENABLE([blake2s],
    [AS_HELP_STRING([--enable-blake2s],[Enable wolfSSL BLAKE2s support (default: disabled)])],
    [ ENABLED_BLAKE2S=$enableval ],
    [ ENABLED_BLAKE2S=no ]
    )

if test "$ENABLED_BLAKE2S" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_BLAKE2S"
    ENABLED_BLAKE2="yes"
fi


# set sha224 default
SHA224_DEFAULT=no
if test "$host_cpu" = "x86_64" || test "$host_cpu" = "aarch64" || test "$host_cpu" = "amd64"
then
    if test "x$ENABLED_AFALG" = "xno" && test "x$ENABLED_DEVCRYPTO" = "xno" &&
        ( test "x$ENABLED_FIPS" = "xno" ||
          ( test "$HAVE_FIPS_VERSION" = 2 && test "$HAVE_FIPS_VERSION_MINOR" != 1 ) )
    then
        SHA224_DEFAULT=yes
    fi
fi

# SHA224
AC_ARG_ENABLE([sha224],
    [AS_HELP_STRING([--enable-sha224],[Enable wolfSSL SHA-224 support (default: enabled on x86_64/amd64/aarch64)])],
    [ ENABLED_SHA224=$enableval ],
    [ ENABLED_SHA224=$SHA224_DEFAULT ]
    )

if test "$ENABLED_SHA224" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SHA224"
fi


# set sha3 default
SHA3_DEFAULT=no
if (test "$host_cpu" = "x86_64" || test "$host_cpu" = "aarch64" || test "$host_cpu" = "amd64") && test "$ENABLED_32BIT" = "no"
then
    if test "x$ENABLED_FIPS" = "xno" || test "$HAVE_FIPS_VERSION" -ge 2
    then
        SHA3_DEFAULT=yes
    fi
fi

# SHA3
AC_ARG_ENABLE([sha3],
    [AS_HELP_STRING([--enable-sha3],[Enable wolfSSL SHA-3 support (default: enabled on x86_64/amd64/aarch64)])],
    [ ENABLED_SHA3=$enableval ],
    [ ENABLED_SHA3=$SHA3_DEFAULT ]
    )

if test "$ENABLED_SHA3" = "small"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SHA3_SMALL"
fi

# SHAKE128
AC_ARG_ENABLE([shake128],
    [AS_HELP_STRING([--enable-shake128],[Enable wolfSSL SHAKE128 support (default: disabled)])],
    [ ENABLED_SHAKE128=$enableval ],
    [ ENABLED_SHAKE128=no ]
    )

# SHAKE256
AC_ARG_ENABLE([shake256],
    [AS_HELP_STRING([--enable-shake256],[Enable wolfSSL SHAKE256 support (default: disabled)])],
    [ ENABLED_SHAKE256=$enableval ],
    [ ENABLED_SHAKE256=no ]
    )

# SHA512
AC_ARG_ENABLE([sha512],
    [AS_HELP_STRING([--enable-sha512],[Enable wolfSSL SHA-512 support (default: enabled)])],
    [ ENABLED_SHA512=$enableval ],
    [ ENABLED_SHA512=yes ]
    )

# options that don't require sha512
if test "$ENABLED_LEANPSK" = "yes" || test "$ENABLED_LEANTLS" = "yes" || test "$ENABLED_32BIT" = "yes" || test "$ENABLED_16BIT" = "yes"
then
    ENABLED_SHA512="no"
fi

# options that require sha512
if test "$ENABLED_OPENSSH" = "yes" || test "$ENABLED_WPAS" = "yes" || test "$ENABLED_FORTRESS" = "yes"
then
    ENABLED_SHA512="yes"
    ENABLED_SHA384="yes"
fi

if test "$ENABLED_SHA512" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SHA512"
fi


# SHA384
AC_ARG_ENABLE([sha384],
    [AS_HELP_STRING([--enable-sha384],[Enable wolfSSL SHA-384 support (default: enabled)])],
    [ ENABLED_SHA384=$enableval ],
    [ ENABLED_SHA384=yes ]
    )

# options that don't require sha384
if test "$ENABLED_LEANPSK" = "yes" || test "$ENABLED_LEANTLS" = "yes" || test "$ENABLED_32BIT" = "yes" || test "$ENABLED_16BIT" = "yes"
then
    ENABLED_SHA384="no"
fi

# options that require sha384
if test "$ENABLED_OPENSSH" = "yes" || test "$ENABLED_WPAS" = "yes" || test "$ENABLED_FORTRESS" = "yes"
then
    ENABLED_SHA384="yes"
fi
if test "$ENABLED_SHA384" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SHA384"
fi


# SM3
AC_ARG_ENABLE([sm3],
    [AS_HELP_STRING([--enable-sm3],[Enable wolfSSL SM3 support (default: disabled)])],
    [ ENABLED_SM3=$enableval ],
    [ ENABLED_SM3=no ]
    )

if test "$ENABLED_SM3" = "small"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SM3_SMALL"
fi
if test "$ENABLED_SM3" != "no"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SM3"
fi

# SESSION CERTS
AC_ARG_ENABLE([sessioncerts],
    [AS_HELP_STRING([--enable-sessioncerts],[Enable session cert storing (default: disabled)])],
    [ ENABLED_SESSIONCERTS=$enableval ],
    [ ENABLED_SESSIONCERTS=no ]
    )

if test "x$ENABLED_NGINX" = "xyes" || test "x$ENABLED_OPENVPN" = "xyes" || \
   test "x$ENABLED_LIGHTY" = "xyes" || test "x$ENABLED_NETSNMP" = "xyes" || \
   test "x$ENABLED_STRONGSWAN" = "xyes" || test "x$ENABLED_HITCH" = "xyes"
then
    ENABLED_SESSIONCERTS=yes
fi
if test "$ENABLED_TLS13" = "yes" && test "$ENABLED_PSK" = "yes"
then
    ENABLED_SESSIONCERTS=yes
fi

if test "$ENABLED_SESSIONCERTS" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DSESSION_CERTS"
fi


# KEY GENERATION
AC_ARG_ENABLE([keygen],
    [AS_HELP_STRING([--enable-keygen],[Enable key generation (default: disabled)])],
    [ ENABLED_KEYGEN=$enableval ],
    [ ENABLED_KEYGEN=no ]
    )

if test "$ENABLED_BIND" = "yes" || test "$ENABLED_NTP" = "yes" || \
   test "$ENABLED_LIBSSH2" = "yes" || test "$ENABLED_OPENRESTY" = "yes" || \
   test "$ENABLED_NGINX" = "yes" || test "$ENABLED_WOLFENGINE" = "yes" || \
   test "$ENABLED_STRONGSWAN" = "yes" || test "$ENABLED_SE050" = "yes"
then
    ENABLED_KEYGEN=yes
fi


# CERT GENERATION
AC_ARG_ENABLE([certgen],
    [AS_HELP_STRING([--enable-certgen],[Enable cert generation (default: disabled)])],
    [ ENABLED_CERTGEN=$enableval ],
    [ ENABLED_CERTGEN=no ]
    )
if test "$ENABLED_OPENVPN" = "yes" || test "$ENABLED_OPENSSH" = "yes" || \
   test "$ENABLED_BIND" = "yes" || test "$ENABLED_NTP" = "yes" || \
   test "$ENABLED_CHRONY" = "yes" || test "$ENABLED_STRONGSWAN" = "yes" || \
   test "$ENABLED_OPENLDAP" = "yes" || test "$ENABLED_HITCH" = "yes"
then
    ENABLED_CERTGEN=yes
fi

# CERT REQUEST GENERATION
AC_ARG_ENABLE([certreq],
    [AS_HELP_STRING([--enable-certreq],[Enable cert request generation (default: disabled)])],
    [ ENABLED_CERTREQ=$enableval ],
    [ ENABLED_CERTREQ=no ]
    )

if test "$ENABLED_WPAS_DPP" = "yes"
then
    ENABLED_CERTREQ="yes"
fi

# CERT REQUEST EXTENSION
AC_ARG_ENABLE([certext],
    [AS_HELP_STRING([--enable-certext],[Enable cert request extensions (default: disabled)])],
    [ ENABLED_CERTEXT=$enableval ],
    [ ENABLED_CERTEXT=no ]
    )
if test "$ENABLED_OPENVPN" = "yes" || test "$ENABLED_STRONGSWAN" = "yes"
then
    ENABLED_CERTEXT=yes
fi


# DECODED CERT CACHE
AC_ARG_ENABLE([certgencache],
    [AS_HELP_STRING([--enable-certgencache],[Enable decoded cert caching (default: disabled)])],
    [ ENABLED_certgencache=$enableval ],
    [ ENABLED_certgencache=no ]
    )

if test "$ENABLED_certgencache" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_CERT_GEN_CACHE"
fi


# SEP
AC_ARG_ENABLE([sep],
    [AS_HELP_STRING([--enable-sep],[Enable sep extensions (default: disabled)])],
    [ ENABLED_SEP=$enableval ],
    [ ENABLED_SEP=no ]
    )
if test "$ENABLED_SEP" = "yes"
then
  AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SEP -DKEEP_PEER_CERT"
fi


# HKDF
AC_ARG_ENABLE([hkdf],
    [AS_HELP_STRING([--enable-hkdf],[Enable HKDF (HMAC-KDF) support (default: disabled)])],
    [ ENABLED_HKDF=$enableval ],
    [ ENABLED_HKDF=no ]
    )
if test "$ENABLED_TLS13" = "yes"
then
  ENABLED_HKDF="yes"
fi
if test "$ENABLED_HKDF" = "yes"
then
  AM_CFLAGS="$AM_CFLAGS -DHAVE_HKDF"
fi


# HPKE
AC_ARG_ENABLE([hpke],
    [AS_HELP_STRING([--enable-hpke],[Enable HKPE support (default: disabled)])],
    [ ENABLED_HPKE=$enableval ],
    [ ENABLED_HPKE=no ]
    )
if test "$ENABLED_HPKE" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_HPKE"

    test "$enable_hkdf" = "" && enable_hkdf=yes
fi

# X9.63 KDF
AC_ARG_ENABLE([x963kdf],
    [AS_HELP_STRING([--enable-x963kdf],[Enable X9.63 KDF support (default: disabled)])],
    [ ENABLED_X963KDF=$enableval ],
    [ ENABLED_X963KDF=no ]
    )
if test "$ENABLED_X963KDF" = "yes" || test "$ENABLED_WOLFENGINE" = "yes"
then
  AM_CFLAGS="$AM_CFLAGS -DHAVE_X963_KDF"
fi

# DSA
AC_ARG_ENABLE([dsa],
    [AS_HELP_STRING([--enable-dsa],[Enable DSA (default: disabled)])],
    [ ENABLED_DSA=$enableval ],
    [ ENABLED_DSA=no ]
    )

if test "$enable_dsa" = ""
then
    if (test "$ENABLED_OPENSSH" = "yes" && test "x$ENABLED_FIPS" = "xno") || test "$ENABLED_OPENVPN" = "yes" || test "$ENABLED_NGINX" = "yes" || test "$ENABLED_WPAS" = "yes" || test "$ENABLED_QT" = "yes" || test "$ENABLED_BIND" = "yes" || test "$ENABLED_LIBSSH2" = "yes" || test "$ENABLED_NTP" = "yes"
    then
        ENABLED_DSA="yes"
    fi
fi

if test "$ENABLED_DSA" = "no"
then
    AM_CFLAGS="$AM_CFLAGS -DNO_DSA"
else
    ENABLED_CERTS=yes
fi

# ECC Shamir
AC_ARG_ENABLE([eccshamir],
    [AS_HELP_STRING([--enable-eccshamir],[Enable ECC Shamir (default: enabled)])],
    [ ENABLED_ECC_SHAMIR=$enableval ],
    [ ENABLED_ECC_SHAMIR=yes ]
    )


# ECC
AC_ARG_ENABLE([ecc],
    [AS_HELP_STRING([--enable-ecc],[Enable ECC (default: enabled)])],
    [ ENABLED_ECC=$enableval ],
    [ ENABLED_ECC=yes ]
    )

# lean psk doesn't need ecc
if test "$ENABLED_LEANPSK" = "yes"
then
    ENABLED_ECC=no
fi

if test "$ENABLED_OPENSSH" = "yes" || test "$ENABLED_NGINX" = "yes" || test "$ENABLED_SIGNAL" = "yes"
then
    ENABLED_ECC="yes"
fi

if test "$ENABLED_ECC" != "no"
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_ECC -DTFM_ECC256"
    if test "$ENABLED_ECC_SHAMIR" = "yes" && test "$ENABLED_LOWRESOURCE" = "no"
    then
        AM_CFLAGS="$AM_CFLAGS -DECC_SHAMIR"
    fi

    if test "$ENABLED_ECC" = "nonblock"
    then
        AM_CFLAGS="$AM_CFLAGS -DWC_ECC_NONBLOCK"
    fi

    if test "$ENABLED_LOWRESOURCE" = "yes" && test "$ENABLED_FASTMATH" = "yes"
    then
        AM_CFLAGS="$AM_CFLAGS -DALT_ECC_SIZE"
    fi

    ENABLED_CERTS=yes
fi


# SM2
AC_ARG_ENABLE([sm2],
    [AS_HELP_STRING([--enable-sm2],[Enable wolfSSL SM2 support (default: disabled)])],
    [ ENABLED_SM2=$enableval ],
    [ ENABLED_SM2=no ]
    )

if test "$ENABLED_SP_MATH" = "yes"
then
    ENABLED_SM2="no"
fi
if test "$ENABLED_SM2" = "yes"
then
    if test "$ENABLED_ECC" = "no"
    then
        AC_MSG_ERROR([Cannot enable SM2 without enabling ecc.])
    fi

    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SM2 -DWOLFSSL_BASE16"
fi


# ECC Custom Curves
AC_ARG_ENABLE([ecccustcurves],
    [AS_HELP_STRING([--enable-ecccustcurves],[Enable ECC custom curves (default: disabled)])],
    [ ENABLED_ECCCUSTCURVES=$enableval ],
    [ ENABLED_ECCCUSTCURVES=no ]
    )

if test "$ENABLED_WPAS_DPP" = "yes"
then
    ENABLED_ECCCUSTCURVES="all"
fi

if test "$ENABLED_ECCCUSTCURVES" != "no"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_CUSTOM_CURVES"

    # For distro, all or ecccustcurves=all builds, enable all curve types
    if test "$ENABLED_DISTRO" = "yes" || test "$ENABLED_ALL" = "yes" || test "$ENABLED_ECCCUSTCURVES" = "all"
    then
        # Enable ECC SECPR2, SECPR3, BRAINPOOL and KOBLITZ curves
        AM_CFLAGS="$AM_CFLAGS -DHAVE_ECC_SECPR2 -DHAVE_ECC_SECPR3 -DHAVE_ECC_BRAINPOOL -DHAVE_ECC_KOBLITZ"

        # Enable ECC Cofactor support
        AM_CFLAGS="$AM_CFLAGS -DHAVE_ECC_CDH"

        # If fastmath enabled and on x86 use speedups
        if test "x$ENABLED_FASTMATH" = "xyes" && test "$host_cpu" = "x86_64" -o "$host_cpu" = "amd64"
        then
            AM_CFLAGS="$AM_CFLAGS -DTFM_ECC192 -DTFM_ECC224 -DTFM_ECC256 -DTFM_ECC384 -DTFM_ECC521"
        fi
    fi
fi

# ECC Minimum Key Size
ENABLED_ECCMINSZ=224
AC_ARG_WITH([eccminsz],
    [AS_HELP_STRING([--with-eccminsz=BITS],[Sets the ECC minimum key size (default: 224 bits)])],
    [
        ENABLED_ECCMINSZ=$withval
        AM_CFLAGS="$AM_CFLAGS -DECC_MIN_KEY_SZ=$withval"
    ]
)

# Compressed Key
AC_ARG_ENABLE([compkey],
    [AS_HELP_STRING([--enable-compkey],[Enable compressed keys support (default: disabled)])],
    [ ENABLED_COMPKEY=$enableval ],
    [ ENABLED_COMPKEY=no ]
    )

if test "$ENABLED_WPAS" = "yes" || test "$ENABLED_OPENSSLALL" = "yes"
then
    ENABLED_COMPKEY=yes
fi


# Brainpool (depends on _ECCCUSTCURVES)
if test "$ENABLED_ECCCUSTCURVES" != "no"
then
    BRAINPOOL_DEFAULT=yes
else
    BRAINPOOL_DEFAULT=no
fi

AC_ARG_ENABLE([brainpool],
    [AS_HELP_STRING([--enable-brainpool],[Enable Brainpool ECC curves (default: enabled with ECC custom curves)])],
    [ ENABLED_BRAINPOOL=$enableval ],
    [ ENABLED_BRAINPOOL="$BRAINPOOL_DEFAULT" ]
    )

if test "$ENABLED_BRAINPOOL" != "no"
then
    if test "$ENABLED_ECCCUSTCURVES" = "no"
    then
        AC_MSG_ERROR([cannot enable Brainpool without enabling ecccustcurves.])
    fi
    AM_CFLAGS="$AM_CFLAGS -DHAVE_ECC_BRAINPOOL"
fi


# for using memory optimization setting on both curve25519 and ed25519
ENABLED_CURVE25519_SMALL=no
ENABLED_ED25519_SMALL=no

# CURVE25519
AC_ARG_ENABLE([curve25519],
    [AS_HELP_STRING([--enable-curve25519],[Enable Curve25519 (default: disabled)])],
    [ ENABLED_CURVE25519=$enableval ],
    [ ENABLED_CURVE25519=no ]
    )

if test "$ENABLED_QUIC" = "yes" && test "$ENABLED_CURVE25519" = "no"
then
    ENABLED_CURVE25519=yes
fi

if test "$ENABLED_OPENSSH" = "yes" && test "x$ENABLED_FIPS" = "xno"
then
    ENABLED_CURVE25519="yes"
fi

if test "$ENABLED_CURVE25519" != "no"
then
    if test "$ENABLED_CURVE25519" = "small" || test "$ENABLED_LOWRESOURCE" = "yes"
    then
        AM_CFLAGS="$AM_CFLAGS -DCURVE25519_SMALL"
        ENABLED_CURVE25519_SMALL=yes
        ENABLED_CURVE25519=yes
    fi

    if test "$ENABLED_CURVE25519" = "no128bit" || test "$ENABLED_32BIT" = "yes"
    then
        AM_CFLAGS="$AM_CFLAGS -DNO_CURVED25519_128BIT"
        ENABLED_CURVE25519=yes
    fi

    AM_CFLAGS="$AM_CFLAGS -DHAVE_CURVE25519"
    ENABLED_FEMATH=yes
fi


# ED25519
AC_ARG_ENABLE([ed25519],
    [AS_HELP_STRING([--enable-ed25519],[Enable ED25519 (default: disabled)])],
    [ ENABLED_ED25519=$enableval ],
    [ ENABLED_ED25519=no ]
    )
AC_ARG_ENABLE([ed25519-stream],
    [AS_HELP_STRING([--enable-ed25519-stream],[Enable wolfSSL ED25519 support with streaming verify APIs (default: disabled)])],
    [ ENABLED_ED25519_STREAM=$enableval ],
    [ ENABLED_ED25519_STREAM=no ]
    )


if (test "$ENABLED_OPENSSH" = "yes" && test "x$ENABLED_FIPS" = "xno") || \
   test "$ENABLED_CHRONY" = "yes"
then
    ENABLED_ED25519="yes"
fi

# for using memory optimization setting on both curve448 and ed448
ENABLED_CURVE448_SMALL=no
ENABLED_ED448_SMALL=no

# CURVE448
AC_ARG_ENABLE([curve448],
    [AS_HELP_STRING([--enable-curve448],[Enable Curve448 (default: disabled)])],
    [ ENABLED_CURVE448=$enableval ],
    [ ENABLED_CURVE448=no ]
    )

if test "$ENABLED_CURVE448" != "no"
then
    if test "$ENABLED_CURVE448" = "small" || test "$ENABLED_LOWRESOURCE" = "yes"
    then
        AM_CFLAGS="$AM_CFLAGS -DCURVE448_SMALL"
        ENABLED_CURVE448_SMALL=yes
        ENABLED_CURVE448=yes
    fi

    if test "$ENABLED_CURVE448" = "no128bit" || test "$ENABLED_32BIT" = "yes"
    then
        AM_CFLAGS="$AM_CFLAGS -DNO_CURVED448_128BIT"
        ENABLED_CURVE448=yes
    fi

    AM_CFLAGS="$AM_CFLAGS -DHAVE_CURVE448"
    ENABLED_FE448=yes
fi

# ED448
AC_ARG_ENABLE([ed448],
    [AS_HELP_STRING([--enable-ed448],[Enable ED448 (default: disabled)])],
    [ ENABLED_ED448=$enableval ],
    [ ENABLED_ED448=no ]
    )
AC_ARG_ENABLE([ed448-stream],
    [AS_HELP_STRING([--enable-ed448-stream],[Enable wolfSSL ED448 support with streaming verify APIs (default: disabled)])],
    [ ENABLED_ED448_STREAM=$enableval ],
    [ ENABLED_ED448_STREAM=no ]
    )

if test "$ENABLED_ED448" != "no" && test "$ENABLED_32BIT" = "no"
then
    if test "$ENABLED_ED448" = "small" || test "$ENABLED_LOWRESOURCE" = "yes"
    then
        AM_CFLAGS="$AM_CFLAGS -DED448_SMALL"
        ENABLED_ED448_SMALL=yes
        ENABLED_CURVE448_SMALL=yes
        ENABLED_ED448=yes
    fi

    if test "$ENABLED_SHA512" = "no"
    then
        AC_MSG_ERROR([cannot enable ed448 without enabling sha512.])
    fi
    if test "$HAVE_FIPS_VERSION" = 2
    then
        AC_MSG_ERROR([cannot enable ed448 w/ dependency shake256 in FIPSv2 mode])
    fi
    ENABLED_FE448=yes
    ENABLED_GE448=yes
    AM_CFLAGS="$AM_CFLAGS -DHAVE_ED448"

    # EdDSA448 requires SHAKE256 which requires SHA-3
    if test "$ENABLED_SHA3" = "no"
    then
        ENABLED_SHA3=yes
    fi
    ENABLED_SHAKE256=yes

    ENABLED_CERTS=yes
fi

if test "$ENABLED_ED448_STREAM" != "no"
then
    if test "$ENABLED_ED448" = "no"
    then
        AC_MSG_ERROR([ED448 verify streaming enabled but ED448 is disabled])
    else
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_ED448_STREAMING_VERIFY"
        AM_CCASFLAGS="$AM_CCASFLAGS -DWOLFSSL_ED448_STREAMING_VERIFY"
    fi
fi


# FP ECC, Fixed Point cache ECC
AC_ARG_ENABLE([fpecc],
    [AS_HELP_STRING([--enable-fpecc],[Enable Fixed Point cache ECC (default: disabled)])],
    [ ENABLED_FPECC=$enableval ],
    [ ENABLED_FPECC=no ]
    )

if test "$ENABLED_FPECC" = "yes"
then
    if test "$ENABLED_ECC" = "no"
    then
        AC_MSG_ERROR([cannot enable fpecc without enabling ecc.])
    fi
    AM_CFLAGS="$AM_CFLAGS -DFP_ECC"
fi


# ECC encrypt
AC_ARG_ENABLE([eccencrypt],
    [AS_HELP_STRING([--enable-eccencrypt],[Enable ECC encrypt (default: disabled). yes = SEC1 standard, geniv = Generate IV, iso18033 = ISO 18033 standard, old = original wolfSSL algorithm])],
    [ ENABLED_ECC_ENCRYPT=$enableval ],
    [ ENABLED_ECC_ENCRYPT=no ]
    )

if test "$ENABLED_ECC_ENCRYPT" != "no"
then
    if test "$ENABLED_ECC" = "no"
    then
        AC_MSG_ERROR([cannot enable eccencrypt without enabling ecc.])
    fi
    if test "$ENABLED_HKDF" = "no"
    then
        AC_MSG_ERROR([cannot enable eccencrypt without enabling hkdf.])
    fi
    AM_CFLAGS="$AM_CFLAGS -DHAVE_ECC_ENCRYPT"
    if test "$ENABLED_ECC_ENCRYPT" = "old"
    then
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_ECIES_OLD"
    fi
    if test "$ENABLED_ECC_ENCRYPT" = "iso18033"
    then
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_ECIES_ISO18033"
    fi
    if test "$ENABLED_ECC_ENCRYPT" = "geniv"
    then
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_ECIES_GEN_IV"
    fi
fi

# Elliptic Curve-Based Certificateless Signatures for Identity-Based Encryption (ECCSI)
AC_ARG_ENABLE([eccsi],
    [AS_HELP_STRING([--enable-eccsi],[Enable ECCSI (default: disabled)])],
    [ ENABLED_ECCSI=$enableval ],
    [ ENABLED_ECCSI=no ]
    )

if test "x$ENABLED_ECCSI" = "xyes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFCRYPT_HAVE_ECCSI -DWOLFSSL_PUBLIC_MP"
fi

# Sakai-Kasahara Key Encryption (SAKKE) - pairing based crypto
AC_ARG_ENABLE([sakke],
    [AS_HELP_STRING([--enable-sakke],[Enable SAKKE - paring based crypto (default: disabled)])],
    [ ENABLED_SAKKE=$enableval ],
    [ ENABLED_SAKKE=no ]
    )

if test "x$ENABLED_SAKKE" = "xsmall"
then
    ENABLED_SAKKE="yes"
    ENABLED_SAKKE_SMALL="yes"
    AM_CFLAGS="$AM_CFLAGS -DWOLFCRYPT_SAKKE_SMALL"
fi
if test "x$ENABLED_SAKKE" = "xyes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFCRYPT_HAVE_SAKKE"
fi


# PSK
AC_ARG_ENABLE([psk],
    [AS_HELP_STRING([--enable-psk],[Enable PSK (default: disabled)])],
    [ ENABLED_PSK=$enableval ],
    [ ENABLED_PSK=no ]
    )

# Single PSK identity
AC_ARG_ENABLE([psk-one-id],
    [AS_HELP_STRING([--enable-psk-one-id],[Enable PSK (default: disabled)])],
    [ ENABLED_PSK_ONE_ID=$enableval ],
    [ ENABLED_PSK_ONE_ID=no ]
    )
if test "$ENABLED_PSK_ONE_ID" = "yes"
then
    if test "$ENABLED_PSK" = "no"
    then
        ENABLED_PSK="yes"
    fi
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_PSK_ONE_ID"
fi

# ERROR STRINGS
AC_ARG_ENABLE([errorstrings],
    [AS_HELP_STRING([--enable-errorstrings],[Enable error strings table (default: enabled)])],
    [ ENABLED_ERROR_STRINGS=$enableval ],
    [ ENABLED_ERROR_STRINGS=yes ]
    )

if test "$ENABLED_ERROR_STRINGS" = "no"
then
    AM_CFLAGS="$AM_CFLAGS -DNO_ERROR_STRINGS"
else
    # turn off error strings if leanpsk or leantls on
    if test "$ENABLED_LEANPSK" = "yes" || test "$ENABLED_LEANTLS" = "yes"
    then
        AM_CFLAGS="$AM_CFLAGS -DNO_ERROR_STRINGS"
        ENABLED_ERROR_STRINGS=no
    fi
fi


# ERROR QUEUE
AC_ARG_ENABLE([errorqueue],
    [AS_HELP_STRING([--enable-errorqueue],[Disables adding nodes to error queue when compiled with OPENSSL_EXTRA (default: enabled)])],
    [ ENABLED_ERROR_QUEUE=$enableval ],
    [ ENABLED_ERROR_QUEUE=yes ]
    )

# OLD TLS
AC_ARG_ENABLE([oldtls],
    [AS_HELP_STRING([--enable-oldtls],[Enable old TLS versions < 1.2 (default: enabled)])],
    [ ENABLED_OLD_TLS=$enableval ],
    [ ENABLED_OLD_TLS=yes ]
    )

if test "$ENABLED_CRYPTONLY" = "yes" || test "x$ENABLED_HARDEN_TLS" != "xno"
then
    ENABLED_OLD_TLS=no
fi
if test "$ENABLED_OLD_TLS" = "no"
then
    AM_CFLAGS="$AM_CFLAGS -DNO_OLD_TLS"
else
    # turn off old if leanpsk or leantls on
    if test "$ENABLED_LEANPSK" = "yes" || test "$ENABLED_LEANTLS" = "yes"
    then
        AM_CFLAGS="$AM_CFLAGS -DNO_OLD_TLS"
        ENABLED_OLD_TLS=no
    fi
fi


# TLSv1.2
AC_ARG_ENABLE([tlsv12],
    [AS_HELP_STRING([--enable-tlsv12],[Enable TLS versions 1.2 (default: enabled)])],
    [ ENABLED_TLSV12=$enableval ],
    [ ENABLED_TLSV12=yes ]
    )

if test "$ENABLED_CRYPTONLY" = "yes"
then
    ENABLED_TLSV12=no
fi
if test "$ENABLED_TLSV12" = "no"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_NO_TLS12 -DNO_OLD_TLS"
fi


# TLSv1.0
AC_ARG_ENABLE([tlsv10],
    [AS_HELP_STRING([--enable-tlsv10],[Enable old TLS versions 1.0 (default: disabled)])],
    [ ENABLED_TLSV10=$enableval ],
    [ ENABLED_TLSV10=no ]
    )

if test "$ENABLED_CRYPTONLY" = "yes"
then
    ENABLED_TLSV12=no
fi
if test "$ENABLED_TLSV10" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_ALLOW_TLSV10"
fi


# SSLv3
AC_ARG_ENABLE([sslv3],
    [AS_HELP_STRING([--enable-sslv3],[Enable SSL version 3.0 (default: disabled)])],
    [ ENABLED_SSLV3=$enableval ],
    [ ENABLED_SSLV3=no]
    )

if test "x$ENABLED_HAPROXY" = "xyes" && test "x$ENABLED_ALL" = "xno"
then
    ENABLED_SSLV3="yes"
fi
if test "$ENABLED_CRYPTONLY" = "yes"
then
    ENABLED_SSLV3=no
fi

if test "$ENABLED_SSLV3" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_ALLOW_SSLV3"
fi


# STACK SIZE info for testwolfcrypt and examples
AC_ARG_ENABLE([stacksize],
    [AS_HELP_STRING([--enable-stacksize],[Enable stack size info on examples (default: disabled)])],
    [ ENABLED_STACKSIZE=$enableval ],
    [ ENABLED_STACKSIZE=no ]
    )

if test "$ENABLED_STACKSIZE" != "no"
then
    AC_CHECK_FUNC([posix_memalign], [], [AC_MSG_ERROR(stacksize needs posix_memalign)])
    AC_CHECK_DECL([posix_memalign], [], [AC_MSG_ERROR(stacksize needs posix_memalign)])
    AC_CHECK_FUNC([pthread_attr_setstack], [], AC_CHECK_LIB([pthread],[pthread_attr_setstack]))
    AC_CHECK_DECL([pthread_attr_setstack], [], [AC_MSG_ERROR(stacksize needs pthread_attr_setstack)], [[#include <pthread.h>]])
    AM_CFLAGS="$AM_CFLAGS -DHAVE_STACK_SIZE"
fi

if test "$ENABLED_STACKSIZE" = "verbose"
then
    if test "$thread_ls_on" != "yes"
    then
        AC_MSG_ERROR(stacksize-verbose needs thread-local storage.)
    fi
    AM_CFLAGS="$AM_CFLAGS -DHAVE_STACK_SIZE_VERBOSE"
fi


# MEMORY
AC_ARG_ENABLE([memory],
    [AS_HELP_STRING([--enable-memory],[Enable memory callbacks (default: enabled)])],
    [ ENABLED_MEMORY=$enableval ],
    [ ENABLED_MEMORY=yes ]
    )

if test "$ENABLED_MEMORY" = "no"
then
    AM_CFLAGS="$AM_CFLAGS -DNO_WOLFSSL_MEMORY"
else
    # turn off memory cb if leanpsk or leantls on
    if test "$ENABLED_LEANPSK" = "yes" || test "$ENABLED_LEANTLS" = "yes"
    then
        # but don't turn on NO_WOLFSSL_MEMORY because using own
        ENABLED_MEMORY=no
    fi
fi


# MEMORY SIZE info
AC_ARG_ENABLE([trackmemory],
    [AS_HELP_STRING([--enable-trackmemory],[Enable memory use info on wolfCrypt and wolfSSL cleanup (default: disabled)])],
    [ ENABLED_TRACKMEMORY=$enableval ],
    [ ENABLED_TRACKMEMORY=no ]
    )

if test "$ENABLED_TRACKMEMORY" != "no"
then
    if test "$ENABLED_MEMORY" = "yes"
    then
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_TRACK_MEMORY"
    else
        AC_MSG_ERROR([trackmemory requires using wolfSSL memory (--enable-memory).])
    fi
    if test "$ENABLED_TRACKMEMORY" = "verbose"
    then
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_TRACK_MEMORY_VERBOSE"
    fi
fi

# MEMORY usage logging
AC_ARG_ENABLE([memorylog],
    [AS_HELP_STRING([--enable-memorylog],[Enable dynamic memory logging (default: disabled)])],
    [ ENABLED_MEMORYLOG=$enableval ],
    [ ENABLED_MEMORYLOG=no ]
    )

if test "$ENABLED_MEMORYLOG" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_MEMORY_LOG"
fi


# STACK usage logging
AC_ARG_ENABLE([stacklog],
    [AS_HELP_STRING([--enable-stacklog],[Enable stack logging (default: disabled)])],
    [ ENABLED_STACKLOG=$enableval ],
    [ ENABLED_STACKLOG=no ]
    )

if test "$ENABLED_STACKLOG" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_STACK_LOG -finstrument-functions"
fi


ENABLED_WOLFSENTRY=no

AC_ARG_WITH([wolfsentry],
    [AS_HELP_STRING([--with-wolfsentry=PATH],[PATH to directory with wolfSentry installation])],
    [WOLFSENTRY_INSTALLDIR=$withval],
    [WOLFSENTRY_INSTALLDIR=""])

AC_ARG_WITH([wolfsentry-lib],
    [AS_HELP_STRING([--with-wolfsentry-lib=PATH],[PATH to directory with wolfSentry library])],
    [WOLFSENTRY_LIB=$withval],
    [WOLFSENTRY_LIB=""])

AC_ARG_WITH([wolfsentry-include],
    [AS_HELP_STRING([--with-wolfsentry-include=PATH],[PATH to directory with wolfSentry header files])],
    [WOLFSENTRY_INCLUDE=$withval],
    [WOLFSENTRY_INCLUDE=""])

if test -n "$WOLFSENTRY_INSTALLDIR" || test -n "$WOLFSENTRY_LIB" || test -n "$WOLFSENTRY_INCLUDE"
then
    ENABLED_WOLFSENTRY=yes
fi

AC_ARG_ENABLE([wolfsentry],
    [AS_HELP_STRING([--enable-wolfsentry],[Enable wolfSentry hooks and plugins (default: disabled)])],
    [ ENABLED_WOLFSENTRY=$enableval ],
    [ ]
    )

if test "$WOLFSENTRY_LIB" = "" && test -n "$WOLFSENTRY_INSTALLDIR"
then
    WOLFSENTRY_LIB="${WOLFSENTRY_INSTALLDIR}/lib"
fi

if test "$WOLFSENTRY_INCLUDE" = "" && test -n "$WOLFSENTRY_INSTALLDIR"
then
    WOLFSENTRY_INCLUDE="${WOLFSENTRY_INSTALLDIR}/include"
fi

if test -n "$WOLFSENTRY_LIB"
then
    AC_MSG_CHECKING([for $WOLFSENTRY_LIB])
    if ! test -d "$WOLFSENTRY_LIB"
    then
        AC_MSG_ERROR([wolfSentry lib dir $WOLFSENTRY_LIB not found.])
    fi
    AC_MSG_RESULT([yes])
    WOLFSENTRY_LIB="-L$WOLFSENTRY_LIB"
fi

if test -n "$WOLFSENTRY_INCLUDE"
then
    AC_MSG_CHECKING([for $WOLFSENTRY_INCLUDE])
    if ! test -d "$WOLFSENTRY_INCLUDE"
    then
        AC_MSG_ERROR([wolfSentry include dir $WOLFSENTRY_INCLUDE not found.])
    fi
    AC_MSG_RESULT([yes])
    WOLFSENTRY_INCLUDE="-I$WOLFSENTRY_INCLUDE"
fi

if test "$ENABLED_WOLFSENTRY" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_WOLFSENTRY_HOOKS -DHAVE_EX_DATA -DHAVE_EX_DATA_CLEANUP_HOOKS"
    WOLFSENTRY_LIB="$WOLFSENTRY_LIB -lwolfsentry"
fi

AC_SUBST([WOLFSENTRY_LIB])
AC_SUBST([WOLFSENTRY_INCLUDE])


if test "$ENABLED_QT" = "yes"
then
    # Requires opensslextra and opensslall
    if test "x$ENABLED_OPENSSLALL" = "xno" && test "x$ENABLED_OPENSSLCOEXIST" = "xno"
    then
        ENABLED_OPENSSLALL="yes"
        ENABLED_OPENSSLEXTRA="yes"
        AM_CFLAGS="$AM_CFLAGS -DOPENSSL_EXTRA -DOPENSSL_ALL -DHAVE_EX_DATA"
    fi
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_QT -DSESSION_CERTS -DOPENSSL_NO_SSL2"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_KEY_GEN -DHAVE_EX_DATA"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_CUSTOM_CURVES -DHAVE_ECC_SECPR2 -DHAVE_ECC_SECPR3 -DHAVE_ECC_BRAINPOOL -DHAVE_ECC_KOBLITZ"
    if test "x$ENABLED_ALL" = "xno"; then
        # Don't enable old SSL/TLS for --enable-all, which is used by distro
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_ALLOW_SSLV3 -DWOLFSSL_ALLOW_TLSV10"
    fi

    # Requires OCSP make sure on
    if test "x$ENABLED_OCSP" = "xno"
    then
        ENABLED_OCSP="yes"
    fi

    # Requires PSK make sure on
    if test "x$ENABLED_PSK" = "xno"
    then
        ENABLED_PSK="yes"
    fi

    # Requires RC4 make sure on (if not forcefully disabled with --disable-arc4)
    test "$enable_arc4" = "" && enable_arc4=yes

    if test "x$ENABLED_CERTEXT" = "xno"
    then
        ENABLED_CERTEXT="yes"
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_CERT_EXT"
    fi

    if test "x$ENABLED_CERTGEN" = "xno"
    then
        ENABLED_CERTGEN="yes"
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_CERT_GEN"
    fi

    # requires oldnames disabled
    enable_oldnames=no
fi

AC_ARG_ENABLE([qt-test],
    [AS_HELP_STRING([--enable-qt-test],[Enable qt tests (default: disabled)])],
    [ ENABLED_QT_TEST=$enableval ],
    [ ENABLED_QT_TEST=no ]
    )

if test "$ENABLED_QT_TEST" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DOPENSSL_NO_SSL3 -DWOLFSSL_STATIC_RSA"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_STATIC_PSK"
fi

# RSA
AC_ARG_ENABLE([rsa],
    [AS_HELP_STRING([--enable-rsa],[Enable RSA (default: enabled)])],
    [ ENABLED_RSA=$enableval ],
    [ ENABLED_RSA=yes ]
    )

if test "$ENABLED_RSA" = "no"
then
    AM_CFLAGS="$AM_CFLAGS -DNO_RSA"
else
    # turn off RSA if leanpsk or leantls on
    if test "$ENABLED_LEANPSK" = "yes" || test "$ENABLED_LEANTLS" = "yes"
    then
        AM_CFLAGS="$AM_CFLAGS -DNO_RSA"
        ENABLED_RSA=no
     else
        ENABLED_CERTS=yes
    fi
fi

AC_ARG_ENABLE([oaep],
    [AS_HELP_STRING([--enable-oaep],[Enable RSA OAEP (default: enabled)])],
    [ ENABLED_OAEP=$enableval ],
    [ ENABLED_OAEP=yes ]
    )

if test "$ENABLED_OAEP" = "no"
then
    AM_CFLAGS="$AM_CFLAGS -DWC_NO_RSA_OAEP"
fi

AC_ARG_ENABLE([rsapub],
    [AS_HELP_STRING([--enable-rsapub],[Enable RSA Public Only (default: disabled)])],
    [ ENABLED_RSAPUB=$enableval ],
    [ ENABLED_RSAPUB=no ]
    )

if test "$ENABLED_RSAPUB" = "yes"
then
    if test "$ENABLED_RSA" = "no"
    then
        ENABLED_RSA="yes"
    fi
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_RSA_PUBLIC_ONLY"
fi

AC_ARG_ENABLE([rsavfy],
    [AS_HELP_STRING([--enable-rsavfy],[Enable RSA Verify Inline Only (default: disabled)])],
    [ ENABLED_RSAVFY=$enableval ],
    [ ENABLED_RSAVFY=no ]
    )

if test "$ENABLED_RSAVFY" = "yes"
then
    if test "$ENABLED_RSA" = "no"
    then
        ENABLED_RSA="yes"
    fi
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_RSA_PUBLIC_ONLY -DWOLFSSL_RSA_VERIFY_ONLY"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_RSA_VERIFY_INLINE -DNO_SIG_WRAPPER"
    AM_CFLAGS="$AM_CFLAGS -DNO_CHECK_PRIVATE_KEY"
fi


# RSA-PSS
AC_ARG_ENABLE([rsapss],
    [  --enable-rsapss         Enable RSA-PSS (default: disabled)],
    [ ENABLED_RSAPSS=$enableval ],
    [ ENABLED_RSAPSS=no ]
    )

if test "$ENABLED_RSA" = "no"
then
    ENABLED_RSAPSS="no"
else
    if test "$ENABLED_TLS13" = "yes"
    then
        ENABLED_RSAPSS="yes"
    fi
fi
if test "$ENABLED_RSAPSS" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWC_RSA_PSS -DWOLFSSL_PSS_LONG_SALT"
fi


# DH
AC_ARG_ENABLE([dh],
    [AS_HELP_STRING([--enable-dh],[Enable DH (default: enabled)])],
    [ ENABLED_DH=$enableval ],
    [ ENABLED_DH=yes ]
    )

if test "$ENABLED_OPENSSH" = "yes" && test "$ENABLED_DH" = "no"
then
    ENABLED_DH="yes"
fi

if test "$ENABLED_DH" = "no"
then
    AM_CFLAGS="$AM_CFLAGS -DNO_DH"
else
    # turn off DH if leanpsk or leantls on
    if test "$ENABLED_LEANPSK" = "yes" || test "$ENABLED_LEANTLS" = "yes"
    then
        AM_CFLAGS="$AM_CFLAGS -DNO_DH"
        ENABLED_DH=no
    fi
fi

if test "$ENABLED_DH" = "const"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_DH_CONST"
fi

if test "$ENABLED_SNIFFER" = "yes" && test "$ENABLED_DH" != "no"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_DH_EXTRA"
fi


# Anonymous
AC_ARG_ENABLE([anon],
    [AS_HELP_STRING([--enable-anon],[Enable Anonymous (default: disabled)])],
    [ ENABLED_ANON=$enableval ],
    [ ENABLED_ANON=no ]
    )

if test "x$ENABLED_WPAS" = "xyes"    || test "x$ENABLED_NGINX" = "xyes" || \
   test "x$ENABLED_HAPROXY" = "xyes" || test "$ENABLED_RSYSLOG" = "yes"
then
    ENABLED_ANON=yes
fi
if test "x$ENABLED_ANON" = "xyes"
then
    if test "$ENABLED_DH" = "no"
    then
        AC_MSG_ERROR([Anonymous suite requires DH.])
    fi
    AM_CFLAGS="$AM_CFLAGS -DHAVE_ANON"
fi


# ASN
# turn off asn, which means no certs, no rsa, no dsa, no ecc,
# and no big int (unless dh is on)
AC_ARG_ENABLE([asn],
    [AS_HELP_STRING([--enable-asn],[Enable ASN (default: enabled)])],
    [ ENABLED_ASN=$enableval ],
    [ ENABLED_ASN=yes ]
    )

if test "$ENABLED_ASN" = "no"
then
    AM_CFLAGS="$AM_CFLAGS -DNO_ASN -DNO_ASN_CRYPT"
    enable_pwdbased=no
else
    if test "$ENABLED_ASN" = "template"; then
        ENABLED_ASN="yes"
    fi
    if test "$ENABLED_ASN" = "yes"; then
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_ASN_TEMPLATE"
    else
        if test "$ENABLED_ASN" != "original"; then
            AC_MSG_ERROR([Invalid asn option. Valid are: template or original. Seen: $ENABLED_ASN.])
        fi
    fi

    # turn off ASN if leanpsk on
    if test "$ENABLED_LEANPSK" = "yes"
    then
        AM_CFLAGS="$AM_CFLAGS -DNO_ASN -DNO_BIG_INT"
        ENABLED_ASN=no
    else
        if test "$ENABLED_ASN" = "nocrypt"
        then
            AM_CFLAGS="$AM_CFLAGS -DNO_ASN_CRYPT"
            enable_pwdbased=no
        fi
    fi
fi

if test "$ENABLED_RSA" = "yes" && test "$ENABLED_RSAVFY" = "no" && \
   test "$ENABLED_ASN" = "no" && test "$ENABLED_LOWRESOURCE" = "no"
then
    AC_MSG_ERROR([please disable rsa if disabling asn.])
fi

if test "$ENABLED_DSA" = "yes" && test "$ENABLED_ASN" = "no"
then
    AC_MSG_ERROR([please disable dsa if disabling asn.])
fi

if test "x$ENABLED_ECC" != "xno" && test "x$ENABLED_ASN" = "xno"
then
    AC_MSG_ERROR([please disable ecc if disabling asn.])
fi

# No Big Int (ASN, DSA, RSA, DH, ECC and compatibility layer need bigint)
if test "$ENABLED_ASN" = "no" && test "$ENABLED_DSA" = "no" && \
   test "$ENABLED_DH" = "no" && test "$ENABLED_ECC" = "no" && \
   test "$ENABLED_RSA" = "no" && test "$ENABLED_OPENSSLEXTRA" = "no" && \
   test "$ENABLED_OPENSSLALL" = "yes"
then
    ENABLED_SP_MATH_ALL="no"
    ENABLED_FASTMATH="no"
    ENABLED_HEAPMATH="no"
    ENABLED_BIGNUM="no"
else
    ENABLED_BIGNUM="yes"
fi

case $host_os in
*linux* | *darwin* | *freebsd*)
    DEF_ASN_PRINT="yes"
    ;;
*)
    DEF_ASN_PRINT="no"
    ;;
esac

AC_ARG_ENABLE([asn-print],
    [AS_HELP_STRING([--enable-asn-print],[Enable ASN Print API (default: enabled)])],
    [ ENABLED_ASN_PRINT=$enableval ],
    [ ENABLED_ASN_PRINT=$DEF_ASN_PRINT ]
    )

if test "$ENABLED_ASN_PRINT" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_ASN_PRINT"
fi


# AES
AC_ARG_ENABLE([aes],
    [AS_HELP_STRING([--enable-aes],[Enable AES (default: enabled)])],
    [ ENABLED_AES=$enableval ],
    [ ENABLED_AES=yes ]
    )

if test "$ENABLED_AES" = "no"
then
    AM_CFLAGS="$AM_CFLAGS -DNO_AES"
    if test "$ENABLED_FORTRESS" = "yes"
    then
        AC_MSG_ERROR([fortress requires aes])
    fi
    if test "$ENABLED_ECC_ENCRYPT" = "yes"
    then
        AC_MSG_ERROR([cannot enable eccencrypt and hkdf without aes.])
    fi
    if test "$ENABLED_AESGCM" != "no"
    then
        AC_MSG_ERROR([AESGCM requires AES.])
    fi
    if test "$ENABLED_AESCCM" = "yes"
    then
        AC_MSG_ERROR([AESCCM requires AES.])
    fi
    if test "$ENABLED_AESCTR" = "yes"
    then
        AC_MSG_ERROR([AESCTR requires AES.])
    fi
else
    # turn off AES if leanpsk on
    if test "$ENABLED_LEANPSK" = "yes"
    then
        AM_CFLAGS="$AM_CFLAGS -DNO_AES"
        ENABLED_AES=no
    fi
fi

# DTLSv1.3
AC_ARG_ENABLE([dtls13],
    [AS_HELP_STRING([--enable-dtls13],[Enable wolfSSL DTLS v1.3 (default: disabled)])],
    [ ENABLED_DTLS13=$enableval ],
    [ ENABLED_DTLS13=no ]
    )
if test "x$ENABLED_DTLS13" = "xyes"
then
        if test "x$ENABLED_DTLS" != "xyes" || test "x$ENABLED_TLS13" != "xyes"
        then
                AC_MSG_ERROR([You need to enable both DTLS and TLSv1.3 to use DTLSv1.3])
        fi
        if test "x$ENABLED_SEND_HRR_COOKIE" = "xundefined"
        then
                AC_MSG_NOTICE([DTLSv1.3 is enabled, enabling HRR cookie])
                AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SEND_HRR_COOKIE"
                ENABLED_SEND_HRR_COOKIE="yes"
        fi
  AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_DTLS13 -DWOLFSSL_W64_WRAPPER"
  if test "x$ENABLED_AES" = "xyes"
  then
      AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_AES_DIRECT"
  fi
fi

# DTLS CID support
AC_ARG_ENABLE([dtlscid],
    [AS_HELP_STRING([--enable-dtlscid],[Enable wolfSSL DTLS ConnectionID (default: disabled)])],
    [ ENABLED_DTLS_CID=$enableval ],
    [ ENABLED_DTLS_CID=no ]
    )
if test "x$ENABLED_DTLS_CID" = "xyes"
then
        if test "x$ENABLED_DTLS13" != "xyes"
        then
                AC_MSG_ERROR([You need to enable DTLSv1.3 to use DTLS ConnectionID])
        fi
  AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_DTLS_CID"
fi

# CODING
AC_ARG_ENABLE([coding],
    [AS_HELP_STRING([--enable-coding],[Enable Coding base 16/64 (default: enabled)])],
    [ ENABLED_CODING=$enableval ],
    [ ENABLED_CODING=yes ]
    )

if test "$ENABLED_CODING" = "no"
then
    AM_CFLAGS="$AM_CFLAGS -DNO_CODING"
else
    # turn off CODING if leanpsk on
    if test "$ENABLED_LEANPSK" = "yes"
    then
        AM_CFLAGS="$AM_CFLAGS -DNO_CODING"
        ENABLED_CODING=no
    fi
fi


# Base64 Encode
BASE64ENCODE_DEFAULT=no
if test "$host_cpu" = "x86_64" || test "$host_cpu" = "amd64"
then
BASE64ENCODE_DEFAULT=yes
fi
AC_ARG_ENABLE([base64encode],
    [AS_HELP_STRING([--enable-base64encode],[Enable Base64 encoding (default: enabled on x86_64/amd64)])],
    [ ENABLED_BASE64ENCODE=$enableval ],
    [ ENABLED_BASE64ENCODE=$BASE64ENCODE_DEFAULT ]
    )
if test "$ENABLED_BASE64ENCODE" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_BASE64_ENCODE"
fi


# Base16
AC_ARG_ENABLE([base16],
    [AS_HELP_STRING([--enable-base16],[Enable Base16 encoding/decoding (default: disabled)])],
    [ ENABLED_BASE16=$enableval ],
    [ ENABLED_BASE16=no ]
    )

if test "$ENABLED_CAAM" = "qnx"
then
    ENABLED_BASE16=yes
fi

if test "$ENABLED_BASE16" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_BASE16"
fi


# MD4
AC_ARG_ENABLE([md4],
    [AS_HELP_STRING([--enable-md4],[Enable MD4 (default: disabled)])],
    [ ENABLED_MD4=$enableval ],
    [ ENABLED_MD4=no ]
    )


# DES3
AC_ARG_ENABLE([des3],
    [AS_HELP_STRING([--enable-des3],[Enable DES3 (default: disabled)])],
    [ ENABLED_DES3=$enableval ],
    [ ENABLED_DES3=no ]
    )

# Enable 3DES with OpenSSH and FIPS 140-2 but not 140-3
if (test "$ENABLED_OPENSSH" = "yes" && \
        (test "x$ENABLED_FIPS" = "xno" || test "$HAVE_FIPS_VERSION" -le 2)) || \
   test "$ENABLED_QT" = "yes" || test "$ENABLED_OPENVPN" = "yes" || \
   test "x$ENABLED_WPAS" != "xno"  || test "$ENABLED_NETSNMP" = "yes" || \
   test "$ENABLED_LIBSSH2" = "yes" || test "$ENABLED_KRB" = "yes" || \
   test "$ENABLED_WOLFENGINE" = "yes" || test "$ENABLED_STRONGSWAN" = "yes"
then
    ENABLED_DES3="yes"
fi

# ARC4
if (test "$ENABLED_OPENSSH" = "yes" && test "x$ENABLED_FIPS" = "xno") || \
   test "$ENABLED_WPAS" = "yes" || test "$ENABLED_KRB" = "yes"
then
    # Requires RC4 make sure on (if not forcefully disabled with --disable-arc4)
    test "$enable_arc4" = "" && enable_arc4=yes
fi

AC_ARG_ENABLE([arc4],
    [AS_HELP_STRING([--enable-arc4],[Enable ARC4 (default: disabled)])],
    [ ENABLED_ARC4=$enableval ],
    [ ENABLED_ARC4=no ]
    )

# MD5
AC_ARG_ENABLE([md5],
    [AS_HELP_STRING([--enable-md5],[Enable MD5 (default: enabled)])],
    [ ENABLED_MD5=$enableval ],
    [ ENABLED_MD5=yes ]
    )

# SHA
AC_ARG_ENABLE([sha],
    [AS_HELP_STRING([--enable-sha],[Enable SHA (default: enabled)])],
    [ ENABLED_SHA=$enableval ],
    [ ENABLED_SHA=yes ]
    )
if test "$ENABLED_SHA" = "no"
then
    AM_CFLAGS="$AM_CFLAGS -DNO_SHA -DNO_OLD_TLS"
else
    # turn off SHA if leanpsk or leantls on
    if test "$ENABLED_LEANPSK" = "yes" || test "$ENABLED_LEANTLS" = "yes"
    then
        AM_CFLAGS="$AM_CFLAGS -DNO_SHA -DNO_OLD_TLS"
        ENABLED_SHA=no
    fi
fi

if test "$ENABLED_SHA" = "no" && test "$ENABLED_DSA" != "no"
then
    AC_MSG_ERROR([please disable DSA if disabling SHA-1.])
fi


# SipHash
AC_ARG_ENABLE([siphash],
    [AS_HELP_STRING([--enable-siphash],[Enable SipHash (default: disabled)])],
    [ ENABLED_SIPHASH=$enableval ],
    [ ENABLED_SIPHASH=no ]
    )

AS_IF([test "x$ENABLED_SIPHASH" = "xyes"],
      [AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SIPHASH"])


# CMAC
AC_ARG_ENABLE([cmac],
    [AS_HELP_STRING([--enable-cmac],[Enable CMAC (default: disabled)])],
    [ ENABLED_CMAC=$enableval ],
    [ ENABLED_CMAC=no ]
    )

if test "$ENABLED_WPAS" != "no" || test "$ENABLED_NTP" = "yes" || test "$ENABLED_AESSIV" = "yes" || test "$ENABLED_WOLFENGINE" = "yes"
then
    ENABLED_CMAC=yes
fi

AS_IF([test "x$ENABLED_CMAC" = "xyes"],
      [AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_CMAC -DWOLFSSL_AES_DIRECT"])


# AES-XTS
AC_ARG_ENABLE([xts],
    [AS_HELP_STRING([--enable-xts],[Enable XTS (default: disabled)])],
    [ ENABLED_XTS=$enableval ],
    [ ENABLED_XTS=no ]
    )

AS_IF([test "x$ENABLED_XTS" = "xyes"],
      [AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_AES_XTS -DWOLFSSL_AES_DIRECT"])

# Web Server Build
AC_ARG_ENABLE([webserver],
    [AS_HELP_STRING([--enable-webserver],[Enable Web Server (default: disabled)])],
    [ ENABLED_WEBSERVER=$enableval ],
    [ ENABLED_WEBSERVER=no ]
    )

if test "$ENABLED_WEBSERVER" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_WEBSERVER"
fi

# Web Client Build (HTTP Client)
AC_ARG_ENABLE([webclient],
    [AS_HELP_STRING([--enable-webclient],[Enable Web Client (HTTP) (default: disabled)])],
    [ ENABLED_WEBCLIENT=$enableval ],
    [ ENABLED_WEBCLIENT=no ]
    )

if test "$ENABLED_WEBCLIENT" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_HTTP_CLIENT"
fi

# RC2
AC_ARG_ENABLE([rc2],
    [AS_HELP_STRING([--enable-rc2],[Enable RC2 encryption (default: disabled)])],
    [ ENABLED_RC2=$enableval ],
    [ ENABLED_RC2=no ]
    )

if test "$ENABLED_RC2" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWC_RC2"
fi


# FIPS feature and macro setup
AS_CASE([$FIPS_VERSION],
    [v5*|ready|dev], [ # FIPS 140-3

        AM_CFLAGS="$AM_CFLAGS \
            -DHAVE_FIPS \
            -DHAVE_FIPS_VERSION=$HAVE_FIPS_VERSION \
            -DHAVE_FIPS_VERSION_MINOR=$HAVE_FIPS_VERSION_MINOR \
            -DHAVE_ECC_CDH \
            -DWC_RSA_NO_PADDING \
            -DECC_USER_CURVES \
            -DHAVE_ECC384 \
            -DHAVE_ECC521 \
            -DWOLFSSL_VALIDATE_FFC_IMPORT \
            -DHAVE_FFDHE_Q \
            -DHAVE_FFDHE_3072 \
            -DHAVE_FFDHE_4096 \
            -DHAVE_FFDHE_6144 \
            -DHAVE_FFDHE_8192"

        # KCAPI API does not support custom k for sign, don't force enable ECC key sizes and do not use seed callback
        AS_IF([test "x$ENABLED_KCAPI_ECC" = "xno"],
            [AM_CFLAGS="$AM_CFLAGS \
                -DWC_RNG_SEED_CB \
                -DWOLFSSL_ECDSA_SET_K \
                -DWOLFSSL_VALIDATE_ECC_IMPORT \
                -DWOLFSSL_VALIDATE_ECC_KEYGEN \
                -DHAVE_ECC192 \
                -DHAVE_ECC224 \
                -DHAVE_ECC256"])

        DEFAULT_MAX_CLASSIC_ASYM_KEY_BITS=8192

        # force various features to FIPS 140-3 defaults, unless overridden with dev:

        AS_IF([test "$ENABLED_KEYGEN" != "yes" && (test "$FIPS_VERSION" != "dev" || test "$enable_keygen" != "no")],
            [ENABLED_KEYGEN="yes"; AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_KEY_GEN"])

        AS_IF([test "$ENABLED_COMPKEY" = "yes" && (test "$FIPS_VERSION" != "dev" || test "$enable_compkey" != "yes")],
            [ENABLED_COMPKEY="no"])

        AS_IF([test "$ENABLED_SHA224" != "yes" && (test "$FIPS_VERSION" != "dev" || test "$enable_sha224" != "no")],
            [ENABLED_SHA224="yes"; AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SHA224"])

        AS_IF([test "$ENABLED_WOLFSSH" != "yes" && (test "$FIPS_VERSION" != "dev" || test "$enable_ssh" != "no")],
            [enable_ssh="yes"])

        # Shake128 is a SHA-3 algorithm not in our FIPS algorithm list
        AS_IF([test "$ENABLED_SHAKE128" != "no" && (test "$FIPS_VERSION" != "dev" || test "$enable_shake128" != "yes")],
            [ENABLED_SHAKE128=no; AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_NO_SHAKE128"])

        # Shake256 is a SHA-3 algorithm not in our FIPS algorithm list
        AS_IF([test "$ENABLED_SHAKE256" != "no" && (test "$FIPS_VERSION" != "dev" || test "$enable_shake256" != "yes")],
            [ENABLED_SHAKE256=no; AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_NO_SHAKE256"])

        # SHA512-224 and SHA512-256 are SHA-2 algorithms not in our FIPS algorithm list
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_NOSHA512_224 -DWOLFSSL_NOSHA512_256"

        AS_IF([test "$ENABLED_AESCCM" != "yes" && (test "$FIPS_VERSION" != "dev" || test "$enable_aesccm" != "no")],
            [ENABLED_AESCCM="yes"; AM_CFLAGS="$AM_CFLAGS -DHAVE_AESCCM"])

        AS_IF([test "$ENABLED_RSAPSS" != "yes" && (test "$FIPS_VERSION" != "dev" || test "$enable_rsapss" != "no")],
            [ENABLED_RSAPSS="yes"; AM_CFLAGS="$AM_CFLAGS -DWC_RSA_PSS"])

        AS_IF([test "$ENABLED_ECC" != "yes" && (test "$FIPS_VERSION" != "dev" || test "$enable_ecc" != "no")],
            [ENABLED_ECC="yes"; AM_CFLAGS="$AM_CFLAGS -DHAVE_ECC -DTFM_ECC256"
             AS_IF([test "$ENABLED_ECC_SHAMIR" = "yes" && (test "$FIPS_VERSION" != "dev" || test "$enable_eccshamir" != "no")],
                 [AM_CFLAGS="$AM_CFLAGS -DECC_SHAMIR"])])

        AS_IF([test "$ENABLED_AESCTR" != "yes" && (test "$FIPS_VERSION" != "dev" || test "$enable_aesctr" != "no")],
            [ENABLED_AESCTR="yes"; AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_AES_COUNTER"])

        AS_IF([test "$ENABLED_CMAC" != "yes" && (test "$FIPS_VERSION" != "dev" || test "$enable_cmac" != "no")],
            [ENABLED_CMAC="yes"; AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_CMAC"])

        AS_IF([test "$ENABLED_HKDF" != "yes" && (test "$FIPS_VERSION" != "dev" || test "$enable_hkdf" != "no")],
            [ENABLED_HKDF="yes"; AM_CFLAGS="$AM_CFLAGS -DHAVE_HKDF"])

        AS_IF([test "$ENABLED_INTELASM" = "yes"],
            [AM_CFLAGS="$AM_CFLAGS -DFORCE_FAILURE_RDSEED"])

        AS_IF([test "$ENABLED_SHA512" = "no" && (test "$FIPS_VERSION" != "dev" || test "$enable_sha512" != "no")],
            [ENABLED_SHA512="yes"; AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SHA512 -DWOLFSSL_SHA384"])

        AS_IF([test "$ENABLED_AESGCM" = "no" && (test "$FIPS_VERSION" != "dev" || test "$enable_aesgcm" != "no")],
            [ENABLED_AESGCM="yes"; AM_CFLAGS="$AM_CFLAGS -DHAVE_AESGCM"; AM_CCASFLAGS="$AM_CCASFLAGS -DHAVE_AESGCM"])

        # AES-GCM streaming isn't part of the current FIPS suite.
        AS_IF([test "$ENABLED_AESGCM_STREAM" = "yes" && (test "$FIPS_VERSION" != "dev" || test "$enable_aesgcm_stream" != "yes")],
            [ENABLED_AESGCM_STREAM="no"])

        # Old TLS requires MD5 + HMAC, which is not allowed under FIPS 140-3
        AS_IF([test "$ENABLED_OLD_TLS" != "no"],
            [ENABLED_OLD_TLS="no"; AM_CFLAGS="$AM_CFLAGS -DNO_OLD_TLS"])

        AS_IF([test $HAVE_FIPS_VERSION_MINOR -ge 2],
            [AS_IF([test "x$ENABLED_AESOFB" = "xno" && (test "$FIPS_VERSION" != "dev" || test "$enable_aesofb" != "no")],
                [ENABLED_AESOFB="yes"; AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_AES_OFB"])])

        AS_IF([(test "$ENABLED_AESCCM" = "yes" && test "$HAVE_AESCCM_PORT" != "yes") ||
               (test "$ENABLED_AESCTR" = "yes" && test "$HAVE_AESCTR_PORT" != "yes") ||
               (test "$ENABLED_AESGCM" = "yes" && test "$HAVE_AESGCM_PORT" != "yes") ||
               (test "$ENABLED_AESOFB" = "yes" && test "$HAVE_AESOFB_PORT" != "yes")],
            [AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_AES_DIRECT -DHAVE_AES_ECB"])
    ],

    [v2],[ # FIPS 140-2, Cert 3389
        AM_CFLAGS="$AM_CFLAGS \
            -DHAVE_FIPS \
            -DHAVE_FIPS_VERSION=$HAVE_FIPS_VERSION \
            -DHAVE_FIPS_VERSION_MINOR=$HAVE_FIPS_VERSION_MINOR \
            -DWOLFSSL_KEY_GEN \
            -DWOLFSSL_SHA224 \
            -DWOLFSSL_AES_DIRECT \
            -DHAVE_AES_ECB \
            -DHAVE_ECC_CDH \
            -DWC_RSA_NO_PADDING \
            -DWOLFSSL_VALIDATE_FFC_IMPORT \
            -DHAVE_FFDHE_Q \
            -DHAVE_PUBLIC_FFDHE"

        ENABLED_KEYGEN="yes"
        ENABLED_SHA224="yes"
        ENABLED_DES3="yes"
        # Shake256 is a SHA-3 algorithm not in our FIPS algorithm list
        ENABLED_SHAKE256=no
        # SHA512-224 and SHA512-256 are SHA-2 algorithms not in our FIPS algorithm list
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_NOSHA512_224 -DWOLFSSL_NOSHA512_256"
        AS_IF([test "x$ENABLED_AESCCM" != "xyes"],
              [ENABLED_AESCCM="yes"
               AM_CFLAGS="$AM_CFLAGS -DHAVE_AESCCM"])
        AS_IF([test "x$ENABLED_RSAPSS" != "xyes"],
              [ENABLED_RSAPSS="yes"
               AM_CFLAGS="$AM_CFLAGS -DWC_RSA_PSS"])
        AS_IF([test "x$ENABLED_ECC" != "xyes"],
              [ENABLED_ECC="yes"
               AM_CFLAGS="$AM_CFLAGS -DHAVE_ECC -DTFM_ECC256 -DWOLFSSL_VALIDATE_ECC_IMPORT"
               AS_IF([test "x$ENABLED_ECC_SHAMIR" = "xyes"],
                     [AM_CFLAGS="$AM_CFLAGS -DECC_SHAMIR"])],
              [AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_VALIDATE_ECC_IMPORT"])
        AS_IF([test "x$ENABLED_AESCTR" != "xyes"],
              [ENABLED_AESCTR="yes"
               AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_AES_COUNTER"])
        AS_IF([test "x$ENABLED_AESCTR" != "xyes"],
              [ENABLED_AESCTR="yes"
               AM_CCASFLAGS="$AM_CCASFLAGS -DWOLFSSL_AES_COUNTER"])
        AS_IF([test "x$ENABLED_CMAC" != "xyes"],
              [ENABLED_CMAC="yes"
               AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_CMAC"])
        AS_IF([test "x$ENABLED_HKDF" != "xyes"],
              [ENABLED_HKDF="yes"
               AM_CFLAGS="$AM_CFLAGS -DHAVE_HKDF"])
        AS_IF([test "x$ENABLED_INTELASM" = "xyes"],
              [AM_CFLAGS="$AM_CFLAGS -DFORCE_FAILURE_RDSEED"])
        AS_IF([test "x$ENABLED_SHA512" = "xno"],
            [ENABLED_SHA512="yes"; AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SHA512 -DWOLFSSL_SHA384"])
        AS_IF([test "x$ENABLED_AESGCM" = "xno"],
            [ENABLED_AESGCM="yes"; AM_CFLAGS="$AM_CFLAGS -DHAVE_AESGCM"])
    ],

    ["rand"],[
        AM_CFLAGS="$AM_CFLAGS -DWOLFCRYPT_FIPS_RAND -DHAVE_FIPS -DHAVE_FIPS_VERSION=$HAVE_FIPS_VERSION -DHAVE_FIPS_VERSION_MINOR=$HAVE_FIPS_VERSION_MINOR"
    ],

    ["v1"],[ # FIPS 140-2, Cert 2425
        AM_CFLAGS="$AM_CFLAGS -DHAVE_FIPS"
        AS_IF([test "x$ENABLED_SHA512" = "xno"],
            [ENABLED_SHA512="yes"; AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SHA512 -DWOLFSSL_SHA384"])
        AS_IF([test "x$ENABLED_AESGCM" = "xno"],
            [ENABLED_AESGCM="yes"; AM_CFLAGS="$AM_CFLAGS -DHAVE_AESGCM"])
        AS_IF([test "x$ENABLED_DES3" = "xno"],[ENABLED_DES3="yes"])
    ])

AS_IF([test "x$ENABLED_FIPS" = "xyes" && test "x$thread_ls_on" = "xno" && test "$ENABLE_LINUXKM" = "no"],
    [AC_MSG_ERROR([FIPS requires Thread Local Storage])])

AS_IF([(test "$ENABLED_NULL_CIPHER" = "yes" || test "$ENABLED_LEANPSK" = "yes") && test "$ENABLED_FIPS" != "no" && test "$FIPS_VERSION" != "dev"],
    [AC_MSG_ERROR([FIPS is incompatible with nullcipher])])

# SELFTEST
AC_ARG_ENABLE([selftest],
    [AS_HELP_STRING([--enable-selftest],[Enable selftest, Will NOT work w/o CAVP selftest license (default: disabled)])],
    [ ENABLED_SELFTEST=$enableval ],
    [ ENABLED_SELFTEST="no" ]
    )

AS_CASE([$ENABLED_SELFTEST],
    ["v2"],[
        # selftest v2 (wolfCrypt 4.1.0)
        ENABLED_SELFTEST="yes"
        SELFTEST_VERSION="v2"
    ],
    ["no"],[SELFTEST_VERSION="none"],
    [
        # selftest v1 (wolfCrypt 3.14.2)
        ENABLED_SELFTEST="yes"
        SELFTEST_VERSION="v1"
    ])

AS_CASE([$SELFTEST_VERSION],
    ["v2"],[
        AM_CFLAGS="$AM_CFLAGS -DHAVE_SELFTEST -DHAVE_SELFTEST_VERSION=2 -DHAVE_PUBLIC_FFDHE"
    ],
    ["v1"],[
        AM_CFLAGS="$AM_CFLAGS -DHAVE_SELFTEST -DHAVE_PUBLIC_FFDHE"
    ])


# Set SHA-3 flags
if test "$ENABLED_SHA3" != "no" && test "$ENABLED_32BIT" = "no"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SHA3"
fi

# Set SHAKE128 flags
# FIPS does not support SHAKE 128
AS_IF([test "x$ENABLED_FIPS" = "xyes"],[ENABLED_SHAKE128="no"])

if test "$ENABLED_SHAKE128" != "no" && test "$ENABLED_32BIT" = "no"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SHAKE128"
    if test "$ENABLED_SHA3" = "no"
    then
        AC_MSG_ERROR([Must have SHA-3 enabled: --enable-sha3])
    fi
else
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_NO_SHAKE128"
fi

# Set SHAKE256 flags
# FIPS does not support SHAKE 256
AS_IF([test "x$ENABLED_FIPS" = "xyes"],[ENABLED_SHAKE256="no"])

if test "$ENABLED_SHAKE256" != "no" && test "$ENABLED_32BIT" = "no"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SHAKE256"
    if test "$ENABLED_SHA3" = "no"
    then
        AC_MSG_ERROR([Must have SHA-3 enabled: --enable-sha3])
    fi
else
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_NO_SHAKE256"
fi

# set POLY1305 default
POLY1305_DEFAULT=yes

if test "x$ENABLED_FIPS" = "xyes"
then
POLY1305_DEFAULT=no
fi

# POLY1305
AC_ARG_ENABLE([poly1305],
    [AS_HELP_STRING([--enable-poly1305],[Enable wolfSSL POLY1305 support (default: enabled)])],
    [ ENABLED_POLY1305=$enableval ],
    [ ENABLED_POLY1305=$POLY1305_DEFAULT]
    )

# leanpsk and leantls don't need poly1305
if test "$ENABLED_LEANPSK" = "yes" || test "$ENABLED_LEANTLS" = "yes"
then
    ENABLED_POLY1305=no
fi

if test "$ENABLED_POLY1305" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_POLY1305"
fi


# set CHACHA default
CHACHA_DEFAULT=yes

if test "x$ENABLED_FIPS" = "xyes"
then
CHACHA_DEFAULT=no
fi

# CHACHA
AC_ARG_ENABLE([chacha],
    [AS_HELP_STRING([--enable-chacha],[Enable CHACHA (default: enabled). Use `=noasm` to disable ASM AVX/AVX2 speedups])],
    [ ENABLED_CHACHA=$enableval ],
    [ ENABLED_CHACHA=$CHACHA_DEFAULT]
    )

# leanpsk and leantls don't need chacha
if test "$ENABLED_LEANPSK" = "yes" || test "$ENABLED_LEANTLS" = "yes"
then
    ENABLED_CHACHA=no
fi

if test "$ENABLED_CHACHA" = "noasm" || test "$ENABLED_ASM" = "no"
then
    AM_CFLAGS="$AM_CFLAGS -DNO_CHACHA_ASM"
fi

if test "$ENABLED_CHACHA" != "no"
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_CHACHA"
fi


# XCHACHA
AC_ARG_ENABLE([xchacha],
    [AS_HELP_STRING([--enable-xchacha],[Enable XCHACHA (default: disabled).])],
    [ ENABLED_XCHACHA=$enableval ],
    [ ENABLED_XCHACHA=no]
    )

if test "$ENABLED_XCHACHA" = "yes"
then
    if test "$ENABLED_CHACHA" = "no"
    then
        AC_MSG_ERROR([XChaCha (--enable-xchacha) depends on ChaCha (--enable-chacha)])
    fi
    AM_CFLAGS="$AM_CFLAGS -DHAVE_XCHACHA"
fi


# Hash DRBG
AC_ARG_ENABLE([hashdrbg],
    [AS_HELP_STRING([--enable-hashdrbg],[Enable Hash DRBG support (default: enabled)])],
    [ ENABLED_HASHDRBG=$enableval ],
    [ ENABLED_HASHDRBG=yes ]
    )

if test "x$ENABLED_HASHDRBG" = "xyes"
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_HASHDRBG"
else
    # turn on Hash DRBG if FIPS is on (don't force on for KCAPI)
    if test "x$ENABLED_FIPS" = "xyes" && test "x$ENABLED_KCAPI" = "xno"
    then
        AM_CFLAGS="$AM_CFLAGS -DHAVE_HASHDRBG"
        ENABLED_HASHDRBG=yes
    else
        AM_CFLAGS="$AM_CFLAGS -DWC_NO_HASHDRBG"
    fi
fi


# MemUse Entropy
AC_ARG_ENABLE([entropy-memuse],
    [AS_HELP_STRING([--enable-entropy-memuse],[Enable memuse entropy support (default: disabled)])],
    [ ENABLED_ENTROPY_MEMUSE=$enableval ],
    [ ENABLED_ENTROPY_MEMUSE=no ]
    )

if test "x$ENABLED_ENTROPY_MEMUSE" != "xno"
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_ENTROPY_MEMUSE"
    enable_sha3=yes

    for v in `echo $ENABLED_ENTROPY_MEMUSE | tr "," " "`
    do
      case $v in
      yes)
        ;;
      thread)
        AM_CFLAGS="$AM_CFLAGS -DENTROPY_MEMUSE_THREAD"
        ;;
      nofallback)
        AM_CFLAGS="$AM_CFLAGS -DENTROPY_MEMUSE_FORCE_FAILURE"
        ;;
      *)
        AC_MSG_ERROR([Invalid MemUse Entropy option. Valid are: thread, nofallback. Seen: $ENABLED_ENTROPY_MEMUSE.])
        break;;
      esac
    done
fi


# Filesystem Build
if test "$ENABLED_LINUXKM" = "yes"
then
    ENABLED_FILESYSTEM_DEFAULT=no
else
    ENABLED_FILESYSTEM_DEFAULT=yes
fi
AC_ARG_ENABLE([filesystem],
    [AS_HELP_STRING([--enable-filesystem],[Enable Filesystem support (default: enabled)])],
    [ ENABLED_FILESYSTEM=$enableval ],
    [ ENABLED_FILESYSTEM=$ENABLED_FILESYSTEM_DEFAULT ]
    )

if test "$ENABLED_FILESYSTEM" = "no"
then
    AM_CFLAGS="$AM_CFLAGS -DNO_FILESYSTEM"
else
    # turn off filesystem if leanpsk on
    if test "$ENABLED_LEANPSK" = "yes" || test "$ENABLED_LINUXKM" = "yes"
    then
        AM_CFLAGS="$AM_CFLAGS -DNO_FILESYSTEM"
        ENABLED_FILESYSTEM=no
    fi
fi


# inline Build
AC_ARG_ENABLE([inline],
    [AS_HELP_STRING([--enable-inline],[Enable inline functions (default: enabled)])],
    [ ENABLED_INLINE=$enableval ],
    [ ENABLED_INLINE=yes ]
    )

if test "$ENABLED_INLINE" = "no"
then
    AM_CFLAGS="$AM_CFLAGS -DNO_INLINE"
fi


# OCSP
if test "x$ENABLED_OPENSSLALL" = "xyes" || test "x$ENABLED_NGINX" = "xyes" || test "x$ENABLED_LIGHTY" = "xyes"
then
    test "$enable_ocsp" = "" && enable_ocsp=yes
fi

AC_ARG_ENABLE([ocsp],
    [AS_HELP_STRING([--enable-ocsp],[Enable OCSP (default: disabled)])],
    [ ENABLED_OCSP=$enableval ],
    [ ENABLED_OCSP=no ]
    )

if test "$ENABLED_OCSP" = "yes"
then
    # check openssl command tool for testing ocsp
    AC_CHECK_PROG([HAVE_OPENSSL_CMD],[openssl],[yes],[no])

    if test "$HAVE_OPENSSL_CMD" = "yes"
    then
        AM_CFLAGS="$AM_CFLAGS -DHAVE_OPENSSL_CMD"
    else
        AC_MSG_WARN([openssl command line tool not available for testing ocsp])
    fi
fi


# Certificate Status Request : a.k.a. OCSP Stapling
AC_ARG_ENABLE([ocspstapling],
    [AS_HELP_STRING([--enable-ocspstapling],[Enable OCSP Stapling (default: disabled)])],
    [ ENABLED_CERTIFICATE_STATUS_REQUEST=$enableval ],
    [ ENABLED_CERTIFICATE_STATUS_REQUEST=no ]
    )

if test "x$ENABLED_NGINX" = "xyes" || test "x$ENABLED_WPAS" = "xyes" || test "x$ENABLED_HAPROXY" = "xyes" || test "x$ENABLED_LIGHTY" = "xyes"
then
    ENABLED_CERTIFICATE_STATUS_REQUEST="yes"
fi

if test "x$ENABLED_CERTIFICATE_STATUS_REQUEST" = "xyes"
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_TLS_EXTENSIONS -DHAVE_CERTIFICATE_STATUS_REQUEST"

    # Requires OCSP make sure on
    if test "x$ENABLED_OCSP" = "xno"
    then
        ENABLED_OCSP="yes"
    fi
fi


# Certificate Status Request v2 : a.k.a. OCSP stapling v2
AC_ARG_ENABLE([ocspstapling2],
    [AS_HELP_STRING([--enable-ocspstapling2],[Enable OCSP Stapling v2 (default: disabled)])],
    [ ENABLED_CERTIFICATE_STATUS_REQUEST_V2=$enableval ],
    [ ENABLED_CERTIFICATE_STATUS_REQUEST_V2=no ]
    )

if test "x$ENABLED_NGINX" = "xyes" || test "x$ENABLED_WPAS" = "xyes" || test "x$ENABLED_HAPROXY" = "xyes" || test "x$ENABLED_LIGHTY" = "xyes"
then
    ENABLED_CERTIFICATE_STATUS_REQUEST_V2=yes
fi

if test "x$ENABLED_CERTIFICATE_STATUS_REQUEST_V2" = "xyes"
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_TLS_EXTENSIONS -DHAVE_CERTIFICATE_STATUS_REQUEST_V2"

    # Requires OCSP make sure on
    if test "x$ENABLED_OCSP" = "xno"
    then
        ENABLED_OCSP="yes"
    fi
fi


# CRL
AC_ARG_ENABLE([crl],
    [AS_HELP_STRING([--enable-crl],[Enable CRL (Use =io for inline CRL HTTP GET) (default: disabled)])],
    [ ENABLED_CRL=$enableval ],
    [ ENABLED_CRL=no ]
    )

if test "x$ENABLED_NGINX" = "xyes" || test "x$ENABLED_HAPROXY" = "xyes" || test "x$ENABLED_OPENVPN" = "xyes" || \
   test "x$ENABLED_WPAS" != "xno" || test "x$ENABLED_LIGHTY" = "xyes" || test "x$ENABLED_NETSNMP" = "xyes" || \
   test "x$ENABLED_KRB" = "xyes" || test "x$ENABLED_STRONGSWAN" = "xyes"
then
    ENABLED_CRL=yes
fi

if test "$ENABLED_CRL" != "no"
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_CRL"
fi
if test "$ENABLED_CRL" = "io"
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_CRL_IO"
fi


# CRL Monitor
AC_ARG_ENABLE([crl-monitor],
    [AS_HELP_STRING([--enable-crl-monitor],[Enable CRL Monitor (default: disabled)])],
    [ ENABLED_CRL_MONITOR=$enableval ],
    [ ENABLED_CRL_MONITOR=no ]
    )

if test "$ENABLED_CRL_MONITOR" = "yes"
then
    case $host_os in
    *linux* | *darwin* | *freebsd*)
        if test "x$ENABLED_SINGLETHREADED" = "xno"; then
            AM_CFLAGS="$AM_CFLAGS -DHAVE_CRL_MONITOR"
        else
            ENABLED_CRL_MONITOR="no"
            AC_MSG_ERROR([crl monitor requires threading / pthread])
        fi
        ;;
    *)
        if test "x$ENABLED_DISTRO" = "xyes" ; then
            ENABLED_CRL_MONITOR="no"
        else
            AC_MSG_ERROR( [crl monitor only allowed on linux, OS X, or freebsd])
        fi
        break;;
    esac
fi


# USER CRYPTO
ENABLED_USER_CRYPTO="no"
ENABLED_USER_RSA="no"
AC_DEFINE([BUILD_USER_RSA], [], [User RSA is being defined])
trycryptodir=""
AC_ARG_WITH([user-crypto],
    [AS_HELP_STRING([--with-user-crypto=PATH],[Path to USER_CRYPTO install (default /usr/local)])],
    [
        CPPFLAGS="$CPPFLAGS -DHAVE_USER_CRYPTO"
        LIBS="$LIBS -lusercrypto"

        if test "x$withval" != "xno" ; then
            trycryptodir=$withval
        fi
        if test "x$withval" = "xyes" ; then
            trycryptodir="/usr/local"
        fi

        LDFLAGS="$LDFLAGS -L$trycryptodir/lib"
        CPPFLAGS="$CPPFLAGS -I$trycryptodir/include"

        #Look for RSA Init function in usercrypto lib
        AC_CHECK_LIB([usercrypto], [wc_InitRsaKey], [user_rsa_linked=yes], [user_rsa_linked=no])

       if test "x$user_rsa_linked" = "xyes" ; then
            AC_MSG_NOTICE([User user_rsa.h being used])
            AM_CFLAGS="$AM_CFLAGS -DHAVE_USER_RSA"
            ENABLED_USER_RSA=yes
            ENABLED_USER_CRYPTO=yes
       fi


        #Display check and find result of link attempts
        AC_MSG_CHECKING([for USER_CRYPTO])
        if test "x$ENABLED_USER_CRYPTO" = "xno" ; then
            AC_MSG_RESULT([no])
            AC_MSG_ERROR([USER_CRYPTO not found. Either move to /usr/include and /usr/lib or
                    Specify its path using --with-user-crypto=/dir/])
        else
            AC_MSG_RESULT([yes])
            # Check if .la is available if not then rely on exported path
            if test -e $trycryptodir/lib/libusercrypto.la
            then
                LIB_ADD="$trycryptodir/lib/libusercrypto.la $LIB_ADD"
            else
                LIB_ADD="-lusercrypto $LIB_ADD"
            fi
            AM_LDFLAGS="$AM_LDFLAGS -L$trycryptodir/lib"
            AM_CFLAGS="$AM_CFLAGS -DHAVE_USER_CRYPTO"
        fi
    ]
)

if test "$ENABLED_USER_CRYPTO" = "yes" && test "$ENABLED_FIPS" = "yes"
then
    AC_MSG_ERROR([cannot enable user crypto and fips, user crypto possibility of using code in fips boundary.])
fi

# Whitewood netRandom client library
ENABLED_WNR="no"
trywnrdir=""
AC_ARG_WITH([wnr],
    [AS_HELP_STRING([--with-wnr=PATH],[Path to Whitewood netRandom install (default /usr/local)])],
    [
        AC_MSG_CHECKING([for Whitewood netRandom])
        LIBS="$LIBS -lwnr"

        AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <wnr.h>]], [[ wnr_setup(0, 0); ]])], [ wnr_linked=yes ],[ wnr_linked=no ])

        if test "x$wnr_linked" = "xno" ; then
            if test "x$withval" != "xno" ; then
                trywnrdir=$withval
            fi
            if test "x$withval" = "xyes" ; then
                trywnrdir="/usr/local"
            fi

            CPPFLAGS="$AM_CPPFLAGS -DHAVE_WNR -I$trywnrdir/include"
            LDFLAGS="$AM_LDFLAGS $LDFLAGS -L$trywnrdir/lib"

            AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <wnr.h>]], [[ wnr_setup(0, 0); ]])], [ wnr_linked=yes ],[ wnr_linked=no ])

            if test "x$wnr_linked" = "xno" ; then
                AC_MSG_ERROR([Whitewood netRandom isn't found.
                If it's already installed, specify its path using --with-wnr=/dir/])
            fi
            AC_MSG_RESULT([yes])
            AM_CPPFLAGS="$CPPFLAGS"
            AM_LDFLAGS="$AM_LDFLAGS -L$trywnrdir/lib"
        else
            AC_MSG_RESULT([yes])
        fi

        AM_CFLAGS="$AM_CFLAGS -DHAVE_WNR"
        ENABLED_WNR="yes"
    ]
)


# SNI
# enable SNI automatically for x86_64/x86/aarch64/amd64
SNI_DEFAULT=no
if test "$host_cpu" = "x86_64" || test "$host_cpu" = "x86" || test "$host_cpu" = "aarch64" || test "$host_cpu" = "amd64"
then
    SNI_DEFAULT=yes
fi
AC_ARG_ENABLE([sni],
    [AS_HELP_STRING([--enable-sni],[Enable SNI (default: enabled on x86_64/x86/aarch64/amd64)])],
    [ ENABLED_SNI=$enableval ],
    [ ENABLED_SNI=$SNI_DEFAULT ]
    )
if test "x$ENABLED_QT" = "xyes" || test "$ENABLED_QUIC" = "yes"
then
    ENABLED_SNI="yes"
fi

if test "x$ENABLED_SNI" = "xyes"
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_TLS_EXTENSIONS -DHAVE_SNI"
fi

# Maximum Fragment Length
AC_ARG_ENABLE([maxfragment],
    [AS_HELP_STRING([--enable-maxfragment],[Enable Maximum Fragment Length (default: disabled)])],
    [ ENABLED_MAX_FRAGMENT=$enableval ],
    [ ENABLED_MAX_FRAGMENT=no ]
    )

# ALPN
AC_ARG_ENABLE([alpn],
    [AS_HELP_STRING([--enable-alpn],[Enable ALPN (default: disabled)])],
    [ ENABLED_ALPN=$enableval ],
    [ ENABLED_ALPN=no ]
    )

if test "$ENABLED_BIND" = "yes"
then
    ENABLED_ALPN=yes
fi

if test "$ENABLED_QUIC" = "yes"
then
    ENABLED_ALPN=yes
fi

if test "x$ENABLED_ALPN" = "xyes"
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_TLS_EXTENSIONS -DHAVE_ALPN"
fi

# Maximum Fragment Length
if test "x$ENABLED_MAX_FRAGMENT" = "xyes"
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_TLS_EXTENSIONS -DHAVE_MAX_FRAGMENT"
fi

# Trusted CA Indication Extension
AC_ARG_ENABLE([trustedca],
    [AS_HELP_STRING([--enable-trustedca],[Enable Trusted CA Indication (default: disabled)])],
    [ ENABLED_TRUSTED_CA=$enableval ],[ ENABLED_TRUSTED_CA=no ])

AS_IF([test "x$ENABLED_TRUSTED_CA" = "xyes"],
      [AM_CFLAGS="$AM_CFLAGS -DHAVE_TLS_EXTENSIONS -DHAVE_TRUSTED_CA"])

# Truncated HMAC
AC_ARG_ENABLE([truncatedhmac],
    [AS_HELP_STRING([--enable-truncatedhmac],[Enable Truncated HMAC (default: disabled)])],
    [ ENABLED_TRUNCATED_HMAC=$enableval ],
    [ ENABLED_TRUNCATED_HMAC=no ]
    )

if test "x$ENABLED_TRUNCATED_HMAC" = "xyes"
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_TLS_EXTENSIONS -DHAVE_TRUNCATED_HMAC"
fi

# Renegotiation Indication - (FAKE Secure Renegotiation)
# Client will send TLS_EMPTY_RENEGOTIATION_INFO_SCSV, not supported
# with enabling secure renegotiation
AC_ARG_ENABLE([renegotiation-indication],
    [AS_HELP_STRING([--enable-renegotiation-indication],[Enable Renegotiation Indication for client via empty cipher  (default: disabled)])],
    [ ENABLED_RENEGOTIATION_INDICATION=$enableval ],
    [ ENABLED_RENEGOTIATION_INDICATION=no ]
    )

if test "x$ENABLED_RENEGOTIATION_INDICATION" = "xyes"
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_RENEGOTIATION_INDICATION"
fi

# Secure Renegotiation
AC_ARG_ENABLE([secure-renegotiation],
    [AS_HELP_STRING([--enable-secure-renegotiation],[Enable Secure Renegotiation (default: disabled)])],
    [ ENABLED_SECURE_RENEGOTIATION=$enableval ],
    [ ENABLED_SECURE_RENEGOTIATION=no ]
    )

if test "x$ENABLED_HAPROXY" = "xyes"
then
    ENABLED_SECURE_RENEGOTIATION=yes
fi

if test "x$ENABLED_SECURE_RENEGOTIATION" = "xyes"
then
    if test "x$ENABLED_RENEGOTIATION_INDICATION" = "xyes"
    then
        AC_MSG_ERROR([cannot enable renegotiation-indication and secure-renegotiation.])
    fi
    AM_CFLAGS="$AM_CFLAGS -DHAVE_TLS_EXTENSIONS -DHAVE_SECURE_RENEGOTIATION -DHAVE_SERVER_RENEGOTIATION_INFO"
fi

# Secure Renegotiation Info
AC_ARG_ENABLE([secure-renegotiation-info],
    [AS_HELP_STRING([--enable-secure-renegotiation-info],[Enable Secure Renegotiation info extension (default: enabled)])],
    [ ENABLED_SECURE_RENEGOTIATION_INFO=$enableval ],
    [ ENABLED_SECURE_RENEGOTIATION_INFO=yes ]
    )


# Fallback SCSV
AC_ARG_ENABLE([fallback-scsv],
    [AS_HELP_STRING([--enable-fallback-scsv],[Enable Fallback SCSV (default: disabled)])],
    [ ENABLED_FALLBACK_SCSV=$enableval ],
    [ ENABLED_FALLBACK_SCSV=no ]
    )

if test "x$ENABLED_FALLBACK_SCSV" = "xyes"
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_FALLBACK_SCSV"
fi

# Exporting Keying Material
AC_ARG_ENABLE([keying-material],
    [AS_HELP_STRING([--enable-keying-material],[Enable Keying Material Exporters (default: disabled)])],
    [ ENABLED_KEYING_MATERIAL=$enableval ],
    [ ENABLED_KEYING_MATERIAL=no ]
    )

if test "$ENABLED_CHRONY" = "yes" || test "$ENABLED_SRTP" = "yes"
then
    ENABLED_KEYING_MATERIAL=yes
fi
if test "x$ENABLED_KEYING_MATERIAL" = "xyes"
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_KEYING_MATERIAL"
fi

# Supported Elliptic Curves Extensions
AC_ARG_ENABLE([supportedcurves],
    [AS_HELP_STRING([--enable-supportedcurves],[Enable Supported Elliptic Curves (default: enabled)])],
    [ENABLED_SUPPORTED_CURVES=$enableval],
    [ENABLED_SUPPORTED_CURVES=yes])

if test "x$ENABLED_SUPPORTED_CURVES" = "xyes"
then
    AS_IF([test "x$ENABLED_ECC" = "xno" && test "x$ENABLED_CURVE25519" = "xno" && test "x$ENABLED_CURVE448" = "xno"],
          [ENABLED_SUPPORTED_CURVES=no],
          [AM_CFLAGS="$AM_CFLAGS -DHAVE_TLS_EXTENSIONS -DHAVE_SUPPORTED_CURVES"])
fi

# Diffie-Hellman
if test "$ENABLED_DH" != "no"
then
    if test "$ENABLED_TLS13" = "yes" || test "$ENABLED_SUPPORTED_CURVES" = "yes"
    then
        AM_CFLAGS="$AM_CFLAGS -DHAVE_FFDHE_2048"
    fi
fi

# FFDHE parameters only
AC_ARG_ENABLE([ffdhe-only],
    [AS_HELP_STRING([--enable-ffdhe-only],[Enable using only FFDHE in client (default: disabled)])],
    [ ENABLED_FFDHE_ONLY=$enableval ],
    [ ENABLED_FFDHE_ONLY=no ]
    )

if test "x$ENABLED_FFDHE_ONLY" = "xyes"
then
    if test "$ENABLED_DH" = "no"
    then
        AC_MSG_ERROR([FFDHE only support requires DH support])
    fi
    if test "$ENABLED_SUPPORTED_CURVES" = "no"
    then
        AC_MSG_ERROR([FFDHE only support requires Supported Curves extension])
    fi
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_REQUIRE_FFDHE"
fi

# TLS 1.3 Requires either ECC or (RSA/DH), or CURVE25519/ED25519 or CURVE448/ED448 or libOQS
if test "x$ENABLED_PSK" = "xno" && test "x$ENABLED_ECC" = "xno" && \
    (test "x$ENABLED_RSA" = "xno" || test "x$ENABLED_DH" = "xno") && \
    (test "x$ENABLED_CURVE25519" = "xno" || test "x$ENABLED_ED25519" = "xno") && \
    (test "x$ENABLED_CURVE448" = "xno" || test "x$ENABLED_ED448" = "xno") && \
    test "x$ENABLED_LIBOQS" = "xno"
then
    # disable TLS 1.3
    ENABLED_TLS13=no
fi
if test "$ENABLED_TLS13" = "yes" && (test "x$ENABLED_ECC" = "xyes" || \
    test "$ENABLED_DH" != "no")
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_SUPPORTED_CURVES"
fi
if test "$ENABLED_TLS13" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_TLS13 -DHAVE_TLS_EXTENSIONS"
fi


# Session Ticket Extension
AC_ARG_ENABLE([session-ticket],
    [AS_HELP_STRING([--enable-session-ticket],[Enable Session Ticket (default: disabled)])],
    [ ENABLED_SESSION_TICKET=$enableval ],
    [ ENABLED_SESSION_TICKET=no ]
    )

if test "x$ENABLED_NGINX" = "xyes" || test "$ENABLED_WPAS" = "yes" || test "x$ENABLED_HAPROXY" = "xyes" || test "x$ENABLED_LIGHTY" = "xyes"
then
    ENABLED_SESSION_TICKET=yes
fi

if test "x$ENABLED_SESSION_TICKET" = "xyes"
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_TLS_EXTENSIONS -DHAVE_SESSION_TICKET"
fi

AC_ARG_ENABLE([ticket-nonce-malloc],
    [AS_HELP_STRING([--enable-ticket-nonce-malloc], [Enable dynamic allocation of ticket nonces (default: disabled)])],
    [ ENABLED_TICKET_NONCE_MALLOC=$enableval ],
    [ ENABLED_TICKET_NONCE_MALLOC=no ]
    )

if test "$ENABLED_TICKET_NONCE_MALLOC" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_TICKET_NONCE_MALLOC"
fi

# Extended Master Secret Extension
AC_ARG_ENABLE([extended-master],
    [AS_HELP_STRING([--enable-extended-master],[Enable Extended Master Secret (default: enabled)])],
    [ ENABLED_EXTENDED_MASTER=$enableval ],
    [ ENABLED_EXTENDED_MASTER=yes ]
    )

if test "$ENABLED_CRYPTONLY" = "yes"
then
    ENABLED_EXTENDED_MASTER=no
fi
if test "x$ENABLED_EXTENDED_MASTER" = "xyes"
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_EXTENDED_MASTER"
fi

# TLS Extensions
AC_ARG_ENABLE([tlsx],
    [AS_HELP_STRING([--enable-tlsx],[Enable all TLS Extensions (default: disabled)])],
    [ ENABLED_TLSX=$enableval ],
    [ ENABLED_TLSX=no ]
    )

if test "x$ENABLED_NGINX" = "xyes" || test "x$ENABLED_HAPROXY" = "xyes" || test "x$ENABLED_SIGNAL" = "xyes" || test "x$ENABLED_LIGHTY" = "xyes" || test "$ENABLED_CHRONY" = "yes"
then
    ENABLED_TLSX=yes
fi

if test "x$ENABLED_TLSX" = "xyes"
then
    ENABLED_SNI=yes
    ENABLED_MAX_FRAGMENT=yes
    ENABLED_TRUNCATED_HMAC=yes
    ENABLED_ALPN=yes
    ENABLED_TRUSTED_CA=yes
    ENABLED_ENCRYPT_THEN_MAC=yes
    AM_CFLAGS="$AM_CFLAGS -DHAVE_TLS_EXTENSIONS -DHAVE_SNI -DHAVE_MAX_FRAGMENT -DHAVE_TRUNCATED_HMAC -DHAVE_ALPN -DHAVE_TRUSTED_CA"
    # Check the ECC supported curves prereq
    AS_IF([test "x$ENABLED_ECC" != "xno" || test "x$ENABLED_CURVE25519" = "xyes" || test "x$ENABLED_CURVE448" = "xyes" || test "x$ENABLED_TLS13" = "xyes"],
          [ENABLED_SUPPORTED_CURVES=yes
           AM_CFLAGS="$AM_CFLAGS -DHAVE_SUPPORTED_CURVES"])
fi

# Early Data handshake in TLS v1.3 and above
AC_ARG_ENABLE([earlydata],
    [AS_HELP_STRING([--enable-earlydata],[Enable Early Data handshake with wolfSSL TLS v1.3 (default: disabled)])],
    [ ENABLED_TLS13_EARLY_DATA=$enableval ],
    [ ENABLED_TLS13_EARLY_DATA=no ]
    )

if test "$ENABLED_TLS13_EARLY_DATA" = "group"
then
    ENABLED_TLS13_EARLY_DATA="yes"
    # Group EarlyData with ClientHello
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_EARLY_DATA_GROUP"
fi
if test "$ENABLED_TLS13_EARLY_DATA" = "yes"
then
    if test "x$ENABLED_TLS13" = "xno" && test "x$ENABLED_ALL" = "xno"
    then
        AC_MSG_ERROR([cannot enable earlydata without enabling tls13.])
    fi
    if test "x$ENABLED_SESSION_TICKET" = "xno" && test "x$ENABLED_PSK" = "xno"
    then
        AC_MSG_ERROR([cannot enable earlydata without enabling session tickets and/or PSK.])
    fi
    if test "x$ENABLED_TLS13" = "xyes"
    then
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_EARLY_DATA"
    fi
fi

if test "$ENABLED_TLSV12" = "no" && test "$ENABLED_TLS13" = "yes" && test "x$ENABLED_SESSION_TICKET" = "xno"
then
    AM_CFLAGS="$AM_CFLAGS -DNO_SESSION_CACHE"
fi

# PKCS7
AC_ARG_ENABLE([pkcs7],
    [AS_HELP_STRING([--enable-pkcs7],[Enable PKCS7 (default: disabled)])],
    [ ENABLED_PKCS7=$enableval ],
    [ ENABLED_PKCS7=no ]
    )

if test "x$ENABLED_WPAS_DPP" = "xyes"
then
    ENABLED_PKCS7=yes
fi

# wolfSSH Options
AC_ARG_ENABLE([wolfssh],
    [AS_HELP_STRING([--enable-wolfssh],[Enable wolfSSH options (default: disabled)])],
    [ ENABLED_WOLFSSH=$enableval ],
    [ ENABLED_WOLFSSH=no ]
    )

AC_ARG_ENABLE([ssh],
    [AS_HELP_STRING([--enable-ssh],[Enable wolfSSH options (default: disabled)])],
    [ ENABLED_SSH=$enableval ],
    [ ENABLED_SSH=no ]
    )

if test "x$ENABLED_SSH" = "xyes"
then
    ENABLED_WOLFSSH="yes"
fi

# wolfTPM Options
AC_ARG_ENABLE([wolftpm],
    [AS_HELP_STRING([--enable-wolftpm],[Enable wolfTPM options (default: disabled)])],
    [ ENABLED_WOLFTPM=$enableval ],
    [ ENABLED_WOLFTPM=no ]
    )

# wolfCLU Options
AC_ARG_ENABLE([wolfclu],
    [AS_HELP_STRING([--enable-wolfclu],[Enable wolfCLU options (default: disabled)])],
    [ ENABLED_WOLFCLU=$enableval ],
    [ ENABLED_WOLFCLU=no ]
    )


if test "x$ENABLED_WOLFTPM" = "xyes"
then
    # Requires cryptocb (set in its enable section)
    # Requires certgen, certreq, certext
    if test "x$ENABLED_CERTGEN" = "xno"
    then
        ENABLED_CERTGEN="yes"
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_CERT_GEN"
    fi
    if test "x$ENABLED_CERTREQ" = "xno"
    then
        ENABLED_CERTREQ="yes"
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_CERT_REQ"
    fi
    if test "x$ENABLED_CERTEXT" = "xno"
    then
        ENABLED_CERTEXT="yes"
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_CERT_EXT"
    fi

    # Requires PKCS7
    if test "x$ENABLED_PKCS7" = "xno"
    then
        ENABLED_PKCS7="yes"
    fi

    # Requires aescfb
    if test "x$ENABLED_AESCFB" = "xno"
    then
        ENABLED_AESCFB="yes"
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_AES_CFB"
    fi

    # Requires public mp_
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_PUBLIC_MP"
fi

if test "x$ENABLED_SMIME" = "xyes"
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_SMIME"
    # Requires PKCS7
    if test "x$ENABLED_PKCS7" = "xno"
    then
        ENABLED_PKCS7="yes"
    fi
fi

# Simple Certificate Enrollment Protocol (SCEP)
AC_ARG_ENABLE([scep],
    [AS_HELP_STRING([--enable-scep],[Enable wolfSCEP (default: disabled)])],
    [ ENABLED_WOLFSCEP=$enableval ],
    [ ENABLED_WOLFSCEP=no ]
    )


# Secure Remote Password
AC_ARG_ENABLE([srp],
    [AS_HELP_STRING([--enable-srp],[Enable Secure Remote Password (default: disabled)])],
    [ ENABLED_SRP=$enableval ],
    [ ENABLED_SRP=no ]
    )

if test "x$ENABLED_SRP" = "xyes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFCRYPT_HAVE_SRP"
fi


# Indefinite length encoded BER message support
AC_ARG_ENABLE([indef],
    [AS_HELP_STRING([--enable-indef],[Enable parsing of indefinite length encoded msgs (default: disabled)])],
    [ ENABLED_BER_INDEF=$enableval ],
    [ ENABLED_BER_INDEF=no ]
    )

if test "x$ENABLED_BER_INDEF" = "xyes"
then
    AM_CFLAGS="$AM_CFLAGS -DASN_BER_TO_DER"
fi

# Alternate certification chains, as opposed to requiring full chain validation.
# Certificate validation behavior is relaxed, similar to openssl and
# browsers. Only the peer certificate must validate to a trusted
# certificate. Without this, all certificates sent by a peer must be
# used in the trust chain or the connection will be rejected.
AC_ARG_ENABLE([altcertchains],
    [AS_HELP_STRING([--enable-altcertchains],[Enable using alternative certificate chains, only require leaf certificate to validate to trust root (default: disabled)])],
    [ ENABLED_ALT_CERT_CHAINS=$enableval ],
    [ ENABLED_ALT_CERT_CHAINS=no ]
    )

if test "x$ENABLED_ALT_CERT_CHAINS" = "xyes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_ALT_CERT_CHAINS"
fi

# Small Stack - Cache on object
AC_ARG_ENABLE([smallstackcache],
    [AS_HELP_STRING([--enable-smallstackcache],[Enable Small Stack Usage Caching (default: disabled)])],
    [ ENABLED_SMALL_STACK_CACHE=$enableval ],
    [ ENABLED_SMALL_STACK_CACHE=no ]
    )

if test "x$ENABLED_SMALL_STACK_CACHE" = "xyes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SMALL_STACK_CACHE"
fi

# Small Stack
if test "$ENABLED_LINUXKM_DEFAULTS" = "yes"
then
    ENABLED_SMALL_STACK_DEFAULT=yes
else
    ENABLED_SMALL_STACK_DEFAULT=no
fi
AC_ARG_ENABLE([smallstack],
    [AS_HELP_STRING([--enable-smallstack],[Enable Small Stack Usage (default: disabled)])],
    [ ENABLED_SMALL_STACK=$enableval ],
    [ ENABLED_SMALL_STACK=$ENABLED_SMALL_STACK_DEFAULT ]
    )

if test "x$ENABLED_SMALL_STACK_CACHE" = "xyes"
then
    ENABLED_SMALL_STACK=yes
fi
if test "x$ENABLED_SMALL_STACK" = "xyes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SMALL_STACK"
fi


#valgrind
AC_ARG_ENABLE([valgrind],
    [AS_HELP_STRING([--enable-valgrind],[Enable valgrind for unit tests (default: disabled)])],
    [ ENABLED_VALGRIND=$enableval ],
    [ ENABLED_VALGRIND=no ]
    )

if test "$ENABLED_VALGRIND" = "yes"
then
    AC_CHECK_PROG([HAVE_VALGRIND],[valgrind],[yes],[no])

    if test "$HAVE_VALGRIND" = "no"
    then
        AC_MSG_ERROR([Valgrind not found.])
    fi
    enable_shared=no
    enable_static=yes
    AM_CFLAGS="$AM_CFLAGS -DHAVE_VALGRIND"
fi


# Test certs, use internal cert functions for extra testing
AC_ARG_ENABLE([testcert],
    [AS_HELP_STRING([--enable-testcert],[Enable Test Cert (default: disabled)])],
    [ ENABLED_TESTCERT=$enableval ],
    [ ENABLED_TESTCERT=no ]
    )

if test "$ENABLED_TESTCERT" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_TEST_CERT"
fi


# I/O Pool, an example to show user how to override memory handler and use
# a pool for the input/output buffer requests
AC_ARG_ENABLE([iopool],
    [AS_HELP_STRING([--enable-iopool],[Enable I/O Pool example (default: disabled)])],
    [ ENABLED_IOPOOL=$enableval ],
    [ ENABLED_IOPOOL=no ]
    )

if test "$ENABLED_IOPOOL" = "yes"
then
    if test "$thread_ls_on" = "no"
    then
        AC_MSG_ERROR([I/O Pool example requires Thread Local Storage])
    fi
    AM_CFLAGS="$AM_CFLAGS -DHAVE_IO_POOL -DXMALLOC_USER"
fi


# Certificate Service Support
AC_ARG_ENABLE([certservice],
    [AS_HELP_STRING([--enable-certservice],[Enable cert service (default: disabled)])],
    [ ENABLED_CERT_SERVICE=$enableval ],
    [ ENABLED_CERT_SERVICE=no ]
    )
if test "$ENABLED_CERT_SERVICE" = "yes"
then
    # Requires ecc,certgen, and opensslextra make sure on
    if test "x$ENABLED_CERTGEN" = "xno"
    then
        ENABLED_CERTGEN="yes"
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_CERT_GEN"
    fi
    if test "x$ENABLED_ECC" = "xno"
    then
        ENABLED_ECC="yes"
        AM_CFLAGS="$AM_CFLAGS -DHAVE_ECC -DTFM_ECC256"

        if test "$ENABLED_ECC_SHAMIR" = "yes"
        then
            AM_CFLAGS="$AM_CFLAGS -DECC_SHAMIR"
        fi
    fi
    if test "x$ENABLED_OPENSSLEXTRA" = "xno" && test "x$ENABLED_OPENSSLCOEXIST" = "xno"
    then
        ENABLED_OPENSSLEXTRA="yes"
        AM_CFLAGS="$AM_CFLAGS -DOPENSSL_EXTRA"
    fi
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_HAVE_CERT_SERVICE"
fi


# wolfSSL JNI
AC_ARG_ENABLE([jni],
    [AS_HELP_STRING([--enable-jni],[Enable wolfSSL JNI (default: disabled)])],
    [ ENABLED_JNI=$enableval ],
    [ ENABLED_JNI=no ]
    )
if test "$ENABLED_JNI" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_JNI -DHAVE_EX_DATA"

    # Enable prereqs if not already enabled
    if test "x$ENABLED_DTLS" = "xno"
    then
        ENABLED_DTLS="yes"
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_DTLS"
    fi
    if test "x$ENABLED_OPENSSLEXTRA" = "xno"
    then
        ENABLED_OPENSSLEXTRA="yes"
        AM_CFLAGS="$AM_CFLAGS -DOPENSSL_EXTRA"
    fi
    if test "x$ENABLED_OPENSSLALL" = "xno"
    then
        ENABLED_OPENSSLALL="yes"
        AM_CFLAGS="$AM_CFLAGS -DOPENSSL_ALL"
    fi
    if test "x$ENABLED_CRL" = "xno"
    then
        ENABLED_CRL="yes"
        AM_CFLAGS="$AM_CFLAGS -DHAVE_CRL"
    fi
    if test "x$ENABLED_OCSP" = "xno"
    then
        ENABLED_OCSP="yes"
    fi
    if test "x$ENABLED_CRL_MONITOR" = "xno" && test "x$ENABLED_DISTRO" = "xno"
    then
        ENABLED_CRL_MONITOR="yes"
        AM_CFLAGS="$AM_CFLAGS -DHAVE_CRL_MONITOR"
    fi
    if test "x$ENABLED_SAVESESSION" = "xno"
    then
        ENABLED_SAVESESSION="yes"
        AM_CFLAGS="$AM_CFLAGS -DPERSIST_SESSION_CACHE"
    fi
    if test "x$ENABLED_SAVECERT" = "xno"
    then
        ENABLED_SAVECERT="yes"
        AM_CFLAGS="$AM_CFLAGS -DPERSIST_CERT_CACHE"
    fi
    if test "x$ENABLED_ATOMICUSER" = "xno"
    then
        ENABLED_ATOMICUSER="yes"
        AM_CFLAGS="$AM_CFLAGS -DATOMIC_USER"
    fi
    if test "x$ENABLED_ECC" = "xno"
    then
        ENABLED_ECC="yes"
        AM_CFLAGS="$AM_CFLAGS -DHAVE_ECC -DTFM_ECC256"

        if test "$ENABLED_ECC_SHAMIR" = "yes"
        then
            AM_CFLAGS="$AM_CFLAGS -DECC_SHAMIR"
        fi
    fi
    # Do not enable PK Callbacks in FIPS mode with JNI
    if test "x$ENABLED_PKCALLBACKS" = "xno" && test "$ENABLED_FIPS" = "no"
    then
        ENABLED_PKCALLBACKS="yes"
        AM_CFLAGS="$AM_CFLAGS -DHAVE_PK_CALLBACKS"
    fi
    if test "x$ENABLED_DH" = "xno"
    then
        ENABLED_DH="yes"
        AM_CFLAGS="$AM_CFLAGS -DHAVE_DH"
    fi
    if test "x$ENABLED_PSK" = "xno"
    then
        ENABLED_PSK="yes"
    fi
    if test "x$ENABLED_CERTEXT" = "xno"
    then
        ENABLED_CERTEXT="yes"
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_CERT_EXT"
    fi
    if test "x$ENABLED_CERTGEN" = "xno"
    then
        ENABLED_CERTGEN="yes"
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_CERT_GEN"
    fi
    if test "x$ENABLED_SNI" = "xno"
    then
        ENABLED_SNI="yes"
        AM_CFLAGS="$AM_CFLAGS -DHAVE_TLS_EXTENSIONS -DHAVE_SNI"
    fi
    if test "x$ENABLED_ALPN" = "xno"
    then
        ENABLED_ALPN="yes"
        AM_CFLAGS="$AM_CFLAGS -DHAVE_ALPN"
    fi

    # cert gen requires alt names
    ENABLED_ALTNAMES="yes"
fi

if test "$ENABLED_LIGHTY" = "yes"
then
    # Requires opensslextra make sure on
    if test "x$ENABLED_OPENSSLEXTRA" = "xno" && test "x$ENABLED_OPENSSLCOEXIST" = "xno"
    then
        ENABLED_OPENSSLEXTRA="yes"
        AM_CFLAGS="$AM_CFLAGS -DOPENSSL_EXTRA"
    fi
    AM_CFLAGS="$AM_CFLAGS -DHAVE_LIGHTY -DHAVE_WOLFSSL_SSL_H=1"
    AM_CFLAGS="$AM_CFLAGS -DHAVE_EX_DATA"
    AM_CFLAGS="$AM_CFLAGS -DOPENSSL_ALL"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_KEY_GEN"
    # recommended if building wolfSSL specifically for use by lighttpd
    if test "x$ENABLED_ALL" = "xno"; then
        AM_CFLAGS="$AM_CFLAGS -DOPENSSL_NO_SSL2 -DOPENSSL_NO_COMP"
        if test "x$ENABLED_SSLV3" = "xno"; then
            AM_CFLAGS="$AM_CFLAGS -DOPENSSL_NO_SSL3"
            if test "x$ENABLED_TLSV10" = "xno"; then
                AM_CFLAGS="$AM_CFLAGS -DNO_OLD_TLS"
                ENABLED_OLD_TLS=no
            fi
        fi
        if test "x$ENABLED_CRL_MONITOR" = "xno"; then
            AM_CFLAGS="$AM_CFLAGS -DSINGLE_THREADED"
            ENABLED_SINGLETHREADED="yes"
        fi

        # w/ lighttpd 1.4.56 once wolfSSL updated to expose non-filesystem funcs
        #AM_CFLAGS="$AM_CFLAGS -DNO_BIO"
        #AM_CFLAGS="$AM_CFLAGS -DNO_FILESYSTEM"
        #ENABLED_FILESYSTEM=no
    fi
fi

if test "$ENABLED_NGINX" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_NGINX -DWOLFSSL_SIGNER_DER_CERT"
    AM_CFLAGS="$AM_CFLAGS -DOPENSSL_COMPATIBLE_DEFAULTS"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_ERROR_CODE_OPENSSL"
fi

if test "$ENABLED_HAPROXY" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_HAPROXY -DOPENSSL_COMPATIBLE_DEFAULTS"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SIGNER_DER_CERT"
    # --enable-all defines its own DEFAULT_MAX_CLASSIC_ASYM_KEY_BITS
    if test -z "$DEFAULT_MAX_CLASSIC_ASYM_KEY_BITS"
    then
        DEFAULT_MAX_CLASSIC_ASYM_KEY_BITS=8192
    fi
    # Requires opensslextra and opensslall
    if test "x$ENABLED_OPENSSLALL" = "xno" && test "x$ENABLED_OPENSSLCOEXIST" = "xno"
    then
        ENABLED_OPENSSLALL="yes"
        ENABLED_OPENSSLEXTRA="yes"
        AM_CFLAGS="$AM_CFLAGS -DOPENSSL_EXTRA -DOPENSSL_ALL"
    fi

    if test "x$ENABLED_CERTGEN" = "xno"
    then
        ENABLED_CERTGEN="yes"
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_CERT_GEN"
    fi

    if test "x$ENABLED_CERTREQ" = "xno"
    then
        ENABLED_CERTREQ="yes"
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_CERT_REQ"
    fi

    # Requires sessioncerts make sure on
    if test "x$ENABLED_SESSIONCERTS" = "xno"
    then
        ENABLED_SESSIONCERTS="yes"
        AM_CFLAGS="$AM_CFLAGS -DSESSION_CERTS"
    fi

    # Requires key gen make sure on
    if test "x$ENABLED_KEYGEN" = "xno"
    then
        ENABLED_KEYGEN="yes"
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_KEY_GEN"
    fi
fi

if test "$ENABLED_NETSNMP" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_EX_DATA"

    if test "x$ENABLED_AESCFB" = "xno"
    then
        ENABLED_AESCFB="yes"
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_AES_CFB"
    fi

    if test "x$ENABLED_DTLS" = "xno"
    then
        ENABLED_DTLS="yes"
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_DTLS"
    fi
fi

if test "$ENABLED_KRB" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_KRB -DWOLFSSL_AES_DIRECT -DWOLFSSL_DES_ECB"
    AM_CFLAGS="$AM_CFLAGS -DHAVE_EX_DATA"

    # Requires PKCS7
    if test "x$ENABLED_PKCS7" = "xno"
    then
        ENABLED_PKCS7="yes"
    fi
fi

if test "$ENABLED_FFMPEG" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_FFMPEG -DOPENSSL_COMPATIBLE_DEFAULTS"
fi

if test "$ENABLED_SIGNAL" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SIGNAL -DWOLFSSL_AES_COUNTER -DWOLFSSL_AES_DIRECT"
    # Requires opensslextra make sure on
    if test "x$ENABLED_OPENSSLEXTRA" = "xno" && test "x$ENABLED_OPENSSLCOEXIST" = "xno"
    then
        ENABLED_OPENSSLEXTRA="yes"
        AM_CFLAGS="$AM_CFLAGS -DOPENSSL_EXTRA"
    fi
fi

if test "$ENABLED_BIND" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_BIND -DWOLFSSL_DSA_768_MODULUS"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_AES_DIRECT -DHAVE_AES_ECB -DWOLFSSL_DES_ECB"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SHA224 -DWOLFSSL_SHA384 -DWOLFSSL_SHA512"
    AM_CFLAGS="$AM_CFLAGS -DOPENSSL_COMPATIBLE_DEFAULTS"
    ENABLED_SHA224="yes"
    ENABLED_SHA384="yes"
    ENABLED_SHA512="yes"
fi

if test "$ENABLED_RSYSLOG" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_RSYSLOG -DWOLFSSL_ERROR_CODE_OPENSSL"
    AM_CFLAGS="$AM_CFLAGS -DHAVE_EX_DATA -DOPENSSL_COMPATIBLE_DEFAULTS"
fi

if test "$ENABLED_OPENVPN" = "yes"
then
    ENABLED_SUPPORTED_CURVES="yes"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_OPENVPN -DHAVE_KEYING_MATERIAL"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_DES_ECB -DHAVE_EX_DATA -DWOLFSSL_KEY_GEN"
fi

if test "$ENABLED_HITCH" = "yes"
then
    # Requires opensslextra make sure on
    if test "x$ENABLED_OPENSSLEXTRA" = "xno" && test "x$ENABLED_OPENSSLCOEXIST" = "xno"
    then
        ENABLED_OPENSSLEXTRA="yes"
        AM_CFLAGS="$AM_CFLAGS -DOPENSSL_EXTRA"
    fi

    # Requires OCSP make sure on
    if test "x$ENABLED_OCSP" = "xno"
    then
        ENABLED_OCSP="yes"
    fi

    # Requires ALPN
    if test "x$ENABLED_ALPN" = "xno"
    then
        ENABLED_ALPN="yes"
        AM_CFLAGS="$AM_CFLAGS -DHAVE_TLS_EXTENSIONS -DHAVE_ALPN"
    fi

    if test "x$ENABLED_KEYGEN" = "xno"
    then
        ENABLED_KEYGEN="yes"
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_KEY_GEN"
    fi

    # Requires sessioncerts make sure on
    if test "x$ENABLED_SESSIONCERTS" = "xno"
    then
        ENABLED_SESSIONCERTS="yes"
        AM_CFLAGS="$AM_CFLAGS -DSESSION_CERTS"
    fi

    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_HITCH -DHAVE_EX_DATA -DWOLFSSL_SIGNER_DER_CERT"
    AM_CFLAGS="$AM_CFLAGS -DOPENSSL_COMPATIBLE_DEFAULTS -DWOLFSSL_CIPHER_INTERNALNAME"
fi


if test "$ENABLED_NGINX" = "yes"|| test "x$ENABLED_HAPROXY" = "xyes" || test "x$ENABLED_LIGHTY" = "xyes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_ALWAYS_VERIFY_CB"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_ALWAYS_KEEP_SNI"
    AM_CFLAGS="$AM_CFLAGS -DKEEP_OUR_CERT -DKEEP_PEER_CERT"
    AM_CFLAGS="$AM_CFLAGS -DHAVE_EXT_CACHE -DHAVE_EX_DATA"

    ENABLED_CERTGEN="yes"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_CERT_GEN"
fi

if (test "$ENABLED_OPENSSH" = "yes" && test "x$ENABLED_FIPS" = "xno") || \
   test "$ENABLED_WPAS" = "yes" || test "$ENABLED_QT" = "yes"
then
    test "$enable_arc4" = "" && enable_arc4=yes
fi

if test "$ENABLED_ARC4" = "no"
then
    AM_CFLAGS="$AM_CFLAGS -DNO_RC4"
else
    # turn off ARC4 if leanpsk or leantls on
    if test "$ENABLED_LEANPSK" = "yes" || test "$ENABLED_LEANTLS" = "yes"
    then
        AM_CFLAGS="$AM_CFLAGS -DNO_RC4"
        ENABLED_ARC4=no
    fi
fi

# Asio Support
AC_ARG_ENABLE([asio],
    [AS_HELP_STRING([--enable-asio],[Enable asio (default: disabled)])],
    [ ENABLED_ASIO=$enableval ],
    [ ENABLED_ASIO=no ]
    )
if test "$ENABLED_ASIO" = "yes"
then
    # Requires opensslextra and opensslall
    if test "x$ENABLED_OPENSSLALL" = "xno" && test "x$ENABLED_OPENSSLCOEXIST" = "xno"
    then
        ENABLED_OPENSSLALL="yes"
        ENABLED_OPENSSLEXTRA="yes"
        AM_CFLAGS="$AM_CFLAGS -DOPENSSL_EXTRA -DOPENSSL_ALL"
    fi
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_ASIO -DASIO_USE_WOLFSSL -DWOLFSSL_KEY_GEN"
    AM_CFLAGS="$AM_CFLAGS -DBOOST_ASIO_USE_WOLFSSL -DHAVE_EX_DATA"
    AM_CFLAGS="$AM_CFLAGS -DSSL_TXT_TLSV1_2"
    AM_CFLAGS="$AM_CFLAGS -DOPENSSL_NO_SSL2 -DOPENSSL_NO_SSL3"
    if test "$ENABLED_TLSV10" = "yes"
    then
        AM_CFLAGS="$AM_CFLAGS -DSSL_TXT_TLSV1"
    fi
    if test "$ENABLED_OLD_TLS" = "yes"
    then
        AM_CFLAGS="$AM_CFLAGS -DSSL_TXT_TLSV1_1"
    fi

    # Requires OCSP make sure on
    if test "x$ENABLED_OCSP" = "xno"
    then
        ENABLED_OCSP="yes"
    fi
fi

# Apache HTTPD
AC_ARG_ENABLE([apachehttpd],
    [AS_HELP_STRING([--enable-apachehttpd],[Enable Apache httpd (default: disabled)])],
    [ ENABLED_APACHE_HTTPD=$enableval ],
    [ ENABLED_APACHE_HTTPD=no ]
    )
if test "$ENABLED_APACHE_HTTPD" = "yes"
then
    # Requires opensslextra and opensslall
    if test "x$ENABLED_OPENSSLALL" = "xno" && test "x$ENABLED_OPENSSLCOEXIST" = "xno"
    then
        ENABLED_OPENSSLALL="yes"
        ENABLED_OPENSSLEXTRA="yes"
        AM_CFLAGS="$AM_CFLAGS -DOPENSSL_EXTRA -DOPENSSL_ALL"
    fi
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_APACHE_HTTPD"
    AM_CFLAGS="$AM_CFLAGS -DOPENSSL_NO_SSL2 -DOPENSSL_NO_SSL3 -DOPENSSL_NO_COMP"
    AM_CFLAGS="$AM_CFLAGS -DHAVE_EX_DATA -DWOLFSSL_SIGNER_DER_CERT"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_CERT_EXT -DWOLFSSL_CERT_GEN"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_ALWAYS_KEEP_SNI"
    AM_CFLAGS="$AM_CFLAGS -DOPENSSL_COMPATIBLE_DEFAULTS"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_NO_OCSP_ISSUER_CHECK"

    # Requires OCSP make sure on
    if test "x$ENABLED_OCSP" = "xno"
    then
        ENABLED_OCSP="yes"
    fi

    # Requires sessioncerts make sure on
    if test "x$ENABLED_SESSIONCERTS" = "xno"
    then
        ENABLED_SESSIONCERTS="yes"
        AM_CFLAGS="$AM_CFLAGS -DSESSION_CERTS"
    fi

    # Requires ALPN
    if test "x$ENABLED_ALPN" = "xno"
    then
        ENABLED_ALPN="yes"
        AM_CFLAGS="$AM_CFLAGS -DHAVE_TLS_EXTENSIONS -DHAVE_ALPN"
    fi

    # Requires CRL
    if test "x$ENABLED_CRL" = "xno"
    then
        ENABLED_CRL="yes"
        AM_CFLAGS="$AM_CFLAGS -DHAVE_CRL"
    fi

    # Requires Certificate Generation, Request and Extensions
    if test "x$ENABLED_CERTGEN" = "xno"
    then
        ENABLED_CERTGEN="yes"
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_CERT_GEN"
    fi
    if test "x$ENABLED_CERTREQ" = "xno"
    then
        ENABLED_CERTREQ="yes"
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_CERT_REQ"
    fi
    if test "x$ENABLED_CERTEXT" = "xno"
    then
        ENABLED_CERTEXT="yes"
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_CERT_EXT"
    fi

    # Requires Secure Renegotiation
    if test "x$ENABLED_SECURE_RENEGOTIATION" = "xno"
    then
            AM_CFLAGS="$AM_CFLAGS -DHAVE_TLS_EXTENSIONS -DHAVE_SECURE_RENEGOTIATION -DHAVE_SERVER_RENEGOTIATION_INFO"
    fi
fi

# Encrypt-Then-Mac
AC_ARG_ENABLE([enc-then-mac],
    [AS_HELP_STRING([--enable-enc-then-mac],[Enable Encrypt-Then-Mac extension (default: enabled)])],
    [ ENABLED_ENCRYPT_THEN_MAC=$enableval ],
    [ ENABLED_ENCRYPT_THEN_MAC=yes ]
    )

if test "x$ENABLED_TLSX" = "xyes"
then
    ENABLED_ENCRYPT_THEN_MAC=yes
fi

if test "x$ENABLED_ENCRYPT_THEN_MAC" = "xyes"
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_ENCRYPT_THEN_MAC"
fi


# stunnel Support
AC_ARG_ENABLE([stunnel],
    [AS_HELP_STRING([--enable-stunnel],[Enable stunnel (default: disabled)])],
    [ ENABLED_STUNNEL=$enableval ],
    [ ENABLED_STUNNEL=no ]
    )
if test "$ENABLED_WPAS" = "yes"
then
    ENABLED_STUNNEL="yes"
fi
# stunnel support requires all the features enabled within this conditional.
if test "$ENABLED_STUNNEL" = "yes"
then
    if test "x$ENABLED_OPENSSLEXTRA" = "xno" && test "x$ENABLED_OPENSSLCOEXIST" = "xno"
    then
        ENABLED_OPENSSLEXTRA="yes"
        AM_CFLAGS="$AM_CFLAGS -DOPENSSL_EXTRA"
    fi

    if test "x$ENABLED_SESSION_TICKET" = "xno"
    then
        ENABLED_SESSION_TICKET="yes"
        AM_CFLAGS="$AM_CFLAGS -DHAVE_SESSION_TICKET"
    fi

    if test "x$ENABLED_OCSP" = "xno"
    then
        ENABLED_OCSP="yes"
    fi

    if test "x$ENABLED_CODING" = "xno"
    then
        ENABLED_CODING="yes"
    fi

    if test "x$ENABLED_SESSIONCERTS" = "xno"
    then
        ENABLED_SESSIONCERTS="yes"
        AM_CFLAGS="$AM_CFLAGS -DSESSION_CERTS"
    fi

    if test "x$ENABLED_CRL" = "xno"
    then
        ENABLED_CRL="yes"
        AM_CFLAGS="$AM_CFLAGS -DHAVE_CRL"
    fi

    if test "x$ENABLED_DES3" = "xno"
    then
        ENABLED_DES3="yes"
    fi

    if test "x$ENABLED_TLSX" = "xno"
    then
        ENABLED_TLSX="yes"
        AM_CFLAGS="$AM_CFLAGS -DHAVE_TLS_EXTENSIONS -DHAVE_SNI -DHAVE_MAX_FRAGMENT -DHAVE_TRUNCATED_HMAC"

        # Check the ECC supported curves prereq
        AS_IF([test "x$ENABLED_ECC" != "xno" || test "x$ENABLED_CURVE25519" = "xyes"],
              [ENABLED_SUPPORTED_CURVES=yes
               AM_CFLAGS="$AM_CFLAGS -DHAVE_SUPPORTED_CURVES"])
    fi

    if test "x$ENABLED_ECC" = "xno"
    then
        ENABLED_OPENSSLEXTRA="yes"
        ENABLED_ECC="yes"
        AM_CFLAGS="$AM_CFLAGS -DHAVE_ECC -DTFM_ECC256"

        if test "$ENABLED_ECC_SHAMIR" = "yes"
        then
            AM_CFLAGS="$AM_CFLAGS -DECC_SHAMIR"
        fi
    fi

    if test "x$ENABLED_CERTEXT" = "xno"
    then
        ENABLED_CERTEXT="yes"
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_CERT_EXT"
    fi

    if test "x$ENABLED_CERTGEN" = "xno"
    then
        ENABLED_CERTGEN="yes"
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_CERT_GEN"
    fi

    if test "x$ENABLED_KEYGEN" = "xno"
    then
        ENABLED_KEYGEN="yes"
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_KEY_GEN"
    fi

    AM_CFLAGS="$AM_CFLAGS -DHAVE_STUNNEL -DWOLFSSL_ALWAYS_VERIFY_CB"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_ALWAYS_KEEP_SNI -DHAVE_EX_DATA"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_DES_ECB -DWOLFSSL_SIGNER_DER_CERT"
    AM_CFLAGS="$AM_CFLAGS -DOPENSSL_COMPATIBLE_DEFAULTS -DWOLFSSL_TICKET_HAVE_ID"
fi

# curl Support
AC_ARG_ENABLE([curl],
    [AS_HELP_STRING([--enable-curl],[Enable curl (default: disabled)])],
    [ ENABLED_CURL=$enableval ],
    [ ENABLED_CURL=no ]
    )
# curl support requires all the features enabled within this conditional.
if test "$ENABLED_CURL" = "yes"
then
    if test "$ENABLED_MD4" = "no"
    then
        ENABLED_MD4="yes"
    fi

    if test "x$ENABLED_DES3" = "xno"
    then
        ENABLED_DES3="yes"
    fi

    if test "x$ENABLED_ALPN" = "xno"
    then
        ENABLED_ALPN="yes"
    fi

    if test "x$ENABLED_WOLFSSH" = "xno"
    then
        ENABLED_WOLFSSH="yes"
    fi

    if test "x$ENABLED_OPENSSLEXTRA" = "xno" && test "x$ENABLED_OPENSSLCOEXIST" = "xno"
    then
        ENABLED_OPENSSLEXTRA="yes"
    fi

    if test "x$ENABLED_CRL" = "xno"
    then
        ENABLED_CRL="yes"
        AM_CFLAGS="$AM_CFLAGS -DHAVE_CRL"
    fi

    if test "x$ENABLED_OCSP" = "xno"
    then
        ENABLED_OCSP="yes"
    fi

    if test "x$ENABLED_CERTIFICATE_STATUS_REQUEST" = "xno"
    then
        ENABLED_CERTIFICATE_STATUS_REQUEST="yes"
        AM_CFLAGS="$AM_CFLAGS -DHAVE_TLS_EXTENSIONS -DHAVE_CERTIFICATE_STATUS_REQUEST"
    fi

    if test "x$ENABLED_CERTIFICATE_STATUS_REQUEST_V2" = "xno"
    then
        ENABLED_CERTIFICATE_STATUS_REQUEST_V2="yes"
        AM_CFLAGS="$AM_CFLAGS -DHAVE_TLS_EXTENSIONS -DHAVE_CERTIFICATE_STATUS_REQUEST_V2"
    fi

    if test "x$ENABLED_SNI" = "xno"
    then
        ENABLED_SNI="yes"
        AM_CFLAGS="$AM_CFLAGS -DHAVE_TLS_EXTENSIONS -DHAVE_SNI"
    fi

    if test "x$ENABLED_ALT_CERT_CHAINS" = "xno"
    then
        ENABLED_ALT_CERT_CHAINS="yes"
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_ALT_CERT_CHAINS"
    fi

    if test "x$ENABLE_IP_ALT_NAME" = "xno"
    then
        ENABLE_IP_ALT_NAME="yes"
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_IP_ALT_NAME"
    fi

    if test "x$ENABLED_SESSION_TICKET" = "xno"
    then
        ENABLED_SESSION_TICKET="yes"
        AM_CFLAGS="$AM_CFLAGS -DHAVE_TLS_EXTENSIONS -DHAVE_SESSION_TICKET"
    fi

    # FTPS server requires pointer to session cache
    AM_CFLAGS="$AM_CFLAGS -DNO_SESSION_CACHE_REF"

    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_DES_ECB"
fi

if test "$ENABLED_PSK" = "no" && test "$ENABLED_LEANPSK" = "no" \
    && test "x$ENABLED_STUNNEL" = "xno"
then
    AM_CFLAGS="$AM_CFLAGS -DNO_PSK"
fi

if test "$ENABLED_PSK" = "no" &&  \
    (test "$ENABLED_LEANPSK" = "yes" || test "x$ENABLED_STUNNEL" = "xyes")
then
    ENABLED_PSK=yes
fi

# tcpdump support
AC_ARG_ENABLE([tcpdump],
    [AS_HELP_STRING([--enable-tcpdump],[Enable tcpdump (default: disabled)])],
    [ ENABLED_TCPDUMP=$enableval ],
    [ ENABLED_TCPDUMP=no ]
    )
# tcpdump support requires all the features enabled within this conditional.
if test "$ENABLED_TCPDUMP" = "yes"
then
    if test "x$ENABLED_OPENSSLEXTRA" = "xno" && test "x$ENABLED_OPENSSLCOEXIST" = "xno"
    then
        ENABLED_OPENSSLEXTRA="yes"
        AM_CFLAGS="$AM_CFLAGS -DOPENSSL_EXTRA"
    fi

    if test "x$ENABLED_DES3" = "xno"
    then
        ENABLED_DES3="yes"
    fi
fi

# sblim-sfcb support
AC_ARG_ENABLE([sblim-sfcb],
    [AS_HELP_STRING([--enable-sblim-sfcb],[Enable sblim-sfcb support (default: disabled)])],
    [ ENABLED_SBLIM_SFCB=$enableval ],
    [ ENABLED_SBLIM_SFCB=no ]
    )
# sblim-sfcb support requires all the features enabled within this conditional.
if test "$ENABLED_SBLIM_SFCB" = "yes"
then
    if test "x$ENABLED_OPENSSLEXTRA" = "xno" && test "x$ENABLED_OPENSSLCOEXIST" = "xno"
    then
        ENABLED_OPENSSLEXTRA="yes"
        AM_CFLAGS="$AM_CFLAGS -DOPENSSL_EXTRA"
    fi

    if test "x$ENABLED_CERTGEN" = "xno"
    then
        ENABLED_CERTGEN="yes"
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_CERT_GEN"
    fi

    AM_CFLAGS="$AM_CFLAGS -DHAVE_SBLIM_SFCB -DWOLFSSL_SIGNER_DER_CERT"
fi

# libest Support
AC_ARG_ENABLE([libest],
    [AS_HELP_STRING([--enable-libest],[Enable libest (default: disabled)])],
    [ ENABLED_LIBEST=$enableval ],
    [ ENABLED_LIBEST=no ]
    )

if test "$ENABLED_LIBEST" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_EX_DATA -DHAVE_LIBEST -DWOLFSSL_ALT_NAMES"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_PSS_SALT_LEN_DISCOVER"

    # Requires opensslextra and opensslall
    if test "x$ENABLED_OPENSSLALL" = "xno" && test "x$ENABLED_OPENSSLCOEXIST" = "xno"
    then
        ENABLED_OPENSSLALL="yes"
        ENABLED_OPENSSLEXTRA="yes"
        AM_CFLAGS="$AM_CFLAGS -DOPENSSL_EXTRA -DOPENSSL_ALL"
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_EITHER_SIDE -DWC_RSA_NO_PADDING"
        AM_CFLAGS="$AM_CFLAGS -DWC_RSA_PSS -DWOLFSSL_PSS_LONG_SALT"
    fi

    # Requires OCSP
    if test "x$ENABLED_OCSP" = "xno"
    then
        ENABLED_OCSP="yes"
    fi

    # Requires PKCS7
    if test "x$ENABLED_PKCS7" = "xno"
    then
        ENABLED_PKCS7="yes"
    fi

    # Requires Certificate Generation and Request
    if test "x$ENABLED_CERTGEN" = "xno"
    then
        ENABLED_CERTGEN="yes"
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_CERT_GEN"
    fi
    if test "x$ENABLED_CERTREQ" = "xno"
    then
        ENABLED_CERTREQ="yes"
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_CERT_REQ"
    fi
    if test "x$ENABLED_CERTEXT" = "xno"
    then
        ENABLED_CERTEXT="yes"
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_CERT_EXT"
    fi

    # Requires CRL
    if test "x$ENABLED_CRL" = "xno"
    then
        ENABLED_CRL="yes"
        AM_CFLAGS="$AM_CFLAGS -DHAVE_CRL"
    fi

    if test "x$ENABLED_SRP" = "xno"
    then
        ENABLED_SRP="yes"
        AM_CFLAGS="$AM_CFLAGS -DWOLFCRYPT_HAVE_SRP"
    fi

    # Enable prereqs if not already enabled
    if test "x$ENABLED_KEYGEN" = "xno"
    then
        ENABLED_KEYGEN="yes"
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_KEY_GEN"
    fi

    # Requires sessioncerts make sure on
    if test "x$ENABLED_SESSIONCERTS" = "xno"
    then
        ENABLED_SESSIONCERTS="yes"
        AM_CFLAGS="$AM_CFLAGS -DSESSION_CERTS"
    fi

    if test "x$ENABLED_DSA" = "xno"
    then
        AC_MSG_WARN([Enabling DSA with --enable-dsa is recommended for libest])
    fi
fi

if test "$ENABLED_MD4" = "no"
then
    #turn on MD4 if using stunnel
    if test "x$ENABLED_STUNNEL" = "xyes" || test "x$ENABLED_WPAS" != "xno" || test "x$ENABLED_KRB" = "xyes"
    then
        ENABLED_MD4="yes"
    else
        AM_CFLAGS="$AM_CFLAGS -DNO_MD4"
    fi
fi


# Encrypted keys
AC_ARG_ENABLE([enckeys],
    [AS_HELP_STRING([--enable-enckeys],[Enable PEM encrypted private key support (default: disabled)])],
    [ ENABLED_ENCKEYS=$enableval ],
    [ ENABLED_ENCKEYS=no ]
    )

if test "$ENABLED_OPENSSLEXTRA" = "yes" || test "$ENABLED_WEBSERVER" = "yes" || test "$ENABLED_WPAS" != "no"
then
    ENABLED_ENCKEYS=yes
fi

if test "$ENABLED_ENCKEYS" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_ENCRYPTED_KEYS"
fi


# PKCS#12
# set PKCS#12 default
PKCS12_DEFAULT=yes
if test "$ENABLED_ASN" = "no" || test "$FIPS_VERSION" = "rand"
then
    PKCS12_DEFAULT=no
fi
AC_ARG_ENABLE([pkcs12],
    [AS_HELP_STRING([--enable-pkcs12],[Enable pkcs12 (default: enabled)])],
    [ ENABLED_PKCS12=$enableval ],
    [ ENABLED_PKCS12=$PKCS12_DEFAULT ]
    )

if test "x$ENABLED_PKCS12" = "xno"
then
    AM_CFLAGS="$AM_CFLAGS -DNO_PKCS12"
fi

# PWDBASED has to come after certservice since we want it on w/o explicit on
# PWDBASED
AC_ARG_ENABLE([pwdbased],
    [AS_HELP_STRING([--enable-pwdbased],[Enable PWDBASED (default: disabled)])],
    [ ENABLED_PWDBASED=$enableval ],
    [ ENABLED_PWDBASED=no ]
    )

if test "$ENABLED_PWDBASED" = "no"
then
    if test "$ENABLED_OPENSSLEXTRA" = "yes" || test "$ENABLED_OPENSSLALL" = "yes" || \
       test "$ENABLED_WEBSERVER" = "yes" || test "$ENABLED_ENCKEYS" = "yes" || \
       test "$ENABLED_PKCS12" = "yes"
    then
        # opensslextra, opensslall, webserver, enckeys and pkcs12 need pwdbased
        ENABLED_PWDBASED=yes
    else
        AM_CFLAGS="$AM_CFLAGS -DNO_PWDBASED"
    fi
fi


AC_ARG_ENABLE([scrypt],
    [AS_HELP_STRING([--enable-scrypt],[Enable SCRYPT (default: disabled)])],
    [ ENABLED_SCRYPT=$enableval ],
    [ ENABLED_SCRYPT=no ]
    )

if test "$ENABLED_SCRYPT" = "yes"
then
    if test "$ENABLED_PWDBASED" = "no"
    then
        AC_MSG_ERROR([cannot enable scrypt without enabling pwdbased.])
    fi
    AM_CFLAGS="$AM_CFLAGS -DHAVE_SCRYPT"
fi


# wolfCrypt Only Build
if test "$ENABLED_CRYPTONLY" = "yes"
then
    if test "$ENABLED_OPENSSLALL" = "yes"
    then
        AC_MSG_ERROR([cryptonly and opensslall are mutually incompatible.])
    fi
    AM_CFLAGS="$AM_CFLAGS -DWOLFCRYPT_ONLY"
fi

if test "x$ENABLED_CRYPTONLY" = "xno"
then
    if test "x$ENABLED_PSK" = "xno" && test "x$ENABLED_ASN" = "xno"
    then
        AC_MSG_ERROR([please enable psk if disabling asn.])
    fi

    if test "$ENABLED_AFALG" = "yes"
    then
        # for TLS connections the intermediate hash needs to store buffer
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_AFALG_HASH_KEEP"
    fi

    if test "$ENABLED_DEVCRYPTO" = "yes"
    then
        # for TLS connections the intermediate hash needs to store buffer
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_DEVCRYPTO_HASH_KEEP"
    fi
fi


# Enable Examples, used to disable examples
if test "$ENABLED_LINUXKM" = "yes"
then
    ENABLED_EXAMPLES_DEFAULT=no
else
    ENABLED_EXAMPLES_DEFAULT=yes
fi
AC_ARG_ENABLE([examples],
    [AS_HELP_STRING([--enable-examples],[Enable Examples  (default: enabled)])],
    [ ENABLED_EXAMPLES=$enableval ],
    [ ENABLED_EXAMPLES=$ENABLED_EXAMPLES_DEFAULT ]
    )

AS_IF([test "x$ENABLED_FILESYSTEM" = "xno"], [ENABLED_EXAMPLES="no"])
AS_IF([test "x$ENABLED_CRYPTONLY" = "xyes"], [ENABLED_EXAMPLES="no"])


# Enable wolfCrypt test and benchmark
if test "$ENABLED_LINUXKM" = "yes"
then
    ENABLED_CRYPT_TESTS_DEFAULT=no
else
    ENABLED_CRYPT_TESTS_DEFAULT=yes
fi
AC_ARG_ENABLE([crypttests],
    [AS_HELP_STRING([--enable-crypttests],[Enable Crypt Bench/Test  (default: enabled)])],
    [ ENABLED_CRYPT_TESTS=$enableval ],
    [ ENABLED_CRYPT_TESTS=$ENABLED_CRYPT_TESTS_DEFAULT ]
    )
AC_SUBST([ENABLED_CRYPT_TESTS])

# Build wolfCrypt test and benchmark as libraries. This will compile test.c and
# benchmark.c and make their functions available via libraries, libwolfcrypttest
# and libwolfcryptbench, respectively. Note that this feature is not enabled by
# default, and the API of these libraries should NOT be treated as stable.
AC_ARG_ENABLE([crypttests-libs],
    [AS_HELP_STRING([--enable-crypttests-libs],[Enable wolfcrypt test and benchmark libraries (default: disabled)])],
    [ ENABLED_CRYPT_TESTS_LIBS=$enableval ],
    [ ENABLED_CRYPT_TESTS_LIBS=no ]
    )

# LIBZ
ENABLED_LIBZ="no"
trylibzdir=""
AC_ARG_WITH([libz],
    [  --with-libz=PATH        PATH to libz install (default /usr/) ],
    [
        AC_MSG_CHECKING([for libz])
        CPPFLAGS="$CPPFLAGS -DHAVE_LIBZ"
        LIBS="$LIBS -lz"

        AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <zlib.h>]], [[ deflateInit(0, 8); ]])],[ libz_linked=yes ],[ libz_linked=no ])

        if test "x$libz_linked" = "xno" ; then
            if test "x$withval" != "xno" ; then
                trylibzdir=$withval
            fi
            if test "x$withval" = "xyes" ; then
                trylibzdir="/usr"
            fi

            LDFLAGS="$LDFLAGS -L$trylibzdir/lib"
            CPPFLAGS="$CPPFLAGS -I$trylibzdir/include"

            AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <zlib.h>]], [[ deflateInit(0, 8); ]])],[ libz_linked=yes ],[ libz_linked=no ])

            if test "x$libz_linked" = "xno" ; then
                AC_MSG_ERROR([libz isn't found.
                If it's already installed, specify its path using --with-libz=/dir/])
            fi
            AC_MSG_RESULT([yes])
        else
            AC_MSG_RESULT([yes])
        fi
        ENABLED_LIBZ="yes"
    ]
)


# PKCS#11
AC_ARG_ENABLE([pkcs11],
    [AS_HELP_STRING([--enable-pkcs11],[Enable pkcs11 access (default: disabled)])],
    [ ENABLED_PKCS11=$enableval ],
    [ ENABLED_PKCS11=no ]
    )

if test "x$ENABLED_PKCS11" != "xno"
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_PKCS11 -DHAVE_WOLF_BIGINT"
    if test "x$ENABLED_PKCS11" != "xstatic"
    then
        LIBS="$LIBS -ldl"
    else
        AM_CFLAGS="$AM_CFLAGS -DHAVE_PKCS11_STATIC"
        ENABLED_PKCS11="yes"
    fi
fi


# PKCS#8
AC_ARG_ENABLE([pkcs8],
    [AS_HELP_STRING([--enable-pkcs8],[Enable PKCS #8 key packages (default: enabled)])],
    [ ENABLED_PKCS8=$enableval ],
    [ ENABLED_PKCS8=yes ]
    )

if test "x$ENABLED_PKCS8" = "xno"
then
    AM_CFLAGS="$AM_CFLAGS -DNO_PKCS8"
fi


# cavium
trycaviumdir=""
AC_ARG_WITH([cavium],
    [  --with-cavium=PATH      PATH to cavium/software dir ],
    [
        AC_MSG_CHECKING([for cavium])
        LIB_ADD="-lrt $LIB_ADD"

        if test "x$withval" = "xyes" ; then
            AC_MSG_ERROR([need a PATH for --with-cavium])
        fi
        if test "x$withval" != "xno" ; then
            trycaviumdir=$withval
        fi

        CPPFLAGS="$AM_CPPFLAGS -DHAVE_CAVIUM -I$trycaviumdir/include"
        LDFLAGS="$AM_LDFLAGS $trycaviumdir/api/cavium_common.o"

        AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include "cavium_common.h"]], [[ CspShutdown(CAVIUM_DEV_ID); ]])],[ cavium_linked=yes ],[ cavium_linked=no ])

        if test "x$cavium_linked" = "xno" ; then
            AC_MSG_ERROR([cavium isn't found.
            If it's already installed, specify its path using --with-cavium=/dir/])
        else
            AM_CPPFLAGS="$CPPFLAGS"
            AM_LDFLAGS="$LDFLAGS"
        fi
        AC_MSG_RESULT([yes])
        enable_shared=no
        enable_static=yes
        ENABLED_CAVIUM=yes
    ],
    [ ENABLED_CAVIUM=no ]
)

# cavium V
trycaviumdir=""
AC_ARG_WITH([cavium-v],
    [  --with-cavium-v=PATH    PATH to Cavium V/software dir ],
    [
        AC_MSG_CHECKING([for cavium])
        AM_CFLAGS="$AM_CFLAGS -DHAVE_CAVIUM -DHAVE_CAVIUM_V"
        LIB_ADD="-lrt -lcrypto $LIB_ADD"

        if test "x$withval" = "xyes" ; then
            AC_MSG_ERROR([need a PATH for --with-cavium])
        fi
        if test "x$withval" != "xno" ; then
            trycaviumdir=$withval
        fi

        if test -e $trycaviumdir/lib/libnitrox.a
        then
            AM_CPPFLAGS="-I$trycaviumdir/include $AM_CPPFLAGS"
        else
            ENABLED_CAVIUM_V=no
        fi
        LIB_STATIC_ADD="$trycaviumdir/lib/libnitrox.a $LIB_STATIC_ADD"

        if test "$ENABLED_CAVIUM_V" = "no"; then
            AC_MSG_ERROR([Could not find Nitrox library])
        fi

        enable_shared=no
        enable_static=yes
        enable_opensslextra=yes

        ENABLED_CAVIUM=yes
        ENABLED_CAVIUM_V=yes
    ],
    [
        ENABLED_CAVIUM_=no
        ENABLED_CAVIUM_V=no
    ]
)


# Cavium Octeon
OCTEON_ROOT=""
: ${OCTEON_OBJ="obj-octeon2"}
: ${OCTEON_HOST="standalone"}

AC_ARG_WITH([octeon-sync],
    [AS_HELP_STRING([--with-octeon-sync=PATH],[PATH to Cavium Octeon SDK dir (sync)])],
    [
        AC_MSG_CHECKING([for octeon])

        if test "x$withval" = "xyes" ; then
            AC_MSG_ERROR([need a PATH for --with-octeon])
        fi
        if test "x$withval" != "xno" ; then
            OCTEON_ROOT=$withval
        fi

        AM_CFLAGS="$AM_CFLAGS -DHAVE_CAVIUM_OCTEON_SYNC"
        AM_CFLAGS="$AM_CFLAGS -DOCTEON_MODEL=$OCTEON_MODEL"
        AM_CFLAGS="$AM_CFLAGS -I$OCTEON_ROOT/executive"
        AS_CASE([$OCTEON_HOST],['linux'],[AM_CFLAGS="$AM_CFLAGS -DCVMX_BUILD_FOR_LINUX_HOST"])

        #-I$OCTEON_ROOT/target/include
        AM_LDFLAGS="$AM_LDFLAGS -lrt -Xlinker -T -Xlinker $OCTEON_ROOT/executive/cvmx-shared-linux.ld"
        AM_LDFLAGS="$AM_LDFLAGS -L$OCTEON_ROOT/executive/$OCTEON_OBJ -lcvmx -lfdt"

        enable_shared=no
        enable_static=yes

        ENABLED_OCTEON_SYNC=yes
        AC_MSG_RESULT([yes])
    ],
    [ENABLED_OCTEON_SYNC=no]
)


# Intel QuickAssist
QAT_DIR=""
BUILD_INTEL_QAT_VERSION=2

AC_ARG_WITH([intelqa],
    [AS_HELP_STRING([--with-intelqa=PATH],[PATH to Intel QuickAssist (QAT) driver dir])],
    [ENABLED_INTEL_QA=yes; QAT_DIR=$withval],
    [ENABLED_INTEL_QA=no])

AC_ARG_WITH([intelqa-sync],
    [AS_HELP_STRING([--with-intelqa-sync=PATH],[PATH to Intel QuickAssist (QAT) driver dir (sync)])],
    [ENABLED_INTEL_QA_SYNC=yes; QAT_DIR=$withval],
    [ENABLED_INTEL_QA_SYNC=no])

AS_IF([test "x$ENABLED_INTEL_QA" = "xyes" && test "x$ENABLED_INTEL_QA_SYNC" = "xyes"],
    [AC_MSG_ERROR([Both Intel QA Async and Sync are selected, only select one.])])

AS_IF([test "x$ENABLED_INTEL_QA" = "xyes" || test "x$ENABLED_INTEL_QA_SYNC" = "xyes"],
    [AC_MSG_CHECKING([for intelqa])
     AS_IF([test "x$ENABLED_INTEL_QA" = "xyes"],
           [AM_CPPFLAGS="$AM_CPPFLAGS -DHAVE_INTEL_QA -DDO_CRYPTO -DUSER_SPACE"; intelqa_opt=""],
           [AM_CPPFLAGS="$AM_CPPFLAGS -DHAVE_INTEL_QA_SYNC -DQAT_USE_POLLING_THREAD -DO_CRYPTO -DUSER_SPACE"; intelqa_opt="-sync"])
     OLD_LIBS="$LIBS"
     OLD_CPPFLAGS="$CPPFLAGS"

     AS_IF([test "x$QAT_DIR" = "xyes"],[AC_MSG_ERROR([need a PATH for --with-intelqa$intelqa_opt])])

     QAT_FLAGS="-I$QAT_DIR/quickassist/include -I$QAT_DIR/quickassist/include/lac -I$QAT_DIR/quickassist/utilities/osal/include \
         -I$QAT_DIR/quickassist/utilities/osal/src/linux/user_space/include -I$QAT_DIR/quickassist/lookaside/access_layer/include \
         -I$QAT_DIR/quickassist/lookaside/access_layer/src/common/include -I$srcdir/wolfssl -I$srcdir/wolfssl/wolfcrypt/port/intel \
         -I$QAT_DIR/quickassist/utilities/libusdm_drv"
     AM_CPPFLAGS="$AM_CPPFLAGS $QAT_FLAGS"
     CPPFLAGS="$AM_CPPFLAGS"

     LDFLAGS="$LDFLAGS -L$QAT_DIR/build"
     LIBS="$LIBS -lqat_s -lusdm_drv_s"

     AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include "cpa_cy_common.h"]],[[Cpa16U count = 0; cpaCyGetNumInstances(&count);]])],[intelqa_linked=yes],[intelqa_linked=no])

     AS_IF([test "x$intelqa_linked" = "xno"],
           [# Try old QAT driver libraries
            LIBS="$OLD_LIBS -licp_qa_al_s"
            AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include "cpa_cy_common.h"]],[[Cpa16U count = 0; cpaCyGetNumInstances(&count);]])],[intelqa_linked=yes],[intelqa_linked=no])
            AS_IF([test "x$intelqa_linked" = "xno"],
                [AC_MSG_ERROR([Intel QuickAssist not found. If it's already installed, specify its path using --with-intelqa$intelqa_opt=/dir/])],
                [BUILD_INTEL_QAT_VERSION=1])
           ])

     AC_MSG_RESULT([yes])

     AS_IF([test "x$BUILD_INTEL_QAT_VERSION" = "x1"],
        [LIB_ADD="-ladf_proxy -losal -lrt $LIB_ADD"],
        [LIB_ADD="-losal -lrt $LIB_ADD"])
     CPPFLAGS="$OLD_CPPFLAGS"
])

################################################################################
# Single Precision option handling                                             #
################################################################################

ENABLED_SP_RSA=no
ENABLED_SP_DH=no
ENABLED_SP_FF_2048=no
ENABLED_SP_FF_3072=no
ENABLED_SP_FF_4096=no
ENABLED_SP_ECC=no
ENABLED_SP_EC_256=no
ENABLED_SP_EC_384=no
ENABLED_SP_EC_521=no
ENABLED_SP_SAKKE_1024=$ENABLED_SAKKE
ENABLED_SP_NO_MALLOC=no
ENABLED_SP_NONBLOCK=no
ENABLED_SP_SMALL=no
for v in `echo $ENABLED_SP | tr "," " "`
do
  case $v in
  small)
    ENABLED_SP_SMALL=yes
    ENABLED_SP_RSA=yes
    ENABLED_SP_DH=yes
    ENABLED_SP_FF_2048=yes
    ENABLED_SP_FF_3072=yes
    ENABLED_SP_ECC=yes
    ENABLED_SP_EC_256=yes
    if test "$host_cpu" = "x86_64" || test "$host_cpu" = "aarch64" || test "$host_cpu" = "amd64"; then
      ENABLED_SP_FF_4096=yes
      ENABLED_SP_EC_384=yes
      ENABLED_SP_EC_521=yes
    fi
    ;;

  smallfast)
    ENABLED_SP_SMALL=yes
    ENABLED_SP_RSA=yes
    ENABLED_SP_DH=yes
    ENABLED_SP_FF_2048=yes
    ENABLED_SP_FF_3072=yes
    ENABLED_SP_ECC=yes
    ENABLED_SP_EC_256=yes
    if test "$host_cpu" = "x86_64" || test "$host_cpu" = "aarch64" || test "$host_cpu" = "amd64"; then
      ENABLED_SP_FF_4096=yes
      ENABLED_SP_EC_384=yes
      ENABLED_SP_EC_521=yes
    fi
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SP_FAST_MODEXP"
    ;;

  yes)
    ENABLED_SP_RSA=yes
    ENABLED_SP_DH=yes
    ENABLED_SP_FF_2048=yes
    ENABLED_SP_FF_3072=yes
    ENABLED_SP_ECC=yes
    ENABLED_SP_EC_256=yes
    if test "$host_cpu" = "x86_64" || test "$host_cpu" = "aarch64" || test "$host_cpu" = "amd64"; then
      ENABLED_SP_FF_4096=yes
      ENABLED_SP_EC_384=yes
      ENABLED_SP_EC_521=yes
    fi
    ;;

  no)
    ;;

  smallec256 | smallp256 | small256)
    ENABLED_SP_SMALL=yes
    ENABLED_SP_ECC=yes
    ENABLED_SP_EC_256=yes
    ;;
  ec256 | p256 | 256)
    ENABLED_SP_ECC=yes
    ENABLED_SP_EC_256=yes
    ;;
  smallec384 | smallp384 | small384)
    ENABLED_SP_SMALL=yes
    ENABLED_SP_ECC=yes
    ENABLED_SP_EC_384=yes
    ;;
  ec384 | p384 | 384)
    ENABLED_SP_ECC=yes
    ENABLED_SP_EC_384=yes
    ;;
  smallec521 | smallp521 | small521)
    ENABLED_SP_SMALL=yes
    ENABLED_SP_ECC=yes
    ENABLED_SP_EC_521=yes
    ;;
  ec521 | p521 | 521)
    ENABLED_SP_ECC=yes
    ENABLED_SP_EC_521=yes
    ;;
  smallec1024 | smallp1024 | small1024)
    ENABLED_SP_ECC=yes
    ENABLED_SP_SMALL=yes
    ENABLED_SP_SAKKE_1024=yes
    ;;
  ec1024 | p1024 | 1024)
    ENABLED_SP_ECC=yes
    ENABLED_SP_SAKKE_1024=yes
    ;;

  small2048)
    ENABLED_SP_SMALL=yes
    ENABLED_SP_RSA=yes
    ENABLED_SP_DH=yes
    ENABLED_SP_FF_2048=yes
    ;;
  2048)
    ENABLED_SP_RSA=yes
    ENABLED_SP_DH=yes
    ENABLED_SP_FF_2048=yes
    ;;
  smallrsa2048)
    ENABLED_SP_SMALL=yes
    ENABLED_SP_RSA=yes
    ENABLED_SP_FF_2048=yes
    ;;
  rsa2048)
    ENABLED_SP_RSA=yes
    ENABLED_SP_FF_2048=yes
    ;;

  small3072)
    ENABLED_SP_SMALL=yes
    ENABLED_SP_RSA=yes
    ENABLED_SP_DH=yes
    ENABLED_SP_FF_3072=yes
    ;;
  3072)
    ENABLED_SP_RSA=yes
    ENABLED_SP_DH=yes
    ENABLED_SP_FF_3072=yes
    ;;
  smallrsa3072)
    ENABLED_SP_SMALL=yes
    ENABLED_SP_RSA=yes
    ENABLED_SP_FF_3072=yes
    ;;
  rsa3072)
    ENABLED_SP_RSA=yes
    ENABLED_SP_FF_3072=yes
    ;;

  small4096)
    ENABLED_SP_SMALL=yes
    ENABLED_SP_RSA=yes
    ENABLED_SP_DH=yes
    ENABLED_SP_FF_4096=yes
    ;;
  4096 | +4096)
    ENABLED_SP_RSA=yes
    ENABLED_SP_DH=yes
    ENABLED_SP_FF_4096=yes
    ;;
  smallrsa4096)
    ENABLED_SP_SMALL=yes
    ENABLED_SP_RSA=yes
    ENABLED_SP_FF_4096=yes
    ;;
  rsa4096)
    ENABLED_SP_RSA=yes
    ENABLED_SP_FF_4096=yes
    ;;

  smallstack)
    ENABLED_SP_SMALL_STACK=yes
    ;;

  nomalloc)
    ENABLED_SP_NO_MALLOC=yes
    ;;

  nonblock)
    # Requires small and no malloc
    ENABLED_SP_NONBLOCK=yes
    ENABLED_SP_NO_MALLOC=yes
    ENABLED_SP_SMALL=yes
    ;;

  asm)
    ENABLED_SP_ASM=yes
    ;;

  noasm)
    ENABLED_SP_ASM=no
    ;;

  *)
    AC_MSG_ERROR([Invalid choice of Single Precision length in bits [256, 384, 521, 1024, 2048, 3072, 4096]: $ENABLED_SP.])
    break;;
  esac
done

ENABLED_SP_LINE="$ENABLE_SP"
ENABLED_SP=no
if test "$ENABLED_RSA" = "yes" && test "$ENABLED_SP_RSA" = "yes"; then
    ENABLED_SP=yes
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_HAVE_SP_RSA"
    AM_CCASFLAGS="$AM_CCASFLAGS -DWOLFSSL_HAVE_SP_RSA"
fi
if test "$ENABLED_DH" != "no" && test "$ENABLED_SP_DH" = "yes"; then
    ENABLED_SP=yes
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_HAVE_SP_DH"
    AM_CCASFLAGS="$AM_CCASFLAGS -DWOLFSSL_HAVE_SP_DH"
fi
if test "$ENABLED_SP_RSA" = "yes" || test "$ENABLED_SP_DH" = "yes"; then
    if test "$ENABLED_SP_FF_2048" = "no"; then
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SP_NO_2048"
        AM_CCASFLAGS="$AM_CCASFLAGS -DWOLFSSL_SP_NO_2048"
    fi
    if test "$ENABLED_SP_FF_3072" = "no"; then
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SP_NO_3072"
        AM_CCASFLAGS="$AM_CCASFLAGS -DWOLFSSL_SP_NO_3072"
    fi
    if test "$ENABLED_SP_FF_4096" = "yes"; then
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SP_4096"
        AM_CCASFLAGS="$AM_CCASFLAGS -DWOLFSSL_SP_4096"
    fi

    case $host_cpu in
    *x86_64* | *aarch64* | *amd64*)
      AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SP_LARGE_CODE"
      ;;
    *)
      ;;
    esac
fi
if test "$ENABLED_ECC" != "no" && test "$ENABLED_SP_ECC" = "yes"; then
    ENABLED_SP=yes
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_HAVE_SP_ECC"
    AM_CCASFLAGS="$AM_CCASFLAGS -DWOLFSSL_HAVE_SP_ECC"
    if test "$ENABLED_SP_EC_256" = "no"; then
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SP_NO_256"
        AM_CCASFLAGS="$AM_CCASFLAGS -DWOLFSSL_SP_NO_256"
    fi
    if test "$ENABLED_SP_EC_384" = "yes"; then
        AM_CFLAGS="$AM_CFLAGS -DHAVE_ECC384 -DWOLFSSL_SP_384"
        AM_CCASFLAGS="$AM_CCASFLAGS -DWOLFSSL_SP_384"
    fi
    if test "$ENABLED_SP_EC_521" = "yes"; then
        AM_CFLAGS="$AM_CFLAGS -DHAVE_ECC521 -DWOLFSSL_SP_521"
        AM_CCASFLAGS="$AM_CCASFLAGS -DWOLFSSL_SP_521"
    fi
    if test "$ENABLED_SP_SAKKE_1024" = "yes"; then
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SP_1024"
        AM_CCASFLAGS="$AM_CCASFLAGS -DWOLFSSL_SP_1024"
    fi
fi
if test "$ENABLED_SP_SMALL" = "yes"; then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SP_SMALL"
    AM_CCASFLAGS="$AM_CCASFLAGS -DWOLFSSL_SP_SMALL"
fi
if test "$ENABLED_SP_SMALL_STACK" = "yes"; then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SP_SMALL_STACK"
    AM_CCASFLAGS="$AM_CCASFLAGS -DWOLFSSL_SP_SMALL_STACK"
fi
if test "$ENABLED_SP_NO_MALLOC" = "yes"; then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SP_NO_MALLOC"
    AM_CCASFLAGS="$AM_CCASFLAGS -DWOLFSSL_SP_NO_MALLOC"
fi
if test "$ENABLED_SP_NONBLOCK" = "yes"; then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SP_NONBLOCK"
    AM_CCASFLAGS="$AM_CCASFLAGS -DWOLFSSL_SP_NONBLOCK"
fi

if test "$ENABLED_SP_MATH" = "yes"; then
    if test "$ENABLED_SP" = "no"; then
        if test "$ENABLED_RSA" != "no"; then
            AC_MSG_ERROR([Must have SP enabled with SP math for RSA: --enable-sp])
        fi
        if test "$ENABLED_DH" != "no"; then
            AC_MSG_ERROR([Must have SP enabled with SP math for DH: --enable-sp])
        fi
        if test "$ENABLED_ECC" != "no"; then
            AC_MSG_ERROR([Must have SP enabled with SP math for ECC: --enable-sp])
        fi
    fi
    if test "$ENABLED_ECCCUSTCURVES" != "no"; then
        AC_MSG_ERROR([Cannot use single precision math and custom curves])
    fi
    if test "$ENABLED_DSA" = "yes"; then
        AC_MSG_ERROR([Cannot use single precision math and DSA])
    fi
    if test "$ENABLED_SRP" = "yes"; then
        AC_MSG_ERROR([Cannot use single precision math and SRP])
    fi
    if test "$ENABLED_SP_RSA" = "no" && test "$ENABLED_RSA" = "yes"; then
        AC_MSG_ERROR([Cannot use RSA single precision only math and RSA])
    fi
    if test "$ENABLED_SP_DH" = "no" && test "$ENABLED_DH" != "no"; then
        AC_MSG_ERROR([Cannot use DH single precision only math and DH])
    fi
fi

for v in `echo $ENABLED_SP_MATH_ALL | tr "," " "`
do
  case $v in
  yes | no)
    ;;
  small)
    ENABLED_SP_MATH_ALL="yes"
    ENABLED_SP_SMALL="yes"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SP_SMALL"
    AM_CCASFLAGS="$AM_CCASFLAGS -DWOLFSSL_SP_SMALL"
    ;;
  huge)
    ENABLED_SP_MATH_ALL="yes"
    ENABLED_FASTHUGEMATH="yes"
    AM_CFLAGS="$AM_CFLAGS $OPTIMIZE_HUGE_CFLAGS"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SP_INT_LARGE_COMBA"
    ;;
  256 | 384 | 521 | 1024 | 2048 | 3072 | 4096)
    if test -z "$DEFAULT_MAX_CLASSIC_ASYM_KEY_BITS" -o "$DEFAULT_MAX_CLASSIC_ASYM_KEY_BITS" -lt "$v"
    then
        DEFAULT_MAX_CLASSIC_ASYM_KEY_BITS="$v"
    fi
    ENABLED_SP_MATH_ALL="yes"
    ;;
  nomalloc)
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SP_NO_MALLOC"
    ENABLED_SP_MATH_ALL="yes"
    ;;
  neg)
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SP_INT_NEGATIVE"
    ENABLED_SP_MATH_ALL="yes"
    ;;
  *)
    AC_MSG_ERROR([Support SP int bit sizes: 256, 384, 521, 1024, 2048, 3072, 4096. $ENABLED_SP_MATH_ALL not supported])
    ;;
  esac
done

AC_ARG_WITH([arm-target],
    [AS_HELP_STRING([--with-arm-target=x],[x can be "thumb" or "cortex"])],
    [ARM_TARGET="$withval"],
    [ARM_TARGET=''])

if test "$ENABLED_SP_MATH_ALL" = "yes" && test "$ENABLED_ASM" != "no"; then

  ENABLED_FASTMATH="no"
  ENABLED_SLOWMATH="no"

  AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SP_MATH_ALL"

  case $host_cpu in
  *x86_64* | *amd64*)
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SP_X86_64"
    ;;
  *x86*)
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SP_X86"
    ;;
  *aarch64*)
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SP_ARM64 -DWOLFSSL_AARCH64_BUILD"
    ;;
  *arm*)
    if test "$host_alias" = "thumb" || test "$ARM_TARGET" = "thumb"; then
      AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SP_ARM_THUMB"
    else
      if test "$host_alias" = "cortex" || test "$ARM_TARGET" = "cortex"; then
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SP_ARM_CORTEX_M"
      else
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SP_ARM32"
      fi
    fi
    ;;
  *ppc64* | *powerpc64*)
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SP_PPC64"
    ;;
  *ppc* | *powerpc*)
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SP_PPC"
    ;;
  *mips64*)
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SP_MIPS64"
    ;;
  *mips*)
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SP_MIPS"
    ;;
  *riscv32*)
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SP_RISCV32"
    ;;
  *riscv64*)
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SP_RISCV64"
    ;;
  *s390x*)
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SP_S390X"
    ;;
  esac

  if test "$ENABLED_FIPS" != "no" || test "$SELFTEST_VERSION" != "none"; then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SP_INT_NEGATIVE"
  fi
fi


if test "$ENABLED_SP_ASM" = "yes" && test "$ENABLED_SP" = "yes"; then
  if test "$ENABLED_SP_NONBLOCK" = "yes"; then
    AC_MSG_ERROR([SP non-blocking not supported with sp-asm])
  fi
  if test "$ENABLED_ASM" = "no"; then
    AC_MSG_ERROR([Assembly code turned off])
  fi

  AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SP_ASM"
  AM_CCASFLAGS="$AM_CCASFLAGS -DWOLFSSL_SP_ASM"
  case $host_cpu in
  *aarch64*)
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SP_ARM64_ASM"
    AM_CCASFLAGS="$AM_CCASFLAGS -DWOLFSSL_SP_ARM64_ASM"
    ENABLED_SP_ARM64_ASM=yes
    ;;
  *arm*)
    if test "$host_alias" = "thumb" || test "$ARM_TARGET" = "thumb"; then
      AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SP_ARM_THUMB_ASM"
      AM_CCASFLAGS="$AM_CCASFLAGS -DWOLFSSL_SP_ARM_THUMB_ASM"
      ENABLED_SP_ARM_THUMB_ASM=yes
    else
      if test "$host_alias" = "cortex" || test "$ARM_TARGET" = "cortex"; then
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SP_ARM_CORTEX_M_ASM"
        AM_CCASFLAGS="$AM_CCASFLAGS -DWOLFSSL_SP_ARM_CORTEX_M_ASM"
        ENABLED_SP_ARM_CORTEX_ASM=yes
      else
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SP_ARM32_ASM"
        AM_CCASFLAGS="$AM_CCASFLAGS -DWOLFSSL_SP_ARM32_ASM"
        ENABLED_SP_ARM32_ASM=yes
      fi
    fi
    ;;
  *x86_64* | *amd64*)
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SP_X86_64_ASM"
    AM_CCASFLAGS="$AM_CCASFLAGS -DWOLFSSL_SP_X86_64_ASM"
    ENABLED_SP_X86_64_ASM=yes
    ;;
  *)
    AC_MSG_ERROR([ASM not available for CPU. Supported CPUs: x86_64, aarch64, arm])
    ;;
  esac

fi


if test "$ENABLED_SP_MATH" = "yes"; then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SP_MATH"
fi

################################################################################
# End - Single Precision option handling                                       #
################################################################################

# Fast RSA using Intel IPP
ippdir="${srcdir}/IPP"
ipplib="lib" # if autoconf guesses 32bit system changes lib directory
fastRSA_found=no
abs_path=`pwd`

# set up variables used
IPPLIBS=
IPPHEADERS=
IPPLINK=

AC_ARG_ENABLE([fast-rsa],
    [AS_HELP_STRING([--enable-fast-rsa],[Enable RSA using Intel IPP (default: disabled)])],
    [ ENABLED_FAST_RSA=$enableval ],
    [ ENABLED_FAST_RSA=no ],
    )

# Fast RSA does not support RSA-PSS
if test "$ENABLED_RSAPSS" = "yes"; then
    ENABLED_FAST_RSA=no
fi

if test "$ENABLED_USER_RSA" = "no" && test "$ENABLED_FIPS" = "no"; then

    if test "$ac_cv_sizeof_long" = "4" && test "$ac_cv_sizeof_long_long" = "8"; then
        ipplib="lib_32" # 32 bit OS detected
    fi

#   Use static IPP Libraries
if test "$enable_shared" = "no" && test "$ENABLED_FAST_RSA" = "yes"; then
    case $host_os in
    *darwin*)
        ipplib="$ipplib/mac_static"
        AC_MSG_ERROR([Issue with static linking to libippcp.a on Mac.
                Dynamic IPP libraries supported on Mac])
        break;;

    *linux*)
        ipplib="$ipplib/linux_static"
        break;;
    *)
        ENABLED_FAST_RSA=no
    esac

    if test -e $srcdir/IPP/$ipplib/libippcore.a && test -e $srcdir/IPP/$ipplib/libippcp.a
    then
        :
    else
        ENABLED_FAST_RSA=no
    fi
    AC_CHECK_HEADERS([IPP/include/ipp.h IPP/include/ippcp.h], [AM_CPPFLAGS="-I$srcdir/IPP/include $AM_CPPFLAGS"], [ENABLED_FAST_RSA=no])
    LIB_STATIC_ADD="$srcdir/IPP/$ipplib/libippcp.a $srcdir/IPP/$ipplib/libippcore.a $LIB_STATIC_ADD"
    if test "$ENABLED_FAST_RSA" = "no"; then
        AC_MSG_ERROR([Could not find fast rsa libraries])
    fi
else

#   Check for and use bundled IPP libraries
if test "$ENABLED_FAST_RSA" = "yes"; then
    AC_MSG_NOTICE([Using local IPP crypto library])

    AC_CHECK_HEADER([$abs_path/IPP/include/ippcp.h],
    [
        # build and default locations on linux and mac
        STORE_LDFLAGS=${LDFLAGS}
        STORE_CPPFLAGS=${CPPFLAGS}

        # using LDFLAGS instead of AM_ temporarily to test link to library
        LDFLAGS="-L$ippdir/$ipplib -lippcp -lippcore"
        CPPFLAGS="-I$ippdir/include"
        AC_CHECK_HEADERS([ippcp.h], [AC_CHECK_LIB([ippcp], [ippsRSAEncrypt_PKCSv15], [fastRSA_found=yes], [fastRSA_found=no])], [fastRSA_found=no])
        name="$ippdir/$ipplib/libippcp"
        case $host_os in
        *darwin*)
            # check file existence and conditionally set variables
            if test -e $abs_path/IPP/$ipplib/libippcp.dylib
            then
                IPPLIBS="${name}.dylib ${name}-9.0.dylib ${name}e9-9.0.dylib ${name}g9-9.0.dylib ${name}h9-9.0.dylib ${name}k0-9.0.dylib ${name}l9-9.0.dylib ${name}n8-9.0.dylib ${name}p8-9.0.dylib ${name}s8-9.0.dylib ${name}y8-9.0.dylib IPP/lib/libippcore.dylib IPP/lib/libippcore-9.0.dylib"
                IPPLINK="mkdir -p src/.libs && ln -f ${name}.dylib src/.libs/libippcp.dylib && ln -f ${srcdir}/${name}-9.0.dylib src/.libs/libippcp-9.0.dylib && ln -f ${srcdir}/${name}e9-9.0.dylib src/.libs/libippcpe9-9.0.dylib && ln -f ${srcdir}/${name}g9-9.0.dylib src/.libs/libippcpg9-9.0.dylib && ln -f ${srcdir}/${name}h9-9.0.dylib src/.libs/libippcph9-9.0.dylib && ln -f ${srcdir}/${name}k0-9.0.dylib src/.libs/libippcpk0-9.0.dylib && ln -f ${srcdir}/${name}l9-9.0.dylib src/.libs/libippcpl9-9.0.dylib && ln -f ${srcdir}/${name}n8-9.0.dylib src/.libs/libippcpn8-9.0.dylib && ln -f ${srcdir}/${name}p8-9.0.dylib src/.libs/libippcpp8-9.0.dylib && ln -f ${srcdir}/${name}s8-9.0.dylib src/.libs/libippcps8-9.0.dylib && ln -f ${srcdir}/${name}y8-9.0.dylib src/.libs/libippcpy8-9.0.dylib && ln -f ${srcdir}/IPP/lib/libippcore.dylib src/.libs/libippcore.dylib && ln -f ${srcdir}/IPP/lib/libippcore-9.0.dylib src/.libs/libippcore-9.0.dylib"
            else
                fastRSA_found=no
            fi
            break;;

        *linux*)
            # check file existence and conditionally set variables
            if test -e $abs_path/IPP/$ipplib/libippcp.so.9.0
            then
                if test "$ac_cv_sizeof_long" = "4" && test "$ac_cv_sizeof_long_long" = "8"; then
                    IPPLIBS="${name}.so.9.0 ${name}g9.so.9.0 ${name}h9.so.9.0 ${name}p8.so.9.0 ${name}px.so.9.0 ${name}s8.so.9.0 ${name}.so ${name}w7.so.9.0 IPP/$ipplib/libippcore.so IPP/$ipplib/libippcore.so.9.0"
                    IPPLINK="mkdir -p src/.libs && ln -f ${name}.so.9.0 src/.libs/libippcp.so.9.0 && ln -f ${name}g9.so.9.0 src/.libs/libippcpg9.so.9.0 && ln -f ${name}h9.so.9.0 src/.libs/libippcph9.so.9.0 && ln -f ${name}p8.so.9.0 src/.libs/libippcpp8.so.9.0 && ln -f ${name}px.so.9.0 src/.libs/libippcppx.so.9.0 && ln -f ${name}s8.so.9.0 src/.libs/libippcps8.so.9.0 && ln -f ${name}.so src/.libs/libippcp.so && ln -f ${name}w7.so.9.0 src/.libs/libippcpw7.so.9.0 && ln -f IPP/$ipplib/libippcore.so src/.libs/libippcore.so && ln -f IPP/$ipplib/libippcore.so.9.0 src/.libs/libippcore.so.9.0"
                else
                    IPPLIBS="${name}.so.9.0 ${name}e9.so.9.0 ${name}k0.so.9.0 ${name}l9.so.9.0 ${name}m7.so.9.0 ${name}mx.so.9.0 ${name}.so ${name}n8.so.9.0 ${name}y8.so.9.0 IPP/lib/libippcore.so IPP/lib/libippcore.so.9.0"
                    IPPLINK="mkdir -p src/.libs && ln -f ${name}.so.9.0 src/.libs/libippcp.so.9.0 && ln -f ${name}e9.so.9.0 src/.libs/libippcpe9.so.9.0 && ln -f ${name}k0.so.9.0 src/.libs/libippcpk0.so.9.0 && ln -f ${name}l9.so.9.0 src/.libs/libippcpl9.so.9.0 && ln -f ${name}m7.so.9.0 src/.libs/libippcpm7.so.9.0 && ln -f ${name}mx.so.9.0 src/.libs/libippcpmx.so.9.0 && ln -f ${name}.so src/.libs/libippcp.so && ln -f ${name}n8.so.9.0 src/.libs/libippcpn8.so.9.0 && ln -f ${name}y8.so.9.0 src/.libs/libippcpy8.so.9.0 && ln -f IPP/lib/libippcore.so src/.libs/libippcore.so && ln -f IPP/lib/libippcore.so.9.0 src/.libs/libippcore.so.9.0"
                fi
            else
                fastRSA_found=no
            fi
            break;;
        *)
            fastRSA_found=no
        esac

        if test "$fastRSA_found" = "yes"; then
            # was successful so add tested LDFLAGS to AM_ flags
            AM_LDFLAGS="${AM_LDFLAGS} ${LDFLAGS}"
            AM_CPPFLAGS="${AM_CPPFLAGS} ${CPPFLAGS}"
            IPPHEADERS="${srcdir}/IPP/include/*.h"
        fi

        # restore LDFLAGS to user set
        LDFLAGS=${STORE_LDFLAGS}
        CPPFLAGS=${STORE_CPPFLAGS}
    ], [fastRSA_found=no])
fi

# Don't cache the result so it can be checked
AS_UNSET([ac_cv_header_ippcp_h])
AS_UNSET([ac_cv_header_ipp_h])
AS_UNSET([ac_cv_lib_ippcp_ippsRSAEncrypt_PKCSv15]);

#   Check link and see if user has pre-existing IPP Libraries if not using local
if test "$ENABLED_FAST_RSA" = "yes" && test "$fastRSA_found" = "no"; then
    AC_MSG_NOTICE([Checking if IPP crypto library installed])
    AC_CHECK_HEADER([ippcp.h], [AC_CHECK_LIB([ippcp], [ippsRSAEncrypt_PKCSv15],
        [
            fastRSA_found=yes
            AM_LDFLAGS="${AM_LDFLAGS} -lippcore -lippcp"
        ], [ fastRSA_found=no])
    ], [fastRSA_found=no])

    # Error out on not finding libraries
    if test "$fastRSA_found" = "no"; then
        AC_MSG_ERROR([Could not find fast rsa libraries])
    fi
fi
fi # end of if for shared library
else # if user rsa is set than do not use fast rsa option
    if test "$ENABLED_FAST_RSA" = "yes"; then
        AC_MSG_ERROR([Could not use fast rsa libraries with user crypto or fips])
    fi
fi # end of if for user rsa crypto or fips

#   End result of checking for IPP Libraries
AC_MSG_CHECKING([for fast RSA])
if test "$ENABLED_FAST_RSA" = "yes"; then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_FAST_RSA -DHAVE_USER_RSA"
    # add in user crypto header that uses Intel IPP
    AM_CPPFLAGS="$AM_CPPFLAGS -I$srcdir/wolfcrypt/user-crypto/include"
    if test "$enable_shared" = "yes"; then
        LIBS="$LIBS -lippcore -lippcp"
        LIB_ADD="-lippcp -lippcore $LIB_ADD"
    else
        LIB_ADD="$srcdir/IPP/$ipplib/libippcp.a $srcdir/IPP/$ipplib/libippcore.a $LIB_ADD"
    fi
    AC_MSG_RESULT([yes])
else
    AC_MSG_RESULT([no])
fi

AC_SUBST([IPPLIBS])
AC_SUBST([IPPHEADERS])
AC_SUBST([IPPLINK])


# static memory use
AC_ARG_ENABLE([staticmemory],
    [AS_HELP_STRING([--enable-staticmemory],[Enable static memory use (default: disabled)])],
    [ ENABLED_STATICMEMORY=$enableval ],
    [ ENABLED_STATICMEMORY=no ]
    )

if test "x$ENABLED_STATICMEMORY" = "xyes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_STATIC_MEMORY"

    if test "x$ENABLED_HEAPMATH" = "xyes"
    then
        AC_MSG_ERROR([please use --enable-fastmath if enabling staticmemory.])
    fi
    if test "$ENABLED_LOWRESOURCE" = "yes" && test "$ENABLED_RSA" = "no"
    then
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_STATIC_MEMORY_SMALL"
    fi
fi


# microchip api
AC_ARG_ENABLE([mcapi],
    [AS_HELP_STRING([--enable-mcapi],[Enable Microchip API (default: disabled)])],
    [ ENABLED_MCAPI=$enableval ],
    [ ENABLED_MCAPI=no ]
    )

if test "$ENABLED_MCAPI" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_MCAPI"
    if test "x$ENABLED_AESCTR" != "xyes"
    then
        # These flags are already implied by --enable-aesctr
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_AES_COUNTER -DWOLFSSL_AES_DIRECT"
    fi
    if test "x$ENABLED_AESGCM" != "xyes" && test "x$ENABLED_AESGCM" != "xno"
    then
        # Use the smaller object size implementation
        ENABLED_AESGCM=yes
    fi
fi

if test "$ENABLED_MCAPI" = "yes" && test "$ENABLED_SHA512" = "no"
then
    AC_MSG_ERROR([please enable sha512 if enabling mcapi.])
fi

if test "$ENABLED_MCAPI" = "yes" && test "$ENABLED_ECC" = "no"
then
    AC_MSG_ERROR([please enable ecc if enabling mcapi.])
fi

if test "$ENABLED_MCAPI" = "yes" && test "$ENABLED_LIBZ" = "no"
then
    AC_MSG_ERROR([please use --with-libz if enabling mcapi.])
fi

# Asynchronous Crypto
AC_ARG_ENABLE([asynccrypt],
    [AS_HELP_STRING([--enable-asynccrypt],[Enable Asynchronous Crypto (default: disabled)])],
    [ ENABLED_ASYNCCRYPT=$enableval ],
    [ ENABLED_ASYNCCRYPT=no ]
    )

# Asynchronous crypto using software (i.e. not hardware). Required for
# non-blocking crypto with TLS/DTLS.
AC_ARG_ENABLE([asynccrypt-sw],
    [AS_HELP_STRING([--enable-asynccrypt-sw],[Enable asynchronous software-based crypto (default: disabled)])],
    [ ENABLED_ASYNCCRYPT_SW=$enableval ],
    [ ENABLED_ASYNCCRYPT_SW=no ]
    )
if test "$ENABLED_ASYNCCRYPT_SW" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_ASYNC_CRYPT_SW"
    ENABLED_ASYNCCRYPT=yes
fi

if test "$ENABLED_ASYNCCRYPT" = "yes"
then
    if ! test -f ${srcdir}/wolfcrypt/src/async.c || ! test -f ${srcdir}/wolfssl/wolfcrypt/async.h
    then
        AC_MSG_ERROR([--enable-asynccrypt requested, but WOLFSSL_ASYNC_CRYPT source files are missing.])
    fi

    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_ASYNC_CRYPT -DHAVE_WOLF_EVENT -DHAVE_WOLF_BIGINT -DWOLFSSL_NO_HASH_RAW"

    # If no async backend (hardware or software) has been explicitly enabled,
    # use the software backend for testing.
    if test "x$ENABLED_CAVIUM" = "xno" && test "x$ENABLED_INTEL_QA" = "xno" &&
       test "x$ENABLED_ASYNCCRYPT_SW" = "xno"
    then
        # Async threading is Linux specific
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_ASYNC_CRYPT_SW"
    fi
fi


# check for async if using Intel QuckAssist or Cavium
if test "x$ENABLED_INTEL_QA" = "xyes" || test "x$ENABLED_CAVIUM" = "xyes" ; then
    if test "x$ENABLED_ASYNCCRYPT" = "xno" ; then
        AC_MSG_ERROR([Please enable asynchronous support using --enable-asynccrypt])
    fi
fi


# Asynchronous threading
AC_ARG_ENABLE([asyncthreads],
    [AS_HELP_STRING([--enable-asyncthreads],[Enable Asynchronous Threading (default: enabled)])],
    [ ENABLED_ASYNCTHREADS=$enableval ],
    [ ENABLED_ASYNCTHREADS=yes ]
    )

if test "$ENABLED_ASYNCCRYPT" = "yes" && test "$ENABLED_ASYNCTHREADS" = "yes"
then
    AX_PTHREAD([ENABLED_ASYNCTHREADS=yes],[ENABLED_ASYNCTHREADS=no])
else
    ENABLED_ASYNCTHREADS=no
fi

if test "$ENABLED_ASYNCTHREADS" = "yes"
then
    LIB_ADD="-lpthread $LIB_ADD"
    AM_CFLAGS="$AM_CFLAGS -D_GNU_SOURCE"
else
    AM_CFLAGS="$AM_CFLAGS -DWC_NO_ASYNC_THREADING"
fi


# cryptodev is old name, replaced with cryptocb
AC_ARG_ENABLE([cryptodev],
    [AS_HELP_STRING([--enable-cryptodev],[DEPRECATED, use cryptocb instead])],
    [ ENABLED_CRYPTOCB=$enableval ],[ ENABLED_CRYPTOCB=no ])

# Support for crypto callbacks
AC_ARG_ENABLE([cryptocb],
    [AS_HELP_STRING([--enable-cryptocb],[Enable crypto callbacks (default: disabled)])],
    [ ENABLED_CRYPTOCB=$enableval ],
    [ ENABLED_CRYPTOCB=no ]
    )

if test "x$ENABLED_PKCS11" = "xyes" || test "x$ENABLED_WOLFTPM" = "xyes" || test "$ENABLED_CAAM" != "no"
then
    ENABLED_CRYPTOCB=yes
fi
if test "$ENABLED_CRYPTOCB" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLF_CRYPTO_CB"
fi


# Session Export
AC_ARG_ENABLE([sessionexport],
    [AS_HELP_STRING([--enable-sessionexport],[Enable export and import of sessions (default: disabled)])],
    [ ENABLED_SESSIONEXPORT=$enableval ],
    [ ENABLED_SESSIONEXPORT=no ]
    )

if test "$ENABLED_SESSIONEXPORT" = "yes" ||
   test "$ENABLED_SESSIONEXPORT" = "nopeer"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SESSION_EXPORT"

    if test "$ENABLED_SESSIONEXPORT" = "nopeer"
    then
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SESSION_EXPORT_NOPEER"
    fi
fi


# AES key wrap
AC_ARG_ENABLE([aeskeywrap],
    [AS_HELP_STRING([--enable-aeskeywrap],[Enable AES key wrap support (default: disabled)])],
    [ ENABLED_AESKEYWRAP=$enableval ],
    [ ENABLED_AESKEYWRAP=no ]
    )

if test "$ENABLED_WPAS" != "no" && test "$ENABLED_FIPS" = "no"
then
    ENABLED_AESKEYWRAP="yes"
fi

if test "$ENABLED_AESKEYWRAP" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_AES_KEYWRAP -DWOLFSSL_AES_DIRECT"
fi


# Old name support for backwards compatibility
AC_ARG_ENABLE([oldnames],
    [AS_HELP_STRING([--enable-oldnames],[Keep backwards compat with old names (default: enabled)])],
    [ ENABLED_OLDNAMES=$enableval ],
    [ ENABLED_OLDNAMES=yes ]
    )

if test "x$ENABLED_OLDNAMES" = "xno" && test "x$ENABLED_OPENSSLCOEXIST" = "xno"
then
    AM_CFLAGS="$AM_CFLAGS -DNO_OLD_RNGNAME -DNO_OLD_WC_NAMES -DNO_OLD_SSL_NAMES"
    AM_CFLAGS="$AM_CFLAGS -DNO_OLD_SHA_NAMES -DNO_OLD_MD5_NAME"
fi


# Memory Tests
AC_ARG_ENABLE([memtest],
    [AS_HELP_STRING([--enable-memtest],[Memory testing option, for internal use (default: disabled)])],
    [ ENABLED_MEMTEST=$enableval ],
    [ ENABLED_MEMTEST=no ]
    )

if test "x$ENABLED_MEMTEST" != "xno"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_TRACK_MEMORY -DWOLFSSL_DEBUG_MEMORY"
fi

if test "x$ENABLED_MEMTEST" = "xfail"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_FORCE_MALLOC_FAIL_TEST"
fi

# Enable hash flags support
# Hash flags are useful for runtime options such as SHA3 KECCAK256 selection
AC_ARG_ENABLE([hashflags],
    [AS_HELP_STRING([--enable-hashflags],[Enable support for hash flags (default: disabled)])],
    [ ENABLED_HASHFLAGS=$enableval ],
    [ ENABLED_HASHFLAGS=no ]
    )

if test "x$ENABLED_HASHFLAGS" != "xno"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_HASH_FLAGS"
fi

# Support for enabling setting default DH parameters in TLS
AC_ARG_ENABLE([defaultdhparams],
    [AS_HELP_STRING([--enable-defaultdhparams],[Enables option for default dh parameters (default: disabled)])],
    [ ENABLED_DHDEFAULTPARAMS=$enableval ],
    [ ENABLED_DHDEFAULTPARAMS=yes ]
    )
if test "x$ENABLED_DH" = "xyes" && test "x$ENABLED_DHDEFAULTPARAMS" = "xyes" && test "x$ENABLED_QT" != "xyes"
then
    ENABLED_DHDEFAULTPARAMS=yes
    AM_CFLAGS="$AM_CFLAGS -DHAVE_DH_DEFAULT_PARAMS"
fi


AC_ARG_WITH([max-rsa-bits],
    [AS_HELP_STRING([--with-max-rsa-bits=number],[number of bits to support for RSA, DH, and DSA keys])],
    [WITH_MAX_CLASSIC_ASYM_KEY_BITS=$withval],
    [WITH_MAX_CLASSIC_ASYM_KEY_BITS="$DEFAULT_MAX_CLASSIC_ASYM_KEY_BITS"])

if test -n "$WITH_MAX_CLASSIC_ASYM_KEY_BITS"; then
   if test "$WITH_MAX_CLASSIC_ASYM_KEY_BITS" -lt 1024 -o "$WITH_MAX_CLASSIC_ASYM_KEY_BITS" -gt 16384; then
       AC_MSG_ERROR([--with-max-rsa-bits argument must be between 1024 and 16384 inclusive])
   fi
   if test "$ENABLED_FIPS" = "no"
   then
      AM_CFLAGS="$AM_CFLAGS -DRSA_MAX_SIZE=$WITH_MAX_CLASSIC_ASYM_KEY_BITS"
   fi
   MPI_MAX_KEY_BITS=$WITH_MAX_CLASSIC_ASYM_KEY_BITS
fi

AC_ARG_WITH([max-ecc-bits],
    [AS_HELP_STRING([--with-max-ecc-bits=number],[number of bits to support for ECC algorithms])],
    [WITH_MAX_ECC_BITS=$withval],
    [WITH_MAX_ECC_BITS="$DEFAULT_MAX_ECC_BITS"])

if test -n "$WITH_MAX_ECC_BITS"; then
   if test "$WITH_MAX_ECC_BITS" -lt 112 -o "$WITH_MAX_ECC_BITS" -gt 1024; then
       AC_MSG_ERROR([--with-max-ecc-bits argument must be between 112 and 1024 inclusive])
   fi
   AM_CFLAGS="$AM_CFLAGS -DMAX_ECC_BITS=$WITH_MAX_ECC_BITS"
fi

if test -n "$MPI_MAX_KEY_BITS" -o -n "$WITH_MAX_ECC_BITS"; then
   if test -n "$MAX_MPI_KEY_BITS" -a -n "$WITH_MAX_ECC_BITS"; then
       if test "$MAX_MPI_KEY_BITS" -lt "$WITH_MAX_ECC_BITS"; then
           MPI_MAX_KEY_BITS="$WITH_MAX_ECC_BITS"
       fi
   elif test -n "$WITH_MAX_ECC_BITS"; then
       MPI_MAX_KEY_BITS="$WITH_MAX_ECC_BITS"
   fi
   if test "$MPI_MAX_KEY_BITS" -gt 1024; then
       AM_CFLAGS="$AM_CFLAGS -DFP_MAX_BITS=$((MPI_MAX_KEY_BITS * 2)) -DSP_INT_BITS=$MPI_MAX_KEY_BITS"
   fi
fi

# Library Suffix
LIBSUFFIX=""
AC_ARG_WITH([libsuffix],
    [AS_HELP_STRING([--with-libsuffix=SUFFIX],[Library artifact SUFFIX, ie libwolfsslSUFFIX.so])],
    [
        if test "x$withval" != "xno" ; then
            LIBSUFFIX=$withval
        fi
        if test "x$withval" = "xyes" ; then
            AC_MSG_ERROR([Invalid argument to --with-libsuffix, no suffix given])
        fi
    ]
)
AC_SUBST(LIBSUFFIX)

AC_ARG_ENABLE([context-extra-user-data],
    [AS_HELP_STRING([--enable-context-extra-user-data],[Enables option for storing user-defined data in TLS API contexts, with optional argument the number of slots to allocate (default: disabled)])],
    [ ENABLED_EX_DATA=$enableval ],
    [ ENABLED_EX_DATA=no ]
    )
case "$ENABLED_EX_DATA" in
no) ;;
yes) AM_CFLAGS="$AM_CFLAGS -DHAVE_EX_DATA"
     ;;
[[1-9]]|[[1-9]][[0-9]]) AM_CFLAGS="$AM_CFLAGS -DHAVE_EX_DATA -DMAX_EX_DATA=$ENABLED_EX_DATA"
       ;;
*) AC_MSG_ERROR([Invalid argument to --enable-context-extra-user-data -- must be yes, no, or a number from 1 to 99])
   ;;
esac

# IoT-Safe support
AC_ARG_ENABLE([iotsafe],
    [AS_HELP_STRING([--enable-iotsafe],[Enables support for IoT-Safe secure applet (default: disabled)])],
    [ ENABLED_IOTSAFE=$enableval ],
    [ ENABLED_IOTSAFE=no ]
    )

AC_ARG_ENABLE([iotsafe-hwrng],
    [AS_HELP_STRING([--enable-iotsafe-hwrng],[Enables support for IoT-Safe RNG (default: disabled)])],
    [ ENABLED_IOTSAFE_HWRNG=$enableval ],
    [ ENABLED_IOTSAFE_HWRNG=no ]
    )

# Make clean
AC_ARG_ENABLE([makeclean],
    [AS_HELP_STRING([--enable-makeclean], [Enables forced "make clean" at the
    end of configure (default: enabled)])],
    [ ENABLED_MAKECLEAN=$enableval ],
    [ ENABLED_MAKECLEAN=yes ]
    )

# User Settings
AC_ARG_ENABLE([usersettings],
    [AS_HELP_STRING([--enable-usersettings],[Use your own user_settings.h and do not add Makefile CFLAGS (default: disabled)])],
    [ ENABLED_USERSETTINGS=$enableval ],
    [ ENABLED_USERSETTINGS=no ]
    )


# Default optimization CFLAGS enable
AC_ARG_ENABLE([optflags],
    [AS_HELP_STRING([--enable-optflags],[Enable default optimization CFLAGS for the compiler (default: enabled)])],
    [ ENABLED_OPTFLAGS=$enableval ],
    [ ENABLED_OPTFLAGS=yes ]
    )

# Adds functionality to load CA certificates from the operating system.
AC_ARG_ENABLE([sys-ca-certs],
    [AS_HELP_STRING([--enable-sys-ca-certs],[Enable ability to load CA certs from OS (default: enabled)])],
    [ ENABLED_SYS_CA_CERTS=$enableval ],
    [ ENABLED_SYS_CA_CERTS=yes ]
    )

# check if should run the trusted peer certs test
# (for now checking both C_FLAGS and C_EXTRA_FLAGS)
AS_CASE(["$CFLAGS $CPPFLAGS"],[*'WOLFSSL_TRUST_PEER_CERT'*],[ENABLED_TRUSTED_PEER_CERT=yes])


AS_CASE(["$CFLAGS $CPPFLAGS $AM_CFLAGS"],[*'OPENSSL_COMPATIBLE_DEFAULTS'*],
    [ENABLED_OPENSSL_COMPATIBLE_DEFAULTS=yes])
if test "x$ENABLED_OPENSSL_COMPATIBLE_DEFAULTS" = "xyes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_TRUST_PEER_CERT"
    AM_CFLAGS="$AM_CFLAGS -DNO_SESSION_CACHE_REF"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_TLS13_NO_PEEK_HANDSHAKE_DONE"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_ALT_CERT_CHAINS"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_PRIORITIZE_PSK"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_CHECK_ALERT_ON_ERR"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_TICKET_HAVE_ID"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_NO_OCSP_ISSUER_CHECK"
    ENABLED_TRUSTED_PEER_CERT=yes
fi

# determine if we have key validation mechanism
if test "x$ENABLED_ECC" != "xno" || test "x$ENABLED_RSA" = "xyes"
then
    if test "x$ENABLED_ASN" = "xyes"
    then
        ENABLED_PKI="yes"
    fi
fi

# When building for wolfRand, strip out all options to disable everything.
AS_IF([test "x$ENABLED_FIPS" = "xyes" && test "x$FIPS_VERSION" = "xrand"],
[NEW_AM_CFLAGS="-DNO_AES -DNO_DH -DNO_ASN -DNO_RSA -DNO_SHA -DNO_MD5 -DNO_BIG_INT"
for v in $AM_CFLAGS
do
  case $v in
-DHAVE_FFDHE_2048 | -DTFM_TIMING_RESISTANT | -DECC_TIMING_RESISTANT | \
-DWC_RSA_BLINDING | -DHAVE_AESGCM | -DWOLFSSL_SHA512 | -DWOLFSSL_SHA384 | \
-DHAVE_ECC | -DTFM_ECC256 | -DECC_SHAMIR | -DHAVE_TLS_EXTENSIONS | \
-DHAVE_SUPPORTED_CURVES | -DHAVE_EXTENDED_MASTER | -DUSE_FAST_MATH | \
-DWOLFSSL_SHA3)
    AS_ECHO(["ignoring $v"])
    ;;
  *)
    NEW_AM_CFLAGS="$NEW_AM_CFLAGS $v"
    ;;
  esac
done
AM_CFLAGS=$NEW_AM_CFLAGS])

case $host_cpu in
  *arm*)
    if test "$host_alias" = "thumb" || test "$ARM_TARGET" = "thumb"; then
      AM_CFLAGS="$AM_CFLAGS -mthumb -march=armv6"
    else
      if test "$host_alias" = "cortex" || test "$ARM_TARGET" = "cortex"; then
        AM_CFLAGS="$AM_CFLAGS -mcpu=cortex-r5"
      fi
    fi
    ;;
esac

if test "$ENABLED_LOWRESOURCE" = "yes" && test "$ENABLED_ECC" = "yes" && (test "$ENABLED_RSA" = "yes" || test "$ENABLED_DH" = "yes") && (test "$ENABLED_SP_MATH" = "yes" || test "$ENABLED_SP_MATH_ALL" = "yes")
then
    AM_CFLAGS="$AM_CFLAGS -DALT_ECC_SIZE"
fi

################################################################################
# Update ENABLE_* variables                                                    #
################################################################################

if test "x$ENABLED_SYS_CA_CERTS" = "xyes"
then
    if test "x$ENABLED_FILESYSTEM" = "xno"
    then
        ENABLED_SYS_CA_CERTS="no"
    elif test "x$ENABLED_CERTS" = "xno"
    then
        ENABLED_SYS_CA_CERTS="no"
    fi

    case $host_os in
        *darwin*)
            AC_CHECK_HEADERS([Security/SecTrustSettings.h],
                [
                    # For Mac we need these frameworks to load system CA certs
                    LDFLAGS="$LDFLAGS -framework CoreFoundation -framework Security"
                ],
                [
                    AC_MSG_NOTICE([Can't enable system CA certs without Security/SecTrustSettings.h])
                    ENABLED_SYS_CA_CERTS="no"
                ]
            )
            ;;
    esac
fi

if test "x$ENABLED_WOLFCLU" = "xyes"
then
    if test "x$ENABLED_CERTGEN" = "xno"
    then
        ENABLED_CERTGEN="yes"
    fi
    if test "x$ENABLED_CERTREQ" = "xno"
    then
        ENABLED_CERTREQ="yes"
    fi
    if test "x$ENABLED_CERTEXT" = "xno"
    then
        ENABLED_CERTEXT="yes"
    fi

    # Requires md5
    if test "$ENABLED_MD5" = "no"
    then
        ENABLED_MD5="yes"
    fi

    # Requires aesctr
    if test "x$ENABLED_AESCTR" = "xno"
    then
        ENABLED_AESCTR="yes"
    fi

    # Uses key generation
    if test "x$ENABLED_KEYGEN" = "xno"
    then
        ENABLED_KEYGEN="yes"
    fi

    # Uses functions guarded by opensslall
    if test "$ENABLED_OPENSSLALL" = "no"
    then
        ENABLED_OPENSSLALL="yes"
    fi

    # Has option for signing with ED25519
    if test "$ENABLED_ED25519" = "no"
    then
        ENABLED_ED25519=yes
        ENABLED_FEMATH=yes
        ENABLED_GEMATH=yes
        ENABLED_CERTS=yes
    fi

    # Has sha512 hashing
    if test "$ENABLED_SHA512" = "no"
    then
        ENABLED_SHA512="yes"
    fi

    # Has support for DES3 encrypt/decrypt
    if test "$ENABLED_DES3" = "no"
    then
        ENABLED_DES3="yes"
    fi

    # Uses alt name
    ENABLED_ALTNAMES="yes"

    AM_CFLAGS="$AM_CFLAGS -DHAVE_OID_ENCODING -DWOLFSSL_NO_ASN_STRICT"

fi

if test "$ENABLED_STRONGSWAN" = "yes"; then
    if test "$ENABLED_CERTREQ" = "no"; then
        ENABLED_CERTREQ="yes"
    fi

    if test "$ENABLED_OCSP" = "no"; then
        ENABLED_OCSP="yes"
    fi
fi

AS_IF([test "x$ENABLED_MCAPI" = "xyes"],
      [AS_IF([test "x$ENABLED_DES3" = "xno"],[ENABLED_DES3="yes"])])

if test "$ENABLED_OPENSSH" = "yes" || test "$ENABLED_NGINX" = "yes" || \
   test "$ENABLED_SIGNAL" = "yes" || test "$ENABLED_WPAS" = "yes" || \
   test "$ENABLED_FORTRESS" = "yes" || test "$ENABLED_BUMP" = "yes" || \
   test "$ENABLED_OPENSSLALL" = "yes" || \
   test "$ENABLED_LIBWEBSOCKETS" = "yes" || \
   test "x$ENABLED_LIGHTY" = "xyes" || test "$ENABLED_LIBSSH2" = "yes" || \
   test "x$ENABLED_NTP" = "xyes" || test "$ENABLED_RSYSLOG" = "yes" || \
   test "$ENABLED_OPENLDAP" = "yes" || test "$ENABLED_HITCH" = "yes"
then
    ENABLED_OPENSSLEXTRA="yes"
fi

if test "$ENABLED_ED25519" != "no" && test "$ENABLED_32BIT" = "no"
then
    if test "$ENABLED_ED25519" = "small" || test "$ENABLED_LOWRESOURCE" = "yes"
    then
        ENABLED_ED25519_SMALL=yes
        ENABLED_CURVE25519_SMALL=yes
        ENABLED_ED25519=yes
    fi

    ENABLED_FEMATH=yes
    ENABLED_GEMATH=yes
    ENABLED_CERTS=yes
fi

if test "$ENABLED_ED25519" != "no" || test "$ENABLED_ED448" != "no"
then
    ENABLED_CERTS=yes
fi

if test "$ENABLED_MD5" = "yes"
then
    # turn off MD5 if leanpsk or leantls on
    if test "$ENABLED_LEANPSK" = "yes" || test "$ENABLED_LEANTLS" = "yes"
    then
        ENABLED_MD5=no
    fi
fi

if test "x$ENABLED_LEANPSK" = "xyes" || test "x$ENABLED_CERTS" = "xno" || \
   test "x$ENABLED_ASN" = "xno"
then
   ENABLED_CERTS=no
   ENABLED_ASN=no
fi

################################################################################
# Check for build-type conflicts                                               #
################################################################################

AS_IF([test "x$ENABLED_MAXSTRENGTH" = "xyes" && \
       test "x$ENABLED_LEANPSK" = "xyes"],
      [AC_MSG_ERROR([Cannot use Max Strength and Lean PSK at the same time.])])

AS_IF([test "x$ENABLED_OCSP" = "xyes" && \
       test "x$ENABLED_ASN" = "xno"],
      [AC_MSG_ERROR([please enable asn if enabling ocsp.])])

AS_IF([test "x$ENABLED_SMIME" = "xyes" && \
       test "x$ENABLED_ASN" = "xno"],
       [AC_MSG_ERROR([please enable asn if enabling S/MIME.])])

AS_IF([test "x$ENABLED_OCSP" = "xyes" && \
       test "x$ENABLED_RSA" = "xno" && \
       test "x$ENABLED_ECC" = "xno"],
      [AC_MSG_ERROR([please enable rsa or ecc if enabling ocsp.])])

# Sync Intel QA and Sync Cavium Octeon require the crypto callback
AS_IF([test "x$ENABLED_INTEL_QA_SYNC" = "xyes" || test "x$ENABLED_OCTEON_SYNC" = "xyes"],
        [AS_IF([test "x$ENABLED_CRYPTOCB" = "xno"],
         [AC_MSG_ERROR([please enable the crypto callback support using --enable-cryptocb])])])

# checks for pkcs7 needed enables
AS_IF([test "x$ENABLED_PKCS7" = "xyes" && \
       test "x$ENABLED_RSA" = "xno" && \
       test "x$ENABLED_ECC" = "xno"],
      [AC_MSG_ERROR([please enable ecc or rsa if enabling pkcs7.])])

AS_IF([test "x$ENABLED_PKCS7" = "xyes" && \
       test "x$ENABLED_SHA" = "xno" && \
       test "x$ENABLED_SHA256" = "xno"],
      [AC_MSG_ERROR([please enable sha or sha256 if enabling pkcs7.])])

AS_IF([test "x$ENABLED_PKCS7" = "xyes" && \
       test "x$ENABLED_AES" = "xno" && \
       test "x$ENABLED_DES3" = "xno"],
      [AC_MSG_ERROR([please enable either AES or 3DES if enabling pkcs7.])])

AS_IF([test "x$ENABLED_WOLFSCEP" = "xyes" && \
       test "x$ENABLED_AES" = "xno" && \
       test "x$ENABLED_DES3" = "xno"],
      [AC_MSG_ERROR([please enable either AES or 3DES if enabling scep.])])

AS_IF([test "x$ENABLED_LEANTLS" = "xyes" && \
       test "x$ENABLED_ECC" = "xno"],
      [AC_MSG_ERROR([please enable ecc if enabling leantls.])])

AS_IF([test "x$ENABLED_SNIFFER" = "xyes" && \
       test "x$ENABLED_RSA" = "xno" && \
       test "x$ENABLED_ECC" = "xno" && \
       test "x$ENABLED_CURVE25519" = "xno"],
      [AC_MSG_ERROR([please enable ecc, rsa or curve25519 if enabling sniffer.])])

# Lean TLS forces off prereqs of SCEP.
AS_IF([test "x$ENABLED_SCEP" = "xyes" && \
       test "x$ENABLED_LEANTLS" = "xyes"],
      [AC_MSG_ERROR([Cannot use SCEP and Lean TLS at the same time.])])

# CMAC currently requires AES.
AS_IF([test "x$ENABLED_CMAC" = "xyes" && \
       test "x$ENABLED_AES" = "xno"],
      [AC_MSG_ERROR([cannot use CMAC without AES.])])

# certreq requires certgen
AS_IF([test "x$ENABLED_CERT_REQ" = "xyes" && \
       test "x$ENABLED_CERT_GEN" = "xno"],
      [AC_MSG_ERROR([cannot use certreq without certgen.])])

# ed25519 requires sha512
AS_IF([test "x$ENABLED_ED25519" = "xyes" && \
       test "x$ENABLED_SHA512" = "xno" && \
       test "x$ENABLED_32BIT" = "xno"],
      [AC_MSG_ERROR([cannot enable ed25519 without enabling sha512.])])

# ed25519 stream requires ed25519
AS_IF([test "x$ENABLED_ED25519_STREAM" = "xyes" && \
       test "x$ENABLED_ED25519" = "xno"],
      [AC_MSG_ERROR([ED25519 verify streaming enabled but ED25519 is disabled])])

# Ensure only one size is enabled
AS_IF([test "x$ENABLED_64BIT" = "xyes" && \
       test "x$ENABLED_32BIT" = "xyes"],
      [AC_MSG_ERROR([cannot specify 64-bit build and 32-bit build.])])
AS_IF([test "x$ENABLED_64BIT" = "xyes" && \
       test "x$ENABLED_16BIT" = "xyes"],
      [AC_MSG_ERROR([cannot specify 64-bit build and 16-bit build.])])
AS_IF([test "x$ENABLED_32BIT" = "xyes" && \
       test "x$ENABLED_16BIT" = "xyes"],
      [AC_MSG_ERROR([cannot specify 32-bit build and 16-bit build.])])

# 16-bit build not supported with SP
AS_IF([test "x$ENABLED_16BIT" = "xyes" && \
       test "x$ENABLED_SP" = "xyes"],
      [AC_MSG_ERROR([16-bit build not available with SP.])])

################################################################################
# Update CFLAGS based on options                                               #
################################################################################
AS_IF([test "x$ENABLED_SP_MATH_ALL" = "xno" && test "x$ENABLED_FASTMATH" = "xno" &&
    test "x$ENABLED_HEAPMATH" = "xno"],
      [AM_CFLAGS="$AM_CFLAGS -DNO_BIG_INT"])

AS_IF([test "x$ENABLED_CERTS" = "xno"],
      [AM_CFLAGS="$AM_CFLAGS -DNO_CERTS"])

AS_IF([test "x$ENABLED_ASN" = "xno"],
      [AM_CFLAGS="$AM_CFLAGS -DNO_ASN"])

AS_IF([test "x$ENABLED_SYS_CA_CERTS" = "xyes"],
      [AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SYS_CA_CERTS"])

AS_IF([test "x$ENABLED_ALTNAMES" = "xyes"],
      [AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_ALT_NAMES"])

AS_IF([test "x$ENABLED_KEYGEN" = "xyes"],
      [AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_KEY_GEN"])

AS_IF([test "x$ENABLED_CERTREQ" = "xyes"],
      [AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_CERT_REQ"])

AS_IF([test "x$ENABLED_CERTGEN" = "xyes"],
      [AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_CERT_GEN"])

AS_IF([test "x$ENABLED_CERTEXT" = "xyes"],
      [AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_CERT_EXT"])

AS_IF([test "x$ENABLED_ED25519" = "xyes" && test "x$ENABLED_32BIT" = "xno"],
      [AM_CFLAGS="$AM_CFLAGS -DHAVE_ED25519"])

AS_IF([test "x$ENABLED_ED25519_SMALL" = "xyes"],
      [AM_CFLAGS="$AM_CFLAGS -DED25519_SMALL"])

AS_IF([test "x$ENABLED_OCSP" = "xyes"],
      [AM_CFLAGS="$AM_CFLAGS -DHAVE_OCSP"])

AS_IF([test "x$ENABLED_STRONGSWAN" = "xyes"],
      [AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_DES_ECB -DWOLFSSL_LOG_PRINTF -DWOLFSSL_PUBLIC_MP -DHAVE_EX_DATA"])

AS_IF([test "x$ENABLED_OPENLDAP" = "xyes"], [AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SIGNER_DER_CERT"])

if test "$ENABLED_ED25519_STREAM" != "no" && test "$ENABLED_SE050" != "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_ED25519_STREAMING_VERIFY"
    AM_CCASFLAGS="$AM_CCASFLAGS -DWOLFSSL_ED25519_STREAMING_VERIFY"
fi

if test "$ENABLED_ERROR_QUEUE" = "no" || test "$ENABLED_JNI" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DNO_ERROR_QUEUE"
fi

AS_IF([test "x$ENABLED_OPENSSLALL" = "xyes"],
      [AM_CFLAGS="$AM_CFLAGS -DOPENSSL_ALL -DWOLFSSL_EITHER_SIDE -DWC_RSA_NO_PADDING -DWC_RSA_PSS -DWOLFSSL_PSS_LONG_SALT -DWOLFSSL_TICKET_HAVE_ID -DWOLFSSL_ERROR_CODE_OPENSSL -DWOLFSSL_CERT_NAME_ALL"])

AS_IF([test "x$ENABLED_AESSIV" = "xyes"], [AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_AES_SIV"])

AS_IF([test "x$ENABLED_AESCTR" = "xyes" && test "x$ENABLED_FORTRESS" != "xyes"],
      [AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_AES_COUNTER -DWOLFSSL_AES_DIRECT"])

if test "$ENABLED_MD5" = "no"
then
    AM_CFLAGS="$AM_CFLAGS -DNO_MD5 -DNO_OLD_TLS"
fi

if test "$ENABLED_HMAC" = "no"
then
    AM_CFLAGS="$AM_CFLAGS -DNO_HMAC"
fi

if test "$ENABLED_OPENSSLEXTRA" = "yes" && test "x$ENABLED_OPENSSLCOEXIST" = "xno"
then
  AM_CFLAGS="$AM_CFLAGS -DOPENSSL_EXTRA -DWOLFSSL_ALWAYS_VERIFY_CB"
  AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_VERIFY_CB_ALL_CERTS -DWOLFSSL_EXTRA_ALERTS"
  AM_CFLAGS="$AM_CFLAGS -DHAVE_EXT_CACHE -DWOLFSSL_FORCE_CACHE_ON_TICKET"
  AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_AKID_NAME -DHAVE_CTS"
fi

if test "$ENABLED_OPENSSLEXTRA" = "x509small"
then
  AC_MSG_NOTICE([Enabling only a subset of X509 opensslextra])
  AM_CFLAGS="$AM_CFLAGS -DOPENSSL_EXTRA_X509_SMALL"
  AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_EKU_OID -DWOLFSSL_MULTI_ATTRIB"
  AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_NO_OPENSSL_RAND_CB"
fi

if test "$ENABLED_WOLFSCEP" = "yes"
then
    # Enable prereqs if not already enabled
    if test "x$ENABLED_KEYGEN" = "xno"
    then
        ENABLED_KEYGEN="yes"
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_KEY_GEN"
    fi
    if test "x$ENABLED_CERTGEN" = "xno"
    then
        ENABLED_CERTGEN="yes"
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_CERT_GEN"
    fi
    if test "x$ENABLED_CERTREQ" = "xno"
    then
        ENABLED_CERTREQ="yes"
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_CERT_REQ"
    fi
    if test "x$ENABLED_CERTEXT" = "xno"
    then
        ENABLED_CERTEXT="yes"
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_CERT_EXT"
    fi
    if test "x$ENABLED_PKCS7" = "xno"
    then
        ENABLED_PKCS7="yes"
    fi
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_HAVE_WOLFSCEP"
fi

if test "x$ENABLED_PKCS7" = "xyes"
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_PKCS7"
    # Enable prereqs if not already enabled
    if test "x$ENABLED_AESKEYWRAP" = "xno"
    then
        ENABLED_AESKEYWRAP="yes"
        AM_CFLAGS="$AM_CFLAGS -DHAVE_AES_KEYWRAP -DWOLFSSL_AES_DIRECT"
    fi
    if test "x$ENABLED_X963KDF" = "xno" && test "$ENABLED_ECC" != "no"
    then
        ENABLED_X963KDF="yes"
        AM_CFLAGS="$AM_CFLAGS -DHAVE_X963_KDF"
    fi
fi

if test "x$ENABLED_DES3" = "xno"
then
    AM_CFLAGS="$AM_CFLAGS -DNO_DES3"
else
    # turn off DES3 if leanpsk or leantls on
    if test "$ENABLED_LEANPSK" = "yes" || test "$ENABLED_LEANTLS" = "yes"
    then
        AM_CFLAGS="$AM_CFLAGS -DNO_DES3"
        ENABLED_DES3=no
    fi
fi

if test "$ENABLED_AESGCM" != "no"
then
    if test "$ENABLED_AESGCM" = "word"
    then
        ENABLED_AESGCM=yes
    fi

    if test "$ENABLED_AESGCM" = "word32"
    then
        AM_CFLAGS="$AM_CFLAGS -DGCM_WORD32"
        ENABLED_AESGCM=yes
    fi

    if test "$ENABLED_AESGCM" = "small" || test "$ENABLED_LOWRESOURCE" = "yes"
    then
        AM_CFLAGS="$AM_CFLAGS -DGCM_SMALL"
        ENABLED_AESGCM=yes
    fi

    if test "$ENABLED_AESGCM" = "table"
    then
        AM_CFLAGS="$AM_CFLAGS -DGCM_TABLE"
        ENABLED_AESGCM=yes
    fi

    if test "$ENABLED_AESGCM" = "4bit"
    then
        AM_CFLAGS="$AM_CFLAGS -DGCM_TABLE_4BIT"
        ENABLED_AESGCM=yes
    fi

    AM_CFLAGS="$AM_CFLAGS -DHAVE_AESGCM"
fi
if test "$ENABLED_AESGCM_STREAM" != "no"
then
    if test "$ENABLED_AESGCM" = "no"
    then
        AC_MSG_ERROR([AES-GCM streaming enabled but AES-GCM is disabled])
    else
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_AESGCM_STREAM"
        AM_CCASFLAGS="$AM_CCASFLAGS -DWOLFSSL_AESGCM_STREAM"
    fi
fi

if test "$ENABLED_IOTSAFE" != "no"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_IOTSAFE"
    ENABLED_IOTSAFE=yes
fi

if test "$ENABLED_IOTSAFE_HWRNG" != "no"
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_IOTSAFE_HWRNG"
    ENABLED_IOTSAFE_HWRNG=yes
fi

if test "x$ENABLED_WOLFENGINE" = "xyes"
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_AES_ECB"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_AES_DIRECT"
    AM_CFLAGS="$AM_CFLAGS -DWC_RSA_NO_PADDING"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_PUBLIC_MP"
    AM_CFLAGS="$AM_CFLAGS -DECC_MIN_KEY_SZ=192"
fi

if test "$ENABLED_WOLFENGINE" = "yes" && test "$ENABLED_FIPS" != "no"
then
    AM_CFLAGS="$AM_CFLAGS -DSha3=wc_Sha3"
    AM_CFLAGS="$AM_CFLAGS -DNO_OLD_SHA256_NAMES"
    AM_CFLAGS="$AM_CFLAGS -DNO_OLD_MD5_NAME"
fi

if test "$ENABLED_WOLFENGINE" = "yes" && test "$FIPS_VERSION" != "v2"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_PSS_LONG_SALT"
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_PSS_SALT_LEN_DISCOVER"
fi

AS_IF([test "x$ENABLED_MAXSTRENGTH" = "xyes"],
      [AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_MAX_STRENGTH -DWOLFSSL_CIPHER_TEXT_CHECK"])

AS_IF([test "x$ENABLED_MAXSTRENGTH" = "xyes" && \
       test "x$ENABLED_OLD_TLS" = "xyes"],
      [AM_CFLAGS="$AM_CFLAGS -DNO_OLD_TLS"
       ENABLED_OLD_TLS=no])

AS_IF([test "x$ENABLED_MAXSTRENGTH" = "xyes" && \
       test "x$ENABLED_SSLV3" = "xyes"],
      [AC_MSG_ERROR([Cannot use Max Strength and SSLv3 at the same time.])])

AS_IF([test "x$ENABLED_SCTP" = "xyes"],
      [AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SCTP"])

AS_IF([test "x$ENABLED_SRTP" = "xyes"],
      [AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SRTP"])

AS_IF([test "x$ENABLED_MCAST" = "xyes"],
      [AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_MULTICAST"])

# WOLFSSL_AFALG does not support SHA224 yet
AS_IF([(test "x$ENABLED_AFALG" = "xyes") && (test "x$ENABLED_SHA224" = "xyes")],
      [AC_MSG_ERROR([--enable-sha224 with --enable-afalg not yet supported])])

# WOLFSSL_DEVCRYPTO does not support SHA224 yet
AS_IF([(test "x$ENABLED_DEVCRYPTO" = "xyes") && \
       (test "x$ENABLED_CAAM" = "xno") && \
       (test "x$ENABLED_SHA224" = "xyes")],
      [AC_MSG_ERROR([--enable-sha224 with --enable-devcrypto not yet supported])])

# SCTP, Multicast, SRTP, and strongSwan require DTLS
AS_IF([(test "x$ENABLED_DTLS" = "xno") && \
        (test "x$ENABLED_SCTP" = "xyes" || test "x$ENABLED_MCAST" = "xyes" || \
         test "x$ENABLED_SRTP" = "xyes" || \
         test "x$ENABLED_STRONGSWAN" = "xyes")],
      [AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_DTLS"
       ENABLED_DTLS=yes])

# Multicast requires the null cipher
AS_IF([test "x$ENABLED_NULL_CIPHER" = "xno" && \
        test "x$ENABLED_MCAST" = "xyes"],
      [AM_CFLAGS="$AM_CFLAGS -DHAVE_NULL_CIPHER"
       ENABLED_NULL_CIPHER=yes])

# wolfSSH and WPA Supplicant both need Public MP, only enable once.
# This will let you know if you enabled wolfSSH but have any of the prereqs
# disabled. Some of these options, disabling them adds things to the FLAGS and
# you need to check and add items in two places depending on the option.
AS_IF([test "x$ENABLED_WOLFSSH" = "xyes"],[AS_IF([test "x$ENABLED_WPAS" = "xno"],[AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_PUBLIC_MP"])])

if test "x$ENABLED_OPENSSLCOEXIST" = "xyes"; then
    if test "x$ENABLED_OPENSSLALL" = "xyes"; then
        AC_MSG_ERROR([Cannot use --enable-opensslcoexist with --enable-opensslall])
    fi
    if test "x$ENABLED_OPENSSLEXTRA" = "xyes"; then
        AC_MSG_ERROR([Cannot use --enable-opensslcoexist with --enable-opensslextra])
    fi
fi

AS_IF([test "x$ENABLED_WOLFSSH" = "xyes"],[AM_CPPFLAGS="$AM_CPPFLAGS -DWOLFSSL_WOLFSSH"])

# only allow secure renegotiation info with TLSV12 and ASN
if test "x$ENABLED_ASN" = "xno" || \
    test "x$ENABLED_TLSV12" = "xno" || \
    test "x$ENABLED_RENEGOTIATION_INDICATION" = "xyes"; then
    ENABLED_SECURE_RENEGOTIATION_INFO="no"
fi

if test "x$ENABLED_SECURE_RENEGOTIATION_INFO" = "xyes"; then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_TLS_EXTENSIONS -DHAVE_SERVER_RENEGOTIATION_INFO"
fi


if test "$ENABLED_COMPKEY" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_COMP_KEY"
fi


# Deprecated Algorithm Handling
if test "$ENABLED_ARC4" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_ALLOW_RC4"
fi


# Link with the math library iff needed.
if test "$ENABLED_DH" != "no" && test "$ENABLED_DH" != "const"; then
   LT_LIB_M
fi


################################################################################

# USER SETTINGS
if test "x$ENABLED_USERSETTINGS" = "xyes"
then
    # Replace all options and just use WOLFSSL_USER_SETTINGS and
    # WOLFSSL_USER_SETTINGS_ASM.
    AM_CFLAGS="-DWOLFSSL_USER_SETTINGS -DWOLFSSL_USER_SETTINGS_ASM"
    AM_CCASFLAGS="$AM_CCASFLAGS -DWOLFSSL_USER_SETTINGS -DWOLFSSL_USER_SETTINGS_ASM"

    # Generate assembly-safe user_settings_asm.h (just preprocessor directives
    # from user_settings.h).
    $srcdir/scripts/user_settings_asm.sh "$CPPFLAGS $CFLAGS $CXXFLAGS"
    if test $? -ne 0; then
        AC_MSG_ERROR([$srcdir/scripts/user_settings_asm.sh failed.])
    fi
fi

# OPTIMIZE FLAGS
# For distro disable custom build options that interfere with symbol generation
if test "$GCC" = "yes" && test "$ENABLED_DISTRO" = "no"
then
    AM_CFLAGS="$AM_CFLAGS -Wall -Wno-unused"
    if test "$ax_enable_debug" = "no"
    then
    AS_IF([test "x$ENABLED_OPTFLAGS" = "xyes"], [
        if test "$ENABLED_FASTMATH" = "yes"
        then
            AM_CFLAGS="$AM_CFLAGS $OPTIMIZE_FAST_CFLAGS"
            if test "$ENABLED_FASTHUGEMATH" = "yes"
            then
                AM_CFLAGS="$AM_CFLAGS $OPTIMIZE_HUGE_CFLAGS"
            fi
        else
            if (test "$ENABLED_SP" = "yes" || test "$ENABLED_SP_MATH_ALL" = "yes") && test "$ENABLED_SP_SMALL" = "no"
            then
                AM_CFLAGS="$AM_CFLAGS $OPTIMIZE_FAST_CFLAGS"
                if test "$ENABLED_FASTHUGEMATH" = "yes"
                then
                    AM_CFLAGS="$AM_CFLAGS $OPTIMIZE_HUGE_CFLAGS"
                    AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_SP_INT_LARGE_COMBA"
                fi
            else
                AM_CFLAGS="$AM_CFLAGS $OPTIMIZE_CFLAGS"
            fi
        fi
    ])
    fi
fi

# ICC command line warning for non supported warning flags
if test "$CC" = "icc"
then
    AM_CFLAGS="$AM_CFLAGS -wd10006"
fi

# Expose HAVE___UINT128_T to options flags"
if test "$ac_cv_type___uint128_t" = "yes"
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE___UINT128_T=1"
fi

LIB_SOCKET_NSL
AX_HARDEN_CC_COMPILER_FLAGS

case $host_os in
    mingw*)
        # if mingw then link to ws2_32 for sockets, and crypt32
        LDFLAGS="$LDFLAGS -lws2_32"
        LIB_ADD="$LIB_ADD -lcrypt32"
        if test "$enable_shared" = "yes"
        then
            AC_DEFINE([WOLFSSL_DLL], [1], [Use __declspec(dllexport) when building library])
            if test "$enable_static" = "yes"
            then
                MINGW_LIB_WARNING="yes"
            fi
        fi ;;
esac

if test "$enable_shared" = "no"; then
    if test "$enable_static" = "yes"; then
        AM_CFLAGS="$AM_CFLAGS -DWOLFSSL_TEST_STATIC_BUILD"
    fi
fi

if test "x$ENABLED_LINUXKM" = "xyes"; then
    AX_SIMD_CC_COMPILER_FLAGS
    AC_SUBST([CFLAGS_FPU_DISABLE])
    AC_SUBST([CFLAGS_FPU_ENABLE])
    AC_SUBST([CFLAGS_SIMD_DISABLE])
    AC_SUBST([CFLAGS_SIMD_ENABLE])
    AC_SUBST([CFLAGS_AUTO_VECTORIZE_DISABLE])
    AC_SUBST([CFLAGS_AUTO_VECTORIZE_ENABLE])
    AC_SUBST([ASFLAGS_FPU_DISABLE_SIMD_ENABLE])
    AC_SUBST([ASFLAGS_FPU_ENABLE_SIMD_DISABLE])
    AC_SUBST([ASFLAGS_FPUSIMD_DISABLE])
    AC_SUBST([ASFLAGS_FPUSIMD_ENABLE])

    if test "$ENABLED_OPENSSLEXTRA" != "no" && test "$ENABLED_CRYPTONLY" = "no"; then
        AC_MSG_ERROR([--enable-opensslextra without --enable-cryptonly is incompatible with --enable-linuxkm.])
    fi
    if test "$ENABLED_FILESYSTEM" = "yes"; then
        AC_MSG_ERROR([--enable-filesystem is incompatible with --enable-linuxkm.])
    fi
    if test "$ENABLED_AFALG" = "yes"; then
        AC_MSG_ERROR([--enable-afalg is incompatible with --enable-linuxkm.])
    fi
    if test "$ENABLED_DEVCRYPTO" = "yes"; then
        AC_MSG_ERROR([--enable-devcrypto is incompatible with --enable-linuxkm.])
    fi
    if test "$ENABLED_PKCS11" = "yes"; then
        AC_MSG_ERROR([--enable-pkcs11 is incompatible with --enable-linuxkm.])
    fi
    if test "$ENABLED_JNI" = "yes"; then
        AC_MSG_ERROR([--enable-jni is incompatible with --enable-linuxkm.])
    fi
    if test "$ENABLED_16BIT" = "yes"; then
        AC_MSG_ERROR([--enable-16bit is incompatible with --enable-linuxkm.])
    fi
    if test "$ENABLED_SINGLETHREADED" = "yes"; then
        AC_MSG_ERROR([--enable-singlethreaded is incompatible with --enable-linuxkm.])
    fi
    if test "$ENABLED_VALGRIND" = "yes"; then
        AC_MSG_ERROR([--enable-valgrind is incompatible with --enable-linuxkm.])
    fi
    if test "$ENABLED_FASTMATH" = "yes"; then
        AC_MSG_ERROR([--enable-fastmath is incompatible with --enable-linuxkm (exceeds stack limit).])
    fi
    if test "$ENABLED_FAST_RSA" = "yes"; then
        AC_MSG_ERROR([--enable-fastrsa is incompatible with --enable-linuxkm.])
    fi
    if test "$ENABLED_LIBZ_RSA" = "yes"; then
        AC_MSG_ERROR([--with-libz is incompatible with --enable-linuxkm.])
    fi
    if test "$ENABLED_IOPOOL" = "yes"; then
        AC_MSG_ERROR([--enable-iopool is incompatible with --enable-linuxkm.])
    fi
    if test "$ENABLED_EXAMPLES" = "yes"; then
        AC_MSG_ERROR([--enable-examples is incompatible with --enable-linuxkm.])
    fi
    if test "$ENABLED_SMALL_STACK" != "yes"; then
        AC_MSG_ERROR([--enable-smallstack is required for --enable-linuxkm.])
    fi
    if test "$ENABLED_SP_MATH" = "no" && test "$ENABLED_SP_MATH_ALL" = "no" && test "$ENABLED_BIGNUM" != "no"; then
        AC_MSG_ERROR([--enable-sp-math or --enable-sp-math-all is required for --enable-linuxkm.])
    fi
    if test "$ENABLED_STACKSIZE" != "no"; then
        AC_MSG_ERROR([--enable-stacksize is incompatible with --enable-linuxkm.])
    fi
    if test "$ENABLED_STACKLOG" = "yes"; then
        AC_MSG_ERROR([--enable-stacklog is incompatible with --enable-linuxkm.])
    fi
    if test "$ENABLED_COMPKEY" = "yes"; then
        AC_MSG_ERROR([--enable-compkey is incompatible with --enable-linuxkm.])
    fi
fi

# The following AM_CONDITIONAL statements set flags for use in the Makefiles.
# Some of these affect build targets and objects, some trigger different
# test scripts for make check.
AM_CONDITIONAL([BUILD_DISTRO],[test "x$ENABLED_DISTRO" = "xyes"])
AM_CONDITIONAL([BUILD_ALL],[test "x$ENABLED_ALL" = "xyes"])
AM_CONDITIONAL([BUILD_TLS13],[test "x$ENABLED_TLS13" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_RNG],[test "x$ENABLED_RNG" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_SCTP],[test "x$ENABLED_SCTP" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_SRTP],[test "x$ENABLED_SRTP" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_MCAST],[test "x$ENABLED_MCAST" = "xyes"])
AM_CONDITIONAL([BUILD_IPV6],[test "x$ENABLED_IPV6" = "xyes"])
AM_CONDITIONAL([BUILD_LEANPSK],[test "x$ENABLED_LEANPSK" = "xyes"])
AM_CONDITIONAL([BUILD_LEANTLS],[test "x$ENABLED_LEANTLS" = "xyes"])
AM_CONDITIONAL([BUILD_LOWMEM],[test "x$ENABLED_LOWRESOURCE" = "xyes"])
AM_CONDITIONAL([BUILD_PKCALLBACKS],  [ test "x$ENABLED_PKCALLBACKS" = "xyes"])
AM_CONDITIONAL([BUILD_CRYPTOAUTHLIB],[test "x$ENABLED_CRYPTOAUTHLIB" = "xyes"])
AM_CONDITIONAL([BUILD_SNIFFER],  [ test "x$ENABLED_SNIFFER"   = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_SNIFFTEST],[ test "x$ENABLED_SNIFFTEST" = "xyes"])
AM_CONDITIONAL([BUILD_AESGCM],[test "x$ENABLED_AESGCM" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_AESCCM],[test "x$ENABLED_AESCCM" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_ARMASM],[test "x$ENABLED_ARMASM" = "xyes"])
AM_CONDITIONAL([BUILD_ARMASM_INLINE],[test "x$ENABLED_ARMASM_INLINE" = "xyes"])
AM_CONDITIONAL([BUILD_ARMASM_CRYPTO],[test "x$ENABLED_ARMASM_CRYPTO" = "xyes"])
AM_CONDITIONAL([BUILD_XILINX],[test "x$ENABLED_XILINX" = "xyes"])
AM_CONDITIONAL([BUILD_AESNI],[test "x$ENABLED_AESNI" = "xyes"])
AM_CONDITIONAL([BUILD_INTELASM],[test "x$ENABLED_INTELASM" = "xyes"])
AM_CONDITIONAL([BUILD_X86_ASM],[test "x$ENABLED_X86_ASM" = "xyes"])
AM_CONDITIONAL([BUILD_AFALG],[test "x$ENABLED_AFALG" = "xyes"])
AM_CONDITIONAL([BUILD_KCAPI],[test "x$ENABLED_KCAPI" = "xyes"])
AM_CONDITIONAL([BUILD_DEVCRYPTO],[test "x$ENABLED_DEVCRYPTO" = "xyes"])
AM_CONDITIONAL([BUILD_CAMELLIA],[test "x$ENABLED_CAMELLIA" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_MD2],[test "x$ENABLED_MD2" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_RIPEMD],[test "x$ENABLED_RIPEMD" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_BLAKE2],[test "x$ENABLED_BLAKE2" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_BLAKE2S],[test "x$ENABLED_BLAKE2S" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_SHA512],[test "x$ENABLED_SHA512" = "xyes" || test "x$ENABLED_SHA384" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_DSA],[test "x$ENABLED_DSA" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_ECC],[test "x$ENABLED_ECC" != "xno" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_ED25519],[test "x$ENABLED_ED25519" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_ED25519_SMALL],[test "x$ENABLED_ED25519_SMALL" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_FEMATH], [test "x$ENABLED_FEMATH" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_GEMATH], [test "x$ENABLED_GEMATH" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_CURVE25519],[test "x$ENABLED_CURVE25519" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_CURVE25519_SMALL],[test "x$ENABLED_CURVE25519_SMALL" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_ED448],[test "x$ENABLED_ED448" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_ED448_SMALL],[test "x$ENABLED_ED448_SMALL" = "xyes"])
AM_CONDITIONAL([BUILD_FE448], [test "x$ENABLED_FE448" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_GE448], [test "x$ENABLED_GE448" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_CURVE448],[test "x$ENABLED_CURVE448" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_CURVE448_SMALL],[test "x$ENABLED_CURVE448_SMALL" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_WC_LMS],[test "x$ENABLED_WC_LMS" != "xno" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_WC_KYBER],[test "x$ENABLED_WC_KYBER" != "xno" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_ECCSI],[test "x$ENABLED_ECCSI" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_SAKKE],[test "x$ENABLED_SAKKE" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_MEMORY],[test "x$ENABLED_MEMORY" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_RSA],[test "x$ENABLED_RSA" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_DH],[test "x$ENABLED_DH" != "xno" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_ASN],[test "x$ENABLED_ASN" != "xno" || test "x$ENABLED_RSA" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_AES],[test "x$ENABLED_AES" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_CODING],[test "x$ENABLED_CODING" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_RC4],[test "x$ENABLED_ARC4" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_MD5],[test "x$ENABLED_MD5" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_SHA],[test "x$ENABLED_SHA" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_FIPS],[test "x$ENABLED_FIPS" = "xyes"])
AC_SUBST([ENABLED_FIPS])
AM_CONDITIONAL([BUILD_FIPS_V1],[test "$HAVE_FIPS_VERSION" = 1])
AM_CONDITIONAL([BUILD_FIPS_V2],[test "$HAVE_FIPS_VERSION" = 2 && test "$HAVE_FIPS_VERSION_MINOR" = 0])
AM_CONDITIONAL([BUILD_FIPS_RAND],[test "$HAVE_FIPS_VERSION" = 2 && test "$HAVE_FIPS_VERSION_MINOR" = 1])
AM_CONDITIONAL([BUILD_FIPS_V5],[test "$HAVE_FIPS_VERSION" = 5])
AM_CONDITIONAL([BUILD_FIPS_CURRENT],[test "$HAVE_FIPS_VERSION" -ge 2 ])
    # BUILD_FIPS_CURRENT is for builds after cert 2425.
AM_CONDITIONAL([BUILD_SIPHASH],[test "x$ENABLED_SIPHASH" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_CMAC],[test "x$ENABLED_CMAC" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_SELFTEST],[test "x$ENABLED_SELFTEST" = "xyes"])
AM_CONDITIONAL([BUILD_SHA224],[test "x$ENABLED_SHA224" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_SHA3],[test "x$ENABLED_SHA3" != "xno" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_POLY1305],[test "x$ENABLED_POLY1305" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_CHACHA],[test "x$ENABLED_CHACHA" = "xyes" || test "x$ENABLED_CHACHA" = "xnoasm" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_XCHACHA],[test "x$ENABLED_XCHACHA" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_SM2],[test "x$ENABLED_SM2" != "xno" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_SM3],[test "x$ENABLED_SM3" != "xno" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_SM4],[test "x$ENABLED_SM4" != "xno" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_INLINE],[test "x$ENABLED_INLINE" = "xyes"])
AM_CONDITIONAL([BUILD_OCSP],[test "x$ENABLED_OCSP" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_OCSP_STAPLING],[test "x$ENABLED_CERTIFICATE_STATUS_REQUEST" = "xyes"])
AM_CONDITIONAL([BUILD_OCSP_STAPLING_V2],[test "x$ENABLED_CERTIFICATE_STATUS_REQUEST_V2" = "xyes"])
AM_CONDITIONAL([BUILD_CRL],[test "x$ENABLED_CRL" != "xno" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_CRL_MONITOR],[test "x$ENABLED_CRL_MONITOR" = "xyes"])
AM_CONDITIONAL([BUILD_USER_RSA],[test "x$ENABLED_USER_RSA" = "xyes"] )
AM_CONDITIONAL([BUILD_USER_CRYPTO],[test "x$ENABLED_USER_CRYPTO" = "xyes"])
AM_CONDITIONAL([BUILD_LIBLMS],[test "x$ENABLED_LIBLMS" = "xyes"])
AM_CONDITIONAL([BUILD_LIBOQS],[test "x$ENABLED_LIBOQS" = "xyes"])
AM_CONDITIONAL([BUILD_WNR],[test "x$ENABLED_WNR" = "xyes"])
AM_CONDITIONAL([BUILD_SRP],[test "x$ENABLED_SRP" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([USE_VALGRIND],[test "x$ENABLED_VALGRIND" = "xyes"])
AM_CONDITIONAL([BUILD_MD4],[test "x$ENABLED_MD4" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_PWDBASED],[test "x$ENABLED_PWDBASED" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_SCRYPT],[test "x$ENABLED_SCRYPT" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_CRYPTONLY],[test "x$ENABLED_CRYPTONLY" = "xyes" && test "x$ENABLED_OPENSSLEXTRA" = "xno"])
AM_CONDITIONAL([BUILD_FASTMATH],[test "x$ENABLED_FASTMATH" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_HEAPMATH],[test "x$ENABLED_HEAPMATH" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_EXAMPLE_SERVERS],[test "x$ENABLED_EXAMPLES" = "xyes" && test "x$ENABLED_LEANTLS" = "xno"])
AM_CONDITIONAL([BUILD_EXAMPLE_CLIENTS],[test "x$ENABLED_EXAMPLES" = "xyes"])
AM_CONDITIONAL([BUILD_EXAMPLE_ASN1],[test "x$ENABLED_EXAMPLES" = "xyes"] && [test "x$ENABLED_ASN_PRINT" = "xyes"] && [test "x$ENABLED_ASN" = "xyes"])
AM_CONDITIONAL([BUILD_TESTS],[test "x$ENABLED_EXAMPLES" = "xyes"])
AM_CONDITIONAL([BUILD_THREADED_EXAMPLES],[test "x$ENABLED_SINGLETHREADED" = "xno" && test "x$ENABLED_EXAMPLES" = "xyes" && test "x$ENABLED_LEANTLS" = "xno"])
AM_CONDITIONAL([BUILD_WOLFCRYPT_TESTS],[test "x$ENABLED_CRYPT_TESTS" = "xyes"])
AM_CONDITIONAL([BUILD_WOLFCRYPT_TESTS_LIBS],[test "x$ENABLED_CRYPT_TESTS_LIBS" = "xyes"])
AM_CONDITIONAL([BUILD_LIBZ],[test "x$ENABLED_LIBZ" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_PKCS11],[test "x$ENABLED_PKCS11" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_PKCS12],[test "x$ENABLED_PKCS12" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_PKCS8],[test "x$ENABLED_PKCS8" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_CAVIUM],[test "x$ENABLED_CAVIUM" = "xyes"])
AM_CONDITIONAL([BUILD_CAVIUM_V],[test "x$ENABLED_CAVIUM_V" = "xyes"])
AM_CONDITIONAL([BUILD_OCTEON_SYNC],[test "x$ENABLED_OCTEON_SYNC" = "xyes"])
AM_CONDITIONAL([BUILD_INTEL_QA],[test "x$ENABLED_INTEL_QA" = "xyes"])
AM_CONDITIONAL([BUILD_INTEL_QA_SYNC],[test "x$ENABLED_INTEL_QA_SYNC" = "xyes"])
INCLUDE_SP_INT="no"
AM_CONDITIONAL([BUILD_SP],[test "x$ENABLED_SP" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_SP_C32],[ ( ( (test "$ENABLED_SP_RSA" = "yes" || test "$ENABLED_SP_DH" = "yes" \
                               || test "$ENABLED_SP_ECC" = "yes") && test "x$ENABLED_SP_ASM" = "xno") \
                             || test "x$ENABLED_USERSETTINGS" = "xyes") && test "x$ENABLED_64BIT" != "xyes"])
AM_CONDITIONAL([BUILD_SP_C64],[ ( ( (test "$ENABLED_SP_RSA" = "yes" || test "$ENABLED_SP_DH" = "yes" \
                               || test "$ENABLED_SP_ECC" = "yes") && test "x$ENABLED_SP_ASM" = "xno") \
                             || test "x$ENABLED_USERSETTINGS" = "xyes") && test "x$ENABLED_32BIT" != "xyes"])
AM_CONDITIONAL([BUILD_SP_ARM64],[test "x$ENABLED_SP_ARM64_ASM" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_SP_ARM32],[test "x$ENABLED_SP_ARM32_ASM" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_SP_ARM_THUMB],[test "x$ENABLED_SP_ARM_THUMB_ASM" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_SP_ARM_CORTEX],[test "x$ENABLED_SP_ARM_CORTEX_ASM" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_SP_X86_64],[test "x$ENABLED_SP_X86_64_ASM" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_SP_INT],[test "x$ENABLED_SP_MATH" = "xyes" || test "x$ENABLED_SP_MATH_ALL" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_COND_IF([BUILD_SP], [INCLUDE_SP_INT="yes"])
AM_COND_IF([BUILD_SP_INT], [INCLUDE_SP_INT="yes"])
AC_SUBST([INCLUDE_SP_INT])
AM_CONDITIONAL([BUILD_FAST_RSA],[test "x$ENABLED_FAST_RSA" = "xyes"])
AM_CONDITIONAL([BUILD_MCAPI],[test "x$ENABLED_MCAPI" = "xyes"])
AM_CONDITIONAL([BUILD_ASYNCCRYPT],[test "x$ENABLED_ASYNCCRYPT" = "xyes"])
AM_CONDITIONAL([BUILD_WOLFEVENT],[test "x$ENABLED_ASYNCCRYPT" = "xyes"])
AM_CONDITIONAL([BUILD_CRYPTOCB],[test "x$ENABLED_CRYPTOCB" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_PSK],[test "x$ENABLED_PSK" = "xyes"])
AM_CONDITIONAL([BUILD_TRUST_PEER_CERT],[test "x$ENABLED_TRUSTED_PEER_CERT" = "xyes"])
AM_CONDITIONAL([BUILD_PKI],[test "x$ENABLED_PKI" = "xyes"])
AM_CONDITIONAL([BUILD_DES3],[test "x$ENABLED_DES3" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_PKCS7],[test "x$ENABLED_PKCS7" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_SMIME],[test "x$ENABLED_SMIME" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_HASHFLAGS],[test "x$ENABLED_HASHFLAGS" = "xyes"])
AM_CONDITIONAL([BUILD_LINUXKM],[test "$ENABLED_LINUXKM" = "yes"])
AM_CONDITIONAL([BUILD_NO_LIBRARY],[test "$ENABLED_NO_LIBRARY" = "yes"])
AM_CONDITIONAL([BUILD_BENCHMARK],[test "$ENABLED_BENCHMARK" = "yes"])
AM_CONDITIONAL([BUILD_RC2],[test "x$ENABLED_RC2" = "xyes"])
AM_CONDITIONAL([BUILD_CAAM],[test "x$ENABLED_CAAM" != "xno"])
AM_CONDITIONAL([BUILD_QNXCAAM],[test "x$ENABLED_CAAM_QNX" = "xyes"])
AM_CONDITIONAL([BUILD_IOTSAFE],[test "x$ENABLED_IOTSAFE" = "xyes"])
AM_CONDITIONAL([BUILD_IOTSAFE_HWRNG],[test "x$ENABLED_IOTSAFE_HWRNG" = "xyes"])
AM_CONDITIONAL([BUILD_SE050],[test "x$ENABLED_SE050" = "xyes"])
AM_CONDITIONAL([BUILD_KDF],[test "x$ENABLED_KDF" = "xyes"])
AM_CONDITIONAL([BUILD_HMAC],[test "x$ENABLED_HMAC" = "xyes"])
AM_CONDITIONAL([BUILD_ERROR_STRINGS],[test "x$ENABLED_ERROR_STRINGS" = "xyes"])
AM_CONDITIONAL([BUILD_DO178],[test "x$ENABLED_DO178" = "xyes"])
AM_CONDITIONAL([BUILD_PSA],[test "x$ENABLED_PSA" = "xyes"])
AM_CONDITIONAL([BUILD_DTLS13],[test "x$ENABLED_DTLS13" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_QUIC],[test "x$ENABLED_QUIC" = "xyes"])
AM_CONDITIONAL([BUILD_DTLS_CID],[test "x$ENABLED_DTLS_CID" = "xyes"])
AM_CONDITIONAL([BUILD_HPKE],[test "x$ENABLED_HPKE" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_DTLS],[test "x$ENABLED_DTLS" = "xyes" || test "x$ENABLED_USERSETTINGS" = "xyes"])
AM_CONDITIONAL([BUILD_MAXQ10XX],[test "x$ENABLED_MAXQ10XX" = "xyes"])
AM_CONDITIONAL([BUILD_ARIA],[test "x$ENABLED_ARIA" = "xyes"])

if test "$ENABLED_REPRODUCIBLE_BUILD" != "yes" &&
   (test "$ax_enable_debug" = "yes" ||
        test "$ENABLED_STACKSIZE" != "no" ||
        (test "$ENABLED_LEANTLS" = "no" &&
             test "$ENABLED_LEANPSK" = "no" &&
             test "$ENABLED_LOWRESOURCE" = "no"))
then
    AM_CFLAGS="$AM_CFLAGS -DHAVE_WC_INTROSPECTION"
fi

if test "$ENABLED_REPRODUCIBLE_BUILD" != "yes"
then
    AM_CFLAGS="$AM_CFLAGS -include ${output_objdir}/.build_params"
fi

AM_CPPFLAGS="$AM_CPPFLAGS $EXTRA_CPPFLAGS"
AM_CFLAGS="$AM_CFLAGS $EXTRA_CFLAGS"
AM_CCASFLAGS="$AM_CCASFLAGS $EXTRA_CCASFLAGS"
AM_LDFLAGS="$AM_LDFLAGS $EXTRA_LDFLAGS"

CREATE_HEX_VERSION
AC_SUBST([AM_CPPFLAGS])
AC_SUBST([AM_CFLAGS])
AC_SUBST([AM_LDFLAGS])
AC_SUBST([AM_CCASFLAGS])
AC_SUBST([LIB_ADD])
AC_SUBST([LIB_STATIC_ADD])

# FINAL
AC_CONFIG_FILES([stamp-h], [echo timestamp > stamp-h])
AC_CONFIG_FILES([Makefile
        wolfssl/version.h
        wolfssl/options.h
        support/wolfssl.pc
        rpm/spec
        wolfcrypt/test/test_paths.h
        ])
AC_CONFIG_FILES([scripts/unit.test],[chmod +x scripts/unit.test])

AX_CREATE_GENERIC_CONFIG
AX_AM_JOBSERVER([yes])

# See Automake 9.4.1 Built Sources Example
AC_DEFUN([AX_OUT_OF_TREE_FILE],[
  AC_CONFIG_COMMANDS([$1], [test ! -f $srcdir/$1 && >> $srcdir/$1])
])

AX_OUT_OF_TREE_FILE([wolfssl/wolfcrypt/async.h])
AX_OUT_OF_TREE_FILE([wolfssl/wolfcrypt/fips.h])
AX_OUT_OF_TREE_FILE([wolfssl/wolfcrypt/port/cavium/cavium_nitrox.h])
AX_OUT_OF_TREE_FILE([wolfssl/wolfcrypt/port/intel/quickassist.h])
AX_OUT_OF_TREE_FILE([wolfssl/wolfcrypt/port/intel/quickassist_mem.h])

AC_OUTPUT


if test "$ENABLED_MAKECLEAN" = "yes"
then
    # force make clean
    AC_MSG_NOTICE([---])
    AC_MSG_NOTICE([Running make clean...])
    if test -z "$MAKE"; then
        MAKE="make"
    fi

    if test "$verbose" = "yes"; then
        $MAKE clean
    else
        $MAKE clean >/dev/null
    fi
fi

if test "$ENABLED_REPRODUCIBLE_BUILD" != "yes"
then
    ESCAPED_ARGS=$(echo "$ac_configure_args" | sed 's/\\/\\\\/g;s/\"/\\\"/g')
    ESCAPED_GLOBAL_CFLAGS=$(echo "$CPPFLAGS $AM_CPPFLAGS $CFLAGS $AM_CFLAGS" | sed 's/\\/\\\\/g;s/\"/\\\"/g')
    echo "#define LIBWOLFSSL_CONFIGURE_ARGS \"$ESCAPED_ARGS\"" > "${output_objdir}/.build_params" &&
        echo "#define LIBWOLFSSL_GLOBAL_CFLAGS \"$ESCAPED_GLOBAL_CFLAGS\" LIBWOLFSSL_GLOBAL_EXTRA_CFLAGS" >> "${output_objdir}/.build_params" ||
        AC_MSG_ERROR([Couldn't create ${output_objdir}/.build_params.])
else
    rm -f "${output_objdir}/.build_params"
fi

# generate user options header
AC_MSG_NOTICE([---])
AC_MSG_NOTICE([Generating user options header...])

OPTION_FILE="wolfssl/options.h"
rm -f $OPTION_FILE

echo "/* wolfssl options.h" > $OPTION_FILE
echo " * generated from configure options" >> $OPTION_FILE
echo " *" >> $OPTION_FILE
echo " * Copyright (C) 2006-2023 wolfSSL Inc." >> $OPTION_FILE
echo " *" >> $OPTION_FILE
echo " * This file is part of wolfSSL. (formerly known as CyaSSL)" >> $OPTION_FILE
echo " *" >> $OPTION_FILE
echo " */" >> $OPTION_FILE

echo "" >> $OPTION_FILE
echo "#ifndef WOLFSSL_OPTIONS_H" >> $OPTION_FILE
echo "#define WOLFSSL_OPTIONS_H" >> $OPTION_FILE
echo "" >> $OPTION_FILE
echo "" >> $OPTION_FILE
echo "#ifdef __cplusplus" >> $OPTION_FILE
echo "extern \"C\" {" >> $OPTION_FILE
echo "#endif" >> $OPTION_FILE
echo "" >> $OPTION_FILE

# Check for supported command to trim option with.
# note: cut requires an argument to exit with success.
if colrm >/dev/null 2>&1 </dev/null; then
    TRIM="colrm 3"
elif cut --version >/dev/null 2>&1 </dev/null; then
    TRIM="cut -c1-2"
else
    AC_MSG_ERROR([Could not find colrm or cut to make options file])
fi

for option in $CPPFLAGS $AM_CPPFLAGS $CFLAGS $AM_CFLAGS; do
    opt_type=$(echo $option | $TRIM )
    case "$opt_type" in
    -D)
        option=$(echo "$option" | tr -d '\\')
        RHS_only=$(echo "$option" | sed 's/^-D//')
        noequalsign=$(echo "$RHS_only" | tr '=' ' ')
        if test "$noequalsign" = "NDEBUG" || test "$noequalsign" = "DEBUG"
        then
            if test "$verbose" = "yes"; then
                AC_MSG_NOTICE([not outputting (N)DEBUG to $OPTION_FILE])
            fi
            continue
        fi

        # allow user to ignore system options
        ignoresys=$(echo "$noequalsign" | grep '^_.*')
        if test -n "$ignoresys"
        then
            echo "#ifndef WOLFSSL_OPTIONS_IGNORE_SYS" >> $OPTION_FILE
        fi

        # note need to use both autotools-style [] quoting and shell-style ''
        # quoting for sed script with [] character set expression here.
        noarg=$(echo "$RHS_only" | sed ['s/\(([^=)]*)\)\{0,1\}=.*//'])
        echo "#undef  $noarg" >> $OPTION_FILE
        echo "#define $noequalsign" >> $OPTION_FILE

        if test -n "$ignoresys"
        then
            echo "#endif" >> $OPTION_FILE
        fi

        echo "" >> $OPTION_FILE
        ;;
    -U)
        RHS_only=$(echo $option | sed 's/^-U//')
        echo "#undef  $RHS_only" >> $OPTION_FILE
        echo "" >> $OPTION_FILE
        ;;
    *)
        if test "$verbose" = "yes"; then
            AC_MSG_NOTICE([option "$option" is not a preprocessor directive -- not saving to $OPTION_FILE])
        fi
        ;;
    esac
done

echo "" >> $OPTION_FILE
echo "#ifdef __cplusplus" >> $OPTION_FILE
echo "}" >> $OPTION_FILE
echo "#endif" >> $OPTION_FILE
echo "" >> $OPTION_FILE
echo "" >> $OPTION_FILE
echo "#endif /* WOLFSSL_OPTIONS_H */" >> $OPTION_FILE
echo "" >> $OPTION_FILE

if test "$ENABLED_OPENSSLEXTRA" = "yes" && test "$ENABLED_LINUXKM" = "no"
then
    SAVE_CFLAGS=$CFLAGS
    CFLAGS="$CFLAGS $DEFS -I. -I$srcdir"
    if test "$ENABLED_INTEL_QA" = "yes"
    then
        CFLAGS="$CFLAGS $QAT_FLAGS"
    fi
    build_pwd="$(pwd)"
    cd "$srcdir"
    openssl_headers=$(echo wolfssl/openssl/*.h)
    cd "$build_pwd"
    for header in $openssl_headers
    do
        AC_CHECK_HEADER([$header], [], [
                AC_MSG_ERROR([Header file inconsistency detected -- error including ${header}.])
            ], [
            #include <${OPTION_FILE}>
            extern int dummy_int_to_make_compiler_happy;
        ])
    done
    CFLAGS=$SAVE_CFLAGS
fi

if test "$silent" != "yes"; then

# output config summary
echo "---"
echo "Configuration summary for $PACKAGE_NAME version $VERSION"
echo ""
echo "   * Installation prefix:        $prefix"
echo "   * System type:                $host_vendor-$host_os"
echo "   * Host CPU:                   $host_cpu"
echo "   * C Compiler:                 $CC"
echo "   * C Flags:                    $CFLAGS"
echo "   * C++ Compiler:               $CXX"
echo "   * C++ Flags:                  $CXXFLAGS"
echo "   * CPP Flags:                  $CPPFLAGS"
echo "   * CCAS Flags:                 $CCASFLAGS"
echo "   * LD Flags:                   $LDFLAGS"
echo "   * LIB Flags:                  $LIB"
echo "   * Library Suffix:             $LIBSUFFIX"

test "$ENABLED_LINUXKM" = "yes" && \
echo "   * Linux Kernel Build Root:    $KERNEL_ROOT" && \
echo "   * Linux Kernel Build Arch:    $KERNEL_ARCH" && \
echo "   * fpu disable C flags:        $CFLAGS_FPU_DISABLE" && \
echo "   * fpu enable C flags:         $CFLAGS_FPU_ENABLE" && \
echo "   * SIMD disable C flags:       $CFLAGS_SIMD_DISABLE" && \
echo "   * SIMD enable C flags:        $CFLAGS_SIMD_ENABLE" && \
echo "   * No-auto-vectorize C flags:  $CFLAGS_AUTO_VECTORIZE_DISABLE" && \
echo "   * Auto-vectorize C flags:     $CFLAGS_AUTO_VECTORIZE_ENABLE" && \
echo "   * SIMD enable as flags:       $ASFLAGS_FPU_DISABLE_SIMD_ENABLE" && \
echo "   * FPU enable as flags:        $ASFLAGS_FPU_ENABLE_SIMD_DISABLE" && \
echo "   * SIMD+FPU disable as flags:  $ASFLAGS_FPUSIMD_DISABLE" && \
echo "   * SIMD+FPU enable as flags:   $ASFLAGS_FPUSIMD_ENABLE" && \
echo "   * Linux kernel module PIE:    $ENABLED_LINUXKM_PIE"
echo "   * Linux kernel module bench:  $ENABLED_LINUXKM_BENCHMARKS"

echo "   * Debug enabled:              $ax_enable_debug"
echo "   * Coverage enabled:           $ax_enable_coverage"
echo "   * Warnings as failure:        $ac_cv_warnings_as_errors"
echo "   * make -j:                    $enable_jobserver"
echo "   * VCS checkout:               $ac_cv_vcs_checkout"
echo
echo "   Features "
if test "$ENABLED_FIPS" = "yes"; then
echo "   * FIPS:                       $FIPS_VERSION"
else
echo "   * FIPS:                       $ENABLED_FIPS"
fi
echo "   * Single threaded:            $ENABLED_SINGLETHREADED"
echo "   * Filesystem:                 $ENABLED_FILESYSTEM"
echo "   * OpenSSH Build:              $ENABLED_OPENSSH"
echo "   * OpenSSL Extra API:          $ENABLED_OPENSSLEXTRA"
echo "   * OpenSSL Coexist:            $ENABLED_OPENSSLCOEXIST"
echo "   * Old Names:                  $ENABLED_OLDNAMES"
echo "   * Max Strength Build:         $ENABLED_MAXSTRENGTH"
echo "   * Distro Build:               $ENABLED_DISTRO"
echo "   * Reproducible Build:         $ENABLED_REPRODUCIBLE_BUILD"
echo "   * Side-channel Hardening:     $ENABLED_HARDEN"

echo "   * Single Precision Math:      $ENABLED_SP"
if test "$ENABLED_SP_MATH_ALL" != "no"
then
    ENABLED_SP_MATH_DESC="all"
else
    if test "$ENABLED_SP_MATH" != "no"
    then
        ENABLED_SP_MATH_DESC="restricted"
    else
        ENABLED_SP_MATH_DESC="no"
    fi
fi
echo "   * SP implementation:          $ENABLED_SP_MATH_DESC"
echo "   * Fast Math:                  $ENABLED_FASTMATH"
echo "   * Heap Math:                  $ENABLED_HEAPMATH"
echo "   * Assembly Allowed:           $ENABLED_ASM"
echo "   * sniffer:                    $ENABLED_SNIFFER"
echo "   * snifftest:                  $ENABLED_SNIFFTEST"
echo "   * ARC4:                       $ENABLED_ARC4"
echo "   * AES:                        $ENABLED_AES"
echo "   * AES-NI:                     $ENABLED_AESNI"
echo "   * AES-CBC:                    $ENABLED_AESCBC"
echo "   * AES-CBC length checks:      $ENABLED_AESCBC_LENGTH_CHECKS"
echo "   * AES-GCM:                    $ENABLED_AESGCM"
echo "   * AES-GCM streaming:          $ENABLED_AESGCM_STREAM"
echo "   * AES-CCM:                    $ENABLED_AESCCM"
echo "   * AES-CTR:                    $ENABLED_AESCTR"
echo "   * AES-CFB:                    $ENABLED_AESCFB"
echo "   * AES-OFB:                    $ENABLED_AESOFB"
echo "   * AES-SIV:                    $ENABLED_AESSIV"
echo "   * ARIA:                       $ENABLED_ARIA"
echo "   * DES3:                       $ENABLED_DES3"
echo "   * Camellia:                   $ENABLED_CAMELLIA"
echo "   * SM4-ECB:                    $ENABLED_SM4_ECB"
echo "   * SM4-CBC:                    $ENABLED_SM4_CBC"
echo "   * SM4-CTR:                    $ENABLED_SM4_CTR"
echo "   * SM4-GCM:                    $ENABLED_SM4_GCM"
echo "   * SM4-CCM:                    $ENABLED_SM4_CCM"
echo "   * NULL Cipher:                $ENABLED_NULL_CIPHER"
echo "   * MD2:                        $ENABLED_MD2"
echo "   * MD4:                        $ENABLED_MD4"
echo "   * MD5:                        $ENABLED_MD5"
echo "   * RIPEMD:                     $ENABLED_RIPEMD"
echo "   * SHA:                        $ENABLED_SHA"
echo "   * SHA-224:                    $ENABLED_SHA224"
echo "   * SHA-384:                    $ENABLED_SHA384"
echo "   * SHA-512:                    $ENABLED_SHA512"
echo "   * SHA3:                       $ENABLED_SHA3"
echo "   * SHAKE128:                   $ENABLED_SHAKE128"
echo "   * SHAKE256:                   $ENABLED_SHAKE256"
echo "   * SM3:                        $ENABLED_SM3"
echo "   * BLAKE2:                     $ENABLED_BLAKE2"
echo "   * BLAKE2S:                    $ENABLED_BLAKE2S"
echo "   * SipHash:                    $ENABLED_SIPHASH"
echo "   * CMAC:                       $ENABLED_CMAC"
echo "   * keygen:                     $ENABLED_KEYGEN"
echo "   * certgen:                    $ENABLED_CERTGEN"
echo "   * certreq:                    $ENABLED_CERTREQ"
echo "   * certext:                    $ENABLED_CERTEXT"
echo "   * certgencache:               $ENABLED_certgencache"
echo "   * CHACHA:                     $ENABLED_CHACHA"
echo "   * XCHACHA:                    $ENABLED_XCHACHA"
echo "   * Hash DRBG:                  $ENABLED_HASHDRBG"
echo "   * MmemUse Entropy:            $ENABLED_ENTROPY_MEMUSE"
echo "   * PWDBASED:                   $ENABLED_PWDBASED"
echo "   * Encrypted keys:             $ENABLED_ENCKEYS"
echo "   * scrypt:                     $ENABLED_SCRYPT"
echo "   * wolfCrypt Only:             $ENABLED_CRYPTONLY"
echo "   * HKDF:                       $ENABLED_HKDF"
echo "   * HPKE:                       $ENABLED_HPKE"
echo "   * X9.63 KDF:                  $ENABLED_X963KDF"
echo "   * PSK:                        $ENABLED_PSK"
echo "   * Poly1305:                   $ENABLED_POLY1305"
echo "   * LEANPSK:                    $ENABLED_LEANPSK"
echo "   * LEANTLS:                    $ENABLED_LEANTLS"
echo "   * RSA:                        $ENABLED_RSA"
echo "   * RSA-PSS:                    $ENABLED_RSAPSS"
echo "   * DSA:                        $ENABLED_DSA"
echo "   * DH:                         $ENABLED_DH"
echo "   * DH Default Parameters:      $ENABLED_DHDEFAULTPARAMS"
echo "   * ECC:                        $ENABLED_ECC"
echo "   * ECC Custom Curves:          $ENABLED_ECCCUSTCURVES"
echo "   * ECC Minimum Bits:           $ENABLED_ECCMINSZ"
echo "   * FPECC:                      $ENABLED_FPECC"
echo "   * ECC_ENCRYPT:                $ENABLED_ECC_ENCRYPT"
echo "   * Brainpool:                  $ENABLED_BRAINPOOL"
echo "   * SM2:                        $ENABLED_SM2"
echo "   * CURVE25519:                 $ENABLED_CURVE25519"
echo "   * ED25519:                    $ENABLED_ED25519"
echo "   * ED25519 streaming:          $ENABLED_ED25519_STREAM"
echo "   * CURVE448:                   $ENABLED_CURVE448"
echo "   * ED448:                      $ENABLED_ED448"
echo "   * ED448 streaming:            $ENABLED_ED448_STREAM"
echo "   * LMS:                        $ENABLED_LMS"
echo "   * LMS wolfSSL impl:           $ENABLED_WC_LMS"
echo "   * KYBER:                      $ENABLED_KYBER"
echo "   * KYBER wolfSSL impl:         $ENABLED_WC_KYBER"
echo "   * ECCSI                       $ENABLED_ECCSI"
echo "   * SAKKE                       $ENABLED_SAKKE"
echo "   * ASN:                        $ENABLED_ASN"
echo "   * Anonymous cipher:           $ENABLED_ANON"
echo "   * CODING:                     $ENABLED_CODING"
echo "   * MEMORY:                     $ENABLED_MEMORY"
echo "   * I/O POOL:                   $ENABLED_IOPOOL"
echo "   * wolfSentry:                 $ENABLED_WOLFSENTRY"
echo "   * LIGHTY:                     $ENABLED_LIGHTY"
echo "   * WPA Supplicant:             $ENABLED_WPAS"
echo "   * HAPROXY:                    $ENABLED_HAPROXY"
echo "   * STUNNEL:                    $ENABLED_STUNNEL"
echo "   * tcpdump:                    $ENABLED_TCPDUMP"
echo "   * libssh2:                    $ENABLED_LIBSSH2"
echo "   * ntp:                        $ENABLED_NTP"
echo "   * rsyslog:                    $ENABLED_RSYSLOG"
echo "   * Apache httpd:               $ENABLED_APACHE_HTTPD"
echo "   * NGINX:                      $ENABLED_NGINX"
echo "   * OpenResty:                  $ENABLED_OPENRESTY"
echo "   * ASIO:                       $ENABLED_ASIO"
echo "   * LIBWEBSOCKETS:              $ENABLED_LIBWEBSOCKETS"
echo "   * Qt:                         $ENABLED_QT"
echo "   * Qt Unit Testing:            $ENABLED_QT_TEST"
echo "   * SIGNAL:                     $ENABLED_SIGNAL"
echo "   * chrony:                     $ENABLED_CHRONY"
echo "   * strongSwan:                 $ENABLED_STRONGSWAN"
echo "   * OpenLDAP:                   $ENABLED_OPENLDAP"
echo "   * hitch:                      $ENABLED_HITCH"
echo "   * ERROR_STRINGS:              $ENABLED_ERROR_STRINGS"
echo "   * DTLS:                       $ENABLED_DTLS"
echo "   * DTLS v1.3:                  $ENABLED_DTLS13"
echo "   * SCTP:                       $ENABLED_SCTP"
echo "   * SRTP:                       $ENABLED_SRTP"
echo "   * Indefinite Length:          $ENABLED_BER_INDEF"
echo "   * Multicast:                  $ENABLED_MCAST"
echo "   * SSL v3.0 (Old):             $ENABLED_SSLV3"
echo "   * TLS v1.0 (Old):             $ENABLED_TLSV10"
echo "   * TLS v1.1 (Old):             $ENABLED_OLD_TLS"
echo "   * TLS v1.2:                   $ENABLED_TLSV12"
echo "   * TLS v1.3:                   $ENABLED_TLS13"
echo "   * Post-handshake Auth:        $ENABLED_TLS13_POST_AUTH"
echo "   * Early Data:                 $ENABLED_TLS13_EARLY_DATA"
echo "   * QUIC:                       $ENABLED_QUIC"
echo "   * Send State in HRR Cookie:   $ENABLED_SEND_HRR_COOKIE"
echo "   * OCSP:                       $ENABLED_OCSP"
echo "   * OCSP Stapling:              $ENABLED_CERTIFICATE_STATUS_REQUEST"
echo "   * OCSP Stapling v2:           $ENABLED_CERTIFICATE_STATUS_REQUEST_V2"
echo "   * CRL:                        $ENABLED_CRL"
echo "   * CRL-MONITOR:                $ENABLED_CRL_MONITOR"
echo "   * Persistent session cache:   $ENABLED_SAVESESSION"
echo "   * Persistent cert    cache:   $ENABLED_SAVECERT"
echo "   * Atomic User Record Layer:   $ENABLED_ATOMICUSER"
echo "   * Public Key Callbacks:       $ENABLED_PKCALLBACKS"
echo "   * liblms:                     $ENABLED_LIBLMS"
echo "   * liboqs:                     $ENABLED_LIBOQS"
echo "   * Whitewood netRandom:        $ENABLED_WNR"
echo "   * Server Name Indication:     $ENABLED_SNI"
echo "   * ALPN:                       $ENABLED_ALPN"
echo "   * Maximum Fragment Length:    $ENABLED_MAX_FRAGMENT"
echo "   * Trusted CA Indication:      $ENABLED_TRUSTED_CA"
echo "   * Truncated HMAC:             $ENABLED_TRUNCATED_HMAC"
echo "   * Supported Elliptic Curves:  $ENABLED_SUPPORTED_CURVES"
echo "   * FFDHE only in client:       $ENABLED_FFDHE_ONLY"
echo "   * Session Ticket:             $ENABLED_SESSION_TICKET"
echo "   * Extended Master Secret:     $ENABLED_EXTENDED_MASTER"
echo "   * Renegotiation Indication:   $ENABLED_RENEGOTIATION_INDICATION"
echo "   * Secure Renegotiation:       $ENABLED_SECURE_RENEGOTIATION"
echo "   * Fallback SCSV:              $ENABLED_FALLBACK_SCSV"
echo "   * Keying Material Exporter:   $ENABLED_KEYING_MATERIAL"
echo "   * All TLS Extensions:         $ENABLED_TLSX"
echo "   * PKCS#7:                     $ENABLED_PKCS7"
echo "   * S/MIME:                     $ENABLED_SMIME"
echo "   * wolfSSH:                    $ENABLED_WOLFSSH"
echo "   * wolfEngine:                 $ENABLED_WOLFENGINE"
echo "   * wolfTPM:                    $ENABLED_WOLFTPM"
echo "   * wolfCLU:                    $ENABLED_WOLFCLU"
echo "   * wolfSCEP:                   $ENABLED_WOLFSCEP"
echo "   * Secure Remote Password:     $ENABLED_SRP"
echo "   * Small Stack:                $ENABLED_SMALL_STACK"
echo "   * Linux Kernel Module:        $ENABLED_LINUXKM"
echo "   * valgrind unit tests:        $ENABLED_VALGRIND"
echo "   * LIBZ:                       $ENABLED_LIBZ"
echo "   * Examples:                   $ENABLED_EXAMPLES"
echo "   * Crypt tests:                $ENABLED_CRYPT_TESTS"
echo "   * Stack sizes in tests:       $ENABLED_STACKSIZE"
echo "   * Heap stats in tests:        $ENABLED_TRACKMEMORY"
echo "   * User Crypto:                $ENABLED_USER_CRYPTO"
echo "   * Fast RSA:                   $ENABLED_FAST_RSA"
echo "   * Async Crypto:               $ENABLED_ASYNCCRYPT"
echo "   * PKCS#8:                     $ENABLED_PKCS8"
echo "   * PKCS#11:                    $ENABLED_PKCS11"
echo "   * PKCS#12:                    $ENABLED_PKCS12"
echo "   * Cavium Nitrox:              $ENABLED_CAVIUM"
echo "   * Cavium Octeon (Sync):       $ENABLED_OCTEON_SYNC"
echo "   * Intel Quick Assist:         $ENABLED_INTEL_QA"
if test "$ENABLED_ARMASM_INLINE" = "yes"
then
    ENABLED_ARMASM="inline C"
fi
echo "   * ARM ASM:                    $ENABLED_ARMASM"
echo "   * ARM ASM SHA512/SHA3 Crypto  $ENABLED_ARMASM_SHA3"
echo "   * ARM ASM SM3/SM4 Crypto      $ENABLED_ARMASM_CRYPTO_SM4"
echo "   * AES Key Wrap:               $ENABLED_AESKEYWRAP"
echo "   * Write duplicate:            $ENABLED_WRITEDUP"
echo "   * Xilinx Hardware Acc.:       $ENABLED_XILINX"
echo "   * Inline Code:                $ENABLED_INLINE"
echo "   * Linux AF_ALG:               $ENABLED_AFALG"
echo "   * Linux KCAPI:                $ENABLED_KCAPI"
echo "   * Linux devcrypto:            $ENABLED_DEVCRYPTO"
echo "   * Crypto callbacks:           $ENABLED_CRYPTOCB"
echo "   * i.MX CAAM:                  $ENABLED_CAAM"
echo "   * IoT-Safe:                   $ENABLED_IOTSAFE"
echo "   * IoT-Safe HWRNG:             $ENABLED_IOTSAFE_HWRNG"
echo "   * NXP SE050:                  $ENABLED_SE050"
echo "   * Maxim Integrated MAXQ10XX:  $ENABLED_MAXQ10XX"
echo "   * PSA:                        $ENABLED_PSA"
echo "   * System CA certs:            $ENABLED_SYS_CA_CERTS"
echo "   * ERR Queues per Thread:      $ENABLED_ERRORQUEUEPERTHREAD"
echo "   * rwlock:                     $ENABLED_RWLOCK"
echo ""
echo "---"

fi # $silent != yes

################################################################################
# Show warnings at bottom so they are noticed
################################################################################

if test "$ENABLED_ASYNCCRYPT" = "yes" && ! test -s $srcdir/wolfcrypt/src/async.c
then
    AC_MSG_WARN([Make sure real async files are loaded. Contact wolfSSL for details on using the asynccrypt option.])
fi

# MinGW static vs shared library
# Reference URL from libtool for MinGW is located at
# http://www.gnu.org/software/libtool/manual/libtool.html#Cygwin-to-MinGW-Cross
# this allows for not even having dllimport/dllexport on functions
# with recent libtools, only requiring it with global variables.
#
# The following warning is displayed here because if not using "contemporary GNU
# tools" there is the possibility of export/import issues.
# wolfSSL uses __declspec(dllexport) and "contemporary GNU tools" handle the
# case where both static and shared libraries are built.
#
# More can be found about the MinGW linker at
# https://sourceware.org/binutils/docs/ld/WIN32.html
if test "$MINGW_LIB_WARNING" = "yes"
then
    AC_MSG_WARN([Building with shared and static library at the same time on this system may cause export/import problems when using non contemporary GNU tools.])
fi
