/* ssl_certman.c
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

 #include <wolfssl/internal.h>

#if !defined(WOLFSSL_SSL_CERTMAN_INCLUDED)
    #ifndef WOLFSSL_IGNORE_FILE_WARN
        #warning ssl_certman.c does not need to be compiled separately from ssl.c
    #endif
#else

#ifndef NO_CERTS

/* Pick an available TLS method.
 *
 * Used when creating temporary WOLFSSL_CTX.
 *
 * @return  A TLS method on success.
 * @return  NULL when no TLS method built into wolfSSL.
 */
static WC_INLINE WOLFSSL_METHOD* cm_pick_method(void)
{
    #ifndef NO_WOLFSSL_CLIENT
        #if !defined(NO_OLD_TLS) && defined(WOLFSSL_ALLOW_SSLV3)
            return wolfSSLv3_client_method();
        #elif !defined(NO_OLD_TLS) && defined(WOLFSSL_ALLOW_TLSV10)
            return wolfTLSv1_client_method();
        #elif !defined(NO_OLD_TLS)
            return wolfTLSv1_1_client_method();
        #elif !defined(WOLFSSL_NO_TLS12)
            return wolfTLSv1_2_client_method();
        #elif defined(WOLFSSL_TLS13)
            return wolfTLSv1_3_client_method();
        #else
            return NULL;
        #endif
    #elif !defined(NO_WOLFSSL_SERVER)
        #if !defined(NO_OLD_TLS) && defined(WOLFSSL_ALLOW_SSLV3)
            return wolfSSLv3_server_method();
        #elif !defined(NO_OLD_TLS) && defined(WOLFSSL_ALLOW_TLSV10)
            return wolfTLSv1_server_method();
        #elif !defined(NO_OLD_TLS)
            return wolfTLSv1_1_server_method();
        #elif !defined(WOLFSSL_NO_TLS12)
            return wolfTLSv1_2_server_method();
        #elif defined(WOLFSSL_TLS13)
            return wolfTLSv1_3_server_method();
        #else
            return NULL;
        #endif
    #else
        return NULL;
    #endif
}

/* Create a new certificate manager with a heap hint.
 *
 * @param [in] heap  Heap hint.
 * @return  Certificate manager object on success.
 * @return  NULL on failure.
 */
WOLFSSL_CERT_MANAGER* wolfSSL_CertManagerNew_ex(void* heap)
{
    int err = 0;
    WOLFSSL_CERT_MANAGER* cm;

    WOLFSSL_ENTER("wolfSSL_CertManagerNew");

    /* Allocate memory for certificate manager. */
    cm = (WOLFSSL_CERT_MANAGER*)XMALLOC(sizeof(WOLFSSL_CERT_MANAGER), heap,
        DYNAMIC_TYPE_CERT_MANAGER);
    if (cm == NULL) {
        err = 1;
    }
    if (!err) {
        /* Reset all fields. */
        XMEMSET(cm, 0, sizeof(WOLFSSL_CERT_MANAGER));

        /* Create a mutex for use when modify table of stored CAs. */
        if (wc_InitMutex(&cm->caLock) != 0) {
            WOLFSSL_MSG("Bad mutex init");
            err = 1;
        }
    }
    if (!err) {
        /* Initialize reference count. */
        wolfSSL_RefInit(&cm->ref, &err);
    #ifdef WOLFSSL_REFCNT_ERROR_RETURN
        if (err != 0) {
            WOLFSSL_MSG("Bad reference count init");
        }
    #endif
    }
#ifdef WOLFSSL_TRUST_PEER_CERT
    /* Create a mutex for use when modify table of trusted peers. */
    if ((!err) && (wc_InitMutex(&cm->tpLock) != 0)) {
        WOLFSSL_MSG("Bad mutex init");
        err = 1;
    }
#endif
    if (!err) {
        /* Set default minimum key sizes allowed. */
    #ifndef NO_RSA
        cm->minRsaKeySz = MIN_RSAKEY_SZ;
    #endif
    #ifdef HAVE_ECC
        cm->minEccKeySz = MIN_ECCKEY_SZ;
    #endif
    #ifdef HAVE_PQC
    #ifdef HAVE_FALCON
        cm->minFalconKeySz = MIN_FALCONKEY_SZ;
    #endif /* HAVE_FALCON */
    #ifdef HAVE_DILITHIUM
        cm->minDilithiumKeySz = MIN_DILITHIUMKEY_SZ;
    #endif /* HAVE_DILITHIUM */
    #endif /* HAVE_PQC */

        /* Set heap hint to use in certificate manager operations. */
        cm->heap = heap;
    }

    /* Dispose of certificate manager on error. */
    if (err && (cm != NULL)) {
        wolfSSL_CertManagerFree(cm);
        cm = NULL;
    }
    return cm;
}

/* Create a new certificate manager.
 *
 * @return  Certificate manager object on success.
 * @return  NULL on failure.
 */
WOLFSSL_CERT_MANAGER* wolfSSL_CertManagerNew(void)
{
    /* No heap hint. */
    return wolfSSL_CertManagerNew_ex(NULL);
}

/* Dispose of certificate manager.
 *
 * @param [in, out] cm  Certificate manager.
 */
void wolfSSL_CertManagerFree(WOLFSSL_CERT_MANAGER* cm)
{
    WOLFSSL_ENTER("wolfSSL_CertManagerFree");

    /* Validate parameter. */
    if (cm != NULL) {
        int doFree = 0;
        int ret;

        /* Decrement reference count and check if value is 0. */
        wolfSSL_RefDec(&cm->ref, &doFree, &ret);
    #ifdef WOLFSSL_REFCNT_ERROR_RETURN
        if (ret != 0) {
            WOLFSSL_MSG("Couldn't lock cm mutex");
        }
    #else
        (void)ret;
    #endif
        if (doFree) {
        #ifdef HAVE_CRL
            /* Dispose of CRL handler. */
            if (cm->crl != NULL) {
                /* Dispose of CRL object - indicating dynamically allocated. */
                FreeCRL(cm->crl, 1);
            }
        #endif

    #ifdef HAVE_OCSP
            /* Dispose of OCSP handler. */
            if (cm->ocsp != NULL) {
                FreeOCSP(cm->ocsp, 1);
            }
            /* Dispose of URL. */
            XFREE(cm->ocspOverrideURL, cm->heap, DYNAMIC_TYPE_URL);
        #if !defined(NO_WOLFSSL_SERVER) && \
            (defined(HAVE_CERTIFICATE_STATUS_REQUEST) || \
             defined(HAVE_CERTIFICATE_STATUS_REQUEST_V2))
            /* Dispose of OCSP stapling handler. */
            if (cm->ocsp_stapling) {
                FreeOCSP(cm->ocsp_stapling, 1);
            }
        #endif
    #endif /* HAVE_OCSP */

            /* Dispose of CA table and mutex. */
            FreeSignerTable(cm->caTable, CA_TABLE_SIZE, cm->heap);
            wc_FreeMutex(&cm->caLock);

        #ifdef WOLFSSL_TRUST_PEER_CERT
            /* Dispose of trusted peer table and mutex. */
            FreeTrustedPeerTable(cm->tpTable, TP_TABLE_SIZE, cm->heap);
            wc_FreeMutex(&cm->tpLock);
        #endif

            /* Dispose of reference count. */
            wolfSSL_RefFree(&cm->ref);
            /* Dispose of certificate manager memory. */
            XFREE(cm, cm->heap, DYNAMIC_TYPE_CERT_MANAGER);
        }
    }
}

/* Increase reference count on certificate manager.
 *
 * @param [in, out] cm  Certificate manager.
 * @return  WOLFSSL_SUCCESS on success.
 * @return  0 when cm is NULL or locking mutex fails.
 */
int wolfSSL_CertManager_up_ref(WOLFSSL_CERT_MANAGER* cm)
{
    int ret = WOLFSSL_SUCCESS;

    /* Validate parameter. */
    if (cm == NULL) {
        ret = 0;
    }
    if (ret == WOLFSSL_SUCCESS) {
        int err;

        /* Increment reference. */
        wolfSSL_RefInc(&cm->ref, &err);
    #ifdef WOLFSSL_REFCNT_ERROR_RETURN
        if (err) {
            WOLFSSL_MSG("Failed to lock cm mutex");
            ret = 0;
        }
    #else
        (void)err;
    #endif
    }

    return ret;
}

#if defined(OPENSSL_EXTRA) && !defined(NO_FILESYSTEM)
#if defined(WOLFSSL_SIGNER_DER_CERT)
static WC_INLINE int wolfssl_cm_get_certs_der(WOLFSSL_CERT_MANAGER* cm,
    DerBuffer*** buffers, int* cnt)
{
    int err = 0;
    Signer* signers = NULL;
    DerBuffer** certBuffers = NULL;
    int i = 0;
    word32 row = 0;
    int numCerts = 0;

    /* Iterate once to get the number of certs, for memory allocation
     * purposes. */
    for (row = 0; row < CA_TABLE_SIZE; row++) {
        /* Get signer information of CAs in a row. */
        signers = cm->caTable[row];
        /* Count each signer in row that has a DER certificate buffer. */
        while ((signers != NULL) && (signers->derCert != NULL) &&
                (signers->derCert->buffer != NULL)) {
            ++numCerts;
            signers = signers->next;
        }
    }
    /* Check we found certificates. */
    if (numCerts == 0) {
        err = 1;
    }

    if (!err) {
        /* Allocate memory for pointers to each DER buffer. */
        certBuffers = (DerBuffer**)XMALLOC(
            sizeof(DerBuffer*) * (size_t)numCerts, cm->heap,
            DYNAMIC_TYPE_TMP_BUFFER);
        if (certBuffers == NULL) {
            err = 1;
        }
    }
    if (!err) {
        /* Reset pointers. */
        XMEMSET(certBuffers, 0, sizeof(DerBuffer*) * (size_t)numCerts);
    }

    /* Copy the certs locally so that we can release the caLock. If the lock
     * is held when wolfSSL_d2i_X509 is called, GetCA will also try to get
     * the lock, leading to deadlock. */
    for (row = 0; (!err) && (row < CA_TABLE_SIZE); row++) {
        /* Get signer information of CAs in a row. */
        signers = cm->caTable[row];
        /* Copy each DER certificate buffer of signers in a row. */
        while ((signers != NULL) && (signers->derCert != NULL) &&
                (signers->derCert->buffer != NULL)) {
            /* Allocate memory to hold DER certificate buffer. */
            int ret = AllocDer(&certBuffers[i], signers->derCert->length,
                CA_TYPE, cm->heap);
            if (ret < 0) {
                err = 1;
                break;
            }

            /* Copy buffer into array element. */
            XMEMCPY(certBuffers[i]->buffer, signers->derCert->buffer,
                signers->derCert->length);
            certBuffers[i]->length = signers->derCert->length;

            /* Store in next index. */
            ++i;
            /* Move on to next signer in row. */
            signers = signers->next;
        }
    }

    *buffers = certBuffers;
    *cnt = numCerts;
    return err;
}

