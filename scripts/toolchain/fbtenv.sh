#!/bin/sh

# shellcheck disable=SC2034,SC2016,SC2086

fbtenv_get_kernel_type()
{
    SYS_TYPE="$(uname -s)";
    ARCH_TYPE="$(uname -m)";
    if [ "$ARCH_TYPE" != "x86_64" ] && [ "$SYS_TYPE" != "Darwin" ]; then
        echo "Now we provide toolchain only for x86_64 arhitecture. Sorry..";
        return 1;
    fi
    if [ "$SYS_TYPE" = "Darwin" ]; then
        fbtenv_check_rosetta || return 1;
        TOOLCHAIN_ARCH_DIR="$FBT_TOOLCHAIN_PATH/toolchain/x86_64-darwin";
        TOOLCHAIN_URL="https://update.flipperzero.one/builds/toolchain/gcc-arm-none-eabi-10.3-x86_64-darwin-flipper-$FBT_TOOLCHAIN_VERSION.tar.gz";
    elif [ "$SYS_TYPE" = "Linux" ]; then
        TOOLCHAIN_ARCH_DIR="$FBT_TOOLCHAIN_PATH/toolchain/x86_64-linux";
        TOOLCHAIN_URL="https://update.flipperzero.one/builds/toolchain/gcc-arm-none-eabi-10.3-x86_64-linux-flipper-$FBT_TOOLCHAIN_VERSION.tar.gz";
    elif echo "$SYS_TYPE" | grep -q "MINGW"; then
        echo "In MinGW shell use \"fbt.cmd\" instead of \"fbt\"";
        return 1;
    else
        echo "Your system is not recognized. Sorry.. Please report us your configuration.";
        return 1;
    fi
    return 0;
}

fbtenv_check_rosetta()
{
    if [ "$ARCH_TYPE" = "arm64" ]; then
        if ! /usr/bin/pgrep -q oahd; then
            echo "Flipper Zero Toolchain needs Rosetta2 to run under Apple Silicon";
            echo "Please instal it by typing 'softwareupdate --install-rosetta --agree-to-license'";
            return 1;
        fi
    fi
    return 0;
}

fbtenv_check_tar()
{
    printf "Checking tar..";
    if ! tar --version > /dev/null 2>&1; then
        echo "no";
        return 1;
    fi
    echo "yes";
    return 0;
}

fbtenv_check_downloaded_toolchain()
{
    printf "Checking downloaded toolchain tgz..";
    if [ ! -f "$FBT_TOOLCHAIN_PATH/toolchain/$TOOLCHAIN_TAR" ]; then
        echo "no";
        return 1;
    fi
    echo "yes";
    return 0;
}

fbtenv_download_toolchain_tar()
{
    echo "Downloading toolchain:";
    mkdir -p "$FBT_TOOLCHAIN_PATH/toolchain" || return 1;
    "$DOWNLOADER" $DOWNLOADER_ARGS "$FBT_TOOLCHAIN_PATH/toolchain/$TOOLCHAIN_TAR" "$TOOLCHAIN_URL" || return 1;
    echo "done";
    return 0;
}

fbtenv_remove_old_tooclhain()
{
    printf "Removing old toolchain (if exist)..";
    rm -rf "${TOOLCHAIN_ARCH_DIR}";
    echo "done";
}

fbtenv_show_unpack_percentage()
{
    LINE=0;
    while read -r line; do
        LINE=$(( LINE + 1 ));
        if [ $(( LINE % 300 )) -eq 0 ]; then
            printf "#";
        fi
    done
    echo " 100.0%";
}

fbtenv_unpack_toolchain()
{
    echo "Unpacking toolchain:";
    tar -xvf "$FBT_TOOLCHAIN_PATH/toolchain/$TOOLCHAIN_TAR" -C "$FBT_TOOLCHAIN_PATH/toolchain" 2>&1 | fbtenv_show_unpack_percentage;
    mkdir -p "$FBT_TOOLCHAIN_PATH/toolchain" || return 1;
    mv "$FBT_TOOLCHAIN_PATH/toolchain/$TOOLCHAIN_DIR" "$TOOLCHAIN_ARCH_DIR" || return 1;
    echo "done";
    return 0;
}

