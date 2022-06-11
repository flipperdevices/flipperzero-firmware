#!/bin/sh
SYS_TYPE="$(uname -s)"
if [ "$SYS_TYPE" = "Darwin" ]; then
    printf "Downloading Mac OS toolchain..";
else
    printf "Downloading Linux toolchain..";
fi

echo "done!"