/* Retrieve stack of X509 certificates in a certificate manager (CM).
 *
 * @param [in] cm  Certificate manager.
 *
 * @return  Stack of X509 certs on success
 * @return  NULL on failure.
 */
WOLFSSL_STACK* wolfSSL_CertManagerGetCerts(WOLFSSL_CERT_MANAGER* cm)
{
    WOLFSSL_STACK* sk = NULL;
    int numCerts = 0;
    DerBuffer** certBuffers = NULL;
    int i = 0;
    int err = 0;

    WOLFSSL_ENTER("wolfSSL_CertManagerGetCerts");

    /* Validate parameter. */
    if (cm == NULL) {
        err = 1;
    }
    if (!err) {
        /* Create an empty certificate stack to return. */
        sk = wolfSSL_sk_X509_new_null();
        if (sk == NULL) {
            err = 1;
        }
    }
    /* Lock CA table. */
    if ((!err) && (wc_LockMutex(&cm->caLock) != 0)) {
        err = 1;
    }
    if (!err) {
        err = wolfssl_cm_get_certs_der(cm, &certBuffers, &numCerts);
        /* Release CA lock. */
        wc_UnLockMutex(&cm->caLock);
    }

    /* Put each DER certificate buffer into a stack of WOLFSSL_X509 */
    for (i = 0; (!err) && (i < numCerts); ++i) {
        const byte* derBuffer = NULL;
        WOLFSSL_X509* x509 = NULL;

        /* Get pointer to DER encoding of certificate. */
        derBuffer = certBuffers[i]->buffer;
        /* Decode certificate. */
        wolfSSL_d2i_X509(&x509, &derBuffer, (int)certBuffers[i]->length);
        if (x509 == NULL) {
            err = 1;
        }

        /* Decode certificate. */
        if ((!err) && (wolfSSL_sk_X509_push(sk, x509) != WOLFSSL_SUCCESS)) {
            wolfSSL_X509_free(x509);
            err = 1;
        }
    }

    if (certBuffers != NULL) {
        /* Dispose of temporary cert storage (for access outside of lock). */
        for (i = 0; i < numCerts && certBuffers[i] != NULL; ++i) {
            FreeDer(&certBuffers[i]);
        }
        XFREE(certBuffers, cm->heap, DYNAMIC_TYPE_TMP_BUFFER);
    }

    /* Dispose of stack of certificates on error. */
    if (err && (sk != NULL)) {
        wolfSSL_sk_X509_pop_free(sk, NULL);
        sk = NULL;
    }
    return sk;
}

#endif /* WOLFSSL_SIGNER_DER_CERT */
#endif /* OPENSSL_EXTRA && !NO_FILESYSTEM */

/* Unload the CA signer table.
 *
 * @param [in] cm  Certificate manager.
 * @return  WOLFSSL_SUCCESS on success.
 * @return  BAD_FUNC_ARG when cm is NULL.
 * @return  BAD_MUTEX_E when locking fails.
 */
int wolfSSL_CertManagerUnloadCAs(WOLFSSL_CERT_MANAGER* cm)
{
    int ret = WOLFSSL_SUCCESS;

    WOLFSSL_ENTER("wolfSSL_CertManagerUnloadCAs");

    /* Validate parameter. */
    if (cm == NULL) {
        ret = BAD_FUNC_ARG;
    }
    /* Lock CA table. */
    if ((ret == WOLFSSL_SUCCESS) && (wc_LockMutex(&cm->caLock) != 0)) {
        ret = BAD_MUTEX_E;
    }
    if (ret == WOLFSSL_SUCCESS) {
        /* Dispose of CA table. */
        FreeSignerTable(cm->caTable, CA_TABLE_SIZE, cm->heap);

        /* Unlock CA table. */
        wc_UnLockMutex(&cm->caLock);
    }

    return ret;
}


#ifdef WOLFSSL_TRUST_PEER_CERT
/* Unload the trusted peers table.
 *
 * @param [in] cm  Certificate manager.
 * @return  WOLFSSL_SUCCESS on success.
 * @return  BAD_FUNC_ARG when cm is NULL.
 * @return  BAD_MUTEX_E when locking fails.
 */
int wolfSSL_CertManagerUnload_trust_peers(WOLFSSL_CERT_MANAGER* cm)
{
    int ret = WOLFSSL_SUCCESS;

    WOLFSSL_ENTER("wolfSSL_CertManagerUnload_trust_peers");

    /* Validate parameter. */
    if (cm == NULL) {
        ret = BAD_FUNC_ARG;
    }
    /* Lock trusted peers table. */
    if ((ret == WOLFSSL_SUCCESS) && (wc_LockMutex(&cm->tpLock) != 0)) {
        ret = BAD_MUTEX_E;
    }
    if (ret == WOLFSSL_SUCCESS) {
        /* Dispose of trusted peers table. */
        FreeTrustedPeerTable(cm->tpTable, TP_TABLE_SIZE, cm->heap);

        /* Unlock trusted peers table. */
        wc_UnLockMutex(&cm->tpLock);
    }

    return ret;
}
#endif /* WOLFSSL_TRUST_PEER_CERT */

/* Load certificate/s from buffer with flags.
 *
 * @param [in] cm         Certificate manager.
 * @param [in] buff       Buffer holding encoding of certificate.
 * @param [in] sz         Length in bytes of data in buffer.
 * @param [in] format     Format of encoding. Valid values:
 *                          WOLFSSL_FILETYPE_ASN1, WOLFSSL_FILETYPE_PEM.
 * @param [in] userChain  Indicates buffer holds chain of certificates.
 * @param [in] flags      Flags to modify behaviour of loading. Valid flags:
 *                          WOLFSSL_LOAD_FLAG_IGNORE_ERR,
 *                          WOLFSSL_LOAD_FLAG_DATE_ERR_OKAY,
 *                          WOLFSSL_LOAD_FLAG_PEM_CA_ONLY,
 *                          WOLFSSL_LOAD_FLAG_IGNORE_BAD_PATH_ERR, and
 *                          WOLFSSL_LOAD_FLAG_IGNORE_ZEROFILE.
 * @return  WOLFSSL_SUCCESS on success.
 * @return  WOLFSSL_FATAL_ERROR when cm is NULL or failed create WOLFSSL_CTX.
 * @return  Other values on loading failure.
 */
int wolfSSL_CertManagerLoadCABuffer_ex(WOLFSSL_CERT_MANAGER* cm,
    const unsigned char* buff, long sz, int format, int userChain, word32 flags)
{
    int ret = WOLFSSL_SUCCESS;
    WOLFSSL_CTX* tmp = NULL;

    WOLFSSL_ENTER("wolfSSL_CertManagerLoadCABuffer_ex");

    /* Validate parameters. */
    if (cm == NULL) {
        WOLFSSL_MSG("No CertManager error");
        ret = WOLFSSL_FATAL_ERROR;
    }
    /* Allocate a temporary WOLFSSL_CTX to load with. */
    if ((ret == WOLFSSL_SUCCESS) && ((tmp = wolfSSL_CTX_new(cm_pick_method()))
            == NULL)) {
        WOLFSSL_MSG("CTX new failed");
        ret = WOLFSSL_FATAL_ERROR;
    }
    if (ret == WOLFSSL_SUCCESS) {
        /* Some configurations like OPENSSL_COMPATIBLE_DEFAULTS may turn off
         * verification by default. Let's restore our desired defaults. */
        wolfSSL_CTX_set_verify(tmp, WOLFSSL_VERIFY_DEFAULT, NULL);

        /* Replace certificate manager with one to load certificate/s into. */
        wolfSSL_CertManagerFree(tmp->cm);
        tmp->cm = cm;

        /* Load certificate buffer. */
        ret = wolfSSL_CTX_load_verify_buffer_ex(tmp, buff, sz, format,
            userChain, flags);

        /* Clear certificate manager in WOLFSSL_CTX so it won't be freed. */
        tmp->cm = NULL;
    }

    /* Dispose of temporary WOLFSSL_CTX. */
    wolfSSL_CTX_free(tmp);
    return ret;
}

/* Load certificate/s from buffer into table.
 *
 * Uses default load verification flags and is not a user chain.
 *
 * @param [in] cm         Certificate manager.
 * @param [in] buff       Buffer holding encoding of certificate.
 * @param [in] sz         Length in bytes of data in buffer.
 * @param [in] format     Format of encoding. Valid values:
 *                          WOLFSSL_FILETYPE_ASN1, WOLFSSL_FILETYPE_PEM.
 * @return  WOLFSSL_SUCCESS on success.
 * @return  WOLFSSL_FATAL_ERROR when cm is NULL or failed create WOLFSSL_CTX.
 * @return  Other values on loading failure.
 */
int wolfSSL_CertManagerLoadCABuffer(WOLFSSL_CERT_MANAGER* cm,
    const unsigned char* buff, long sz, int format)
{
    return wolfSSL_CertManagerLoadCABuffer_ex(cm, buff, sz, format, 0,
        WOLFSSL_LOAD_VERIFY_DEFAULT_FLAGS);
}

#ifndef NO_WOLFSSL_CM_VERIFY
/* Set the verification callback into certificate manager.
 *
 * @param [in] cm  Certificate manager.
 * @param [in] vc  Verification callback.
 */
void wolfSSL_CertManagerSetVerify(WOLFSSL_CERT_MANAGER* cm, VerifyCallback vc)
{
    WOLFSSL_ENTER("wolfSSL_CertManagerSetVerify");
    if (cm != NULL) {
        cm->verifyCallback = vc;
    }
}
#endif /* NO_WOLFSSL_CM_VERIFY */

#if !defined(NO_WOLFSSL_CLIENT) || !defined(WOLFSSL_NO_CLIENT_AUTH)
/* Verify the certificate.
 *
 * Uses the verification callback if available.
 *
 * @param [in] cm        Certificate manager.
 * @param [in] buff      Buffer holding encoded certificate.
 * @param [in] sz        Size in bytes of data in buffer.
 * @param [in] format    Format of encoding. Valid values:
 *                         WOLFSSL_FILETYPE_ASN1, WOLFSSL_FILETYPE_PEM.
 * @param [in] prev_err  Previous error. Passed to callback.
 * @return  WOLFSSL_SUCCESS on success.
 * @return  MEMORY_E when dynamic memory allocation fails.
 * @return  NOT_COMPILED_IN when converting from PEM to DER is not a feature of
 *          the wolfSSL build.
 */
