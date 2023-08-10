#!/bin/sh

# Script for generating RSA and ECC Intermediate CA and server/client certs based on it.

# Result is chains that looks like:
# RSA Server
#  ROOT: ./certs/ca-cert.pem
#      C=US, ST=Montana, L=Bozeman, O=Sawtooth, OU=Consulting, CN=www.wolfssl.com/emailAddress=info@wolfssl.com)
#    INTERMEDIATE: ./certs/intermediate/ca-int-cert.pem
#        C=US, ST=Washington, L=Seattle, O=wolfSSL, OU=Development, CN=wolfSSL Intermediate CA/emailAddress=info@wolfssl.com
#      INTERMEDIATE2: ./certs/intermediate/ca-int2-cert.pem
#          C=US, ST=Washington, L=Seattle, O=wolfSSL, OU=Development, CN=wolfSSL Intermediate2 CA/emailAddress=info@wolfssl.com
#        SERVER: ./certs/intermediate/server-int-cert.pem
#            C=US, ST=Washington, L=Seattle, O=wolfSSL, OU=Development, CN=wolfSSL Server Chain/emailAddress=info@wolfssl.com

# RSA Client
#  ROOT: ./certs/ca-cert.pem
#      C=US, ST=Montana, L=Bozeman, O=Sawtooth, OU=Consulting, CN=www.wolfssl.com/emailAddress=info@wolfssl.com)
#    INTERMEDIATE: ./certs/intermediate/ca-int-cert.pem
#        C=US, ST=Washington, L=Seattle, O=wolfSSL, OU=Development, CN=wolfSSL Intermediate CA/emailAddress=info@wolfssl.com
#      INTERMEDIATE: ./certs/intermediate/ca-int2-cert.pem
#          C=US, ST=Washington, L=Seattle, O=wolfSSL, OU=Development, CN=wolfSSL Intermediate2 CA/emailAddress=info@wolfssl.com
#        CLIENT: ./certs/intermediate/client-int-cert.pem
#            C=US, ST=Washington, L=Seattle, O=wolfSSL, OU=Development, CN=wolfSSL Client Chain/emailAddress=info@wolfssl.com

# ECC Server
#  ROOT: ./certs/ca-ecc-cert.pem
#      C=US, ST=Washington, L=Seattle, O=wolfSSL, OU=Development, CN=www.wolfssl.com/emailAddress=info@wolfssl.com
#    INTERMEDIATE: ./certs/intermediate/ca-int-ecc-cert.pem
#        C=US, ST=Washington, L=Seattle, O=wolfSSL, OU=Development, CN=wolfSSL Intermediate CA ECC/emailAddress=info@wolfssl.com
#      INTERMEDIATE2: ./certs/intermediate/ca-int-ecc-cert.pem
#          C=US, ST=Washington, L=Seattle, O=wolfSSL, OU=Development, CN=wolfSSL Intermediate2 CA ECC/emailAddress=info@wolfssl.com
#        SERVER: ./certs/intermediate/server-int-ecc-cert.pem
#            C=US, ST=Washington, L=Seattle, O=wolfSSL, OU=Development, CN=wolfSSL Server Chain ECC/emailAddress=info@wolfssl.com

# ECC Client
#  ROOT: ./certs/ca-ecc-cert.pem
#      C=US, ST=Washington, L=Seattle, O=wolfSSL, OU=Development, CN=www.wolfssl.com/emailAddress=info@wolfssl.com
#    INTERMEDIATE: ./certs/intermediate/ca-int-ecc-cert.pem
#        C=US, ST=Washington, L=Seattle, O=wolfSSL, OU=Development, CN=wolfSSL Intermediate CA ECC/emailAddress=info@wolfssl.com
#      INTERMEDIATE2: ./certs/intermediate/ca-int2-ecc-cert.pem
#          C=US, ST=Washington, L=Seattle, O=wolfSSL, OU=Development, CN=wolfSSL Intermediate2 CA ECC/emailAddress=info@wolfssl.com
#        CLIENT: ./certs/intermediate/client-int-ecc-cert.pem
#            C=US, ST=Washington, L=Seattle, O=wolfSSL, OU=Development, CN=wolfSSL Client Chain ECC/emailAddress=info@wolfssl.com


