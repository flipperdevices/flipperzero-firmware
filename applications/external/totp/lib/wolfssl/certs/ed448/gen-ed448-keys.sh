#!/bin/sh

for key in root ca server client
do

  openssl genpkey -algorithm ED448 > ${key}-ed448-priv.pem

  openssl pkey -in ${key}-ed448-priv.pem -outform DER -out ${key}-ed448-priv.der

  openssl pkey -in ${key}-ed448-priv.pem -outform PEM -pubout -out ${key}-ed448-key.pem

  openssl pkey -in ${key}-ed448-priv.pem -outform DER -pubout -out ${key}-ed448-key.der

done


