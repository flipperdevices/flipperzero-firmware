#!/bin/bash

# gencrls, crl config already done, see taoCerts.txt for setup
check_result(){
    if [ $1 -ne 0 ]; then
        echo "Step failed, Abort"
        exit 1
    else
        echo "Step Succeeded!"
    fi
}

setup_files() {
    #set up the file system for updating the crls
    echo "setting up the file system for generating the crls..."
    echo ""
    mkdir demoCA || exit 1
    touch ./demoCA/index.txt || exit 1
    touch ./index.txt || exit 1
    touch ../crl/index.txt || exit 1
    touch ./crlnumber || exit 1
    touch ../crl/crlnumber || exit 1
    echo "01" >> crlnumber || exit 1
    echo "01" >> ../crl/crlnumber || exit 1
    touch ./blank.index.txt || exit 1
    touch ./demoCA/index.txt.attr || exit 1
    touch ../crl/index.txt.attr || exit 1
}

cleanup_files() {
    rm blank.index.txt || exit 1
    rm index.* || exit 1
    rm crlnumber* || exit 1
    rm -rf demoCA || exit 1
    echo "Removed ../wolfssl.cnf, blank.index.txt, index.*, crlnumber*, demoCA/"
    echo "        ../crl/index.txt"
    echo ""
    exit 0
}
trap cleanup_files EXIT

#setup the files
setup_files

# caCrl
# revoke server-revoked-cert.pem
echo "Step 1"
openssl ca -config ../renewcerts/wolfssl.cnf -gencrl -crldays 1000 -out crl2.pem -keyfile ../client-key.pem -cert ../client-cert.pem
check_result $?

echo "Step 2"
openssl ca -config ../renewcerts/wolfssl.cnf -revoke ../server-revoked-cert.pem -keyfile ../ca-key.pem -cert ../ca-cert.pem
check_result $?

echo "Step 3"
openssl ca -config ../renewcerts/wolfssl.cnf -gencrl -crldays 1000 -out crl.pem -keyfile ../ca-key.pem -cert ../ca-cert.pem
check_result $?

# metadata
echo "Step 4"
openssl crl -in crl.pem -text > tmp
check_result $?
mv tmp crl.pem
# install (only needed if working outside wolfssl)
#cp crl.pem ~/wolfssl/certs/crl/crl.pem

# crl2 create
echo "Step 5"
openssl crl -in crl.pem -text > tmp
check_result $?
echo "Step 6"
openssl crl -in crl2.pem -text >> tmp
check_result $?
mv tmp crl2.pem

# caCrl server revoked
echo "Step 7"
openssl ca -config ../renewcerts/wolfssl.cnf -revoke ../server-cert.pem -keyfile ../ca-key.pem -cert ../ca-cert.pem
check_result $?

# caCrl server revoked generation
echo "Step 8"
openssl ca -config ../renewcerts/wolfssl.cnf -gencrl -crldays 1000 -out crl.revoked -keyfile ../ca-key.pem -cert ../ca-cert.pem
check_result $?

# metadata
echo "Step 9"
openssl crl -in crl.revoked -text > tmp
check_result $?
mv tmp crl.revoked
# install (only needed if working outside wolfssl)
#cp crl.revoked ~/wolfssl/certs/crl/crl.revoked


# remove revoked so next time through the normal CA won't have server revoked
cp blank.index.txt demoCA/index.txt

# caEccCrl
echo "Step 10"
openssl ca -config ../renewcerts/wolfssl.cnf -revoke ../server-revoked-cert.pem -keyfile ../ca-ecc-key.pem -cert ../ca-ecc-cert.pem
check_result $?

echo "Step 11"
openssl ca -config ../renewcerts/wolfssl.cnf -gencrl -crldays 1000 -out caEccCrl.pem -keyfile ../ca-ecc-key.pem -cert ../ca-ecc-cert.pem
check_result $?

# metadata
echo "Step 12"
openssl crl -in caEccCrl.pem -text > tmp
check_result $?
mv tmp caEccCrl.pem
# install (only needed if working outside wolfssl)
#cp caEccCrl.pem ~/wolfssl/certs/crl/caEccCrl.pem

# caEcc384Crl
# server-revoked-cert.pem is already revoked in Step 10
#openssl ca -config ../renewcerts/wolfssl.cnf -revoke ../server-revoked-cert.pem -keyfile ../ca-ecc384-key.pem -cert ../ca-ecc384-cert.pem

echo "Step 13"
openssl ca -config ../renewcerts/wolfssl.cnf -gencrl -crldays 1000 -out caEcc384Crl.pem -keyfile ../ca-ecc384-key.pem -cert ../ca-ecc384-cert.pem
check_result $?

# metadata
echo "Step 14"
openssl crl -in caEcc384Crl.pem -text > tmp
check_result $?
mv tmp caEcc384Crl.pem
# install (only needed if working outside wolfssl)
#cp caEcc384Crl.pem ~/wolfssl/certs/crl/caEcc384Crl.pem

# cliCrl
echo "Step 15"
openssl ca -config ../renewcerts/wolfssl.cnf -gencrl -crldays 1000 -out cliCrl.pem -keyfile ../client-key.pem -cert ../client-cert.pem
check_result $?

# metadata
echo "Step 16"
openssl crl -in cliCrl.pem -text > tmp
check_result $?
mv tmp cliCrl.pem
# install (only needed if working outside wolfssl)
#cp cliCrl.pem ~/wolfssl/certs/crl/cliCrl.pem

# eccCliCRL
echo "Step 17"
openssl ca -config ../renewcerts/wolfssl.cnf -gencrl -crldays 1000 -out eccCliCRL.pem -keyfile ../ecc-client-key.pem -cert ../client-ecc-cert.pem
check_result $?

# metadata
echo "Step 18"
openssl crl -in eccCliCRL.pem -text > tmp
check_result $?
mv tmp eccCliCRL.pem
# install (only needed if working outside wolfssl)
#cp eccCliCRL.pem ~/wolfssl/certs/crl/eccCliCRL.pem

# eccSrvCRL
echo "Step 19"
openssl ca -config ../renewcerts/wolfssl.cnf -gencrl -crldays 1000 -out eccSrvCRL.pem -keyfile ../ecc-key.pem -cert ../server-ecc.pem
check_result $?

# metadata
echo "Step 20"
openssl crl -in eccSrvCRL.pem -text > tmp
check_result $?
mv tmp eccSrvCRL.pem
# install (only needed if working outside wolfssl)
#cp eccSrvCRL.pem ~/wolfssl/certs/crl/eccSrvCRL.pem

# caEccCrl
echo "Step 21"
openssl ca -config ./wolfssl.cnf -gencrl -crldays 1000 -out caEccCrl.pem -keyfile ../ca-ecc-key.pem -cert ../ca-ecc-cert.pem
check_result $?

# ca-ecc384-cert
echo "Step 22"
openssl ca -config ./wolfssl.cnf -gencrl -crldays 1000 -out caEcc384Crl.pem -keyfile ../ca-ecc384-key.pem -cert ../ca-ecc384-cert.pem
check_result $?

# create crl and crl2 der files for unit test
echo "Step 23"
openssl crl -in crl.pem -inform PEM -out crl.der -outform DER
openssl crl -in crl2.pem -inform PEM -out crl2.der -outform DER

exit 0