int CM_VerifyBuffer_ex(WOLFSSL_CERT_MANAGER* cm, const unsigned char* buff,
    long sz, int format, int prev_err)
{
    int ret = 0;
    int fatal = 0;
    DerBuffer* der = NULL;
#ifdef WOLFSSL_SMALL_STACK
    DecodedCert* cert = NULL;
#else
    DecodedCert  cert[1];
#endif

    WOLFSSL_ENTER("CM_VerifyBuffer_ex");

    (void)prev_err;

#ifdef WOLFSSL_SMALL_STACK
    /* Allocate memory for decoded certificate. */
    cert = (DecodedCert*)XMALLOC(sizeof(DecodedCert), cm->heap,
         DYNAMIC_TYPE_DCERT);
    if (cert == NULL) {
        ret = MEMORY_E;
        fatal = 1;
    }
    if (ret == 0)
#endif
    {
        /* Reset fields of decoded certificate. */
        XMEMSET(cert, 0, sizeof(DecodedCert));

        if (format == WOLFSSL_FILETYPE_PEM) {
        #ifndef WOLFSSL_PEM_TO_DER
            ret = NOT_COMPILED_IN;
            fatal = 1;
        #else
            /* Convert to DER from PEM. */
            ret = PemToDer(buff, sz, CERT_TYPE, &der, cm->heap, NULL, NULL);
            if (ret != 0) {
                fatal = 1;
            }
            else {
                /* Replace buffer pointer and size with DER buffer. */
                buff = der->buffer;
                sz = (long)der->length;
            }
        #endif
        }
    }
    if (ret == 0) {
        /* Create a decoded certificate with DER buffer. */
        InitDecodedCert(cert, buff, (word32)sz, cm->heap);

        /* Parse DER into decoded certificate fields and verify signature
         * against a known CA. */
        ret = ParseCertRelative(cert, CERT_TYPE, VERIFY, cm);
     }

#ifdef HAVE_CRL
    if ((ret == 0) && cm->crlEnabled) {
        /* Check for a CRL for the CA and check validity of certificate. */
        ret = CheckCertCRL(cm->crl, cert);
    }
#endif

    (void)fatal;

#ifndef NO_WOLFSSL_CM_VERIFY
    /* Use callback to perform verification too if available. */
    if ((!fatal) && cm->verifyCallback) {
    #ifdef WOLFSSL_SMALL_STACK
        ProcPeerCertArgs* args;
    #else
        ProcPeerCertArgs  args[1];
    #endif
        buffer certBuf;

    #ifdef WOLFSSL_SMALL_STACK
        /* Allocate memory for object to hold arguments for callback. */
        args = (ProcPeerCertArgs*)XMALLOC(sizeof(ProcPeerCertArgs), cm->heap,
            DYNAMIC_TYPE_TMP_BUFFER);
        if (args == NULL) {
            ret = MEMORY_E;
            fatal = 1;
        }
        if (!fatal)
    #endif
        {
            XMEMSET(args, 0, sizeof(ProcPeerCertArgs));

            /* DER encoding. */
            certBuf.buffer = (byte*)buff;
            certBuf.length = (unsigned int)sz;

            /* One certificate available. */
            args->totalCerts = 1;
            args->certs = &certBuf;
            args->dCert = cert;
            args->dCertInit = 1;

            /* Replace value in ret with an error value passed in. */
            if (prev_err != 0) {
                ret = prev_err;
            }
            /* Use callback to verify certificate. */
            ret = DoVerifyCallback(cm, NULL, ret, args);
        }
    #ifdef WOLFSSL_SMALL_STACK
        /* Dispose of allocated callback args. */
        XFREE(args, cm->heap, DYNAMIC_TYPE_TMP_BUFFER);
    #endif
    }
#endif

    /* Dispose of allocated memory. */
    FreeDecodedCert(cert);
    FreeDer(&der);
#ifdef WOLFSSL_SMALL_STACK
    XFREE(cert, cm->heap, DYNAMIC_TYPE_DCERT);
#endif

    /* Convert the ret value to a return value. */
    return (ret == 0) ? WOLFSSL_SUCCESS : ret;
}

/* Verify the certificate.
 *
 * Uses the verification callback if available.
 *
 * @param [in] cm        Certificate manager.
 * @param [in] buff      Buffer holding encoded certificate.
 * @param [in] sz        Size in bytes of data in buffer.
 * @param [in] format    Format of encoding. Valid values:
 *                         WOLFSSL_FILETYPE_ASN1, WOLFSSL_FILETYPE_PEM.
 * @param [in] prev_err  Previous error. Passed to callback.
 * @return  WOLFSSL_SUCCESS on success.
 * @return  BAD_FUNC_ARG when cm or buff is NULL ot sz is negative or zero.
 * @return  WOLFSSL_BAD_FILETYPE when format is invalid.
 * @return  MEMORY_E when dynamic memory allocation fails.
 * @return  NOT_COMPILED_IN when converting from PEM to DER is not a feature of
 *          the wolfSSL build.
 */
int wolfSSL_CertManagerVerifyBuffer(WOLFSSL_CERT_MANAGER* cm,
    const unsigned char* buff, long sz, int format)
{
    int ret;

    WOLFSSL_ENTER("wolfSSL_CertManagerVerifyBuffer");

    /* Validate parameters. */
    if ((cm == NULL) || (buff == NULL) || (sz <= 0)) {
        ret = BAD_FUNC_ARG;
    }
    else if ((format != WOLFSSL_FILETYPE_ASN1) &&
             (format != WOLFSSL_FILETYPE_PEM)) {
        ret = WOLFSSL_BAD_FILETYPE;
    }
    else {
        /* No previous error. */
        ret = CM_VerifyBuffer_ex(cm, buff, sz, format, 0);
    }

    return ret;
}
#endif /* !NO_WOLFSSL_CLIENT || !WOLFSSL_NO_CLIENT_AUTH */

#ifndef NO_FILESYSTEM

#if !defined(NO_WOLFSSL_CLIENT) || !defined(WOLFSSL_NO_CLIENT_AUTH)
/* Verify the certificate loaded from a file.
 *
 * Uses the verification callback if available.
 *
 * @param [in] cm        Certificate manager.
 * @param [in] format    Format of encoding. Valid values:
 *                         WOLFSSL_FILETYPE_ASN1, WOLFSSL_FILETYPE_PEM.
 * @param [in] prev_err  Previous error. Passed to callback.
 * @return  WOLFSSL_SUCCESS on success.
 * @return  BAD_FUNC_ARG when cm or buff is NULL ot sz is negative.
 * @return  WOLFSSL_BAD_FILETYPE when format is invalid.
 * @return  WOLFSSL_BAD_FILE when reading the certificate file fails.
 * @return  MEMORY_E when dynamic memory allocation fails.
 * @return  NOT_COMPILED_IN when converting from PEM to DER is not a feature of
 *          the wolfSSL build.
 */
int wolfSSL_CertManagerVerify(WOLFSSL_CERT_MANAGER* cm, const char* fname,
    int format)
{
    int    ret = WOLFSSL_SUCCESS;
#ifndef WOLFSSL_SMALL_STACK
    byte   staticBuffer[FILE_BUFFER_SIZE];
#endif
    byte*  buff;
    long   sz = 0;
    XFILE  file = XBADFILE;

    WOLFSSL_ENTER("wolfSSL_CertManagerVerify");

#ifndef WOLFSSL_SMALL_STACK
    buff = staticBuffer;
#endif

    /* Validate parameters. cm and format validated in:
     *   wolfSSL_CertManagerVerifyBuffer */
    if ((cm == NULL) || (fname == NULL)) {
        ret = BAD_FUNC_ARG;
    }

    /* Open the file containing a certificate. */
    if ((ret == WOLFSSL_SUCCESS) &&
            ((file = XFOPEN(fname, "rb")) == XBADFILE)) {
        ret = WOLFSSL_BAD_FILE;
    }
    /* Get the length of the file. */
    if (ret == WOLFSSL_SUCCESS) {
        ret = wolfssl_file_len(file, &sz);
        if (ret == 0) {
            ret = WOLFSSL_SUCCESS;
        }
    }
    /* Allocate dynamic memory for file contents if no static buffer or too
     * small. */
#ifndef WOLFSSL_SMALL_STACK
    if ((ret == WOLFSSL_SUCCESS) && (sz > (long)sizeof(staticBuffer)))
#endif
    {
        WOLFSSL_MSG("Getting dynamic buffer");
        buff = (byte*)XMALLOC((size_t)sz, cm->heap, DYNAMIC_TYPE_FILE);
        if (buff == NULL) {
            ret = WOLFSSL_BAD_FILE;
        }
    }
    /* Read all the file into buffer. */
    if ((ret == WOLFSSL_SUCCESS) && (XFREAD(buff, 1, (size_t)sz, file) !=
            (size_t)sz)) {
        ret = WOLFSSL_BAD_FILE;
    }
    /* Close file if opened. */
    if (file != XBADFILE) {
        XFCLOSE(file);
    }
    if (ret == WOLFSSL_SUCCESS) {
        /* Verify the certificate read. */
        ret = wolfSSL_CertManagerVerifyBuffer(cm, buff, sz, format);
    }

    /* Dispose of buffer if it was allocated. */
#ifndef WOLFSSL_SMALL_STACK
    if (buff != staticBuffer)
#endif
    {
        XFREE(buff, cm->heap, DYNAMIC_TYPE_FILE);
    }
    return ret;
}
#endif

/* Load the CA file and/or certificate files in a path.
 *
 * @param [in] cm    Certificate manager.
 * @param [in] file  Name of CA file.
 * @param [in] path  Path to a directory containing certificates.
 * @return  WOLFSSL_SUCCESS on success.
 * @return  WOLFSSL_FATAL_ERROR when cm is NULL or unable to create WOLFSSL_CTX.
 * @return  Otherwise failure.
 */
