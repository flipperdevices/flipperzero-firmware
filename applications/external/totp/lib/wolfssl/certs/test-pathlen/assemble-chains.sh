#!/bin/bash
#
# assemble-chains.sh
# Create certs and assemble all the certificate CA path test cert chains.

check_result(){
    if [ $1 -ne 0 ]; then
        echo "$2 Failed, Abort"
        exit 1
    else
        echo "$2 Succeeded!"
    fi
}

create_an_intermediate(){
    # $1 - chain ID
    # $2 - ICA Number (Example entity signed by ICA1 signed by ICA2 and so on)
    # $2 - pathLength to use
    # $3 - Signer of this Intermediate
    # $4 - The signers Key
    # example: create_an_intermediate "chainA" "ICA1" "0" "../ca-cert.pem" "../ca-key.pem"
    chainID="$1"
    icaNum="$2"
    pathLen="$3"
    signer="$4"
    signerKey="$5"
    echo "pathLen = $3, $pathLen"
    echo ""
    #pipe the following arguments to openssl req...

    if [ "$pathLen" = "no_pathlen" ]; then
        echo "Updating $chainID-$icaNum-$pathLen.pem"

        echo -e "US\\nWashington\\nSeattle\\nwolfSSL Inc.\\nEngineering\\n$chainID-$icaNum-$pathLen\\ninfo@wolfssl.com\\n.\\n.\\n" | openssl req -new -key "$chainID-$icaNum-key.pem" -config ../renewcerts/wolfssl.cnf -nodes -sha256 > temp-req.pem
        check_result $? "Step 1"

        openssl x509 -req -in temp-req.pem -extfile ../renewcerts/wolfssl.cnf -extensions wolfssl_opts_ICA -days 1000 -CA $signer -CAkey $signerKey -set_serial 100 -sha256 > "$chainID-$icaNum-$pathLen.pem"
        check_result $? "Step 2"

        rm temp-req.pem
        openssl x509 -in "$chainID-$icaNum-$pathLen.pem" -text > ca_tmp.pem
        check_result $? "Step 3"
        mv ca_tmp.pem "$chainID-$icaNum-$pathLen.pem"
    else
        echo "Updating $chainID-$icaNum-pathlen$pathLen.pem"

        echo -e "US\\nWashington\\nSeattle\\nwolfSSL Inc.\\nEngineering\\n$chainID-$icaNum-pathlen$pathLen\\ninfo@wolfssl.com\\n.\\n.\\n" | openssl req -new -key "$chainID-$icaNum-key.pem" -config ../renewcerts/wolfssl.cnf -nodes -sha256 > temp-req.pem
        check_result $? "Step 1"

        openssl x509 -req -in temp-req.pem -extfile ../renewcerts/wolfssl.cnf -extensions "pathlen_$pathLen" -days 1000 -CA $signer -CAkey $signerKey -set_serial 100 -sha256 > "$chainID-$icaNum-pathlen$pathLen.pem"
        check_result $? "Step 2"

        rm temp-req.pem
        openssl x509 -in "$chainID-$icaNum-pathlen$pathLen.pem" -text > ca_tmp.pem
        check_result $? "Step 3"
        mv ca_tmp.pem "$chainID-$icaNum-pathlen$pathLen.pem"
    fi
    echo "End of Section"
    echo "-------------------------------------------------------------------------"
}

###########################################################
########## update chainA-entity.pem        ################
###########################################################
create_an_entity(){
    # $1 - chain ID
    # $2 - ICA Number (Example entity signed by ICA1 signed by ICA2 and so on)
    # $2 - pathLength to use
    # $3 - Signer of this Intermediate
    # $4 - The signers Key
    # example: create_an_intermediate "chainA" "ICA1" "0" "../ca-cert.pem" "../ca-key.pem"
    chainID="$1"
    signer="$2"
    signerKey="$3"
    echo "Updating $chainID-entity.pem"
    echo ""
    #pipe the following arguments to openssl req...
    echo -e "US\\nWashington\\nSeattle\\nwolfSSL Inc.\\nEngineering\\n$chainID-entity\\ninfo@wolfssl.com\\n.\\n.\\n" | openssl req -new -key "$chainID-entity-key.pem" -config ../renewcerts/wolfssl.cnf -nodes -sha256 > temp-req.pem
    check_result $? "Step 1"

    openssl x509 -req -in temp-req.pem -extfile ../renewcerts/wolfssl.cnf -extensions test_pathlen -days 1000 -CA "$signer" -CAkey "$signerKey" -set_serial 101 -sha256 > "$chainID"-entity.pem
    check_result $? "Step 2"

    rm temp-req.pem
    openssl x509 -in "$chainID"-entity.pem -text > cert_tmp.pem
    check_result $? "Step 3"
    mv cert_tmp.pem "$chainID"-entity.pem
    echo "End of Section"
    echo "-------------------------------------------------------------------------"
}

