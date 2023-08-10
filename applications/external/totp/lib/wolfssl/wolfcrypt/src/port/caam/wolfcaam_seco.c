/* wolfcaam_seco.c
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

#if defined(WOLFSSL_SECO_CAAM)

#include <hsm/hsm_api.h>
#include <seco_nvm.h>

#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/port/caam/wolfcaam.h>

#include <wolfssl/wolfcrypt/cmac.h>
#include <wolfssl/wolfcrypt/aes.h>

#define MAX_SECO_TIMEOUT 1000

static wolfSSL_Mutex caamMutex;
static pthread_t tid;
static uint32_t nvm_status = 0;
static hsm_hdl_t hsm_session;
static hsm_hdl_t key_store_hdl;
static int wc_TranslateHSMError(int current, hsm_err_t err);

static void* hsm_storage_init(void* args)
{
    seco_nvm_manager(NVM_FLAGS_HSM, &nvm_status);
    (void)args;
    return NULL;
}


/* return 0 on success */
int wc_SECOInitInterface()
{
    int i;
    open_session_args_t session_args;
    hsm_err_t err;

    nvm_status = NVM_STATUS_UNDEF;
    if (wc_InitMutex(&caamMutex) != 0) {
        WOLFSSL_MSG("Could not init mutex");
        return -1;
    }

    (void)pthread_create(&tid, NULL, hsm_storage_init, NULL);

    /* wait for NVM to be ready for SECO */
    for (i = 0 ; i < MAX_SECO_TIMEOUT && nvm_status <= NVM_STATUS_STARTING;
        i++) {
        usleep(1000);
    }
    if (i == MAX_SECO_TIMEOUT) {
        WOLFSSL_MSG("Timed out waiting for SECO setup");
        return -1;
    }

    if (nvm_status == NVM_STATUS_STOPPED) {
        WOLFSSL_MSG("Error with SECO setup");
        return -1;
    }

    session_args.session_priority = 0;
    session_args.operating_mode   = 0;

    err = hsm_open_session(&session_args, &hsm_session);
    if (err != HSM_NO_ERROR) {
        WOLFSSL_MSG("Error with HSM session open");
        return -1;
    }
    WOLFSSL_MSG("SECO HSM setup done");

    return 0;
}


void wc_SECOFreeInterface()
{
    hsm_err_t err;

    err = hsm_close_session(hsm_session);
    if (err != HSM_NO_ERROR) {
        WOLFSSL_MSG("Error with HSM session close");
    }


    if (nvm_status != NVM_STATUS_STOPPED) {
        if (pthread_cancel(tid) != 0) {
            WOLFSSL_MSG("SECO HSM thread shutdown failed");
        }
    }

    seco_nvm_close_session();
    WOLFSSL_MSG("SECO HSM shutdown");

    wc_FreeMutex(&caamMutex);
}

/* open the key management HSM handle
 * return 0 on success
 */
int wc_SECO_OpenHSM(word32 keyStoreId, word32 nonce, word16 maxUpdates,
    byte flag)
{
    hsm_err_t err;
    open_svc_key_store_args_t key_store_args;

    XMEMSET(&key_store_args, 0, sizeof(open_svc_key_store_args_t));
    key_store_args.key_store_identifier = keyStoreId,
    key_store_args.authentication_nonce = nonce;
    key_store_args.max_updates_number   = maxUpdates;
    switch (flag) {
        case CAAM_KEYSTORE_CREATE:
            key_store_args.flags = HSM_SVC_KEY_STORE_FLAGS_CREATE;
            break;
        case CAAM_KEYSTORE_UPDATE:
        #ifdef HSM_SVC_KEY_STORE_FLAGS_UPDATE
            key_store_args.flags = HSM_SVC_KEY_STORE_FLAGS_UPDATE;
        #else
            key_store_args.flags = 0;
        #endif
            break;
        default:
            WOLFSSL_MSG("Unknown flag");
            return -1;
    }

    err = hsm_open_key_store_service(hsm_session, &key_store_args,
            &key_store_hdl);
    if (wc_TranslateHSMError(0, err) != Success) {
        return -1;
    }
    else {
        return 0;
    }
}


/* close the key management HSM handle
 * return 0 on success
 */
int wc_SECO_CloseHSM()
{
    hsm_err_t err = hsm_close_key_store_service(key_store_hdl);
    if (wc_TranslateHSMError(0, err) != Success) {
        return -1;
    }
    else {
        return 0;
    }
}


/* returns error enum found from hsm calls, HSM_NO_ERROR on success */
static hsm_err_t wc_SECO_RNG(unsigned int args[4], CAAM_BUFFER *buf, int sz)
{
    hsm_hdl_t rng;
    hsm_err_t err;
    open_svc_rng_args_t svcArgs  = {0};
    op_get_random_args_t rngArgs = {0};

    err = hsm_open_rng_service(hsm_session, &svcArgs, &rng);

    if (err == HSM_NO_ERROR) {
        rngArgs.output      = (uint8_t*)buf[0].TheAddress;
        rngArgs.random_size = (uint32_t)buf[0].Length;
        err = hsm_get_random(rng, &rngArgs);
    #ifdef DEBUG_SECO
        {
            uint32_t z;
            printf("Pulled rng data from HSM :");
            for (z = 0; z < rngArgs.random_size; z++)
                printf("%02X", rngArgs.output[z]);
            printf("\n");
        }
    #endif
    }

    if (err == HSM_NO_ERROR) {
        err = hsm_close_rng_service(rng);
    }

    (void)args;
    (void)sz;
    return err;
}


