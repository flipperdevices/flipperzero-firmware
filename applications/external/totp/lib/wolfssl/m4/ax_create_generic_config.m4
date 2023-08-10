# ============================================================================
#  http://www.gnu.org/software/autoconf-archive/ax_create_generic_config.html
# ============================================================================
#
# SYNOPSIS
#
#   AX_CREATE_GENERIC_CONFIG [(PACKAGEnlibs [, VERSION])]
#
# DESCRIPTION
#
#   Creates a generic PACKAGE-config file that has all the things that you
#   want, hmm, well, at least it has --cflags, --version, --libs. Ahhm, did
#   you see ax_path_generic in the autoconf-archive? ;-)
#
#   this macros saves you all the typing for a pkg-config.in script, you
#   don't even need to distribute one along. Place this macro in your
#   configure.ac, et voila, you got one that you want to install.
#
#   oh, btw, if the first arg looks like "mylib -lwhat' then it will go to
#   be added to the --libs, and mylib is extracted.
#
#   the defaults: $1 = $PACKAGE $LIBS $2 = $VERSION there is also an
#   AC_SUBST(GENERIC_CONFIG) that will be set to the name of the file that
#   we did output in this macro. Use as:
#
#    install-exec-local:    install-generic-config
#
#    install-generic-config:
#       $(mkinstalldirs) $(DESTDIR)$(bindir)
#       $(INSTALL_SCRIPT) @GENERIC_CONFIG@ $(DESTDIR)$(bindir)
#
# LICENSE
#
#   Copyright (c) 2008 Guido U. Draheim <guidod@gmx.de>
#
#   This program is free software; you can redistribute it and/or modify it
#   under the terms of the GNU General Public License as published by the
#   Free Software Foundation; either version 3 of the License, or (at your
#   option) any later version.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
#   Public License for more details.
#
#   You should have received a copy of the GNU General Public License along
#   with this program. If not, see <http://www.gnu.org/licenses/>.
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

#serial 6

