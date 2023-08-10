#!/bin/bash

generate() {
    # read in certificate and alter the last part of the signature
    num_lines=$(wc -l < $cert)
    i=1

    rm -f $pem_out
    touch $pem_out
    while IFS= read -r line
    do
        if [[ $((i+1)) -eq ${num_lines} ]]; then
            # last line before END tag. Alter the sig here
            idx=`expr ${#line} - 4`
            chr=${line:idx:1}
            if [ "$chr" == "x" ] || [ "$chr" == "X" ]; then
                echo "${line:0:${idx}}a${line:$((idx+1)):$((idx+4))}" >> $pem_out
            else
                echo "${line:0:${idx}}x${line:$((idx+1)):$((idx+4))}" >> $pem_out
            fi
        else
            echo "$line" >> $pem_out
        fi
    let i++
    done < "$cert"

    # output to DER format also
    openssl x509 -in $pem_out -out $der_out -outform DER
}

# create server RSA certificate with bad signature
cert="../server-cert.pem"
pem_out=server-cert-rsa-badsig.pem
der_out=server-cert-rsa-badsig.der
generate

# create server ECC certificate with bad signature
cert="../server-ecc.pem"
pem_out=server-cert-ecc-badsig.pem
der_out=server-cert-ecc-badsig.der
generate

