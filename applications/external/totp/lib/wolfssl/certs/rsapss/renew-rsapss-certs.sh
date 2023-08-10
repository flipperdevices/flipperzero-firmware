#!/bin/bash

check_result(){
    if [ $1 -ne 0 ]; then
        echo "Failed at \"$2\", Abort"
        exit 1
    else
        echo "Step Succeeded!"
    fi
}

############################################################
####### update the self-signed root-rsapss.pem #############
############################################################
echo "Updating root-rsapss.pem"
echo ""
#pipe the following arguments to openssl req...
echo -e "US\\nMontana\\nBozeman\\nwolfSSL_RSA-PSS\\nRoot-RSA-PSS\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n.\\n.\\n" | \
openssl req -new -key root-rsapss-priv.pem -config ../renewcerts/wolfssl.cnf -nodes -out root-rsapss.csr
check_result $? "Generate request"

openssl x509 -req -in root-rsapss.csr -days 1000 -extfile ../renewcerts/wolfssl.cnf -extensions ca_ecc_cert -signkey root-rsapss-priv.pem -out root-rsapss.pem
check_result $? "Generate certificate"
rm root-rsapss.csr

openssl x509 -in root-rsapss.pem -outform DER > root-rsapss.der
check_result $? "Convert to DER"
openssl x509 -in root-rsapss.pem -text > tmp.pem
check_result $? "Add text"
mv tmp.pem root-rsapss.pem
echo "End of section"
echo "---------------------------------------------------------------------"

############################################################
####### update ca-rsapss.pem signed by root ################
############################################################
echo "Updating ca-rsapss.pem"
echo ""
#pipe the following arguments to openssl req...
echo -e "US\\nMontana\\nBozeman\\nwolfSSL_RSAPSS\\nCA-RSAPSS\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n\\n\\n\\n" | openssl req -new -key ca-rsapss-priv.pem -config ../renewcerts/wolfssl.cnf -nodes -out ca-rsapss.csr
check_result $? "Generate request"

openssl x509 -req -in ca-rsapss.csr -days 1000 -extfile ../renewcerts/wolfssl.cnf -extensions ca_ecc_cert -CA root-rsapss.pem -CAkey root-rsapss-priv.pem -set_serial 01 -out ca-rsapss.pem
check_result $? "Generate certificate"
rm ca-rsapss.csr

openssl x509 -in ca-rsapss.pem -outform DER > ca-rsapss.der
check_result $? "Convert to DER"
openssl x509 -in ca-rsapss.pem -text > tmp.pem
check_result $? "Add text"
mv tmp.pem ca-rsapss.pem
echo "End of section"
echo "---------------------------------------------------------------------"

############################################################
####### update server-mix-rsapss.pem signed by ca ##########
############################################################
echo "Updating server-mix-rsapss.pem"
echo ""
#pipe the following arguments to openssl req...
echo -e "US\\nMontana\\nBozeman\\nwolfSSL_RSAPSS\\nServer-MIX-RSAPSS\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n\\n\\n\\n" | openssl req -new -key ../server-key.pem -config ../renewcerts/wolfssl.cnf -nodes -out server-mix-rsapss.csr
check_result $? "Generate request"

openssl x509 -req -in server-mix-rsapss.csr -days 1000 -extfile ../renewcerts/wolfssl.cnf -extensions server_ecc -CA ../ca-cert.pem -CAkey ../ca-key.pem -sigopt rsa_padding_mode:pss -set_serial 01 -out server-mix-rsapss-cert.pem
check_result $? "Generate certificate"
rm server-mix-rsapss.csr

openssl x509 -in server-mix-rsapss-cert.pem -text > tmp.pem
check_result $? "Add text"
mv tmp.pem server-mix-rsapss-cert.pem
echo "End of section"
echo "---------------------------------------------------------------------"

############################################################
####### update server-rsapss.pem signed by ca ##############
############################################################
echo "Updating server-rsapss.pem"
echo ""
#pipe the following arguments to openssl req...
echo -e "US\\nMontana\\nBozeman\\nwolfSSL_RSAPSS\\nServer-RSAPSS\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n\\n\\n\\n" | openssl req -new -key server-rsapss-priv.pem -config ../renewcerts/wolfssl.cnf -nodes -out server-rsapss.csr
check_result $? "Generate request"

openssl x509 -req -in server-rsapss.csr -days 1000 -extfile ../renewcerts/wolfssl.cnf -extensions server_ecc -CA ca-rsapss.pem -CAkey ca-rsapss-priv.pem -set_serial 01 -out server-rsapss-cert.pem
check_result $? "Generate certificate"
rm server-rsapss.csr

openssl x509 -in server-rsapss-cert.pem -outform DER > server-rsapss.der
check_result $? "Convert to DER"
openssl x509 -in server-rsapss-cert.pem -text > tmp.pem
check_result $? "Add text"
mv tmp.pem server-rsapss-cert.pem
cat server-rsapss-cert.pem ca-rsapss.pem > server-rsapss.pem
check_result $? "Add CA into server cert"
echo "End of section"
echo "---------------------------------------------------------------------"

############################################################
####### update the self-signed client-rsapss.pem ###########
############################################################
echo "Updating client-rsapss.pem"
echo ""
#pipe the following arguments to openssl req...
echo -e "US\\nMontana\\nBozeman\\nwolfSSL_RSAPSS\\nClient-RSAPSS\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n\\n\\n\\n" | openssl req -new -key client-rsapss-priv.pem -config ../renewcerts/wolfssl.cnf -nodes -out client-rsapss.csr
check_result $? "Generate request"