int wolfSSL_CertManagerLoadCA(WOLFSSL_CERT_MANAGER* cm, const char* file,
                             const char* path)
{
    int ret = WOLFSSL_SUCCESS;
    WOLFSSL_CTX* tmp = NULL;

    WOLFSSL_ENTER("wolfSSL_CertManagerLoadCA");

    /* Validate parameters. file and path validated in:
     *   wolfSSL_CTX_load_verify_locations*/
    if (cm == NULL) {
        WOLFSSL_MSG("No CertManager error");
        ret = WOLFSSL_FATAL_ERROR;
    }
    /* Create temporary WOLFSSL_CTX. */
    if ((ret == WOLFSSL_SUCCESS) && ((tmp = wolfSSL_CTX_new(cm_pick_method()))
            == NULL)) {
        WOLFSSL_MSG("CTX new failed");
        ret = WOLFSSL_FATAL_ERROR;
    }
    if (ret == WOLFSSL_SUCCESS) {
        /* Some configurations like OPENSSL_COMPATIBLE_DEFAULTS may turn off
         * verification by default. Let's restore our desired defaults. */
        wolfSSL_CTX_set_verify(tmp, WOLFSSL_VERIFY_DEFAULT, NULL);

        /* Replace certificate manager with one to load certificate/s into. */
        wolfSSL_CertManagerFree(tmp->cm);
        tmp->cm = cm;

        /* Load certificate from file and path. */
        ret = wolfSSL_CTX_load_verify_locations(tmp, file, path);

        /* Clear certificate manager in WOLFSSL_CTX so it won't be freed. */
        tmp->cm = NULL;
    }

    /* Dispose of temporary WOLFSSL_CTX. */
    wolfSSL_CTX_free(tmp);
    return ret;
}

#endif /* NO_FILESYSTEM */

#if defined(PERSIST_CERT_CACHE)

/* Version of layout of cache of CA certificates. */
#define WOLFSSL_CACHE_CERT_VERSION 1

/* CA certificates cache information. */
typedef struct {
    /* Cache certificate layout version id. */
    int version;
    /* Number of hash table rows. Maximum of CA_TABLE_SIZE. */
    int rows;
    /* Number of columns per row. */
    int columns[CA_TABLE_SIZE];
    /* Size of Signer object. */
    int signerSz;
} CertCacheHeader;

/* current cert persistence layout is:

   1) CertCacheHeader
   2) caTable

   update WOLFSSL_CERT_CACHE_VERSION if change layout for the following
   PERSIST_CERT_CACHE functions
*/


/* Return number of bytes of memory needed to persist this signer.
 *
 * Assumes we have locked CA table.
 *
 * @param [in] Signer  Signer entry in CA table.
 * @return  Number of bytes.
 */
static WC_INLINE int cm_get_signer_memory(Signer* signer)
{
    int sz = sizeof(signer->pubKeySize) + sizeof(signer->keyOID)
           + sizeof(signer->nameLen)    + sizeof(signer->subjectNameHash);

#if !defined(NO_SKID)
        sz += (int)sizeof(signer->subjectKeyIdHash);
#endif

    /* Add dynamic bytes needed. */
    sz += (int)signer->pubKeySize;
    sz += signer->nameLen;

    return sz;
}


/* Return number of bytes of memory needed to persist this row.
 *
 * Assumes we have locked CA table.
 *
 * @param [in] row  A row of signers from the CA table.
 * @return  Number of bytes.
 */
static WC_INLINE int cm_get_cert_cache_row_memory(Signer* row)
{
    int sz = 0;

    /* Each signer in row. */
    while (row != NULL) {
        /* Add in size of this signer. */
        sz += cm_get_signer_memory(row);
        row = row->next;
    }

    return sz;
}


/* Return the number of bytes of memory to persist cert cache.
 *
 * Assumes we have locked CA table.
 *
 * @param [in] cm  Certificate manager.
 * @return  Number of bytes.
 */
static WC_INLINE int cm_get_cert_cache_mem_size(WOLFSSL_CERT_MANAGER* cm)
{
    int sz;
    int i;

    sz = sizeof(CertCacheHeader);

    /* Each row in table. */
    for (i = 0; i < CA_TABLE_SIZE; i++) {
        /* Add in size of this row. */
        sz += cm_get_cert_cache_row_memory(cm->caTable[i]);
    }

    return sz;
}


/* Get count of columns for each row.
 *
 * Assumes we have locked CA table.
 *
 * @param [in] cm       Certificate manager.
 * @param [in] columns  Array of row counts.
 */
static WC_INLINE void cm_set_cert_header_Columns(WOLFSSL_CERT_MANAGER* cm,
    int* columns)
{
    int     i;
    Signer* row;

    /* Each row in table. */
    for (i = 0; i < CA_TABLE_SIZE; i++) {
        int count = 0;

        /* Get row from table. */
        row = cm->caTable[i];
        /* Each entry in row. */
        while (row != NULL) {
            /* Update count. */
            ++count;
            row = row->next;
        }
        /* Store row count. */
        columns[i] = count;
    }
}


/* Restore whole cert row from memory,
 *
 * Assumes we have locked CA table.
 *
 * @param [in] cm       Certificate manager.
 * @param [in] current  Buffer containing rows.
 * @param [in] row      Row number being restored.
 * @param [in] listSz   Number of entries in row.
 * @param [in] end      End of data in buffer.
 * @return  Number of bytes consumed on success.
 * @return  PARSE_ERROR when listSz is less than zero.
 * @return  BUFFER_E when buffer is too small.
 * @return  MEMORY_E when dynamic memory allocation fails.
 * @return  Negative value on error.
 */
static WC_INLINE int cm_restore_cert_row(WOLFSSL_CERT_MANAGER* cm,
    byte* current, int row, int listSz, const byte* end)
{
    int ret = 0;
    int idx = 0;

    /* Validate parameters. */
    if (listSz < 0) {
        WOLFSSL_MSG("Row header corrupted, negative value");
        ret = PARSE_ERROR;
    }

    /* Process all entries. */
    while ((ret == 0) && (listSz > 0)) {
        Signer* signer = NULL;
        byte*   publicKey;
        byte*   start = current + idx;  /* for end checks on this signer */
        int     minSz = sizeof(signer->pubKeySize) + sizeof(signer->keyOID) +
                      sizeof(signer->nameLen) + sizeof(signer->subjectNameHash);
        #ifndef NO_SKID
                minSz += (int)sizeof(signer->subjectKeyIdHash);
        #endif

        /* Check minimal size of bytes available. */
        if (start + minSz > end) {
            WOLFSSL_MSG("Would overread restore buffer");
            ret = BUFFER_E;
        }
        /* Make a new signer. */
        if ((ret == 0) && ((signer = MakeSigner(cm->heap)) == NULL)) {
            ret = MEMORY_E;
        }

        if (ret == 0) {
            /* Copy in public key size. */
            XMEMCPY(&signer->pubKeySize, current + idx,
                sizeof(signer->pubKeySize));
            idx += (int)sizeof(signer->pubKeySize);

            /* Copy in public key OID. */
            XMEMCPY(&signer->keyOID, current + idx, sizeof(signer->keyOID));
            idx += (int)sizeof(signer->keyOID);

            /* Check bytes available for public key. */
            if (start + minSz + signer->pubKeySize > end) {
                WOLFSSL_MSG("Would overread restore buffer");
                ret = BUFFER_E;
            }
        }
        if (ret == 0) {
            /* Allocate memory for public key to be stored in. */
            publicKey = (byte*)XMALLOC(signer->pubKeySize, cm->heap,
                DYNAMIC_TYPE_KEY);
            if (publicKey == NULL) {
                ret = MEMORY_E;
            }
        }

        if (ret == 0) {
            /* Copy in public key. */
            XMEMCPY(publicKey, current + idx, signer->pubKeySize);
            signer->publicKey = publicKey;
            idx += (int)signer->pubKeySize;

            /* Copy in certificate name length. */
            XMEMCPY(&signer->nameLen, current + idx, sizeof(signer->nameLen));
            idx += (int)sizeof(signer->nameLen);

            /* Check bytes available for certificate name. */
            if (start + minSz + signer->pubKeySize + signer->nameLen > end) {
                WOLFSSL_MSG("Would overread restore buffer");
                ret = BUFFER_E;
            }
        }
        if (ret == 0) {
            /* Allocate memory for public key to be stored in. */
            signer->name = (char*)XMALLOC((size_t)signer->nameLen, cm->heap,
                DYNAMIC_TYPE_SUBJECT_CN);
            if (signer->name == NULL) {
                ret = MEMORY_E;
            }
        }

        if (ret == 0) {
            /* Copy in certificate name. */
            XMEMCPY(signer->name, current + idx, (size_t)signer->nameLen);
            idx += signer->nameLen;

            /* Copy in hash of subject name. */
            XMEMCPY(signer->subjectNameHash, current + idx, SIGNER_DIGEST_SIZE);
            idx += SIGNER_DIGEST_SIZE;

        #ifndef NO_SKID
            /* Copy in hash of subject key. */
            XMEMCPY(signer->subjectKeyIdHash, current + idx,SIGNER_DIGEST_SIZE);
            idx += SIGNER_DIGEST_SIZE;
        #endif

            /* Make next Signer the head of the row. */
            signer->next = cm->caTable[row];
            /* Add Signer to start of row. */
            cm->caTable[row] = signer;

            /* Done one more Signer. */
            --listSz;
        }

        if ((ret != 0) && (signer != NULL)) {
            /* Dispose of allocated signer. */
            FreeSigner(signer, cm->heap);
        }
    }

    if (ret == 0) {
        /* Return the number of bytes used on success. */
        ret = idx;
    }
    return ret;
}


/* Store whole CA certificate row into memory.
 *
 * Assumes we have locked CA table.
 *
 * @param [in] cm       Certificate manager.
 * @param [in] current  Buffer to write to.
 * @param [in] row      Row number being stored.
 * @return  Number of bytes added.
 */
static WC_INLINE int cm_store_cert_row(WOLFSSL_CERT_MANAGER* cm, byte* current,
    int row)
{
    int     added  = 0;
    Signer* list;

    /* Get the row - a linked list. */
    list  = cm->caTable[row];
    /* Each certificate in row. */
    while (list != NULL) {
        /* Public key size. */
        XMEMCPY(current + added, &list->pubKeySize, sizeof(list->pubKeySize));
        added += (int)sizeof(list->pubKeySize);

        /* Public key OID. */
        XMEMCPY(current + added, &list->keyOID,     sizeof(list->keyOID));
        added += (int)sizeof(list->keyOID);

        /* Public key. */
        XMEMCPY(current + added, list->publicKey, (size_t)list->pubKeySize);
        added += (int)list->pubKeySize;

        /* Certificate name length. */
        XMEMCPY(current + added, &list->nameLen, sizeof(list->nameLen));
        added += (int)sizeof(list->nameLen);

        /* Certificate name. */
        XMEMCPY(current + added, list->name, (size_t)list->nameLen);
        added += list->nameLen;

        /* Hash of subject name. */
        XMEMCPY(current + added, list->subjectNameHash, SIGNER_DIGEST_SIZE);
        added += SIGNER_DIGEST_SIZE;

    #ifndef NO_SKID
        /* Hash of public key. */
        XMEMCPY(current + added, list->subjectKeyIdHash,SIGNER_DIGEST_SIZE);
        added += SIGNER_DIGEST_SIZE;
    #endif

        /* Next certificate in row. */
        list = list->next;
    }

    return added;
}


