#!/bin/bash

SIGOPT=rsa_padding_mode:pss
SIGOPT2=rsa_pss_saltlen:-1

function usage() {
    cat <<_EOT_
    Usage:
      $0 <pri key> <pub key> <file name>

      pri key : private key for sign/verify
      pub key : public key for sign/verify
      file name : file name to be signed

_EOT_
exit 1
}

if [ $# -ne 3 ]; then
    usage
fi

# $1 private key for sign/verify
# $2 public key for verify
# $3 file for sign/verify

openssl dgst -sha256 -sign $1 -sigopt $SIGOPT -sigopt $SIGOPT2 -out $3.sign $3

echo verify by private key
openssl dgst -sha256 -prverify $1 -sigopt $SIGOPT -sigopt $SIGOPT2 -signature $3.sign $3

echo verify by public key
openssl dgst -sha256 -verify $2 -sigopt $SIGOPT -sigopt $SIGOPT2 -signature $3.sign $3

