# ===========================================================================
#      https://github.com/BrianAker/ddm4/
# ===========================================================================
#
# SYNOPSIS
#
#   AX_HARDEN_COMPILER_FLAGS()
#   AX_HARDEN_LINKER_FLAGS()
#   AX_HARDEN_CC_COMPILER_FLAGS()
#   AX_HARDEN_CXX_COMPILER_FLAGS()
#
# DESCRIPTION
#
#   Any compiler flag that "hardens" or tests code. C99 is assumed.
#
#   NOTE: Implementation based on AX_APPEND_FLAG.
#
# LICENSE
#
#  Copyright (C) 2012 Brian Aker
#  All rights reserved.
#  
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are
#  met:
#  
#      * Redistributions of source code must retain the above copyright
#  notice, this list of conditions and the following disclaimer.
#  
#      * Redistributions in binary form must reproduce the above
#  copyright notice, this list of conditions and the following disclaimer
#  in the documentation and/or other materials provided with the
#  distribution.
#  
#      * The names of its contributors may not be used to endorse or
#  promote products derived from this software without specific prior
#  written permission.
#  
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
#  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
#  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
#  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
#  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
#  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
#  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
#  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
#  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
#  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# The Following flags are not checked for
# -Wdeclaration-after-statement is counter to C99
# AX_APPEND_COMPILE_FLAGS([-std=c++11]) -- Not ready yet
# AX_APPEND_COMPILE_FLAGS([-pedantic]) -- ?
# AX_APPEND_COMPILE_FLAGS([-Wstack-protector]) -- Issues on 32bit compile
# AX_APPEND_COMPILE_FLAGS([-fstack-protector-all]) -- Issues on 32bit compile
# AX_APPEND_COMPILE_FLAGS([-Wlong-long]) -- Don't turn on for compatibility issues memcached_stat_st
# AX_APPEND_COMPILE_FLAGS([-Wold-style-definition],,[$ax_append_compile_cflags_extra])
# AX_APPEND_COMPILE_FLAGS([-std=c99],,[$ax_append_compile_cflags_extra])
# AX_APPEND_COMPILE_FLAGS([-Wlogical-op],,[$ax_append_compile_cflags_extra])
# AX_APPEND_COMPILE_FLAGS([-fstack-check],,[$ax_append_compile_cflags_extra]) -- problems with fastmath stack size checks
# AX_APPEND_COMPILE_FLAGS([-floop-parallelize-all],,[$ax_append_compile_cflags_extra]) -- causes RSA verify problem on x64
# AX_APPEND_COMPILE_FLAGS([-Wunreachable-code],,[$ax_append_compile_cflags_extra])  -- older clang and when gcc had it are buggy
# AX_APPEND_COMPILE_FLAGS([-fPIE],,[$ax_append_compile_cflags_extra]) -- Flag for executables not libraries