static hsm_err_t wc_SECO_Hash(unsigned args[4], CAAM_BUFFER *buf, int sz,
    int type)
{
    hsm_hdl_t hash;
    hsm_err_t err = HSM_NO_ERROR;
    op_hash_one_go_args_t hashArgs   = {0};
    open_svc_hash_args_t sessionArgs = {0};

    if (args[0] != CAAM_ALG_FINAL) {
        WOLFSSL_MSG("Only expecting to call the HSM on final");
        err = HSM_GENERAL_ERROR;
    }

    if (err == HSM_NO_ERROR) {
        err = hsm_open_hash_service(hsm_session, &sessionArgs, &hash);
    }

    if (err == HSM_NO_ERROR) {
        switch (type) {
            case CAAM_SHA224:
                hashArgs.algo = HSM_HASH_ALGO_SHA_224;
                break;

            case CAAM_SHA256:
                hashArgs.algo = HSM_HASH_ALGO_SHA_256;
                break;

            case CAAM_SHA384:
                hashArgs.algo = HSM_HASH_ALGO_SHA_384;
                break;

            case CAAM_SHA512:
                hashArgs.algo = HSM_HASH_ALGO_SHA_512;
                break;
        }

        hashArgs.output = (uint8_t*)buf[0].TheAddress;
        hashArgs.output_size = buf[0].Length;
        hashArgs.input = (uint8_t*)buf[1].TheAddress;
        hashArgs.input_size = buf[1].Length;

        err = hsm_hash_one_go(hash, &hashArgs);
        if (err != HSM_NO_ERROR) {
            WOLFSSL_MSG("Error with HSM hash call");
        }

    #ifdef DEBUG_SECO
        {
            word32 z;
            printf("hash algo type = %d\n", hashArgs.algo);
            printf("\tlength of input data = %d\n", hashArgs.input_size);
            printf("\toutput : ");
            for (z = 0; z < hashArgs.output_size; z++)
                printf("%02X", hashArgs.output[z]);
            printf("\n");
        }
    #endif

        /* always try to close the hash handle */
        if (hsm_close_hash_service(hash) != HSM_NO_ERROR) {
            WOLFSSL_MSG("Error with HSM hash close");
            if (err == HSM_NO_ERROR) {
                err = HSM_GENERAL_ERROR;
            }
        }
    }
    (void)sz;
    return err;
}


/* convert ECDSEL type to HSM type
 * return 0 on fail
 */
static hsm_key_type_t ECDSELtoHSM(int ecdsel)
{
    switch (ecdsel) {
        case CAAM_ECDSA_P192:
        case CAAM_ECDSA_P224:
            WOLFSSL_MSG("P192 and P224 are not supported");
            break;

        case CAAM_ECDSA_P256:
            return CAAM_KEYTYPE_ECDSA_P256;

        case CAAM_ECDSA_P384:
            return CAAM_KEYTYPE_ECDSA_P384;

        case CAAM_ECDSA_P521:
            return CAAM_KEYTYPE_ECDSA_P521;
    }
    return 0;
}


static hsm_key_type_t KeyTypeToHSM(int keyTypeIn)
{
    hsm_key_type_t ret = 0;
    switch (keyTypeIn) {
        case CAAM_KEYTYPE_ECDSA_P256:
            ret = HSM_KEY_TYPE_ECDSA_NIST_P256;
            break;

        case CAAM_KEYTYPE_ECDSA_P384:
            ret = HSM_KEY_TYPE_ECDSA_NIST_P384;
            break;

        case CAAM_KEYTYPE_ECDSA_P521:
            ret = HSM_KEY_TYPE_ECDSA_NIST_P521;
            break;

        case CAAM_KEYTYPE_AES128:
            ret = HSM_KEY_TYPE_AES_128;
            break;

        case CAAM_KEYTYPE_AES192:
            ret = HSM_KEY_TYPE_AES_192;
            break;

        case CAAM_KEYTYPE_AES256:
            ret = HSM_KEY_TYPE_AES_256;
            break;

    #ifdef HSM_KEY_TYPE_HMAC_224
        case CAAM_KEYTYPE_HMAC224:
            ret = HSM_KEY_TYPE_HMAC_224;
            break;
    #endif

    #ifdef HSM_KEY_TYPE_HMAC_256
        case CAAM_KEYTYPE_HMAC256:
            ret = HSM_KEY_TYPE_HMAC_256;
            break;
    #endif

    #ifdef HSM_KEY_TYPE_HMAC_384
        case CAAM_KEYTYPE_HMAC384:
            ret = HSM_KEY_TYPE_HMAC_384;
            break;
    #endif

    #ifdef HSM_KEY_TYPE_HMAC_512
        case CAAM_KEYTYPE_HMAC512:
            ret = HSM_KEY_TYPE_HMAC_512;
            break;
    #endif
    }
    return ret;
}


static hsm_key_info_t KeyInfoToHSM(int keyInfoIn)
{
    hsm_key_info_t ret = 0;

    switch (keyInfoIn) {
        case CAAM_KEY_PERSISTENT:
            ret = HSM_KEY_INFO_PERSISTENT;
            break;

        case CAAM_KEY_TRANSIENT:
            ret = HSM_KEY_INFO_TRANSIENT;
            break;

        case CAAM_KEY_KEK:
            ret = HSM_KEY_INFO_KEK;
            break;
    }
    return ret;
}


static int KeyFlagsToHSM(int flags)
{
    int ret = 0;

    #ifdef HSM_OP_KEY_GENERATION_FLAGS_UPDATE
    if (flags & CAAM_UPDATE_KEY) {
        ret = HSM_OP_KEY_GENERATION_FLAGS_UPDATE;
    }
    #endif

    #ifdef HSM_OP_KEY_GENERATION_FLAGS_CREATE
    if (flags & CAAM_GENERATE_KEY) {
        ret = HSM_OP_KEY_GENERATION_FLAGS_CREATE;
    }
    #endif
    return ret;
}


/* generic generate key with HSM
 * return 0 on success
 */
