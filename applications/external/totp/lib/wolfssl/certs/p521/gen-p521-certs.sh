#!/bin/bash

check_result(){
    if [ $1 -ne 0 ]; then
        echo "Failed at \"$2\", Abort"
        exit 1
    else
        echo "Step Succeeded!"
    fi
}

openssl pkey -in root-p521-priv.pem -noout >/dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "OpenSSL does not support P521"
    echo "Skipping P521 certificate renewal"
    exit 0
fi

############################################################
###### update the self-signed root-p521.pem ###############
############################################################
echo "Updating root-p521.pem"
echo ""
#pipe the following arguments to openssl req...
echo -e "US\\nMontana\\nBozeman\\nwolfSSL_P521\\nRoot-P521\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n.\\n.\\n" | \
openssl req -new -key root-p521-priv.pem -config ../renewcerts/wolfssl.cnf -nodes -out root-p521.csr
check_result $? "Generate request"

openssl x509 -req -in root-p521.csr -days 1000 -extfile ../renewcerts/wolfssl.cnf -extensions ca_ecc_cert -signkey root-p521-priv.pem -out root-p521.pem
check_result $? "Generate certificate"
rm root-p521.csr

openssl x509 -in root-p521.pem -outform DER > root-p521.der
check_result $? "Convert to DER"
openssl x509 -in root-p521.pem -text > tmp.pem
check_result $? "Add text"
mv tmp.pem root-p521.pem
echo "End of section"
echo "---------------------------------------------------------------------"

############################################################
###### update ca-p521.pem signed by root ##################
############################################################
echo "Updating ca-p521.pem"
echo ""
#pipe the following arguments to openssl req...
echo -e "US\\nMontana\\nBozeman\\nwolfSSL_p521\\nCA-p521\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n\\n\\n\\n" | openssl req -new -key ca-p521-priv.pem -config ../renewcerts/wolfssl.cnf -nodes -out ca-p521.csr
check_result $? "Generate request"

openssl x509 -req -in ca-p521.csr -days 1000 -extfile ../renewcerts/wolfssl.cnf -extensions ca_ecc_cert -CA root-p521.pem -CAkey root-p521-priv.pem -set_serial 01 -out ca-p521.pem
check_result $? "Generate certificate"
rm ca-p521.csr

openssl x509 -in ca-p521.pem -outform DER > ca-p521.der
check_result $? "Convert to DER"
openssl x509 -in ca-p521.pem -text > tmp.pem
check_result $? "Add text"
mv tmp.pem ca-p521.pem
echo "End of section"
echo "---------------------------------------------------------------------"

############################################################
###### update server-p521.pem signed by ca ################
############################################################
echo "Updating server-p521.pem"
echo ""
#pipe the following arguments to openssl req...
echo -e "US\\nMontana\\nBozeman\\nwolfSSL_p521\\nServer-p521\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n\\n\\n\\n" | openssl req -new -key server-p521-priv.pem -config ../renewcerts/wolfssl.cnf -nodes -out server-p521.csr
check_result $? "Generate request"

openssl x509 -req -in server-p521.csr -days 1000 -extfile ../renewcerts/wolfssl.cnf -extensions server_ecc -CA ca-p521.pem -CAkey ca-p521-priv.pem -set_serial 01 -out server-p521-cert.pem
check_result $? "Generate certificate"
rm server-p521.csr

openssl x509 -in server-p521-cert.pem -outform DER > server-p521.der
check_result $? "Convert to DER"
openssl x509 -in server-p521-cert.pem -text > tmp.pem
check_result $? "Add text"
mv tmp.pem server-p521-cert.pem
cat server-p521-cert.pem ca-p521.pem > server-p521.pem
check_result $? "Add CA into server cert"
echo "End of section"
echo "---------------------------------------------------------------------"

############################################################
###### update the self-signed client-p521.pem #############
############################################################
echo "Updating client-p521.pem"
echo ""
#pipe the following arguments to openssl req...
echo -e "US\\nMontana\\nBozeman\\nwolfSSL_p521\\nClient-p521\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n\\n\\n\\n" | openssl req -new -key client-p521-priv.pem -config ../renewcerts/wolfssl.cnf -nodes -out client-p521.csr
check_result $? "Generate request"

openssl x509 -req -in client-p521.csr -days 1000 -extfile ../renewcerts/wolfssl.cnf -extensions wolfssl_opts -signkey client-p521-priv.pem -out client-p521.pem
check_result $? "Generate certificate"
rm client-p521.csr

openssl x509 -in client-p521.pem -outform DER > client-p521.der
check_result $? "Convert to DER"
openssl x509 -in client-p521.pem -text > tmp.pem
check_result $? "Add text"
mv tmp.pem client-p521.pem
echo "End of section"
echo "---------------------------------------------------------------------"