#serial 4.2
# changes: deleted the clearing of CFLAGS

  AC_DEFUN([AX_HARDEN_LINKER_FLAGS], [
      AX_REQUIRE_DEFINED([AX_CHECK_LINK_FLAG])
      AC_REQUIRE([AX_VCS_CHECKOUT])
      AC_REQUIRE([AX_DEBUG])

      dnl If we are inside of VCS we append -Werror, otherwise we just use it to test other flags
      AX_HARDEN_LIB=
      ax_append_compile_link_flags_extra=
      AS_IF([test "x$ac_cv_vcs_checkout" = "xyes"],[
        AX_CHECK_LINK_FLAG([-Werror],[
          AX_HARDEN_LIB="-Werror $AX_HARDEN_LIB"
          ])
        ],[
        AX_CHECK_LINK_FLAG([-Werror],[
          ax_append_compile_link_flags_extra='-Werror'
          ])
        ])

      AX_CHECK_LINK_FLAG([-z relro -z now],[
        AX_HARDEN_LIB="-z relro -z now $AX_HARDEN_LIB"
        ],,[$ax_append_compile_link_flags_extra])

      AX_CHECK_LINK_FLAG([-pie],[
          AX_HARDEN_LIB="-pie $AX_HARDEN_LIB"
          ],,[$ax_append_compile_link_flags_extra])

      LIB="$LIB $AX_HARDEN_LIB"
      ])

  AC_DEFUN([AX_HARDEN_CC_COMPILER_FLAGS], [
      AX_REQUIRE_DEFINED([AX_APPEND_COMPILE_FLAGS])
      AC_REQUIRE([AX_HARDEN_LINKER_FLAGS])

      AC_LANG_PUSH([C])

      ac_cv_warnings_as_errors=no
      ax_append_compile_cflags_extra=
      AS_IF([test "$ac_cv_vcs_checkout" = "yes"],[
        AX_APPEND_COMPILE_FLAGS([-Werror])
        ac_cv_warnings_as_errors=yes
        ],[
        AX_APPEND_COMPILE_FLAGS([-Werror],[ax_append_compile_cflags_extra])
        ])

      AS_IF([test "$ax_enable_debug" = "yes"], [
        AX_APPEND_COMPILE_FLAGS([-g])
        AX_APPEND_COMPILE_FLAGS([-ggdb],,[$ax_append_compile_cflags_extra])
        AX_APPEND_COMPILE_FLAGS([-O0],,[$ax_append_compile_cflags_extra])
        ],[])

      AX_APPEND_COMPILE_FLAGS([-Wno-pragmas],,[$ax_append_compile_cflags_extra])

      AX_APPEND_COMPILE_FLAGS([-Wall],,[$ax_append_compile_cflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wextra],,[$ax_append_compile_cflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wunknown-pragmas],,[$ax_append_compile_cflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wthis-test-should-fail],,[$ax_append_compile_cflags_extra])
      dnl Anything below this comment please keep sorted.
      AS_IF([test "$CC" = "clang"],[],[
        AX_APPEND_COMPILE_FLAGS([--param=ssp-buffer-size=1],,[$ax_append_compile_cflags_extra])
        ])
      AX_APPEND_COMPILE_FLAGS([-Waddress],,[$ax_append_compile_cflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Warray-bounds],,[$ax_append_compile_cflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wbad-function-cast],,[$ax_append_compile_cflags_extra])
      dnl Not in use -Wc++-compat
      AX_APPEND_COMPILE_FLAGS([-Wchar-subscripts],,[$ax_append_compile_cflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wcomment],,[$ax_append_compile_cflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wfloat-equal],,[$ax_append_compile_cflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wformat-security],,[$ax_append_compile_cflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wformat=2],,[$ax_append_compile_cflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wmaybe-uninitialized],,[$ax_append_compile_cflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wmissing-field-initializers],,[$ax_append_compile_cflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wmissing-noreturn],,[$ax_append_compile_cflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wmissing-prototypes],,[$ax_append_compile_cflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wnested-externs],,[$ax_append_compile_cflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wnormalized=id],,[$ax_append_compile_cflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Woverride-init],,[$ax_append_compile_cflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wpointer-arith],,[$ax_append_compile_cflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wpointer-sign],,[$ax_append_compile_cflags_extra])
dnl      AX_APPEND_COMPILE_FLAGS([-Wredundant-decls],,[$ax_append_compile_cflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wshadow],,[$ax_append_compile_cflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wshorten-64-to-32],,[$ax_append_compile_cflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wsign-compare],,[$ax_append_compile_cflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wstrict-overflow=1],,[$ax_append_compile_cflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wstrict-prototypes],,[$ax_append_compile_cflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wswitch-enum],,[$ax_append_compile_cflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wundef],,[$ax_append_compile_cflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wunused],,[$ax_append_compile_cflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wunused-result],,[$ax_append_compile_cflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wunused-variable],,[$ax_append_compile_cflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wwrite-strings],,[$ax_append_compile_cflags_extra])
      AX_APPEND_COMPILE_FLAGS([-fwrapv],,[$ax_append_compile_cflags_extra])
      AC_LANG_POP
      ])

  AC_DEFUN([AX_HARDEN_CXX_COMPILER_FLAGS], [
      AC_REQUIRE_DEFINED([AX_APPEND_COMPILE_FLAGS])
      AC_REQUIRE([AX_HARDEN_CC_COMPILER_FLAGS])
      AC_LANG_PUSH([C++])

      ax_append_compile_cxxflags_extra=
      AS_IF([test "$ac_cv_warnings_as_errors" = "yes"],[
        AX_APPEND_COMPILE_FLAGS([-Werror])
        ],[
        AX_APPEND_COMPILE_FLAGS([-Werror],[ax_append_compile_cxxflags_extra])
        ])

      AS_IF([test "$ax_enable_debug" = "yes" ], [
        AX_APPEND_COMPILE_FLAGS([-g],,[$ax_append_compile_cxxflags_extra])
        AX_APPEND_COMPILE_FLAGS([-O0],,[$ax_append_compile_cxxflags_extra])
        AX_APPEND_COMPILE_FLAGS([-ggdb],,[$ax_append_compile_cxxflags_extra])
        ],[
        AX_APPEND_COMPILE_FLAGS([-D_FORTIFY_SOURCE=2],,[$ax_append_compile_cxxflags_extra])
        ])

      AS_IF([test "$ac_cv_vcs_checkout" = "yes" ], [
        AX_APPEND_COMPILE_FLAGS([-Werror],,[$ax_append_compile_cxxflags_extra])
        ],[
        AX_APPEND_COMPILE_FLAGS([-Wno-pragmas],,[$ax_append_compile_cxxflags_extra])
        ])

      AX_APPEND_COMPILE_FLAGS([-Wall],,[$ax_append_compile_cxxflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wextra],,[$ax_append_compile_cxxflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wunknown-pragmas],,[$ax_append_compile_cxxflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wthis-test-should-fail],,[$ax_append_compile_cxxflags_extra])
      dnl Anything below this comment please keep sorted.
      AX_APPEND_COMPILE_FLAGS([--param=ssp-buffer-size=1],,[$ax_append_compile_cxxflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Waddress],,[$ax_append_compile_cxxflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Warray-bounds],,[$ax_append_compile_cxxflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wchar-subscripts],,[$ax_append_compile_cxxflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wcomment],,[$ax_append_compile_cxxflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wctor-dtor-privacy],,[$ax_append_compile_cxxflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wfloat-equal],,[$ax_append_compile_cxxflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wformat=2],,[$ax_append_compile_cxxflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wmaybe-uninitialized],,[$ax_append_compile_cxxflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wmissing-field-initializers],,[$ax_append_compile_cxxflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wmissing-noreturn],,[$ax_append_compile_cxxflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wnon-virtual-dtor],,[$ax_append_compile_cxxflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wnormalized=id],,[$ax_append_compile_cxxflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Woverloaded-virtual],,[$ax_append_compile_cxxflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wpointer-arith],,[$ax_append_compile_cxxflags_extra])
dnl      AX_APPEND_COMPILE_FLAGS([-Wredundant-decls],,[$ax_append_compile_cxxflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wshadow],,[$ax_append_compile_cxxflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wshorten-64-to-32],,[$ax_append_compile_cxxflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wsign-compare],,[$ax_append_compile_cxxflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wstrict-overflow=1],,[$ax_append_compile_cxxflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wswitch-enum],,[$ax_append_compile_cxxflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wundef],,[$ax_append_compile_cxxflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wc++11-compat],,[$ax_append_compile_cxxflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wunused],,[$ax_append_compile_cxxflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wunused-result],,[$ax_append_compile_cxxflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wunused-variable],,[$ax_append_compile_cxxflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wwrite-strings],,[$ax_append_compile_cxxflags_extra])
      AX_APPEND_COMPILE_FLAGS([-Wformat-security],,[$ax_append_compile_cxxflags_extra])
      AX_APPEND_COMPILE_FLAGS([-fwrapv],,[$ax_append_compile_cxxflags_extra])
      AC_LANG_POP
  ])

  AC_DEFUN([AX_HARDEN_COMPILER_FLAGS], [
      AC_REQUIRE([AX_HARDEN_CXX_COMPILER_FLAGS])
      ])

  AC_DEFUN([AX_CC_OTHER_FLAGS], [
      AX_REQUIRE_DEFINED([AX_APPEND_COMPILE_FLAGS])
      AC_REQUIRE([AX_HARDEN_CC_COMPILER_FLAGS])

      AC_LANG_PUSH([C])
      AX_APPEND_COMPILE_FLAGS([-pipe],,[$ax_append_compile_cflags_extra])
      AC_LANG_POP
      ])
