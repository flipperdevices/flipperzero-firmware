# ax_linuxkm.m4 -- macros for getting attributes of default configured kernel
#
# Copyright (C) 2006-2023 wolfSSL Inc.
#
# This file is part of wolfSSL.
#
# wolfSSL is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# wolfSSL is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA

AC_DEFUN([AC_PATH_DEFAULT_KERNEL_SOURCE],
[
AC_MSG_CHECKING([for default kernel build root])
if test -d /lib/modules/`uname -r`/build/.config; then
  DEFAULT_KERNEL_ROOT=/lib/modules/`uname -r`/build
  AC_MSG_RESULT([$DEFAULT_KERNEL_ROOT])
elif test -r /usr/src/linux/.config; then
  DEFAULT_KERNEL_ROOT=/usr/src/linux
  AC_MSG_RESULT([$DEFAULT_KERNEL_ROOT])
else
  AC_MSG_RESULT([no default configured kernel found])
fi
])

AC_DEFUN([AC_DEFAULT_KERNEL_ARCH],
[
AC_REQUIRE([AC_PROG_AWK])
AC_MSG_CHECKING([for default kernel arch])
if test -f ${KERNEL_ROOT}/.config; then
  # "# Linux/x86 5.8.1-gentoo Kernel Configuration"
  DEFAULT_KERNEL_ARCH=`$AWK '/^# Linux/\
{split($[]2,arch_fields,"/"); print arch_fields[[2]]; exit(0);}' ${KERNEL_ROOT}/.config`
fi
if test -n "$DEFAULT_KERNEL_ARCH"; then
  AC_MSG_RESULT([$DEFAULT_KERNEL_ARCH])
else
  AC_MSG_RESULT([no default configured kernel arch found])
fi
])