openssl x509 -req -in client-rsapss.csr -days 1000 -extfile ../renewcerts/wolfssl.cnf -extensions wolfssl_opts -signkey client-rsapss-priv.pem -out client-rsapss.pem
check_result $? "Generate certificate"
rm client-rsapss.csr

openssl x509 -in client-rsapss.pem -outform DER > client-rsapss.der
check_result $? "Convert to DER"
openssl x509 -in client-rsapss.pem -text > tmp.pem
check_result $? "Add text"
mv tmp.pem client-rsapss.pem
echo "End of section"
echo "---------------------------------------------------------------------"


################################################################################
# 3072-bit keys. RSA-PSS with SHA-384
################################################################################

############################################################
###### update the self-signed root-3072-rsapss.pem #########
############################################################
echo "Updating root-3072-rsapss.pem"
echo ""
#pipe the following arguments to openssl req...
echo -e "US\\nMontana\\nBozeman\\nwolfSSL_RSA-PSS\\nRoot-RSA-PSS\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n.\\n.\\n" | \
openssl req -new -key root-3072-rsapss-priv.pem -config ../renewcerts/wolfssl.cnf -nodes -out root-3072-rsapss.csr
check_result $? "Generate request"

openssl x509 -req -in root-3072-rsapss.csr -days 1000 -extfile ../renewcerts/wolfssl.cnf -extensions ca_ecc_cert -signkey root-3072-rsapss-priv.pem -sha384 -out root-3072-rsapss.pem
check_result $? "Generate certificate"
rm root-3072-rsapss.csr

openssl x509 -in root-3072-rsapss.pem -outform DER > root-3072-rsapss.der
check_result $? "Convert to DER"
openssl x509 -in root-3072-rsapss.pem -text > tmp.pem
check_result $? "Add text"
mv tmp.pem root-3072-rsapss.pem
echo "End of section"
echo "---------------------------------------------------------------------"

############################################################
###### update ca-3072-rsapss.pem signed by root ############
############################################################
echo "Updating ca-3072-rsapss.pem"
echo ""
#pipe the following arguments to openssl req...
echo -e "US\\nMontana\\nBozeman\\nwolfSSL_RSAPSS\\nCA-RSAPSS\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n\\n\\n\\n" | openssl req -new -key ca-3072-rsapss-priv.pem -config ../renewcerts/wolfssl.cnf -nodes -out ca-3072-rsapss.csr
check_result $? "Generate request"

openssl x509 -req -in ca-3072-rsapss.csr -days 1000 -extfile ../renewcerts/wolfssl.cnf -extensions ca_ecc_cert -CA root-3072-rsapss.pem -CAkey root-3072-rsapss-priv.pem -sha384 -set_serial 01 -out ca-3072-rsapss.pem
check_result $? "Generate certificate"
rm ca-3072-rsapss.csr

openssl x509 -in ca-3072-rsapss.pem -outform DER > ca-3072-rsapss.der
check_result $? "Convert to DER"
openssl x509 -in ca-3072-rsapss.pem -text > tmp.pem
check_result $? "Add text"
mv tmp.pem ca-3072-rsapss.pem
echo "End of section"
echo "---------------------------------------------------------------------"

############################################################
###### update server-3072-rsapss.pem signed by ca ##########
############################################################
echo "Updating server-3072-rsapss.pem"
echo ""
#pipe the following arguments to openssl req...
echo -e "US\\nMontana\\nBozeman\\nwolfSSL_RSAPSS\\nServer-RSAPSS\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n\\n\\n\\n" | openssl req -new -key server-3072-rsapss-priv.pem -config ../renewcerts/wolfssl.cnf -nodes -out server-3072-rsapss.csr
check_result $? "Generate request"

openssl x509 -req -in server-3072-rsapss.csr -days 1000 -extfile ../renewcerts/wolfssl.cnf -extensions server_ecc -CA ca-3072-rsapss.pem -CAkey ca-3072-rsapss-priv.pem -sha384 -set_serial 01 -out server-3072-rsapss-cert.pem
check_result $? "Generate certificate"
rm server-3072-rsapss.csr

openssl x509 -in server-3072-rsapss-cert.pem -outform DER > server-3072-rsapss.der
check_result $? "Convert to DER"
openssl x509 -in server-3072-rsapss-cert.pem -text > tmp.pem
check_result $? "Add text"
mv tmp.pem server-3072-rsapss-cert.pem
cat server-3072-rsapss-cert.pem ca-3072-rsapss.pem > server-3072-rsapss.pem
check_result $? "Add CA into server cert"
echo "End of section"
echo "---------------------------------------------------------------------"

############################################################
###### update the self-signed client-3072-rsapss.pem #######
############################################################
echo "Updating client-3072-rsapss.pem"
echo ""
#pipe the following arguments to openssl req...
echo -e "US\\nMontana\\nBozeman\\nwolfSSL_RSAPSS\\nClient-RSAPSS\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n\\n\\n\\n" | openssl req -new -key client-3072-rsapss-priv.pem -config ../renewcerts/wolfssl.cnf -nodes -out client-3072-rsapss.csr
check_result $? "Generate request"

openssl x509 -req -in client-3072-rsapss.csr -days 1000 -extfile ../renewcerts/wolfssl.cnf -extensions wolfssl_opts -signkey client-3072-rsapss-priv.pem -sha384 -out client-3072-rsapss.pem
check_result $? "Generate certificate"
rm client-3072-rsapss.csr

openssl x509 -in client-3072-rsapss.pem -outform DER > client-3072-rsapss.der
check_result $? "Convert to DER"
openssl x509 -in client-3072-rsapss.pem -text > tmp.pem
check_result $? "Add text"
mv tmp.pem client-3072-rsapss.pem
echo "End of section"
echo "---------------------------------------------------------------------"


