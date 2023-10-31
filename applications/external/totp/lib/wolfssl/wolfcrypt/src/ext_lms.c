/* ext_lms.c
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

#ifdef WOLFSSL_HAVE_LMS
#include <wolfssl/wolfcrypt/ext_lms.h>

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

/* If built against hss_lib_thread.a, the hash-sigs lib will spawn
 * worker threads to parallelize cpu intensive tasks. This will mainly
 * speedup key generation and signing, and to a lesser extent
 * verifying for larger levels values.
 *
 * Their default max is 16 worker threads, but can be capped with
 * hss_extra_info_set_threads(). To be safe we are capping at 4 here.
 * */
#define EXT_LMS_MAX_THREADS (4)

/* The hash-sigs hss_generate_private_key API requires a generate_random
 * callback that only has output and length args. The RNG struct must be global
 * to the function. Maybe there should be a wc_LmsKey_SetRngCb. */
static THREAD_LS_T WC_RNG * LmsRng = NULL;

static bool LmsGenerateRand(void * output, size_t length)
{
    int ret = 0;

    if (output == NULL || LmsRng == NULL) {
        return false;
    }

    if (length == 0) {
        return true;
    }

    ret = wc_RNG_GenerateBlock(LmsRng, output, (word32) length);

    if (ret) {
        WOLFSSL_MSG("error: LmsGenerateRand failed");
        return false;
    }

    return true;
}

/* Write callback passed into hash-sigs hss lib.
 *
 * Returns true on success. */
static bool LmsWritePrivKey(unsigned char *private_key,
                            size_t len_private_key, void *lmsKey)
{
    LmsKey *      key = (LmsKey *) lmsKey;
    enum wc_LmsRc ret = WC_LMS_RC_NONE;

    if (private_key == NULL || key == NULL || len_private_key <= 0) {
        WOLFSSL_MSG("error: LmsWritePrivKey: invalid args");
        return false;
    }

    if (key->state != WC_LMS_STATE_PARMSET && key->state != WC_LMS_STATE_OK) {
       /* The LmsKey is not ready for writing. */
        WOLFSSL_MSG("error: LmsWritePrivKey: LMS key not in writeable state");
        return false;
    }

    if (key->write_private_key == NULL) {
        WOLFSSL_MSG("error: LmsWritePrivKey: LMS key write callback not set");
        key->state = WC_LMS_STATE_BAD;
        return false;
    }

    /* Use write callback that saves private key to non-volatile storage. */
    ret = key->write_private_key(private_key, len_private_key, key->context);

    if (ret != WC_LMS_RC_SAVED_TO_NV_MEMORY) {
        WOLFSSL_MSG("error: LmsKey write_private_key failed");
        WOLFSSL_MSG(wc_LmsKey_RcToStr(ret));
        key->state = WC_LMS_STATE_BAD;
        return false;
    }

    return true;
}

/* Read callback passed into hash-sigs hss lib.
 *
 * Returns true on success. */
static bool LmsReadPrivKey(unsigned char *private_key,
                           size_t len_private_key, void *lmsKey)
{
    LmsKey *      key = (LmsKey *) lmsKey;
    enum wc_LmsRc ret = WC_LMS_RC_NONE;

    if (private_key == NULL || key == NULL || len_private_key <= 0) {
        WOLFSSL_MSG("error: LmsReadPrivKey: invalid args");
        return false;
    }

    if (key->state != WC_LMS_STATE_PARMSET && key->state != WC_LMS_STATE_OK) {
       /* The LmsKey is not ready for reading. */
        WOLFSSL_MSG("error: LmsReadPrivKey: LMS key not in readable state");
        return false;
    }

    if (key->read_private_key == NULL) {
        WOLFSSL_MSG("error: LmsReadPrivKey: LMS key read callback not set");
        key->state = WC_LMS_STATE_BAD;
        return false;
    }

    /* Use read callback that reads private key from non-volatile storage. */
    ret = key->read_private_key(private_key, len_private_key, key->context);

    if (ret != WC_LMS_RC_READ_TO_MEMORY) {
        WOLFSSL_MSG("error: LmsKey read_private_key failed");
        WOLFSSL_MSG(wc_LmsKey_RcToStr(ret));
        key->state = WC_LMS_STATE_BAD;
        return false;
    }

    return true;
}

