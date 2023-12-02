/* ext_kyber.c
 *
 * Copyright (C) 2006-2023 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/logging.h>

#ifdef WOLFSSL_HAVE_KYBER
#include <wolfssl/wolfcrypt/ext_kyber.h>

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

#if defined (HAVE_LIBOQS)

static const char* OQS_ID2name(int id) {
    switch (id) {
        case KYBER_LEVEL1: return OQS_KEM_alg_kyber_512;
        case KYBER_LEVEL3: return OQS_KEM_alg_kyber_768;
        case KYBER_LEVEL5: return OQS_KEM_alg_kyber_1024;
        default:           break;
    }
    return NULL;
}

int ext_kyber_enabled(int id)
{
    const char * name = OQS_ID2name(id);
    return OQS_KEM_alg_is_enabled(name);
}
#endif

/******************************************************************************/
/* Initializer and cleanup functions. */

/**
 * Initialize the Kyber key.
 *
 * @param  [in]   type   Type of key: KYBER512, KYBER768, KYBER1024.
 * @param  [out]  key    Kyber key object to initialize.
 * @param  [in]   heap   Dynamic memory hint.
 * @param  [in]   devId  Device Id.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when key is NULL or type is unrecognized.
 * @return  NOT_COMPILED_IN when key type is not supported.
 */
int wc_KyberKey_Init(int type, KyberKey* key, void* heap, int devId)
{
    int ret = 0;

    /* Validate key. */
    if (key == NULL) {
        ret = BAD_FUNC_ARG;
    }
    if (ret == 0) {
        /* Validate type. */
        switch (type) {
        case KYBER_LEVEL1:
#ifdef HAVE_LIBOQS
        case KYBER_LEVEL3:
        case KYBER_LEVEL5:
#endif /* HAVE_LIBOQS */
            break;
        default:
            /* No other values supported. */
            ret = BAD_FUNC_ARG;
            break;
        }
    }
    if (ret == 0) {
        /* Zero out all data. */
        XMEMSET(key, 0, sizeof(*key));

        /* Keep type for parameters. */
        key->type = type;
    }

    (void)devId;
    (void)heap;

    return ret;
}

/**
 * Free the Kyber key object.
 *
 * @param  [in, out]  key   Kyber key object to dispose of.
 */
void wc_KyberKey_Free(KyberKey* key)
{
    if (key != NULL) {
        /* Ensure all private data is zeroed. */
        ForceZero(key, sizeof(*key));
    }
}

/******************************************************************************/
/* Data size getters. */

/**
 * Get the size in bytes of encoded private key for the key.
 *
 * @param  [in]   key  Kyber key object.
 * @param  [out]  len  Length of encoded private key in bytes.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when key or len is NULL.
 * @return  NOT_COMPILED_IN when key type is not supported.
 */
int wc_KyberKey_PrivateKeySize(KyberKey* key, word32* len)
{
    int ret = 0;

    /* Validate parameters. */
    if ((key == NULL) || (len == NULL)) {
        ret = BAD_FUNC_ARG;
    }

#ifdef HAVE_LIBOQS
    /* NOTE: SHAKE and AES variants have the same length private key. */
    if (ret == 0) {
        switch (key->type) {
        case KYBER_LEVEL1:
            *len = OQS_KEM_kyber_512_length_secret_key;
            break;
        case KYBER_LEVEL3:
            *len = OQS_KEM_kyber_768_length_secret_key;
            break;
        case KYBER_LEVEL5:
            *len = OQS_KEM_kyber_1024_length_secret_key;
            break;
        default:
            /* No other values supported. */
            ret = BAD_FUNC_ARG;
            break;
        }
    }
#endif /* HAVE_LIBOQS */
#ifdef HAVE_PQM4
    (void)key;
    if (ret == 0) {
        *len = PQM4_PRIVATE_KEY_LENGTH;
    }
#endif /* HAVE_PQM4 */

    return ret;
}

