/* kcapi_ecc.c
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

#if defined(WOLFSSL_KCAPI_ECC) && defined(HAVE_ECC)

#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/port/kcapi/wc_kcapi.h>
#include <wolfssl/wolfcrypt/port/kcapi/kcapi_ecc.h>
#include <wolfssl/wolfcrypt/ecc.h>

#ifndef WOLFSSL_HAVE_ECC_KEY_GET_PRIV
    /* FIPS build has replaced ecc.h. */
    #define wc_ecc_key_get_priv(key) (&((key)->k))
    #define WOLFSSL_HAVE_ECC_KEY_GET_PRIV
#endif

#ifndef ECC_CURVE_NIST_P256
#define ECC_CURVE_NIST_P256     2
#endif
#ifndef ECC_CURVE_NIST_P384
#define ECC_CURVE_NIST_P384     3
#endif
#ifndef ECC_CURVE_NIST_P521
#define ECC_CURVE_NIST_P521     4
#endif

#define ECDSA_KEY_VERSION       1
#define ECDH_KEY_VERSION        1

static const char WC_NAME_ECDH[] = "ecdh";
#if defined(HAVE_ECC_SIGN) || defined(HAVE_ECC_VERIFY)
static const char WC_NAME_ECDSA[] = "ecdsa";
#endif

void KcapiEcc_Free(ecc_key* key)
{
    if (key->handle != NULL) {
        kcapi_kpp_destroy(key->handle);
        key->handle = NULL;
    }
}

static int KcapiEcc_CurveId(int curve_id, word32* kcapiCurveId)
{
    int ret = 0;

     switch (curve_id) {
         case ECC_SECP256R1:
             *kcapiCurveId = ECC_CURVE_NIST_P256;
             break;
         case ECC_SECP384R1:
             *kcapiCurveId = ECC_CURVE_NIST_P384;
             break;
         case ECC_SECP521R1:
             *kcapiCurveId = ECC_CURVE_NIST_P521;
             break;
         default:
             ret = BAD_FUNC_ARG;
             break;
     }

     return ret;
}

int KcapiEcc_LoadKey(ecc_key* key, byte* pubkey_raw, word32* pubkey_sz,
    int release_handle)
{
    int ret = 0;
    word32 kcapiCurveId = 0;
    word32 keySz;
    int handleInit = 0;

    if (key == NULL || key->dp == NULL) {
        ret = BAD_FUNC_ARG;
    }

    if (ret == 0) {
        keySz = key->dp->size;
        ret = KcapiEcc_CurveId(key->dp->id, &kcapiCurveId);
    }

    /* if handle doesn't exist create one */
    if (ret == 0 && key->handle == NULL) {
        ret = kcapi_kpp_init(&key->handle, WC_NAME_ECDH, 0);
        if (ret == 0) {
            handleInit = 1;
            ret = kcapi_kpp_ecdh_setcurve(key->handle, kcapiCurveId);
            if (ret >= 0) {
                ret = 0;
            }
        }
    }

    /* set the key */
    if (ret == 0) {
        if (mp_iszero(wc_ecc_key_get_priv(key)) != MP_YES) {
            /* if a private key value is set, load and use it */
            byte priv[MAX_ECC_BYTES];
            ret = wc_export_int(wc_ecc_key_get_priv(key), priv, &keySz, keySz,
                WC_TYPE_UNSIGNED_BIN);
            if (ret == 0) {
                ret = kcapi_kpp_setkey(key->handle, priv, keySz);
            }
        }
        else {
            /* generate new ephemeral key */
            ret = kcapi_kpp_setkey(key->handle, NULL, 0);
        }
        if (ret >= 0) {
            ret = 0;
        }
    }

    /* optionally export public key */
    if (ret == 0 && pubkey_raw != NULL && pubkey_sz != NULL) {
        if (*pubkey_sz < keySz*2) {
            ret = BUFFER_E;
        }
        if (ret == 0) {
            ret = (int)kcapi_kpp_keygen(key->handle, pubkey_raw, keySz*2,
                KCAPI_ACCESS_HEURISTIC);
            if (ret >= 0) {
                *pubkey_sz = ret;
                ret = 0;
            }
        }
    }

    if (handleInit && release_handle && key != NULL && key->handle != NULL) {
        kcapi_kpp_destroy(key->handle);
        key->handle = NULL;
    }

    return ret;
}

