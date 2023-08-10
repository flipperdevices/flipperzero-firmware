#!/bin/sh

for key in root ca server client
do

  openssl genpkey -algorithm sm2 > ${key}-sm2-priv.pem

  openssl pkey -in ${key}-sm2-priv.pem -outform DER -out ${key}-sm2-priv.der

  openssl pkey -in ${key}-sm2-priv.pem -outform PEM -pubout -out ${key}-sm2-key.pem

  openssl pkey -in ${key}-sm2-priv.pem -outform DER -pubout -out ${key}-sm2-key.der

done


