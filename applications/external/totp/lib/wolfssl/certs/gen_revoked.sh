#!/bin/bash

    ###########################################################
    ########## update and sign server-revoked-key.pem ################
    ###########################################################
    echo "Updating server-revoked-cert.pem"
    echo ""
    #pipe the following arguments to openssl req...
    echo -e "US\nMontana\nBozeman\nwolfSSL_revoked\nSupport_revoked\nwww.wolfssl.com\ninfo@wolfssl.com\n.\n.\n" | openssl req -new -key server-revoked-key.pem -nodes > server-revoked-req.pem

    openssl x509 -req -in server-revoked-req.pem -extfile renewcerts/wolfssl.cnf -extensions wolfssl_opts -days 1000 -CA ca-cert.pem -CAkey ca-key.pem -set_serial 02 > server-revoked-cert.pem

    rm server-revoked-req.pem

    openssl x509 -in ca-cert.pem -text > ca_tmp.pem
    openssl x509 -in server-revoked-cert.pem -text > srv_tmp.pem
    mv srv_tmp.pem server-revoked-cert.pem
    cat ca_tmp.pem >> server-revoked-cert.pem
    rm ca_tmp.pem

