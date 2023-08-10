# ===========================================================================
#     https://www.gnu.org/software/autoconf-archive/ax_check_library.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CHECK_LIBRARY(VARIABLE-PREFIX, HEADER-FILE, LIBRARY-FILE,
#                    [ACTION-IF-FOUND], [ACTION-IF-NOT_FOUND])
#
# DESCRIPTION
#
#   Provides a generic test for a given library, similar in concept to the
#   PKG_CHECK_MODULES macro used by pkg-config.
#
#   Most simplest libraries can be checked against simply through the
#   presence of a header file and a library to link to. This macro allows to
#   wrap around the test so that it doesn't have to be recreated each time.
#
#   Rather than define --with-$LIBRARY arguments, it uses variables in the
#   same way that PKG_CHECK_MODULES does. It doesn't, though, use the same
#   names, since you shouldn't provide a value for LIBS or CFLAGS but rather
#   for LDFLAGS and CPPFLAGS, to tell the linker and compiler where to find
#   libraries and headers respectively.
#
#   If the library is find, HAVE_PREFIX is defined, and in all cases
#   PREFIX_LDFLAGS and PREFIX_CPPFLAGS are substituted.
#
#   Example:
#
#     AX_CHECK_LIBRARY([LIBEVENT], [event.h], [event], [],
#                      [AC_MSG_ERROR([Unable to find libevent])])
#
# LICENSE
#
#   Copyright (c) 2010 Diego Elio Petteno` <flameeyes@gmail.com>
#
#   This program is free software: you can redistribute it and/or modify it
#   under the terms of the GNU General Public License as published by the
#   Free Software Foundation, either version 3 of the License, or (at your
#   option) any later version.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
#   Public License for more details.
#
#   You should have received a copy of the GNU General Public License along
#   with this program. If not, see <https://www.gnu.org/licenses/>.
#
#   As a special exception, the respective Autoconf Macro's copyright owner
#   gives unlimited permission to copy, distribute and modify the configure
#   scripts that are the output of Autoconf when processing the Macro. You
#   need not follow the terms of the GNU General Public License when using
#   or distributing such scripts, even though portions of the text of the
#   Macro appear in them. The GNU General Public License (GPL) does govern
#   all other use of the material that constitutes the Autoconf Macro.
#
#   This special exception to the GPL applies to versions of the Autoconf
#   Macro released by the Autoconf Archive. When you make and distribute a
#   modified version of the Autoconf Macro, you may extend this special
#   exception to the GPL to apply to your modified version as well.

#serial 5

AC_DEFUN([AX_CHECK_LIBRARY], [
  AC_ARG_VAR($1[_CPPFLAGS], [C preprocessor flags for ]$1[ headers])
  AC_ARG_VAR($1[_LDFLAGS], [linker flags for ]$1[ libraries])

  AC_CACHE_VAL(AS_TR_SH([ax_cv_have_]$1),
    [save_CPPFLAGS="$CPPFLAGS"
     save_LDFLAGS="$LDFLAGS"
     save_LIBS="$LIBS"

     AS_IF([test "x$]$1[_CPPFLAGS" != "x"],
       [CPPFLAGS="$CPPFLAGS $]$1[_CPPFLAGS"])

     AS_IF([test "x$]$1[_LDFLAGS" != "x"],
       [LDFLAGS="$LDFLAGS $]$1[_LDFLAGS"])

     AC_CHECK_HEADER($2, [
       AC_CHECK_LIB($3, [main],
         [AS_TR_SH([ax_cv_have_]$1)=yes],
         [AS_TR_SH([ax_cv_have_]$1)=no])
     ], [AS_TR_SH([ax_cv_have_]$1)=no])

     CPPFLAGS="$save_CPPFLAGS"
     LDFLAGS="$save_LDFLAGS"
     LIBS="$save_LIBS"
    ])

  AS_IF([test "$]AS_TR_SH([ax_cv_have_]$1)[" = "yes"],
    AC_DEFINE([HAVE_]$1, [1], [Define to 1 if ]$1[ is found])
    [$4],
    [$5])
])
