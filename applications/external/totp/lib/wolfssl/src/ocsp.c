/* ocsp.c
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


  /* Name change compatibility layer no longer needs to be included here */

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

/*
 * WOLFSSL_NO_OCSP_ISSUER_CHAIN_CHECK:
 *     Disable looking for an authorized responder in the verification path of
 *     the issuer. This will make the authorized responder only look at the
 *     OCSP response signer and direct issuer.
 */

#ifndef WOLFCRYPT_ONLY
#ifdef HAVE_OCSP

#include <wolfssl/error-ssl.h>
#include <wolfssl/ocsp.h>
#include <wolfssl/internal.h>

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif


int InitOCSP(WOLFSSL_OCSP* ocsp, WOLFSSL_CERT_MANAGER* cm)
{
    WOLFSSL_ENTER("InitOCSP");

    ForceZero(ocsp, sizeof(WOLFSSL_OCSP));

    if (wc_InitMutex(&ocsp->ocspLock) != 0)
        return BAD_MUTEX_E;

    ocsp->cm = cm;

    return 0;
}


static int InitOcspEntry(OcspEntry* entry, OcspRequest* request)
{
    WOLFSSL_ENTER("InitOcspEntry");

    ForceZero(entry, sizeof(OcspEntry));

    XMEMCPY(entry->issuerHash,    request->issuerHash,    OCSP_DIGEST_SIZE);
    XMEMCPY(entry->issuerKeyHash, request->issuerKeyHash, OCSP_DIGEST_SIZE);

    return 0;
}


static void FreeOcspEntry(OcspEntry* entry, void* heap)
{
    CertStatus *status, *next;

    if (entry == NULL || !entry->ownStatus)
        return;

    WOLFSSL_ENTER("FreeOcspEntry");

    for (status = entry->status; status; status = next) {
        next = status->next;

        if (status->rawOcspResponse)
            XFREE(status->rawOcspResponse, heap, DYNAMIC_TYPE_OCSP_STATUS);

#ifdef OPENSSL_EXTRA
        if (status->serialInt) {
            if (status->serialInt->isDynamic) {
                XFREE(status->serialInt->data, NULL, DYNAMIC_TYPE_OPENSSL);
            }
            XFREE(status->serialInt, NULL, DYNAMIC_TYPE_OPENSSL);
        }
        status->serialInt = NULL;
#endif

        XFREE(status, heap, DYNAMIC_TYPE_OCSP_STATUS);
    }

    (void)heap;
}


void FreeOCSP(WOLFSSL_OCSP* ocsp, int dynamic)
{
    OcspEntry *entry, *next;

    WOLFSSL_ENTER("FreeOCSP");

    for (entry = ocsp->ocspList; entry; entry = next) {
        next = entry->next;
        FreeOcspEntry(entry, ocsp->cm->heap);
        XFREE(entry, ocsp->cm->heap, DYNAMIC_TYPE_OCSP_ENTRY);
    }

    wc_FreeMutex(&ocsp->ocspLock);

    if (dynamic)
        XFREE(ocsp, ocsp->cm->heap, DYNAMIC_TYPE_OCSP);

}


static int xstat2err(int st)
{
    switch (st) {
        case CERT_GOOD:
            return 0;
        case CERT_REVOKED:
            return OCSP_CERT_REVOKED;
        default:
            return OCSP_CERT_UNKNOWN;
    }
}

