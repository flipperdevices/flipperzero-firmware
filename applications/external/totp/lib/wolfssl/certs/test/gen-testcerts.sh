#!/bin/sh
check_result() {
    if [ $1 -ne 0 ]; then
        echo "Step Failed, Abort"
        exit 1
    else
        echo "Step Succeeded!"
    fi
}

# Args: 1=FileName, 2=CN, 3=AltName, 4=CA
build_test_cert_conf() {
    echo "# Generated openssl conf"                      > "$1".conf
    echo ""                                             >> "$1".conf
    echo "[ ca ]"                                       >> "$1".conf
    echo "default_ca = CA_default"                      >> "$1".conf
    echo "[ CA_default ]"                               >> "$1".conf
    echo "certificate     = ../ca-cert.pem"             >> "$1".conf
    echo "database        = ./index.txt"                >> "$1".conf
    echo "new_certs_dir   = ./certs"                    >> "$1".conf
    echo "private_key     = ./private/cakey.pem"        >> "$1".conf
    echo "serial          = ./serial"                   >> "$1".conf
    echo "default_md      = sha256"                     >> "$1".conf
    echo "default_days    = 1000"                       >> "$1".conf
    echo "policy          = default_ca_policy"          >> "$1".conf
    echo ""                                             >> "$1".conf
    echo "[ default_ca_policy ]"                        >> "$1".conf
    echo "commonName              = supplied"           >> "$1".conf
    echo "stateOrProvinceName     = supplied"           >> "$1".conf
    echo "countryName             = supplied"           >> "$1".conf
    echo "emailAddress            = supplied"           >> "$1".conf
    echo "organizationName        = optional"           >> "$1".conf
    echo "organizationalUnitName  = optional"           >> "$1".conf
    echo ""                                             >> "$1".conf
    echo "[ req ]"                                      >> "$1".conf
    echo "prompt = no"                                  >> "$1".conf
    echo "default_bits        = 2048"                   >> "$1".conf
    echo "distinguished_name  = req_distinguished_name" >> "$1".conf
    if [ -n "$3" ]; then
        echo "req_extensions      = req_ext"            >> "$1".conf
    fi
    if [ -n "$4" ]; then
        echo "basicConstraints=CA:true,pathlen:0"       >> "$1".conf
        echo ""                                         >> "$1".conf
    fi
    echo ""                                             >> "$1".conf
    echo "[ req_distinguished_name ]"                   >> "$1".conf
    echo "C = US"                                       >> "$1".conf
    echo "ST = Montana"                                 >> "$1".conf
    echo "L = Bozeman"                                  >> "$1".conf
    echo "OU = Engineering"                             >> "$1".conf
    echo "CN = $2"                                      >> "$1".conf
    echo "emailAddress = info@wolfssl.com"              >> "$1".conf
    echo ""                                             >> "$1".conf
    if [ -n "$3" ]; then
        echo "[ req_ext ]"                              >> "$1".conf
        case "$3" in
            *DER*)
               echo "subjectAltName = $3"               >> "$1".conf
               ;;
            *)
               echo "subjectAltName = @alt_names"       >> "$1".conf
               echo "[alt_names]"                       >> "$1".conf
               echo "DNS.1 = $3"                        >> "$1".conf
               ;;
        esac
    fi
}

