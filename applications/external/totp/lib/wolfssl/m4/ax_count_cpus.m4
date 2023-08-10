# ===========================================================================
#      https://www.gnu.org/software/autoconf-archive/ax_count_cpus.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_COUNT_CPUS([ACTION-IF-DETECTED],[ACTION-IF-NOT-DETECTED])
#
# DESCRIPTION
#
#   Attempt to count the number of logical processor cores (including
#   virtual and HT cores) currently available to use on the machine and
#   place detected value in CPU_COUNT variable.
#
#   On successful detection, ACTION-IF-DETECTED is executed if present. If
#   the detection fails, then ACTION-IF-NOT-DETECTED is triggered. The
#   default ACTION-IF-NOT-DETECTED is to set CPU_COUNT to 1.
#
# LICENSE
#
#   Copyright (c) 2014,2016 Karlson2k (Evgeny Grin) <k2k@narod.ru>
#   Copyright (c) 2012 Brian Aker <brian@tangent.org>
#   Copyright (c) 2008 Michael Paul Bailey <jinxidoru@byu.net>
#   Copyright (c) 2008 Christophe Tournayre <turn3r@users.sourceforge.net>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 22

  AC_DEFUN([AX_COUNT_CPUS],[dnl
      AC_REQUIRE([AC_CANONICAL_HOST])dnl
      AC_REQUIRE([AC_PROG_EGREP])dnl
      AC_MSG_CHECKING([the number of available CPUs])
      CPU_COUNT="0"

      # Try generic methods

      # 'getconf' is POSIX utility, but '_NPROCESSORS_ONLN' and
      # 'NPROCESSORS_ONLN' are platform-specific
      command -v getconf >/dev/null 2>&1 && \
        CPU_COUNT=`getconf _NPROCESSORS_ONLN 2>/dev/null || getconf NPROCESSORS_ONLN 2>/dev/null` || CPU_COUNT="0"
      AS_IF([[test "$CPU_COUNT" -gt "0" 2>/dev/null || ! command -v nproc >/dev/null 2>&1]],[[: # empty]],[dnl
        # 'nproc' is part of GNU Coreutils and is widely available
        CPU_COUNT=`OMP_NUM_THREADS='' nproc 2>/dev/null` || CPU_COUNT=`nproc 2>/dev/null` || CPU_COUNT="0"
      ])dnl

      AS_IF([[test "$CPU_COUNT" -gt "0" 2>/dev/null]],[[: # empty]],[dnl
        # Try platform-specific preferred methods
        AS_CASE([[$host_os]],dnl
          [[*linux*]],[[CPU_COUNT=`lscpu -p 2>/dev/null | $EGREP -e '^@<:@0-9@:>@+,' -c` || CPU_COUNT="0"]],dnl
          [[*darwin*]],[[CPU_COUNT=`sysctl -n hw.logicalcpu 2>/dev/null` || CPU_COUNT="0"]],dnl
          [[freebsd*]],[[command -v sysctl >/dev/null 2>&1 && CPU_COUNT=`sysctl -n kern.smp.cpus 2>/dev/null` || CPU_COUNT="0"]],dnl
          [[netbsd*]], [[command -v sysctl >/dev/null 2>&1 && CPU_COUNT=`sysctl -n hw.ncpuonline 2>/dev/null` || CPU_COUNT="0"]],dnl
          [[solaris*]],[[command -v psrinfo >/dev/null 2>&1 && CPU_COUNT=`psrinfo 2>/dev/null | $EGREP -e '^@<:@0-9@:>@.*on-line' -c 2>/dev/null` || CPU_COUNT="0"]],dnl
          [[mingw*]],[[CPU_COUNT=`ls -qpU1 /proc/registry/HKEY_LOCAL_MACHINE/HARDWARE/DESCRIPTION/System/CentralProcessor/ 2>/dev/null | $EGREP -e '^@<:@0-9@:>@+/' -c` || CPU_COUNT="0"]],dnl
          [[msys*]],[[CPU_COUNT=`ls -qpU1 /proc/registry/HKEY_LOCAL_MACHINE/HARDWARE/DESCRIPTION/System/CentralProcessor/ 2>/dev/null | $EGREP -e '^@<:@0-9@:>@+/' -c` || CPU_COUNT="0"]],dnl
          [[cygwin*]],[[CPU_COUNT=`ls -qpU1 /proc/registry/HKEY_LOCAL_MACHINE/HARDWARE/DESCRIPTION/System/CentralProcessor/ 2>/dev/null | $EGREP -e '^@<:@0-9@:>@+/' -c` || CPU_COUNT="0"]]dnl
        )dnl
      ])dnl

      AS_IF([[test "$CPU_COUNT" -gt "0" 2>/dev/null || ! command -v sysctl >/dev/null 2>&1]],[[: # empty]],[dnl
        # Try less preferred generic method
        # 'hw.ncpu' exist on many platforms, but not on GNU/Linux
        CPU_COUNT=`sysctl -n hw.ncpu 2>/dev/null` || CPU_COUNT="0"
      ])dnl

      AS_IF([[test "$CPU_COUNT" -gt "0" 2>/dev/null]],[[: # empty]],[dnl
      # Try platform-specific fallback methods
      # They can be less accurate and slower then preferred methods
        AS_CASE([[$host_os]],dnl
          [[*linux*]],[[CPU_COUNT=`$EGREP -e '^processor' -c /proc/cpuinfo 2>/dev/null` || CPU_COUNT="0"]],dnl
          [[*darwin*]],[[CPU_COUNT=`system_profiler SPHardwareDataType 2>/dev/null | $EGREP -i -e 'number of cores:'|cut -d : -f 2 -s|tr -d ' '` || CPU_COUNT="0"]],dnl
          [[freebsd*]],[[CPU_COUNT=`dmesg 2>/dev/null| $EGREP -e '^cpu@<:@0-9@:>@+: '|sort -u|$EGREP -e '^' -c` || CPU_COUNT="0"]],dnl
          [[netbsd*]], [[CPU_COUNT=`command -v cpuctl >/dev/null 2>&1 && cpuctl list 2>/dev/null| $EGREP -e '^@<:@0-9@:>@+ .* online ' -c` || \
                           CPU_COUNT=`dmesg 2>/dev/null| $EGREP -e '^cpu@<:@0-9@:>@+ at'|sort -u|$EGREP -e '^' -c` || CPU_COUNT="0"]],dnl
          [[solaris*]],[[command -v kstat >/dev/null 2>&1 && CPU_COUNT=`kstat -m cpu_info -s state -p 2>/dev/null | $EGREP -c -e 'on-line'` || \
                           CPU_COUNT=`kstat -m cpu_info 2>/dev/null | $EGREP -c -e 'module: cpu_info'` || CPU_COUNT="0"]],dnl
          [[mingw*]],[AS_IF([[CPU_COUNT=`reg query 'HKLM\\Hardware\\Description\\System\\CentralProcessor' 2>/dev/null | $EGREP -e '\\\\@<:@0-9@:>@+$' -c`]],dnl
                        [[: # empty]],[[test "$NUMBER_OF_PROCESSORS" -gt "0" 2>/dev/null && CPU_COUNT="$NUMBER_OF_PROCESSORS"]])],dnl
          [[msys*]],[[test "$NUMBER_OF_PROCESSORS" -gt "0" 2>/dev/null && CPU_COUNT="$NUMBER_OF_PROCESSORS"]],dnl
          [[cygwin*]],[[test "$NUMBER_OF_PROCESSORS" -gt "0" 2>/dev/null && CPU_COUNT="$NUMBER_OF_PROCESSORS"]]dnl
        )dnl
      ])dnl

      AS_IF([[test "x$CPU_COUNT" != "x0" && test "$CPU_COUNT" -gt 0 2>/dev/null]],[dnl
          AC_MSG_RESULT([[$CPU_COUNT]])
          m4_ifvaln([$1],[$1],)dnl
        ],[dnl
          m4_ifval([$2],[dnl
            AS_UNSET([[CPU_COUNT]])
            AC_MSG_RESULT([[unable to detect]])
            $2
          ], [dnl
            CPU_COUNT="1"
            AC_MSG_RESULT([[unable to detect (assuming 1)]])
          ])dnl
        ])dnl
      ])dnl
