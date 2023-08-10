#!/bin/sh

for key in root ca server client
do

  openssl genpkey -algorithm ED25519 > ${key}-ed25519-priv.pem

  openssl pkey -in ${key}-ed25519-priv.pem -outform DER -out ${key}-ed25519-priv.der

  openssl pkey -in ${key}-ed25519-priv.pem -outform PEM -pubout -out ${key}-ed25519-key.pem

  openssl pkey -in ${key}-ed25519-priv.pem -outform DER -pubout -out ${key}-ed25519-key.der

done