int KcapiEcc_MakeKey(ecc_key* key, int keysize, int curve_id)
{
    int ret = 0;
    word32 pubkey_sz = (word32)sizeof(key->pubkey_raw);

    /* free existing handle */
    if (key != NULL && key->handle != NULL) {
        kcapi_kpp_destroy(key->handle);
        key->handle = NULL;
    }

    /* check arguments */
    if (key == NULL || key->dp == NULL) {
        ret = BAD_FUNC_ARG;
    }

    ret = KcapiEcc_LoadKey(key, key->pubkey_raw, &pubkey_sz, 0);
    if (ret == 0) {
        ret = mp_read_unsigned_bin(key->pubkey.x,
            key->pubkey_raw, pubkey_sz / 2);
    }
    if (ret == 0) {
        ret = mp_read_unsigned_bin(key->pubkey.y,
            key->pubkey_raw + pubkey_sz / 2, pubkey_sz / 2);
    }
    if (ret == 0) {
        ret = mp_set(key->pubkey.z, 1);
    }
    if (ret == 0) {
        key->type = ECC_PRIVATEKEY;
    }

    /* if error release handle now */
    if (ret != 0 && key->handle != NULL) {
        kcapi_kpp_destroy(key->handle);
        key->handle = NULL;
    }

    /* These are not used. The key->dp is set */
    (void)keysize;
    (void)curve_id;

    return ret;
}

#ifdef HAVE_ECC_DHE
int KcapiEcc_SharedSecret(ecc_key* private_key, ecc_key* public_key, byte* out,
                          word32* outlen)
{
    int ret = 0;
    word32 kcapiCurveId = 0;
    byte* buf_aligned = NULL;
    byte* pub = NULL;
    word32 keySz;
#ifndef KCAPI_USE_XMALLOC
    size_t pageSz = (size_t)sysconf(_SC_PAGESIZE);
#endif

    if (private_key == NULL || private_key->dp == NULL || public_key == NULL) {
        ret = BAD_FUNC_ARG;
    }

    if (ret == 0) {
        pub = public_key->pubkey_raw;
        keySz = private_key->dp->size;
        ret = KcapiEcc_CurveId(private_key->dp->id, &kcapiCurveId);
    }
    if (ret == 0 && private_key->handle == NULL) {
        ret = kcapi_kpp_init(&private_key->handle, WC_NAME_ECDH, 0);
        if (ret == 0) {
            ret = kcapi_kpp_ecdh_setcurve(private_key->handle, kcapiCurveId);
            if (ret >= 0) {
                ret = 0;
            }
        }
    }

    /* if a private key value is set, load and use it */
    if (ret == 0 && mp_iszero(wc_ecc_key_get_priv(private_key)) != MP_YES) {
        byte priv[MAX_ECC_BYTES];
        ret = wc_export_int(wc_ecc_key_get_priv(private_key), priv, &keySz,
            keySz, WC_TYPE_UNSIGNED_BIN);
        if (ret == 0) {
            ret = kcapi_kpp_setkey(private_key->handle, priv, keySz);
            if (ret >= 0) {
                ret = 0;
            }
        }
    }
    if (ret == 0) {
    #ifdef KCAPI_USE_XMALLOC
        buf_aligned = (byte*)XMALLOC(keySz * 2, private_key->heap,
            DYNAMIC_TYPE_TMP_BUFFER);
        if (buf_aligned == NULL) {
            ret = MEMORY_E;
        }
    #else
        ret = posix_memalign((void*)&buf_aligned, pageSz, keySz * 2);
        if (ret != 0) {
            ret = MEMORY_E;
        }
    #endif
    }
    if (ret == 0) {
        XMEMCPY(buf_aligned, pub, keySz * 2);

        ret = (int)kcapi_kpp_ssgen(private_key->handle, buf_aligned,
            keySz * 2, buf_aligned, keySz * 2, KCAPI_ACCESS_HEURISTIC);
        if (ret >= 0) {
            *outlen = ret / 2;
            XMEMCPY(out, buf_aligned, *outlen);

            ret = 0; /* success */
        }
    }

    if (buf_aligned != NULL) {
    #ifdef KCAPI_USE_XMALLOC
        XFREE(buf_aligned, private_key->heap, DYNAMIC_TYPE_TMP_BUFFER);
    #else
        free(buf_aligned);
    #endif
    }

    return ret;
}
#endif

