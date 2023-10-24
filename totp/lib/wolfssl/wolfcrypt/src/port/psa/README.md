wolfSSL using Platform Security Architecture (PSA)
=================================================

## Overview

wolfSSL can be configured to use a PSA library for the following operations:
* hashes: SHA-1, SHA-224, SAH-256
* AES: AES-ECB, AES-CBC, AES-CTR, AES-GCM, AES-CCM
* ECDH PK callbacks (P-256)
* ECDSA PK callbacks (P-256)
* RNG

Client certificates are not supported.

## Build
### Basic configuration

To enable PSA support in wolfSSL you need to provide the name of the library
that implements the PSA interface using `--with-psa-lib-name=LIBNAME`. You may
also provide the path to the library headers (with `--with-psa-include`) and to
to the PSA library itself (with `--with-psa-lib`). As an example, to use mbedtls
PSA implementation you may use:

```
./configure --enable-psa --with-psa-include=/dir/to/mbedtls/include --with-psa-lib=/dir/to/mbedtls/library --with-psa-lib-name=mbedcrypto
```

### Customize build

You can switch off some of the algorithms supported using the following defines:

 * `WOLFSSL_PSA_NO_RNG`: disable PSA random generator support
 * `WOLFSSL_PSA_NO_HASH`: disable PSA hashing support
 * `WOLFSSL_PSA_NO_AES`: disable PSA AES support
 * `WOLFSSL_PSA_NO_PKCBS`: disable PK callbacks support

## Supporting non-thread safe PSA implementation

If the PSA implementation used is not thread-safe, you can serialize all calls
to the PSA library using the define `WOLFSSL_PSA_GLOBAL_LOCK`.

## Using PSA PK callbacks

To use Public Key PSA callbacks you need to enable them using the function:

`wolfSSL_CTX_psa_enable(WOLFSSL_CTX *ctx)`

Context information regarding a single SSL session is stored inside a `struct
psa_ssl_ctx`.  The user needs to define and setup a `struct psa_ssl_ctx`, then
to attach it to an ssl session with `wolfSSL_set_psa_ctx(WOLFSSL *ssl, struct
psa_ssl_ctx *ctx)`. When the session is over, the application should free the
psa context with `wolfSSL_free_psa_ctx(struct psa_ssl_ctx *ctx)`.

Example:
```
    struct psa_ssl_ctx psa_ctx = { 0 };
    WOLFSSL_CTX*     ctx;
    WOLFSSL*         ssl;
    int ret

    /* WOLFSSL_CTX initialization here */

    ret = wolfSSL_CTX_psa_enable(ctx);
    if (ret != WOLFSSL_SUCCESS)
        printf("can't enable PSA on ctx");

    ssl = wolfSSL_new(ctx);
    ret = wolfSSL_set_psa_ctx(ssl, &psa_ctx);
    if (ret != WOLFSSL_SUCCESS)
        printf("can't enable PSA on ctx");

    /* SSL communication */

    wolfSSL_free_psa_ctx(&psa_ctx);
    SSL_free(ssl); ssl = NULL;
    SSL_CTX_free(ctx); ctx = NULL;
```

### Setup server side private key

On the server side, the private key of the server should be loaded inside the
PSA engine storage. To use the key inside the ssl session you have to invoke
`wolfSSL_psa_set_private_key_id(struct psa_ssl_ctx *ctx, psa_key_id_t id);`. The
key needs to have `PSA_KEY_USAGE_SIGN_HASH` usage flag set.  Example:

```
    psa_key_attributes_t key_attr = { 0 };
    struct psa_ssl_ctx psa_ctx = { 0 };
    psa_key_type_t key_type;
    psa_key_id_t key_id;
    psa_status_t status;

    key_type = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1);

    psa_set_key_type(&key_attr, key_type);
    psa_set_key_usage_flags(&key_attr, PSA_KEY_USAGE_SIGN_HASH);
    psa_set_key_algorithm(&key_attr, PSA_ALG_ECDSA_ANY);

    status = psa_import_key(&key_attr, ecc_key_256,
                            sizeof(ecc_key_256), &key_id);

    if (status != PSA_SUCCESS) {
        fprintf(stderr,
                "ERROR: provisioning of private key failed: [%d] \n", status);
        exit(-1);
    }

    wolfSSL_psa_set_private_key_id(&psa_ctx, key_id);
}

```

## Additional note
    - AES-GCM, AES-CTR, AES-CCM are not implemented directly. The software
      implementation uses the AES-ECB PSA one to implement them.
