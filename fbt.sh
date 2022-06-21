#!/bin/sh

# unofficial strict mode
set -eu;

get_kernel_type()
{
    SYS_TYPE="$(uname -s)"
    if [ "$SYS_TYPE" = "Darwin" ]; then
        TOOLCHAIN_PATH="toolchain/x86_64-darwin";
    elif [ "$SYS_TYPE" = "Linux" ]; then
        TOOLCHAIN_PATH="toolchain/x86_64-linux";
    else
        echo "Your system is unsupported.. sorry..";
        exit 1;
    fi
}

download_toolchain()
{
    if [ ! -d "$SCRIPT_PATH/$TOOLCHAIN_PATH" ]; then
        scripts/toolchain/unix-toolchain-download.sh || exit 1;
    fi
}

SCRIPT_PATH="$(dirname -- "$(readlink -f -- "$0")")";
SCONS_DEFAULT_FLAGS="-Q --warn=target-not-built";
get_kernel_type;
download_toolchain;
PATH="$SCRIPT_PATH/$TOOLCHAIN_PATH/python/bin:$PATH";
PATH="$SCRIPT_PATH/$TOOLCHAIN_PATH/bin:$PATH";
PATH="$SCRIPT_PATH/$TOOLCHAIN_PATH/protobuf/bin:$PATH";

python3 lib/scons/scripts/scons.py $SCONS_DEFAULT_FLAGS $*
