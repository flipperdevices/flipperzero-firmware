#!/bin/sh

# unofficial strict mode
set -eu;

check_system()
{
    printf "Checking kernel type..";
    SYS_TYPE="$(uname -s)"
    if [ "$SYS_TYPE" = "Darwin" ]; then
        echo "darwin";
        TOOLCHAIN_URL="https://update.flipperzero.one/builds/toolchain/gcc-arm-none-eabi-10.3-2022.06-x86_64-linux-flipper.tar.gz"
    elif [ "$SYS_TYPE" = "Linux" ]; then
        echo "linux";
        TOOLCHAIN_URL="https://update.flipperzero.one/builds/toolchain/gcc-arm-none-eabi-10.3-2022.06-x86_64-linux-flipper.tar.gz"
    else
        echo "unsupported.";
        echo "Your system is unsupported.. sorry..";
        exit 1;
    fi
}

check_tar()
{
    printf "Checking tar..";
    if ! tar --version > /dev/null 2>&1; then
        echo "no";
        exit 1;
    fi
    echo "yes";
}


curl_wget_check()
{
    printf "Checking curl..";
    if ! curl --version > /dev/null 2>&1; then
        echo "no";
        printf "Checking wget..";
        if ! wget --version > /dev/null 2>&1; then
            echo "no";
            echo "No curl or wget found in your PATH.";
            echo "Please provide it or download this file:";
            echo;
            echo "$TOOLCHAIN_URL";
            echo;
            echo "And place in repo root dir mannualy.";
            exit 1;
        fi
        echo "yes"
        DOWNLOADER="wget";
        DOWNLOADER_ARGS="-qO";
        return;
    fi
    echo "yes"
    DOWNLOADER="curl";
    DOWNLOADER_ARGS="-sSLo";
}

check_downloaded_toolchain()
{
    printf "Checking downloaded toolchain tgz..";
    if [ -f "$REPO_ROOT/$TOOLCHAIN_TAR" ]; then
        echo "yes";
        return 0;
    fi
    echo "no";
    return 1;
}

download_toolchain()
{
    printf "Checkin..oops Downloading toolchain..";
    "$DOWNLOADER" "$DOWNLOADER_ARGS" "$REPO_ROOT/$TOOLCHAIN_TAR" "$TOOLCHAIN_URL";
    echo "done";
}

remove_old_tooclhain()
{
    printf "Removing old toolchain (if exist)..";
    rm -rf "$REPO_ROOT/toolchain";
    echo "done";
}

unpack_toolchain()
{
    printf "Unpacking toolchain..";
    TOOLCHAIN_DIR="$(dirname -- "$(tar -tf "$REPO_ROOT/$TOOLCHAIN_TAR" | head -n 1)")";
    tar -xf "$REPO_ROOT/$TOOLCHAIN_TAR" -C "$REPO_ROOT/";
    mv "$REPO_ROOT/$TOOLCHAIN_DIR" "$REPO_ROOT/toolchain";
    echo "done";
}

clearing()
{
    printf "Clearing..";
    rm -rf "$REPO_ROOT/$TOOLCHAIN_TAR";
    echo "done";
}

main()
{
    SCRIPT_PATH="$(dirname -- "$(readlink -f -- "$0")")";
    REPO_ROOT="$(cd "$SCRIPT_PATH/../../" && pwd)";
    check_system;  # defines $TOOLCHAIN_URL
    check_tar;
    TOOLCHAIN_TAR="$(basename "$TOOLCHAIN_URL")";
    if ! check_downloaded_toolchain; then
        curl_wget_check;
        download_toolchain;
    fi
    remove_old_tooclhain;
    unpack_toolchain;
}

trap clearing EXIT;
trap clearing 2;  # SIGINT not coverable by EXIT
main;
