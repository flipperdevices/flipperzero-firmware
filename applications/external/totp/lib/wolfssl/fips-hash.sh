#!/bin/bash

if test ! -x ./wolfcrypt/test/testwolfcrypt
then
    echo "fips-hash: wolfCrypt test missing"
    exit 1
fi

if test ! -s ./wolfcrypt/src/fips_test.c
then
    echo "fips-hash: fips_test.c missing"
    exit 1
fi

NEWHASH=$(./wolfcrypt/test/testwolfcrypt | sed -n 's/hash = \(.*\)/\1/p')
if test -n "$NEWHASH"
then
    cp wolfcrypt/src/fips_test.c wolfcrypt/src/fips_test.c.bak
    sed "s/^\".*\";/\"${NEWHASH}\";/" wolfcrypt/src/fips_test.c.bak >wolfcrypt/src/fips_test.c
fi

