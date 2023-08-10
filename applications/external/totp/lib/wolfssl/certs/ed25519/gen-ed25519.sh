#!/bin/bash

EXAMPLE=$1
echo "This uses ed25519 certificate generator from wolfssl-examples github"
echo "The script takes in the directory to wolfssl-examples"
echo "https://github.com/wolfSSL/wolfssl-examples/pull/52"

pushd ${EXAMPLE}
make
if [ $? -ne 0 ]; then
    echo "Unable to build example"
    exit 1
fi

./tls.sh
popd
mv ${EXAMPLE}/*.pem .
mv ${EXAMPLE}/*.der .

convert() {
    echo -en "\x30\x2e" > ${NAME}-ed25519-priv.der
    head -c 48 ${NAME}-ed25519-key.der | tail -c 46 >> ${NAME}-ed25519-priv.der

    echo "-----BEGIN PRIVATE KEY-----" > ${NAME}-ed25519-priv.pem
    openssl base64 -in ${NAME}-ed25519-priv.der >> ${NAME}-ed25519-priv.pem
    echo "-----END PRIVATE KEY-----" >> ${NAME}-ed25519-priv.pem
}

NAME=server convert
NAME=client convert
NAME=root convert
NAME=ca convert

cp server-ed25519.pem server-ed25519-cert.pem

