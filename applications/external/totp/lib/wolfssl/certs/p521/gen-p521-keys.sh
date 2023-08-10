#!/bin/sh

for key in root ca server client
do

  openssl ecparam -name secp521r1 -genkey -noout > ${key}-p521-priv.pem

  openssl pkey -in ${key}-p521-priv.pem -outform DER -out ${key}-p521-priv.der

  openssl pkey -in ${key}-p521-priv.pem -outform PEM -pubout -out ${key}-p521-key.pem

  openssl pkey -in ${key}-p521-priv.pem -outform DER -pubout -out ${key}-p521-key.der

done