/**
 * Get the size in bytes of encoded public key for the key.
 *
 * @param  [in]   key  Kyber key object.
 * @param  [out]  len  Length of encoded public key in bytes.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when key or len is NULL.
 * @return  NOT_COMPILED_IN when key type is not supported.
 */
int wc_KyberKey_PublicKeySize(KyberKey* key, word32* len)
{
    int ret = 0;

    /* Validate parameters. */
    if ((key == NULL) || (len == NULL)) {
        ret = BAD_FUNC_ARG;
    }

#ifdef HAVE_LIBOQS
    /* NOTE: SHAKE and AES variants have the same length public key. */
    if (ret == 0) {
        switch (key->type) {
        case KYBER_LEVEL1:
            *len = OQS_KEM_kyber_512_length_public_key;
            break;
        case KYBER_LEVEL3:
            *len = OQS_KEM_kyber_768_length_public_key;
            break;
        case KYBER_LEVEL5:
            *len = OQS_KEM_kyber_1024_length_public_key;
            break;
        default:
            /* No other values supported. */
            ret = BAD_FUNC_ARG;
            break;
        }
    }
#endif /* HAVE_LIBOQS */
#ifdef HAVE_PQM4
    (void)key;
    if (ret == 0) {
        *len = PQM4_PUBLIC_KEY_LENGTH;
    }
#endif /* HAVE_PQM4 */

    return ret;
}

/**
 * Get the size in bytes of cipher text for key.
 *
 * @param  [in]   key  Kyber key object.
 * @param  [out]  len  Length of cipher text in bytes.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when key or len is NULL.
 * @return  NOT_COMPILED_IN when key type is not supported.
 */
int wc_KyberKey_CipherTextSize(KyberKey* key, word32* len)
{
    int ret = 0;

    /* Validate parameters. */
    if ((key == NULL) || (len == NULL)) {
        ret = BAD_FUNC_ARG;
    }

#ifdef HAVE_LIBOQS
    /* NOTE: SHAKE and AES variants have the same length ciphertext. */
    if (ret == 0) {
        switch (key->type) {
        case KYBER_LEVEL1:
            *len = OQS_KEM_kyber_512_length_ciphertext;
            break;
        case KYBER_LEVEL3:
            *len = OQS_KEM_kyber_768_length_ciphertext;
            break;
        case KYBER_LEVEL5:
            *len = OQS_KEM_kyber_1024_length_ciphertext;
            break;
        default:
            /* No other values supported. */
            ret = BAD_FUNC_ARG;
            break;
        }
    }
#endif /* HAVE_LIBOQS */
#ifdef HAVE_PQM4
    (void)key;
    if (ret == 0) {
        *len = PQM4_CIPHERTEXT_LENGTH;
    }
#endif /* HAVE_PQM4 */

    return ret;
}

/**
 * Size of a shared secret in bytes. Always KYBER_SS_SZ.
 *
 * @param  [in]   key  Kyber key object. Not used.
 * @param  [out]  Size of the shared secret created with a Kyber key.
 * @return  0 on success.
 * @return  0 to indicate success.
 */
int wc_KyberKey_SharedSecretSize(KyberKey* key, word32* len)
{
    (void)key;
    /* Validate parameters. */
    if (len == NULL) {
        return BAD_FUNC_ARG;
    }

    *len = KYBER_SS_SZ;

    return 0;
}

/******************************************************************************/
/* Cryptographic operations. */

/**
 * Make a Kyber key object using a random number generator.
 *
 * NOTE: rng is ignored. OQS and PQM4 don't use our RNG.
 *
 * @param  [in, out]  key   Kyber key ovject.
 * @param  [in]       rng   Random number generator.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when key or rng is NULL.
 * @return  MEMORY_E when dynamic memory allocation failed.
 */
