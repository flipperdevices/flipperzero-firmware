#!/bin/sh

# unofficial strict mode
set -eu;

SCRIPT_PATH="$(dirname -- "$(readlink -f -- "$0")")";
SCONS_DEFAULT_FLAGS="-Q --warn=target-not-built";
if [ ! -d "$SCRIPT_PATH/toolchain" ]; then
    scripts/toolchain/unix-toolchain-download.sh || exit 1;
fi
PATH="$SCRIPT_PATH/toolchain/python/bin:$PATH";
PATH="$SCRIPT_PATH/toolchain/bin:$PATH";
PATH="$SCRIPT_PATH/toolchain/protobuf/bin:$PATH";
python3 lib/scons/scripts/scons.py $SCONS_DEFAULT_FLAGS $*
