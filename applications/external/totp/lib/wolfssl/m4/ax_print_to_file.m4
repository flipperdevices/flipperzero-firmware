# ===========================================================================
#     https://www.gnu.org/software/autoconf-archive/ax_print_to_file.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_PRINT_TO_FILE([FILE],[DATA])
#
# DESCRIPTION
#
#   Writes the specified data to the specified file.
#
# LICENSE
#
#   Copyright (c) 2008 Tom Howard <tomhoward@users.sf.net>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 8

AC_DEFUN([AX_PRINT_TO_FILE],[
AC_REQUIRE([AX_FILE_ESCAPES])
printf "$2" > "$1"
])