/* Persist CA certificate cache to memory.
 *
 * Assumes we have locked CA table.
 *
 * @param [in] cm   Certificate manager.
 * @param [in] mem  Memory to persist into.
 * @param [in] sz   Size in bytes of memory.
 * @return  WOLFSSL_SUCCESS on success.
 * @return  BUFFER_E when memory is too small.
 */
static WC_INLINE int cm_do_mem_save_cert_cache(WOLFSSL_CERT_MANAGER* cm,
    void* mem, int sz)
{
    int ret = WOLFSSL_SUCCESS;
    int realSz;
    int i;

    WOLFSSL_ENTER("cm_do_mem_save_cert_cache");

    /* Calculate amount of memory required to store CA certificate table. */
    realSz = cm_get_cert_cache_mem_size(cm);
    if (realSz > sz) {
        WOLFSSL_MSG("Mem output buffer too small");
        ret = BUFFER_E;
    }
    if (ret == WOLFSSL_SUCCESS) {
        byte*           current;
        CertCacheHeader hdr;

        /* Create header for storage. */
        hdr.version  = WOLFSSL_CACHE_CERT_VERSION;
        hdr.rows     = CA_TABLE_SIZE;
        cm_set_cert_header_Columns(cm, hdr.columns);
        hdr.signerSz = (int)sizeof(Signer);

        /* Copy header into memory. */
        XMEMCPY(mem, &hdr, sizeof(CertCacheHeader));
        current = (byte*)mem + sizeof(CertCacheHeader);

        /* Each row of table. */
        for (i = 0; i < CA_TABLE_SIZE; ++i) {
            /* Append row to memory. */
            current += cm_store_cert_row(cm, current, i);
        }
    }

    return ret;
}


#if !defined(NO_FILESYSTEM)

/* Persist CA certificate cache to file.
 *
 * Locks CA table.
 *
 * @param [in] cm     Certificate manager.
 * @param [in] fname  File name to write to.
 * @return  WOLFSSL_SUCCESS on success.
 * @return  WOLFSSL_BAD_FILE when opening file fails.
 * @return  BAD_MUTEX_E when locking fails.
 * @return  MEMORY_E when dynamic memory allocation fails.
 * @return  FWRITE_ERROR when writing to file fails.
 */
int CM_SaveCertCache(WOLFSSL_CERT_MANAGER* cm, const char* fname)
{
    XFILE file;
    int   ret = WOLFSSL_SUCCESS;

    WOLFSSL_ENTER("CM_SaveCertCache");

    /* Open file for writing. */
    file = XFOPEN(fname, "w+b");
    if (file == XBADFILE) {
       WOLFSSL_MSG("Couldn't open cert cache save file");
       ret = WOLFSSL_BAD_FILE;
    }

    /* Lock CA table. */
    if ((ret == WOLFSSL_SUCCESS) && (wc_LockMutex(&cm->caLock) != 0)) {
        WOLFSSL_MSG("wc_LockMutex on caLock failed");
        ret = BAD_MUTEX_E;
    }

    if (ret == WOLFSSL_SUCCESS) {
        byte* mem;
        /* Calculate size of memory required to store CA table. */
        size_t memSz = (size_t)cm_get_cert_cache_mem_size(cm);
        /* Allocate memory to hold CA table. */
        mem = (byte*)XMALLOC(memSz, cm->heap, DYNAMIC_TYPE_TMP_BUFFER);
        if (mem == NULL) {
            WOLFSSL_MSG("Alloc for tmp buffer failed");
            ret = MEMORY_E;
        }
        if (ret == WOLFSSL_SUCCESS) {
            /* Store CA table in memory. */
            ret = cm_do_mem_save_cert_cache(cm, mem, (int)memSz);
        }
        if (ret == WOLFSSL_SUCCESS) {
            /* Write memory to file. */
            int sz = (int)XFWRITE(mem, memSz, 1, file);
            if (sz != 1) {
                WOLFSSL_MSG("Cert cache file write failed");
                ret = FWRITE_ERROR;
            }
            XFREE(mem, cm->heap, DYNAMIC_TYPE_TMP_BUFFER);
        }

        /* Unlock CA table. */
        wc_UnLockMutex(&cm->caLock);
    }

    /* Close file. */
    if (file != XBADFILE) {
        XFCLOSE(file);
    }
    return ret;
}


/* Restore CA certificate cache from file.
 *
 * @param [in] cm     Certificate manager.
 * @param [in] fname  File name to write to.
 * @return  WOLFSSL_SUCCESS on success.
 * @return  WOLFSSL_BAD_FILE when opening or using file fails.
 * @return  MEMORY_E when dynamic memory allocation fails.
 * @return  FREAD_ERROR when reading from file fails.
 */
int CM_RestoreCertCache(WOLFSSL_CERT_MANAGER* cm, const char* fname)
{
    XFILE file;
    int   ret = WOLFSSL_SUCCESS;
    int   memSz = 0;
    byte* mem = NULL;

    WOLFSSL_ENTER("CM_RestoreCertCache");

    /* Open file for reading. */
    file = XFOPEN(fname, "rb");
    if (file == XBADFILE) {
       WOLFSSL_MSG("Couldn't open cert cache save file");
       ret = WOLFSSL_BAD_FILE;
    }

    if (ret == WOLFSSL_SUCCESS) {
        /* Read file into allocated memory. */
        ret = wolfssl_read_file(file, (char**)&mem, &memSz);
        if (ret == 0) {
            ret = WOLFSSL_SUCCESS;
        }
    }
    if (ret == WOLFSSL_SUCCESS) {
        /* Create the CA certificate table from memory. */
        ret = CM_MemRestoreCertCache(cm, mem, memSz);
        if (ret != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("Mem restore cert cache failed");
        }
    }

    /* Dispose of dynamic memory read into. */
    XFREE(mem, cm->heap, DYNAMIC_TYPE_TMP_BUFFER);
    /* Close file. */
    if (file != XBADFILE) {
        XFCLOSE(file);
    }
    return ret;
}

#endif /* NO_FILESYSTEM */


/* Persist CA certificate cache to memory.
 *
 * Locks CA table.
 *
 * @param [in]  cm    Certificate manager.
 * @param [in]  mem   Memory to persist into.
 * @param [in]  sz    Size in bytes of memory.
 * @param [out] used  Number of bytes used when persisting cache.
 * @return  WOLFSSL_SUCCESS on success.
 * @return  BAD_MUTEX_E when locking fails.
 * @return  BUFFER_E when memory is too small.
 */
int CM_MemSaveCertCache(WOLFSSL_CERT_MANAGER* cm, void* mem, int sz, int* used)
{
    int ret = WOLFSSL_SUCCESS;

    WOLFSSL_ENTER("CM_MemSaveCertCache");

    /* Lock CA table. */
    if (wc_LockMutex(&cm->caLock) != 0) {
        WOLFSSL_MSG("wc_LockMutex on caLock failed");
        ret = BAD_MUTEX_E;
    }
    if (ret == WOLFSSL_SUCCESS) {
        /* Save CA table into memory. */
        ret = cm_do_mem_save_cert_cache(cm, mem, sz);
        if (ret == WOLFSSL_SUCCESS) {
            /* Get the number of bytes used. */
            *used  = cm_get_cert_cache_mem_size(cm);
        }

        /* Unlock CA table. */
        wc_UnLockMutex(&cm->caLock);
    }

    return ret;
}


/* Restore CA certificate table from memory,
 *
 * Locks CA table.
 *
 * @param [in] cm   Certificate manager.
 * @param [in] mem  Buffer containing rows.
 * @param [in] sz   Size in bytes of data in buffer.
 * @return  WOLFSSL_SUCCESS on success.
 * @return  BUFFER_E when buffer is too small.
 * @return  BAD_MUTEX_E when locking fails.
 * @return  MEMORY_E when dynamic memory allocation fails.
 */
int CM_MemRestoreCertCache(WOLFSSL_CERT_MANAGER* cm, const void* mem, int sz)
{
    int ret = WOLFSSL_SUCCESS;
    int i;
    CertCacheHeader* hdr = (CertCacheHeader*)mem;
    byte*            current = (byte*)mem + sizeof(CertCacheHeader);
    byte*            end     = (byte*)mem + sz;  /* don't go over */

    WOLFSSL_ENTER("CM_MemRestoreCertCache");

    /* Check memory available is bigger than cache header. */
    if (current > end) {
        WOLFSSL_MSG("Cert Cache Memory buffer too small");
        ret = BUFFER_E;
    }

    /* Validate the cache header. */
    if ((ret == WOLFSSL_SUCCESS) &&
            ((hdr->version  != WOLFSSL_CACHE_CERT_VERSION) ||
             (hdr->rows     != CA_TABLE_SIZE) ||
             (hdr->signerSz != (int)sizeof(Signer)))) {
        WOLFSSL_MSG("Cert Cache Memory header mismatch");
        ret = CACHE_MATCH_ERROR;
    }

    /* Lock CA table. */
    if ((ret == WOLFSSL_SUCCESS) && (wc_LockMutex(&cm->caLock) != 0)) {
        WOLFSSL_MSG("wc_LockMutex on caLock failed");
        ret = BAD_MUTEX_E;
    }

    if (ret == WOLFSSL_SUCCESS) {
        /* Dispose of current CA certificate table. */
        FreeSignerTable(cm->caTable, CA_TABLE_SIZE, cm->heap);

        /* Each row. */
        for (i = 0; i < CA_TABLE_SIZE; ++i) {
            /* Restore a row from memory. */
            int added = cm_restore_cert_row(cm, current, i, hdr->columns[i],
                end);
            /* Bail on error. */
            if (added < 0) {
                WOLFSSL_MSG("cm_restore_cert_row error");
                ret = added;
                break;
            }
            /* Update pointer to data of next row. */
            current += added;
        }

        /* Unlock CA table. */
        wc_UnLockMutex(&cm->caLock);
    }

    return ret;
}


/* Calculate size of CA certificate cache when persisted to memory.
 *
 * Locks CA table.
 *
 * @param [in] cm  Certificate manager.
 * @return  Number of bytes on success.
 * @return  BAD_MUTEX_E when locking fails.
 */
int CM_GetCertCacheMemSize(WOLFSSL_CERT_MANAGER* cm)
{
    int ret;

    WOLFSSL_ENTER("CM_GetCertCacheMemSize");

    /* Lock CA table. */
    if (wc_LockMutex(&cm->caLock) != 0) {
        WOLFSSL_MSG("wc_LockMutex on caLock failed");
        ret = BAD_MUTEX_E;
    }
    else {
        /* Calculate memory size. */
        ret = cm_get_cert_cache_mem_size(cm);

        /* Unlock CA table. */
        wc_UnLockMutex(&cm->caLock);
    }

    return ret;
}

