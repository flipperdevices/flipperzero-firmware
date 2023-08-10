#!/bin/bash
#set -v

# Script to produce a small source/header only package (with CMake support)
# Run this script from the wolfSSL root as `./scripts/makedistsmall.sh`.
# Package requires building with:
#   cmake .. -DWOLFSSL_EXAMPLES=no -DWOLFSSL_FILESYSTEM=no

if [ "$1" == "keep" ]; then KEEP="yes"; else KEEP="no"; fi

WOLFSSL_TEMPDIR=$(mktemp -d) || exit $?
WOLFSSL_BRANCH=$(git symbolic-ref --short HEAD)

function cleanup_on_exit() {
    if [ "$KEEP" == "no" ];
    then
        echo "Removing tmp directory"
        rm -rf "$WOLFSSL_TEMPDIR"
    else
        echo "tmp directory ${WOLFSSL_TEMPDIR} left in place."
    fi
}

trap cleanup_on_exit EXIT

WOLFSSL_VERSION=$(grep -Eo '[0-9]\.[0-9]\.[0-9]+' wolfssl/version.h)
if [ -n "$WOLFSSL_VERSION" ]; then
    echo "Detected wolfSSL Version $WOLFSSL_VERSION"
else
    echo "Couldn't detect wolfSSL version."
    exit 1
fi

# generate a wolfSSL archive with minimum sources
echo "Setting up work directory..."
git clone -q -n --shared . "$WOLFSSL_TEMPDIR" || exit $?
pushd "$WOLFSSL_TEMPDIR" >/dev/null || exit $?
git checkout -q "$WOLFSSL_BRANCH"

# cleanup example directories
echo "Removing files not needed..."
rm -rf -- ./.git*
rm -rf ./build-aux
rm -rf ./certs
rm -rf ./doc
rm -rf ./Docker
# these use test.h, which are not portable
rm -rf ./examples
rm -rf ./IDE
rm -rf ./IPP
rm -rf ./lib
rm -rf ./linuxkm
rm -rf ./m4
rm -rf ./mcapi
rm -rf ./mplabx
rm -rf ./mqx
rm -rf ./rpm
rm -rf ./RTOS
rm -rf ./scripts
rm -rf ./sslSniffer
rm -rf ./swig
rm -rf ./tests
rm -rf ./testsuite
rm -rf ./tirtos
rm -rf ./wolfcrypt/user-crypto
rm -rf ./wrapper
rm -f -- *.rc *.supp *.ac *.am *.conf *.sh *.cproject *.project *.pl
rm -f Vagrantfile SCRIPTS-LIST quit input resource.h

find . -name "*.am" -delete
find . -name "*.vcxproj" -delete
find . -name "*.vcproj" -delete
find . -name "*.sln" -delete

# TLS/Compatibility layer
#rm -rf ./src
#rm -rf ./wolfssl/openssl

# wolfCrypt
#rm -f ./wolfcrypt/src/*.i
#rm -f ./wolfcrypt/src/*.S
#rm -f ./wolfcrypt/src/*.asm
#rm -f ./wolfcrypt/src/arc4.c
#rm -f ./wolfcrypt/src/async.c
#rm -f ./wolfcrypt/src/blake*
#rm -f ./wolfcrypt/src/camellia.c
#rm -f ./wolfcrypt/src/chacha*
#rm -f ./wolfcrypt/src/compress.c
#rm -f ./wolfcrypt/src/cpuid.c
#rm -f ./wolfcrypt/src/curve*
#rm -f ./wolfcrypt/src/cryptocb.c
#rm -f ./wolfcrypt/src/dsa.c
#rm -f ./wolfcrypt/src/ed*
#rm -f ./wolfcrypt/src/evp.c
#rm -f ./wolfcrypt/src/fe*
#rm -f ./wolfcrypt/src/ge*
#rm -f ./wolfcrypt/src/md*.c
#rm -f ./wolfcrypt/src/pkcs*
#rm -f ./wolfcrypt/src/poly*
#rm -f ./wolfcrypt/src/pwdbased.c
#rm -f ./wolfcrypt/src/rc2.c
#rm -f ./wolfcrypt/src/ripemd.c
#rm -f ./wolfcrypt/src/rabbit.c
#rm -f ./wolfcrypt/src/signature.c
#rm -f ./wolfcrypt/src/srp.c
#rm -f ./wolfcrypt/src/wc_dsp.c
#rm -f ./wolfcrypt/src/wolfevent.c
#rm -f ./wolfcrypt/src/wc_encrypt.c
#rm -f ./wolfcrypt/src/wc_pkcs11.c

# SP Math files
#rm -f ./wolfcrypt/src/sp_arm32.c
#rm -f ./wolfcrypt/src/sp_arm64.c
#rm -f ./wolfcrypt/src/sp_armthumb.c
#rm -f ./wolfcrypt/src/sp_cortexm.c
#rm -f ./wolfcrypt/src/sp_dsp32.c
#rm -f ./wolfcrypt/src/sp_x86_64.c

# wolfCrypt Ports
rm -rf ./wolfcrypt/src/port
rm -rf ./wolfssl/wolfcrypt/port

# Setup blank options.h
cp ./wolfssl/options.h.in ./wolfssl/options.h || exit $?
cp ./wolfcrypt/test/test_paths.h.in ./wolfcrypt/test/test_paths.h || exit $?

popd >/dev/null || exit $?

echo "Generating wolfssl-${WOLFSSL_VERSION}-small.tar.gz..."

tar zcf "wolfssl-${WOLFSSL_VERSION}-small.tar.gz" -C "$WOLFSSL_TEMPDIR" . || exit $?

echo "Done"