const char * wc_LmsKey_ParmToStr(enum wc_LmsParm lmsParm)
{
    switch (lmsParm) {
    case WC_LMS_PARM_NONE:
        return "LMS_NONE";

    case WC_LMS_PARM_L1_H15_W2:
        return "LMS/HSS L1_H15_W2";

    case WC_LMS_PARM_L1_H15_W4:
        return "LMS/HSS L1_H15_W4";

    case WC_LMS_PARM_L2_H10_W2:
        return "LMS/HSS L2_H10_W2";

    case WC_LMS_PARM_L2_H10_W4:
        return "LMS/HSS L2_H10_W4";

    case WC_LMS_PARM_L2_H10_W8:
        return "LMS/HSS L2_H10_W8";

    case WC_LMS_PARM_L3_H5_W2:
        return "LMS/HSS L3_H5_W2";

    case WC_LMS_PARM_L3_H5_W4:
        return "LMS/HSS L3_H5_W4";

    case WC_LMS_PARM_L3_H5_W8:
        return "LMS/HSS L3_H5_W8";

    case WC_LMS_PARM_L3_H10_W4:
        return "LMS/HSS L3_H10_W4";

    case WC_LMS_PARM_L4_H5_W8:
        return "LMS/HSS L4_H5_W8";

    default:
        WOLFSSL_MSG("error: invalid LMS parameter");
        break;
    }

    return "LMS_INVALID";
}

const char * wc_LmsKey_RcToStr(enum wc_LmsRc lmsEc)
{
    switch (lmsEc) {
    case WC_LMS_RC_NONE:
        return "LMS_RC_NONE";

    case WC_LMS_RC_BAD_ARG:
        return "LMS_RC_BAD_ARG";

    case WC_LMS_RC_WRITE_FAIL:
        return "LMS_RC_WRITE_FAIL";

    case WC_LMS_RC_READ_FAIL:
        return "LMS_RC_READ_FAIL";

    case WC_LMS_RC_SAVED_TO_NV_MEMORY:
        return "LMS_RC_SAVED_TO_NV_MEMORY";

    case WC_LMS_RC_READ_TO_MEMORY:
        return "LMS_RC_READ_TO_MEMORY";

    default:
        WOLFSSL_MSG("error: invalid LMS error code");
        break;
    }

    return "LMS_RC_INVALID";
}

/* Init an LMS key.
 *
 * Call this before setting the parms of an LMS key.
 *
 * Returns 0 on success.
 * */
int wc_LmsKey_Init(LmsKey * key, void * heap, int devId)
{
    if (key == NULL) {
        return BAD_FUNC_ARG;
    }

    (void) heap;
    (void) devId;

    ForceZero(key, sizeof(LmsKey));

    /* Set the max number of worker threads that hash-sigs can spawn. */
    hss_init_extra_info(&key->info);
    hss_extra_info_set_threads(&key->info, EXT_LMS_MAX_THREADS);

    key->working_key = NULL;
    key->write_private_key = NULL;
    key->read_private_key = NULL;
    key->context = NULL;
    key->state = WC_LMS_STATE_INITED;

    return 0;
}

/* Set the wc_LmsParm of an LMS key.
 *
 * Use this if you wish to set a key with a predefined parameter set,
 * such as WC_LMS_PARM_L2_H10_W8.
 *
 * Key must be inited before calling this.
 *
 * Returns 0 on success.
 * */
