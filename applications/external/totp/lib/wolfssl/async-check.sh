#!/bin/bash

# async-check.sh

# This script creates symbolic links to the required asynchronous
# file for using the asynchronous simulator and make check
#
#     $ ./async-check [keep|clean|setup]
#
#     - keep: (default off) ./async and links kept around for inspection
#     - clean: (default off) only cleanup existing ./async and links
#     - setup: (default off) only setup ./async and links but don't run config
#                            or make
#

ASYNC_REPO=https://github.com/wolfSSL/wolfAsyncCrypt.git

function Usage() {
    printf '\n%s\n\n' "Usage: $0 [keep|clean|setup]"
    printf '%s\n' "Where \"keep\" means keep (default off) async files around for inspection"
    printf '%s\n' "Where \"clean\" means only clean (default off) the async files"
    printf '%s\n\n' "Where \"setup\" means only setup (default off) the async files"
    printf '%s\n' "EXAMPLE:"
    printf '%s\n' "---------------------------------"
    printf '%s\n' "./async-check.sh keep"
    printf '%s\n' "./async-check.sh clean"
    printf '%s\n' "./async-check.sh setup"
    printf '%s\n\n' "---------------------------------"
}

function CleanUp() {
    unlink ./wolfcrypt/src/async.c
    unlink ./wolfssl/wolfcrypt/async.h
    unlink ./wolfcrypt/src/port/intel/quickassist.c
    unlink ./wolfcrypt/src/port/intel/quickassist_mem.c
    unlink ./wolfcrypt/src/port/intel/README.md
    unlink ./wolfssl/wolfcrypt/port/intel/quickassist.h
    unlink ./wolfssl/wolfcrypt/port/intel/quickassist_mem.h
    unlink ./wolfcrypt/src/port/cavium/cavium_nitrox.c
    unlink ./wolfssl/wolfcrypt/port/cavium/cavium_nitrox.h
    unlink ./wolfcrypt/src/port/cavium/README.md

    rm -rf ./async

    # restore original README.md files
    git checkout -- wolfcrypt/src/port/cavium/README.md
    git checkout -- wolfcrypt/src/port/intel/README.md
}

function LinkFiles() {
    # link files
    ln -s -f ../../async/wolfcrypt/src/async.c ./wolfcrypt/src/async.c
    ln -s -f ../../async/wolfssl/wolfcrypt/async.h ./wolfssl/wolfcrypt/async.h
    ln -s -f ../../../../async/wolfcrypt/src/port/intel/quickassist.c ./wolfcrypt/src/port/intel/quickassist.c
    ln -s -f ../../../../async/wolfcrypt/src/port/intel/quickassist_mem.c ./wolfcrypt/src/port/intel/quickassist_mem.c
    ln -s -f ../../../../async/wolfcrypt/src/port/intel/README.md ./wolfcrypt/src/port/intel/README.md
    ln -s -f ../../../../async/wolfssl/wolfcrypt/port/intel/quickassist.h ./wolfssl/wolfcrypt/port/intel/quickassist.h
    ln -s -f ../../../../async/wolfssl/wolfcrypt/port/intel/quickassist_mem.h ./wolfssl/wolfcrypt/port/intel/quickassist_mem.h
    ln -s -f ../../../../async/wolfcrypt/src/port/cavium/cavium_nitrox.c ./wolfcrypt/src/port/cavium/cavium_nitrox.c
    ln -s -f ../../../../async/wolfssl/wolfcrypt/port/cavium/cavium_nitrox.h ./wolfssl/wolfcrypt/port/cavium/cavium_nitrox.h
    ln -s -f ../../../../async/wolfcrypt/src/port/cavium/README.md ./wolfcrypt/src/port/cavium/README.md
}

if [ "$#" -gt 1 ]; then
    Usage
    exit 1
fi

KEEP=no
ONLY_SETUP=no

case "x$1" in
    "xkeep")
        KEEP=yes
        ;;
    "xclean")
        CleanUp
        exit 0
        ;;
    "xsetup")
        ONLY_SETUP=yes
        ;;
    *)
        Usage
        exit 1
        ;;
esac

# Fail on any error in script from now on
set -e

if [ -d ./async ];
then
    echo "\n\nUsing existing async repo\n\n"
else
    # make a clone of the wolfAsyncCrypt repository
    git clone --depth 1 $ASYNC_REPO async
fi

# setup auto-conf
./autogen.sh
LinkFiles
if [ "x$ONLY_SETUP" == "xno" ];
then
    ./configure --enable-asynccrypt --enable-all
    make check
    # Clean up
    if [ "x$KEEP" == "xno" ];
    then
        CleanUp
    fi
fi

