#!/bin/bash
# renewcerts.sh
#
# renews the following certs:
#                       client-cert.pem
#                       client-cert.der
#                       client-ecc-cert.pem
#                       client-ecc-cert.der
#                       ca-cert.pem
#                       ca-cert.der
#                       ca-ecc-cert.pem
#                       ca-ecc-cert.der
#                       ca-ecc384-cert.pem
#                       ca-ecc384-cert.der
#                       server-cert.pem
#                       server-cert.der
#                       server-cert-chain.der
#                       server-ecc-rsa.pem
#                       server-ecc.pem
#                       1024/client-cert.der
#                       1024/client-cert.pem
#                       server-ecc-comp.pem
#                       client-ca.pem
#                       test/digsigku.pem
#                       ecc-privOnlyCert.pem
#                       client-uri-cert.pem
#                       client-absolute-uri.pem
#                       client-relative-uri.pem
#                       client-crl-dist.pem
#                       entity-no-ca-bool-cert.pem
#                       fpki-cert.der
#                       rid-cert.der
# updates the following crls:
#                       crl/cliCrl.pem
#                       crl/crl.pem
#                       crl/crl.revoked
#                       crl/eccCliCRL.pem
#                       crl/eccSrvCRL.pem
#
#                       pkcs7:
#                       test-degenerate.p7b
###############################################################################
######################## FUNCTIONS SECTION ####################################
###############################################################################

check_result(){
    if [ $1 -ne 0 ]; then
        echo "Failed at \"$2\", Abort"
        exit 1
    else
        echo "$2 Succeeded!"
    fi
}