int wc_LmsKey_SetLmsParm(LmsKey * key, enum wc_LmsParm lmsParm)
{
    if (key == NULL) {
        return BAD_FUNC_ARG;
    }

    /* If NONE is passed, default to the lowest predefined set. */
    switch (lmsParm) {
    case WC_LMS_PARM_NONE:
    case WC_LMS_PARM_L1_H15_W2:
        return wc_LmsKey_SetParameters(key, 1, 15, 2);

    case WC_LMS_PARM_L1_H15_W4:
        return wc_LmsKey_SetParameters(key, 1, 15, 4);

    case WC_LMS_PARM_L2_H10_W2:
        return wc_LmsKey_SetParameters(key, 2, 10, 2);

    case WC_LMS_PARM_L2_H10_W4:
        return wc_LmsKey_SetParameters(key, 2, 10, 4);

    case WC_LMS_PARM_L2_H10_W8:
        return wc_LmsKey_SetParameters(key, 2, 10, 8);

    case WC_LMS_PARM_L3_H5_W2:
        return wc_LmsKey_SetParameters(key, 3, 5, 2);

    case WC_LMS_PARM_L3_H5_W4:
        return wc_LmsKey_SetParameters(key, 3, 5, 4);

    case WC_LMS_PARM_L3_H5_W8:
        return wc_LmsKey_SetParameters(key, 3, 5, 8);

    case WC_LMS_PARM_L3_H10_W4:
        return wc_LmsKey_SetParameters(key, 3, 10, 4);

    case WC_LMS_PARM_L4_H5_W8:
        return wc_LmsKey_SetParameters(key, 4, 5, 8);

    default:
        WOLFSSL_MSG("error: invalid LMS parameter set");
        break;
    }

    return BAD_FUNC_ARG;
}

/* Set the parameters of an LMS key.
 *
 * Use this if you wish to set specific parameters not found in the
 * wc_LmsParm predefined sets. See comments in lms.h for allowed
 * parameters.
 *
 * Key must be inited before calling this.
 *
 * Returns 0 on success.
 * */
int wc_LmsKey_SetParameters(LmsKey * key, int levels, int height,
    int winternitz)
{
    int         i = 0;
    param_set_t lm = LMS_SHA256_N32_H5;
    param_set_t ots = LMOTS_SHA256_N32_W1;

    if (key == NULL) {
        return BAD_FUNC_ARG;
    }

    if (key->state != WC_LMS_STATE_INITED) {
        WOLFSSL_MSG("error: LmsKey needs init");
        return -1;
    }

    /* Verify inputs make sense.
     *
     * Note: there does not seem to be a define for min or
     * max Winternitz integer in hash-sigs lib or RFC8554. */

    if (levels < MIN_HSS_LEVELS || levels > MAX_HSS_LEVELS) {
        WOLFSSL_MSG("error: invalid level parameter");
        return BAD_FUNC_ARG;
    }

    if (height < MIN_MERKLE_HEIGHT || height > MAX_MERKLE_HEIGHT) {
        WOLFSSL_MSG("error: invalid height parameter");
        return BAD_FUNC_ARG;
    }

    switch (height) {
    case 5:
        lm = LMS_SHA256_N32_H5;
        break;
    case 10:
        lm = LMS_SHA256_N32_H10;
        break;
    case 15:
        lm = LMS_SHA256_N32_H15;
        break;
    case 20:
        lm = LMS_SHA256_N32_H20;
        break;
    case 25:
        lm = LMS_SHA256_N32_H25;
        break;
    default:
        WOLFSSL_MSG("error: invalid height parameter");
        return BAD_FUNC_ARG;
    }

    switch (winternitz) {
    case 1:
        ots = LMOTS_SHA256_N32_W1;
        break;
    case 2:
        ots = LMOTS_SHA256_N32_W2;
        break;
    case 4:
        ots = LMOTS_SHA256_N32_W4;
        break;
    case 8:
        ots = LMOTS_SHA256_N32_W8;
        break;
    default:
        WOLFSSL_MSG("error: invalid winternitz parameter");
        return BAD_FUNC_ARG;
    }

    key->levels = levels;

    for (i = 0; i < levels; ++i) {
        key->lm_type[i] = lm;
        key->lm_ots_type[i] = ots;
    }

    /* Move the state to parms set.
     * Key is ready for MakeKey or Reload. */
    key->state = WC_LMS_STATE_PARMSET;

    return 0;
}

/* Frees the LMS key from memory.
 *
 * This does not affect the private key saved to non-volatile storage.
 * */