int wc_SECO_GenerateKey(int flags, int group, byte* out, int outSz,
    int keyTypeIn, int keyInfoIn, unsigned int* keyIdOut)
{
    hsm_err_t err;
    hsm_hdl_t key_mgmt_hdl;
    open_svc_key_management_args_t key_mgmt_args;
    op_generate_key_args_t         key_args;
    hsm_key_type_t keyType;
    hsm_key_info_t keyInfo;

    if (flags == CAAM_UPDATE_KEY && group != 0) {
        WOLFSSL_MSG("Group must be 0 if updating key");
        return BAD_FUNC_ARG;
    }

    keyType = KeyTypeToHSM(keyTypeIn);
    keyInfo = KeyInfoToHSM(keyInfoIn);

    if (wc_LockMutex(&caamMutex) != 0) {
        return BAD_MUTEX_E;
    }

    XMEMSET(&key_mgmt_args, 0, sizeof(key_mgmt_args));
    err = hsm_open_key_management_service(
        key_store_hdl, &key_mgmt_args, &key_mgmt_hdl);

    /* setup key arguments */
    if (err == HSM_NO_ERROR) {
        XMEMSET(&key_args, 0, sizeof(key_args));

        key_args.key_identifier = keyIdOut;
        key_args.out_size = outSz;
        key_args.out_key  = out;

        /* default to strict operations with key in NVM */
        key_args.flags = KeyFlagsToHSM(flags) |
                            HSM_OP_KEY_GENERATION_FLAGS_STRICT_OPERATION;
        key_args.key_group = group;
        key_args.key_info  = keyInfo;
        key_args.key_type  = keyType;
    #ifdef DEBUG_SECO
        printf("Generating key using:\n");
        printf("\tflags = %d\n", key_args.flags);
        printf("\tgroup = %d\n", key_args.key_group);
        printf("\tinfo  = %d\n", key_args.key_info);
        printf("\ttype  = %d\n", key_args.key_type);
        printf("\tout   = %p\n", key_args.out_key);
        printf("\toutSZ = %d\n", key_args.out_size);
    #endif
        err = hsm_generate_key(key_mgmt_hdl, &key_args);
        if (err != HSM_NO_ERROR) {
            WOLFSSL_MSG("Key generation error");
        }
    #ifdef DEBUG_SECO
        if (err == HSM_NO_ERROR) {
            printf("KeyID generated = %u\n", *key_args.key_identifier);
        }
    #endif

        /* always try to close key management if open */
        if (hsm_close_key_management_service(key_mgmt_hdl) != HSM_NO_ERROR) {
            err = HSM_GENERAL_ERROR;
        }
    }
    else {
        WOLFSSL_MSG("Could not open key management");
    }

    wc_UnLockMutex(&caamMutex);

    if (wc_TranslateHSMError(0, err) == Success) {
        return 0;
    }
    else {
        return -1;
    }
}


/* delete a key
 * return 0 on success
 */
int wc_SECO_DeleteKey(unsigned int keyId, int group, int keyTypeIn)
{
    hsm_hdl_t key_mgmt_hdl;
    open_svc_key_management_args_t key_mgmt_args;
    op_manage_key_args_t del_args;
    hsm_err_t err;

    XMEMSET(&key_mgmt_args, 0, sizeof(key_mgmt_args));
    err = hsm_open_key_management_service(
        key_store_hdl, &key_mgmt_args, &key_mgmt_hdl);

    if (err == HSM_NO_ERROR) {
        XMEMSET(&del_args, 0, sizeof(del_args));
        del_args.key_identifier = &keyId;
        del_args.flags = HSM_OP_MANAGE_KEY_FLAGS_DELETE;
        del_args.key_type = KeyTypeToHSM(keyTypeIn);
        del_args.key_group = group;
    #ifdef DEBUG_SECO
        printf("Trying to delete key:\n");
        printf("\tkeyID    : %u\n", keyId);
        printf("\tkey type : %d\n", del_args.key_type);
        printf("\tkey grp  : %d\n", del_args.key_group);
    #endif
        err = hsm_manage_key(key_mgmt_hdl, &del_args);

        /* always try to close key management if open */
        if (hsm_close_key_management_service(key_mgmt_hdl) != HSM_NO_ERROR) {
            err = HSM_GENERAL_ERROR;
        }
    }

    if (wc_TranslateHSMError(0, err) == Success) {
        return 0;
    }
    else {
        return -1;
    }
}


#if defined(WOLFSSL_CMAC)
void wc_SECO_CMACSetKeyID(Cmac* cmac, int keyId)
{
    cmac->blackKey = keyId;
}


int wc_SECO_CMACGetKeyID(Cmac* cmac)
{
    return cmac->blackKey;
}
#endif

void wc_SECO_AesSetKeyID(Aes* aes, int keyId)
{
    aes->blackKey = keyId;
}


int wc_SECO_AesGetKeyID(Aes* aes)
{
    return aes->blackKey;
}


/* return 0 on success and fill in out buffer (must be 32 bytes) */
int wc_SECO_ExportKEK(byte* out, byte outSz, byte isCommon)
{
    hsm_err_t err;
    op_export_root_kek_args_t export_args;

    XMEMSET(&export_args, 0, sizeof(export_args));

    export_args.signed_message  = NULL;
    export_args.signed_msg_size = 0;

    if (isCommon == 1) {
        export_args.flags = HSM_OP_EXPORT_ROOT_KEK_FLAGS_COMMON_KEK;
    }
    else {
        export_args.flags = HSM_OP_EXPORT_ROOT_KEK_FLAGS_UNIQUE_KEK;
    }
    export_args.out_root_kek  = out;
    export_args.root_kek_size = outSz;

    err = hsm_export_root_key_encryption_key (hsm_session,  &export_args);
    if (wc_TranslateHSMError(0, err) != Success) {
        return -1;
    }
    else {
        return 0;
    }
}


/* make a black key using HSM */
static hsm_err_t wc_SECO_ECDSA_Make(unsigned int args[4], CAAM_BUFFER *buf,
    int sz)
{
    hsm_key_type_t keyType;

    (void)sz;
    keyType = ECDSELtoHSM(args[1] ^ CAAM_ECDSA_KEYGEN_PD);
    if (wc_SECO_GenerateKey(HSM_OP_KEY_GENERATION_FLAGS_CREATE,
                               1,
                               (byte*)buf[1].TheAddress,
                               buf[1].Length,
                               keyType,
                               CAAM_KEY_TRANSIENT,
                               (word32*)&buf[0].TheAddress) == 0) {
        return HSM_NO_ERROR;
    }
    else {
        return HSM_GENERAL_ERROR;
    }
}


