#!/bin/sh

# this script will reformat the wolfSSL source code to be compatible with
# an Arduino project
# run as bash ./wolfssl-arduino.sh

DIR=${PWD##*/}

space(){
    echo "" >> "$1"
}

if [ "$DIR" = "ARDUINO" ]; then
	if [ ! -d "wolfSSL" ]; then
	    mkdir wolfSSL
    fi

    cp ../../src/*.c ./wolfSSL
    cp ../../wolfcrypt/src/*.c ./wolfSSL

    if [ ! -d "wolfSSL/wolfssl" ]; then
	    mkdir wolfSSL/wolfssl
    fi
    cp ../../wolfssl/*.h ./wolfSSL/wolfssl
    if [ ! -d "wolfSSL/wolfssl/wolfcrypt" ]; then
        mkdir wolfSSL/wolfssl/wolfcrypt
    fi
    cp ../../wolfssl/wolfcrypt/*.h ./wolfSSL/wolfssl/wolfcrypt

    # support misc.c as include in wolfcrypt/src
    if [ ! -d "./wolfSSL/wolfcrypt" ]; then
        mkdir ./wolfSSL/wolfcrypt
    fi
    if [ ! -d "./wolfSSL/wolfcrypt/src" ]; then
        mkdir ./wolfSSL/wolfcrypt/src
    fi
    cp ../../wolfcrypt/src/misc.c ./wolfSSL/wolfcrypt/src
    cp ../../wolfcrypt/src/asm.c  ./wolfSSL/wolfcrypt/src

    # put bio and evp as includes
    mv ./wolfSSL/bio.c ./wolfSSL/wolfssl
	mv ./wolfSSL/evp.c ./wolfSSL/wolfssl

    # make a copy of evp.c and bio.c for ssl.c to include inline
    cp ./wolfSSL/wolfssl/evp.c ./wolfSSL/wolfcrypt/src/evp.c
    cp ./wolfSSL/wolfssl/bio.c ./wolfSSL/wolfcrypt/src/bio.c
    
    # copy openssl compatibility headers to their appropriate location
    if [ ! -d "./wolfSSL/wolfssl/openssl" ]; then
        mkdir ./wolfSSL/wolfssl/openssl
    fi
    cp ../../wolfssl/openssl/* ./wolfSSL/wolfssl/openssl

    echo "/* Generated wolfSSL header file for Arduino */" > ./wolfSSL/wolfssl.h
    echo "#include <user_settings.h>" >> ./wolfSSL/wolfssl.h
    echo "#include <wolfssl/wolfcrypt/settings.h>" >> ./wolfSSL/wolfssl.h
    echo "#include <wolfssl/ssl.h>" >> ./wolfSSL/wolfssl.h

    if [ ! -f "./wolfSSL/user_settings.h" ]; then
        echo "/* Generated wolfSSL user_settings.h file for Arduino */" > ./wolfSSL/user_settings.h
        echo "#ifndef ARDUINO_USER_SETTINGS_H" >> ./wolfSSL/user_settings.h
        echo "#define ARDUINO_USER_SETTINGS_H" >> ./wolfSSL/user_settings.h
        space ./wolfSSL/user_settings.h
        echo "/* Platform */" >> ./wolfSSL/user_settings.h
        echo "#define WOLFSSL_ARDUINO" >> ./wolfSSL/user_settings.h
        space ./wolfSSL/user_settings.h
        echo "/* Math library (remove this to use normal math)*/" >>  ./wolfSSL/user_settings.h
        echo "#define USE_FAST_MATH" >> ./wolfSSL/user_settings.h
        echo "#define TFM_NO_ASM" >> ./wolfSSL/user_settings.h
        space ./wolfSSL/user_settings.h
        echo "/* RNG DEFAULT !!FOR TESTING ONLY!! */" >>  ./wolfSSL/user_settings.h
        echo "/* comment out the error below to get started w/ bad entropy source" >>  ./wolfSSL/user_settings.h
        echo " * This will need fixed before distribution but is OK to test with */" >>  ./wolfSSL/user_settings.h
        echo "#error \"needs solved, see: https://www.wolfssl.com/docs/porting-guide/\"" >>  ./wolfSSL/user_settings.h
        echo "#define WOLFSSL_GENSEED_FORTEST" >> ./wolfSSL/user_settings.h
        space ./wolfSSL/user_settings.h
        echo "#endif /* ARDUINO_USER_SETTINGS_H */" >> ./wolfSSL/user_settings.h
    fi

    cp wolfSSL/wolfssl/wolfcrypt/settings.h wolfSSL/wolfssl/wolfcrypt/settings.h.bak
    echo " /* wolfSSL Generated ARDUINO settings */" > ./wolfSSL/wolfssl/wolfcrypt/settings.h
    echo "#ifndef WOLFSSL_USER_SETTINGS" >> ./wolfSSL/wolfssl/wolfcrypt/settings.h
    echo "    #define WOLFSSL_USER_SETTINGS" >> ./wolfSSL/wolfssl/wolfcrypt/settings.h
    echo "#endif /* WOLFSSL_USER_SETTINGS */" >> ./wolfSSL/wolfssl/wolfcrypt/settings.h
    echo " /* wolfSSL Generated ARDUINO settings: END */" >> ./wolfSSL/wolfssl/wolfcrypt/settings.h
    cat ./wolfSSL/wolfssl/wolfcrypt/settings.h.bak >> ./wolfSSL/wolfssl/wolfcrypt/settings.h

else
    echo "ERROR: You must be in the IDE/ARDUINO directory to run this script"
fi
