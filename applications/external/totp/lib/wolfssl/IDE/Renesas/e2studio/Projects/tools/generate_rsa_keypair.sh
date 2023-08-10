#!/bin/bash

function usage(){
    cat << _EOT_
    Usage :
     $0 [-g] [-]

    Description:
     Generate 2048 bit Rsa key pair and Display modulus and public exponent
     
    Options:
      -g    generate rsa key pair, default on
      -s    only show modulus and public exponent
_EOT_
exit 1
}

FLAG_G="on"
FLAG_S="off"

    while getopts gsh OPT
    do
        case $OPT in
        g)
          FLAG_G="on"
          ;;
        s)
          FLAG_S="on"
          FLAG_G="off"
          ;;
        h)
          usage
          ;;
        \?)
          usage
          ;;
        esac
    done

if [ $FLAG_G = "on" ]; then
    # generate 2048bit Rsa private key
    openssl genrsa 2048 2> /dev/null > private-key.pem 
    # expose public key
    openssl rsa -in private-key.pem -pubout -out public-key.pem 2> /dev/null
fi

if [ $FLAG_S = "on" ]; then
    # display modulus and exponent
    openssl rsa -modulus < private-key.pem 2>/dev/null | grep Modulus=
    openssl rsa -text < private-key.pem 2> /dev/null | grep publicExponent
fi

