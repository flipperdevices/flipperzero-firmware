AC_DEFUN([CREATE_HEX_VERSION],[

  HEX_VERSION=`echo $VERSION | sed 's|[\-a-z0-9]*$||' | \
    awk -F. '{printf "0x%0.2d%0.3d%0.3d", $[]1, $[]2, $[]3}'`
  AC_SUBST([HEX_VERSION])
])