AU_ALIAS([AC_CREATE_GENERIC_CONFIG], [AX_CREATE_GENERIC_CONFIG])
AC_DEFUN([AX_CREATE_GENERIC_CONFIG],[# create a generic PACKAGE-config file
L=`echo ifelse($1, , $PACKAGE $LIBS, $1)`
P=`echo $L | sed -e 's/ -.*//'`
P=`echo $P`
V=`echo ifelse($2, , $VERSION, $2)`
F=`echo $P-config`
L=`echo -l$L | sed -e 's/^-llib/-l/'`
AC_MSG_RESULT(creating $F - generic $V for $L)
test "x$prefix" = xNONE && prefix="$ac_default_prefix"
test "x$exec_prefix" = xNONE && exec_prefix='${prefix}'
echo '#! /bin/sh' >$F
echo ' ' >>$F
echo 'package="'$P'"' >>$F
echo 'version="'$V'"' >>$F
echo 'libs="'$L'"' >>$F
echo ' ' >>$F
# in the order of occurrence a standard automake Makefile
echo 'prefix="'$prefix'"' >>$F
echo 'exec_prefix="'$exec_prefix'"' >>$F
echo 'bindir="'$bindir'"' >>$F
echo 'sbindir="'$sbindir'"' >>$F
echo 'libexecdir="'$libexecdir'"' >>$F
echo 'datadir="'$datadir'"' >>$F
echo 'sysconfdir="'$sysconfdir'"' >>$F
echo 'sharedstatedir="'$sharedstatedir'"' >>$F
echo 'localstatedir="'$localstatedir'"' >>$F
echo 'libdir="'$libdir'"' >>$F
echo 'infodir="'$infodir'"' >>$F
echo 'mandir="'$mandir'"' >>$F
echo 'includedir="'$includedir'"' >>$F
echo 'target="'$target'"' >>$F
echo 'host="'$host'"' >>$F
echo 'build="'$build'"' >>$F
echo ' ' >>$F
echo 'if test "'"\$""#"'" -eq 0; then' >>$F
echo '   cat <<EOF' >>$F
echo 'Usage: $package-config [OPTIONS]' >>$F
echo 'Options:' >>$F
echo '  --prefix[=DIR]) : \$prefix' >>$F
echo '  --package) : \$package' >>$F
echo '  --version) : \$version' >>$F
echo '  --cflags) : -I\$includedir' >>$F
echo '  --libs) : -L\$libdir -l\$package' >>$F
echo '  --help) print all the options (not just these)' >>$F
echo 'EOF' >>$F
echo 'fi' >>$F
echo ' ' >>$F
echo 'o=""' >>$F
echo 'h=""' >>$F
echo 'for i in "[$]@"; do' >>$F
echo '  case $i in' >>$F
echo '  --prefix=*) prefix=`echo $i | sed -e "s/--prefix=//"` ;;' >>$F
echo '  --prefix)    o="$o $prefix" ;;' >>$F
echo '  --package)   o="$o $package" ;;' >>$F
echo '  --version)   o="$o $version" ;;' >>$F
echo '  --cflags) if test "_$includedir" != "_/usr/include"' >>$F
echo '          then o="$o -I$includedir" ; fi' >>$F
echo '  ;;' >>$F
echo '  --libs)      o="$o -L$libdir $libs" ;;' >>$F
echo '  --exec_prefix|--eprefix) o="$o $exec_prefix" ;;' >>$F
echo '  --bindir)                o="$o $bindir" ;;' >>$F
echo '  --sbindir)               o="$o $sbindir" ;;' >>$F
echo '  --libexecdir)            o="$o $libexecdir" ;;' >>$F
echo '  --datadir)               o="$o $datadir" ;;' >>$F
echo '  --datainc)               o="$o -I$datadir" ;;' >>$F
echo '  --datalib)               o="$o -L$datadir" ;;' >>$F
echo '  --sysconfdir)            o="$o $sysconfdir" ;;' >>$F
echo '  --sharedstatedir)        o="$o $sharedstatedir" ;;' >>$F
echo '  --localstatedir)         o="$o $localstatedir" ;;' >>$F
echo '  --libdir)                o="$o $libdir" ;;' >>$F
echo '  --libadd)                o="$o -L$libdir" ;;' >>$F
echo '  --infodir)               o="$o $infodir" ;;' >>$F
echo '  --mandir)                o="$o $mandir" ;;' >>$F
echo '  --target)                o="$o $target" ;;' >>$F
echo '  --host)                  o="$o $host" ;;' >>$F
echo '  --build)                 o="$o $build" ;;' >>$F
echo '  --data)                  o="$o -I$datadir/$package" ;;' >>$F
echo '  --pkgdatadir)            o="$o $datadir/$package" ;;' >>$F
echo '  --pkgdatainc)            o="$o -I$datadir/$package" ;;' >>$F
echo '  --pkgdatalib)            o="$o -L$datadir/$package" ;;' >>$F
echo '  --pkglibdir)             o="$o $libdir/$package" ;;' >>$F
echo '  --pkglibinc)             o="$o -I$libinc/$package" ;;' >>$F
echo '  --pkglibadd)             o="$o -L$libadd/$package" ;;' >>$F
echo '  --pkgincludedir)         o="$o $includedir/$package" ;;' >>$F
echo '  --help) h="1" ;;' >>$F
echo '  -?//*|-?/*//*|-?./*//*|//*|/*//*|./*//*) ' >>$F
echo '       v=`echo $i | sed -e s://:\$:g`' >>$F
echo '       v=`eval "echo $v"` ' >>$F
echo '       o="$o $v" ;; ' >>$F
echo '  esac' >>$F
echo 'done' >>$F
echo ' ' >>$F
echo 'o=`eval "echo $o"`' >>$F
echo 'o=`eval "echo $o"`' >>$F
echo 'eval "echo $o"' >>$F
echo ' ' >>$F
echo 'if test ! -z "$h" ; then ' >>$F
echo 'cat <<EOF' >>$F
echo '  --prefix=xxx)      (what is that for anyway?)' >>$F
echo '  --prefix)         \$prefix        $prefix' >>$F
echo '  --package)        \$package       $package' >>$F
echo '  --version)        \$version       $version' >>$F
echo '  --cflags)         -I\$includedir    unless it is /usr/include' >>$F
echo '  --libs)           -L\$libdir -l\$PACKAGE \$LIBS' >>$F
echo '  --exec_prefix) or... ' >>$F
echo '  --eprefix)        \$exec_prefix   $exec_prefix' >>$F
echo '  --bindir)         \$bindir        $bindir' >>$F
echo '  --sbindir)        \$sbindir       $sbindir' >>$F
echo '  --libexecdir)     \$libexecdir    $libexecdir' >>$F
echo '  --datadir)        \$datadir       $datadir' >>$F
echo '  --sysconfdir)     \$sysconfdir    $sysconfdir' >>$F
echo '  --sharedstatedir) \$sharedstatedir$sharedstatedir' >>$F
echo '  --localstatedir)  \$localstatedir $localstatedir' >>$F
echo '  --libdir)         \$libdir        $libdir' >>$F
echo '  --infodir)        \$infodir       $infodir' >>$F
echo '  --mandir)         \$mandir        $mandir' >>$F
echo '  --target)         \$target        $target' >>$F
echo '  --host)           \$host          $host' >>$F
echo '  --build)          \$build         $build' >>$F
echo '  --data)           -I\$datadir/\$package' >>$F
echo '  --pkgdatadir)     \$datadir/\$package' >>$F
echo '  --pkglibdir)      \$libdir/\$package' >>$F
echo '  --pkgincludedir)  \$includedir/\$package' >>$F
echo '  --help)           generated by ax_create_generic_config.m4' >>$F
echo '  -I//varname and other inc-targets like --pkgdatainc supported' >>$F
echo '  -L//varname and other lib-targets, e.g. --pkgdatalib or --libadd' >>$F
echo 'EOF' >>$F
echo 'fi' >>$F
GENERIC_CONFIG="$F"
AC_SUBST(GENERIC_CONFIG)
])