int CheckCertOCSP_ex(WOLFSSL_OCSP* ocsp, DecodedCert* cert, WOLFSSL* ssl)
{
    int ret = OCSP_LOOKUP_FAIL;

#ifdef WOLFSSL_SMALL_STACK
    OcspRequest* ocspRequest;
#else
    OcspRequest ocspRequest[1];
#endif

    WOLFSSL_ENTER("CheckCertOCSP");


#ifdef WOLFSSL_SMALL_STACK
    ocspRequest = (OcspRequest*)XMALLOC(sizeof(OcspRequest), NULL,
                                                       DYNAMIC_TYPE_TMP_BUFFER);
    if (ocspRequest == NULL) {
        WOLFSSL_LEAVE("CheckCertOCSP", MEMORY_ERROR);
        return MEMORY_E;
    }
#endif

    if (InitOcspRequest(ocspRequest, cert, ocsp->cm->ocspSendNonce,
                                                         ocsp->cm->heap) == 0) {
        ocspRequest->ssl = ssl;
        ret = CheckOcspRequest(ocsp, ocspRequest, NULL, NULL);

        FreeOcspRequest(ocspRequest);
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(ocspRequest, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    WOLFSSL_LEAVE("CheckCertOCSP", ret);
    return ret;
}
int CheckCertOCSP(WOLFSSL_OCSP* ocsp, DecodedCert* cert)
{
    return CheckCertOCSP_ex(ocsp, cert, NULL);
}

static int GetOcspEntry(WOLFSSL_OCSP* ocsp, OcspRequest* request,
                                                              OcspEntry** entry)
{
    WOLFSSL_ENTER("GetOcspEntry");

    *entry = NULL;

    if (wc_LockMutex(&ocsp->ocspLock) != 0) {
        WOLFSSL_LEAVE("CheckCertOCSP", BAD_MUTEX_E);
        return BAD_MUTEX_E;
    }

    for (*entry = ocsp->ocspList; *entry; *entry = (*entry)->next)
        if (XMEMCMP((*entry)->issuerHash,    request->issuerHash,
                                                         OCSP_DIGEST_SIZE) == 0
        &&  XMEMCMP((*entry)->issuerKeyHash, request->issuerKeyHash,
                                                         OCSP_DIGEST_SIZE) == 0)
            break;

    if (*entry == NULL) {
        *entry = (OcspEntry*)XMALLOC(sizeof(OcspEntry),
                                       ocsp->cm->heap, DYNAMIC_TYPE_OCSP_ENTRY);
        if (*entry) {
            InitOcspEntry(*entry, request);
            (*entry)->next = ocsp->ocspList;
            ocsp->ocspList = *entry;
        }
    }

    wc_UnLockMutex(&ocsp->ocspLock);

    return *entry ? 0 : MEMORY_ERROR;
}


/* Mallocs responseBuffer->buffer and is up to caller to free on success
 *
 * Returns OCSP status
 */
static int GetOcspStatus(WOLFSSL_OCSP* ocsp, OcspRequest* request,
                  OcspEntry* entry, CertStatus** status, buffer* responseBuffer,
                  void* heap)
{
    int ret = OCSP_INVALID_STATUS;

    WOLFSSL_ENTER("GetOcspStatus");

    (void)heap;
    *status = NULL;

    if (wc_LockMutex(&ocsp->ocspLock) != 0) {
        WOLFSSL_LEAVE("CheckCertOCSP", BAD_MUTEX_E);
        return BAD_MUTEX_E;
    }

    for (*status = entry->status; *status; *status = (*status)->next)
        if ((*status)->serialSz == request->serialSz
        &&  !XMEMCMP((*status)->serial, request->serial, (*status)->serialSz))
            break;

    if (responseBuffer && *status && !(*status)->rawOcspResponse) {
        /* force fetching again */
        ret = OCSP_INVALID_STATUS;
    }
    else if (*status) {
#ifndef NO_ASN_TIME
        if (XVALIDATE_DATE((*status)->thisDate,
                                             (*status)->thisDateFormat, BEFORE)
        &&  ((*status)->nextDate[0] != 0)
        &&  XVALIDATE_DATE((*status)->nextDate,
                                             (*status)->nextDateFormat, AFTER))
#endif
        {
            ret = xstat2err((*status)->status);

            if (responseBuffer) {
                responseBuffer->buffer = (byte*)XMALLOC(
                   (*status)->rawOcspResponseSz, heap,
                   DYNAMIC_TYPE_TMP_BUFFER);

                if (responseBuffer->buffer) {
                    responseBuffer->length = (*status)->rawOcspResponseSz;
                    XMEMCPY(responseBuffer->buffer,
                            (*status)->rawOcspResponse,
                            (*status)->rawOcspResponseSz);
                }
            }
        }
    }

    wc_UnLockMutex(&ocsp->ocspLock);

    return ret;
}

/* Check that the response for validity. Store result in status.
 *
 * ocsp           Context object for OCSP status.
 * response       OCSP response message data.
 * responseSz     Length of OCSP response message data.
 * reponseBuffer  Buffer object to return the response with.
 * status         The certificate status object.
 * entry          The OCSP entry for this certificate.
 * ocspRequest    Request corresponding to response.
 * heap           Heap hint used for responseBuffer
 * returns OCSP_LOOKUP_FAIL when the response is bad and 0 otherwise.
 */
int CheckOcspResponse(WOLFSSL_OCSP *ocsp, byte *response, int responseSz,
                      WOLFSSL_BUFFER_INFO *responseBuffer, CertStatus *status,
                      OcspEntry *entry, OcspRequest *ocspRequest, void* heap)
{
#ifdef WOLFSSL_SMALL_STACK
    CertStatus*   newStatus;
    OcspEntry*    newSingle;
    OcspResponse* ocspResponse;
#else
    CertStatus    newStatus[1];
    OcspEntry     newSingle[1];
    OcspResponse  ocspResponse[1];
#endif
    int           ret;
    int           validated      = 0;    /* ocsp validation flag */

    (void)heap;

#ifdef WOLFSSL_SMALL_STACK
    newStatus = (CertStatus*)XMALLOC(sizeof(CertStatus), NULL,
                                                       DYNAMIC_TYPE_OCSP_STATUS);
    newSingle = (OcspEntry*)XMALLOC(sizeof(OcspEntry), NULL,
                                                       DYNAMIC_TYPE_OCSP_ENTRY);
    ocspResponse = (OcspResponse*)XMALLOC(sizeof(OcspResponse), NULL,
                                                       DYNAMIC_TYPE_OCSP_REQUEST);

    if (newStatus == NULL || newSingle == NULL || ocspResponse == NULL) {
        if (newStatus) XFREE(newStatus, NULL, DYNAMIC_TYPE_OCSP_STATUS);
        if (newSingle) XFREE(newSingle, NULL, DYNAMIC_TYPE_OCSP_ENTRY);
        if (ocspResponse) XFREE(ocspResponse, NULL, DYNAMIC_TYPE_OCSP_REQUEST);

        WOLFSSL_LEAVE("CheckCertOCSP", MEMORY_ERROR);
        return MEMORY_E;
    }
#endif
    InitOcspResponse(ocspResponse, newSingle, newStatus, response, responseSz,
                     ocsp->cm->heap);

    ret = OcspResponseDecode(ocspResponse, ocsp->cm, ocsp->cm->heap, 0);
    if (ret != 0) {
        ocsp->error = ret;
        WOLFSSL_LEAVE("OcspResponseDecode failed", ocsp->error);
        goto end;
    }

    if (ocspResponse->responseStatus != OCSP_SUCCESSFUL) {
        WOLFSSL_MSG("OcspResponse status bad");
        goto end;
    }
    if (ocspRequest != NULL) {
        /* Has the chance to bubble up response changing ocspResponse->single to
           no longer be pointing at newSingle */
        ret = CompareOcspReqResp(ocspRequest, ocspResponse);
        if (ret != 0) {
            goto end;
        }
    }

    if (responseBuffer) {
        responseBuffer->buffer = (byte*)XMALLOC(responseSz, heap,
                                                DYNAMIC_TYPE_TMP_BUFFER);

        if (responseBuffer->buffer) {
            responseBuffer->length = responseSz;
            XMEMCPY(responseBuffer->buffer, response, responseSz);
        }
    }

    ret = xstat2err(ocspResponse->single->status->status);
    if (ret == 0) {
        validated = 1;
    }

    if (wc_LockMutex(&ocsp->ocspLock) != 0) {
        ret = BAD_MUTEX_E;
        goto end;
    }

    if (status != NULL) {
        if (status->rawOcspResponse) {
            XFREE(status->rawOcspResponse, ocsp->cm->heap,
                  DYNAMIC_TYPE_OCSP_STATUS);
        }

        /* Replace existing certificate entry with updated */
        ocspResponse->single->status->next = status->next;
        XMEMCPY(status, ocspResponse->single->status, sizeof(CertStatus));
    }
    else {
        /* Save new certificate entry */
        status = (CertStatus*)XMALLOC(sizeof(CertStatus),
                                      ocsp->cm->heap, DYNAMIC_TYPE_OCSP_STATUS);
        if (status != NULL) {
            XMEMCPY(status, ocspResponse->single->status, sizeof(CertStatus));
            status->next  = entry->status;
            entry->status = status;
            entry->ownStatus = 1;
            entry->totalStatus++;
        }
    }

    if (status && responseBuffer && responseBuffer->buffer) {
        status->rawOcspResponse = (byte*)XMALLOC(responseBuffer->length,
                                                 ocsp->cm->heap,
                                                 DYNAMIC_TYPE_OCSP_STATUS);

        if (status->rawOcspResponse) {
            status->rawOcspResponseSz = responseBuffer->length;
            XMEMCPY(status->rawOcspResponse, responseBuffer->buffer,
                    responseBuffer->length);
        }
    }

    wc_UnLockMutex(&ocsp->ocspLock);

end:
    if (ret == 0 && validated == 1) {
        WOLFSSL_MSG("New OcspResponse validated");
    } else if ((ret == ocsp->error) && (ocspResponse->single->status->status == CERT_UNKNOWN)) {
        WOLFSSL_MSG("OCSP unknown");
        ret = OCSP_CERT_UNKNOWN;
    } else if (ret != OCSP_CERT_REVOKED) {
        WOLFSSL_MSG("OCSP lookup failure");
        ret = OCSP_LOOKUP_FAIL;
    }

    FreeOcspResponse(ocspResponse);
#ifdef WOLFSSL_SMALL_STACK
    XFREE(newStatus,    NULL, DYNAMIC_TYPE_OCSP_STATUS);
    XFREE(newSingle,    NULL, DYNAMIC_TYPE_OCSP_ENTRY);
    XFREE(ocspResponse, NULL, DYNAMIC_TYPE_OCSP_REQUEST);
#endif
    return ret;
}

/* 0 on success */
/* allow user to override the maximum request size at build-time */
#ifndef OCSP_MAX_REQUEST_SZ
#define OCSP_MAX_REQUEST_SZ 2048
#endif
int CheckOcspRequest(WOLFSSL_OCSP* ocsp, OcspRequest* ocspRequest,
                     buffer* responseBuffer, void* heap)
{
    OcspEntry*  entry          = NULL;
    CertStatus* status         = NULL;
    byte*       request        = NULL;
    int         requestSz      = OCSP_MAX_REQUEST_SZ;
    int         responseSz     = 0;
    byte*       response       = NULL;
    const char* url            = NULL;
    int         urlSz          = 0;
    int         ret            = -1;
    WOLFSSL*    ssl;
    void*       ioCtx;

    WOLFSSL_ENTER("CheckOcspRequest");

    if (ocsp == NULL || ocspRequest == NULL)
        return BAD_FUNC_ARG;

    if (responseBuffer) {
        responseBuffer->buffer = NULL;
        responseBuffer->length = 0;
    }

    ret = GetOcspEntry(ocsp, ocspRequest, &entry);
    if (ret != 0)
        return ret;

    ret = GetOcspStatus(ocsp, ocspRequest, entry, &status, responseBuffer,
                        heap);
    if (ret != OCSP_INVALID_STATUS)
        return ret;

    if (responseBuffer) {
        XFREE(responseBuffer->buffer, heap, DYNAMIC_TYPE_TMP_BUFFER);
        responseBuffer->buffer = NULL;
    }

    /* get SSL and IOCtx */
    ssl = (WOLFSSL*)ocspRequest->ssl;
    ioCtx = (ssl && ssl->ocspIOCtx != NULL) ?
                                        ssl->ocspIOCtx : ocsp->cm->ocspIOCtx;

#if defined(OPENSSL_ALL) || defined(WOLFSSL_NGINX) || defined(WOLFSSL_HAPROXY)
    if (ocsp->statusCb != NULL && ssl != NULL) {
        WOLFSSL_MSG("Calling ocsp->statusCb");
        ret = ocsp->statusCb(ssl, ioCtx);
        switch (ret) {
            case SSL_TLSEXT_ERR_OK:
                ret = wolfSSL_get_ocsp_response(ssl, &response);
                ret = CheckOcspResponse(ocsp, response, ret, responseBuffer,
                                        status, entry, NULL, heap);
                if (response != NULL)
                    XFREE(response, NULL, DYNAMIC_TYPE_OPENSSL);
                break;
            case SSL_TLSEXT_ERR_NOACK:
                ret = OCSP_LOOKUP_FAIL;
                break;
            case SSL_TLSEXT_ERR_ALERT_FATAL:
            default:
                WOLFSSL_LEAVE("CheckOcspRequest", ocsp->error);
                ret = WOLFSSL_FATAL_ERROR;
                break;
        }
        WOLFSSL_LEAVE("CheckOcspRequest", ret);
        return ret;
    }
#endif

    if (ocsp->cm->ocspUseOverrideURL) {
        url = ocsp->cm->ocspOverrideURL;
        if (url != NULL && url[0] != '\0')
            urlSz = (int)XSTRLEN(url);
        else
            return OCSP_NEED_URL;
    }
    else if (ocspRequest->urlSz != 0 && ocspRequest->url != NULL) {
        url = (const char *)ocspRequest->url;
        urlSz = ocspRequest->urlSz;
    }
    else {
        /* cert doesn't have extAuthInfo, assuming CERT_GOOD */
        WOLFSSL_MSG("Cert has no OCSP URL, assuming CERT_GOOD");
        return 0;
    }

    request = (byte*)XMALLOC(requestSz, ocsp->cm->heap, DYNAMIC_TYPE_OCSP);
    if (request == NULL) {
        WOLFSSL_LEAVE("CheckCertOCSP", MEMORY_ERROR);
        return MEMORY_ERROR;
    }

    requestSz = EncodeOcspRequest(ocspRequest, request, requestSz);
    if (requestSz > 0 && ocsp->cm->ocspIOCb) {
        responseSz = ocsp->cm->ocspIOCb(ioCtx, url, urlSz,
                                        request, requestSz, &response);
    }
    if (responseSz == WOLFSSL_CBIO_ERR_WANT_READ) {
        ret = OCSP_WANT_READ;
    }

    XFREE(request, ocsp->cm->heap, DYNAMIC_TYPE_OCSP);

    if (responseSz >= 0 && response) {
        ret = CheckOcspResponse(ocsp, response, responseSz, responseBuffer, status,
                            entry, ocspRequest, heap);
    }

    if (response != NULL && ocsp->cm->ocspRespFreeCb)
        ocsp->cm->ocspRespFreeCb(ioCtx, response);

    /* Keep responseBuffer in the case of getting to response check. Caller
     * should free responseBuffer after checking OCSP return value in "ret" */
    WOLFSSL_LEAVE("CheckOcspRequest", ret);
    return ret;
}

#ifndef WOLFSSL_NO_OCSP_ISSUER_CHAIN_CHECK
static int CheckOcspResponderChain(OcspEntry* single, DecodedCert *cert,
        void* vp) {
    /* Attempt to build a chain up to cert's issuer */
    WOLFSSL_CERT_MANAGER* cm = (WOLFSSL_CERT_MANAGER*)vp;
    Signer* ca = NULL;
    Signer* prev = NULL;
    int passed = 0;

    /*
     *       Relation between certs:
     *                 CA
     *        /                 \
     *  intermediate(s)   cert in OCSP response
     *        |           with OCSP key usage ext
     *  issuer of cert
     *  in OCSP request
     */

    /* End loop if no more issuers found or if we have found a self
     * signed cert (ca == prev) */
    for (ca = GetCAByName(cm, single->issuerHash); ca != NULL && ca != prev;
            prev = ca, ca = GetCAByName(cm, ca->issuerNameHash)) {
        if (XMEMCMP(cert->issuerHash, ca->issuerNameHash,
                OCSP_DIGEST_SIZE) == 0) {
            WOLFSSL_MSG("\tOCSP Response signed by authorized "
                    "responder delegated by issuer "
                    "(found in chain)");
            passed = 1;
            break;
        }
    }
    return passed;
}
#endif

/**
 * Enforce https://www.rfc-editor.org/rfc/rfc6960#section-4.2.2.2
 * @param bs   The basic response to verify
 * @param cert The decoded bs->cert
 * @return
 */
int CheckOcspResponder(OcspResponse *bs, DecodedCert *cert, void* vp)
{
    int ret = 0;
    OcspEntry* single;

    /* Both evaluate to enum values so can't use a pre-processor check */
    WOLFSSL_ASSERT_EQ(OCSP_DIGEST_SIZE, SIGNER_DIGEST_SIZE);

    (void)vp;

    WOLFSSL_ENTER("CheckOcspResponder");

    /* In the future if this API is used more then it could be beneficial to
     * implement calling InitDecodedCert and ParseCertRelative here
     * automatically when cert == NULL. */
    if (bs == NULL || cert == NULL)
        return BAD_FUNC_ARG;

    /* Traverse the list and check that the cert has the authority to provide
     * an OCSP response for each entry. */
    for (single = bs->single; single != NULL; single = single->next) {
        int passed = 0;

        if (XMEMCMP(cert->subjectHash, single->issuerHash, OCSP_DIGEST_SIZE)
                == 0) {
            WOLFSSL_MSG("\tOCSP Response signed by issuer");
            passed = 1;
        }
        else if ((cert->extExtKeyUsage & EXTKEYUSE_OCSP_SIGN) != 0) {
            if (XMEMCMP(cert->issuerHash, single->issuerHash,
                        OCSP_DIGEST_SIZE) == 0) {
                WOLFSSL_MSG("\tOCSP Response signed by authorized responder "
                            "delegated by issuer");
                passed = 1;
            }
#ifndef WOLFSSL_NO_OCSP_ISSUER_CHAIN_CHECK
            else if (vp != NULL) {
                passed = CheckOcspResponderChain(single, cert, vp);
            }
#endif
        }

        if (!passed) {
            WOLFSSL_MSG("\tOCSP Responder not authorized");
#ifdef OPENSSL_EXTRA
            bs->verifyError = OCSP_BAD_ISSUER;
#endif
            ret = BAD_OCSP_RESPONDER;
            break;
        }
    }
    return ret;
}

#if defined(OPENSSL_ALL) || defined(WOLFSSL_NGINX) || defined(WOLFSSL_HAPROXY) || \
    defined(WOLFSSL_APACHE_HTTPD) || defined(HAVE_LIGHTY)
int wolfSSL_OCSP_resp_find_status(WOLFSSL_OCSP_BASICRESP *bs,
    WOLFSSL_OCSP_CERTID* id, int* status, int* reason,
    WOLFSSL_ASN1_TIME** revtime, WOLFSSL_ASN1_TIME** thisupd,
    WOLFSSL_ASN1_TIME** nextupd)
{
    WOLFSSL_OCSP_SINGLERESP* single;

    if (bs == NULL || id == NULL)
        return WOLFSSL_FAILURE;

    single = bs->single;
    while (single != NULL) {
        if ((XMEMCMP(single->status->serial, id->status->serial, single->status->serialSz) == 0)
         && (XMEMCMP(single->issuerHash, id->issuerHash, OCSP_DIGEST_SIZE) == 0)
         && (XMEMCMP(single->issuerKeyHash, id->issuerKeyHash, OCSP_DIGEST_SIZE) == 0)) {
            break;
        }
        single = single->next;
    }

    if (single == NULL)
        return WOLFSSL_FAILURE;

    if (status != NULL)
        *status = single->status->status;
    if (thisupd != NULL)
        *thisupd = &single->status->thisDateParsed;
    if (nextupd != NULL)
        *nextupd = &single->status->nextDateParsed;

    /* TODO: Not needed for Nginx or httpd */
    if (reason != NULL)
        *reason = 0;
    if (revtime != NULL)
        *revtime = NULL;

    return WOLFSSL_SUCCESS;
}

const char *wolfSSL_OCSP_cert_status_str(long s)
{
    switch (s) {
        case CERT_GOOD:
            return "good";
        case CERT_REVOKED:
            return "revoked";
        case CERT_UNKNOWN:
            return "unknown";
        default:
            return "(UNKNOWN)";
    }
}

int wolfSSL_OCSP_check_validity(WOLFSSL_ASN1_TIME* thisupd,
    WOLFSSL_ASN1_TIME* nextupd, long sec, long maxsec)
{
    (void)thisupd;
    (void)nextupd;
    (void)sec;
    (void)maxsec;
    /* Dates validated in DecodeSingleResponse. */
    return WOLFSSL_SUCCESS;
}

void wolfSSL_OCSP_CERTID_free(WOLFSSL_OCSP_CERTID* certId)
{
    FreeOcspEntry(certId, NULL);
    XFREE(certId, NULL, DYNAMIC_TYPE_OPENSSL);
}

WOLFSSL_OCSP_CERTID* wolfSSL_OCSP_cert_to_id(
    const WOLFSSL_EVP_MD *dgst, const WOLFSSL_X509 *subject,
    const WOLFSSL_X509 *issuer)
{
    WOLFSSL_OCSP_CERTID* certId = NULL;
    CertStatus* certStatus = NULL;
    WOLFSSL_CERT_MANAGER* cm = NULL;
    int ret = -1;
    DerBuffer* derCert = NULL;
#ifdef WOLFSSL_SMALL_STACK
    DecodedCert *cert = NULL;
#else
    DecodedCert cert[1];
#endif

    (void)dgst;

    cm = wolfSSL_CertManagerNew();
    if (cm == NULL
            || subject == NULL || subject->derCert == NULL
            || issuer  == NULL || issuer->derCert  == NULL)
        goto out;

#ifdef WOLFSSL_SMALL_STACK
    cert = (DecodedCert *)XMALLOC(sizeof(*cert), cm->heap, DYNAMIC_TYPE_DCERT);
    if (cert == NULL)
        goto out;
#endif

    ret = AllocDer(&derCert, issuer->derCert->length,
        issuer->derCert->type, NULL);
    if (ret == 0) {
        /* AddCA() frees the buffer. */
        XMEMCPY(derCert->buffer, issuer->derCert->buffer,
                issuer->derCert->length);
        ret = AddCA(cm, &derCert, WOLFSSL_USER_CA, 1);
        if (ret != WOLFSSL_SUCCESS) {
            goto out;
        }
        derCert = NULL;
    }

    ret = -1;

    certId = (WOLFSSL_OCSP_CERTID*)XMALLOC(sizeof(WOLFSSL_OCSP_CERTID),
                                           cm->heap, DYNAMIC_TYPE_OPENSSL);
    if (certId == NULL)
        goto out;

    certStatus = (CertStatus*)XMALLOC(sizeof(CertStatus), cm->heap,
                                           DYNAMIC_TYPE_OPENSSL);
    if (certStatus == NULL)
        goto out;

    XMEMSET(certId, 0, sizeof(WOLFSSL_OCSP_CERTID));
    XMEMSET(certStatus, 0, sizeof(CertStatus));

    certId->status = certStatus;
    certId->ownStatus = 1;

    InitDecodedCert(cert, subject->derCert->buffer,
                    subject->derCert->length, NULL);
    if (ParseCertRelative(cert, CERT_TYPE, VERIFY_OCSP, cm) != 0) {
        FreeDecodedCert(cert);
        goto out;
    }
    else {
        XMEMCPY(certId->issuerHash, cert->issuerHash, OCSP_DIGEST_SIZE);
        XMEMCPY(certId->issuerKeyHash, cert->issuerKeyHash, OCSP_DIGEST_SIZE);
        XMEMCPY(certId->status->serial, cert->serial, cert->serialSz);
        certId->status->serialSz = cert->serialSz;
        FreeDecodedCert(cert);
    }

    ret = 0;

out:

    if (ret != 0) {
        if (derCert != NULL)
            FreeDer(&derCert);
        if (certId != NULL) {
            XFREE(certId, cm->heap, DYNAMIC_TYPE_OPENSSL);
            certId = NULL;
        }
        if (certStatus)
            XFREE(certStatus, cm->heap, DYNAMIC_TYPE_OPENSSL);
    }

#ifdef WOLFSSL_SMALL_STACK
    if (cert != NULL)
        XFREE(cert, cm->heap, DYNAMIC_TYPE_DCERT);
#endif

    if (cm != NULL)
        wolfSSL_CertManagerFree(cm);

    return certId;
}

void wolfSSL_OCSP_BASICRESP_free(WOLFSSL_OCSP_BASICRESP* basicResponse)
{
    wolfSSL_OCSP_RESPONSE_free(basicResponse);
}

/* Signature verified in DecodeBasicOcspResponse.
 * But no store available to verify certificate. */
int wolfSSL_OCSP_basic_verify(WOLFSSL_OCSP_BASICRESP *bs,
    WOLF_STACK_OF(WOLFSSL_X509) *certs, WOLFSSL_X509_STORE *st, unsigned long flags)
{
    int         ret = WOLFSSL_FAILURE;
#ifdef WOLFSSL_SMALL_STACK
    DecodedCert *cert;
#else
    DecodedCert cert[1];
#endif
    byte        certInit = 0;
    int         idx;

    (void)certs;

    if (flags & OCSP_NOVERIFY)
        return WOLFSSL_SUCCESS;

#ifdef WOLFSSL_SMALL_STACK
    cert = (DecodedCert *)
        XMALLOC(sizeof(*cert), (st && st->cm) ? st->cm->heap : NULL,
                DYNAMIC_TYPE_DCERT);
    if (cert == NULL)
        return WOLFSSL_FAILURE;
#endif

#ifdef OPENSSL_EXTRA
    if (bs->verifyError != OCSP_VERIFY_ERROR_NONE)
        goto out;
#endif

    if (flags & OCSP_TRUSTOTHER) {
        for (idx = 0; idx < wolfSSL_sk_X509_num(certs); idx++) {
            WOLFSSL_X509* x = wolfSSL_sk_X509_value(certs, idx);
            int derSz = 0;
            const byte* der = wolfSSL_X509_get_der(x, &derSz);

            if (derSz == (int)bs->certSz && XMEMCMP(bs->cert, der, derSz) == 0) {
                ret = WOLFSSL_SUCCESS;
                goto out;
            }
        }
    }

    InitDecodedCert(cert, bs->cert, bs->certSz, NULL);
    certInit = 1;
    if (ParseCertRelative(cert, CERT_TYPE, VERIFY, st->cm) < 0)
        goto out;

    if (!(flags & OCSP_NOCHECKS)) {
        if (CheckOcspResponder(bs, cert, st->cm) != 0)
            goto out;
    }

    ret = WOLFSSL_SUCCESS;
out:
    if (certInit)
        FreeDecodedCert(cert);

#ifdef WOLFSSL_SMALL_STACK
    XFREE(cert, (st && st->cm) ? st->cm->heap : NULL, DYNAMIC_TYPE_DCERT);
#endif

    return ret;
}

void wolfSSL_OCSP_RESPONSE_free(OcspResponse* response)
{
    if (response == NULL)
        return;

    if (response->single != NULL) {
        FreeOcspEntry(response->single, NULL);
        XFREE(response->single, NULL, DYNAMIC_TYPE_OCSP_ENTRY);
    }

    if (response->source != NULL)
        XFREE(response->source, NULL, DYNAMIC_TYPE_TMP_BUFFER);

    XFREE(response, NULL, DYNAMIC_TYPE_OCSP_REQUEST);
}

#ifndef NO_BIO
OcspResponse* wolfSSL_d2i_OCSP_RESPONSE_bio(WOLFSSL_BIO* bio,
    OcspResponse** response)
{
    byte*         data;
    byte*         p;
    int           len;
    int           dataAlloced = 0;
    OcspResponse* ret = NULL;

    if (bio == NULL)
        return NULL;

    if (bio->type == WOLFSSL_BIO_MEMORY) {
        len = wolfSSL_BIO_get_mem_data(bio, &data);
        if (len <= 0 || data == NULL) {
            return NULL;
        }
    }
#ifndef NO_FILESYSTEM
    else if (bio->type == WOLFSSL_BIO_FILE) {
        long fcur;
        long flen;

        if (bio->ptr == NULL)
            return NULL;

        fcur = XFTELL((XFILE)bio->ptr);
        if (fcur < 0)
            return NULL;
        if(XFSEEK((XFILE)bio->ptr, 0, SEEK_END) != 0)
            return NULL;
        flen = XFTELL((XFILE)bio->ptr);
        if (flen < 0)
            return NULL;
        if (XFSEEK((XFILE)bio->ptr, fcur, SEEK_SET) != 0)
            return NULL;

        /* check calculated length */
        fcur = flen - fcur;
        if (fcur > MAX_WOLFSSL_FILE_SIZE || fcur <= 0)
            return NULL;

        data = (byte*)XMALLOC(fcur, 0, DYNAMIC_TYPE_TMP_BUFFER);
        if (data == NULL)
            return NULL;
        dataAlloced = 1;

        len = wolfSSL_BIO_read(bio, (char *)data, (int)flen);
    }
#endif
    else
        return NULL;

    if (len > 0) {
        p = data;
        ret = wolfSSL_d2i_OCSP_RESPONSE(response, (const unsigned char **)&p,
            len);
    }

    if (dataAlloced)
        XFREE(data, 0, DYNAMIC_TYPE_TMP_BUFFER);

    return ret;
}
#endif /* !NO_BIO */

OcspResponse* wolfSSL_d2i_OCSP_RESPONSE(OcspResponse** response,
    const unsigned char** data, int len)
{
    OcspResponse *resp = NULL;
    word32 idx = 0;
    int length = 0;
    int ret;

    if (data == NULL)
        return NULL;

    if (response != NULL)
        resp = *response;
    if (resp == NULL) {
        resp = (OcspResponse*)XMALLOC(sizeof(OcspResponse), NULL,
                                      DYNAMIC_TYPE_OCSP_REQUEST);
        if (resp == NULL)
            return NULL;
        XMEMSET(resp, 0, sizeof(OcspResponse));
    }

    resp->source = (byte*)XMALLOC(len, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (resp->source == NULL) {
        XFREE(resp, NULL, DYNAMIC_TYPE_OCSP_REQUEST);
        return NULL;
    }
    resp->single = (OcspEntry*)XMALLOC(sizeof(OcspEntry), NULL,
                                      DYNAMIC_TYPE_OCSP_ENTRY);
    if (resp->single == NULL) {
        XFREE(resp->source, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(resp, NULL, DYNAMIC_TYPE_OCSP_REQUEST);
        return NULL;
    }
    XMEMSET(resp->single, 0, sizeof(OcspEntry));
    resp->single->status = (CertStatus*)XMALLOC(sizeof(CertStatus), NULL,
                                      DYNAMIC_TYPE_OCSP_STATUS);
    resp->single->ownStatus = 1;
    if (resp->single->status == NULL) {
        XFREE(resp->source, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(resp->single, NULL, DYNAMIC_TYPE_OCSP_ENTRY);
        XFREE(resp, NULL, DYNAMIC_TYPE_OCSP_REQUEST);
        return NULL;
    }
    XMEMSET(resp->single->status, 0, sizeof(CertStatus));

    XMEMCPY(resp->source, *data, len);
    resp->maxIdx = len;

    ret = OcspResponseDecode(resp, NULL, NULL, 1);
    if (ret != 0 && ret != ASN_OCSP_CONFIRM_E) {
        /* for just converting from a DER to an internal structure the CA may
         * not yet be known to this function for signature verification */
        wolfSSL_OCSP_RESPONSE_free(resp);
        return NULL;
    }

    if (GetSequence(*data, &idx, &length, len) >= 0)
        (*data) += idx + length;

    return resp;
}

int wolfSSL_i2d_OCSP_RESPONSE(OcspResponse* response,
    unsigned char** data)
{
    if (data == NULL)
        return response->maxIdx;

    XMEMCPY(*data, response->source, response->maxIdx);
    return response->maxIdx;
}

int wolfSSL_OCSP_response_status(OcspResponse *response)
{
    return response->responseStatus;
}

const char *wolfSSL_OCSP_response_status_str(long s)
{
    switch (s) {
        case OCSP_SUCCESSFUL:
            return "successful";
        case OCSP_MALFORMED_REQUEST:
            return "malformedrequest";
        case OCSP_INTERNAL_ERROR:
            return "internalerror";
        case OCSP_TRY_LATER:
            return "trylater";
        case OCSP_SIG_REQUIRED:
            return "sigrequired";
        case OCSP_UNAUTHORIZED:
            return "unauthorized";
        default:
            return "(UNKNOWN)";
    }
}

WOLFSSL_OCSP_BASICRESP* wolfSSL_OCSP_response_get1_basic(OcspResponse* response)
{
    WOLFSSL_OCSP_BASICRESP* bs;

    bs = (WOLFSSL_OCSP_BASICRESP*)XMALLOC(sizeof(WOLFSSL_OCSP_BASICRESP), NULL,
                                          DYNAMIC_TYPE_OCSP_REQUEST);
    if (bs == NULL)
        return NULL;

    XMEMCPY(bs, response, sizeof(OcspResponse));
    bs->single = (OcspEntry*)XMALLOC(sizeof(OcspEntry), NULL,
                                    DYNAMIC_TYPE_OCSP_ENTRY);
    bs->source = (byte*)XMALLOC(bs->maxIdx, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (bs->single == NULL || bs->source == NULL) {
        if (bs->single) {
            XFREE(bs->single, NULL, DYNAMIC_TYPE_OCSP_ENTRY);
            bs->single = NULL;
        }
        wolfSSL_OCSP_RESPONSE_free(bs);
        bs = NULL;
    }
    else {
        XMEMCPY(bs->single, response->single, sizeof(OcspEntry));
        XMEMCPY(bs->source, response->source, response->maxIdx);
        bs->single->ownStatus = 0;
    }
    return bs;
}

OcspRequest* wolfSSL_OCSP_REQUEST_new(void)
{
    OcspRequest* request;

    request = (OcspRequest*)XMALLOC(sizeof(OcspRequest), NULL,
                                    DYNAMIC_TYPE_OPENSSL);
    if (request != NULL)
        XMEMSET(request, 0, sizeof(OcspRequest));

    return request;
}

void wolfSSL_OCSP_REQUEST_free(OcspRequest* request)
{
    FreeOcspRequest(request);
    XFREE(request, NULL, DYNAMIC_TYPE_OPENSSL);
}

int wolfSSL_i2d_OCSP_REQUEST(OcspRequest* request, unsigned char** data)
{
    int size;

    size = EncodeOcspRequest(request, NULL, 0);
    if (size <= 0 || data == NULL)
        return size;

    return EncodeOcspRequest(request, *data, size);
}

WOLFSSL_OCSP_ONEREQ* wolfSSL_OCSP_request_add0_id(OcspRequest *req,
    WOLFSSL_OCSP_CERTID *cid)
{
    if (req == NULL || cid == NULL || cid->status == NULL)
        return NULL;

    if (req->cid != NULL)
        wolfSSL_OCSP_CERTID_free((WOLFSSL_OCSP_CERTID*)req->cid);
    /* Keep to free */
    req->cid = (void*)cid;

    XMEMCPY(req->issuerHash, cid->issuerHash, KEYID_SIZE);
    XMEMCPY(req->issuerKeyHash, cid->issuerKeyHash, KEYID_SIZE);
    if (cid->status->serialSz > req->serialSz) {
        if (req->serial != NULL)
            XFREE(req->serial, req->heap, DYNAMIC_TYPE_OCSP);
        req->serial = (byte*)XMALLOC(cid->status->serialSz,
                req->heap, DYNAMIC_TYPE_OCSP_REQUEST);
        if (req->serial == NULL)
            return NULL;
    }
    XMEMCPY(req->serial, cid->status->serial, cid->status->serialSz);
    req->serialSz = cid->status->serialSz;

    return req;
}

WOLFSSL_OCSP_CERTID* wolfSSL_OCSP_CERTID_dup(WOLFSSL_OCSP_CERTID* id)
{
    WOLFSSL_OCSP_CERTID* certId;

    if (id == NULL)
        return NULL;

    certId = (WOLFSSL_OCSP_CERTID*)XMALLOC(sizeof(WOLFSSL_OCSP_CERTID),
        NULL, DYNAMIC_TYPE_OPENSSL);
    if (certId) {
        XMEMCPY(certId, id, sizeof(WOLFSSL_OCSP_CERTID));
    }
    return certId;
}
#endif

#if defined(OPENSSL_ALL) || defined(APACHE_HTTPD) || defined(WOLFSSL_HAPROXY)
#ifndef NO_BIO
int wolfSSL_i2d_OCSP_REQUEST_bio(WOLFSSL_BIO* out,
        WOLFSSL_OCSP_REQUEST *req)
{
    int size = -1;
    unsigned char* data = NULL;

    WOLFSSL_ENTER("wolfSSL_i2d_OCSP_REQUEST_bio");
    if (out == NULL || req == NULL)
        return WOLFSSL_FAILURE;

    size = wolfSSL_i2d_OCSP_REQUEST(req, NULL);
    if (size > 0) {
        data = (unsigned char*) XMALLOC(size, out->heap,
                DYNAMIC_TYPE_TMP_BUFFER);
    }

    if (data != NULL) {
        size = wolfSSL_i2d_OCSP_REQUEST(req, &data);
    }

    if (size <= 0) {
        XFREE(data, out->heap, DYNAMIC_TYPE_TMP_BUFFER);
        return WOLFSSL_FAILURE;
    }

    if (wolfSSL_BIO_write(out, data, size) == (int)size) {
        XFREE(data, out->heap, DYNAMIC_TYPE_TMP_BUFFER);
        return WOLFSSL_SUCCESS;
    }

    XFREE(data, out->heap, DYNAMIC_TYPE_TMP_BUFFER);
    return WOLFSSL_FAILURE;
}
#endif /* !NO_BIO */

int wolfSSL_i2d_OCSP_CERTID(WOLFSSL_OCSP_CERTID* id, unsigned char** data)
{
    if (id == NULL || data == NULL)
        return WOLFSSL_FAILURE;

    if (*data != NULL) {
        XMEMCPY(*data, id->rawCertId, id->rawCertIdSize);
        *data = *data + id->rawCertIdSize;
    }
    else {
        *data = (unsigned char*)XMALLOC(id->rawCertIdSize, NULL, DYNAMIC_TYPE_OPENSSL);
        if (*data == NULL) {
            return WOLFSSL_FAILURE;
        }
        XMEMCPY(*data, id->rawCertId, id->rawCertIdSize);
    }

    return id->rawCertIdSize;
}

WOLFSSL_OCSP_CERTID* wolfSSL_d2i_OCSP_CERTID(WOLFSSL_OCSP_CERTID** cidOut,
                                             const unsigned char** derIn,
                                             int length)
{
    WOLFSSL_OCSP_CERTID *cid = NULL;

    if ((cidOut != NULL) && (derIn != NULL) && (*derIn != NULL) &&
        (length > 0)) {

        cid = *cidOut;

        /* If a NULL is passed we allocate the memory for the caller. */
        if (cid == NULL) {
            cid = (WOLFSSL_OCSP_CERTID*)XMALLOC(sizeof(*cid), NULL,
                                                DYNAMIC_TYPE_OPENSSL);
        }
        else if (cid->rawCertId != NULL) {
            XFREE(cid->rawCertId, NULL, DYNAMIC_TYPE_OPENSSL);
            cid->rawCertId = NULL;
            cid->rawCertIdSize = 0;
        }

        if (cid != NULL) {
            cid->rawCertId = (byte*)XMALLOC(length + 1, NULL, DYNAMIC_TYPE_OPENSSL);
            if (cid->rawCertId != NULL) {
                XMEMCPY(cid->rawCertId, *derIn, length);
                cid->rawCertIdSize = length;

                /* Per spec. advance past the data that is being returned
                 * to the caller. */
                *cidOut = cid;
                *derIn = *derIn + length;

                return cid;
            }
        }
    }

    if ((cid != NULL) && ((cidOut == NULL) || (cid != *cidOut))) {
        XFREE(cid, NULL, DYNAMIC_TYPE_OPENSSL);
    }

    return NULL;
}

const WOLFSSL_OCSP_CERTID* wolfSSL_OCSP_SINGLERESP_get0_id(const WOLFSSL_OCSP_SINGLERESP *single)
{
    return single;
}

/**
 * Compare two WOLFSSL_OCSP_CERTID objects
 * @param a
 * @param b
 * @return 0 on success and when objects have the same id otherwise either
 *         the id's don't match or an error occurred
 */
int wolfSSL_OCSP_id_cmp(WOLFSSL_OCSP_CERTID *a, WOLFSSL_OCSP_CERTID *b)
{
    int ret = 0;
    if (a == NULL || b == NULL)
        return WOLFSSL_FATAL_ERROR;

    ret = a->hashAlgoOID != b->hashAlgoOID;
    if (ret == 0)
        ret = XMEMCMP(a->issuerHash, b->issuerHash, OCSP_DIGEST_SIZE);
    if (ret == 0)
        ret = XMEMCMP(a->issuerKeyHash, b->issuerKeyHash, OCSP_DIGEST_SIZE);
    if (ret == 0) {
        if (a->status != NULL && b->status != NULL) {
            if (a->status->serialSz == b->status->serialSz)
                ret = XMEMCMP(a->status->serial, b->status->serial,
                        a->status->serialSz);
            else
                ret = -1;
        }
        else if (a->status != b->status) {
            /* If either is not null then return non-zero */
            ret = -1;
        }
    }
    return ret;
}

int wolfSSL_OCSP_single_get0_status(WOLFSSL_OCSP_SINGLERESP *single,
                                    int *reason,
                                    WOLFSSL_ASN1_TIME **revtime,
                                    WOLFSSL_ASN1_TIME **thisupd,
                                    WOLFSSL_ASN1_TIME **nextupd)
{
    if (single == NULL)
        return WOLFSSL_FAILURE;

    if (thisupd != NULL)
        *thisupd = &single->status->thisDateParsed;
    if (nextupd != NULL)
        *nextupd = &single->status->nextDateParsed;

    if (reason != NULL)
        *reason = 0;
    if (revtime != NULL)
        *revtime = NULL;

    return single->status->status;
}

int wolfSSL_OCSP_resp_count(WOLFSSL_OCSP_BASICRESP *bs)
{
    WOLFSSL_OCSP_SINGLERESP* single;
    int count = 0;

    if (bs == NULL)
        return WOLFSSL_FAILURE;

    single = bs->single;
    while(single != NULL)
    {
        ++count;
        single = single->next;
    }

    return count;
}

WOLFSSL_OCSP_SINGLERESP* wolfSSL_OCSP_resp_get0(WOLFSSL_OCSP_BASICRESP *bs, int idx)
{
    WOLFSSL_OCSP_SINGLERESP* single;
    int currIdx = 0;

    if (bs == NULL)
        return NULL;

    single = bs->single;
    while(single != NULL && currIdx != idx)
    {
        single = single->next;
        ++currIdx;
    }

    return single;
}

#endif /* OPENSSL_ALL || APACHE_HTTPD || WOLFSSL_HAPROXY */

#ifdef OPENSSL_EXTRA
#ifndef NO_WOLFSSL_STUB
int wolfSSL_OCSP_REQUEST_add_ext(OcspRequest* req, WOLFSSL_X509_EXTENSION* ext,
        int idx)
{
    WOLFSSL_STUB("wolfSSL_OCSP_REQUEST_add_ext");
    (void)req;
    (void)ext;
    (void)idx;
    return WOLFSSL_FATAL_ERROR;
}
#endif

#ifndef NO_WOLFSSL_STUB
OcspResponse* wolfSSL_OCSP_response_create(int status,
    WOLFSSL_OCSP_BASICRESP* bs)
{
    WOLFSSL_STUB("wolfSSL_OCSP_response_create");
    (void)status;
    (void)bs;
    return NULL;
}
#endif

#ifndef NO_WOLFSSL_STUB
const char* wolfSSL_OCSP_crl_reason_str(long s)
{
    WOLFSSL_STUB("wolfSSL_OCSP_crl_reason_str");
    (void)s;
    return NULL;
}
#endif

/* Returns elements of an OCSP_CERTID struct. Currently only supports
 * returning the serial number, and returns an error if user requests
 * any of name, pmd, and/or keyHash.
 * Return 1 on success, 0 on failure */
int wolfSSL_OCSP_id_get0_info(WOLFSSL_ASN1_STRING **name,
  WOLFSSL_ASN1_OBJECT **pmd, WOLFSSL_ASN1_STRING **keyHash,
  WOLFSSL_ASN1_INTEGER **serial, WOLFSSL_OCSP_CERTID *cid)
{
    WOLFSSL_ENTER("wolfSSL_OCSP_id_get0_info");

    if (cid == NULL)
        return 0;

    /* build up ASN1_INTEGER for serial */
    if (serial != NULL) {
        int i = 0;
        WOLFSSL_ASN1_INTEGER* ser;

        ser = wolfSSL_ASN1_INTEGER_new();
        if (ser == NULL)
            return 0;

        if (cid->status->serialSz > (WOLFSSL_ASN1_INTEGER_MAX - 2)) {
            /* allocate data buffer, +2 for type and length */
            ser->data = (unsigned char*)XMALLOC(cid->status->serialSz + 2, NULL,
                DYNAMIC_TYPE_OPENSSL);
            if (ser->data == NULL) {
                wolfSSL_ASN1_INTEGER_free(ser);
                return 0;
            }
            ser->dataMax = cid->status->serialSz + 2;
            ser->isDynamic = 1;
        } else {
            /* Use array instead of dynamic memory */
            ser->data    = ser->intData;
            ser->dataMax = WOLFSSL_ASN1_INTEGER_MAX;
        }

        #if defined(WOLFSSL_QT) || defined(WOLFSSL_HAPROXY)
            /* Serial number starts at 0 index of ser->data */
            XMEMCPY(&ser->data[i], cid->status->serial, cid->status->serialSz);
            ser->length = cid->status->serialSz;
        #else
            ser->data[i++] = ASN_INTEGER;
            i += SetLength(cid->status->serialSz, ser->data + i);
            XMEMCPY(&ser->data[i], cid->status->serial, cid->status->serialSz);
            ser->length = i + cid->status->serialSz;
        #endif

        cid->status->serialInt = ser;
        *serial = ser;
    }

    /* Not needed for Apache, return error if user is requesting */
    if (name != NULL || pmd != NULL || keyHash != NULL) {
        if (name != NULL)
            *name = NULL;

        if (pmd != NULL)
            *pmd = NULL;

        if (keyHash != NULL)
            *keyHash = NULL;
        return 0;
    }

    return 1;
}

int wolfSSL_OCSP_request_add1_nonce(OcspRequest* req, unsigned char* val,
        int sz)
{
    WC_RNG rng;

    WOLFSSL_ENTER("wolfSSL_OCSP_request_add1_nonce");

    if (req == NULL || sz > MAX_OCSP_NONCE_SZ) {
        WOLFSSL_MSG("Bad parameter");
        return WOLFSSL_FAILURE;
    }

    if (sz <= 0)
        sz = MAX_OCSP_NONCE_SZ;

    if (val != NULL) {
        XMEMCPY(req->nonce, val, sz);
    }
    else {
        if (
#ifndef HAVE_FIPS
            wc_InitRng_ex(&rng, req->heap, INVALID_DEVID)
#else
            wc_InitRng(&rng)
#endif
            != 0) {
            WOLFSSL_MSG("RNG init failed");
            return WOLFSSL_FAILURE;
        }
        if (wc_RNG_GenerateBlock(&rng, req->nonce, sz) != 0) {
            WOLFSSL_MSG("wc_RNG_GenerateBlock failed");
            wc_FreeRng(&rng);
            return WOLFSSL_FAILURE;
        }
        wc_FreeRng(&rng);
    }
    req->nonceSz = sz;

    return WOLFSSL_SUCCESS;
}

/* Returns result of OCSP nonce comparison. Return values:
 *  1 - nonces are both present and equal
 *  2 - both nonces are absent
 *  3 - nonce only present in response
 * -1 - nonce only present in request
 *  0 - both nonces present and equal
 */
int wolfSSL_OCSP_check_nonce(OcspRequest* req, WOLFSSL_OCSP_BASICRESP* bs)
{
    byte* reqNonce = NULL;
    byte* rspNonce = NULL;
    int reqNonceSz = 0;
    int rspNonceSz = 0;

    WOLFSSL_ENTER("wolfSSL_OCSP_check_nonce");

    if (req != NULL) {
        reqNonce = req->nonce;
        reqNonceSz = req->nonceSz;
    }

    if (bs != NULL) {
        rspNonce = bs->nonce;
        rspNonceSz = bs->nonceSz;
    }

    /* nonce absent in both req and rsp */
    if (reqNonce == NULL && rspNonce == NULL)
        return 2;

    /* nonce present in rsp only */
    if (reqNonce == NULL && rspNonce != NULL)
        return 3;

    /* nonce present in req only */
    if (reqNonce != NULL && rspNonce == NULL)
        return -1;

    /* nonces are present and equal, return 1. Extra NULL check for fixing
        scan-build warning. */
    if (reqNonceSz == rspNonceSz && reqNonce && rspNonce) {
        if (XMEMCMP(reqNonce, rspNonce, reqNonceSz) == 0)
            return 1;
    }

    /* nonces are present but not equal */
    return 0;
}
#endif /* OPENSSL_EXTRA */

#else /* HAVE_OCSP */


#ifdef _MSC_VER
    /* 4206 warning for blank file */
    #pragma warning(disable: 4206)
#endif


#endif /* HAVE_OCSP */
#endif /* WOLFCRYPT_ONLY */