void wc_LmsKey_Free(LmsKey* key)
{
    if (key == NULL) {
        return;
    }

    if (key->working_key != NULL) {
        hss_free_working_key(key->working_key);
        key->working_key = NULL;
    }

    ForceZero(key, sizeof(LmsKey));

    key->state = WC_LMS_STATE_FREED;

    return;
}

/* Set the write private key callback to the LMS key structure.
 *
 * The callback must be able to write/update the private key to
 * non-volatile storage.
 *
 * Returns 0 on success.
 * */
int wc_LmsKey_SetWriteCb(LmsKey * key, write_private_key_cb write_cb)
{
    if (key == NULL || write_cb == NULL) {
        return BAD_FUNC_ARG;
    }

    /* Changing the write callback of an already working key is forbidden. */
    if (key->state == WC_LMS_STATE_OK) {
        WOLFSSL_MSG("error: wc_LmsKey_SetWriteCb: key in use");
        return -1;
    }

    key->write_private_key = write_cb;

    return 0;
}

/* Set the read private key callback to the LMS key structure.
 *
 * The callback must be able to read the private key from
 * non-volatile storage.
 *
 * Returns 0 on success.
 * */
int wc_LmsKey_SetReadCb(LmsKey * key, read_private_key_cb read_cb)
{
    if (key == NULL || read_cb == NULL) {
        return BAD_FUNC_ARG;
    }

    /* Changing the read callback of an already working key is forbidden. */
    if (key->state == WC_LMS_STATE_OK) {
        WOLFSSL_MSG("error: wc_LmsKey_SetReadCb: key in use");
        return -1;
    }

    key->read_private_key = read_cb;

    return 0;
}

/* Sets the context to be used by write and read callbacks.
 *
 * E.g. this could be a filename if the callbacks write/read to file.
 *
 * Returns 0 on success.
 * */
int wc_LmsKey_SetContext(LmsKey * key, void * context)
{
    if (key == NULL || context == NULL) {
        return BAD_FUNC_ARG;
    }

    /* Setting context of an already working key is forbidden. */
    if (key->state == WC_LMS_STATE_OK) {
        WOLFSSL_MSG("error: wc_LmsKey_SetContext: key in use");
        return -1;
    }

    key->context = context;

    return 0;
}

/* Make the LMS private/public key pair. The key must have its parameters
 * set before calling this.
 *
 * Write/read callbacks, and context data, must be set prior.
 * Key must have parameters set.
 *
 * Returns 0 on success.
 * */
int wc_LmsKey_MakeKey(LmsKey* key, WC_RNG * rng)
{
    bool result = true;

    if (key == NULL || rng == NULL) {
        return BAD_FUNC_ARG;
    }

    if (key->state != WC_LMS_STATE_PARMSET) {
        WOLFSSL_MSG("error: LmsKey not ready for generation");
        return -1;
    }

    if (key->write_private_key == NULL || key->read_private_key == NULL) {
        WOLFSSL_MSG("error: LmsKey write/read callbacks are not set");
        return -1;
    }

    if (key->context == NULL) {
        WOLFSSL_MSG("error: LmsKey context is not set");
        return -1;
    }

    LmsRng = rng;

   /* TODO: The hash-sigs lib allows you to save variable length auxiliary
    * data, which can be used to speed up key reloading when signing. The
    * aux data can be 300B - 1KB in size.
    *
    * Not implemented at the moment.
    *
    * key->aux_data_len = hss_get_aux_data_len(AUX_DATA_MAX_LEN, key->levels,
    *                                          key->lm_type,
    *                                          key->lm_ots_type);
    *
    * key->aux_data = XMALLOC(key->aux_data_len, NULL,
    *                         DYNAMIC_TYPE_TMP_BUFFER);
    */

    /* First generate the private key using the parameters and callbacks.
     * If successful, private key will be saved to non-volatile storage,
     * and the public key will be in memory. */
    result = hss_generate_private_key(LmsGenerateRand, key->levels,
                                      key->lm_type, key->lm_ots_type,
                                      LmsWritePrivKey, key,
                                      key->pub, sizeof(key->pub),
                                      NULL, 0, &key->info);

    if (!result) {
        WOLFSSL_MSG("error: hss_generate_private_key failed");
        key->state = WC_LMS_STATE_BAD;
        return -1;
    }

    /* Once generated, now we must load the private key so we have
     * an hss working key for signing operations. */
    key->working_key = hss_load_private_key(LmsReadPrivKey, key,
                                            0, NULL, 0, &key->info);

    if (key->working_key == NULL) {
        WOLFSSL_MSG("error: hss_load_private_key failed");
        key->state = WC_LMS_STATE_BAD;
        return -1;
    }

    /* This should not happen, but check just in case. */
    if (wc_LmsKey_SigsLeft(key) == 0) {
        WOLFSSL_MSG("error: generated LMS key signatures exhausted");
        key->state = WC_LMS_STATE_NOSIGS;
        return -1;
    }

    key->state = WC_LMS_STATE_OK;

    return 0;
}