# Run from wolfssl-root as  `./certs/intermediate/genintcerts.sh`
# To cleanup temp files use `./certs/intermediate/genintcerts.sh clean`
# To cleanup all files use  `./certs/intermediate/genintcerts.sh cleanall`

dir="."

cleanup_files(){
    rm -f ./certs/intermediate/index.*
    rm -f ./certs/intermediate/*.old
    rm -f ./certs/intermediate/serial
    rm -f ./certs/intermediate/crlnumber
    rm -f ./certs/intermediate/*.cnf
    rm -rf ./certs/intermediate/new_certs
    exit 0
}

check_result() {
    if [ $1 -ne 0 ]; then
        echo "Step Failed, Abort"
        exit 1
    else
        echo "Step Succeeded!"
    fi
}

# Args: 1=CnfFile, 2=Key, 3=Cert
create_ca_config() {
    echo "# Generated openssl conf"                              > "$1"
    echo "[ ca ]"                                               >> "$1"
    echo "default_ca = CA_default"                              >> "$1"
    echo ""                                                     >> "$1"
    echo "[ CA_default ]"                                       >> "$1"
    echo "certs             = $dir/certs/intermediate"          >> "$1"
    echo "new_certs_dir     = $dir/certs/intermediate/new_certs">> "$1"
    echo "database          = $dir/certs/intermediate/index.txt">> "$1"
    echo "serial            = $dir/certs/intermediate/serial"   >> "$1"
    echo "RANDFILE          = $dir/private/.rand"               >> "$1"
    echo ""                                                     >> "$1"
    echo "private_key       = $dir/$2"                          >> "$1"
    echo "certificate       = $dir/$3"                          >> "$1"
    echo ""                                                     >> "$1"
    echo "crlnumber         = $dir/certs/intermediate/crlnumber">> "$1"
    echo "crl_extensions    = crl_ext"                          >> "$1"
    echo "default_crl_days  = 1000"                             >> "$1"
    echo "default_md        = sha256"                           >> "$1"
    echo ""                                                     >> "$1"
    echo "name_opt          = ca_default"                       >> "$1"
    echo "cert_opt          = ca_default"                       >> "$1"
    echo "default_days      = 3650"                             >> "$1"
    echo "preserve          = no"                               >> "$1"
    echo "policy            = policy_loose"                     >> "$1"
    echo ""                                                     >> "$1"
    echo "[ policy_strict ]"                                    >> "$1"
    echo "countryName             = match"                      >> "$1"
    echo "stateOrProvinceName     = match"                      >> "$1"
    echo "organizationName        = match"                      >> "$1"
    echo "organizationalUnitName  = optional"                   >> "$1"
    echo "commonName              = supplied"                   >> "$1"
    echo "emailAddress            = optional"                   >> "$1"
    echo ""                                                     >> "$1"
    echo "[ policy_loose ]"                                     >> "$1"
    echo "countryName             = optional"                   >> "$1"
    echo "stateOrProvinceName     = optional"                   >> "$1"
    echo "localityName            = optional"                   >> "$1"
    echo "organizationName        = optional"                   >> "$1"
    echo "organizationalUnitName  = optional"                   >> "$1"
    echo "commonName              = supplied"                   >> "$1"
    echo "emailAddress            = optional"                   >> "$1"
    echo ""                                                     >> "$1"
    echo "[ req ]"                                              >> "$1"
    echo "default_bits        = 2048"                           >> "$1"
    echo "distinguished_name  = req_distinguished_name"         >> "$1"
    echo "string_mask         = utf8only"                       >> "$1"
    echo "default_md          = sha256"                         >> "$1"
    echo "x509_extensions     = v3_ca"                          >> "$1"
    echo ""                                                     >> "$1"
    echo "[ req_distinguished_name ]"                           >> "$1"
    echo "countryName                     = US"                 >> "$1"
    echo "stateOrProvinceName             = Washington"         >> "$1"
    echo "localityName                    = Seattle"            >> "$1"
    echo "organizationName                = wolfSSL"            >> "$1"
    echo "organizationalUnitName          = Development"        >> "$1"
    echo "commonName                      = www.wolfssl.com"    >> "$1"
    echo "emailAddress                    = info@wolfssl.com"   >> "$1"
    echo ""                                                     >> "$1"
    echo "[ v3_ca ]"                                            >> "$1"
    echo "subjectKeyIdentifier = hash"                          >> "$1"
    echo "authorityKeyIdentifier = keyid:always,issuer"         >> "$1"
    echo "basicConstraints = critical, CA:true"                 >> "$1"
    echo "keyUsage = critical, digitalSignature, cRLSign, keyCertSign">> "$1"
    echo ""                                                     >> "$1"
    echo "[ v3_intermediate_ca ]"                               >> "$1"
    echo "subjectKeyIdentifier = hash"                          >> "$1"
    echo "authorityKeyIdentifier = keyid:always,issuer"         >> "$1"
    echo "basicConstraints = critical, CA:true, pathlen:1"      >> "$1"
    echo "keyUsage = critical, digitalSignature, cRLSign, keyCertSign">> "$1"
    echo ""                                                     >> "$1"
    echo "[ v3_intermediate2_ca ]"                              >> "$1"
    echo "subjectKeyIdentifier = hash"                          >> "$1"
    echo "authorityKeyIdentifier = keyid:always,issuer"         >> "$1"
    echo "basicConstraints = critical, CA:true, pathlen:1"      >> "$1"
    echo "keyUsage = critical, digitalSignature, cRLSign, keyCertSign">> "$1"
    echo ""                                                     >> "$1"
    echo "[ usr_cert ]"                                         >> "$1"
    echo "basicConstraints = CA:FALSE"                          >> "$1"
    echo "nsCertType = client, email"                           >> "$1"
    echo "subjectKeyIdentifier = hash"                          >> "$1"
    echo "authorityKeyIdentifier = keyid,issuer"                >> "$1"
    echo "keyUsage = critical, nonRepudiation, digitalSignature, keyEncipherment">> "$1"
    echo "extendedKeyUsage = clientAuth, emailProtection"       >> "$1"
    echo ""                                                     >> "$1"
    echo "[ server_cert ]"                                      >> "$1"
    echo "basicConstraints = CA:FALSE"                          >> "$1"
    echo "nsCertType = server"                                  >> "$1"
    echo "subjectKeyIdentifier = hash"                          >> "$1"
    echo "authorityKeyIdentifier = keyid,issuer:always"         >> "$1"
    echo "keyUsage = critical, digitalSignature, keyEncipherment, keyAgreement">> "$1"
    echo "extendedKeyUsage = serverAuth"                        >> "$1"
    echo ""                                                     >> "$1"
    echo "[ crl_ext ]"                                          >> "$1"
    echo "authorityKeyIdentifier=keyid:always"                  >> "$1"
}

# Args: 1=reqcnf, 2=signcnf, 3=keyfile, 4=certfile, 5=ext, 6=subj, 7=days
create_cert() {
    openssl req -config ./certs/intermediate/$1.cnf -new -sha256 \
        -key $3 \
        -out ./certs/intermediate/tmp.csr \
        -subj "/C=US/ST=Washington/L=Seattle/O=wolfSSL/OU=Development/CN=$6/emailAddress=info@wolfssl.com"
    check_result $?
    openssl ca -config ./certs/intermediate/$2.cnf -extensions $5 -days $7 -notext -md sha256 \
        -in ./certs/intermediate/tmp.csr -out ./certs/intermediate/$4.pem -batch
    check_result $?
    rm ./certs/intermediate/tmp.csr

    # Convert Cert to DER
    openssl x509 -in ./certs/intermediate/$4.pem -inform PEM -out ./certs/intermediate/$4.der -outform DER
    check_result $?

    # Add text to cert PEM file
    openssl x509 -in ./certs/intermediate/$4.pem -text > ./certs/intermediate/tmp.pem
    check_result $?
    mv ./certs/intermediate/tmp.pem ./certs/intermediate/$4.pem
}

if [ "$1" = "clean" ]; then
    echo "Cleaning temp files"
    cleanup_files
fi
if [ "$1" = "cleanall" ]; then
    echo "Cleaning all files"
    rm -f ./certs/intermediate/*.pem
    rm -f ./certs/intermediate/*.der
    rm -f ./certs/intermediate/*.csr
    cleanup_files
fi

# Make sure required CA files exist and are populated
rm -f ./certs/intermediate/index.*
touch ./certs/intermediate/index.txt
if [ ! -f ./certs/intermediate/serial ]; then
    echo 1000 > ./certs/intermediate/serial
fi
if [ ! -f ./certs/intermediate/crlnumber ]; then
    echo 2000 > ./certs/intermediate/crlnumber
fi
if [ ! -d ./certs/intermediate/new_certs ]; then
    mkdir ./certs/intermediate/new_certs
fi


# RSA
echo "Creating RSA CA configuration cnf files"
create_ca_config ./certs/intermediate/wolfssl_root.cnf certs/ca-key.pem certs/ca-cert.pem
create_ca_config ./certs/intermediate/wolfssl_int.cnf certs/intermediate/ca-int-key.pem certs/intermediate/ca-int-cert.pem
create_ca_config ./certs/intermediate/wolfssl_int2.cnf certs/intermediate/ca-int2-key.pem certs/intermediate/ca-int2-cert.pem

if [ ! -f ./certs/intermediate/ca-int-key.pem ]; then
    echo "Make Intermediate RSA CA Key"
    openssl genrsa -out ./certs/intermediate/ca-int-key.pem 2048
    check_result $?
    openssl rsa -in ./certs/intermediate/ca-int-key.pem -inform PEM -out ./certs/intermediate/ca-int-key.der -outform DER
    check_result $?
fi
if [ ! -f ./certs/intermediate/ca-int2-key.pem ]; then
    echo "Make Intermediate2 RSA CA Key"
    openssl genrsa -out ./certs/intermediate/ca-int2-key.pem 2048
    check_result $?
    openssl rsa -in ./certs/intermediate/ca-int2-key.pem -inform PEM -out ./certs/intermediate/ca-int2-key.der -outform DER
    check_result $?
fi

echo "Create RSA Intermediate CA signed by root"
create_cert wolfssl_int wolfssl_root ./certs/intermediate/ca-int-key.pem ca-int-cert v3_intermediate_ca "wolfSSL Intermediate CA" 7300

echo "Create RSA Intermediate2 CA signed by RSA Intermediate"
create_cert wolfssl_int2 wolfssl_int ./certs/intermediate/ca-int2-key.pem ca-int2-cert v3_intermediate2_ca "wolfSSL Intermediate2 CA" 7300

echo "Create RSA Server Certificate signed by intermediate2"
create_cert wolfssl_int2 wolfssl_int2 ./certs/server-key.pem server-int-cert server_cert "wolfSSL Server Chain" 3650

echo "Create RSA Client Certificate signed by intermediate2"
create_cert wolfssl_int2 wolfssl_int2 ./certs/client-key.pem client-int-cert usr_cert "wolfSSL Client Chain" 3650

echo "Generate CRLs for new certificates"
openssl ca -config ./certs/intermediate/wolfssl_root.cnf -gencrl -crldays 1000 -out ./certs/crl/ca-int.pem -keyfile ./certs/intermediate/ca-int-key.pem -cert ./certs/intermediate/ca-int-cert.pem
check_result $?
openssl ca -config ./certs/intermediate/wolfssl_int.cnf  -gencrl -crldays 1000 -out ./certs/crl/ca-int2.pem -keyfile ./certs/intermediate/ca-int2-key.pem -cert ./certs/intermediate/ca-int2-cert.pem
check_result $?
openssl ca -config ./certs/intermediate/wolfssl_int2.cnf -gencrl -crldays 1000 -out ./certs/crl/server-int.pem -keyfile ./certs/server-key.pem -cert ./certs/intermediate/server-int-cert.pem
check_result $?
openssl ca -config ./certs/intermediate/wolfssl_int2.cnf -gencrl -crldays 1000 -out ./certs/crl/client-int.pem -keyfile ./certs/client-key.pem -cert ./certs/intermediate/client-int-cert.pem
check_result $?

echo "Assemble test chains - peer first, then intermediate2, then intermediate"
openssl x509 -in ./certs/intermediate/server-int-cert.pem  > ./certs/intermediate/server-chain.pem
openssl x509 -in ./certs/intermediate/ca-int2-cert.pem    >> ./certs/intermediate/server-chain.pem
openssl x509 -in ./certs/intermediate/ca-int-cert.pem     >> ./certs/intermediate/server-chain.pem
cat ./certs/intermediate/server-int-cert.der ./certs/intermediate/ca-int2-cert.der ./certs/intermediate/ca-int-cert.der > ./certs/intermediate/server-chain.der

openssl x509 -in ./certs/intermediate/client-int-cert.pem  > ./certs/intermediate/client-chain.pem
openssl x509 -in ./certs/intermediate/ca-int2-cert.pem    >> ./certs/intermediate/client-chain.pem
openssl x509 -in ./certs/intermediate/ca-int-cert.pem     >> ./certs/intermediate/client-chain.pem
cat ./certs/intermediate/client-int-cert.der ./certs/intermediate/ca-int2-cert.der ./certs/intermediate/ca-int-cert.der > ./certs/intermediate/client-chain.der

echo "Assemble cert chain with extra cert for testing alternate chains"
cp ./certs/intermediate/server-chain.pem ./certs/intermediate/server-chain-alt.pem
cp ./certs/intermediate/client-chain.pem ./certs/intermediate/client-chain-alt.pem
openssl x509 -in ./certs/external/ca-google-root.pem      >> ./certs/intermediate/server-chain-alt.pem
openssl x509 -in ./certs/external/ca-google-root.pem      >> ./certs/intermediate/client-chain-alt.pem


# ECC
echo "Creating ECC CA configuration cnf files"
create_ca_config ./certs/intermediate/wolfssl_root_ecc.cnf certs/ca-ecc-key.pem certs/ca-ecc-cert.pem
create_ca_config ./certs/intermediate/wolfssl_int_ecc.cnf certs/intermediate/ca-int-ecc-key.pem certs/intermediate/ca-int-ecc-cert.pem
create_ca_config ./certs/intermediate/wolfssl_int2_ecc.cnf certs/intermediate/ca-int2-ecc-key.pem certs/intermediate/ca-int2-ecc-cert.pem

if [ ! -f ./certs/intermediate/ca-int-ecc-key.pem ]; then
    echo "Make Intermediate ECC CA Key"
    openssl ecparam -name prime256v1 -genkey -noout -out ./certs/intermediate/ca-int-ecc-key.pem
    check_result $?
    openssl ec -in ./certs/intermediate/ca-int-ecc-key.pem -inform PEM -out ./certs/intermediate/ca-int-ecc-key.der -outform DER
    check_result $?
fi
if [ ! -f ./certs/intermediate/ca-int2-ecc-key.pem ]; then
    echo "Make Intermediate2 ECC CA Key"
    openssl ecparam -name prime256v1 -genkey -noout -out ./certs/intermediate/ca-int2-ecc-key.pem
    check_result $?
    openssl ec -in ./certs/intermediate/ca-int2-ecc-key.pem -inform PEM -out ./certs/intermediate/ca-int2-ecc-key.der -outform DER
    check_result $?
fi

echo "Create ECC Intermediate CA signed by root"
create_cert wolfssl_int_ecc wolfssl_root_ecc ./certs/intermediate/ca-int-ecc-key.pem ca-int-ecc-cert v3_intermediate_ca "wolfSSL Intermediate CA ECC" 7300

echo "Create ECC Intermediate2 CA signed by Intermediate"
create_cert wolfssl_int2_ecc wolfssl_int_ecc ./certs/intermediate/ca-int2-ecc-key.pem ca-int2-ecc-cert v3_intermediate2_ca "wolfSSL Intermediate2 CA ECC" 7300

echo "Create ECC Server Certificate signed by intermediate2"
create_cert wolfssl_int2_ecc wolfssl_int2_ecc ./certs/ecc-key.pem server-int-ecc-cert server_cert "wolfSSL Server Chain ECC" 3650

echo "Create ECC Client Certificate signed by intermediate2"
create_cert wolfssl_int2_ecc wolfssl_int2_ecc ./certs/ecc-client-key.pem client-int-ecc-cert usr_cert "wolfSSL Client Chain ECC" 3650

echo "Generate CRLs for new certificates"
openssl ca -config ./certs/intermediate/wolfssl_root_ecc.cnf -gencrl -crldays 1000 -out ./certs/crl/ca-int-ecc.pem -keyfile ./certs/intermediate/ca-int-ecc-key.pem -cert ./certs/intermediate/ca-int-ecc-cert.pem
check_result $?
openssl ca -config ./certs/intermediate/wolfssl_int_ecc.cnf -gencrl -crldays 1000 -out ./certs/crl/ca-int2-ecc.pem -keyfile ./certs/intermediate/ca-int2-ecc-key.pem -cert ./certs/intermediate/ca-int2-ecc-cert.pem
check_result $?
openssl ca -config ./certs/intermediate/wolfssl_int2_ecc.cnf -gencrl -crldays 1000 -out ./certs/crl/server-int-ecc.pem -keyfile ./certs/ecc-key.pem -cert ./certs/intermediate/server-int-ecc-cert.pem
check_result $?
openssl ca -config ./certs/intermediate/wolfssl_int2_ecc.cnf -gencrl -crldays 1000 -out ./certs/crl/client-int-ecc.pem -keyfile ./certs/ecc-client-key.pem -cert ./certs/intermediate/client-int-ecc-cert.pem
check_result $?

echo "Assemble test chains - peer first, then intermediate2, then intermediate"
openssl x509 -in ./certs/intermediate/server-int-ecc-cert.pem  > ./certs/intermediate/server-chain-ecc.pem
openssl x509 -in ./certs/intermediate/ca-int2-ecc-cert.pem    >> ./certs/intermediate/server-chain-ecc.pem
openssl x509 -in ./certs/intermediate/ca-int-ecc-cert.pem     >> ./certs/intermediate/server-chain-ecc.pem
cat ./certs/intermediate/server-int-ecc-cert.der ./certs/intermediate/ca-int2-ecc-cert.der ./certs/intermediate/ca-int-ecc-cert.der > ./certs/intermediate/server-chain-ecc.der

openssl x509 -in ./certs/intermediate/client-int-ecc-cert.pem  > ./certs/intermediate/client-chain-ecc.pem
openssl x509 -in ./certs/intermediate/ca-int2-ecc-cert.pem    >> ./certs/intermediate/client-chain-ecc.pem
openssl x509 -in ./certs/intermediate/ca-int-ecc-cert.pem     >> ./certs/intermediate/client-chain-ecc.pem
cat ./certs/intermediate/client-int-ecc-cert.der ./certs/intermediate/ca-int2-ecc-cert.der ./certs/intermediate/ca-int-ecc-cert.der > ./certs/intermediate/client-chain-ecc.der

echo "Assemble cert chain with extra untrusted cert for testing alternate chains"
cp ./certs/intermediate/server-chain-ecc.pem ./certs/intermediate/server-chain-alt-ecc.pem
cp ./certs/intermediate/client-chain-ecc.pem ./certs/intermediate/client-chain-alt-ecc.pem
openssl x509 -in ./certs/external/ca-google-root.pem          >> ./certs/intermediate/server-chain-alt-ecc.pem
openssl x509 -in ./certs/external/ca-google-root.pem          >> ./certs/intermediate/client-chain-alt-ecc.pem