###########################################################
########## Create the certs                ################
###########################################################
create_an_intermediate "chainA" "ICA1" "0" "../ca-cert.pem" "../ca-key.pem"
create_an_entity "chainA" "chainA-ICA1-pathlen0.pem" "chainA-ICA1-key.pem"

create_an_intermediate "chainB" "ICA2" "1" "../ca-cert.pem" "../ca-key.pem"
create_an_intermediate "chainB" "ICA1" "0" "chainB-ICA2-pathlen1.pem" "chainB-ICA2-key.pem"
create_an_entity "chainB" "chainB-ICA1-pathlen0.pem" "chainB-ICA1-key.pem"

create_an_intermediate "chainC" "ICA1" "1" "../ca-cert.pem" "../ca-key.pem"
create_an_entity "chainC" "chainC-ICA1-pathlen1.pem" "chainC-ICA1-key.pem"

create_an_intermediate "chainD" "ICA1" "127" "../ca-cert.pem" "../ca-key.pem"
create_an_entity "chainD" "chainD-ICA1-pathlen127.pem" "chainD-ICA1-key.pem"

create_an_intermediate "chainE" "ICA1" "128" "../ca-cert.pem" "../ca-key.pem"
create_an_entity "chainE" "chainE-ICA1-pathlen128.pem" "chainE-ICA1-key.pem"

create_an_intermediate "chainF" "ICA2" "0" "../ca-cert.pem" "../ca-key.pem"
create_an_intermediate "chainF" "ICA1" "1" "chainF-ICA2-pathlen0.pem" "chainF-ICA2-key.pem"
create_an_entity "chainF" "chainF-ICA1-pathlen1.pem" "chainF-ICA1-key.pem"

create_an_intermediate "chainG" "ICA7" "100" "../ca-cert.pem" "../ca-key.pem"
create_an_intermediate "chainG" "ICA6" "10" "chainG-ICA7-pathlen100.pem" "chainG-ICA7-key.pem"
create_an_intermediate "chainG" "ICA5" "20" "chainG-ICA6-pathlen10.pem" "chainG-ICA6-key.pem"
create_an_intermediate "chainG" "ICA4" "5" "chainG-ICA5-pathlen20.pem" "chainG-ICA5-key.pem"
create_an_intermediate "chainG" "ICA3" "99" "chainG-ICA4-pathlen5.pem" "chainG-ICA4-key.pem"
create_an_intermediate "chainG" "ICA2" "1" "chainG-ICA3-pathlen99.pem" "chainG-ICA3-key.pem"
create_an_intermediate "chainG" "ICA1" "0" "chainG-ICA2-pathlen1.pem" "chainG-ICA2-key.pem"
create_an_entity "chainG" "chainG-ICA1-pathlen0.pem" "chainG-ICA1-key.pem"

# Fail: PathLen of 2, signing PathLen of 2, signing Pathlen of 2, signing PathLen 0
#       max_path_len = 2, max_path_len -= 1 (1), max_path_len -= 1 (0), max-path_len 0, non-entity cert.
create_an_intermediate "chainH" "ICA4" "2" "../ca-cert.pem" "../ca-key.pem"
create_an_intermediate "chainH" "ICA3" "2" "chainH-ICA4-pathlen2.pem" "chainH-ICA4-key.pem"
create_an_intermediate "chainH" "ICA2" "2" "chainH-ICA3-pathlen2.pem" "chainH-ICA3-key.pem"
create_an_intermediate "chainH" "ICA1" "0" "chainH-ICA2-pathlen2.pem" "chainH-ICA2-key.pem"
create_an_entity "chainH" "chainH-ICA1-pathlen0.pem" "chainH-ICA1-key.pem"

# Success, PathLen of 2 followed by 2 Intermediates with no pathLen set
create_an_intermediate "chainI" "ICA3" "2" "../ca-cert.pem" "../ca-key.pem"
create_an_intermediate "chainI" "ICA2" "no_pathlen" "chainI-ICA3-pathlen2.pem" "chainI-ICA3-key.pem"
create_an_intermediate "chainI" "ICA1" "no_pathlen" "chainI-ICA2-no_pathlen.pem" "chainI-ICA2-key.pem"
create_an_entity "chainI" "chainI-ICA1-no_pathlen.pem" "chainI-ICA1-key.pem"

