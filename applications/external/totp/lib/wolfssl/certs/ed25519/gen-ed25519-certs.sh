#!/bin/bash

check_result(){
    if [ $1 -ne 0 ]; then
        echo "Failed at \"$2\", Abort"
        exit 1
    else
        echo "Step Succeeded!"
    fi
}

openssl pkey -in root-ed25519-priv.pem -noout >/dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "OpenSSL does not support Ed25519"
    echo "Skipping Ed25519 certificate renewal"
    exit 0
fi

############################################################
###### update the self-signed root-ed25519.pem #############
############################################################
echo "Updating root-ed25519.pem"
echo ""
#pipe the following arguments to openssl req...
echo -e "US\\nMontana\\nBozeman\\nwolfSSL_Ed25519\\nRoot-Ed25519\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n.\\n.\\n" | \
openssl req -new -key root-ed25519-priv.pem -config ../renewcerts/wolfssl.cnf -nodes -out root-ed25519.csr
check_result $? "Generate request"

openssl x509 -req -in root-ed25519.csr -days 1000 -extfile ../renewcerts/wolfssl.cnf -extensions ca_ecc_cert -signkey root-ed25519-priv.pem -out root-ed25519.pem
check_result $? "Generate certificate"
rm root-ed25519.csr

openssl x509 -in root-ed25519.pem -outform DER > root-ed25519.der
check_result $? "Convert to DER"
openssl x509 -in root-ed25519.pem -text > tmp.pem
check_result $? "Add text"
mv tmp.pem root-ed25519.pem
echo "End of section"
echo "---------------------------------------------------------------------"

############################################################
###### update ca-ed25519.pem signed by root ################
############################################################
echo "Updating ca-ed25519.pem"
echo ""
#pipe the following arguments to openssl req...
echo -e "US\\nMontana\\nBozeman\\nwolfSSL_ed25519\\nCA-ed25519\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n\\n\\n\\n" | openssl req -new -key ca-ed25519-priv.pem -config ../renewcerts/wolfssl.cnf -nodes -out ca-ed25519.csr
check_result $? "Generate request"

openssl x509 -req -in ca-ed25519.csr -days 1000 -extfile ../renewcerts/wolfssl.cnf -extensions ca_ecc_cert -CA root-ed25519.pem -CAkey root-ed25519-priv.pem -set_serial 01 -out ca-ed25519.pem
check_result $? "Generate certificate"
rm ca-ed25519.csr

openssl x509 -in ca-ed25519.pem -outform DER > ca-ed25519.der
check_result $? "Convert to DER"
openssl x509 -in ca-ed25519.pem -text > tmp.pem
check_result $? "Add text"
mv tmp.pem ca-ed25519.pem
echo "End of section"
echo "---------------------------------------------------------------------"

############################################################
###### update server-ed25519.pem signed by ca ##############
############################################################
echo "Updating server-ed25519.pem"
echo ""
#pipe the following arguments to openssl req...
echo -e "US\\nMontana\\nBozeman\\nwolfSSL_ed25519\\nServer-ed25519\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n\\n\\n\\n" | openssl req -new -key server-ed25519-priv.pem -config ../renewcerts/wolfssl.cnf -nodes -out server-ed25519.csr
check_result $? "Generate request"

openssl x509 -req -in server-ed25519.csr -days 1000 -extfile ../renewcerts/wolfssl.cnf -extensions server_ecc -CA ca-ed25519.pem -CAkey ca-ed25519-priv.pem -set_serial 01 -out server-ed25519-cert.pem
check_result $? "Generate certificate"
rm server-ed25519.csr

openssl x509 -in server-ed25519-cert.pem -outform DER > server-ed25519.der
check_result $? "Convert to DER"
openssl x509 -in server-ed25519-cert.pem -text > tmp.pem
check_result $? "Add text"
mv tmp.pem server-ed25519-cert.pem
cat server-ed25519-cert.pem ca-ed25519.pem > server-ed25519.pem
check_result $? "Add CA into server cert"
echo "End of section"
echo "---------------------------------------------------------------------"

############################################################
###### update the self-signed client-ed25519.pem ###########
############################################################
echo "Updating client-ed25519.pem"
echo ""
#pipe the following arguments to openssl req...
echo -e "US\\nMontana\\nBozeman\\nwolfSSL_ed25519\\nClient-ed25519\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n\\n\\n\\n" | openssl req -new -key client-ed25519-priv.pem -config ../renewcerts/wolfssl.cnf -nodes -out client-ed25519.csr
check_result $? "Generate request"

openssl x509 -req -in client-ed25519.csr -days 1000 -extfile ../renewcerts/wolfssl.cnf -extensions wolfssl_opts -signkey client-ed25519-priv.pem -out client-ed25519.pem
check_result $? "Generate certificate"
rm client-ed25519.csr

openssl x509 -in client-ed25519.pem -outform DER > client-ed25519.der
check_result $? "Convert to DER"
openssl x509 -in client-ed25519.pem -text > tmp.pem
check_result $? "Add text"
mv tmp.pem client-ed25519.pem
echo "End of section"
echo "---------------------------------------------------------------------"

