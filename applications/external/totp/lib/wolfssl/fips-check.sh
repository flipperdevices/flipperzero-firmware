#!/bin/bash

# fips-check.sh
# This script checks the current revision of the code against the
# previous release of the FIPS code. While wolfSSL and wolfCrypt
# may be advancing, they must work correctly with the last tested
# copy of our FIPS approved code.
#
# This should check out all the approved flavors. The command line
# option selects the flavor. The keep option keeps the output
# directory.
#
# Some variables may be overridden on the command line.

Usage() {
    cat <<usageText
Usage: $0 [flavor] [keep]
Flavor is one of:
    netbsd-selftest
    marvell-linux-selftest
    linuxv2 (FIPSv2, use for Win10)
    wolfrand
    solaris
    linuxv5 (current FIPS 140-3)
    fips-ready (ready FIPS 140-3)
    fips-dev (dev FIPS 140-3)
Keep (default off) retains the XXX-fips-test temp dir for inspection.

Example:
    $0 windows keep
usageText
}

# These variables may be overridden on the command line.
MAKE="${MAKE:-make}"
GIT="${GIT:-git -c advice.detachedHead=false}"
TEST_DIR="${TEST_DIR:-XXX-fips-test}"
FLAVOR="${FLAVOR:-linux}"
KEEP="${KEEP:-no}"

while [ "$1" ]; do
  if [ "$1" = 'keep' ]; then KEEP='yes'; else FLAVOR="$1"; fi
  shift
done

FIPS_REPO='git@github.com:wolfssl/fips.git'
CRYPT_INC_PATH='wolfssl/wolfcrypt'
CRYPT_SRC_PATH='wolfcrypt/src'

case "$FLAVOR" in
linuxv2|fipsv2-OE-ready)
  FIPS_OPTION='v2'
  FIPS_VERSION='WCv4-stable'
  CRYPT_VERSION='WCv4-stable'
  RNG_VERSION='WCv4-rng-stable'
  WC_MODS=('aes' 'aes_asm' 'cmac' 'des3' 'dh' 'ecc' 'hmac' 'random' 'rsa' 'sha' 'sha256' 'sha3' 'sha512')
  FIPS_SRCS=('fips.c' 'fips_test.c' 'wolfcrypt_first.c' 'wolfcrypt_last.c')
  FIPS_INCS=('fips.h')
  ;;
netbsd-selftest)
  # non-FIPS, CAVP only but pull in selftest
  FIPS_OPTION='cavp-selftest'
  FIPS_VERSION='v3.14.2b'
  CRYPT_VERSION='v3.14.2'
  RNG_VERSION='v3.14.2'
  WC_MODS=('aes' 'dh' 'dsa' 'ecc' 'hmac' 'random' 'rsa' 'sha' 'sha256' 'sha512')
  FIPS_SRCS=('selftest.c')
  ;;
marvell-linux-selftest)
  # non-FIPS, CAVP only but pull in selftest
  FIPS_OPTION='cavp-selftest-v2'
  FIPS_VERSION='v3.14.2b'
  CRYPT_VERSION='v4.1.0-stable'
  RNG_VERSION='v4.1.0-stable'
  WC_MODS=('aes' 'dh' 'dsa' 'ecc' 'hmac' 'random' 'rsa' 'sha' 'sha256' 'sha512')
  FIPS_SRCS=('selftest.c')
  ;;
linuxv5)
  FIPS_OPTION='v5'
  FIPS_VERSION='WCv5.0-RC12'
  CRYPT_VERSION='WCv5.0-RC12'
  RNG_VERSION='WCv5.0-RC12'
  WC_MODS=('aes' 'aes_asm' 'cmac' 'dh' 'ecc' 'hmac' 'kdf' 'random' 'rsa' 'sha' 'sha256' 'sha256_asm' 'sha3' 'sha512' 'sha512_asm')
  FIPS_SRCS=('fips.c' 'fips_test.c' 'wolfcrypt_first.c' 'wolfcrypt_last.c')
  FIPS_INCS=('fips.h')
  COPY_DIRECT=('wolfcrypt/src/aes_gcm_asm.S')
  ;;
fips-ready)
  FIPS_OPTION='ready'
  FIPS_VERSION='master'
  FIPS_SRCS=('fips.c' 'fips_test.c' 'wolfcrypt_first.c' 'wolfcrypt_last.c')
  FIPS_INCS=('fips.h')
  ;;
fips-dev)
  FIPS_OPTION='dev'
  FIPS_VERSION='master'
  FIPS_SRCS=('fips.c' 'fips_test.c' 'wolfcrypt_first.c' 'wolfcrypt_last.c')
  FIPS_INCS=('fips.h')
  ;;
wolfrand)
  FIPS_OPTION='rand'
  FIPS_VERSION='WRv4-stable'
  CRYPT_VERSION='WCv4-stable'
  RNG_VERSION='WCv4-rng-stable'
  WC_MODS=('hmac' 'random' 'sha256')
  FIPS_SRCS=('fips.c' 'fips_test.c' 'wolfcrypt_first.c' 'wolfcrypt_last.c')
  FIPS_INCS=('fips.h')
  ;;
solaris)
  FIPS_OPTION='v2'
  FIPS_VERSION='WCv4-stable'
  CRYPT_VERSION='WCv4-stable'
  RNG_VERSION='WCv4-rng-stable'
  WC_MODS=('aes' 'aes_asm' 'cmac' 'des3' 'dh' 'ecc' 'hmac' 'random' 'rsa' 'sha' 'sha256' 'sha3' 'sha512')
  FIPS_SRCS=('fips.c' 'fips_test.c' 'wolfcrypt_first.c' 'wolfcrypt_last.c')
  FIPS_INCS=('fips.h')
  MAKE='gmake'
  ;;
