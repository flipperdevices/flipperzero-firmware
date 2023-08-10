# ===========================================================================
#       https://www.gnu.org/software/autoconf-archive/ax_am_macros.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_AM_MACROS
#
# DESCRIPTION
#
#   Adds support for macros that create Make rules. You must manually add
#   the following line
#
#     @INC_AMINCLUDE@
#
#   to your Makefile.in (or Makefile.am if you use Automake) files.
#
# LICENSE
#
#   Copyright (c) 2009 Tom Howard <tomhoward@users.sf.net>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 11

AC_DEFUN([AX_AM_MACROS],
[
AC_MSG_NOTICE([adding automake macro support])
AMINCLUDE="aminclude.am"
AC_SUBST(AMINCLUDE)
AC_MSG_NOTICE([creating $AMINCLUDE])
AMINCLUDE_TIME=`LC_ALL=C date`
AX_PRINT_TO_FILE([$AMINCLUDE],[[
# generated automatically by configure from AX_AUTOMAKE_MACROS
# on $AMINCLUDE_TIME

]])

INC_AMINCLUDE="include \$(top_builddir)/$AMINCLUDE"
AC_SUBST(INC_AMINCLUDE)
])
