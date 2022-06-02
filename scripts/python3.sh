#!/bin/sh

# function return values
FALSE=1
TRUE=0

is_flipper_toolchain_installed()
{
    TOOLCHAIN_GDB="$(which arm-none-eabi-gdb-py)"
    if test -z "$TOOLCHAIN_GDB"; then
        return "$FALSE";
    fi
    TOOLCHAIN_DIR="$(/usr/bin/dirname -- "$(readlink -f -- "$TOOLCHAIN_GDB")")";
    FLIPPER_PY_DIR="$(cd "$TOOLCHAIN_DIR/../python/bin/" && pwd)";
    if ! test -d "$FLIPPER_PY_DIR"; then
        return "$FALSE";
    fi
    if ! test -f "$FLIPPER_PY_DIR/python3"; then
        return "$FALSE";
    fi
    return "$TRUE";
}

py3_host()
{
    /usr/bin/env python3 "$@"
}

py3_flipper()
{
    "$FLIPPER_PY_DIR/python3" "$@"
}

if is_flipper_toolchain_installed; then
    py3_flipper "$@";
else
    py3_host "$@";
fi