# Fail: PathLen of 2 followed by 3 Intermediates with no pathLen set
create_an_intermediate "chainJ" "ICA4" "2" "../ca-cert.pem" "../ca-key.pem"
create_an_intermediate "chainJ" "ICA3" "no_pathlen" "chainJ-ICA4-pathlen2.pem" "chainJ-ICA4-key.pem"
create_an_intermediate "chainJ" "ICA2" "no_pathlen" "chainJ-ICA3-no_pathlen.pem" "chainJ-ICA3-key.pem"
create_an_intermediate "chainJ" "ICA1" "no_pathlen" "chainJ-ICA2-no_pathlen.pem" "chainJ-ICA2-key.pem"
create_an_entity "chainJ" "chainJ-ICA1-no_pathlen.pem" "chainJ-ICA1-key.pem"

###########################################################
########## Assemble Chains                 ################
###########################################################
# Success: PathLen of 0
## chainA-ICA1-pathlen0.pem: signed by ca-cert.pem
## chainA-entity.pem: signed by chainA-ICA1-pathlen0.pem
cat chainA-entity.pem chainA-ICA1-pathlen0.pem > chainA-assembled.pem

# Success: PathLen of 1
## chainB-ICA2-pathlen1.pem: signed by ca-cert.pem
## chainB-ICA1-pathlen0.pem: signed by chainB-ICA2-pathlen1.pem
## chainB-entity.pem: signed by chainB-ICA1-pathlen0.pem
cat chainB-entity.pem chainB-ICA1-pathlen0.pem chainB-ICA2-pathlen1.pem > chainB-assembled.pem
## chainC-entity.pem: signed by chainC-ICA1-pathlen1.pem
cat chainC-entity.pem chainC-ICA1-pathlen1.pem > chainC-assembled.pem

# Success: PathLen of 127
## chainD-ICA1-pathlen127.pem: signed by ca-cert.pem
## chainD-entity.pem: signed by chainD-entity.pem
cat chainD-entity.pem chainD-ICA1-pathlen127.pem > chainD-assembled.pem

# Failure: PathLen of 128
## chainE-ICA1-pathlen128.pem: signed by ca-cert.pem
## chainE-entity.pem: signed by chainE-ICA1-pathlen128.pem
cat chainE-entity.pem chainE-ICA1-pathlen128.pem > chainE-assembled.pem

# Failure: PathLen of 0, signing PathLen of 1
## chainF-ICA1-pathlen1.pem: signed by chainA-ICA1-pathlen0.pem
## chainF-entity.pem: signed by chainF-ICA1-pathlen1.pem
cat chainF-entity.pem chainF-ICA1-pathlen1.pem chainF-ICA2-pathlen0.pem > chainF-assembled.pem

# Success: PathLen of 127, signing PathLen of 10, signing PathLen of 20, signing
#          PathLen of 5, signing PathLen of 99, signing PathLen of 1, signing
#          PathLen of 0
cat chainG-entity.pem chainG-ICA1-pathlen0.pem > chainG-assembled.pem
cat chainG-ICA2-pathlen1.pem chainG-ICA3-pathlen99.pem >> chainG-assembled.pem
cat chainG-ICA4-pathlen5.pem chainG-ICA5-pathlen20.pem >> chainG-assembled.pem
cat chainG-ICA6-pathlen10.pem chainG-ICA7-pathlen100.pem >> chainG-assembled.pem

# Fail: PathLen of 2, signing PathLen of 2, signing Pathlen of 2, signing PathLen 0
#       max_path_len = 2, max_path_len -= 1 (1), max_path_len -= 1 (0), max-path_len 0, non-entity cert.
cat chainH-entity.pem chainH-ICA1-pathlen0.pem > chainH-assembled.pem
cat chainH-ICA2-pathlen2.pem chainH-ICA3-pathlen2.pem >> chainH-assembled.pem
cat chainH-ICA4-pathlen2.pem >> chainH-assembled.pem

# Fail:
cat chainI-entity.pem chainI-ICA1-no_pathlen.pem > chainI-assembled.pem
cat chainI-ICA2-no_pathlen.pem chainI-ICA3-pathlen2.pem >> chainI-assembled.pem

# Fail: PathLen of 2, signing PathLen of 2, signing Pathlen of 2, signing PathLen 0
#       max_path_len = 2, max_path_len -= 1 (1), max_path_len -= 1 (0), max-path_len 0, non-entity cert.
cat chainJ-entity.pem chainJ-ICA1-no_pathlen.pem > chainJ-assembled.pem
cat chainJ-ICA2-no_pathlen.pem chainJ-ICA3-no_pathlen.pem >> chainJ-assembled.pem
cat chainJ-ICA4-pathlen2.pem >> chainJ-assembled.pem