/* sign a message (hash(msg)) using a hsm key */
static hsm_err_t wc_SECO_ECDSA_Sign(unsigned int args[4], CAAM_BUFFER *buf,
    int sz)
{
    hsm_err_t err;
    hsm_hdl_t sig_gen_hdl;
    open_svc_sign_gen_args_t open_args;
    op_generate_sign_args_t  sig_args;
    byte sigOut[2*MAX_ECC_BYTES];

    if (args[3] != 32 && args[3] != 48) {
        WOLFSSL_MSG("Unexpected key size");
        return BAD_FUNC_ARG;
    }

    if (buf[1].Length != (int)args[3]) {
        WOLFSSL_MSG("Bad message input size");
        return BAD_FUNC_ARG;
    }

    if (wc_LockMutex(&caamMutex) != 0) {
        return BAD_MUTEX_E;
    }

    XMEMSET(&open_args, 0, sizeof(open_args));
    err = hsm_open_signature_generation_service(key_store_hdl, &open_args,
            &sig_gen_hdl);
    if (err == HSM_NO_ERROR) {
        XMEMSET(&sig_args, 0, sizeof(sig_args));
        sig_args.key_identifier = buf[0].TheAddress;
        sig_args.message        = (uint8_t*)buf[1].TheAddress;
        sig_args.message_size   = buf[1].Length;
        sig_args.signature      = sigOut;
        sig_args.signature_size = buf[2].Length + buf[3].Length + 1;

        if (args[3] == 32) {
            sig_args.scheme_id = HSM_SIGNATURE_SCHEME_ECDSA_NIST_P256_SHA_256;
        }
        else {
            sig_args.scheme_id = HSM_SIGNATURE_SCHEME_ECDSA_NIST_P384_SHA_384;
        }
        sig_args.flags = HSM_OP_GENERATE_SIGN_FLAGS_INPUT_DIGEST;

    #ifdef DEBUG_SECO
        printf("Trying to create an ECC signature:\n");
        printf("\tkeyID    : %u\n", sig_args.key_identifier);
        printf("\tmsg size : %d\n", sig_args.message_size);
        printf("\tsig size : %d\n", sig_args.signature_size);
    #endif
        err = hsm_generate_signature(sig_gen_hdl, &sig_args);

        /* always try to close sign service when open */
        if (hsm_close_signature_generation_service(sig_gen_hdl)
                != HSM_NO_ERROR) {
            err = HSM_GENERAL_ERROR;
        }
    }

    /* copy out r and s on success */
    if (err == HSM_NO_ERROR) {
        XMEMCPY((byte*)buf[2].TheAddress, sigOut, buf[2].Length);
        XMEMCPY((byte*)buf[3].TheAddress, sigOut + buf[2].Length,
            buf[3].Length);
    }

    wc_UnLockMutex(&caamMutex);

    (void)sz;
    if (wc_TranslateHSMError(0, err) != Success) {
        return -1;
    }
    else {
        return 0;
    }
}


/* verify a signature (hash(msg)) using HSM */
static hsm_err_t wc_SECO_ECDSA_Verify(unsigned int args[4], CAAM_BUFFER *buf,
    int sz)
{
    hsm_err_t err;
    hsm_hdl_t sig_ver_hdl;
    open_svc_sign_ver_args_t  open_sig_ver_args;
    op_verify_sign_args_t     sig_ver_args;
    hsm_verification_status_t verify;

    byte rsR[2*MAX_ECC_BYTES];
    word32 rsRSz = 2*MAX_ECC_BYTES;

    if (args[3] != 32 && args[3] != 48) {
        WOLFSSL_MSG("Unexpected key size");
        return BAD_FUNC_ARG;
    }

    if (buf[1].Length != (int)args[3]) {
        WOLFSSL_MSG("Bad message input size");
        return BAD_FUNC_ARG;
    }

    if (wc_LockMutex(&caamMutex) != 0) {
        return BAD_MUTEX_E;
    }

    XMEMSET(rsR, 0, rsRSz);
    XMEMCPY(rsR, (byte*)buf[2].TheAddress, buf[2].Length);
    XMEMCPY(rsR + buf[2].Length, (byte*)buf[3].TheAddress, buf[3].Length);
    rsRSz = buf[2].Length + buf[3].Length + 1; /* +1 for the HSM compression */

    XMEMSET(&open_sig_ver_args, 0, sizeof(open_sig_ver_args));
    err = hsm_open_signature_verification_service(hsm_session,
                    &open_sig_ver_args, &sig_ver_hdl);
    if (err == HSM_NO_ERROR) {
        XMEMSET(&sig_ver_args, 0, sizeof(sig_ver_args));
        sig_ver_args.key      = (uint8_t*)buf[0].TheAddress;
        sig_ver_args.key_size = buf[0].Length;
        sig_ver_args.message  = (uint8_t*)buf[1].TheAddress;
        sig_ver_args.message_size   = buf[1].Length;
        sig_ver_args.signature      = rsR;
        sig_ver_args.signature_size = rsRSz;
        if (args[3] == 32) {
            sig_ver_args.scheme_id =
                HSM_SIGNATURE_SCHEME_ECDSA_NIST_P256_SHA_256;
        }
        else {
            sig_ver_args.scheme_id =
                HSM_SIGNATURE_SCHEME_ECDSA_NIST_P384_SHA_384;
        }
        sig_ver_args.flags = HSM_OP_VERIFY_SIGN_FLAGS_INPUT_DIGEST;

    #ifdef DEBUG_SECO
        {
            word32 i;

            printf("Trying to verify an ECC signature:\n");
            printf("\tpublic key : ");
            for (i = 0; i < sig_ver_args.key_size; i++)
                printf("%02X", sig_ver_args.key[i]);
            printf("\n");
            printf("\tsignature : ");
            for (i = 0; i < sig_ver_args.signature_size; i++)
                printf("%02X", sig_ver_args.signature[i]);
            printf("\n");
            printf("\tmsg size : %d\n", sig_ver_args.message_size);
        }
    #endif
        err = hsm_verify_signature (sig_ver_hdl, &sig_ver_args,
                &verify);
        if (verify != HSM_VERIFICATION_STATUS_SUCCESS) {
            WOLFSSL_MSG("Verification found bad signature");
            err = HSM_GENERAL_ERROR;
        }

        if (hsm_close_signature_verification_service(sig_ver_hdl) !=
                HSM_NO_ERROR) {
            err = HSM_GENERAL_ERROR;
        }
    }

    wc_UnLockMutex(&caamMutex);

    (void)sz;
    return err;
}