int wc_KyberKey_MakeKey(KyberKey* key, WC_RNG* rng)
{
    int ret = 0;
#ifdef HAVE_LIBOQS
    const char* algName = NULL;
    OQS_KEM *kem = NULL;
#endif

    (void)rng;

    /* Validate parameter. */
    if (key == NULL) {
        return BAD_FUNC_ARG;
    }

#ifdef HAVE_LIBOQS
    if (ret == 0) {
        algName = OQS_ID2name(key->type);
        if (algName == NULL) {
            ret = BAD_FUNC_ARG;
        }
    }

    if (ret == 0) {
        algName = OQS_ID2name(key->type);
        if (algName == NULL) {
            ret = BAD_FUNC_ARG;
        }
    }
    if (ret == 0) {
        kem = OQS_KEM_new(algName);
        if (kem == NULL) {
            ret = BAD_FUNC_ARG;
        }
    }
    if (ret == 0) {
        if (OQS_KEM_keypair(kem, key->pub, key->priv) !=
            OQS_SUCCESS) {
            ret = BAD_FUNC_ARG;
        }
    }
    OQS_KEM_free(kem);
#endif /* HAVE_LIBOQS */
#ifdef HAVE_PQM4
    if (ret == 0) {
        if (crypto_kem_keypair(key->pub, key->priv) != 0) {
            WOLFSSL_MSG("PQM4 keygen failure");
            ret = BAD_FUNC_ARG;
        }
    }
#endif /* HAVE_PQM4 */

    if (ret != 0) {
        ForceZero(key, sizeof(*key));
    }

    return ret;
}

/**
 * Make a Kyber key object using random data.
 *
 * @param  [in, out]  key   Kyber key ovject.
 * @param  [in]       rng   Random number generator.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when key or rand is NULL.
 * @return  BUFFER_E when length is not KYBER_MAKEKEY_RAND_SZ.
 * @return  NOT_COMPILED_IN when key type is not supported.
 * @return  MEMORY_E when dynamic memory allocation failed.
 */
int wc_KyberKey_MakeKeyWithRandom(KyberKey* key, const unsigned char* rand,
    int len)
{
    (void)rand;
    (void)len;
    /* OQS and PQM4 don't support external randomness. */
    return wc_KyberKey_MakeKey(key, NULL);
}

/**
 * Encapsulate with random number generator and derive secret.
 *
 * @param  [in]   key  Kyber key object.
 * @param  [out]  ct   Cipher text.
 * @param  [out]  ss   Shared secret generated.
 * @param  [in]   rng  Random number generator.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when key, ct, ss or RNG is NULL.
 * @return  NOT_COMPILED_IN when key type is not supported.
 * @return  MEMORY_E when dynamic memory allocation failed.
 */
int wc_KyberKey_Encapsulate(KyberKey* key, unsigned char* ct, unsigned char* ss,
    WC_RNG* rng)
{
    int ret = 0;
#ifdef HAVE_LIBOQS
    const char * algName = NULL;
    OQS_KEM *kem = NULL;
#endif

    (void)rng;

    /* Validate parameters. */
    if ((key == NULL) || (ct == NULL) || (ss == NULL)) {
        ret = BAD_FUNC_ARG;
    }

#ifdef HAVE_LIBOQS
    if (ret == 0) {
        algName = OQS_ID2name(key->type);
        if (algName == NULL) {
            ret = BAD_FUNC_ARG;
        }
    }
    if (ret == 0) {
        kem = OQS_KEM_new(algName);
        if (kem == NULL) {
            ret = BAD_FUNC_ARG;
        }
    }
    if (ret == 0) {
        if (OQS_KEM_encaps(kem, ct, ss, key->pub) != OQS_SUCCESS) {
            ret = BAD_FUNC_ARG;
        }
    }

    OQS_KEM_free(kem);
#endif /* HAVE_LIBOQS */
#ifdef HAVE_PQM4
    if (ret == 0) {
        if (crypto_kem_enc(ct, ss, key->pub) != 0) {
            WOLFSSL_MSG("PQM4 Encapsulation failure.");
            ret = BAD_FUNC_ARG;
        }
    }
#endif /* HAVE_PQM4 */

    return ret;
}