#the function that will be called when we are ready to renew the certs.
run_renewcerts(){

    #call update for some ecc certs
    ./certs/ecc/genecc.sh
    check_result $? "Step 0"

    cd certs/ || { echo "Couldn't cd to certs directory"; exit 1; }
    echo ""

    #move the custom cnf into our working directory
    cp renewcerts/wolfssl.cnf wolfssl.cnf || exit 1

    # To generate these all in sha1 add the flag "-sha1" on appropriate lines
    # That is all lines beginning with:  "openssl req"

    ############################################################
    #### update the self-signed (2048-bit) client-uri-cert.pem #
    ############################################################
    echo "Updating 2048-bit client-uri-cert.pem"
    echo ""
    #pipe the following arguments to openssl req...
    echo -e "US\\nMontana\\nBozeman\\nwolfSSL_2048\\nURI\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n.\\n.\\n" | openssl req -new -key client-key.pem -config ./wolfssl.cnf -nodes -out client-cert.csr
    check_result $? "Step 1"

    openssl x509 -req -in client-cert.csr -days 1000 -extfile wolfssl.cnf -extensions uri -signkey client-key.pem -out client-uri-cert.pem
    check_result $? "Step 2"
    rm client-cert.csr

    openssl x509 -in client-uri-cert.pem -text > tmp.pem
    check_result $? "Step 3"
    mv tmp.pem client-uri-cert.pem
    echo "End of section"
    echo "---------------------------------------------------------------------"

    ############################################################
    # Public Versions of client-key.pem
    ############################################################
    openssl rsa -inform pem -in client-key.pem -outform der -out client-keyPub.der -pubout
    openssl rsa -inform pem -in client-key.pem -outform pem -out client-keyPub.pem -pubout

    ############################################################
    # Public Versions of server-key.pem
    ############################################################
    #openssl rsa -inform pem -in server-key.pem -outform der -out server-keyPub.der -pubout
    openssl rsa -inform pem -in server-key.pem -outform pem -out server-keyPub.pem -pubout

    ############################################################
    # Public Versions of ecc-key.pem
    ############################################################
    #openssl ec -inform pem -in ecc-key.pem -outform der -out ecc-keyPub.der -pubout
    openssl ec -inform pem -in ecc-key.pem -outform pem -out ecc-keyPub.pem -pubout

    ############################################################
    #### update the self-signed (2048-bit) client-absolute-urn.pem
    ############################################################
    echo "Updating 2048-bit client-absolute-urn.pem"
    echo ""
    #pipe the following arguments to openssl req...
    echo -e "US\\nMontana\\nBozeman\\nwolfSSL_2048\\nABSOLUTE_URN\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n.\\n.\\n" | openssl req -new -key client-key.pem -config ./wolfssl.cnf -nodes -out client-cert.csr
    check_result $? "Step 1"


    openssl x509 -req -in client-cert.csr -days 1000 -extfile wolfssl.cnf -extensions absolute_urn -signkey client-key.pem -out client-absolute-urn.pem
    check_result $? "Step 2"
    rm client-cert.csr

    openssl x509 -in client-absolute-urn.pem -text > tmp.pem
    check_result $? "Step 3"
    mv tmp.pem client-absolute-urn.pem
    echo "End of section"
    echo "---------------------------------------------------------------------"

    ############################################################
    #### update the self-signed (2048-bit) client-relative-uri.pem
    ############################################################
    echo "Updating 2048-bit client-relative-uri.pem"
    echo ""
    #pipe the following arguments to openssl req...
    echo -e "US\\nMontana\\nBozeman\\nwolfSSL_2048\\nRELATIVE_URI\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n.\\n.\\n" | openssl req -new -key client-key.pem -config ./wolfssl.cnf -nodes -out client-cert.csr
    check_result $? "Step 1"


    openssl x509 -req -in client-cert.csr -days 1000 -extfile wolfssl.cnf -extensions relative_uri -signkey client-key.pem -out client-relative-uri.pem
    check_result $? "Step 2"
    rm client-cert.csr

    openssl x509 -in client-relative-uri.pem -text > tmp.pem
    check_result $? "Step 3"
    mv tmp.pem client-relative-uri.pem
    echo "End of section"
    echo "---------------------------------------------------------------------"
    ############################################################
    #### update the self-signed (2048-bit) client-cert-ext.pem
    ############################################################
    echo "Updating 2048-bit client-cert-ext.pem"
    echo ""
    #pipe the following arguments to openssl req...
    echo -e "US\\nMontana\\nBozeman\\nwolfSSL_2048\\nProgramming-2048\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n.\\n.\\n" | openssl req -new -key client-key.pem -config ./wolfssl.cnf -nodes -out client-cert.csr
    check_result $? "Step 1"


    openssl x509 -req -in client-cert.csr -days 1000 -extfile wolfssl.cnf -extensions client_cert_ext -signkey client-key.pem -out client-cert-ext.pem
    check_result $? "Step 2"
    rm client-cert.csr

    openssl x509 -in client-cert-ext.pem -outform DER -out client-cert-ext.der
    check_result $? "Step 3"
    openssl x509 -in client-cert-ext.pem -text > tmp.pem
    check_result $? "Step 4"
    mv tmp.pem client-cert-ext.pem
    echo "End of section"
    echo "---------------------------------------------------------------------"
    ############################################################
    #### update the self-signed (2048-bit) client-crl-dist.pem
    ############################################################
    echo "Updating 2048-bit client-crl-dist.pem"
    echo ""
    #pipe the following arguments to openssl req...
    echo -e "US\\nMontana\\nBozeman\\nwolfSSL_2048\\nCRL_DIST\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n.\\n.\\n" | openssl req -new -key client-key.pem -config ./wolfssl.cnf -nodes -out client-cert.csr
    check_result $? "Step 1"


    openssl x509 -req -in client-cert.csr -days 1000 -extfile wolfssl.cnf -extensions crl_dist_points -signkey client-key.pem -out client-crl-dist.pem
    check_result $? "Step 2"
    rm client-cert.csr

    openssl x509 -in client-crl-dist.pem -text > tmp.pem
    check_result $? "Step 3"
    mv tmp.pem client-crl-dist.pem

    openssl x509 -in client-crl-dist.pem -outform der -out client-crl-dist.der
    echo "End of section"
    echo "---------------------------------------------------------------------"
    ############################################################
    #### update the self-signed (2048-bit) client-cert.pem #####
    ############################################################
    echo "Updating 2048-bit client-cert.pem"
    echo ""
    #pipe the following arguments to openssl req...
    echo -e "US\\nMontana\\nBozeman\\nwolfSSL_2048\\nProgramming-2048\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n.\\n.\\n" | openssl req -new -key client-key.pem -config ./wolfssl.cnf -nodes -out client-cert.csr
    check_result $? "Step 1"


    openssl x509 -req -in client-cert.csr -days 1000 -extfile wolfssl.cnf -extensions wolfssl_opts -signkey client-key.pem -out client-cert.pem
    check_result $? "Step 2"
    rm client-cert.csr

    openssl x509 -in client-cert.pem -text > tmp.pem
    check_result $? "Step 3"
    mv tmp.pem client-cert.pem
    echo "End of section"
    echo "---------------------------------------------------------------------"

    ############################################################
    #### update the self-signed (1024-bit) client-cert.pem #####
    ############################################################
    echo "Updating 1024-bit client-cert.pem"
    echo ""
    #pipe the following arguments to openssl req...
    echo -e "US\\nMontana\\nBozeman\\nwolfSSL_1024\\nProgramming-1024\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n.\\n.\\n" | openssl req -new -key ./1024/client-key.pem -config ./wolfssl.cnf -nodes -out ./1024/client-cert.csr
    check_result $? "Step 1"


    openssl x509 -req -in ./1024/client-cert.csr -days 1000 -extfile wolfssl.cnf -extensions wolfssl_opts -signkey ./1024/client-key.pem -out ./1024/client-cert.pem
    check_result $? "Step 2"
    rm ./1024/client-cert.csr

    openssl x509 -in ./1024/client-cert.pem -text > ./1024/tmp.pem
    check_result $? "Step 3"
    mv ./1024/tmp.pem ./1024/client-cert.pem
    echo "End of section"
    echo "---------------------------------------------------------------------"
    ############################################################
    #### update the self-signed (3072-bit) client-cert.pem #####
    ############################################################
    echo "Updating 3072-bit client-cert.pem"
    echo ""
    #pipe the following arguments to openssl req...
    echo -e "US\\nMontana\\nBozeman\\nwolfSSL_3072\\nProgramming-3072\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n.\\n.\\n" | openssl req -new -key ./3072/client-key.pem -config ./wolfssl.cnf -nodes -out ./3072/client-cert.csr
    check_result $? "Step 1"

    openssl x509 -req -in ./3072/client-cert.csr -days 1000 -extfile wolfssl.cnf -extensions wolfssl_opts -signkey ./3072/client-key.pem -out ./3072/client-cert.pem
    check_result $? "Step 2"
    rm ./3072/client-cert.csr

    openssl x509 -in ./3072/client-cert.pem -text > ./3072/tmp.pem
    check_result $? "Step 3"
    mv ./3072/tmp.pem ./3072/client-cert.pem

    openssl rsa -in ./3072/client-key.pem -outform der -out ./3072/client-key.der
    openssl rsa -inform pem -in ./3072/client-key.pem -outform der -out ./3072/client-keyPub.der -pubout
    openssl x509 -in ./3072/client-cert.pem -outform der -out ./3072/client-cert.der

    echo "End of section"
    echo "---------------------------------------------------------------------"

    ############################################################
    #### update the self-signed (4096-bit) client-cert.pem #####
    ############################################################
    echo "Updating 4096-bit client-cert.pem"
    echo ""
    #pipe the following arguments to openssl req...
    echo -e "US\\nMontana\\nBozeman\\nwolfSSL_4096\\nProgramming-4096\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n.\\n.\\n" | openssl req -new -key ./4096/client-key.pem -config ./wolfssl.cnf -nodes -out ./4096/client-cert.csr
    check_result $? "Step 1"

    openssl x509 -req -in ./4096/client-cert.csr -days 1000 -extfile wolfssl.cnf -extensions wolfssl_opts -signkey ./4096/client-key.pem -out ./4096/client-cert.pem
    check_result $? "Step 2"
    rm ./4096/client-cert.csr

    openssl x509 -in ./4096/client-cert.pem -text > ./4096/tmp.pem
    check_result $? "Step 3"
    mv ./4096/tmp.pem ./4096/client-cert.pem

    openssl rsa -in ./4096/client-key.pem -outform der -out ./4096/client-key.der
    openssl rsa -inform pem -in ./4096/client-key.pem -outform der -out ./4096/client-keyPub.der -pubout
    openssl x509 -in ./4096/client-cert.pem -outform der -out ./4096/client-cert.der
    echo "End of section"
    echo "---------------------------------------------------------------------"

    ############################################################
    ########## update the self-signed ca-cert.pem ##############
    ############################################################
    echo "Updating ca-cert.pem"
    echo ""
    #pipe the following arguments to openssl req...
    echo -e  "US\\nMontana\\nBozeman\\nSawtooth\\nConsulting\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n.\\n.\\n" | openssl req -new -key ca-key.pem -config ./wolfssl.cnf -nodes -out ca-cert.csr
    check_result $? "Step 1"

    openssl x509 -req -in ca-cert.csr -days 1000 -extfile wolfssl.cnf -extensions wolfssl_opts -signkey ca-key.pem -out ca-cert.pem
    check_result $? "Step 2"
    rm ca-cert.csr

    openssl x509 -in ca-cert.pem -text > tmp.pem
    check_result $? "Step 3"
    mv tmp.pem ca-cert.pem
    echo "End of section"
    echo "---------------------------------------------------------------------"
    ############################################################
    ########## update the self-signed ca-cert-chain.der ########
    ############################################################
    echo "Updating ca-cert-chain.der"
    echo ""
    #pipe the following arguments to openssl req...
    echo -e  "US\\nMontana\\nBozeman\\nSawtooth\\nConsulting\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n.\\n.\\n" | openssl req -new -key 1024/ca-key.pem -config ./wolfssl.cnf -nodes -out ca-cert.csr
    check_result $? "Step 1"

    openssl x509 -req -in ca-cert.csr -days 1000 -extfile wolfssl.cnf -extensions wolfssl_opts -signkey 1024/ca-key.pem -outform DER -out ca-cert-chain.der
    check_result $? "Step 2"
    rm ca-cert.csr
    echo "End of section"
    echo "---------------------------------------------------------------------"
    ############################################################
    ########## update the self-signed ca-ecc-cert.pem ##########
    ############################################################
    echo "Updating ca-ecc-cert.pem"
    echo ""
    #pipe the following arguments to openssl req...
    echo -e  "US\\nWashington\\nSeattle\\nwolfSSL\\nDevelopment\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n.\\n.\\n" | openssl req -new -key ca-ecc-key.pem -config ./wolfssl.cnf -nodes -out ca-ecc-cert.csr
    check_result $? "Step 1"

    openssl x509 -req -in ca-ecc-cert.csr -days 1000 -extfile wolfssl.cnf -extensions ca_ecc_cert -signkey ca-ecc-key.pem -out ca-ecc-cert.pem
    check_result $? "Step 2"
    rm ca-ecc-cert.csr

    openssl x509 -in ca-ecc-cert.pem -text > tmp.pem
    check_result $? "Step 3"
    mv tmp.pem ca-ecc-cert.pem
    echo "End of section"
    echo "---------------------------------------------------------------------"
    ############################################################
    ########## update the self-signed ca-ecc384-cert.pem #######
    ############################################################
    echo "Updating ca-ecc384-cert.pem"
    echo ""
    #pipe the following arguments to openssl req...
    echo -e  "US\\nWashington\\nSeattle\\nwolfSSL\\nDevelopment\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n.\\n.\\n" | openssl req -new -key ca-ecc384-key.pem -config ./wolfssl.cnf -nodes -sha384 -out ca-ecc384-cert.csr
    check_result $? "Step 1"

    openssl x509 -req -in ca-ecc384-cert.csr -days 1000 -extfile wolfssl.cnf -extensions ca_ecc_cert -signkey ca-ecc384-key.pem -sha384 -out ca-ecc384-cert.pem
    check_result $? "Step 2"
    rm ca-ecc384-cert.csr

    openssl x509 -in ca-ecc384-cert.pem -text > tmp.pem
    check_result $? "Step 3"
    mv tmp.pem ca-ecc384-cert.pem
    echo "End of section"
    echo "---------------------------------------------------------------------"
    ############################################################
    ##### update the self-signed (1024-bit) ca-cert.pem ########
    ############################################################
    echo "Updating 1024-bit ca-cert.pem"
    echo ""
    #pipe the following arguments to openssl req...
    echo -e  "US\\nMontana\\nBozeman\\nSawtooth\\nConsulting_1024\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n.\\n.\\n" | openssl req -new -key ./1024/ca-key.pem -config ./wolfssl.cnf -nodes -sha1 -out ./1024/ca-cert.csr
    check_result $? "Step 1"

    openssl x509 -req -in ./1024/ca-cert.csr -days 1000 -extfile wolfssl.cnf -extensions wolfssl_opts -signkey ./1024/ca-key.pem -out ./1024/ca-cert.pem
    check_result $? "Step 2"
    rm ./1024/ca-cert.csr

    openssl x509 -in ./1024/ca-cert.pem -text > ./1024/tmp.pem
    check_result $? "Step 3"
    mv ./1024/tmp.pem ./1024/ca-cert.pem
    echo "End of section"
    echo "---------------------------------------------------------------------"
    ###########################################################
    ########## update and sign fpki-cert.der ################
    ###########################################################
    echo "Updating fpki-cert.der"
    echo ""
    #pipe the following arguments to openssl req...
    echo -e "US\\nMontana\\nBozeman\\nwolfSSL\\nFPKI\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n.\\n.\\n" | openssl req -new -key server-key.pem -config ./wolfssl.cnf -nodes > fpki-req.pem
    check_result $? "Step 1"

    openssl x509 -req -in fpki-req.pem -extfile wolfssl.cnf -extensions fpki_ext -days 1000 -CA ca-cert.pem -CAkey ca-key.pem -set_serial 01 -out fpki-cert.der -outform DER
    check_result $? "Step 2"
    rm fpki-req.pem
    echo "End of section"
    echo "---------------------------------------------------------------------"
    ###########################################################
    ########## update and sign rid-cert.der ################
    ###########################################################
    echo "Updating rid-cert.der"
    echo ""
    #pipe the following arguments to openssl req...
    echo -e "US\\nMontana\\nBozeman\\nwolfSSL\\nRID\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n.\\n.\\n" | openssl req -new -key server-key.pem -config ./wolfssl.cnf -nodes > rid-req.pem
    check_result $? "Step 1"

    openssl x509 -req -in rid-req.pem -extfile wolfssl.cnf -extensions rid_ext -days 1000 -CA ca-cert.pem -CAkey ca-key.pem -set_serial 7 -out rid-cert.der -outform DER
    check_result $? "Step 2"
    rm rid-req.pem
    echo "End of section"
    echo "---------------------------------------------------------------------"
    ###########################################################
    ########## update and sign server-cert.pem ################
    ###########################################################
    echo "Updating server-cert.pem"
    echo ""
    #pipe the following arguments to openssl req...
    echo -e "US\\nMontana\\nBozeman\\nwolfSSL\\nSupport\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n.\\n.\\n" | openssl req -new -key server-key.pem -config ./wolfssl.cnf -nodes > server-req.pem
    check_result $? "Step 1"

    openssl x509 -req -in server-req.pem -extfile wolfssl.cnf -extensions wolfssl_opts -days 1000 -CA ca-cert.pem -CAkey ca-key.pem -set_serial 01 > server-cert.pem
    check_result $? "Step 2"

    rm server-req.pem

    openssl x509 -in ca-cert.pem -text > ca_tmp.pem
    check_result $? "Step 3"
    openssl x509 -in server-cert.pem -text > srv_tmp.pem
    check_result $? "Step 4"
    mv srv_tmp.pem server-cert.pem
    cat ca_tmp.pem >> server-cert.pem
    rm ca_tmp.pem
    echo "End of section"
    echo "---------------------------------------------------------------------"
    ###########################################################
    ########## update and sign server-revoked-key.pem #########
    ###########################################################
    echo "Updating server-revoked-cert.pem"
    echo ""
    #pipe the following arguments to openssl req...
    echo -e "US\\nMontana\\nBozeman\\nwolfSSL_revoked\\nSupport_revoked\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n.\\n.\\n" | openssl req -new -key server-revoked-key.pem -config ./wolfssl.cnf -nodes > server-revoked-req.pem
    check_result $? "Step 1"

    openssl x509 -req -in server-revoked-req.pem -extfile wolfssl.cnf -extensions wolfssl_opts -days 1000 -CA ca-cert.pem -CAkey ca-key.pem -set_serial 02 > server-revoked-cert.pem
    check_result $? "Step 2"
    rm server-revoked-req.pem

    openssl x509 -in ca-cert.pem -text > ca_tmp.pem
    check_result $? "Step 3"
    openssl x509 -in server-revoked-cert.pem -text > srv_tmp.pem
    check_result $? "Step 4"
    mv srv_tmp.pem server-revoked-cert.pem
    cat ca_tmp.pem >> server-revoked-cert.pem
    rm ca_tmp.pem
    echo "End of section"
    echo "---------------------------------------------------------------------"
    ###########################################################
    ########## update and sign server-duplicate-policy.pem ####
    ###########################################################
    echo "Updating server-duplicate-policy.pem"
    echo ""
    #pipe the following arguments to openssl req...
    echo -e "US\\nMontana\\nBozeman\\nwolfSSL\\ntesting duplicate policy\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n.\\n.\\n" | openssl req -new -key server-key.pem -config ./wolfssl.cnf -nodes > ./test/server-duplicate-policy-req.pem
    check_result $? "Step 1"

    openssl x509 -req -in ./test/server-duplicate-policy-req.pem -extfile wolfssl.cnf -extensions policy_test -days 1000 -CA ca-cert.pem -CAkey ca-key.pem -set_serial 02 > ./test/server-duplicate-policy.pem
    check_result $? "Step 2"
    rm ./test/server-duplicate-policy-req.pem

    openssl x509 -in ca-cert.pem -text > ca_tmp.pem
    check_result $? "Step 3"
    openssl x509 -in ./test/server-duplicate-policy.pem -text > srv_tmp.pem
    check_result $? "Step 4"
    mv srv_tmp.pem ./test/server-duplicate-policy.pem
    cat ca_tmp.pem >> ./test/server-duplicate-policy.pem
    rm ca_tmp.pem
    echo "End of section"
    echo "---------------------------------------------------------------------"
    ###########################################################
    #### update and sign (1024-bit) server-cert.pem ###########
    ###########################################################
    echo "Updating 1024-bit server-cert.pem"
    echo ""
    #pipe the following arguments to openssl req...
    echo -e "US\\nMontana\\nBozeman\\nwolfSSL\\nSupport_1024\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n.\\n.\\n" | openssl req -new -key ./1024/server-key.pem -config ./wolfssl.cnf -nodes -sha1 > ./1024/server-req.pem
    check_result $? "Step 1"

    openssl x509 -req -in ./1024/server-req.pem -extfile wolfssl.cnf -extensions wolfssl_opts -days 1000 -CA ./1024/ca-cert.pem -CAkey ./1024/ca-key.pem -set_serial 01 > ./1024/server-cert.pem
    check_result $? "Step 2"
    rm ./1024/server-req.pem

    openssl x509 -in ./1024/ca-cert.pem -text > ./1024/ca_tmp.pem
    check_result $? "Step 3"
    openssl x509 -in ./1024/server-cert.pem -text > ./1024/srv_tmp.pem
    check_result $? "Step 4"
    mv ./1024/srv_tmp.pem ./1024/server-cert.pem
    cat ./1024/ca_tmp.pem >> ./1024/server-cert.pem
    rm ./1024/ca_tmp.pem
    echo "End of section"
    echo "---------------------------------------------------------------------"
    ############################################################
    ########## update and sign the server-ecc-rsa.pem ##########
    ############################################################
    echo "Updating server-ecc-rsa.pem"
    echo ""
    echo -e "US\\nMontana\\nBozeman\\nElliptic - RSAsig\\nECC-RSAsig\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n.\\n.\\n" | openssl req -new -key ecc-key.pem -config ./wolfssl.cnf -nodes > server-ecc-req.pem
    check_result $? "Step 1"

    openssl x509 -req -in server-ecc-req.pem -extfile wolfssl.cnf -extensions wolfssl_opts -days 1000 -CA ca-cert.pem -CAkey ca-key.pem -set_serial 01 > server-ecc-rsa.pem
    check_result $? "Step 2"
    rm server-ecc-req.pem

    openssl x509 -in server-ecc-rsa.pem -text > tmp.pem
    check_result $? "Step 3"
    mv tmp.pem server-ecc-rsa.pem
    echo "End of section"
    echo "---------------------------------------------------------------------"
    ############################################################
    ####### update the self-signed client-ecc-cert.pem #########
    ############################################################
    echo "Updating client-ecc-cert.pem"
    echo ""
    #pipe the following arguments to openssl req...
    echo -e "US\\nOregon\\nSalem\\nClient ECC\\nFast\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n.\\n.\\n" | openssl req -new -key ecc-client-key.pem -config ./wolfssl.cnf -nodes -out client-ecc-cert.csr
    check_result $? "Step 1"

    openssl x509 -req -in client-ecc-cert.csr -days 1000 -extfile wolfssl.cnf -extensions wolfssl_opts -signkey ecc-client-key.pem -out client-ecc-cert.pem
    check_result $? "Step 2"
    rm client-ecc-cert.csr

    openssl x509 -in client-ecc-cert.pem -text > tmp.pem
    check_result $? "Step 3"
    mv tmp.pem client-ecc-cert.pem
    echo "End of section"
    echo "---------------------------------------------------------------------"
    ############################################################
    ########## update the server-ecc.pem #######################
    ############################################################
    echo "Updating server-ecc.pem"
    echo ""
    #pipe the following arguments to openssl req...
    echo -e "US\\nWashington\\nSeattle\\nEliptic\\nECC\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n.\\n.\\n" | openssl req -new -key ecc-key.pem -config ./wolfssl.cnf -nodes -out server-ecc.csr
    check_result $? "Step 1"

    openssl x509 -req -in server-ecc.csr -days 1000 -extfile wolfssl.cnf -extensions server_ecc -CA ca-ecc-cert.pem -CAkey ca-ecc-key.pem -set_serial 03 -out server-ecc.pem
    check_result $? "Step 2"
    rm server-ecc.csr

    openssl x509 -in server-ecc.pem -text > tmp.pem
    check_result $? "Step 3"
    mv tmp.pem server-ecc.pem
    echo "End of section"
    echo "---------------------------------------------------------------------"
    ############################################################
    ###### update the self-signed server-ecc-comp.pem ##########
    ############################################################
    echo "Updating server-ecc-comp.pem"
    echo ""
    #pipe the following arguments to openssl req...
    echo -e "US\\nMontana\\nBozeman\\nElliptic - comp\\nServer ECC-comp\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n.\\n.\\n" | openssl req -new -key ecc-key-comp.pem -config ./wolfssl.cnf -nodes -out server-ecc-comp.csr
    check_result $? "Step 1"

    openssl x509 -req -in server-ecc-comp.csr -days 1000 -extfile wolfssl.cnf -extensions wolfssl_opts -signkey ecc-key-comp.pem -out server-ecc-comp.pem
    check_result $? "Step 2"
    rm server-ecc-comp.csr

    openssl x509 -in server-ecc-comp.pem -text > tmp.pem
    check_result $? "Step 3"
    mv tmp.pem server-ecc-comp.pem
    echo "End of section"
    echo "---------------------------------------------------------------------"
    ############################################################
    ############## create the client-ca.pem file ###############
    ############################################################
    echo "Updating client-ca.pem"
    echo ""
    cat client-cert.pem client-ecc-cert.pem > client-ca.pem
    echo "End of section"
    echo "---------------------------------------------------------------------"
    ############################################################
    ###### update the self-signed ecc-privOnlyCert.pem #########
    ############################################################
    echo "Updating ecc-privOnlyCert.pem"
    echo ""
    #pipe the following arguments to openssl req...
    echo -e ".\\n.\\n.\\nWR\\n.\\nDE\\n.\\n.\\n.\\n" | openssl req -new -key ecc-privOnlyKey.pem -config ./wolfssl.cnf -nodes -out ecc-privOnly.csr
    check_result $? "Step 1"

    openssl x509 -req -in ecc-privOnly.csr -days 1000 -signkey ecc-privOnlyKey.pem -out ecc-privOnlyCert.pem
    check_result $? "Step 2"
    rm ecc-privOnly.csr
    echo "End of section"
    echo "---------------------------------------------------------------------"
    ############################################################
    ###### update the self-signed test/digsigku.pem   ##########
    ############################################################
    echo "Updating test/digsigku.pem"
    echo ""
    #pipe the following arguments to openssl req...
    echo -e "US\\nWashington\\nSeattle\\nFoofarah\\nArglebargle\\nfoobarbaz\\ninfo@worlss.com\\n.\\n.\\n" | openssl req -new -key ecc-key.pem -config ./wolfssl.cnf -nodes -sha1 -out digsigku.csr
    check_result $? "Step 1"

    openssl x509 -req -in digsigku.csr -days 1000 -extfile wolfssl.cnf -extensions digsigku -signkey ecc-key.pem -sha1 -set_serial 16393466893990650224 -out digsigku.pem
    check_result $? "Step 2"
    rm digsigku.csr

    openssl x509 -in digsigku.pem -text > tmp.pem
    check_result $? "Step 3"
    mv tmp.pem digsigku.pem
    mv digsigku.pem test/digsigku.pem
    echo "End of section"
    echo "---------------------------------------------------------------------"


    ###########################################################
    #### update and sign entity-no-ca-bool-cert.pem ###########
    ###########################################################
    echo "Updating entity-no-ca-bool-cert.pem"
    echo ""
    #pipe the following arguments to openssl req...
    echo -e "US\\nMontana\\nBozeman\\nwolfSSL\\nNoCaBool\\nwww.wolfssl.com\\ninfo@wolfssl.com\\n.\\n.\\n" | openssl req -new -key entity-no-ca-bool-key.pem -config ./wolfssl.cnf  -nodes > entity-no-ca-bool-req.pem
    check_result $? "Step 1"

    openssl x509 -req -in entity-no-ca-bool-req.pem -extfile ./wolfssl.cnf -extensions "entity_no_CA_BOOL" -days 1000 -CA ca-cert.pem -CAkey ca-key.pem -set_serial 01 > entity-no-ca-bool-cert.pem
    check_result $? "Step 2"

    rm entity-no-ca-bool-req.pem

    openssl x509 -in ca-cert.pem -text > ca_tmp.pem
    check_result $? "Step 3"
    openssl x509 -in entity-no-ca-bool-cert.pem -text > entity_tmp.pem
    check_result $? "Step 4"
    mv entity_tmp.pem entity-no-ca-bool-cert.pem
    cat ca_tmp.pem >> entity-no-ca-bool-cert.pem
    rm ca_tmp.pem
    echo "End of section"

    ############################################################
    ########## make .der files from .pem files #################
    ############################################################
    echo "Creating der formatted certs..."
    echo ""
    openssl x509 -inform PEM -in ./1024/client-cert.pem -outform DER -out ./1024/client-cert.der
    check_result $? "Der Cert 1"
    openssl x509 -inform PEM -in ./1024/server-cert.pem -outform DER -out ./1024/server-cert.der
    check_result $? "Der Cert 2"
    openssl x509 -inform PEM -in ./1024/ca-cert.pem -outform DER -out ./1024/ca-cert.der
    check_result $? "Der Cert 3"

    openssl x509 -inform PEM -in ca-cert.pem -outform DER -out ca-cert.der
    check_result $? "Der Cert 4"
    openssl x509 -inform PEM -in ca-ecc-cert.pem -outform DER -out ca-ecc-cert.der
    check_result $? "Der Cert 5"
    openssl x509 -inform PEM -in ca-ecc384-cert.pem -outform DER -out ca-ecc384-cert.der
    check_result $? "Der Cert 6"
    openssl x509 -inform PEM -in client-cert.pem -outform DER -out client-cert.der
    check_result $? "Der Cert 7"
    openssl x509 -inform PEM -in server-cert.pem -outform DER -out server-cert.der
    check_result $? "Der Cert 8"
    openssl x509 -inform PEM -in client-ecc-cert.pem -outform DER -out client-ecc-cert.der
    check_result $? "Der Cert 9"
    openssl x509 -inform PEM -in server-ecc-rsa.pem -outform DER -out server-ecc-rsa.der
    check_result $? "Der Cert 10"
    openssl x509 -inform PEM -in server-ecc.pem -outform DER -out server-ecc.der
    check_result $? "Der Cert 11"
    openssl x509 -inform PEM -in server-ecc-comp.pem -outform DER -out server-ecc-comp.der
    check_result $? "Der Cert 12"
    cat server-cert.der ca-cert.der >server-cert-chain.der
    check_result $? "Der Cert 13"
    echo "End of section"
    echo "---------------------------------------------------------------------"

    ############################################################
    ########## generate RSA-PSS certificates ###################
    ############################################################
    echo "Renewing RSA-PSS certificates"
    cd rsapss
    ./renew-rsapss-certs.sh
    cd ..
    echo "End of section"
    echo "---------------------------------------------------------------------"

    ############################################################
    ########## generate Ed25519 certificates ###################
    ############################################################
    echo "Renewing Ed25519 certificates"
    cd ed25519
    ./gen-ed25519-certs.sh
    cd ..
    echo "End of section"
    echo "---------------------------------------------------------------------"

    ############################################################
    ########## generate Ed448 certificates #####################
    ############################################################
    echo "Renewing Ed448 certificates"
    cd ed448
    ./gen-ed448-certs.sh
    cd ..
    echo "End of section"
    echo "---------------------------------------------------------------------"

    ############################################################
    ########## generate P-521 certificates #####################
    ############################################################
    echo "Renewing Ed448 certificates"
    cd p521
    ./gen-p521-certs.sh
    cd ..
    echo "End of section"
    echo "---------------------------------------------------------------------"

    ############################################################
    ###### update the ecc-rsa-server.p12 file ##################
    ############################################################
    echo "Updating ecc-rsa-server.p12 (password is \"\")"
    echo ""
    echo "" | openssl pkcs12 -des3 -descert -export -in server-ecc-rsa.pem -inkey ecc-key.pem -certfile server-ecc.pem -out ecc-rsa-server.p12 -password stdin
    check_result $? "Step 1"
    echo "End of section"
    echo "---------------------------------------------------------------------"
    ############################################################
    ###### update the test-servercert.p12 file #################
    ############################################################
    echo "Updating test-servercert.p12 (password is \"wolfSSL test\")"
    echo ""
    echo "wolfSSL test" | openssl pkcs12 -des3 -descert -export -in server-cert.pem -inkey server-key.pem -certfile ca-cert.pem -out test-servercert.p12 -password stdin
    check_result $? "Step 1"
    echo "End of section"
    echo "---------------------------------------------------------------------"
    ############################################################
    ###### update the test-servercert-rc2.p12 file #############
    ############################################################
    echo "Updating test-servercert-rc2.p12 (password is \"wolfSSL test\")"
    echo ""
    echo "wolfSSL test" | openssl pkcs12 -export -in server-cert.pem -inkey server-key.pem -certfile ca-cert.pem -out test-servercert-rc2.p12 -password stdin
    check_result $? "Step 1"
    echo "End of section"
    echo "---------------------------------------------------------------------"
    ############################################################
    ###### calling gen-ext-certs.sh           ##################
    ############################################################
    echo "Calling gen-ext-certs.sh"
    echo ""
    cd .. || exit 1
    ./certs/test/gen-ext-certs.sh
    check_result $? "gen-ext-certs.sh"
    cd ./certs || { echo "Couldn't cd to certs directory"; exit 1; }
    echo "End of section"
    echo "---------------------------------------------------------------------"
    ############################################################
    ###### calling gen-badsig.sh              ##################
    ############################################################
    echo "Calling gen-badsig.sh"
    echo ""
    cd ./test || { echo "Failed to switch to dir ./test"; exit 1; }
    ./gen-badsig.sh
    check_result $? "gen-badsig.sh"
    cd ../ || exit 1
    echo "End of section"
    echo "---------------------------------------------------------------------"
    ############################################################
    ###### calling gen-testcerts.sh           ##################
    ############################################################
    echo "Calling gen-testcerts.sh"
    echo ""
    cd ./test || { echo "Failed to switch to dir ./test"; exit 1; }
    ./gen-testcerts.sh
    check_result $? "gen-testcerts.sh"
    cd ../ || exit 1
    echo "End of section"
    echo "---------------------------------------------------------------------"
    ############################################################
    ###### generate cms bundles in test directory ##############
    ############################################################
    echo "Generating CMS bundle"
    echo ""
    cd ./test || { echo "Failed to switch to dir ./test"; exit 1; }
    echo "test" | openssl cms -encrypt -binary -keyid -out ktri-keyid-cms.msg -outform der -recip ../client-cert.pem -nocerts
    check_result $? "generate ktri-keyid-cms.msg"
    cd ../ || exit 1
    echo "End of section"
    echo "---------------------------------------------------------------------"
    ############################################################
    ########## generate ocsp certs        ######################
    ############################################################
    echo "Changing directory to ocsp..."
    echo ""

    # guard against recursive calls to renewcerts.sh
    if [ -d ocsp ]; then
        cd ./ocsp || { echo "Failed to switch to dir ./ocsp"; exit 1; }
        echo "Execute ocsp/renewcerts.sh..."
        ./renewcerts.sh
        check_result $? "renewcerts.sh"
        cd ../ || exit 1
    else
        echo "Error could not find ocsp directory"
        exit 1
    fi
    echo "End of section"
    echo "---------------------------------------------------------------------"
    ############################################################
    ###### calling assemble-chains.sh         ##################
    ############################################################
    echo "Calling assemble-chains.sh"
    echo ""
    cd ./test-pathlen || { echo "Failed to switch to dir ./test-pathlen";
                           exit 1; }
    ./assemble-chains.sh
    check_result $? "assemble-chains.sh"
    cd ../ || exit 1
    echo "End of section"
    echo "---------------------------------------------------------------------"
    ############################################################
    ########## store DER files as buffers ######################
    ############################################################
    echo "Changing directory to wolfssl root..."
    echo ""
    cd ../ || exit 1
    echo "Execute ./gencertbuf.pl..."
    echo ""
    ./gencertbuf.pl
    check_result $? "gencertbuf.pl"
    echo "End of section"
    echo "---------------------------------------------------------------------"
    ############################################################
    ########## generate the new crls ###########################
    ############################################################

    echo "Change directory to wolfssl/certs"
    echo ""
    cd ./certs || { echo "Failed to switch to dir ./certs"; exit 1; }
    echo "We are back in the certs directory"
    echo ""

    echo "Updating the crls..."
    echo ""
    cd ./crl || { echo "Failed to switch to dir ./crl"; exit 1; }
    echo "changed directory: cd/crl"
    echo ""
    ./gencrls.sh
    check_result $? "gencrls.sh"
    echo "ran ./gencrls.sh"
    echo ""

    ############################################################
    ########## generate PKCS7 bundles ##########################
    ############################################################
    echo "Changing directory to wolfssl certs..."
    echo ""
    cd ../ || exit 1
    echo "Creating test-degenerate.p7b..."
    echo ""
    openssl crl2pkcs7 -nocrl -certfile ./client-cert.pem -out test-degenerate.p7b -outform DER
    check_result $? ""
    echo "End of section"
    echo "---------------------------------------------------------------------"

    #cleanup the file system now that we're done
    echo "Performing final steps, cleaning up the file system..."
    echo ""

    rm ../wolfssl.cnf
    echo "End of Updates. Everything was successfully updated!"
    echo "---------------------------------------------------------------------"
}

###############################################################################
##################### THE EXECUTABLE BODY #####################################
###############################################################################

#start in root.
cd ../ || exit 1

if [ ! -z "$1" ]; then
    echo "No arguments expected"
    exit 1
fi

echo "Running make clean"
echo ""
make clean
check_result $? "make clean"

touch certs/.rnd || exit 1

run_renewcerts
cd ../ || exit 1
rm ./certs/wolfssl.cnf

exit 0