*)
  Usage
  exit 1
esac

if ! $GIT clone . "$TEST_DIR"; then
    echo "fips-check: Couldn't duplicate current working directory."
    exit 1
fi

pushd "$TEST_DIR" || exit 2

case "$FIPS_OPTION" in

*dev)
    echo "Don't need to copy in tagged wolfCrypt files for fips-dev."
    ;;

*ready)
    echo "Don't need to copy in tagged wolfCrypt files for FIPS Ready."
    ;;

cavp-selftest*|v2|rand|v5*)
    $GIT branch --no-track "my$CRYPT_VERSION" "$CRYPT_VERSION" || exit $?
    # Checkout the fips versions of the wolfCrypt files from the repo.
    for MOD in "${WC_MODS[@]}"
    do
        if [ -f "$CRYPT_SRC_PATH/$MOD.c" ]; then
            $GIT checkout "my$CRYPT_VERSION" -- "$CRYPT_SRC_PATH/$MOD.c" || exit $?
        fi
        # aes_asm.S, sha256_asm.S sha512_asm.S
        if [ -f "$CRYPT_SRC_PATH/$MOD.S" ]; then
            echo "Checking out asm file: $MOD.S"
            $GIT checkout "my$CRYPT_VERSION" -- "$CRYPT_SRC_PATH/$MOD.S" || exit $?
        fi
        # aes_asm.asm
        if [ -f "$CRYPT_SRC_PATH/$MOD.asm" ]; then
            echo "Checking out asm file: $MOD.asm"
            $GIT checkout "my$CRYPT_VERSION" -- "$CRYPT_SRC_PATH/$MOD.asm" || exit $?
        fi
        if [ -f "$CRYPT_INC_PATH/$MOD.h" ]; then
            $GIT checkout "my$CRYPT_VERSION" -- "$CRYPT_INC_PATH/$MOD.h" || exit $?
        fi
    done

    for MOD in "${COPY_DIRECT[@]}"
    do
        $GIT checkout "my$CRYPT_VERSION" -- "$MOD" || exit $?
    done

    $GIT branch --no-track "myrng$RNG_VERSION" "$RNG_VERSION" || exit $?
    # Checkout the fips versions of the wolfCrypt files from the repo.
    $GIT checkout "myrng$RNG_VERSION" -- "$CRYPT_SRC_PATH/random.c" "$CRYPT_INC_PATH/random.h" || exit $?
    ;;

*)
    echo "fips-check: Invalid FIPS option \"${FIPS_OPTION}\"."
    exit 1
    ;;
esac

# clone the FIPS repository
case "$FIPS_OPTION" in
*dev)
    if ! $GIT clone --depth 1 "$FIPS_REPO" fips; then
        echo "fips-check: Couldn't check out the FIPS repository for fips-dev."
        exit 1
    fi
    ;;
*)
    if ! $GIT clone --depth 1 -b "$FIPS_VERSION" "$FIPS_REPO" fips; then
        echo "fips-check: Couldn't check out ${FIPS_VERSION} from repository ${FIPS_REPO}."
        exit 1
    fi
    ;;
esac

for SRC in "${FIPS_SRCS[@]}"
do
    cp "fips/$SRC" "$CRYPT_SRC_PATH"
done

for INC in "${FIPS_INCS[@]}"
do
    cp "fips/$INC" "$CRYPT_INC_PATH"
done

# When checking out cert 3389 ready code, NIST will no longer perform
# new certifications on 140-2 modules. If we were to use the latest files from
# master that would require re-cert due to changes in the module boundary.
# Since OE additions can still be processed for cert3389 we will call 140-2
# ready "fipsv2-OE-ready" indicating it is ready to use for an OE addition but
# would not be good for a new certification effort with the latest files.
if [ "$FLAVOR" = 'fipsv2-OE-ready' ]; then
    OLD_VERSION="    return \"v4.0.0-alpha\";"
    OE_READY_VERSION="    return \"fipsv2-OE-ready\";"
    cp "${CRYPT_SRC_PATH}/fips.c" "${CRYPT_SRC_PATH}/fips.c.bak"
    sed "s/^${OLD_VERSION}/${OE_READY_VERSION}/" "${CRYPT_SRC_PATH}/fips.c.bak" >"${CRYPT_SRC_PATH}/fips.c"
fi

# run the make test
./autogen.sh

case "$FIPS_OPTION" in
cavp-selftest)
    ./configure --enable-selftest
    ;;
cavp-selftest-v2)
    ./configure --enable-selftest=v2
    ;;
*)
    ./configure --enable-fips=$FIPS_OPTION
    ;;
esac

if ! $MAKE
then
    echo "fips-check: Make failed. Debris left for analysis."
    exit 3
fi

if [ -s "${CRYPT_SRC_PATH}/fips_test.c" ]
then
    NEWHASH=$(./wolfcrypt/test/testwolfcrypt | sed -n 's/hash = \(.*\)/\1/p')
    if [ -n "$NEWHASH" ]; then
        cp "${CRYPT_SRC_PATH}/fips_test.c" "${CRYPT_SRC_PATH}/fips_test.c.bak"
        sed "s/^\".*\";/\"${NEWHASH}\";/" "${CRYPT_SRC_PATH}/fips_test.c.bak" >"${CRYPT_SRC_PATH}/fips_test.c"
        make clean
    fi
fi

if ! $MAKE check
then
    echo "fips-check: Test failed. Debris left for analysis."
    exit 3
fi

# Clean up
popd || exit 2
if [ "$KEEP" = "no" ];
then
    rm -rf "$TEST_DIR"
fi
