/* ext_xmss.c
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
#include <wolfssl/wolfcrypt/sha256.h>

#ifdef WOLFSSL_HAVE_XMSS
#include <wolfssl/wolfcrypt/ext_xmss.h>

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

#include <xmss_callbacks.h>

#ifndef WOLFSSL_XMSS_VERIFY_ONLY
static THREAD_LS_T WC_RNG * xmssRng = NULL;

/* RNG callback used by xmss.
 * */
static int rng_cb(void * output, size_t length)
{
    int ret = 0;

    if (output == NULL || xmssRng == NULL) {
        return -1;
    }

    if (length == 0) {
        return 0;
    }

    ret = wc_RNG_GenerateBlock(xmssRng, (byte *)output, (word32)length);

    if (ret) {
        WOLFSSL_MSG("error: XMSS rng_cb failed");
        return -1;
    }

    return 0;
}
#endif /* ifndef WOLFSSL_XMSS_VERIFY_ONLY */

/* SHA256 callback used by XMSS.
 * */
static int sha256_cb(const unsigned char *in, unsigned long long inlen,
                     unsigned char *out)
{
    wc_Sha256 sha;

    if (wc_InitSha256_ex(&sha, NULL, INVALID_DEVID) != 0) {
        WOLFSSL_MSG("SHA256 Init failed");
        return -1;
    }

    if (wc_Sha256Update(&sha, in, (word32) inlen) != 0) {
        WOLFSSL_MSG("SHA256 Update failed");
        return -1;
    }

    if (wc_Sha256Final(&sha, out) != 0) {
        WOLFSSL_MSG("SHA256 Final failed");
        wc_Sha256Free(&sha);
        return -1;
    }
    wc_Sha256Free(&sha);

    return 0;
}

/* Init an XMSS key.
 *
 * Call this before setting the parms of an XMSS key.
 *
 *  key         [in]  The XMSS key to init.
 *  heap        [in]  Unused.
 *  devId       [in]  Unused.
 *
 *  returns     0 on success.
 *  returns     BAD_FUNC_ARG when a parameter is NULL.
 * */
int wc_XmssKey_Init(XmssKey * key, void * heap, int devId)
{
    if (key == NULL) {
        return BAD_FUNC_ARG;
    }

    (void) heap;
    (void) devId;

    ForceZero(key, sizeof(XmssKey));

#ifndef WOLFSSL_XMSS_VERIFY_ONLY
    key->sk = NULL;
    key->sk_len = 0;
    key->write_private_key = NULL;
    key->read_private_key = NULL;
    key->context = NULL;
#endif /* ifndef WOLFSSL_XMSS_VERIFY_ONLY */
    key->state = WC_XMSS_STATE_INITED;

    return 0;
}

/* Sets the XMSS key parameters, given an OID.
 *
 * Note: XMSS and XMSS^MT parameter sets do have overlapping
 * OIDs, therefore is_xmssmt is necessary to toggle.
 *
 *  key         [in]  The XMSS key to set.
 *  OID         [in]  The XMSS parameter set OID.
 *  is_xmssmt   [in]  1 The OID is assumed to be XMSS^MT.
 *                    0 The OID is assumed to be XMSS.
 *
 *  returns     0 on success.
 *  returns     BAD_FUNC_ARG when a parameter is NULL.
 *  returns     -1 on parse failure.
 * */
static int wc_XmssKey_SetOid(XmssKey * key, uint32_t oid, int is_xmssmt)
{
    int ret = 0;

    if (key == NULL || oid == 0) {
        return BAD_FUNC_ARG;
    }

    /* Parse the OID and load the XMSS params structure. */
    if (is_xmssmt) {
        ret = xmssmt_parse_oid(&key->params, oid);
    }
    else {
        ret = xmss_parse_oid(&key->params, oid);
    }

    if (ret != 0) {
        WOLFSSL_MSG("error: XMSS parse oid failed");
        return -1;
    }

    /* Finally, sanity check that this is a supported parameter set.
     *
     * We are only supporting XMSS/XMSS^MT with SHA256 parameter sets
     * that NIST SP 800-208 has standardized. See patched xmss-reference
     * params.h for the defines. */
    if (key->params.func != XMSS_SHA2 ||
        key->params.n != XMSS_SHA256_N ||
        key->params.padding_len != XMSS_SHA256_PADDING_LEN ||
        key->params.wots_w != 16 ||
        key->params.wots_len != XMSS_SHA256_WOTS_LEN) {
        WOLFSSL_MSG("error: unsupported XMSS/XMSS^MT parameter set");
        return -1;
    }

    ret = xmss_set_sha_cb(sha256_cb);
    if (ret != 0) {
        WOLFSSL_MSG("error: xmss_set_sha_cb failed");
        return -1;
    }

#ifndef WOLFSSL_XMSS_VERIFY_ONLY
    ret = xmss_set_rng_cb(rng_cb);
    if (ret != 0) {
        WOLFSSL_MSG("error: xmss_set_rng_cb failed");
        return -1;
    }
#endif

    key->oid = oid;
    key->is_xmssmt = is_xmssmt;
    key->state = WC_XMSS_STATE_PARMSET;

    return 0;
}