/**
 * Encapsulate with random data and derive secret.
 *
 * @param  [out]  ct    Cipher text.
 * @param  [out]  ss    Shared secret generated.
 * @param  [in]   rand  Random data.
 * @param  [in]   len   Random data.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when key, ct, ss or RNG is NULL.
 * @return  BUFFER_E when len is not KYBER_ENC_RAND_SZ.
 * @return  NOT_COMPILED_IN when key type is not supported.
 * @return  MEMORY_E when dynamic memory allocation failed.
 */
int wc_KyberKey_EncapsulateWithRandom(KyberKey* key, unsigned char* ct,
    unsigned char* ss, const unsigned char* rand, int len)
{
    (void)rand;
    (void)len;
    /* OQS and PQM4 don't support external randomness. */
    return wc_KyberKey_Encapsulate(key, ct, ss, NULL);
}

/**
 * Decapsulate the cipher text to calculate the shared secret.
 *
 * Validates the cipher text by encapsulating and comparing with data passed in.
 *
 * @param  [in]   key  Kyber key object.
 * @param  [out]  ss   Shared secret.
 * @param  [in]   ct   Cipher text.
 * @param  [in]   len  Length of cipher text.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when key, ss or cr are NULL.
 * @return  NOT_COMPILED_IN when key type is not supported.
 * @return  BUFFER_E when len is not the length of cipher text for the key type.
 * @return  MEMORY_E when dynamic memory allocation failed.
 */
int wc_KyberKey_Decapsulate(KyberKey* key, unsigned char* ss,
    const unsigned char* ct, word32 len)
{
    int ret = 0;
    word32 ctlen = 0;
#ifdef HAVE_LIBOQS
    const char * algName = NULL;
    OQS_KEM *kem = NULL;
#endif

    /* Validate parameters. */
    if ((key == NULL) || (ss == NULL) || (ct == NULL)) {
        ret = BAD_FUNC_ARG;
    }
    if (ret == 0) {
        ret = wc_KyberKey_CipherTextSize(key, &ctlen);
    }
    if ((ret == 0) && (len != ctlen)) {
        ret = BUFFER_E;
    }

#ifdef HAVE_LIBOQS
    if (ret == 0) {
        algName = OQS_ID2name(key->type);
        if (algName == NULL) {
            ret = BAD_FUNC_ARG;
        }
    }
    if (ret == 0) {
        kem = OQS_KEM_new(algName);
        if (kem == NULL) {
            ret = BAD_FUNC_ARG;
        }
    }
    if (ret == 0) {
        if (OQS_KEM_decaps(kem, ss, ct, key->priv) != OQS_SUCCESS) {
            ret = BAD_FUNC_ARG;
        }
    }

    OQS_KEM_free(kem);
#endif /* HAVE_LIBOQS */
#ifdef HAVE_PQM4
    if (ret == 0) {
        if (crypto_kem_dec(ss, ct, key->priv) != 0) {
            WOLFSSL_MSG("PQM4 Decapsulation failure.");
            ret = BAD_FUNC_ARG;
        }
    }
#endif /* HAVE_PQM4 */

    return ret;

}

/******************************************************************************/
/* Encoding and decoding functions. */

/**
 * Decode the private key.
 *
 * We store the whole thing in the private key buffer. Note this means we cannot
 * do the encapsulation operation with the private key. But generally speaking
 * this is never done.
 *
 * @param  [in, out]  key  Kyber key object.
 * @param  [in]       in   Buffer holding encoded key.
 * @param  [in]       len  Length of data in buffer.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when key or in is NULL.
 * @return  NOT_COMPILED_IN when key type is not supported.
 * @return  BUFFER_E when len is not the correct size.
 */
