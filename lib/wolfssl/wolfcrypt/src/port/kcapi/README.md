

wolfSSL using libkcapi
======================

## Overview

wolfSSL can be configured to use libkcapi for the following operations:
* hashes: SHA-224, SHA-256, SHA-384, SHA-512
* HMAC: HMAC-SHA1, HMAC-SHA224, HMAC-SHA256, HMAC-SHA384, HMAC-SHA512
* AES: AES-ECB, AES-CBC, AES-CTR, AES-GCM
* RSA PKCS 1.5, PSS and OAEP padding (raw encrypt/decrypt used with libkcapi)
* DH
* ECDH (P-192, P-256, P-384, P-521)
* ECDSA (P-192, P-256, P-384, P-521)

Note: Linux kernel does not support public key operations without patching.


## Building libkcapi

Based on README from https://github.com/smuellerDD/libkcapi

```sh
git clone https://github.com/smuellerDD/libkcapi
cd libkcapi

autoreconf -i

./configure	--enable-kcapi-test \
    --enable-kcapi-speed \
    --enable-kcapi-hasher \
    --enable-kcapi-rngapp \
    --enable-kcapi-encapp \
    --enable-kcapi-dgstapp \
    --enable-lib-asym \
    --enable-lib-kpp \
    --prefix=/usr/
make
sudo make install

# Optional testing
cd test
./test.sh
```

## Building wolfSSL

#### Basic configuration

To enable libkcapi support in wolfSSL for non-public key algorithms:

```
./configure --enable-kcapi
```

The following defines are added to the compile line:
 - WOLFSSL_KCAPI_AES
 - WOLFSSL_KCAPI_HASH
 - WOLFSSL_KCAPI_HMAC

If AES-CCM is enabled then the following define is also added:
 - WOLFSSL_AES_DIRECT


#### Other defines

If hash operations need to be copied or have an intermediate hash result
returned then add 'WOLFSSL_KCAPI_HASH_KEEP' to the compile line.

For example:

```
./configure --enable-kcapi C_EXTRA_FLAGS=-DWOLFSSL_KCAPI_HASH_KEEP
```

Note: This is required for TLS.


#### RSA configuration

To enable libkcapi support in wolfSSL for RSA:

```
./configure --enable-kcapi --enable-kcapi-rsa
```

This adds 'WOLFSSL_KCAPI_RSA' to the compile line.

#### DH configuration

To enable libkcapi support in wolfSSL for DH:

```
./configure --enable-kcapi --enable-kcapi-dh
```

#### ECC configuration

To enable libkcapi support in wolfSSL for ECC:

```
./configure --enable-kcapi --enable-kcapi-ecc
```

This enables support for ECDH and ECDSA.

#### Make

Ensure libkcapi is installed.

```
make
```


#### Testing

Testing is only supported for the basic configuration.

```
./configure --enable-kcapi C_EXTRA_FLAGS=-DWOLFSSL_KCAPI_HASH_KEEP --disable-md5 --disable-sha3
make check
```

Disabling MD5 and SHA-3 allows all HMAC tests to pass.


#### Install

```
make install
```


## Basic Algorithms with libkcapi

The basic algorithms include all those supported by the Linux kernel.

#### Hash

Cipher Name: "sha224", "sha256", "sha384", "sha512"

When partial results are needed from a hash then define: WOLFSSL_KCAPI_HASH_KEEP

This will keep a copy of all the message data. When a hash result is required,
the data is passed to libkcapi to perform the operation. When the final hash is
requested (eg wc_Sha256Final) then the message data is disposed of. (Required for TLS)

#### HMAC

Cipher Name: "hmac(sha1)", "hmac(sha224)", "hmac(sha256)", "hmac(sha384)", "hmac(sha512)"

#### AES

Cipher Name: "ecb(aes)", "cbc(aes)", "gcm(aes)", "ctr(aes)"

AES-CCM is supported through the support of AES-ECB in libkcapi.


## Public Key Algorithms with libkcapi

The Linux kernel must be patched in order for the public key algorithms to be
available. (See libkcapi README.md)

Algorithms available in the Linux kernel: DH, RSA, ECDH

wolfSSL supports ECDSA for specific hardware through libkcapi.

#### RSA

Cipher Name: "rsa"

RSA operations are supported by using the raw RSA encrypt/decrypt operations
through libkcapi.
This means that wolfSSL performs the padding. Therefore the following padding schemes are supported:
* PKCS v1.5 for sign/verify and encrypt/decrypt
* PSS for sign/verify
* OAEP for encrypt/decrypt

#### ECDH

Cipher Name: "ecdh"

The Linux kernel only supports the curves P-192 and P-256.

The following curves have been added for hardware specific use:
* P-384 using value of 3
* P-521 using value of 4

The curve is set using kcapi_kpp_ecdh_setcurve().

#### ECDSA

The Linux kernel does not support ECDSA operations.

Support for specific hardware has been added with the following details.

Cipher Name: "ecdsa"

Private Key format: version|curve-id|private-key
Public Key format: version|curve-id|x-ord|y-ord

version - one byte and has a value of 1.
curve-id - one byte and has the same value as used by ECDH.
private-key - big-endian encoded number the length of the curve.
x-ord - big-endian encoded number the length of the curve in bytes.
y-ord - big-endian encoded number the length of the curve in bytes.