#ifdef HAVE_ECC_SIGN
static int KcapiEcc_SetPrivKey(ecc_key* key)
{
    int ret;
    byte priv[KCAPI_PARAM_SZ + MAX_ECC_BYTES];
    word32 keySz = key->dp->size;
    word32 kcapiCurveId;

    ret = KcapiEcc_CurveId(key->dp->id, &kcapiCurveId);
    if (ret == 0) {
        priv[0] = ECDSA_KEY_VERSION;
        priv[1] = kcapiCurveId;
    #ifdef WOLF_PRIVATE_KEY_ID
        if (key->idLen > 0) {
            WOLFSSL_MSG("Using ID based private key");
            keySz = key->idLen;
            XMEMCPY(priv + KCAPI_PARAM_SZ, key->id, keySz);
        }
        else
    #endif
        {
            ret = wc_export_int(wc_ecc_key_get_priv(key), priv + KCAPI_PARAM_SZ,
                &keySz, keySz, WC_TYPE_UNSIGNED_BIN);
        }
    }
    if (ret == 0) {
        /* call with NULL to so KCAPI treats incoming data as hash */
        ret = kcapi_akcipher_setkey(key->handle, NULL, 0);
        if (ret >= 0) {
            ret = kcapi_akcipher_setkey(key->handle, priv, KCAPI_PARAM_SZ + keySz);
            if (ret >= 0) {
                ret = 0;
            }
        }
    }

    return ret;
}