static hsm_err_t wc_SECO_CMAC(unsigned int args[4], CAAM_BUFFER* buf, int sz)
{
    unsigned int blackKey;
    hsm_err_t err;
    hsm_hdl_t mac_hdl;
    open_svc_mac_args_t  mac_svc_args;
    op_mac_one_go_args_t mac_args;
    hsm_mac_verification_status_t status;

    if ((args[0] & CAAM_ALG_FINAL) == 0) {
        WOLFSSL_MSG("CMAC expected only in final case!");
        return HSM_GENERAL_ERROR;
    }

    blackKey = args[2];
    /* black key listed as 0, the key needs to be imported */
    if (blackKey == 0) {
        int keyGroup = 1; /* group one was chosen arbitrarily */
        byte importIV[GCM_NONCE_MID_SZ];
        int importIVSz = GCM_NONCE_MID_SZ;
        int keyType = 0;
        WC_RNG rng;

        if (wc_InitRng(&rng) != 0) {
            WOLFSSL_MSG("RNG init for IV failed");
            return HSM_GENERAL_ERROR;
        }

        if (wc_RNG_GenerateBlock(&rng, importIV, importIVSz) != 0) {
            WOLFSSL_MSG("Generate IV failed");
            wc_FreeRng(&rng);
            return HSM_GENERAL_ERROR;
        }
        wc_FreeRng(&rng);

        switch (buf[0].Length) {
            case AES_128_KEY_SIZE: keyType = CAAM_KEYTYPE_AES128; break;
            case AES_192_KEY_SIZE: keyType = CAAM_KEYTYPE_AES192; break;
            case AES_256_KEY_SIZE: keyType = CAAM_KEYTYPE_AES256; break;
        }

        blackKey = wc_SECO_WrapKey(0, (byte*)buf[0].TheAddress, buf[0].Length,
            importIV, importIVSz, keyType, CAAM_KEY_TRANSIENT, keyGroup);

        if (blackKey == 0) {
            return WC_HW_E;
        }
    }

    err = hsm_open_mac_service(key_store_hdl, &mac_svc_args, &mac_hdl);
    if (err == HSM_NO_ERROR) {
        mac_args.key_identifier = blackKey; /* blackKey / HSM */
        mac_args.algorithm = HSM_OP_MAC_ONE_GO_ALGO_AES_CMAC;
        mac_args.flags     = HSM_OP_MAC_ONE_GO_FLAGS_MAC_GENERATION;

        mac_args.payload      = (uint8_t*)buf[2].TheAddress;
        mac_args.payload_size = buf[2].Length;

        mac_args.mac      = (uint8_t*)buf[1].TheAddress;
        mac_args.mac_size = (buf[1].Length < AES_BLOCK_SIZE)? buf[1].Length:
                                                              AES_BLOCK_SIZE;
    #ifdef DEBUG_SECO
        printf("CMAC arguments used:\n");
        printf("\tkey id       = %d\n", mac_args.key_identifier);
        printf("\tpayload      = %p\n", mac_args.payload);
        printf("\tpayload size = %d\n", mac_args.payload_size);
        printf("\tmac out      = %p\n", mac_args.mac);
        printf("\tmac out size = %d\n", mac_args.mac_size);
    #endif
        err = hsm_mac_one_go(mac_hdl, &mac_args, &status);

        /* always try to close mac service if open */
        if (hsm_close_mac_service(mac_hdl) != HSM_NO_ERROR) {
            WOLFSSL_MSG("Error closing down mac service handle");
            err = HSM_GENERAL_ERROR;
        }
    }

    (void)sz;
    return err;
}


/* common code between CBC,ECB, and CCM modes */
static hsm_err_t wc_SEC_AES_Common(unsigned int args[4], CAAM_BUFFER* buf,
    int sz, hsm_op_cipher_one_go_algo_t algo,
    uint8_t* in, int inSz, uint8_t* out, int outSz)
{
    int dir;
    hsm_hdl_t cipher_hdl;
    open_svc_cipher_args_t  open_args;
    op_cipher_one_go_args_t cipher_args;
    hsm_err_t err;

    XMEMSET(&open_args, 0, sizeof(open_args));
    err = hsm_open_cipher_service(key_store_hdl, &open_args, &cipher_hdl);
    if (err == HSM_NO_ERROR) {
        XMEMSET(&cipher_args, 0, sizeof(cipher_args));
        cipher_args.key_identifier = args[3]; /* black key / HSM */
        if (algo == HSM_CIPHER_ONE_GO_ALGO_AES_ECB) {
            cipher_args.iv_size = 0; /* no iv with AES-ECB */
        }
        else {
            cipher_args.iv      = (uint8_t*)buf[1].TheAddress;
            cipher_args.iv_size = buf[1].Length;
        }

        cipher_args.cipher_algo = algo;
        dir = args[0] & 0xFFFF; /* extract direction enc/dec from input args */
        if (dir == CAAM_DEC) {
            cipher_args.flags = HSM_CIPHER_ONE_GO_FLAGS_DECRYPT;
        }
        else {
            cipher_args.flags = HSM_CIPHER_ONE_GO_FLAGS_ENCRYPT;
        }

        cipher_args.input      = in;
        cipher_args.input_size = inSz;
        cipher_args.output      = out;
        cipher_args.output_size = outSz;

    #ifdef DEBUG_SECO
        printf("AES Operation :\n");
        printf("\tkeyID    : %u\n", cipher_args.key_identifier);
        printf("\tinput    : %p\n", cipher_args.input);
        printf("\tinput sz : %d\n", cipher_args.input_size);
        printf("\toutput    : %p\n", cipher_args.output);
        printf("\toutput sz : %d\n", cipher_args.output_size);
        printf("\tiv       : %p\n", cipher_args.iv);
        printf("\tiv sz    : %d\n", cipher_args.iv_size);
    #endif
        err = hsm_cipher_one_go(cipher_hdl, &cipher_args);

        /* always try to close cipher service if open */
        if (hsm_close_cipher_service(cipher_hdl) != HSM_NO_ERROR) {
            err = HSM_GENERAL_ERROR;
        }
    }

    (void)sz;
    return err;
}


