# ===========================================================================
#     https://www.gnu.org/software/autoconf-archive/ax_am_jobserver.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_AM_JOBSERVER([default_value])
#
# DESCRIPTION
#
#   Enables the use of make's jobserver for the purpose of parallel building
#   by passing the -j option to make.
#
#   The option --enable-jobserver is added to configure which can accept a
#   yes, no, or an integer. The integer is the number of separate jobs to
#   allow. If 'yes' is given, then the is assumed to be one more than the
#   number of CPUs (determined through AX_COUNT_CPUS). If the value of no is
#   given, then the jobserver is disabled. The default value is given by the
#   first argument of the macro, or 'yes' if the argument is omitted.
#
#   This macro makes use of AX_AM_MACROS, so you must add the following line
#
#     @INC_AMINCLUDE@
#
#   to your Makefile.am files.
#
# LICENSE
#
#   Copyright (c) 2008 Michael Paul Bailey <jinxidoru@byu.net>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 8.1
# Local update: Make the increment of enable-jobserver so it isn't executable
# and is more posix syntax.

AC_DEFUN([AX_AM_JOBSERVER], [
    AC_REQUIRE([AX_COUNT_CPUS])
    AC_REQUIRE([AX_AM_MACROS])
    AC_ARG_ENABLE( jobserver,
    [  --enable-jobserver@<:@=no/yes/@%:@@:>@ default=m4_ifval([$1],[$1],[yes])
                        Enable up to @%:@ make jobs
                        yes: enable one more than CPU count
    ],, [enable_jobserver=m4_ifval([$1],[$1],[yes])])
    if test "x$enable_jobserver" = "xyes"; then
        enable_jobserver=$CPU_COUNT
        : $((enable_jobserver+=1))
    fi
    m4_pattern_allow(AM_MAKEFLAGS)
    if test "x$enable_jobserver" != "xno"; then
        AC_MSG_NOTICE([added jobserver support to make for $enable_jobserver jobs])
        AX_ADD_AM_MACRO( AM_MAKEFLAGS += -j$enable_jobserver )
    fi
])
