#!/bin/bash

check_result(){
    if [ $1 -ne 0 ]; then
        echo "Failed at \"$2\", Abort"
        exit 1
    else
        echo "Step Succeeded!"
    fi
}

openssl pkey -in root-ed448-priv.pem -noout >/dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "OpenSSL does not support Ed448"
    echo "Skipping Ed448 certificate renewal"
    exit 0
fi

############################################################
###### update the self-signed root-ed448.pem ###############
############################################################
echo "Updating root-ed448.pem"
echo ""
#pipe the following arguments to openssl req...
echo -e "US\\nMontana\\nBozeman\\nwolfSSL_Ed448\\nRoot-Ed448\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n.\\n.\\n" | \
openssl req -new -key root-ed448-priv.pem -config ../renewcerts/wolfssl.cnf -nodes -out root-ed448.csr
check_result $? "Generate request"

openssl x509 -req -in root-ed448.csr -days 1000 -extfile ../renewcerts/wolfssl.cnf -extensions ca_ecc_cert -signkey root-ed448-priv.pem -out root-ed448.pem
check_result $? "Generate certificate"
rm root-ed448.csr

openssl x509 -in root-ed448.pem -outform DER > root-ed448.der
check_result $? "Convert to DER"
openssl x509 -in root-ed448.pem -text > tmp.pem
check_result $? "Add text"
mv tmp.pem root-ed448.pem
echo "End of section"
echo "---------------------------------------------------------------------"

############################################################
###### update ca-ed448.pem signed by root ##################
############################################################
echo "Updating ca-ed448.pem"
echo ""
#pipe the following arguments to openssl req...
echo -e "US\\nMontana\\nBozeman\\nwolfSSL_ed448\\nCA-ed448\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n\\n\\n\\n" | openssl req -new -key ca-ed448-priv.pem -config ../renewcerts/wolfssl.cnf -nodes -out ca-ed448.csr
check_result $? "Generate request"

openssl x509 -req -in ca-ed448.csr -days 1000 -extfile ../renewcerts/wolfssl.cnf -extensions ca_ecc_cert -CA root-ed448.pem -CAkey root-ed448-priv.pem -set_serial 01 -out ca-ed448.pem
check_result $? "Generate certificate"
rm ca-ed448.csr

openssl x509 -in ca-ed448.pem -outform DER > ca-ed448.der
check_result $? "Convert to DER"
openssl x509 -in ca-ed448.pem -text > tmp.pem
check_result $? "Add text"
mv tmp.pem ca-ed448.pem
echo "End of section"
echo "---------------------------------------------------------------------"

############################################################
###### update server-ed448.pem signed by ca ################
############################################################
echo "Updating server-ed448.pem"
echo ""
#pipe the following arguments to openssl req...
echo -e "US\\nMontana\\nBozeman\\nwolfSSL_ed448\\nServer-ed448\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n\\n\\n\\n" | openssl req -new -key server-ed448-priv.pem -config ../renewcerts/wolfssl.cnf -nodes -out server-ed448.csr
check_result $? "Generate request"

openssl x509 -req -in server-ed448.csr -days 1000 -extfile ../renewcerts/wolfssl.cnf -extensions server_ecc -CA ca-ed448.pem -CAkey ca-ed448-priv.pem -set_serial 01 -out server-ed448-cert.pem
check_result $? "Generate certificate"
rm server-ed448.csr

openssl x509 -in server-ed448-cert.pem -outform DER > server-ed448.der
check_result $? "Convert to DER"
openssl x509 -in server-ed448-cert.pem -text > tmp.pem
check_result $? "Add text"
mv tmp.pem server-ed448-cert.pem
cat server-ed448-cert.pem ca-ed448.pem > server-ed448.pem
check_result $? "Add CA into server cert"
echo "End of section"
echo "---------------------------------------------------------------------"

############################################################
###### update the self-signed client-ed448.pem #############
############################################################
echo "Updating client-ed448.pem"
echo ""
#pipe the following arguments to openssl req...
echo -e "US\\nMontana\\nBozeman\\nwolfSSL_ed448\\nClient-ed448\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n\\n\\n\\n" | openssl req -new -key client-ed448-priv.pem -config ../renewcerts/wolfssl.cnf -nodes -out client-ed448.csr
check_result $? "Generate request"

openssl x509 -req -in client-ed448.csr -days 1000 -extfile ../renewcerts/wolfssl.cnf -extensions wolfssl_opts -signkey client-ed448-priv.pem -out client-ed448.pem
check_result $? "Generate certificate"
rm client-ed448.csr

openssl x509 -in client-ed448.pem -outform DER > client-ed448.der
check_result $? "Convert to DER"
openssl x509 -in client-ed448.pem -text > tmp.pem
check_result $? "Add text"
mv tmp.pem client-ed448.pem
echo "End of section"
echo "---------------------------------------------------------------------"