/* Reload a key that has been prepared with the appropriate parms and
 * data. Use this if you wish to resume signing with an existing key.
 *
 * Write/read callbacks, and context data, must be set prior.
 * Key must have parameters set.
 *
 * Returns 0 on success. */
int wc_LmsKey_Reload(LmsKey * key)
{
    bool result = true;

    if (key == NULL) {
        return BAD_FUNC_ARG;
    }

    if (key->state != WC_LMS_STATE_PARMSET) {
        WOLFSSL_MSG("error: LmsKey not ready for reload");
        return -1;
    }

    if (key->write_private_key == NULL || key->read_private_key == NULL) {
        WOLFSSL_MSG("error: LmsKey write/read callbacks are not set");
        return -1;
    }

    if (key->context == NULL) {
        WOLFSSL_MSG("error: LmsKey context is not set");
        return -1;
    }

    key->working_key = hss_load_private_key(LmsReadPrivKey, key,
                                            0, NULL, 0, &key->info);

    if (key->working_key == NULL) {
        WOLFSSL_MSG("error: hss_load_private_key failed");
        key->state = WC_LMS_STATE_BAD;
        return -1;
    }

    result = hss_get_parameter_set(&key->levels, key->lm_type,
                                   key->lm_ots_type, LmsReadPrivKey, key);

    if (!result) {
        WOLFSSL_MSG("error: hss_get_parameter_set failed");
        key->state = WC_LMS_STATE_BAD;
        hss_free_working_key(key->working_key);
        key->working_key = NULL;
        return -1;
    }

    /* Double check the key actually has signatures left. */
    if (wc_LmsKey_SigsLeft(key) == 0) {
        WOLFSSL_MSG("error: reloaded LMS key signatures exhausted");
        key->state = WC_LMS_STATE_NOSIGS;
        return -1;
    }

    key->state = WC_LMS_STATE_OK;

    return 0;
}

/* Given a levels, height, winternitz parameter set, determine
 * the private key length */
int wc_LmsKey_GetPrivLen(LmsKey * key, word32 * len)
{
    if (key == NULL || len == NULL) {
        return BAD_FUNC_ARG;
    }

    *len = (word32) hss_get_private_key_len(key->levels, key->lm_type,
                                            key->lm_ots_type);

    return 0;
}

/* Given a levels, height, winternitz parameter set, determine
 * the public key length */
int wc_LmsKey_GetPubLen(LmsKey * key, word32 * len)
{
    if (key == NULL || len == NULL) {
        return BAD_FUNC_ARG;
    }

    *len = (word32) hss_get_public_key_len(key->levels, key->lm_type,
                                           key->lm_ots_type);

    return 0;
}

/* Export a generated public key and parameter set from one LmsKey
 * to another. Use this to prepare a signature verification LmsKey
 * that is pub only.
 *
 * Though the public key is all that is used to verify signatures,
 * the parameter set is needed to calculate the signature length
 * before hand. */