int wc_KyberKey_DecodePrivateKey(KyberKey* key, unsigned char* in, word32 len)
{
    int ret = 0;
    word32 privLen = 0;

    /* Validate parameters. */
    if ((key == NULL) || (in == NULL)) {
        ret = BAD_FUNC_ARG;
    }

    if (ret == 0) {
        ret = wc_KyberKey_PrivateKeySize(key, &privLen);
    }

    /* Ensure the data is the correct length for the key type. */
    if ((ret == 0) && (len != privLen)) {
        ret = BUFFER_E;
    }

    if (ret == 0) {
        XMEMCPY(key->priv, in, privLen);
    }

    return ret;
}

/**
 * Decode public key.
 *
 * We store the whole thing in the public key buffer.
 *
 * @param  [in, out]  key  Kyber key object.
 * @param  [in]       in   Buffer holding encoded key.
 * @param  [in]       len  Length of data in buffer.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when key or in is NULL.
 * @return  NOT_COMPILED_IN when key type is not supported.
 * @return  BUFFER_E when len is not the correct size.
 */
int wc_KyberKey_DecodePublicKey(KyberKey* key, unsigned char* in, word32 len)
{
    int ret = 0;
    word32 pubLen = 0;

    /* Validate parameters. */
    if ((key == NULL) || (in == NULL)) {
        ret = BAD_FUNC_ARG;
    }

    if (ret == 0) {
        ret = wc_KyberKey_PublicKeySize(key, &pubLen);
    }

    /* Ensure the data is the correct length for the key type. */
    if ((ret == 0) && (len != pubLen)) {
        ret = BUFFER_E;
    }

    if (ret == 0) {
        XMEMCPY(key->pub, in, pubLen);
    }

    return ret;
}

/**
 * Encode the private key.
 *
 * We stored it as a blob so we can just copy it over.
 *
 * @param  [in]   key  Kyber key object.
 * @param  [out]  out  Buffer to hold data.
 * @param  [in]   len  Size of buffer in bytes.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when key or out is NULL or private/public key not
 * available.
 * @return  NOT_COMPILED_IN when key type is not supported.
 */
int wc_KyberKey_EncodePrivateKey(KyberKey* key, unsigned char* out, word32 len)
{
    int ret = 0;
    unsigned int privLen = 0;

    if ((key == NULL) || (out == NULL)) {
        ret = BAD_FUNC_ARG;
    }

    if (ret == 0) {
        ret = wc_KyberKey_PrivateKeySize(key, &privLen);
    }

    /* Check buffer is big enough for encoding. */
    if ((ret == 0) && (len != privLen)) {
        ret = BUFFER_E;
    }

    if (ret == 0) {
        XMEMCPY(out, key->priv, privLen);
    }

    return ret;
}

/**
 * Encode the public key.
 *
 * We stored it as a blob so we can just copy it over.
 *
 * @param  [in]   key  Kyber key object.
 * @param  [out]  out  Buffer to hold data.
 * @param  [in]   len  Size of buffer in bytes.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when key or out is NULL or public key not available.
 * @return  NOT_COMPILED_IN when key type is not supported.
 */
int wc_KyberKey_EncodePublicKey(KyberKey* key, unsigned char* out, word32 len)
{
    int ret = 0;
    unsigned int pubLen = 0;

    if ((key == NULL) || (out == NULL)) {
        ret = BAD_FUNC_ARG;
    }

    if (ret == 0) {
        ret = wc_KyberKey_PublicKeySize(key, &pubLen);
    }

    /* Check buffer is big enough for encoding. */
    if ((ret == 0) && (len != pubLen)) {
        ret = BUFFER_E;
    }

    if (ret == 0) {
        XMEMCPY(out, key->pub, pubLen);
    }

    return ret;
}

#endif /* WOLFSSL_HAVE_KYBER */
