#!/bin/bash

# Run this script from the wolfSSL root as `./scripts/memtest.sh`.

./autogen.sh
./configure --enable-debug --disable-shared --enable-memtest \
        --enable-opensslextra --enable-des3 --enable-dh --enable-ecc --enable-aesgcm --enable-aesccm \
        --enable-sniffer --enable-psk --enable-camellia --enable-sha512 --enable-crl --enable-ocsp --enable-savesession \
        --enable-savecert --enable-atomicuser --enable-pkcallbacks --enable-scep;

        #DTLS has issue with trapping client/server failure disconnect since its stateless. Need to find way to communicate failure through file system.
        #--enable-dtls
make

for i in {1..1000}
do
    echo "Trying $i...\n"

        ./tests/unit.test > ./scripts/memtest.txt 2>&1

        RESULT=$?
        [ $RESULT -eq 139 ] && echo "Mem Seg Fault" && exit 1
done
echo "Loop SUCCESS"