#endif /* PERSIST_CERT_CACHE */

/*******************************************************************************
 * CRL handling
 ******************************************************************************/

/* Enables/disables the use of CRLs when validating certificates.
 *
 * @param [in] cm       Certificate manager.
 * @param [in] options  Options for using CRLs. Valid flags:
 *                        WOLFSSL_CRL_CHECKALL, WOLFSSL_CRL_CHECK.
 * @return  WOLFSSL_SUCCESS on success.
 * @return  WOLFSSL_FAILURE when initializing the CRL object fails.
 * @return  BAD_FUNC_ARG when cm is NULL.
 * @return  MEMORY_E when dynamic memory allocation fails.
 * @return  NOT_COMPILED_IN when the CRL feature is disabled.
 */
int wolfSSL_CertManagerEnableCRL(WOLFSSL_CERT_MANAGER* cm, int options)
{
    int ret = WOLFSSL_SUCCESS;

    WOLFSSL_ENTER("wolfSSL_CertManagerEnableCRL");

    (void)options;

    /* Validate parameters. */
    if (cm == NULL) {
        ret = BAD_FUNC_ARG;
    }

#if defined(OPENSSL_COMPATIBLE_DEFAULTS)
    /* If disabling then don't worry about whether CRL feature is enabled. */
    if ((ret == WOLFSSL_SUCCESS) && (options == 0)) {
        /* Disable leaf CRL check. */
        cm->crlEnabled = 0;
        /* Disable all CRL checks. */
        cm->crlCheckAll = 0;
    }
    else
#endif
    if (ret == WOLFSSL_SUCCESS) {
#ifndef HAVE_CRL
        /* CRL feature not enabled. */
        ret = NOT_COMPILED_IN;
#else
        /* Create CRL object if not present. */
        if (cm->crl == NULL) {
            /* Allocate memory for CRL object. */
            cm->crl = (WOLFSSL_CRL*)XMALLOC(sizeof(WOLFSSL_CRL), cm->heap,
                                            DYNAMIC_TYPE_CRL);
            if (cm->crl == NULL) {
                ret = MEMORY_E;
            }
            if (ret == WOLFSSL_SUCCESS) {
                /* Reset fields of CRL object. */
                XMEMSET(cm->crl, 0, sizeof(WOLFSSL_CRL));
                /* Initialize CRL object. */
                if (InitCRL(cm->crl, cm) != 0) {
                    WOLFSSL_MSG("Init CRL failed");
                    /* Dispose of CRL object - indicating dynamically allocated.
                     */
                    FreeCRL(cm->crl, 1);
                    cm->crl = NULL;
                    ret = WOLFSSL_FAILURE;
                }
            }
        }

        if (ret == WOLFSSL_SUCCESS) {
        #if defined(HAVE_CRL_IO) && defined(USE_WOLFSSL_IO)
            /* Use built-in callback to lookup CRL from URL. */
            cm->crl->crlIOCb = EmbedCrlLookup;
        #endif
        #if defined(OPENSSL_COMPATIBLE_DEFAULTS)
            if ((options & WOLFSSL_CRL_CHECKALL) ||
                (options & WOLFSSL_CRL_CHECK))
        #endif
            {
                /* Enable leaf CRL check. */
                cm->crlEnabled = 1;
                if (options & WOLFSSL_CRL_CHECKALL) {
                    /* Enable all CRL check. */
                    cm->crlCheckAll = 1;
                }
            }
        }
#endif
    }

    return ret;
}


/* Disables the CRL checks.
 *
 * @param [in] cm  Certificate manager.
 * @return  WOLFSSL_SUCCESS on success.
 * @return  BAD_FUNC_ARG when cm is NULL.
 */
int wolfSSL_CertManagerDisableCRL(WOLFSSL_CERT_MANAGER* cm)
{
    int ret = WOLFSSL_SUCCESS;

    WOLFSSL_ENTER("wolfSSL_CertManagerDisableCRL");

    /* Validate parameter. */
    if (cm == NULL) {
        ret = BAD_FUNC_ARG;
    }
    if (ret == WOLFSSL_SUCCESS) {
        /* Disable CRL checking. */
        cm->crlEnabled = 0;
    }

    return ret;
}

#ifdef HAVE_CRL

/* Load CRL for use.
 *
 * @param [in] cm    Certificate manager.
 * @param [in] buff  Buffer holding CRL.
 * @param [in] sz    Size in bytes of CRL in buffer.
 * @param [in] type  Format of encoding. Valid values:
 *                     WOLFSSL_FILETYPE_ASN1, WOLFSSL_FILETYPE_PEM.
 * @return  WOLFSSL_SUCCESS on success.
 * @return  BAD_FUNC_ARG when cm or buff is NULL or sz is negative or zero.
 * @return  WOLFSSL_FATAL_ERROR when creating CRL object fails.
 */
int wolfSSL_CertManagerLoadCRLBuffer(WOLFSSL_CERT_MANAGER* cm,
    const unsigned char* buff, long sz, int type)
{
    int ret = WOLFSSL_SUCCESS;

    WOLFSSL_ENTER("wolfSSL_CertManagerLoadCRLBuffer");

    /* Validate parameters. */
    if ((cm == NULL) || (buff == NULL) || (sz <= 0)) {
        ret = BAD_FUNC_ARG;
    }

    /* Create a CRL object if not available and enable CRL checking. */
    if ((ret == WOLFSSL_SUCCESS) && (cm->crl == NULL) &&
            (wolfSSL_CertManagerEnableCRL(cm, WOLFSSL_CRL_CHECK) !=
             WOLFSSL_SUCCESS)) {
        WOLFSSL_MSG("Enable CRL failed");
        ret = WOLFSSL_FATAL_ERROR;
    }

    if (ret == WOLFSSL_SUCCESS) {
        /* Load CRL into CRL object of the certificate manager. */
        ret = BufferLoadCRL(cm->crl, buff, sz, type, VERIFY);
    }

    return ret;
}

/* Free the CRL object of the certificate manager.
 *
 * @param [in] cm  Certificate manager.
 * @return  WOLFSSL_SUCCESS on success.
 * @return  BAD_FUNC_ARG when cm is NULL.
 */
int wolfSSL_CertManagerFreeCRL(WOLFSSL_CERT_MANAGER* cm)
{
    int ret = WOLFSSL_SUCCESS;

    WOLFSSL_ENTER("wolfSSL_CertManagerFreeCRL");

    /* Validate parameter. */
    if (cm == NULL) {
        ret = BAD_FUNC_ARG;
    }
    /* Check whether CRL object exists. */
    if ((ret == WOLFSSL_SUCCESS) && (cm->crl != NULL)) {
        /* Dispose of CRL object - indicating dynamically allocated. */
        FreeCRL(cm->crl, 1);
        cm->crl = NULL;
    }

    return ret;
}

/* Check DER encoded certificate against CRLs if checking enabled.
 *
 * @param [in] cm   Certificate manager.
 * @param [in] der  DER encode certificate.
 * @param [in] sz   Size in bytes of DER encode certificate.
 * @return  WOLFSSL_SUCCESS on success.
 * @return  BAD_FUNC_ARG when cm or der is NULL or sz is negative or zero.
 * @return  MEMORY_E when dynamic memory allocation fails.
 */
int wolfSSL_CertManagerCheckCRL(WOLFSSL_CERT_MANAGER* cm,
    const unsigned char* der, int sz)
{
    int ret = 0;
#ifdef WOLFSSL_SMALL_STACK
    DecodedCert* cert = NULL;
#else
    DecodedCert  cert[1];
#endif

    WOLFSSL_ENTER("wolfSSL_CertManagerCheckCRL");

    /* Validate parameters. */
    if ((cm == NULL) || (der == NULL) || (sz <= 0)) {
        ret = BAD_FUNC_ARG;
    }

    /* Check if CRL checking enabled. */
    if ((ret == 0) && cm->crlEnabled) {
    #ifdef WOLFSSL_SMALL_STACK
        /* Allocate memory for decoded certificate. */
        cert = (DecodedCert*)XMALLOC(sizeof(DecodedCert), NULL,
            DYNAMIC_TYPE_DCERT);
        if (cert == NULL)
            ret = MEMORY_E;
        if (ret == 0)
    #endif
        {
            /* Initialize decoded certificate with buffer. */
            InitDecodedCert(cert, der, (word32)sz, NULL);

            /* Parse certificate and perform CRL checks. */
            ret = ParseCertRelative(cert, CERT_TYPE, VERIFY_CRL, cm);
            if (ret != 0) {
                WOLFSSL_MSG("ParseCert failed");
            }
            /* Do CRL checks with decoded certificate. */
            else if ((ret = CheckCertCRL(cm->crl, cert)) != 0) {
                WOLFSSL_MSG("CheckCertCRL failed");
            }

            /* Dispose of dynamically allocated memory. */
            FreeDecodedCert(cert);
        #ifdef WOLFSSL_SMALL_STACK
            XFREE(cert, NULL, DYNAMIC_TYPE_DCERT);
        #endif
        }
    }

    return (ret == 0) ? WOLFSSL_SUCCESS : ret;
}

/* Set the missing CRL callback.
 *
 * @param [in] cm  Certificate manager.
 * @param [in] cb  Missing CRL callback.
 * @return  WOLFSSL_SUCCESS on success.
 * @return  BAD_FUNC_ARG when cm is NULL.
 */
int wolfSSL_CertManagerSetCRL_Cb(WOLFSSL_CERT_MANAGER* cm, CbMissingCRL cb)
{
    int ret = WOLFSSL_SUCCESS;

    WOLFSSL_ENTER("wolfSSL_CertManagerSetCRL_Cb");

    /* Validate parameters. */
    if (cm == NULL) {
        ret = BAD_FUNC_ARG;
    }
    if (ret == WOLFSSL_SUCCESS) {
        /* Store callback. */
        cm->cbMissingCRL = cb;
    }

    return ret;
}

#ifdef HAVE_CRL_IO
/* Set the CRL I/O callback.
 *
 * @param [in] cm  Certificate manager.
 * @param [in] cb  CRL I/O callback.
 * @return  WOLFSSL_SUCCESS on success.
 * @return  BAD_FUNC_ARG when cm is NULL.
 */
