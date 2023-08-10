#!/bin/bash

# run from wolfssl root

# SECP256R1
openssl ecparam -name prime256v1 -genkey -noout -out certs/statickeys/ecc-secp256r1.pem
openssl ec -inform pem -in certs/statickeys/ecc-secp256r1.pem -outform der -out certs/statickeys/ecc-secp256r1.der

# DH 2048-bit (keySz = 29)
# Using one generated and capture with wolfSSL using wc_DhGenerateKeyPair (openssl generates DH keys with 2048-bits... based on the DH "p" prime size)
openssl genpkey -paramfile certs/statickeys/dh-ffdhe2048-params.pem -outform -out certs/statickeys/dh-ffdhe2048.pem
openssl pkey -inform pem -in certs/statickeys/dh-ffdhe2048.pem -outform der -out certs/statickeys/dh-ffdhe2048.der
# Export DH public key as DER and PEM
openssl pkey -inform pem -in certs/statickeys/dh-ffdhe2048.pem -outform der -out certs/statickeys/dh-ffdhe2048-pub.der -pubout
openssl pkey -inform pem -in certs/statickeys/dh-ffdhe2048.pem -outform pem -out certs/statickeys/dh-ffdhe2048-pub.pem -pubout

# X25519 (Curve25519)
openssl genpkey -algorithm x25519 -outform pem -out certs/statickeys/x25519.pem
openssl pkey -inform pem -in certs/statickeys/x25519.pem -outform der -out certs/statickeys/x25519.der
openssl pkey -inform pem -in certs/statickeys/x25519.pem -outform der -out certs/statickeys/x25519-pub.der -pubout
openssl pkey -inform pem -in certs/statickeys/x25519.pem -outform pem -out certs/statickeys/x25519-pub.pem -pubout
