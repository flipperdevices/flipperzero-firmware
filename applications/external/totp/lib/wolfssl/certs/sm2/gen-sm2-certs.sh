#!/bin/bash

check_result(){
    if [ $1 -ne 0 ]; then
        echo "Failed at \"$2\", Abort"
        exit 1
    else
        echo "Step Succeeded!"
    fi
}

openssl pkey -in root-sm2-priv.pem -noout >/dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "OpenSSL does not support SM2"
    echo "Skipping SM2 certificate renewal"
    exit 0
fi

############################################################
###### update the self-signed root-sm2.pem #############
############################################################
echo "Updating root-sm2.pem"
echo ""
#pipe the following arguments to openssl req...
echo -e "US\\nMontana\\nBozeman\\nwolfSSL_SM2\\nRoot-SM2\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n.\\n.\\n" | \
openssl req -new -key root-sm2-priv.pem -config ../renewcerts/wolfssl.cnf -nodes -out root-sm2.csr
check_result $? "Generate request"

openssl x509 -req -in root-sm2.csr -days 1000 -extfile ../renewcerts/wolfssl.cnf -extensions ca_ecc_cert -signkey root-sm2-priv.pem -out root-sm2.pem
check_result $? "Generate certificate"
rm root-sm2.csr

openssl x509 -in root-sm2.pem -outform DER > root-sm2.der
check_result $? "Convert to DER"
openssl x509 -in root-sm2.pem -text > tmp.pem
check_result $? "Add text"
mv tmp.pem root-sm2.pem
echo "End of section"
echo "---------------------------------------------------------------------"

############################################################
###### update ca-sm2.pem signed by root ################
############################################################
echo "Updating ca-sm2.pem"
echo ""
#pipe the following arguments to openssl req...
echo -e "US\\nMontana\\nBozeman\\nwolfSSL_sm2\\nCA-sm2\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n\\n\\n\\n" | openssl req -new -key ca-sm2-priv.pem -config ../renewcerts/wolfssl.cnf -nodes -out ca-sm2.csr
check_result $? "Generate request"

openssl x509 -req -in ca-sm2.csr -days 1000 -extfile ../renewcerts/wolfssl.cnf -extensions ca_ecc_cert -CA root-sm2.pem -CAkey root-sm2-priv.pem -set_serial 01 -out ca-sm2.pem
check_result $? "Generate certificate"
rm ca-sm2.csr

openssl x509 -in ca-sm2.pem -outform DER > ca-sm2.der
check_result $? "Convert to DER"
openssl x509 -in ca-sm2.pem -text > tmp.pem
check_result $? "Add text"
mv tmp.pem ca-sm2.pem
echo "End of section"
echo "---------------------------------------------------------------------"

############################################################
###### update server-sm2.pem signed by ca ##############
############################################################
echo "Updating server-sm2.pem"
echo ""
#pipe the following arguments to openssl req...
echo -e "US\\nMontana\\nBozeman\\nwolfSSL_sm2\\nServer-sm2\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n\\n\\n\\n" | openssl req -new -key server-sm2-priv.pem -config ../renewcerts/wolfssl.cnf -nodes -out server-sm2.csr
check_result $? "Generate request"

openssl x509 -req -in server-sm2.csr -days 1000 -extfile ../renewcerts/wolfssl.cnf -extensions server_ecc -CA ca-sm2.pem -CAkey ca-sm2-priv.pem -set_serial 01 -out server-sm2-cert.pem
check_result $? "Generate certificate"
rm server-sm2.csr

openssl x509 -in server-sm2-cert.pem -outform DER > server-sm2.der
check_result $? "Convert to DER"
openssl x509 -in server-sm2-cert.pem -text > tmp.pem
check_result $? "Add text"
mv tmp.pem server-sm2-cert.pem
cat server-sm2-cert.pem ca-sm2.pem > server-sm2.pem
check_result $? "Add CA into server cert"
echo "End of section"
echo "---------------------------------------------------------------------"

############################################################
###### update the self-signed client-sm2.pem ###########
############################################################
echo "Updating client-sm2.pem"
echo ""
#pipe the following arguments to openssl req...
echo -e "US\\nMontana\\nBozeman\\nwolfSSL_sm2\\nClient-sm2\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n\\n\\n\\n" | openssl req -new -key client-sm2-priv.pem -config ../renewcerts/wolfssl.cnf -nodes -out client-sm2.csr
check_result $? "Generate request"

openssl x509 -req -in client-sm2.csr -days 1000 -extfile ../renewcerts/wolfssl.cnf -extensions wolfssl_opts -signkey client-sm2-priv.pem -out client-sm2.pem
check_result $? "Generate certificate"
rm client-sm2.csr

openssl x509 -in client-sm2.pem -outform DER > client-sm2.der
check_result $? "Convert to DER"
openssl x509 -in client-sm2.pem -text > tmp.pem
check_result $? "Add text"
mv tmp.pem client-sm2.pem
echo "End of section"
echo "---------------------------------------------------------------------"