int wolfSSL_CertManagerSetCRL_IOCb(WOLFSSL_CERT_MANAGER* cm, CbCrlIO cb)
{
    int ret = WOLFSSL_SUCCESS;

    /* Validate parameters. */
    if (cm == NULL) {
        ret = BAD_FUNC_ARG;
    }
    if ((ret == WOLFSSL_SUCCESS) && (cm->crl != NULL)) {
        /* Store callback. */
        cm->crl->crlIOCb = cb;
    }

    return ret;
}
#endif

#ifndef NO_FILESYSTEM
/* Load CRL/s from path with the option of monitoring for changes.
 *
 * @param [in] cm       Certificate manager.
 * @param [in] path     Path to a directory containing CRLs.
 * @param [in] type     Format of encoding. Valid values:
 *                        WOLFSSL_FILETYPE_ASN1, WOLFSSL_FILETYPE_PEM.
 * @param [in] monitor  Whether to monitor path for changes to files.
 * @return  WOLFSSL_SUCCESS on success.
 * @return  BAD_FUNC_ARG when cm or path is NULL.
 * @return  WOLFSSL_FATAL_ERROR when enabling CRLs fails.
 */
int wolfSSL_CertManagerLoadCRL(WOLFSSL_CERT_MANAGER* cm, const char* path,
    int type, int monitor)
{
    int ret = WOLFSSL_SUCCESS;

    WOLFSSL_ENTER("wolfSSL_CertManagerLoadCRL");

    /* Validate parameters. */
    if ((cm == NULL) || (path == NULL)) {
        ret = BAD_FUNC_ARG;
    }

    /* Create a CRL object if not available. */
    if ((ret == WOLFSSL_SUCCESS) && (cm->crl == NULL) &&
            (wolfSSL_CertManagerEnableCRL(cm, WOLFSSL_CRL_CHECK) !=
             WOLFSSL_SUCCESS)) {
        WOLFSSL_MSG("Enable CRL failed");
        ret = WOLFSSL_FATAL_ERROR;
    }

    if (ret == WOLFSSL_SUCCESS) {
        /* Load CRLs from path into CRL object of certificate manager. */
        ret = LoadCRL(cm->crl, path, type, monitor);
    }

    return ret;
}

/* Load CRL from file.
 *
 * @param [in] cm    Certificate manager.
 * @param [in] file  Path to a directory containing CRLs.
 * @param [in] type  Format of encoding. Valid values:
 *                       WOLFSSL_FILETYPE_ASN1, WOLFSSL_FILETYPE_PEM.
 * @return  WOLFSSL_SUCCESS on success.
 * @return  BAD_FUNC_ARG when cm or file is NULL.
 * @return  WOLFSSL_FATAL_ERROR when enabling CRLs fails.
 */
int wolfSSL_CertManagerLoadCRLFile(WOLFSSL_CERT_MANAGER* cm, const char* file,
    int type)
{
    int ret = WOLFSSL_SUCCESS;

    WOLFSSL_ENTER("wolfSSL_CertManagerLoadCRLFile");

    /* Validate parameters. */
    if ((cm == NULL) || (file == NULL)) {
        ret = BAD_FUNC_ARG;
    }

    /* Create a CRL object if not available. */
    if ((ret == WOLFSSL_SUCCESS) && (cm->crl == NULL) &&
            (wolfSSL_CertManagerEnableCRL(cm, WOLFSSL_CRL_CHECK) !=
             WOLFSSL_SUCCESS)) {
        WOLFSSL_MSG("Enable CRL failed");
        ret = WOLFSSL_FATAL_ERROR;
    }

    if (ret == WOLFSSL_SUCCESS) {
        /* Load CRL file into CRL object of certificate manager. */
        ret = ProcessFile(NULL, file, type, CRL_TYPE, NULL, 0, cm->crl, VERIFY);
    }

    return ret;
}
#endif /* !NO_FILESYSTEM */

#endif /* HAVE_CRL */

/*******************************************************************************
 * OCSP handling
 ******************************************************************************/

/* Enables OCSP when validating certificates and sets options.
 *
 * @param [in] cm       Certificate manager.
 * @param [in] options  Options for using OCSP. Valid flags:
 *                        WOLFSSL_OCSP_URL_OVERRIDE, WOLFSSL_OCSP_NO_NONCE,
 *                        WOLFSSL_OCSP_CHECKALL.
 * @return  WOLFSSL_SUCCESS on success.
 * @return  0 when initializing the OCSP object fails.
 * @return  BAD_FUNC_ARG when cm is NULL.
 * @return  MEMORY_E when dynamic memory allocation fails.
 * @return  NOT_COMPILED_IN when the OCSP feature is disabled.
 */
int wolfSSL_CertManagerEnableOCSP(WOLFSSL_CERT_MANAGER* cm, int options)
{
    int ret = WOLFSSL_SUCCESS;

    (void)options;

    WOLFSSL_ENTER("wolfSSL_CertManagerEnableOCSP");

    /* Validate parameters. */
    if (cm == NULL) {
        ret = BAD_FUNC_ARG;
    }

#ifndef HAVE_OCSP
    if (ret == WOLFSSL_SUCCESS) {
        /* OCSP feature not enabled. */
        ret = NOT_COMPILED_IN;
    }
#else
    if (ret == WOLFSSL_SUCCESS) {
        /* Check whether OCSP object is available. */
        if (cm->ocsp == NULL) {
            /* Allocate memory for OCSP object. */
            cm->ocsp = (WOLFSSL_OCSP*)XMALLOC(sizeof(WOLFSSL_OCSP), cm->heap,
                DYNAMIC_TYPE_OCSP);
            if (cm->ocsp == NULL) {
                ret = MEMORY_E;
            }
            if (ret == WOLFSSL_SUCCESS) {
                /* Reset the fields of the OCSP object. */
                XMEMSET(cm->ocsp, 0, sizeof(WOLFSSL_OCSP));
                /* Initialize the OCSP object. */
                if (InitOCSP(cm->ocsp, cm) != 0) {
                    WOLFSSL_MSG("Init OCSP failed");
                    /* Dispose of OCSP object - indicating dynamically allocated.
                     */
                    FreeOCSP(cm->ocsp, 1);
                    cm->ocsp = NULL;
                    ret = 0;
                }
            }
        }
    }
    if (ret == WOLFSSL_SUCCESS) {
        /* Enable OCSP checking. */
        cm->ocspEnabled = 1;
        /* Enable URL override if requested. */
        if (options & WOLFSSL_OCSP_URL_OVERRIDE) {
            cm->ocspUseOverrideURL = 1;
        }
        /* Set nonce option for creating OCSP requests. */
        cm->ocspSendNonce = (options & WOLFSSL_OCSP_NO_NONCE) !=
            WOLFSSL_OCSP_NO_NONCE;
        /* Set all OCSP checks on if requested. */
        if (options & WOLFSSL_OCSP_CHECKALL) {
            cm->ocspCheckAll = 1;
        }
    #ifndef WOLFSSL_USER_IO
        /* Set built-in OCSP lookup. */
        cm->ocspIOCb = EmbedOcspLookup;
        cm->ocspRespFreeCb = EmbedOcspRespFree;
        cm->ocspIOCtx = cm->heap;
    #endif /* WOLFSSL_USER_IO */
    }
#endif /* HAVE_OCSP */

    return ret;
}

/* Disables the OCSP checks.
 *
 * @param [in] cm  Certificate manager.
 * @return  WOLFSSL_SUCCESS on success.
 * @return  BAD_FUNC_ARG when cm is NULL.
 */
int wolfSSL_CertManagerDisableOCSP(WOLFSSL_CERT_MANAGER* cm)
{
    int ret = WOLFSSL_SUCCESS;

    WOLFSSL_ENTER("wolfSSL_CertManagerDisableOCSP");

    /* Validate parameter. */
    if (cm == NULL) {
        ret = BAD_FUNC_ARG;
    }
    if (ret == WOLFSSL_SUCCESS) {
        /* Disable use of OCSP with certificate validation. */
        cm->ocspEnabled = 0;
    }

    return ret;
}

/* Enables OCSP stapling with certificates in manager.
 *
 * @param [in] cm       Certificate manager.
 * @param [in] options  Options for using OCSP. Valid flags:
 *                        WOLFSSL_OCSP_URL_OVERRIDE, WOLFSSL_OCSP_NO_NONCE,
 *                        WOLFSSL_OCSP_CHECKALL.
 * @return  WOLFSSL_SUCCESS on success.
 * @return  0 when initializing the OCSP stapling object fails.
 * @return  BAD_FUNC_ARG when cm is NULL.
 * @return  MEMORY_E when dynamic memory allocation fails.
 * @return  NOT_COMPILED_IN when the OCSP stapling feature is disabled.
 */
int wolfSSL_CertManagerEnableOCSPStapling(WOLFSSL_CERT_MANAGER* cm)
{
    int ret = WOLFSSL_SUCCESS;

    WOLFSSL_ENTER("wolfSSL_CertManagerEnableOCSPStapling");

    /* Validate parameters. */
    if (cm == NULL) {
        ret = BAD_FUNC_ARG;
    }

#if !defined(HAVE_CERTIFICATE_STATUS_REQUEST) && \
    !defined(HAVE_CERTIFICATE_STATUS_REQUEST_V2)
    if (ret == WOLFSSL_SUCCESS) {
        /* OCSP stapling feature not enabled. */
        ret = NOT_COMPILED_IN;
    }
#else
#ifndef NO_WOLFSSL_SERVER
    if (ret == WOLFSSL_SUCCESS) {
        /* Check whether OCSP object is available. */
        if (cm->ocsp_stapling == NULL) {
            /* Allocate memory for OCSP stapling object. */
            cm->ocsp_stapling = (WOLFSSL_OCSP*)XMALLOC(sizeof(WOLFSSL_OCSP),
                cm->heap, DYNAMIC_TYPE_OCSP);
            if (cm->ocsp_stapling == NULL) {
                ret = MEMORY_E;
            }
            if (ret == WOLFSSL_SUCCESS) {
                /* Reset the fields of the OCSP object. */
                XMEMSET(cm->ocsp_stapling, 0, sizeof(WOLFSSL_OCSP));
                /* Initialize the OCSP stapling object. */
                if (InitOCSP(cm->ocsp_stapling, cm) != 0) {
                    WOLFSSL_MSG("Init OCSP failed");
                    /* Dispose of OCSP stapling object - indicating dynamically
                     * allocated. */
                    FreeOCSP(cm->ocsp_stapling, 1);
                    cm->ocsp_stapling = NULL;
                    ret = 0;
                }
            }
        }
    }
#ifndef WOLFSSL_USER_IO
    if (ret == WOLFSSL_SUCCESS) {
        /* Set built-in OCSP lookup. */
        cm->ocspIOCb = EmbedOcspLookup;
        cm->ocspRespFreeCb = EmbedOcspRespFree;
        cm->ocspIOCtx = cm->heap;
    }
#endif /* WOLFSSL_USER_IO */
#endif /* NO_WOLFSSL_SERVER */
    if (ret == WOLFSSL_SUCCESS) {
        /* Enable OCSP stapling. */
        cm->ocspStaplingEnabled = 1;
    }
#endif /* HAVE_CERTIFICATE_STATUS_REQUEST ||
        * HAVE_CERTIFICATE_STATUS_REQUEST_V2 */

    return ret;
}