AC_DEFUN([AX_SIMD_CC_COMPILER_FLAGS], [
    AX_REQUIRE_DEFINED([AX_APPEND_COMPILE_FLAGS])
    AC_REQUIRE([AX_VCS_CHECKOUT])
    AC_REQUIRE([AX_DEBUG])

    AC_LANG_PUSH([C])

    if test "$CFLAGS_FPU_DISABLE" = ""; then
        AX_APPEND_COMPILE_FLAGS([-mno-80387],[CFLAGS_FPU_DISABLE])
        AX_APPEND_COMPILE_FLAGS([-mno-fp-ret-in-387],[CFLAGS_FPU_DISABLE])
        AX_APPEND_COMPILE_FLAGS([-mno-fpu],[CFLAGS_FPU_DISABLE])
    fi

    if test "$CFLAGS_FPU_ENABLE" = ""; then
        AX_APPEND_COMPILE_FLAGS([-m80387],[CFLAGS_FPU_ENABLE])
        AX_APPEND_COMPILE_FLAGS([-mfpu],[CFLAGS_FPU_ENABLE])
    fi

    if test "$CFLAGS_SIMD_DISABLE" = ""; then
        AX_APPEND_COMPILE_FLAGS([-mno-sse],[CFLAGS_SIMD_DISABLE])
        AX_APPEND_COMPILE_FLAGS([-mgeneral-regs-only],[CFLAGS_SIMD_DISABLE])
    fi

    if test "$CFLAGS_SIMD_ENABLE" = ""; then
        AX_APPEND_COMPILE_FLAGS([-msse],[CFLAGS_SIMD_ENABLE])
        AX_APPEND_COMPILE_FLAGS([-mmmx],[CFLAGS_SIMD_ENABLE])
        AX_APPEND_COMPILE_FLAGS([-msse2],[CFLAGS_SIMD_ENABLE])
        AX_APPEND_COMPILE_FLAGS([-msse4],[CFLAGS_SIMD_ENABLE])
        AX_APPEND_COMPILE_FLAGS([-mavx],[CFLAGS_SIMD_ENABLE])
        AX_APPEND_COMPILE_FLAGS([-mavx2],[CFLAGS_SIMD_ENABLE])
        AX_APPEND_COMPILE_FLAGS([-mno-general-regs-only],[CFLAGS_SIMD_ENABLE])
    fi

    if test "$CFLAGS_AUTO_VECTORIZE_DISABLE" = ""; then
        AX_APPEND_COMPILE_FLAGS([-fno-builtin],[CFLAGS_AUTO_VECTORIZE_DISABLE])
        AX_APPEND_COMPILE_FLAGS([-fno-tree-vectorize],[CFLAGS_AUTO_VECTORIZE_DISABLE])
        AX_APPEND_COMPILE_FLAGS([-fno-tree-loop-vectorize],[CFLAGS_AUTO_VECTORIZE_DISABLE])
        AX_APPEND_COMPILE_FLAGS([-fno-tree-slp-vectorize],[CFLAGS_AUTO_VECTORIZE_DISABLE])
    fi

    if test "$CFLAGS_AUTO_VECTORIZE_ENABLE" = ""; then
        AX_APPEND_COMPILE_FLAGS([-fbuiltin],[CFLAGS_AUTO_VECTORIZE_ENABLE])
        AX_APPEND_COMPILE_FLAGS([-ftree-vectorize],[CFLAGS_AUTO_VECTORIZE_ENABLE])
        AX_APPEND_COMPILE_FLAGS([-ftree-loop-vectorize],[CFLAGS_AUTO_VECTORIZE_ENABLE])
        AX_APPEND_COMPILE_FLAGS([-ftree-slp-vectorize],[CFLAGS_AUTO_VECTORIZE_ENABLE])
    fi

    case "$host_cpu" in
    x86_64)
        # note that gnu as accepts archs of the form -march=+no387, signifying the
        # default target arch modified with no387.  by default, we use that here.
        if test "$ASFLAGS_FPU_DISABLE_SIMD_ENABLE" = ""; then
            AX_APPEND_COMPILE_FLAGS([-Wa,-march="${BASE_TARGET_ARCH}+no87+no287+no387+no687+sse+sse2+sse3+ssse3+sse4.1+sse4.2+sse4+avx+avx2+avx512f"],[ASFLAGS_FPU_DISABLE_SIMD_ENABLE])
        fi

        if test "$ASFLAGS_FPU_ENABLE_SIMD_DISABLE" = ""; then
            AX_APPEND_COMPILE_FLAGS([-Wa,-march="${BASE_TARGET_ARCH}+387+687+nosse+nosse2+nosse3+nossse3+nosse4.1+nosse4.2+nosse4+noavx+noavx2+noavx512f"],[ASFLAGS_FPU_ENABLE_SIMD_DISABLE])
        fi

        if test "$ASFLAGS_FPUSIMD_DISABLE" = ""; then
            AX_APPEND_COMPILE_FLAGS([-Wa,-march="${BASE_TARGET_ARCH}+no387+no687+nosse+nosse2+nosse3+nossse3+nosse4.1+nosse4.2+nosse4+noavx+noavx2+noavx512f"],[ASFLAGS_FPUSIMD_DISABLE])
        fi

        if test "$ASFLAGS_FPUSIMD_ENABLE" = ""; then
            AX_APPEND_COMPILE_FLAGS([-Wa,-march="${BASE_TARGET_ARCH}+387+687+sse+sse2+sse3+ssse3+sse4.1+sse4.2+sse4+avx+avx2+avx512f"],[ASFLAGS_FPUSIMD_ENABLE])
        fi

        ;;
    aarch64)
        if test "$BASE_TARGET_ARCH" = ""; then
            BASE_TARGET_ARCH=all
        fi

        if test "$ASFLAGS_FPU_DISABLE_SIMD_ENABLE" = ""; then
            AX_APPEND_COMPILE_FLAGS([-Wa,-march="${BASE_TARGET_ARCH}+nofpu+simd"],[ASFLAGS_FPU_DISABLE_SIMD_ENABLE])
        fi

        if test "$ASFLAGS_FPU_ENABLE_SIMD_DISABLE" = ""; then
            AX_APPEND_COMPILE_FLAGS([-Wa,-march="${BASE_TARGET_ARCH}+fpu+nosimd"],[ASFLAGS_FPU_ENABLE_SIMD_DISABLE])
        fi

        if test "$ASFLAGS_FPUSIMD_DISABLE" = ""; then
            AX_APPEND_COMPILE_FLAGS([-Wa,-march="${BASE_TARGET_ARCH}+nofpu+nosimd"],[ASFLAGS_FPUSIMD_DISABLE])
        fi

        if test "$ASFLAGS_FPUSIMD_ENABLE" = ""; then
            AX_APPEND_COMPILE_FLAGS([-Wa,-march="${BASE_TARGET_ARCH}+fpu+simd"],[ASFLAGS_FPUSIMD_ENABLE])
        fi

        ;;
    *)
        # fall back to defining only $ASFLAGS_FPUSIMD_DISABLE
        if test "$BASE_TARGET_ARCH" = ""; then
            BASE_TARGET_ARCH=all
        fi

        if test "$ASFLAGS_FPUSIMD_DISABLE" = ""; then
            AX_APPEND_COMPILE_FLAGS([-Wa,-march="${BASE_TARGET_ARCH}+nofpu+nosimd"],[ASFLAGS_FPUSIMD_DISABLE])
        fi
        ;;
    esac

    AC_LANG_POP
])
