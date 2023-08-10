#!/bin/bash

set -e # exit on any command failure
if [ ! -d wolfssl ]; then
    if [ -z $WOLFSSL_DIR ]; then
        echo "Failed to find WOLFSSL_DIR, please set it and run again"
    else
        git clone $WOLFSSL_DIR
        git clone https://github.com/wolfssl/wolfssl-examples
    fi
fi