/* Disables OCSP Stapling.
 *
 * @param [in] cm  Certificate manager.
 * @return  WOLFSSL_SUCCESS on success.
 * @return  BAD_FUNC_ARG when cm is NULL.
 */
int wolfSSL_CertManagerDisableOCSPStapling(WOLFSSL_CERT_MANAGER* cm)
{
    int ret = WOLFSSL_SUCCESS;

    WOLFSSL_ENTER("wolfSSL_CertManagerDisableOCSPStapling");

    /* Validate parameter. */
    if (cm == NULL) {
        ret = BAD_FUNC_ARG;
    }
    if (ret == WOLFSSL_SUCCESS) {
    #if defined(HAVE_CERTIFICATE_STATUS_REQUEST) || \
        defined(HAVE_CERTIFICATE_STATUS_REQUEST_V2)
        /* Disable use of OCSP Stapling. */
        cm->ocspStaplingEnabled = 0;
    #else
        /* OCSP stapling feature not enabled. */
        ret = NOT_COMPILED_IN;
    #endif
    }

    return ret;
}

/* Enable the must use OCSP Stapling option.
 *
 * @param [in] cm  Certificate manager.
 * @return  WOLFSSL_SUCCESS on success.
 * @return  BAD_FUNC_ARG when cm is NULL.
 */
int wolfSSL_CertManagerEnableOCSPMustStaple(WOLFSSL_CERT_MANAGER* cm)
{
    int ret = WOLFSSL_SUCCESS;

    WOLFSSL_ENTER("wolfSSL_CertManagerEnableOCSPMustStaple");

    /* Validate parameter. */
    if (cm == NULL) {
        ret = BAD_FUNC_ARG;
    }
    if (ret == WOLFSSL_SUCCESS) {
#if defined(HAVE_CERTIFICATE_STATUS_REQUEST) || \
    defined(HAVE_CERTIFICATE_STATUS_REQUEST_V2)
    #ifndef NO_WOLFSSL_CLIENT
        /* Enable must use OCSP Stapling option. */
        cm->ocspMustStaple = 1;
    #endif
#else
        /* OCSP stapling feature not enabled. */
        ret = NOT_COMPILED_IN;
#endif
    }

    return ret;
}

/* Disable the must use OCSP Stapling option.
 *
 * @param [in] cm  Certificate manager.
 * @return  WOLFSSL_SUCCESS on success.
 * @return  BAD_FUNC_ARG when cm is NULL.
 */
int wolfSSL_CertManagerDisableOCSPMustStaple(WOLFSSL_CERT_MANAGER* cm)
{
    int ret = WOLFSSL_SUCCESS;

    WOLFSSL_ENTER("wolfSSL_CertManagerDisableOCSPMustStaple");

    /* Validate parameter. */
    if (cm == NULL) {
        ret = BAD_FUNC_ARG;
    }

    if (ret == WOLFSSL_SUCCESS) {
#if defined(HAVE_CERTIFICATE_STATUS_REQUEST) || \
    defined(HAVE_CERTIFICATE_STATUS_REQUEST_V2)
    #ifndef NO_WOLFSSL_CLIENT
        /* Disable must use OCSP Stapling option. */
        cm->ocspMustStaple = 0;
    #endif
#else
        /* OCSP stapling feature not enabled. */
        ret = NOT_COMPILED_IN;
#endif
    }

    return ret;
}

#ifdef HAVE_OCSP
/* Check DER encoded certificate against with OCSP if checking enabled.
 *
 * @param [in] cm   Certificate manager.
 * @param [in] der  DER encode certificate.
 * @param [in] sz   Size in bytes of DER encode certificate.
 * @return  WOLFSSL_SUCCESS on success.
 * @return  BAD_FUNC_ARG when cm or der is NULL or sz is negative or 0.
 * @return  MEMORY_E when dynamic memory allocation fails.
 */
int wolfSSL_CertManagerCheckOCSP(WOLFSSL_CERT_MANAGER* cm,
    const unsigned char* der, int sz)
{
    int ret = 0;
#ifdef WOLFSSL_SMALL_STACK
    DecodedCert* cert = NULL;
#else
    DecodedCert  cert[1];
#endif

    WOLFSSL_ENTER("wolfSSL_CertManagerCheckOCSP");

    /* Validate parameters. */
    if ((cm == NULL) || (der == NULL) || (sz <= 0)) {
        ret = BAD_FUNC_ARG;
    }

    /* Check if OCSP checking enabled. */
    if ((ret == 0) && cm->ocspEnabled) {
    #ifdef WOLFSSL_SMALL_STACK
        /* Allocate memory for decoded certificate. */
        cert = (DecodedCert*)XMALLOC(sizeof(DecodedCert), cm->heap,
            DYNAMIC_TYPE_DCERT);
        if (cert == NULL) {
            ret = MEMORY_E;
        }
        if (ret == 0)
    #endif
        {
            /* Initialize decoded certificate with buffer. */
            InitDecodedCert(cert, der, (word32)sz, NULL);

            /* Parse certificate and perform CRL checks. */
            ret = ParseCertRelative(cert, CERT_TYPE, VERIFY_OCSP, cm);
            if (ret != 0) {
                WOLFSSL_MSG("ParseCert failed");
            }
            /* Do OCSP checks with decoded certificate. */
            else if ((ret = CheckCertOCSP(cm->ocsp, cert)) != 0) {
                WOLFSSL_MSG("CheckCertOCSP failed");
            }

            /* Dispose of dynamically allocated memory. */
            FreeDecodedCert(cert);
        #ifdef WOLFSSL_SMALL_STACK
            XFREE(cert, cm->heap, DYNAMIC_TYPE_DCERT);
        #endif
        }
    }

    return (ret == 0) ? WOLFSSL_SUCCESS : ret;
}

/* Check OCSP response.
 *
 * @param [in] cm              Certificate manager.
 * @param [in] response        Buffer holding OCSP response.
 * @param [in] responseSz      Size in bytes of OCSP response.
 * @param [in] responseBuffer  Buffer to copy response into.
 * @param [in] status          Place to store certificate status.
 * @param [in] entry           Place to store OCSP entry.
 * @param [in] ocspRequest     OCSP request to match with response.
 * @return  WOLFSSL_SUCCESS on success.
 * @return  BAD_FUNC_ARG when cm or response is NULL.
 */
int wolfSSL_CertManagerCheckOCSPResponse(WOLFSSL_CERT_MANAGER *cm,
    byte *response, int responseSz, buffer *responseBuffer,
    CertStatus *status, OcspEntry *entry, OcspRequest *ocspRequest)
{
    int ret = 0;

    WOLFSSL_ENTER("wolfSSL_CertManagerCheckOCSPResponse");

    /* Validate parameters. */
    if ((cm == NULL) || (response == NULL)) {
        ret = BAD_FUNC_ARG;
    }
    if ((ret == 0) && cm->ocspEnabled) {
        /* Check OCSP response with OCSP object from certificate manager. */
        ret = CheckOcspResponse(cm->ocsp, response, responseSz, responseBuffer,
            status, entry, ocspRequest, NULL);
    }

    return (ret == 0) ? WOLFSSL_SUCCESS : ret;
}

/* Set the OCSP override URL.
 *
 * @param [in] cm   Certificate manager.
 * @param [in] url  URL to get an OCSP response from.
 * @return  WOLFSSL_SUCCESS on success.
 * @return  BAD_FUNC_ARG when cm is NULL.
 * @return  MEMORY_E when dynamic memory allocation fails.
 */
int wolfSSL_CertManagerSetOCSPOverrideURL(WOLFSSL_CERT_MANAGER* cm,
    const char* url)
{
    int ret = WOLFSSL_SUCCESS;

    WOLFSSL_ENTER("wolfSSL_CertManagerSetOCSPOverrideURL");

    /* Validate parameters. */
    if (cm == NULL) {
        ret = BAD_FUNC_ARG;
    }

    if (ret == WOLFSSL_SUCCESS) {
        /* Dispose of old URL. */
        XFREE(cm->ocspOverrideURL, cm->heap, DYNAMIC_TYPE_URL);
        if (url != NULL) {
            /* Calculate size of URL string. Include terminator character. */
            int urlSz = (int)XSTRLEN(url) + 1;
            /* Allocate memory for URL to be copied into. */
            cm->ocspOverrideURL = (char*)XMALLOC((size_t)urlSz, cm->heap,
                DYNAMIC_TYPE_URL);
            if (cm->ocspOverrideURL == NULL) {
                ret = MEMORY_E;
            }
            if (ret == WOLFSSL_SUCCESS) {
                /* Copy URL into certificate manager. */
                XMEMCPY(cm->ocspOverrideURL, url, (size_t)urlSz);
            }
        }
        else {
            /* No URL to set so make it NULL. */
            cm->ocspOverrideURL = NULL;
        }
    }

    return ret;
}

/* Set the OCSP I/O callback, OCSP response free callback and related data.
 *
 * @param [in] cm          Certificate manager.
 * @param [in] ioCb        OCSP callback.
 * @param [in] respFreeCb  Callback to free OCSP response buffer.
 * @param [in] ioCbCtx     Context daa to pass to OCSP callbacks.
 * @return  WOLFSSL_SUCCESS on success.
 * @return  BAD_FUNC_ARG when cm is NULL.
 */
int wolfSSL_CertManagerSetOCSP_Cb(WOLFSSL_CERT_MANAGER* cm, CbOCSPIO ioCb,
    CbOCSPRespFree respFreeCb, void* ioCbCtx)
{
    int ret = WOLFSSL_SUCCESS;

    WOLFSSL_ENTER("wolfSSL_CertManagerSetOCSP_Cb");

    /* Validate parameters. */
    if (cm == NULL) {
        ret = BAD_FUNC_ARG;
    }
    if (ret == WOLFSSL_SUCCESS) {
        /* Set callbacks and data into certificate manager. */
        cm->ocspIOCb = ioCb;
        cm->ocspRespFreeCb = respFreeCb;
        cm->ocspIOCtx = ioCbCtx;
    }

    return ret;
}

#endif /* HAVE_OCSP */

#endif /* NO_CERTS */

#endif /* !WOLFSSL_SSL_CERTMAN_INCLUDED */