static hsm_err_t wc_SECO_AESECB(unsigned int args[4], CAAM_BUFFER* buf, int sz)
{
    return wc_SEC_AES_Common(args, buf, sz, HSM_CIPHER_ONE_GO_ALGO_AES_ECB,
        (uint8_t*)buf[1].TheAddress, buf[1].Length,
        (uint8_t*)buf[2].TheAddress, buf[2].Length);
}


static hsm_err_t wc_SECO_AESCBC(unsigned int args[4], CAAM_BUFFER* buf, int sz)
{
    return wc_SEC_AES_Common(args, buf, sz, HSM_CIPHER_ONE_GO_ALGO_AES_CBC,
        (uint8_t*)buf[2].TheAddress, buf[2].Length,
        (uint8_t*)buf[3].TheAddress, buf[3].Length);
}


static hsm_err_t wc_SECO_AESCCM(unsigned int args[4], CAAM_BUFFER* buf, int sz)
{
    hsm_err_t err;
    uint8_t* in;
    uint8_t* out;
    int inSz;
    int outSz;
    int dir;

    byte* cipherAndTag   = NULL;
    int   cipherAndTagSz = 0;

    if (buf[1].Length != 12) {
        WOLFSSL_MSG("SECO expecting nonce size of 12");
        return HSM_GENERAL_ERROR;
    }

    if (buf[4].Length != 16) {
        WOLFSSL_MSG("SECO expecting tag size of 16");
        return HSM_GENERAL_ERROR;
    }

    if (buf[5].Length != 0) {
        WOLFSSL_MSG("SECO expecting adata size of 0");
        return HSM_GENERAL_ERROR;
    }

    cipherAndTagSz = buf[4].Length + buf[2].Length;
    cipherAndTag   = (byte*)XMALLOC(cipherAndTagSz, NULL,
        DYNAMIC_TYPE_TMP_BUFFER);
    dir = args[0] & 0xFFFF; /* get if doing enc or dec */
    if (dir == CAAM_ENC) {
        in = (uint8_t*)buf[2].TheAddress;
        inSz  = buf[2].Length;
        out   = cipherAndTag;
        outSz = cipherAndTagSz;
    }
    else {
        XMEMCPY(cipherAndTag, (uint8_t*)buf[2].TheAddress, buf[2].Length);
        XMEMCPY(cipherAndTag + buf[2].Length, (uint8_t*)buf[4].TheAddress,
            buf[4].Length);
        in = cipherAndTag;
        inSz  = cipherAndTagSz;
        out   = (uint8_t*)buf[3].TheAddress;
        outSz = buf[3].Length;
    }

    err = wc_SEC_AES_Common(args, buf, sz, HSM_CIPHER_ONE_GO_ALGO_AES_CCM,
            in, inSz, out, outSz);
    if (err == HSM_NO_ERROR) {
        if (dir == CAAM_ENC) {
            XMEMCPY((uint8_t*)buf[4].TheAddress, cipherAndTag + inSz,
                buf[4].Length);
            XMEMCPY((uint8_t*)buf[3].TheAddress, cipherAndTag, buf[3].Length);
        }
    }
    XFREE(cipherAndTag, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    return err;
}


static hsm_err_t wc_SECO_AESGCM(unsigned int args[4], CAAM_BUFFER* buf, int sz)
{
    hsm_err_t err;
    hsm_hdl_t cipher_hdl;
    op_auth_enc_args_t auth_args;
    open_svc_cipher_args_t  open_args;
    uint8_t* in;
    uint8_t* out;
    int      inSz;
    int      outSz;
    byte* cipherAndTag   = NULL;
    int   cipherAndTagSz = 0;
    int   dir;

    dir = args[0] & 0xFFFF; /* extract direction enc/dec from input args */
    XMEMSET(&open_args, 0, sizeof(open_args));
    err = hsm_open_cipher_service(key_store_hdl, &open_args, &cipher_hdl);
    if (err == HSM_NO_ERROR) {
        cipherAndTagSz = buf[4].Length + buf[2].Length;
        cipherAndTag   = (byte*)XMALLOC(cipherAndTagSz, NULL,
            DYNAMIC_TYPE_TMP_BUFFER);
        if (dir == CAAM_ENC) {
            in = (uint8_t*)buf[2].TheAddress;
            inSz  = buf[2].Length;
            out   = cipherAndTag;
            outSz = cipherAndTagSz;
        }
        else {
            XMEMCPY(cipherAndTag, (uint8_t*)buf[2].TheAddress, buf[2].Length);
            XMEMCPY(cipherAndTag + buf[2].Length, (uint8_t*)buf[4].TheAddress,
                buf[4].Length);
            in = cipherAndTag;
            inSz  = cipherAndTagSz;
            out   = (uint8_t*)buf[3].TheAddress;
            outSz = buf[3].Length;
        }

        auth_args.key_identifier = args[3]; /* black key / HSM */
        auth_args.iv      = (uint8_t*)buf[1].TheAddress;
        auth_args.iv_size = buf[1].Length;
        auth_args.input      = in;
        auth_args.input_size = inSz;
        auth_args.output      = out;
        auth_args.output_size = outSz;
        auth_args.aad      = (uint8_t*)buf[5].TheAddress;
        auth_args.aad_size = buf[5].Length;

        if (dir == CAAM_DEC) {
            auth_args.flags = HSM_AUTH_ENC_FLAGS_DECRYPT;
        }
        else {
            auth_args.flags = HSM_AUTH_ENC_FLAGS_ENCRYPT;
        }
        auth_args.ae_algo = HSM_AUTH_ENC_ALGO_AES_GCM;

    #ifdef DEBUG_SECO
        printf("AES GCM Operation :\n");
        printf("\tkeyID    : %u\n", auth_args.key_identifier);
        printf("\tinput    : %p\n", auth_args.input);
        printf("\tinput sz : %d\n", auth_args.input_size);
        printf("\toutput    : %p\n", auth_args.output);
        printf("\toutput sz : %d\n", auth_args.output_size);
        printf("\tiv       : %p\n", auth_args.iv);
        printf("\tiv sz    : %d\n", auth_args.iv_size);
        printf("\taad      : %p\n", auth_args.aad);
        printf("\taad sz   : %d\n", auth_args.aad_size);
    #endif
        err = hsm_auth_enc(cipher_hdl, &auth_args);

        /* always try to close cipher service if open */
        if (hsm_close_cipher_service(cipher_hdl) != HSM_NO_ERROR) {
            err = HSM_GENERAL_ERROR;
        }
    }

    if (err == HSM_NO_ERROR) {
        if (dir == CAAM_ENC) {
            XMEMCPY((uint8_t*)buf[4].TheAddress, cipherAndTag + inSz,
                buf[4].Length);
            XMEMCPY((uint8_t*)buf[3].TheAddress, cipherAndTag, buf[3].Length);
        }
    }
    XFREE(cipherAndTag, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    (void)sz;
    return HSM_NO_ERROR;
}


/* use KEK to encrypt and import a key
 * return 0 on failure and new key ID on success */
word32 wc_SECO_WrapKey(word32 keyId, byte* in, word32 inSz, byte* iv,
    word32 ivSz, int keyType, int keyInfo, int group)
{
    op_manage_key_args_t key_args;
    hsm_hdl_t key_mgmt_hdl;
    Aes aes;
    int ret = 0;
    word32 outId = 0;
    byte *wrappedKey = NULL;
    word32 wrappedKeySz;
    open_svc_key_management_args_t key_mgmt_args;
    hsm_err_t err;

    if (group > MAX_GROUP) {
        WOLFSSL_MSG("group number is too large");
        return 0;
    }

    if (ivSz != (word32)GCM_NONCE_MID_SZ) {
        WOLFSSL_MSG("expected an IV size of 12");
        return 0;
    }

    /* iv + key + tag */
    wrappedKeySz = GCM_NONCE_MID_SZ + inSz + AES_BLOCK_SIZE;
    wrappedKey = (byte*)XMALLOC(wrappedKeySz, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (wrappedKey == NULL) {
        WOLFSSL_MSG("Error malloc'ing buffer for wrapped key");
        return 0;
    }

    XMEMSET(&key_mgmt_args, 0, sizeof(key_mgmt_args));
    err = hsm_open_key_management_service(
        key_store_hdl, &key_mgmt_args, &key_mgmt_hdl);

    XMEMSET(&key_args, 0, sizeof(key_args));
    XMEMSET(wrappedKey, 0, wrappedKeySz);
    XMEMCPY(wrappedKey, iv, ivSz);

    key_args.flags = HSM_OP_MANAGE_KEY_FLAGS_IMPORT_CREATE;
    if (keyId == 0) { /* use the root unique key if no ID is provided */
        byte KEK[AES_256_KEY_SIZE];
        byte KEKSz = AES_256_KEY_SIZE;

        ret = wc_SECO_ExportKEK(KEK, KEKSz, 0);
        if (ret != 0) {
            WOLFSSL_MSG("error with getting KEK from device");
        }

        if (ret == 0) {
            /* use software implementation for encrypting with KEK */
            ret = wc_AesInit(&aes, NULL, INVALID_DEVID);
        }

        if (ret == 0) {
            ret = wc_AesGcmSetKey(&aes, KEK, KEKSz);
            if (ret != 0) {
                WOLFSSL_MSG("error with AES-GCM set key");
            }
        }

        key_args.flags |= HSM_OP_MANAGE_KEY_FLAGS_PART_UNIQUE_ROOT_KEK;
        #if 0
            /* for now only using the unique kek, this would be for common */
            key_args.flags |= HSM_OP_MANAGE_KEY_FLAGS_COMMON_ROOT_KEK;
        #endif
    }
    else {
        wc_AesInit(&aes, NULL, WOLFSSL_SECO_DEVID);
        wc_SECO_AesSetKeyID(&aes, keyId);
    }

    if (ret == 0) {
        ret = wc_AesGcmEncrypt(&aes, wrappedKey + ivSz, in, inSz,
                wrappedKey, ivSz, wrappedKey + ivSz + inSz, AES_BLOCK_SIZE,
                NULL, 0);
        if (ret != 0) {
            WOLFSSL_MSG("error with AES-GCM encrypt when wrapping key");
        }
    }

    if (err == HSM_NO_ERROR) {
        key_args.key_identifier = &outId;
        key_args.kek_identifier = keyId;
        key_args.key_group  = group;
        key_args.key_type   = KeyTypeToHSM(keyType);
        key_args.key_info   = KeyInfoToHSM(keyInfo);
        key_args.input_data = wrappedKey;
        key_args.input_size = wrappedKeySz;

    #ifdef DEBUG_SECO
        {
            word32 i;
            printf("Import Key Operation :\n");
            printf("\tkey ID    : %u\n", *key_args.key_identifier);
            printf("\tkEK ID    : %u\n", key_args.kek_identifier);
            printf("\tflags     : %u\n", key_args.flags);
            printf("\tgroup     : %u\n", key_args.key_group);
            printf("\tkey type  : %d\n", key_args.key_type);
            printf("\tkey info  : %d\n", key_args.key_info);
            printf("\tkey input Size [iv | key | tag ]: %d\n", key_args.input_size);
            printf("\t[iv]  = ");
            for (i = 0; i < 12; i++)
                printf("%02X", key_args.input_data[i]);
            printf("\n");
            printf("\t[enc] = ");
            for (i = 12; i < 12 + inSz; i++)
                printf("%02X", key_args.input_data[i]);
            printf("\n");
            printf("\t[tag] = ");
            for (i = 12 + inSz; i < 12 + inSz + 16; i++)
                printf("%02X", key_args.input_data[i]);
            printf("\n");
        }
    #endif

        /* only try to import if the AES-GCM encrypt was successful */
        if (ret == 0) {
            err = hsm_manage_key(key_mgmt_hdl, &key_args);
        }

    #ifdef DEBUG_SECO
        if (err == HSM_NO_ERROR) {
            printf("Result of Import Key Operation :\n");
            printf("\tkey ID    : %u\n", *key_args.key_identifier);
        }
    #endif

        /* always try to close key management if open */
        if (hsm_close_key_management_service(key_mgmt_hdl) != HSM_NO_ERROR) {
            err = HSM_GENERAL_ERROR;
        }
    }

    if (wrappedKey != NULL) {
        XFREE(wrappedKey, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }

    if (wc_TranslateHSMError(0, err) != Success) {
        return 0;
    }
    else {
        return *key_args.key_identifier;
    }
}


/* trasnlates the HSM error to wolfSSL error and does debug print out */
int wc_TranslateHSMError(int current, hsm_err_t err)
{
    int ret = -1;

    switch (err) {
        case HSM_NO_ERROR:
            ret = Success;
            break;

        case HSM_INVALID_MESSAGE:
            WOLFSSL_MSG("SECO HSM: Invalid/unknown msg");
            break;

        case HSM_INVALID_ADDRESS:
            WOLFSSL_MSG("SECO HSM: Invalid address");
            break;

        case HSM_UNKNOWN_ID:
            WOLFSSL_MSG("SECO HSM: unknown ID");
            break;

        case HSM_INVALID_PARAM:
            WOLFSSL_MSG("SECO HSM: invalid param");
            break;

        case HSM_NVM_ERROR:
            WOLFSSL_MSG("SECO HSM: generic nvm error");
            break;

        case HSM_OUT_OF_MEMORY:
            WOLFSSL_MSG("SECO HSM: out of memory");
            break;

        case HSM_UNKNOWN_HANDLE:
            WOLFSSL_MSG("SECO HSM: unknown handle");
            break;

        case HSM_UNKNOWN_KEY_STORE:
            WOLFSSL_MSG("SECO HSM: unknown key store");
            break;

        case HSM_KEY_STORE_AUTH:
            WOLFSSL_MSG("SECO HSM: key store auth error");
            break;

        case HSM_KEY_STORE_ERROR:
            WOLFSSL_MSG("SECO HSM: key store error");
            break;

        case HSM_ID_CONFLICT:
            WOLFSSL_MSG("SECO HSM: id conflict");
            break;

        case HSM_RNG_NOT_STARTED:
            WOLFSSL_MSG("SECO HSM: RNG not started");
            break;

        case HSM_CMD_NOT_SUPPORTED:
            WOLFSSL_MSG("SECO HSM: CMD not support");
            break;

        case HSM_INVALID_LIFECYCLE:
            WOLFSSL_MSG("SECO HSM: invalid lifecycle");
            break;

        case HSM_KEY_STORE_CONFLICT:
            WOLFSSL_MSG("SECO HSM: store conflict");
            break;

        case HSM_KEY_STORE_COUNTER:
            WOLFSSL_MSG("SECO HSM: key store counter error");
            break;

        case HSM_FEATURE_NOT_SUPPORTED:
            WOLFSSL_MSG("SECO HSM: feature not supported");
            break;

        case HSM_SELF_TEST_FAILURE:
            WOLFSSL_MSG("SECO HSM: self test failure");
            break;

        case HSM_NOT_READY_RATING:
            WOLFSSL_MSG("SECO HSM: not ready");
            break;

        case HSM_FEATURE_DISABLED:
            WOLFSSL_MSG("SECO HSM: feature is disabled error");
            break;

        case HSM_GENERAL_ERROR:
            WOLFSSL_MSG("SECO HSM: general error found");
            break;

        default:
            WOLFSSL_MSG("SECO HSM: unknown error value found");
    }

    if (current != 0) {
        WOLFSSL_MSG("In an error state before SECO HSM error");
        ret = current;
    }

    return ret;
}


/* Do a synchronous operations and block till done
 * returns Success on success */
int SynchronousSendRequest(int type, unsigned int args[4], CAAM_BUFFER *buf,
        int sz)
{
    int ret = 0;
    hsm_err_t err = HSM_NO_ERROR;
    CAAM_ADDRESS pubkey, privkey;

    switch (type) {
    case CAAM_ENTROPY:
        err = wc_SECO_RNG(args, buf, sz);
        break;

    case CAAM_SHA224:
    case CAAM_SHA256:
    case CAAM_SHA384:
    case CAAM_SHA512:
        err = wc_SECO_Hash(args, buf, sz, type);
        break;

    case CAAM_GET_PART:
    case CAAM_FREE_PART:
    case CAAM_FIND_PART:
    case CAAM_READ_PART:
    case CAAM_WRITE_PART:
        break;

    case CAAM_ECDSA_KEYPAIR:
        err = wc_SECO_ECDSA_Make(args, buf, sz);
        break;

    case CAAM_ECDSA_VERIFY:
        err = wc_SECO_ECDSA_Verify(args, buf, sz);
        break;

    case CAAM_ECDSA_SIGN:
        err = wc_SECO_ECDSA_Sign(args, buf, sz);
        break;

    case CAAM_ECDSA_ECDH:
        break;

    case CAAM_BLOB_ENCAP:
    case CAAM_BLOB_DECAP:
        break;

    case CAAM_AESECB:
        err = wc_SECO_AESECB(args, buf, sz);
        break;

    case CAAM_AESCBC:
        err = wc_SECO_AESCBC(args, buf, sz);
        break;

    case CAAM_AESCCM:
        err = wc_SECO_AESCCM(args, buf, sz);
        break;

    case CAAM_AESGCM:
        err = wc_SECO_AESGCM(args, buf, sz);
        break;

    case CAAM_CMAC:
        err = wc_SECO_CMAC(args, buf, sz);
        break;

    case CAAM_FIFO_S:
    default:
        WOLFSSL_MSG("Unknown/unsupported type");
        ret = -1;
    }

    (void)pubkey;
    (void)privkey;
    (void)sz;
    return wc_TranslateHSMError(ret, err);
}
#endif /* WOLFSSL_SECO_CAAM */

