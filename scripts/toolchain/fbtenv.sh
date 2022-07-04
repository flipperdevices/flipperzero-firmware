#!/bin/sh
# shellcheck disable=SC2086

# unofficial strict mode
set -eu;

FLIPPER_TOOLCHAIN_VERSION="2";
export FBT_ROOT="$(cd "$(dirname "$0")" && pwd -P)";

get_kernel_type()
{
    SYS_TYPE="$(uname -s)"
    if [ "$SYS_TYPE" = "Darwin" ]; then
        TOOLCHAIN_PATH="toolchain/x86_64-darwin";
    elif [ "$SYS_TYPE" = "Linux" ]; then
        TOOLCHAIN_PATH="toolchain/x86_64-linux";
    elif echo "$SYS_TYPE" | grep -q "MINGW"; then
        echo "In MinGW shell use \"fbt.cmd\" instead of \"fbt\"";
        exit 1;
    else
        echo "Your system is not supported. Sorry. Please report us your configuration.";
        exit 1;
    fi
}

check_download_toolchain()
{
    if [ ! -d "$FBT_ROOT/$TOOLCHAIN_PATH" ]; then
        download_toolchain;
    elif [ ! -f "$FBT_ROOT/$TOOLCHAIN_PATH/VERSION" ]; then
        download_toolchain;
    elif [ "$(cat "$FBT_ROOT/$TOOLCHAIN_PATH/VERSION")" -ne "$FLIPPER_TOOLCHAIN_VERSION" ]; then
        download_toolchain;
    fi
}

download_toolchain()
{
    chmod 755 "$FBT_ROOT/scripts/toolchain/unix-toolchain-download.sh";
    "$FBT_ROOT/scripts/toolchain/unix-toolchain-download.sh" "$FLIPPER_TOOLCHAIN_VERSION" || exit 1;
}

check_git()
{
    if [ ! -d "$FBT_ROOT/.git" ]; then
        echo "\".git\" directory not found, please clone repo via \"git clone --recursive\"";
        exit 1;
    fi
}

setup_env()
{
    get_kernel_type;  # sets TOOLCHAIN_PATH
    check_git;
    check_download_toolchain;
    PATH="$FBT_ROOT/$TOOLCHAIN_PATH/python/bin:$PATH";
    PATH="$FBT_ROOT/$TOOLCHAIN_PATH/bin:$PATH";
    PATH="$FBT_ROOT/$TOOLCHAIN_PATH/protobuf/bin:$PATH";
    PATH="$FBT_ROOT/$TOOLCHAIN_PATH/openocd/bin:$PATH";    
}

if [ -z "${FBT_NOENV:-}" ]; then
    setup_env;
fi