int wc_LmsKey_ExportPub(LmsKey * keyDst, const LmsKey * keySrc)
{
    if (keyDst == NULL || keySrc == NULL) {
        return BAD_FUNC_ARG;
    }

    ForceZero(keyDst, sizeof(LmsKey));

    XMEMCPY(keyDst->pub, keySrc->pub, sizeof(keySrc->pub));
    XMEMCPY(keyDst->lm_type, keySrc->lm_type, sizeof(keySrc->lm_type));
    XMEMCPY(keyDst->lm_ots_type, keySrc->lm_ots_type,
            sizeof(keySrc->lm_ots_type));

    keyDst->levels = keySrc->levels;

    /* Mark this key as verify only, to prevent misuse. */
    keyDst->state = WC_LMS_STATE_VERIFYONLY;

    return 0;
}

/* Given a levels, height, winternitz parameter set, determine
 * the signature length.
 *
 * Call this before wc_LmsKey_Sign so you know the length of
 * the required signature buffer. */
int wc_LmsKey_GetSigLen(LmsKey * key, word32 * len)
{
    if (key == NULL || len == NULL) {
        return BAD_FUNC_ARG;
    }

    *len = (word32) hss_get_signature_len(key->levels, key->lm_type,
                                          key->lm_ots_type);

    return 0;
}

int wc_LmsKey_Sign(LmsKey* key, byte * sig, word32 * sigSz, const byte * msg,
    int msgSz)
{
    bool   result = true;
    size_t len = 0;

    if (key == NULL || sig == NULL || sigSz == NULL || msg == NULL) {
        return BAD_FUNC_ARG;
    }

    if (msgSz <= 0) {
        return BAD_FUNC_ARG;
    }

    if (key->state == WC_LMS_STATE_NOSIGS) {
        WOLFSSL_MSG("error: LMS signatures exhausted");
        return -1;
    }
    else if (key->state != WC_LMS_STATE_OK) {
       /* The key had an error the last time it was used, and we
        * can't guarantee its state. */
        WOLFSSL_MSG("error: can't sign, LMS key not in good state");
        return -1;
    }

    len = hss_get_signature_len(key->levels, key->lm_type, key->lm_ots_type);

    if (len == 0) {
        /* Key parameters are invalid. */
        WOLFSSL_MSG("error: hss_get_signature_len failed");
        key->state = WC_LMS_STATE_BAD;
        return -1;
    }

    result = hss_generate_signature(key->working_key, LmsWritePrivKey,
                                    key, (const void *) msg, msgSz,
                                    sig, len, &key->info);

    if (!result) {
        if (wc_LmsKey_SigsLeft(key) == 0) {
            WOLFSSL_MSG("error: LMS signatures exhausted");
            key->state = WC_LMS_STATE_NOSIGS;
            return -1;
        }

        WOLFSSL_MSG("error: hss_generate_signature failed");
        key->state = WC_LMS_STATE_BAD;
        return -1;
    }

    *sigSz = (word32) len;

    return 0;
}

int wc_LmsKey_Verify(LmsKey * key, const byte * sig, word32 sigSz,
    const byte * msg, int msgSz)
{
    bool result = true;

    if (key == NULL || sig == NULL || msg == NULL) {
        return BAD_FUNC_ARG;
    }

    result = hss_validate_signature(key->pub, (const void *) msg, msgSz, sig,
                                    sigSz, &key->info);

    if (!result) {
        WOLFSSL_MSG("error: hss_validate_signature failed");
        return -1;
    }

    return 0;
}


/* Returns 1 if there are signatures remaining.
 * Returns 0 if available signatures are exhausted.
 *
 * Note: the number of remaining signatures is hidden behind an opaque
 * pointer in the hash-sigs lib. We could add a counter here that is
 * decremented on every signature. The number of available signatures
 * grows as
 *   N = 2 ** (levels * height)
 * so it would need to be a big integer. */
int wc_LmsKey_SigsLeft(LmsKey * key)
{
    if (key == NULL) {
        return BAD_FUNC_ARG;
    }

    if (hss_extra_info_test_last_signature(&key->info)) {
        return 0;
    }

    return 1;
}
#endif /* WOLFSSL_HAVE_LMS */