int KcapiEcc_Sign(ecc_key* key, const byte* hash, word32 hashLen, byte* sig,
                  word32 sigLen)
{
    int ret = 0;
    byte* buf_aligned = NULL;
    int handleInit = 0;
    word32 keySz;
    word32 maxBufSz;
#ifndef KCAPI_USE_XMALLOC
    size_t pageSz = (size_t)sysconf(_SC_PAGESIZE);
#endif

    if (key == NULL || key->dp == NULL) {
        ret = BAD_FUNC_ARG;
    }

    if (ret == 0 && key->handle == NULL) {
        ret = kcapi_akcipher_init(&key->handle, WC_NAME_ECDSA, 0);
        if (ret != 0) {
            WOLFSSL_MSG("KcapiEcc_Sign: Failed to initialize");
        }
        if (ret == 0) {
            handleInit = 1;
            ret = KcapiEcc_SetPrivKey(key);
        }
    }

    if (ret == 0) {
        /* make sure signature output is large enough */
        keySz = key->dp->size;
        if (sigLen < keySz*2) {
            ret = BUFFER_E;
        }
    }
    if (ret == 0) {
        maxBufSz = (hashLen > keySz * 2) ? hashLen : (keySz * 2);
    #ifdef KCAPI_USE_XMALLOC
        buf_aligned = (unsigned char*)XMALLOC(maxBufSz, key->heap,
            DYNAMIC_TYPE_TMP_BUFFER);
        if (buf_aligned == NULL) {
            ret = MEMORY_E;
        }
    #else
        ret = posix_memalign((void*)&buf_aligned, pageSz, maxBufSz);
        if (ret != 0) {
            ret = MEMORY_E;
        }
    #endif
    }
    if (ret == 0) {
        XMEMCPY(buf_aligned, hash, hashLen);

        ret = (int)kcapi_akcipher_sign(key->handle, buf_aligned, hashLen,
            buf_aligned, keySz * 2, KCAPI_ACCESS_HEURISTIC);
        if (ret >= 0) {
            XMEMCPY(sig, buf_aligned, ret);
            ret = 0; /* mark success */
        }
    }

    if (buf_aligned != NULL) {
    #ifdef KCAPI_USE_XMALLOC
        XFREE(buf_aligned, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
    #else
        free(buf_aligned);
    #endif
    }

    if (handleInit) {
        kcapi_kpp_destroy(key->handle);
        key->handle = NULL;
    }

    return ret;
}
#endif


#ifdef HAVE_ECC_VERIFY
static int KcapiEcc_SetPubKey(ecc_key* key)
{
    int ret;
    word32 kcapiCurveId;
    word32 keySz = key->dp->size;
    byte pub[KCAPI_PARAM_SZ + (MAX_ECC_BYTES * 2)];
    int pubLen;

    ret = KcapiEcc_CurveId(key->dp->id, &kcapiCurveId);
    if (ret == 0) {
        pub[0] = ECDSA_KEY_VERSION;
        pub[1] = kcapiCurveId;
        XMEMCPY(&pub[KCAPI_PARAM_SZ], key->pubkey_raw, keySz * 2);
        pubLen = KCAPI_PARAM_SZ + (keySz * 2);

        /* call with NULL to so KCAPI treats incoming data as hash */
        ret = kcapi_akcipher_setpubkey(key->handle, NULL, 0);
        if (ret >= 0) {
            ret = kcapi_akcipher_setpubkey(key->handle, pub, pubLen);
            if (ret >= 0) {
                ret = 0;
            }
        }
    }

    return ret;
}

int KcapiEcc_Verify(ecc_key* key, const byte* hash, word32 hashLen, byte* sig,
                    word32 sigLen)
{
    int ret = 0;
    byte* buf_aligned = NULL;
    int handleInit = 0;
    word32 keySz = 0;
#ifndef KCAPI_USE_XMALLOC
    size_t pageSz = (size_t)sysconf(_SC_PAGESIZE);
#endif

    if (key == NULL || key->dp == NULL) {
        ret = BAD_FUNC_ARG;
    }

    if (ret == 0 && key->handle == NULL) {
        ret = kcapi_akcipher_init(&key->handle, WC_NAME_ECDSA, 0);
        if (ret != 0) {
            WOLFSSL_MSG("KcapiEcc_Verify: Failed to initialize");
        }
        if (ret == 0) {
            handleInit = 1;
            ret = KcapiEcc_SetPubKey(key);
        }
    }
    if (ret == 0) {
        keySz = key->dp->size;
    #ifdef KCAPI_USE_XMALLOC
        buf_aligned = (byte*)XMALLOC(sigLen + hashLen, key->heap,
            DYNAMIC_TYPE_TMP_BUFFER);
        if (buf_aligned == NULL) {
            ret = MEMORY_E;
        }
    #else
        ret = posix_memalign((void*)&buf_aligned, pageSz, sigLen + hashLen);
        if (ret != 0) {
            ret = MEMORY_E;
        }
    #endif
    }
    if (ret == 0) {
        XMEMCPY(buf_aligned, sig, sigLen);
        XMEMCPY(buf_aligned + sigLen, hash, hashLen);

        ret = (int)kcapi_akcipher_verify(key->handle, buf_aligned,
            sigLen + hashLen, buf_aligned, keySz * 2,
            KCAPI_ACCESS_HEURISTIC);
        if (ret >= 0) {
            /* verify output in buf_aligned is not used */
            ret = 0;
        }
    }

    if (buf_aligned != NULL) {
    #ifdef KCAPI_USE_XMALLOC
        XFREE(buf_aligned, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
    #else
        free(buf_aligned);
    #endif
    }

    if (handleInit) {
        kcapi_kpp_destroy(key->handle);
        key->handle = NULL;
    }
    return ret;
}
#endif

#endif /* WOLFSSL_KCAPI_ECC && HAVE_ECC */
