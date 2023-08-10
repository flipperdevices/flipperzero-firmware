#!/bin/sh

for key in root ca server client
do

  openssl genpkey -algorithm RSA-PSS -pkeyopt rsa_keygen_bits:2048 -pkeyopt rsa_pss_keygen_md:sha256 -pkeyopt rsa_pss_keygen_mgf1_md:sha256 -pkeyopt rsa_pss_keygen_saltlen:32 > ${key}-rsapss-priv.pem

  openssl pkey -in ${key}-rsapss-priv.pem -outform DER -out ${key}-rsapss-priv.der

  openssl pkey -in ${key}-rsapss-priv.pem -outform PEM -pubout -out ${key}-rsapss-key.pem

  openssl pkey -in ${key}-rsapss-priv.pem -outform DER -pubout -out ${key}-rsapss-key.der

done

for key in root-3072 ca-3072 server-3072 client-3072
do

  openssl genpkey -algorithm RSA-PSS -pkeyopt rsa_keygen_bits:3072 > ${key}-rsapss-priv.pem

  openssl pkey -in ${key}-rsapss-priv.pem -outform DER -out ${key}-rsapss-priv.der

  openssl pkey -in ${key}-rsapss-priv.pem -outform PEM -pubout -out ${key}-rsapss-key.pem

  openssl pkey -in ${key}-rsapss-priv.pem -outform DER -pubout -out ${key}-rsapss-key.der

done