fbtenv_clearing()
{
    printf "Clearing..";
    rm -rf "${FBT_TOOLCHAIN_PATH:?}/toolchain/$TOOLCHAIN_TAR";
    echo "done";
    return 0;
}

fbtenv_curl_wget_check()
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
            echo "And place in $FBT_TOOLCHAIN_PATH/toolchain/ dir mannualy.";
            return 1;
        fi
        echo "yes"
        DOWNLOADER="wget";
        DOWNLOADER_ARGS="--show-progress --progress=bar:force -qO";
        return 0;
    fi
    echo "yes"
    DOWNLOADER="curl";
    DOWNLOADER_ARGS="--progress-bar -SLo";
    return 0;
}

fbtenv_check_download_toolchain()
{
    if [ ! -d "$TOOLCHAIN_ARCH_DIR" ]; then
        fbtenv_download_toolchain || return 1;
    elif [ ! -f "$TOOLCHAIN_ARCH_DIR/VERSION" ]; then
        fbtenv_download_toolchain || return 1;
    elif [ "$(cat "$TOOLCHAIN_ARCH_DIR/VERSION")" -ne "$FBT_TOOLCHAIN_VERSION" ]; then
        fbtenv_download_toolchain || return 1;
    fi
    return 0;
}

fbtenv_show_usage()
{
    echo "fbtenv.sh - sets eviroment variables for Flipper Build Tool (FBT) and downloads toolchain";
    echo "Warning! Not recomended to run this script mannualy! Use ./fbt !";
    echo "Usage:";
    printf "\tFBT_TOOLCHAIN_VERSION=.. FBT_TOOLCHAIN_PATH=.. SCRIPT_PATH=.. source fbtenv.sh\n";
    echo "Example:";
    printf '\tFBT_TOOLCHAIN_VERSION=10 SCRIPT_PATH=$(pwd) FBT_TOOLCHAIN_PATH=/tmp/ source scripts/toolchain/fbtenv.sh\n'
    echo "Arguments:";
    printf "\tFBT_TOOLCHAIN_VERSION - version of Flipper Zero Toolchain\n";
    printf "\tFBT_TOOLCHAIN_PATH - path when Flipper Zero Toolchain will be stored\n";
    printf "\tSCRIPT_PATH - path to fliepperzero-firmware directory\n";
}

fbtenv_download_toolchain()
{
    fbtenv_check_tar || return 1;
    TOOLCHAIN_TAR="$(basename "$TOOLCHAIN_URL")";
    TOOLCHAIN_DIR="$(echo "$TOOLCHAIN_TAR" | sed "s/-$FBT_TOOLCHAIN_VERSION.tar.gz//g")";
    if ! fbtenv_check_downloaded_toolchain; then
        fbtenv_curl_wget_check || return 1;
        fbtenv_download_toolchain_tar;
    fi
    fbtenv_remove_old_tooclhain;
    fbtenv_unpack_toolchain || fbtenv_clearing && return 1;
    fbtenv_clearing;
    return 0;
}

fbtenv_main()
{

    if [ -z "$SCRIPT_PATH" ] || \
            [ -z "$FBT_TOOLCHAIN_VERSION" ] || \
            [ -z "$FBT_TOOLCHAIN_PATH" ]; then
        fbtenv_show_usage;
        return 1;
    fi
    fbtenv_get_kernel_type || return 1;
    fbtenv_check_download_toolchain || return 1;
    PATH="$TOOLCHAIN_ARCH_DIR/python/bin:$PATH";
    PATH="$TOOLCHAIN_ARCH_DIR/bin:$PATH";
    PATH="$TOOLCHAIN_ARCH_DIR/protobuf/bin:$PATH";
    PATH="$TOOLCHAIN_ARCH_DIR/openocd/bin:$PATH";
}

fbtenv_main;