/* Set the XMSS key parameter string.
 *
 * The input string must be one of the supported parm set names in
 * the "Name" section from the table in wolfssl/wolfcrypt/xmss.h,
 * e.g. "XMSS-SHA2_10_256" or "XMSSMT-SHA2_20/4_256".
 *
 *  key         [in]  The XMSS key to set.
 *  str         [in]  The XMSS/XMSS^MT parameter string.
 *
 *  returns     0 on success.
 *  returns     BAD_FUNC_ARG when a parameter is NULL.
 *  returns     -1 on failure.
 * */
int wc_XmssKey_SetParamStr(XmssKey * key, const char * str)
{
    int      ret = 0;
    uint32_t oid = 0;
    int      is_xmssmt = 0;

    if (key == NULL || str == NULL) {
        return BAD_FUNC_ARG;
    }

    if (key->state != WC_XMSS_STATE_INITED) {
        WOLFSSL_MSG("error: XMSS key needs init");
        return BAD_FUNC_ARG;
    }

    switch(XSTRLEN(str)) {
    case XMSS_NAME_LEN:
        is_xmssmt = 0;
        break;
    case XMSSMT_NAME_MIN_LEN:
    case XMSSMT_NAME_MAX_LEN:
        is_xmssmt = 1;
        break;
    default:
        WOLFSSL_MSG("error: XMSS param str invalid length");
        return BAD_FUNC_ARG;
    }

    /* Convert XMSS param string to OID. */
    if (is_xmssmt) {
        ret = xmssmt_str_to_oid(&oid, str);
    }
    else {
        ret = xmss_str_to_oid(&oid, str);
    }

    if (ret != 0) {
        WOLFSSL_MSG("error: xmssmt_str_to_oid failed");
        return -1;
    }

    return wc_XmssKey_SetOid(key, oid, is_xmssmt);
}

/* Force zeros and frees the XMSS key from memory.
 *
 * This does not touch the private key saved to non-volatile storage.
 *
 * This is the only function that frees the key->sk array.
 *
 *  key         [in]  The XMSS key.
 *
 *  returns     void
 * */
