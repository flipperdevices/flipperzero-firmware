#!/bin/sh

# Place CMakeList in needed directories

cp CMakeLists_wolfSSL.txt ../../CMakeLists.txt
echo Copied CMakeLists_wolfSSL.txt to wolfssl/CMakeLists.txt
cp CMakeLists_wolfCrypt.txt ../../wolfcrypt/CMakeLists.txt
echo Copied CMakeLists_wolfCrypt.txt to wolfssl/wolfcrypt/CMakeLists.txt

