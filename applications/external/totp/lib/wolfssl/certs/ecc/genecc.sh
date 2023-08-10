#!/bin/bash

# run from wolfssl root

rm ./certs/ecc/*.old
rm ./certs/ecc/index.txt*
rm ./certs/ecc/serial
rm ./certs/ecc/crlnumber

touch ./certs/ecc/index.txt
echo 1000 > ./certs/ecc/serial
echo 2000 > ./certs/ecc/crlnumber

# generate ECC 256-bit CA
if [ -f ./certs/ca-ecc-key.pem ]; then
    openssl req -config ./certs/ecc/wolfssl.cnf -extensions v3_ca -x509 -nodes -key ./certs/ca-ecc-key.pem -out ./certs/ca-ecc-cert.pem -sha256 \
        -days 7300 -batch -subj "/C=US/ST=Washington/L=Seattle/O=wolfSSL/OU=Development/CN=www.wolfssl.com/emailAddress=info@wolfssl.com"
else
    openssl ecparam -out ./certs/ca-ecc-key.par -name prime256v1
    openssl req -config ./certs/ecc/wolfssl.cnf -extensions v3_ca -x509 -nodes -newkey ec:./certs/ca-ecc-key.par -keyout ./certs/ca-ecc-key.pem -out ./certs/ca-ecc-cert.pem -sha256 \
        -days 7300 -batch -subj "/C=US/ST=Washington/L=Seattle/O=wolfSSL/OU=Development/CN=www.wolfssl.com/emailAddress=info@wolfssl.com"
fi

openssl x509 -in ./certs/ca-ecc-cert.pem -inform PEM -out ./certs/ca-ecc-cert.der -outform DER
openssl ec -in ./certs/ca-ecc-key.pem -inform PEM -out ./certs/ca-ecc-key.der -outform DER

rm ./certs/ca-ecc-key.par

# Gen CA CRL
openssl ca -batch -config ./certs/ecc/wolfssl.cnf -gencrl -crldays 1000 -out ./certs/crl/caEccCrl.pem -keyfile ./certs/ca-ecc-key.pem -cert ./certs/ca-ecc-cert.pem



# Generate ECC 256-bit server cert
openssl req -config ./certs/ecc/wolfssl.cnf -sha256 -new -key ./certs/ecc-key.pem -out ./certs/server-ecc-req.pem -subj "/C=US/ST=Washington/L=Seattle/O=Eliptic/OU=ECC/CN=www.wolfssl.com/emailAddress=info@wolfssl.com/"
openssl x509 -req -in ./certs/server-ecc-req.pem -CA ./certs/ca-ecc-cert.pem -CAkey ./certs/ca-ecc-key.pem -CAcreateserial -out ./certs/server-ecc.pem -sha256

# Sign server certificate
openssl ca -batch -config ./certs/ecc/wolfssl.cnf -extensions server_cert -days 3650 -notext -md sha256 -in ./certs/server-ecc-req.pem -out ./certs/server-ecc.pem
openssl x509 -in ./certs/server-ecc.pem -outform der -out ./certs/server-ecc.der

# Generate ECC 256-bit self-signed server cert
openssl x509 -req -in ./certs/server-ecc-req.pem -days 3650 -extfile ./certs/ecc/wolfssl.cnf -extensions server_cert -signkey ./certs/ecc-key.pem -text -out ./certs/server-ecc-self.pem
openssl x509 -inform pem -in ./certs/server-ecc-self.pem -outform der -out ./certs/server-ecc-self.der

rm ./certs/server-ecc-req.pem



# generate ECC 384-bit CA
if [ -f ./certs/ca-ecc384-key.pem ]; then
    openssl req -config ./certs/ecc/wolfssl_384.cnf -extensions v3_ca -x509 -nodes -key ./certs/ca-ecc384-key.pem -out ./certs/ca-ecc384-cert.pem -sha384 \
        -days 7300 -batch -subj "/C=US/ST=Washington/L=Seattle/O=wolfSSL/OU=Development/CN=www.wolfssl.com/emailAddress=info@wolfssl.com"
else
    openssl ecparam -out ./certs/ca-ecc384-key.par -name secp384r1
    openssl req -config ./certs/ecc/wolfssl_384.cnf -extensions v3_ca -x509 -nodes -newkey ec:./certs/ca-ecc384-key.par -keyout ./certs/ca-ecc384-key.pem -out ./certs/ca-ecc384-cert.pem -sha384 \
        -days 7300 -batch -subj "/C=US/ST=Washington/L=Seattle/O=wolfSSL/OU=Development/CN=www.wolfssl.com/emailAddress=info@wolfssl.com"
fi

openssl x509 -in ./certs/ca-ecc384-cert.pem -inform PEM -out ./certs/ca-ecc384-cert.der -outform DER
openssl ec -in ./certs/ca-ecc384-key.pem -inform PEM -out ./certs/ca-ecc384-key.der -outform DER

rm ./certs/ca-ecc384-key.par

# Gen CA CRL
openssl ca -batch -config ./certs/ecc/wolfssl_384.cnf -gencrl -crldays 1000 -out ./certs/crl/caEcc384Crl.pem -keyfile ./certs/ca-ecc384-key.pem -cert ./certs/ca-ecc384-cert.pem



# Generate ECC 384-bit server cert
if [ -f ./certs/server-ecc384-key.pem ]; then
    openssl req -config ./certs/ecc/wolfssl_384.cnf -sha384 -x509 -nodes -key ./certs/server-ecc384-key.pem -out ./certs/server-ecc384-req.pem \
        -subj "/C=US/ST=Washington/L=Seattle/O=Eliptic/OU=ECC384Srv/CN=www.wolfssl.com/emailAddress=info@wolfssl.com/"
else
    openssl ecparam -out ./certs/server-ecc384-key.par -name secp384r1
    openssl req -config ./certs/ecc/wolfssl_384.cnf -sha384 -x509 -nodes -newkey ec:./certs/server-ecc384-key.par -keyout ./certs/server-ecc384-key.pem -out ./certs/server-ecc384-req.pem \
        -subj "/C=US/ST=Washington/L=Seattle/O=Eliptic/OU=ECC384Srv/CN=www.wolfssl.com/emailAddress=info@wolfssl.com/"
fi
openssl req -config ./certs/ecc/wolfssl_384.cnf -sha384 -new -key ./certs/server-ecc384-key.pem -out ./certs/server-ecc384-req.pem \
        -subj "/C=US/ST=Washington/L=Seattle/O=Eliptic/OU=ECC384Srv/CN=www.wolfssl.com/emailAddress=info@wolfssl.com/"
openssl ec -in ./certs/server-ecc384-key.pem -inform PEM -out ./certs/server-ecc384-key.der -outform DER

# Sign server certificate
openssl ca -batch -config ./certs/ecc/wolfssl_384.cnf -extensions server_cert -days 10950 -notext -md sha384 -in ./certs/server-ecc384-req.pem -out ./certs/server-ecc384-cert.pem
openssl x509 -in ./certs/server-ecc384-cert.pem -outform der -out ./certs/server-ecc384-cert.der

rm ./certs/server-ecc384-req.pem
rm ./certs/server-ecc384-key.par

# Generate ECC 384-bit client cert
if [ -f ./certs/client-ecc384-key.pem ]; then
    openssl req -config ./certs/ecc/wolfssl_384.cnf -sha384 -x509 -nodes -key ./certs/client-ecc384-key.pem -out ./certs/client-ecc384-req.pem \
        -subj "/C=US/ST=Washington/L=Seattle/O=Eliptic/OU=ECC384Cli/CN=www.wolfssl.com/emailAddress=info@wolfssl.com/"
else
    openssl ecparam -out ./certs/client-ecc384-key.par -name secp384r1
    openssl req -config ./certs/ecc/wolfssl_384.cnf -sha384 -x509 -nodes -newkey ec:./certs/client-ecc384-key.par -keyout ./certs/client-ecc384-key.pem -out ./certs/client-ecc384-req.pem \
        -subj "/C=US/ST=Washington/L=Seattle/O=Eliptic/OU=ECC384Cli/CN=www.wolfssl.com/emailAddress=info@wolfssl.com/"
fi
openssl req -config ./certs/ecc/wolfssl_384.cnf -sha384 -new -key ./certs/client-ecc384-key.pem -out ./certs/client-ecc384-req.pem \
        -subj "/C=US/ST=Washington/L=Seattle/O=Eliptic/OU=ECC384Clit/CN=www.wolfssl.com/emailAddress=info@wolfssl.com/"
openssl ec -in ./certs/client-ecc384-key.pem -inform PEM -out ./certs/client-ecc384-key.der -outform DER

# Sign client certificate
openssl ca -batch -config ./certs/ecc/wolfssl_384.cnf -extensions usr_cert -days 10950 -notext -md sha384 -in ./certs/client-ecc384-req.pem -out ./certs/client-ecc384-cert.pem
openssl x509 -in ./certs/client-ecc384-cert.pem -outform der -out ./certs/client-ecc384-cert.der

rm ./certs/client-ecc384-req.pem
rm ./certs/client-ecc384-key.par


# Generate ECC Kerberos Keys
if [ -f ./certs/ecc/secp256k1-key.pem ]; then
        openssl ecparam -name secp256k1 -genkey -noout -out ./certs/ecc/secp256k1-key.pem
        openssl ec -in ./certs/ecc/secp256k1-key.pem -inform PEM -out ./certs/ecc/secp256k1-key.der -outform DER
fi
# Create self-signed ECC Kerberos certificates
openssl req -config ./certs/ecc/wolfssl.cnf -sha256 -new -key ./certs/ecc/secp256k1-key.pem -out ./certs/ecc/server-secp256k1-req.pem -subj "/C=US/ST=Washington/L=Seattle/O=Eliptic/OU=ECC256K1-SRV/CN=www.wolfssl.com/emailAddress=info@wolfssl.com/"
openssl x509 -req -in ./certs/ecc/server-secp256k1-req.pem -days 3650 -extfile ./certs/ecc/wolfssl.cnf -extensions server_cert -signkey ./certs/ecc/secp256k1-key.pem -text -out ./certs/ecc/server-secp256k1-cert.pem
openssl x509 -inform pem -in ./certs/ecc/server-secp256k1-cert.pem -outform der -out ./certs/ecc/server-secp256k1-cert.der
rm ./certs/ecc/server-secp256k1-req.pem

openssl req -config ./certs/ecc/wolfssl.cnf -sha256 -new -key ./certs/ecc/secp256k1-key.pem -out ./certs/ecc/client-secp256k1-req.pem -subj "/C=US/ST=Washington/L=Seattle/O=Eliptic/OU=ECC256K1-CLI/CN=www.wolfssl.com/emailAddress=info@wolfssl.com/"
openssl x509 -req -in ./certs/ecc/client-secp256k1-req.pem -days 3650 -extfile ./certs/ecc/wolfssl.cnf -extensions usr_cert -signkey ./certs/ecc/secp256k1-key.pem -text -out ./certs/ecc/client-secp256k1-cert.pem
openssl x509 -inform pem -in ./certs/ecc/client-secp256k1-cert.pem -outform der -out ./certs/ecc/client-secp256k1-cert.der
rm ./certs/ecc/client-secp256k1-req.pem

# Generate ECC Brainpool Keys
if [ -f ./certs/ecc/bp256r1-key.pem ]; then
        openssl ecparam -name brainpoolP256r1 -genkey -noout -out ./certs/ecc/bp256r1-key.pem
        openssl ec -in ./certs/ecc/bp256r1-key.pem -inform PEM -out ./certs/ecc/bp256r1-key.der -outform DER
fi
# Create self-signed ECC Brainpool certificates
openssl req -config ./certs/ecc/wolfssl.cnf -sha256 -new -key ./certs/ecc/bp256r1-key.pem -out ./certs/ecc/server-bp256r1-req.pem -subj "/C=US/ST=Washington/L=Seattle/O=Eliptic/OU=ECC256BPR1-SRV/CN=www.wolfssl.com/emailAddress=info@wolfssl.com/"
openssl x509 -req -in ./certs/ecc/server-bp256r1-req.pem -days 3650 -extfile ./certs/ecc/wolfssl.cnf -extensions server_cert -signkey ./certs/ecc/bp256r1-key.pem -text -out ./certs/ecc/server-bp256r1-cert.pem
openssl x509 -inform pem -in ./certs/ecc/server-bp256r1-cert.pem -outform der -out ./certs/ecc/server-bp256r1-cert.der
rm ./certs/ecc/server-bp256r1-req.pem

openssl req -config ./certs/ecc/wolfssl.cnf -sha256 -new -key ./certs/ecc/bp256r1-key.pem -out ./certs/ecc/client-bp256r1-req.pem -subj "/C=US/ST=Washington/L=Seattle/O=Eliptic/OU=ECC256BPR1-CLI/CN=www.wolfssl.com/emailAddress=info@wolfssl.com/"
openssl x509 -req -in ./certs/ecc/client-bp256r1-req.pem -days 3650 -extfile ./certs/ecc/wolfssl.cnf -extensions usr_cert -signkey ./certs/ecc/bp256r1-key.pem -text -out ./certs/ecc/client-bp256r1-cert.pem
openssl x509 -inform pem -in ./certs/ecc/client-bp256r1-cert.pem -outform der -out ./certs/ecc/client-bp256r1-cert.der
rm ./certs/ecc/client-bp256r1-req.pem


# update bad certificate with last byte in signature changed
cp ./certs/server-ecc.der ./certs/test/server-cert-ecc-badsig.der
sed '$s/.$/W/' ./certs/test/server-cert-ecc-badsig.der >> ./certs/test/server-cert-ecc-badsig-altered.der
mv ./certs/test/server-cert-ecc-badsig-altered.der ./certs/test/server-cert-ecc-badsig.der
openssl x509 -inform der -in ./certs/test/server-cert-ecc-badsig.der -outform pem -out ./certs/test/server-cert-ecc-badsig.pem

rm ./certs/ecc/*.old
rm ./certs/ecc/index.txt*
rm ./certs/ecc/serial
rm ./certs/ecc/crlnumber
rm ./certs/ecc/index.txt

rm ./certs/1000.pem
rm ./certs/1001.pem
rm ./certs/1002.pem
rm ./certs/ca-ecc-cert.srl

exit 0