void wc_XmssKey_Free(XmssKey* key)
{
    if (key == NULL) {
        return;
    }

#ifndef WOLFSSL_XMSS_VERIFY_ONLY
    if (key->sk != NULL) {
        ForceZero(key->sk, key->sk_len);
        XFREE(key->sk, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        key->sk = NULL;
        key->sk_len = 0;
    }
#endif /* ifndef WOLFSSL_XMSS_VERIFY_ONLY */

    ForceZero(key, sizeof(XmssKey));

    key->state = WC_XMSS_STATE_FREED;

    return;
}

#ifndef WOLFSSL_XMSS_VERIFY_ONLY
/* Sets the XMSS write private key callback.
 *
 * The callback must be able to write/update the private key to
 * non-volatile storage.
 *
 *  key         [in]  The XMSS key.
 *  write_cb    [in]  The write private key callback.
 *
 *  returns     0 on success.
 *  returns     BAD_FUNC_ARG when a parameter is NULL.
 *  returns     -1 on failure.
 * */
int wc_XmssKey_SetWriteCb(XmssKey * key, write_private_key_cb write_cb)
{
    if (key == NULL || write_cb == NULL) {
        return BAD_FUNC_ARG;
    }

    /* Changing the write callback of an already working key is forbidden. */
    if (key->state == WC_XMSS_STATE_OK) {
        WOLFSSL_MSG("error: wc_XmssKey_SetWriteCb: key in use");
        return -1;
    }

    key->write_private_key = write_cb;

    return 0;
}

/* Sets the XMSS read private key callback.
 *
 * The callback must be able to read the private key from
 * non-volatile storage.
 *
 *  key         [in]  The XMSS key.
 *  read_cb     [in]  The read private key callback.
 *
 *  returns     0 on success.
 *  returns     BAD_FUNC_ARG when a parameter is NULL.
 *  returns     -1 on failure.
 * */
int wc_XmssKey_SetReadCb(XmssKey * key, read_private_key_cb read_cb)
{
    if (key == NULL || read_cb == NULL) {
        return BAD_FUNC_ARG;
    }

    /* Changing the read callback of an already working key is forbidden. */
    if (key->state == WC_XMSS_STATE_OK) {
        WOLFSSL_MSG("error: wc_XmssKey_SetReadCb: key in use");
        return -1;
    }

    key->read_private_key = read_cb;

    return 0;
}

/* Sets the XMSS context to be used by write and read callbacks.
 *
 * E.g. this could be a filename if the callbacks write/read to file.
 *
 *  key         [in]  The XMSS key.
 *  context     [in]  The context pointer.
 *
 *  returns     0 on success.
 *  returns     BAD_FUNC_ARG when a parameter is NULL.
 *  returns     -1 on failure.
 * */
int wc_XmssKey_SetContext(XmssKey * key, void * context)
{
    if (key == NULL || context == NULL) {
        return BAD_FUNC_ARG;
    }

    /* Setting context of an already working key is forbidden. */
    if (key->state == WC_XMSS_STATE_OK) {
        WOLFSSL_MSG("error: wc_XmssKey_SetContext: key in use");
        return -1;
    }

    key->context = context;

    return 0;
}


/* Allocates the XMSS secret key (sk) array.
 *
 * The XMSS/XMSS^MT secret key length is a function of the
 * parameters, and can't be allocated until the param string
 * has been set with SetParamStr.
 *
 * This is only called by MakeKey() and Reload().
 *
 * Note: the XMSS sk array is force zeroed after every use.
 *
 *  key         [in]  The XMSS key.
 *
 *  returns     0 on success.
 *  returns     BAD_FUNC_ARG when a parameter is NULL.
 *  returns     -1 on failure.
 * */
static int wc_XmssKey_AllocSk(XmssKey* key)
{
    int ret = 0;

    if (key == NULL) {
        return BAD_FUNC_ARG;
    }

    if (key->sk != NULL) {
        WOLFSSL_MSG("error: XMSS secret key already exists");
        return -1;
    }

    /* The XMSS/XMSS^MT secret key length is a function of the
     * parameters. Therefore can't allocate this until param
     * string has been set. */
    ret = wc_XmssKey_GetPrivLen(key, &key->sk_len);

    if (ret != 0 || key->sk_len <= 0) {
        WOLFSSL_MSG("error: wc_XmssKey_GetPrivLen failed");
        return -1;
    }

    key->sk = (unsigned char *)XMALLOC(key->sk_len, NULL,
                                       DYNAMIC_TYPE_TMP_BUFFER);

    if (key->sk == NULL) {
        WOLFSSL_MSG("error: malloc XMSS key->sk failed");
        return -1;
    }

    ForceZero(key->sk, key->sk_len);

    return 0;
}

/* Make the XMSS/XMSS^MT private/public key pair. The key must have its parameters
 * set before calling this.
 *
 * Write/read callbacks, and context data, must be set prior.
 * Key must have parameters set.
 *
 * This function and Reload() are the only functions that allocate
 * key->sk array. wc_XmssKey_FreeKey is the only function that
 * deallocates key->sk.
 *
 *  key         [in]  The XMSS key to make.
 *  rng         [in]  Initialized WC_RNG pointer.
 *
 *  returns     0 on success.
 *  returns     BAD_FUNC_ARG when a parameter is NULL.
 *  returns     -1 on verify fail.
 * */
int wc_XmssKey_MakeKey(XmssKey* key, WC_RNG * rng)
{
    int            ret = 0;
    enum wc_XmssRc cb_rc = WC_XMSS_RC_NONE;

    if (key == NULL || rng == NULL) {
        return BAD_FUNC_ARG;
    }

    if (key->state != WC_XMSS_STATE_PARMSET) {
        WOLFSSL_MSG("error: XmssKey not ready for generation");
        return -1;
    }

    if (key->write_private_key == NULL || key->read_private_key == NULL) {
        WOLFSSL_MSG("error: XmssKey write/read callbacks are not set");
        return -1;
    }

    if (key->context == NULL) {
        WOLFSSL_MSG("error: XmssKey context is not set");
        return -1;
    }

    /* Allocate sk array. */
    ret = wc_XmssKey_AllocSk(key);

    if (ret != 0) {
        return ret;
    }

    xmssRng = rng;

    /* Finally make the secret public key pair. Immediately write it to NV
     * storage and then clear from memory. */
    if (key->is_xmssmt) {
        ret = xmssmt_keypair(key->pk, key->sk, key->oid);
    }
    else {
        ret = xmss_keypair(key->pk, key->sk, key->oid);
    }

    if (ret == 0) {
        cb_rc = key->write_private_key(key->sk, key->sk_len, key->context);
    }

    ForceZero(key->sk, key->sk_len);

    if (ret != 0) {
        WOLFSSL_MSG("error: XMSS keypair failed");
        key->state = WC_XMSS_STATE_BAD;
        return -1;
    }

    if (cb_rc != WC_XMSS_RC_SAVED_TO_NV_MEMORY) {
        WOLFSSL_MSG("error: XMSS write to NV storage failed");
        key->state = WC_XMSS_STATE_BAD;
        return -1;
    }

    key->state = WC_XMSS_STATE_OK;

    return 0;
}

/* This function allocates the secret key buffer, and does a
 * quick sanity check to verify the secret key is readable
 * from NV storage, and then force zeros the key from memory.
 *
 * On success it sets the key state to OK.
 *
 * Use this function to resume signing with an already existing
 * XMSS key pair.
 *
 * Write/read callbacks, and context data, must be set prior.
 * Key must have parameters set.
 *
 * Returns 0 on success.
 *
 * This function and MakeKey are the only functions that allocate
 * key->sk array. wc_XmssKey_FreeKey is the only function that
 * deallocates key->sk.
 *
 *  key         [in]      XMSS key to load.
 *
 *  returns     0 on success.
 *  returns     BAD_FUNC_ARG when a parameter is NULL.
 *  returns     -1 on load fail.
 * */
int wc_XmssKey_Reload(XmssKey * key)
{
    int            ret = 0;
    enum wc_XmssRc cb_rc = WC_XMSS_RC_NONE;

    if (key == NULL) {
        return BAD_FUNC_ARG;
    }

    if (key->state != WC_XMSS_STATE_PARMSET) {
        WOLFSSL_MSG("error: XmssKey not ready for reload");
        return -1;
    }

    if (key->write_private_key == NULL || key->read_private_key == NULL) {
        WOLFSSL_MSG("error: XmssKey write/read callbacks are not set");
        return -1;
    }

    if (key->context == NULL) {
        WOLFSSL_MSG("error: XmssKey context is not set");
        return -1;
    }

    /* Allocate sk array. */
    ret = wc_XmssKey_AllocSk(key);

    if (ret != 0) {
        return ret;
    }

    /* Read the current secret key from NV storage. Force clear it
     * immediately. This is just to sanity check the secret key
     * is readable from permanent storage. */
    cb_rc = key->read_private_key(key->sk, key->sk_len, key->context);
    ForceZero(key->sk, key->sk_len);

    if (cb_rc != WC_XMSS_RC_READ_TO_MEMORY) {
        WOLFSSL_MSG("error: XMSS read from NV storage failed");
        key->state = WC_XMSS_STATE_BAD;
        return -1;
    }

    key->state = WC_XMSS_STATE_OK;

    return 0;
}

/* Gets the XMSS/XMSS^MT private key length.
 *
 * Parameters must be set before calling this, as the key size (sk_bytes)
 * is a function of the parameters.
 *
 * Note: the XMSS/XMSS^MT private key format is implementation specific,
 * and not standardized. Interoperability of XMSS private keys should
 * not be expected.
 *
 *  key         [in]      The XMSS key.
 *  len         [out]     The length of the private key in bytes.
 *
 *  returns     0 on success.
 *  returns     BAD_FUNC_ARG when a parameter is NULL.
 *  returns     -1 on sign fail.
 * */
int wc_XmssKey_GetPrivLen(const XmssKey * key, word32 * len)
{
    if (key == NULL || len == NULL) {
        return BAD_FUNC_ARG;
    }

    if (key->state != WC_XMSS_STATE_OK && key->state != WC_XMSS_STATE_PARMSET) {
        /* params.sk_bytes not set yet. */
        return -1;
    }

    *len = XMSS_OID_LEN + (word32) key->params.sk_bytes;

    return 0;
}

/* Signs the message using the XMSS secret key, and
 * updates the secret key on NV storage.
 *
 * Both operations must succeed to be considered
 * successful.
 *
 * On success:  sets key state to WC_XMSS_STATE_OK.
 * On failure:  sets key state to WC_XMSS_STATE_BAD
 *
 * If no signatures are left, sets state to WC_XMSS_STATE_NOSIGS.
 */
static void wc_XmssKey_SignUpdate(XmssKey* key, byte * sig, word32 * sigLen,
    const byte * msg, int msgLen)
{
    int                ret = -1;
    unsigned long long len = *sigLen;
    enum wc_XmssRc     cb_rc = WC_XMSS_RC_NONE;

    /* Set the key state to bad by default. State is presumed bad
     * unless a correct sign and update operation happen together. */
    key->state = WC_XMSS_STATE_BAD;
    *sigLen = 0;

    /* Read the current secret key from NV storage.*/
    cb_rc = key->read_private_key(key->sk, key->sk_len, key->context);

    if (cb_rc == WC_XMSS_RC_READ_TO_MEMORY) {
        /* Read was good. Now sign and update the secret key in memory. */
        if (key->is_xmssmt) {
            ret = xmssmt_sign(key->sk, sig, &len, msg, msgLen);
        }
        else {
            ret = xmss_sign(key->sk, sig, &len, msg, msgLen);
        }

        if (ret == 0 && len == key->params.sig_bytes) {
            /* The signature succeeded. key->sk is now updated and must be
             * committed to NV storage. */
            cb_rc = key->write_private_key(key->sk, key->sk_len, key->context);

            if (cb_rc == WC_XMSS_RC_SAVED_TO_NV_MEMORY) {
                /* key->sk was successfully committed to NV storage. Set the
                 * key state to OK, and set the sigLen. */
                key->state = WC_XMSS_STATE_OK;
                *sigLen = (word32) len;
            }
            else {
                /* Write to NV storage failed. Erase the signature from
                 * memory. */
                ForceZero(sig, key->params.sig_bytes);
                WOLFSSL_MSG("error: XMSS write_private_key failed");
            }
        }
        else if (ret == -2) {
            /* Signature space exhausted. */
            key->state = WC_XMSS_STATE_NOSIGS;
            WOLFSSL_MSG("error: no XMSS signatures remaining");
        }
        else {
            /* Something failed or inconsistent in signature. Erase the
             * signature just to be safe. */
            ForceZero(sig, key->params.sig_bytes);
            WOLFSSL_MSG("error: XMSS sign failed");
        }
    }
    else {
        /* Read from NV storage failed. */
        WOLFSSL_MSG("error: XMSS read_private_key failed");
    }

    /* Force zero the secret key from memory always. */
    ForceZero(key->sk, key->sk_len);

    return;
}

/* Sign the message using the XMSS secret key.
 *
 *  key         [in]      XMSS key to use to sign.
 *  sig         [in]      Buffer to write signature into.
 *  sigLen      [in/out]  On in, size of buffer.
 *                        On out, the length of the signature in bytes.
 *  msg         [in]      Message to sign.
 *  msgLen      [in]      Length of the message in bytes.
 *
 *  returns     0 on success.
 *  returns     -1 on sign fail.
 *  returns     BAD_FUNC_ARG when a parameter is NULL.
 *  returns     BUFFER_E when sigLen is too small.
 */
int wc_XmssKey_Sign(XmssKey* key, byte * sig, word32 * sigLen, const byte * msg,
    int msgLen)
{
    if (key == NULL || sig == NULL || sigLen == NULL || msg == NULL) {
        return BAD_FUNC_ARG;
    }

    if (msgLen <= 0) {
        return BAD_FUNC_ARG;
    }

    if (*sigLen < key->params.sig_bytes) {
        /* Signature buffer too small. */
        WOLFSSL_MSG("error: XMSS sig buffer too small");
        return BUFFER_E;
    }

    if (key->state == WC_XMSS_STATE_NOSIGS) {
        WOLFSSL_MSG("error: XMSS signatures exhausted");
        return -1;
    }
    else if (key->state != WC_XMSS_STATE_OK) {
       /* The key had an error the last time it was used, and we
        * can't guarantee its state. */
        WOLFSSL_MSG("error: can't sign, XMSS key not in good state");
        return -1;
    }

    if (key->write_private_key == NULL || key->read_private_key == NULL) {
        WOLFSSL_MSG("error: XmssKey write/read callbacks are not set");
        return -1;
    }

    if (key->context == NULL) {
        WOLFSSL_MSG("error: XmssKey context is not set");
        return -1;
    }

    /* Finally, sign and update the secret key. */
    wc_XmssKey_SignUpdate(key, sig, sigLen, msg, msgLen);

    return (key->state == WC_XMSS_STATE_OK) ? 0 : -1;
}
#endif /* ifndef WOLFSSL_XMSS_VERIFY_ONLY*/

/* Get the XMSS/XMSS^MT public key length. The public key
 * is static in size and does not depend on parameters,
 * other than the choice of SHA256 as hashing function.
 *
 *  key         [in]      The XMSS key.
 *  len         [out]     The length of the public key.
 *
 *  returns     0 on success.
 *  returns     BAD_FUNC_ARG when a parameter is NULL.
 */
int wc_XmssKey_GetPubLen(const XmssKey * key, word32 * len)
{
    if (key == NULL || len == NULL) {
        return BAD_FUNC_ARG;
    }

    *len = XMSS_SHA256_PUBLEN;

    return 0;
}

/* Export a generated public key and parameter set from one XmssKey
 * to another. Use this to prepare a signature verification XmssKey
 * that is pub only.
 *
 *  keyDst      [out]    Destination key for copy.
 *  keySrc      [in]     Source key for copy.
 *
 *  returns     0 on success.
 *  returns     BAD_FUNC_ARG when a parameter is NULL.
 * */
int wc_XmssKey_ExportPub(XmssKey * keyDst, const XmssKey * keySrc)
{
    if (keyDst == NULL || keySrc == NULL) {
        return BAD_FUNC_ARG;
    }

    ForceZero(keyDst, sizeof(XmssKey));

    XMEMCPY(keyDst->pk, keySrc->pk, sizeof(keySrc->pk));

    keyDst->oid = keySrc->oid;
    keyDst->is_xmssmt = keySrc->is_xmssmt;

    /* Mark keyDst as verify only, to prevent misuse. */
    keyDst->state = WC_XMSS_STATE_VERIFYONLY;

    return 0;
}

/* Exports the raw XMSS public key buffer from key to out buffer.
 * The out buffer should be large enough to hold the public key, and
 * outLen should indicate the size of the buffer.
 *
 *  key         [in]      XMSS key.
 *  out         [out]     Array holding public key.
 *  outLen      [in/out]  On in, size of buffer.
 *                        On out, the length of the public key.
 *
 *  returns     0 on success.
 *  returns     -1 on failure.
 *  returns     BAD_FUNC_ARG when a parameter is NULL.
 *  returns     BUFFER_E if array is too small.
 * */
int wc_XmssKey_ExportPubRaw(const XmssKey * key, byte * out, word32 * outLen)
{
    int    ret = 0;
    word32 pubLen = 0;

    if (key == NULL || out == NULL || outLen == NULL) {
        return BAD_FUNC_ARG;
    }

    ret = wc_XmssKey_GetPubLen(key, &pubLen);

    if (ret != 0) {
        WOLFSSL_MSG("error: wc_XmssKey_GetPubLen failed");
        return -1;
    }

    if (*outLen < pubLen) {
        return BUFFER_E;
    }

    XMEMCPY(out, key->pk, pubLen);
    *outLen = pubLen;

    return 0;
}

/* Imports a raw public key buffer from in array to XmssKey key.
 *
 * The XMSS parameters must be set first with wc_XmssKey_SetParamStr,
 * and inLen must match the length returned by wc_XmssKey_GetPubLen.
 *
 *  key         [in]      XMSS key.
 *  in          [in]      Array holding public key.
 *  inLen       [in]      Length of array in bytes.
 *
 *  returns     0 on success.
 *  returns     BAD_FUNC_ARG when a parameter is NULL.
 *  returns     BUFFER_E if array is incorrect size.
 *  returns     -1 on failure.
 * */
int wc_XmssKey_ImportPubRaw(XmssKey * key, const byte * in, word32 inLen)
{
    int    ret = 0;
    word32 pubLen = 0;

    if (key == NULL || in == NULL) {
        return BAD_FUNC_ARG;
    }

    if (key->state != WC_XMSS_STATE_PARMSET) {
        /* XMSS key not ready for import. Param str must be set first. */
        WOLFSSL_MSG("error: XMSS key not ready for import");
        return -1;
    }

    ret = wc_XmssKey_GetPubLen(key, &pubLen);

    if (ret != 0) {
        WOLFSSL_MSG("error: wc_XmssKey_GetPubLen failed");
        return -1;
    }

    if (inLen != pubLen) {
        /* Something inconsistent. Parameters weren't set, or input
         * pub key is wrong.*/
        return BUFFER_E;
    }

    XMEMCPY(key->pk, in, pubLen);

    key->state = WC_XMSS_STATE_VERIFYONLY;

    return 0;
}

/* Gets the XMSS/XMSS^MT signature length.
 *
 * Parameters must be set before calling this, as the signature size
 * is a function of the parameters.
 *
 * Note: call this before wc_XmssKey_Sign or Verify so you know the
 * length of the required signature buffer.
 *
 *  key         [in]      XMSS key to use to sign.
 *  len         [out]     The length of the signature in bytes.
 *
 *  returns     0 on success.
 *  returns     BAD_FUNC_ARG when a parameter is NULL.
 *  returns     -1 on sign fail.
 * */
int wc_XmssKey_GetSigLen(const XmssKey * key, word32 * len)
{
    if (key == NULL || len == NULL) {
        return BAD_FUNC_ARG;
    }

    if (key->state != WC_XMSS_STATE_OK && key->state != WC_XMSS_STATE_PARMSET) {
        return -1;
    }

    *len = key->params.sig_bytes;

    return 0;
}

/* Verify the signature using the XMSS public key.
 *
 * Requires that XMSS parameters have been set with
 * wc_XmssKey_SetParamStr, and that a public key is available
 * from importing or MakeKey().
 *
 * Call wc_XmssKey_GetSigLen() before this function to determine
 * length of the signature buffer.
 *
 *  key         [in]  XMSS key to use to verify.
 *  sig         [in]  Signature to verify.
 *  sigLen      [in]  Size of signature in bytes.
 *  msg         [in]  Message to verify.
 *  msgLen      [in]  Length of the message in bytes.
 *
 *  returns     0 on success.
 *  returns     -1 on verify fail.
 *  returns     BAD_FUNC_ARG when a parameter is NULL.
 *  returns     BUFFER_E when sigLen is too small.
 */
int wc_XmssKey_Verify(XmssKey * key, const byte * sig, word32 sigLen,
    const byte * msg, int msgLen)
{
    int                ret = 0;
    unsigned long long msg_len = 0;

    msg_len = msgLen;

    if (key == NULL || sig == NULL || msg == NULL) {
        return BAD_FUNC_ARG;
    }

    if (sigLen < key->params.sig_bytes) {
        /* Signature buffer too small. */
        return BUFFER_E;
    }

    if (key->state != WC_XMSS_STATE_OK &&
        key->state != WC_XMSS_STATE_VERIFYONLY) {
        /* XMSS key not ready for verification. Param str must be
         * set first, and Reload() called. */
        WOLFSSL_MSG("error: XMSS key not ready for verification");
        return -1;
    }

    if (key->is_xmssmt) {
        ret = xmssmt_sign_open(msg, &msg_len, sig, sigLen, key->pk);
    }
    else {
        ret = xmss_sign_open(msg, &msg_len, sig, sigLen, key->pk);
    }

    if (ret != 0 || (int) msg_len != msgLen) {
        WOLFSSL_MSG("error: XMSS verify failed");
        return -1;
    }

    return ret;
}

#endif /* WOLFSSL_HAVE_XMSS */
