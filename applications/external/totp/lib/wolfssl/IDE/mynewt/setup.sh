#!/bin/bash -e

# this scrypt deploy wolfssl and wolfcrypto source code to mynewt project
# run as bash "mynewt project root directory path"

SCRIPTDIR=`dirname $0`
SCRIPTDIR=`cd $SCRIPTDIR && pwd -P`
WOLFSSL_MYNEWTDIR=${SCRIPTDIR}
WOLFSSL_MYNEWTDIR=`cd $WOLFSSL_MYNEWTDIR && pwd -P`
BASEDIR=${SCRIPTDIR}/../..
BASEDIR=`cd ${BASEDIR} && pwd -P`

if [ $# -ne 1 ]; then
    echo "Usage: $0 'mynewt project root directory path'" 1>&2
    exit 1
fi
MYNEWT_PROJECT=$1

if [ ! -d $MYNEWT_PROJECT ] || [ ! -f $MYNEWT_PROJECT/project.yml ]; then
    echo "target directory is not mynewt project.: $MYNEWT_PROJECT"
    exit 1
fi

# create wolfssl pkgs to mynewt project
pushd $MYNEWT_PROJECT > /dev/null
echo "create crypto/wolfssl pkg"
/bin/rm -rf crypto/wolfssl
newt pkg new crypto/wolfssl
/bin/rm -rf crypto/wolfssl/include
/bin/mkdir -p crypto/wolfssl/include
/bin/rm -rf crypto/wolfssl/src
/bin/mkdir -p crypto/wolfssl/src

echo "create apps/wolfcrypttest pkg"
/bin/rm -rf apps/wolfcrypttest
newt pkg new -t app apps/wolfcrypttest
/bin/rm -rf apps/wolfcrypttest/include 
/bin/rm -rf apps/wolfcrypttest/src
/bin/mkdir -p apps/wolfcrypttest/src

popd > /dev/null # $MYNEWT_PROJECT

# deploy source files and pkg
pushd $BASEDIR > /dev/null

# deploy to crypto/wolfssl
echo "deploy wolfssl sources to crypto/wolfssl"
/bin/cp $WOLFSSL_MYNEWTDIR/crypto.wolfssl.pkg.yml $MYNEWT_PROJECT/crypto/wolfssl/pkg.yml
/bin/cp $WOLFSSL_MYNEWTDIR/crypto.wolfssl.syscfg.yml $MYNEWT_PROJECT/crypto/wolfssl/syscfg.yml

/bin/mkdir -p $MYNEWT_PROJECT/crypto/wolfssl/src/src
/bin/cp ./src/*.c $MYNEWT_PROJECT/crypto/wolfssl/src/src

/bin/mkdir -p $MYNEWT_PROJECT/crypto/wolfssl/src/wolfcrypt/src
/bin/cp ./wolfcrypt/src/*.asm $MYNEWT_PROJECT/crypto/wolfssl/src/wolfcrypt/src
/bin/cp ./wolfcrypt/src/*.c $MYNEWT_PROJECT/crypto/wolfssl/src/wolfcrypt/src
/bin/mkdir -p $MYNEWT_PROJECT/crypto/wolfssl/src/wolfcrypt/src/port/mynewt
/bin/cp ./wolfcrypt/src/port/mynewt/* $MYNEWT_PROJECT/crypto/wolfssl/src/wolfcrypt/src/port/mynewt

/bin/mkdir -p $MYNEWT_PROJECT/crypto/wolfssl/include/wolfssl
/bin/cp -r wolfssl/* $MYNEWT_PROJECT/crypto/wolfssl/include/wolfssl/

# deploy to apps/wolfcrypttest
echo "deploy unit test sources to apps/wolfcrypttest"
/bin/cp $WOLFSSL_MYNEWTDIR/apps.wolfcrypttest.pkg.yml $MYNEWT_PROJECT/apps/wolfcrypttest/pkg.yml

/bin/mkdir -p $MYNEWT_PROJECT/apps/wolfcrypttest/include/wolfcrypt/test
/bin/cp -r wolfcrypt/test/test.h $MYNEWT_PROJECT/apps/wolfcrypttest/include/wolfcrypt/test/

/bin/mkdir -p $MYNEWT_PROJECT/apps/wolfcrypttest/src
/bin/cp wolfcrypt/test/test.c $MYNEWT_PROJECT/apps/wolfcrypttest/src/main.c

popd > /dev/null # $BASEDIR