# Args: 1=FileName
generate_test_cert() {
    rm "$1".der
    rm "$1".pem

    echo "step 1 create configuration"
    build_test_cert_conf "$1" "$2" "$3"
    check_result $?

    echo "step 2 create csr"
    openssl req -new -sha256 -out "$1".csr -key ../server-key.pem -config "$1".conf
    check_result $?

    echo "step 3 check csr"
    openssl req -text -noout -in "$1".csr -config "$1".conf
    check_result $?

    echo "step 4 create cert"
    if [ "$3" = "" ]; then
        openssl x509 -req -days 1000 -sha256 \
                     -in "$1".csr -signkey ../server-key.pem \
                     -out "$1".pem -extfile "$1".conf
    else
        openssl x509 -req -days 1000 -sha256 \
                     -in "$1".csr -signkey ../server-key.pem \
                     -out "$1".pem -extensions req_ext -extfile "$1".conf
    fi
    check_result $?
    rm "$1".conf
    rm "$1".csr

    if [ -n "$4" ]; then
        echo "step 5 generate crl"
        mkdir ../crl/demoCA
        touch ../crl/demoCA/index.txt
        touch ../crl/demoCA/index.txt.attr
        echo "01" > ../crl/crlnumber
        openssl ca -config ../renewcerts/wolfssl.cnf -gencrl -crldays 1000 \
                   -out crl.revoked -keyfile ../server-key.pem -cert "$1".pem
        check_result $?
        rm ../crl/"$1"Crl.pem
        openssl crl -in crl.revoked -text > tmp.pem
        check_result $?
        mv tmp.pem ../crl/"$1"Crl.pem
        rm crl.revoked
        rm -rf ../crl/demoCA #cleans up index.txt and index.txt.attr
        rm ../crl/crlnumber*
    fi

    echo "step 6 add cert text information to pem"
    openssl x509 -inform pem -in "$1".pem -text > tmp.pem
    check_result $?
    mv tmp.pem "$1".pem

    echo "step 7 make binary der version"
    openssl x509 -inform pem -in "$1".pem -outform der -out "$1".der
    check_result $?
}

generate_expired_certs() {
    rm "$1".der
    rm "$1".pem

    mkdir -p certs
    touch ./index.txt
    touch ./index.txt.attr
    echo 1000 > ./serial

    echo "step 1 create configuration"
    build_test_cert_conf "$1" www.wolfssl.com 0 "$3"
    check_result $?

    echo "step 2 create csr"
    openssl req -new -sha256 -out "$1".csr -key "$2" -config "$1".conf
    check_result $?

    echo "step 3 check csr"
    openssl req -text -noout -in "$1".csr -config "$1".conf
    check_result $?

    echo "step 4 create cert"
    openssl ca -config ../renewcerts/wolfssl.cnf -selfsign -config "$1".conf \
               -keyfile "$2" -in "$1".csr -out "$1".pem \
               -startdate 20180731000000Z -enddate 20180830000000Z -batch
    check_result $?
    rm "$1".conf
    rm "$1".csr

    echo "step 5 add cert text information to pem"
    openssl x509 -inform pem -in "$1".pem -text > tmp.pem
    check_result $?
    mv tmp.pem "$1".pem

    echo "step 7 make binary der version"
    openssl x509 -inform pem -in "$1".pem -outform der -out "$1".der
    check_result $?

    rm -rf certs
    rm ./index.txt*
    rm ./serial*
}

# Generate Good CN=localhost, Alt=None
generate_test_cert server-goodcn localhost "" 1

# Generate Good CN=www.nomatch.com, Alt=localhost
generate_test_cert server-goodalt www.nomatch.com localhost 1

# Generate Good CN=*localhost, Alt=None
# Surround "*localhost" with quotes to prevent shell expansion to wildcard
generate_test_cert server-goodcnwild "*localhost" "" 1

# Generate Good CN=www.nomatch.com, Alt=*localhost
# Surround "*localhost" with quotes to prevent shell expansion to wildcard
generate_test_cert server-goodaltwild www.nomatch.com "*localhost" 1

# Generate Bad CN=localhost\0h, Alt=None
# DG: Have not found a way to properly encode null in common name
generate_test_cert server-badcnnull DER:30:0d:82:0b:6c:6f:63:61:6c:68:6f:73:74:00:68

# Generate Bad Name CN=www.nomatch.com, Alt=None
generate_test_cert server-badcn www.nomatch.com

# Generate Bad Alt CN=www.nomatch.com, Alt=localhost\0h
generate_test_cert server-badaltnull www.nomatch.com DER:30:0d:82:0b:6c:6f:63:61:6c:68:6f:73:74:00:68

# Generate Bad Alt Name CN=www.nomatch.com, Alt=www.nomatch.com
generate_test_cert server-badaltname www.nomatch.com www.nomatch.com

# Generate Good Alt Name CN=localhost, Alt=localhost
generate_test_cert server-localhost localhost localhost

# Generate Bad Alt Name CN=localhost, Alt=garbage
generate_test_cert server-garbage localhost garbage


# Generate Expired Certificates
generate_expired_certs expired/expired-ca ../ca-key.pem 1
generate_expired_certs expired/expired-cert ../server-key.pem
