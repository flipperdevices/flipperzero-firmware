/* crl.c
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


/*
CRL Options:
 * CRL_STATIC_REVOKED_LIST:                                        default: off
 *                         Enables fixed static list of RevokedCerts to allow
 *                         for a binary search.
 * CRL_MAX_REVOKED_CERTS:                                          default: 4
 *                         Specifies the number of buffers to hold RevokedCerts.
 *                         The default value is set to 4.
 * CRL_REPORT_LOAD_ERRORS:                                         default: off
 *                         Return any errors encountered during loading CRL
 *                         from a directory.
*/
#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#ifndef WOLFCRYPT_ONLY
#ifdef HAVE_CRL

#include <wolfssl/internal.h>
#include <wolfssl/error-ssl.h>

#ifndef WOLFSSL_LINUXKM
    #include <string.h>
#endif

#ifdef HAVE_CRL_MONITOR
    #if defined(__MACH__) || defined(__FreeBSD__) || defined(__linux__) || \
         defined(_MSC_VER)
        static int StopMonitor(wolfSSL_CRL_mfd_t mfd);
    #else
        #error "CRL monitor only currently supported on linux or mach or windows"
    #endif
#endif /* HAVE_CRL_MONITOR */


/* Initialize CRL members */
int InitCRL(WOLFSSL_CRL* crl, WOLFSSL_CERT_MANAGER* cm)
{
    WOLFSSL_ENTER("InitCRL");
    if(cm != NULL)
        crl->heap = cm->heap;
    else
        crl->heap = NULL;
    crl->cm = cm;
    crl->crlList  = NULL;
    crl->currentEntry = NULL;
    crl->monitors[0].path = NULL;
    crl->monitors[1].path = NULL;
#ifdef HAVE_CRL_MONITOR
    crl->tid = INVALID_THREAD_VAL;
    crl->mfd = WOLFSSL_CRL_MFD_INIT_VAL;
    crl->setup = 0; /* thread setup done predicate */
    if (wolfSSL_CondInit(&crl->cond) != 0) {
        WOLFSSL_MSG("thread condition init failed");
        return BAD_COND_E;
    }
#endif
#ifdef HAVE_CRL_IO
    crl->crlIOCb = NULL;
#endif
    if (wc_InitMutex(&crl->crlLock) != 0) {
        WOLFSSL_MSG("Init Mutex failed");
        return BAD_MUTEX_E;
    }

    return 0;
}


/* Initialize CRL Entry */
static int InitCRL_Entry(CRL_Entry* crle, DecodedCRL* dcrl, const byte* buff,
                         int verified, void* heap)
{
    WOLFSSL_ENTER("InitCRL_Entry");

    XMEMCPY(crle->issuerHash, dcrl->issuerHash, CRL_DIGEST_SIZE);
    /* XMEMCPY(crle->crlHash, dcrl->crlHash, CRL_DIGEST_SIZE);
     * copy the hash here if needed for optimized comparisons */
    XMEMCPY(crle->lastDate, dcrl->lastDate, MAX_DATE_SIZE);
    XMEMCPY(crle->nextDate, dcrl->nextDate, MAX_DATE_SIZE);
    crle->lastDateFormat = dcrl->lastDateFormat;
    crle->nextDateFormat = dcrl->nextDateFormat;
    crle->version = dcrl->version;

#if defined(OPENSSL_EXTRA)
    crle->lastDateAsn1.length = MAX_DATE_SIZE;
    XMEMCPY (crle->lastDateAsn1.data, crle->lastDate,
             crle->lastDateAsn1.length);
    crle->lastDateAsn1.type = crle->lastDateFormat;
    crle->nextDateAsn1.length = MAX_DATE_SIZE;
    XMEMCPY (crle->nextDateAsn1.data, crle->nextDate,
             crle->nextDateAsn1.length);
    crle->nextDateAsn1.type = crle->nextDateFormat;

    crle->issuer = NULL;
    wolfSSL_d2i_X509_NAME(&crle->issuer, (unsigned char**)&dcrl->issuer,
                          dcrl->issuerSz);
    if (crle->issuer == NULL) {
        return WOLFSSL_FAILURE;
    }
#endif
#ifdef CRL_STATIC_REVOKED_LIST
    /* ParseCRL_CertList() has already cached the Revoked certs into
       the crle->certs array */
#else
    crle->certs = dcrl->certs;   /* take ownership */
#endif
    dcrl->certs = NULL;
    crle->totalCerts = dcrl->totalCerts;
    crle->crlNumber = dcrl->crlNumber;
    crle->verified = verified;
    if (!verified) {
        crle->tbsSz = dcrl->sigIndex - dcrl->certBegin;
        crle->signatureSz = dcrl->sigLength;
        crle->signatureOID = dcrl->signatureOID;
        crle->toBeSigned = (byte*)XMALLOC(crle->tbsSz, heap,
                                          DYNAMIC_TYPE_CRL_ENTRY);
        if (crle->toBeSigned == NULL)
            return -1;
        crle->signature = (byte*)XMALLOC(crle->signatureSz, heap,
                                         DYNAMIC_TYPE_CRL_ENTRY);
        if (crle->signature == NULL) {
            XFREE(crle->toBeSigned, heap, DYNAMIC_TYPE_CRL_ENTRY);
            crle->toBeSigned = NULL;
            return -1;
        }
        XMEMCPY(crle->toBeSigned, buff + dcrl->certBegin, crle->tbsSz);
        XMEMCPY(crle->signature, dcrl->signature, crle->signatureSz);
    #ifndef NO_SKID
        crle->extAuthKeyIdSet = dcrl->extAuthKeyIdSet;
        if (crle->extAuthKeyIdSet)
            XMEMCPY(crle->extAuthKeyId, dcrl->extAuthKeyId, KEYID_SIZE);
    #endif
    }
    else {
        crle->toBeSigned = NULL;
        crle->signature = NULL;
    }

    (void)verified;
    (void)heap;

    return 0;
}


/* Free all CRL Entry resources */
static void FreeCRL_Entry(CRL_Entry* crle, void* heap)
{
#ifdef CRL_STATIC_REVOKED_LIST
    if (crle != NULL) {
        XMEMSET(crle->certs, 0, CRL_MAX_REVOKED_CERTS*sizeof(RevokedCert));
    }
#else
    RevokedCert* tmp = crle->certs;
    RevokedCert* next;

    WOLFSSL_ENTER("FreeCRL_Entry");

    while (tmp) {
        next = tmp->next;
        XFREE(tmp, heap, DYNAMIC_TYPE_REVOKED);
        tmp = next;
    }
#endif
    if (crle->signature != NULL)
        XFREE(crle->signature, heap, DYNAMIC_TYPE_CRL_ENTRY);
    if (crle->toBeSigned != NULL)
        XFREE(crle->toBeSigned, heap, DYNAMIC_TYPE_CRL_ENTRY);
#if defined(OPENSSL_EXTRA)
    if (crle->issuer != NULL) {
        FreeX509Name(crle->issuer);
        XFREE(crle->issuer, heap, DYNAMIC_TYPE_X509);
    }
#endif
    (void)heap;
}



/* Free all CRL resources */
void FreeCRL(WOLFSSL_CRL* crl, int dynamic)
{
    CRL_Entry* tmp = crl->crlList;

    WOLFSSL_ENTER("FreeCRL");
    if (crl->monitors[0].path)
        XFREE(crl->monitors[0].path, crl->heap, DYNAMIC_TYPE_CRL_MONITOR);

    if (crl->monitors[1].path)
        XFREE(crl->monitors[1].path, crl->heap, DYNAMIC_TYPE_CRL_MONITOR);

    XFREE(crl->currentEntry, crl->heap, DYNAMIC_TYPE_CRL_ENTRY);
    crl->currentEntry = NULL;
    while(tmp) {
        CRL_Entry* next = tmp->next;
        FreeCRL_Entry(tmp, crl->heap);
        XFREE(tmp, crl->heap, DYNAMIC_TYPE_CRL_ENTRY);
        tmp = next;
    }

#ifdef HAVE_CRL_MONITOR
    if (crl->tid != INVALID_THREAD_VAL) {
        WOLFSSL_MSG("stopping monitor thread");
        if (StopMonitor(crl->mfd) == 0) {
            if (wolfSSL_JoinThread(crl->tid) != 0)
                WOLFSSL_MSG("stop monitor failed in wolfSSL_JoinThread");
        }
        else {
            WOLFSSL_MSG("stop monitor failed");
        }
    }
    if (wolfSSL_CondFree(&crl->cond) != 0)
        WOLFSSL_MSG("wolfSSL_CondFree failed in FreeCRL");
#endif
    wc_FreeMutex(&crl->crlLock);
    if (dynamic)   /* free self */
        XFREE(crl, crl->heap, DYNAMIC_TYPE_CRL);
}

static int FindRevokedSerial(DecodedCert* cert, RevokedCert* rc, int totalCerts)
{
    int ret = 0;
#ifdef CRL_STATIC_REVOKED_LIST
    /* do binary search */
    int low, high, mid;

    low = 0;
    high = totalCerts - 1;

    while (low <= high) {
        mid = (low + high) / 2;

        if (XMEMCMP(rc[mid].serialNumber, cert->serial, rc->serialSz) < 0) {
            low = mid + 1;
        }
        else if (XMEMCMP(rc[mid].serialNumber, cert->serial,
                                                        rc->serialSz) > 0) {
            high = mid - 1;
        }
        else {
            WOLFSSL_MSG("Cert revoked");
            ret = CRL_CERT_REVOKED;
            break;
        }
    }
#else
    (void)totalCerts;
    /* search in the linked list*/

    while (rc) {
        if (rc->serialSz == cert->serialSz &&
               XMEMCMP(rc->serialNumber, cert->serial, rc->serialSz) == 0) {
            WOLFSSL_MSG("Cert revoked");
            ret = CRL_CERT_REVOKED;
            break;
        }
        rc = rc->next;
    }
#endif
    return ret;
}
static int CheckCertCRLList(WOLFSSL_CRL* crl, DecodedCert* cert, int *pFoundEntry)
{
    CRL_Entry* crle;
    int        foundEntry = 0;
    int        ret = 0;

    if (wc_LockMutex(&crl->crlLock) != 0) {
        WOLFSSL_MSG("wc_LockMutex failed");
        return BAD_MUTEX_E;
    }

    crle = crl->crlList;

    while (crle) {
        if (XMEMCMP(crle->issuerHash, cert->issuerHash, CRL_DIGEST_SIZE) == 0) {
            WOLFSSL_MSG("Found CRL Entry on list");

            if (crle->verified == 0) {
                Signer* ca = NULL;
            #ifndef NO_SKID
                byte extAuthKeyId[KEYID_SIZE];
            #endif
                byte issuerHash[CRL_DIGEST_SIZE];
                byte* tbs;
                word32 tbsSz = crle->tbsSz;
                byte* sig = NULL;
                word32 sigSz = crle->signatureSz;
                word32 sigOID = crle->signatureOID;
                SignatureCtx sigCtx;

                tbs = (byte*)XMALLOC(tbsSz, crl->heap, DYNAMIC_TYPE_CRL_ENTRY);
                if (tbs == NULL) {
                    wc_UnLockMutex(&crl->crlLock);
                    return MEMORY_E;
                }
                sig = (byte*)XMALLOC(sigSz, crl->heap, DYNAMIC_TYPE_CRL_ENTRY);
                if (sig == NULL) {
                    XFREE(tbs, crl->heap, DYNAMIC_TYPE_CRL_ENTRY);
                    wc_UnLockMutex(&crl->crlLock);
                    return MEMORY_E;
                }

                XMEMCPY(tbs, crle->toBeSigned, tbsSz);
                XMEMCPY(sig, crle->signature, sigSz);
            #ifndef NO_SKID
                XMEMCPY(extAuthKeyId, crle->extAuthKeyId,
                                                          sizeof(extAuthKeyId));
            #endif
                XMEMCPY(issuerHash, crle->issuerHash, sizeof(issuerHash));

                wc_UnLockMutex(&crl->crlLock);

            #ifndef NO_SKID
                if (crle->extAuthKeyIdSet)
                    ca = GetCA(crl->cm, extAuthKeyId);
                if (ca == NULL)
                    ca = GetCAByName(crl->cm, issuerHash);
            #else /* NO_SKID */
                ca = GetCA(crl->cm, issuerHash);
            #endif /* NO_SKID */
                if (ca == NULL) {
                    XFREE(sig, crl->heap, DYNAMIC_TYPE_CRL_ENTRY);
                    XFREE(tbs, crl->heap, DYNAMIC_TYPE_CRL_ENTRY);
                    WOLFSSL_MSG("Did NOT find CRL issuer CA");
                    return ASN_CRL_NO_SIGNER_E;
                }

                ret = VerifyCRL_Signature(&sigCtx, tbs, tbsSz, sig, sigSz,
                                          sigOID, ca, crl->heap);

                XFREE(sig, crl->heap, DYNAMIC_TYPE_CRL_ENTRY);
                XFREE(tbs, crl->heap, DYNAMIC_TYPE_CRL_ENTRY);

                if (wc_LockMutex(&crl->crlLock) != 0) {
                    WOLFSSL_MSG("wc_LockMutex failed");
                    return BAD_MUTEX_E;
                }

                crle = crl->crlList;
                while (crle) {
                    if (XMEMCMP(crle->issuerHash, cert->issuerHash,
                        CRL_DIGEST_SIZE) == 0) {

                        if (ret == 0)
                            crle->verified = 1;
                        else
                            crle->verified = ret;

                        XFREE(crle->toBeSigned, crl->heap,
                                                        DYNAMIC_TYPE_CRL_ENTRY);
                        crle->toBeSigned = NULL;
                        XFREE(crle->signature, crl->heap,
                                                        DYNAMIC_TYPE_CRL_ENTRY);
                        crle->signature = NULL;
                        break;
                    }
                    crle = crle->next;
                }
                if (crle == NULL || crle->verified < 0)
                    break;
            }
            else if (crle->verified < 0) {
                WOLFSSL_MSG("Cannot use CRL as it didn't verify");
                ret = crle->verified;
                break;
            }

            WOLFSSL_MSG("Checking next date validity");

        #ifdef WOLFSSL_NO_CRL_NEXT_DATE
            if (crle->nextDateFormat != ASN_OTHER_TYPE)
        #endif
            {
            #ifndef NO_ASN_TIME
                if (!XVALIDATE_DATE(crle->nextDate,crle->nextDateFormat, AFTER)) {
                    WOLFSSL_MSG("CRL next date is no longer valid");
                    ret = ASN_AFTER_DATE_E;
                }
            #endif
            }
            if (ret == 0) {
                foundEntry = 1;
            }
            break;
        }
        crle = crle->next;
    }

    if (foundEntry) {
        ret = FindRevokedSerial(cert, crle->certs, crle->totalCerts);
    }

    wc_UnLockMutex(&crl->crlLock);

    *pFoundEntry = foundEntry;

    return ret;
}

/* Is the cert ok with CRL, return 0 on success */
int CheckCertCRL(WOLFSSL_CRL* crl, DecodedCert* cert)
{
    int        foundEntry = 0;
    int        ret = 0;

    WOLFSSL_ENTER("CheckCertCRL");

#ifdef WOLFSSL_CRL_ALLOW_MISSING_CDP
    /* Skip CRL verification in case no CDP in peer cert */
    if (!cert->extCrlInfo) {
        return ret;
    }
#endif

    ret = CheckCertCRLList(crl, cert, &foundEntry);

#ifdef HAVE_CRL_IO
    if (foundEntry == 0) {
        /* perform embedded lookup */
        if (crl->crlIOCb) {
            ret = crl->crlIOCb(crl, (const char*)cert->extCrlInfo,
                                                        cert->extCrlInfoSz);
            if (ret == WOLFSSL_CBIO_ERR_WANT_READ) {
                ret = OCSP_WANT_READ;
            }
            else if (ret >= 0) {
                /* try again */
                ret = CheckCertCRLList(crl, cert, &foundEntry);
            }
        }
    }
#endif

#if defined(OPENSSL_ALL) && defined(WOLFSSL_CERT_GEN) && \
    (defined(WOLFSSL_CERT_REQ) || defined(WOLFSSL_CERT_EXT)) && \
    !defined(NO_FILESYSTEM) && !defined(NO_WOLFSSL_DIR)
    /* if not find entry in the CRL list, it looks at the folder that sets  */
    /* by LOOKUP_ctrl because user would want to use hash_dir.              */
    /* Loading <issuer-hash>.rN form CRL file if find at the folder,        */
    /* and try again checking Cert in the CRL list.                         */
    /* When not set the folder or not use hash_dir, do nothing.             */
    if ((foundEntry == 0) && (ret != OCSP_WANT_READ)) {
        if (crl->cm->x509_store_p != NULL) {
            ret = LoadCertByIssuer(crl->cm->x509_store_p,
                          (WOLFSSL_X509_NAME*)cert->issuerName, X509_LU_CRL);
            if (ret == WOLFSSL_SUCCESS) {
                /* try again */
                ret = CheckCertCRLList(crl, cert, &foundEntry);
            }
        }
    }
#endif
    if (foundEntry == 0) {
        WOLFSSL_MSG("Couldn't find CRL for status check");
        if (ret != CRL_CERT_DATE_ERR) {
            ret = CRL_MISSING;
        }

        if (crl->cm->cbMissingCRL) {
            char url[256];

            WOLFSSL_MSG("Issuing missing CRL callback");
            url[0] = '\0';
            if (cert->extCrlInfo) {
                if (cert->extCrlInfoSz < (int)sizeof(url) -1 ) {
                    XMEMCPY(url, cert->extCrlInfo, cert->extCrlInfoSz);
                    url[cert->extCrlInfoSz] = '\0';
                }
                else  {
                    WOLFSSL_MSG("CRL url too long");
                }
            }

            crl->cm->cbMissingCRL(url);
        }
    }

    return ret;
}


/* Add Decoded CRL, 0 on success */
static int AddCRL(WOLFSSL_CRL* crl, DecodedCRL* dcrl, const byte* buff,
                  int verified)
{
    CRL_Entry* crle = NULL;

    WOLFSSL_ENTER("AddCRL");

    if (crl == NULL)
        return -1;

    crle = crl->currentEntry;

    if (crle == NULL) {
        crle = (CRL_Entry*)XMALLOC(sizeof(CRL_Entry), crl->heap,
                                   DYNAMIC_TYPE_CRL_ENTRY);
        if (crle == NULL) {
            WOLFSSL_MSG("alloc CRL Entry failed");
            return MEMORY_E;
        }
    }

    if (InitCRL_Entry(crle, dcrl, buff, verified, crl->heap) < 0) {
        WOLFSSL_MSG("Init CRL Entry failed");
        FreeCRL_Entry(crle, crl->heap);
        if (crle != crl->currentEntry) {
            XFREE(crle, crl->heap, DYNAMIC_TYPE_CRL_ENTRY);
        }
        return -1;
    }

    if (wc_LockMutex(&crl->crlLock) != 0) {
        WOLFSSL_MSG("wc_LockMutex failed");
        FreeCRL_Entry(crle, crl->heap);
        if (crle != crl->currentEntry) {
            XFREE(crle, crl->heap, DYNAMIC_TYPE_CRL_ENTRY);
        }
        return BAD_MUTEX_E;
    }

    crle->next = crl->crlList;
    crl->crlList = crle;
    wc_UnLockMutex(&crl->crlLock);
    /* Avoid heap-use-after-free after crl->crlList is released */
    crl->currentEntry = NULL;

    return 0;
}


/* Load CRL File of type, WOLFSSL_SUCCESS on ok */
int BufferLoadCRL(WOLFSSL_CRL* crl, const byte* buff, long sz, int type,
                  int verify)
{
    int          ret = WOLFSSL_SUCCESS;
    const byte*  myBuffer = buff;    /* if DER ok, otherwise switch */
    DerBuffer*   der = NULL;
#ifdef WOLFSSL_SMALL_STACK
    DecodedCRL*  dcrl;
#else
    DecodedCRL   dcrl[1];
#endif

    WOLFSSL_ENTER("BufferLoadCRL");

    if (crl == NULL || buff == NULL || sz == 0)
        return BAD_FUNC_ARG;

    if (type == WOLFSSL_FILETYPE_PEM) {
    #ifdef WOLFSSL_PEM_TO_DER
        ret = PemToDer(buff, sz, CRL_TYPE, &der, NULL, NULL, NULL);
        if (ret == 0) {
            myBuffer = der->buffer;
            sz = der->length;
        }
        else {
            WOLFSSL_MSG("Pem to Der failed");
            FreeDer(&der);
            return -1;
        }
    #else
        ret = NOT_COMPILED_IN;
    #endif
    }

#ifdef WOLFSSL_SMALL_STACK
    dcrl = (DecodedCRL*)XMALLOC(sizeof(DecodedCRL), NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (dcrl == NULL) {
        FreeDer(&der);
        return MEMORY_E;
    }
#endif

    crl->currentEntry = (CRL_Entry*)XMALLOC(sizeof(CRL_Entry), crl->heap,
                                        DYNAMIC_TYPE_CRL_ENTRY);
    if (crl->currentEntry == NULL) {
        WOLFSSL_MSG("alloc CRL Entry failed");
    #ifdef WOLFSSL_SMALL_STACK
        XFREE(dcrl, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    #endif
        FreeDer(&der);
        return MEMORY_E;
    }
    XMEMSET(crl->currentEntry, 0, sizeof(CRL_Entry));

    InitDecodedCRL(dcrl, crl->heap);
    ret = ParseCRL(crl->currentEntry->certs, dcrl, myBuffer, (word32)sz,
                   verify, crl->cm);
    if (ret != 0 && !(ret == ASN_CRL_NO_SIGNER_E && verify == NO_VERIFY)) {
        WOLFSSL_MSG("ParseCRL error");
        XFREE(crl->currentEntry, crl->heap, DYNAMIC_TYPE_CRL_ENTRY);
        crl->currentEntry = NULL;
    }
    else {
        ret = AddCRL(crl, dcrl, myBuffer, ret != ASN_CRL_NO_SIGNER_E);
        if (ret != 0) {
            WOLFSSL_MSG("AddCRL error");
        }
    }

    FreeDecodedCRL(dcrl);

#ifdef WOLFSSL_SMALL_STACK
    XFREE(dcrl, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    FreeDer(&der);

    return ret ? ret : WOLFSSL_SUCCESS; /* convert 0 to WOLFSSL_SUCCESS */
}

#if defined(OPENSSL_EXTRA) && defined(HAVE_CRL)
/* helper function to create a new dynamic WOLFSSL_X509_CRL structure */
static WOLFSSL_X509_CRL* wolfSSL_X509_crl_new(WOLFSSL_CERT_MANAGER* cm)
{
    WOLFSSL_X509_CRL* ret;

    ret = (WOLFSSL_X509_CRL*)XMALLOC(sizeof(WOLFSSL_X509_CRL), cm->heap,
                DYNAMIC_TYPE_CRL);
    if (ret != NULL) {
        if (InitCRL(ret, cm) < 0) {
            WOLFSSL_MSG("Unable to initialize new CRL structure");
            XFREE(ret, cm->heap, DYNAMIC_TYPE_CRL);
            ret = NULL;
        }
    }
    return ret;
}

#ifndef CRL_STATIC_REVOKED_LIST
/* returns head of copied list that was alloc'd */
static RevokedCert *DupRevokedCertList(RevokedCert* in, void* heap)
{
    RevokedCert* head = NULL;
    RevokedCert* current = in;
    RevokedCert* prev = NULL;
    while (current) {
        RevokedCert* tmp = (RevokedCert*)XMALLOC(sizeof(RevokedCert), heap,
                DYNAMIC_TYPE_REVOKED);
        if (tmp != NULL) {
            XMEMCPY(tmp->serialNumber, current->serialNumber,
                    EXTERNAL_SERIAL_SIZE);
            tmp->serialSz = current->serialSz;
            XMEMCPY(tmp->revDate, current->revDate,
                    MAX_DATE_SIZE);
            tmp->revDateFormat = current->revDateFormat;
            tmp->next = NULL;
            if (prev != NULL)
                prev->next = tmp;
            if (head == NULL)
                head = tmp;
            prev = tmp;
        }
        else {
            WOLFSSL_MSG("Failed to allocate new RevokedCert structure");
            /* free up any existing list */
            while (head != NULL) {
                current = head;
                head = head->next;
                XFREE(current, heap, DYNAMIC_TYPE_REVOKED);
            }
            return NULL;
        }
        current = current->next;
    }

    (void)heap;
    return head;
}

#endif /* CRL_STATIC_REVOKED_LIST */
/* returns a deep copy of ent on success and null on fail */
static CRL_Entry* DupCRL_Entry(const CRL_Entry* ent, void* heap)
{
    CRL_Entry *dupl;
#ifdef CRL_STATIC_REVOKED_LIST
    if (ent->totalCerts > CRL_MAX_REVOKED_CERTS) {
        return NULL;
    }
#endif
    dupl = (CRL_Entry*)XMALLOC(sizeof(CRL_Entry), heap, DYNAMIC_TYPE_CRL_ENTRY);
    if (dupl == NULL) {
        WOLFSSL_MSG("alloc CRL Entry failed");
        return NULL;
    }
    XMEMSET(dupl, 0, sizeof(CRL_Entry));

    XMEMCPY(dupl->issuerHash, ent->issuerHash, CRL_DIGEST_SIZE);
    XMEMCPY(dupl->lastDate, ent->lastDate, MAX_DATE_SIZE);
    XMEMCPY(dupl->nextDate, ent->nextDate, MAX_DATE_SIZE);
    dupl->lastDateFormat = ent->lastDateFormat;
    dupl->nextDateFormat = ent->nextDateFormat;

#if defined(OPENSSL_EXTRA)
    dupl->lastDateAsn1.length = MAX_DATE_SIZE;
    XMEMCPY (dupl->lastDateAsn1.data, dupl->lastDate,
             dupl->lastDateAsn1.length);
    dupl->lastDateAsn1.type = dupl->lastDateFormat;
    dupl->nextDateAsn1.length = MAX_DATE_SIZE;
    XMEMCPY (dupl->nextDateAsn1.data, dupl->nextDate,
             dupl->nextDateAsn1.length);
    dupl->nextDateAsn1.type = dupl->nextDateFormat;
#endif

#ifdef CRL_STATIC_REVOKED_LIST
    XMEMCPY(dupl->certs, ent->certs, ent->totalCerts*sizeof(RevokedCert));
#else
    dupl->certs = DupRevokedCertList(ent->certs, heap);
#endif
    dupl->totalCerts = ent->totalCerts;
    dupl->verified = ent->verified;

    if (!ent->verified) {
        dupl->tbsSz = ent->tbsSz;
        dupl->signatureSz = ent->signatureSz;
        dupl->signatureOID = ent->signatureOID;
        dupl->toBeSigned = (byte*)XMALLOC(dupl->tbsSz, heap,
                                          DYNAMIC_TYPE_CRL_ENTRY);
        if (dupl->toBeSigned == NULL) {
            FreeCRL_Entry(dupl, heap);
            XFREE(dupl, heap, DYNAMIC_TYPE_CRL_ENTRY);
            return NULL;
        }

        dupl->signature = (byte*)XMALLOC(dupl->signatureSz, heap,
                                         DYNAMIC_TYPE_CRL_ENTRY);
        if (dupl->signature == NULL) {
            FreeCRL_Entry(dupl, heap);
            XFREE(dupl, heap, DYNAMIC_TYPE_CRL_ENTRY);
            return NULL;
        }
        XMEMCPY(dupl->toBeSigned, ent->toBeSigned, dupl->tbsSz);
        XMEMCPY(dupl->signature, ent->signature, dupl->signatureSz);
    #ifndef NO_SKID
        dupl->extAuthKeyIdSet = ent->extAuthKeyIdSet;
        if (dupl->extAuthKeyIdSet)
            XMEMCPY(dupl->extAuthKeyId, ent->extAuthKeyId, KEYID_SIZE);
    #endif
    }
    else {
        dupl->toBeSigned = NULL;
        dupl->tbsSz = 0;
        dupl->signature = NULL;
        dupl->signatureSz = 0;
    }

    return dupl;
}


/* returns the head of a deep copy of the list on success and null on fail */
static CRL_Entry* DupCRL_list(CRL_Entry* crl, void* heap)
{
    CRL_Entry* current;
    CRL_Entry* head = NULL;
    CRL_Entry* prev = NULL;

    current = crl;
    while (current != NULL) {
        CRL_Entry* tmp = DupCRL_Entry(current, heap);
        if (tmp != NULL) {
            tmp->next = NULL;
            if (head == NULL)
                head = tmp;
            if (prev != NULL)
                prev->next = tmp;
            prev = tmp;
        }
        else {
            WOLFSSL_MSG("Failed to allocate new CRL_Entry structure");
            /* free up any existing list */
            while (head != NULL) {
                current = head;
                head = head->next;
                FreeCRL_Entry(current, heap);
                XFREE(current, heap, DYNAMIC_TYPE_CRL_ENTRY);
            }

            return NULL;
        }
        current = current->next;
    }
    return head;
}


/* Duplicates everything except the parent cm pointed to.
 * Expects that Init has already been done to 'dupl'
 * return 0 on success */
static int DupX509_CRL(WOLFSSL_X509_CRL *dupl, const WOLFSSL_X509_CRL* crl)
{
    if (dupl == NULL || crl == NULL) {
        return BAD_FUNC_ARG;
    }

    if (crl->monitors[0].path) {
        int pathSz = (int)XSTRLEN(crl->monitors[0].path) + 1;
        dupl->monitors[0].path = (char*)XMALLOC(pathSz, dupl->heap,
                DYNAMIC_TYPE_CRL_MONITOR);
        if (dupl->monitors[0].path != NULL) {
            XSTRNCPY(dupl->monitors[0].path, crl->monitors[0].path, pathSz);
        }
        else {
            return MEMORY_E;
        }
    }

    if (crl->monitors[1].path) {
        int pathSz = (int)XSTRLEN(crl->monitors[1].path) + 1;
        dupl->monitors[1].path = (char*)XMALLOC(pathSz, dupl->heap,
                DYNAMIC_TYPE_CRL_MONITOR);
        if (dupl->monitors[1].path != NULL) {
            XSTRNCPY(dupl->monitors[1].path, crl->monitors[1].path, pathSz);
        }
        else {
            if (dupl->monitors[0].path != NULL) {
                XFREE(dupl->monitors[0].path, dupl->heap,
                        DYNAMIC_TYPE_CRL_MONITOR);
            }
            return MEMORY_E;
        }
    }

    dupl->crlList = DupCRL_list(crl->crlList, dupl->heap);
#ifdef HAVE_CRL_IO
    dupl->crlIOCb = crl->crlIOCb;
#endif

    return 0;
}

/* returns WOLFSSL_SUCCESS on success. Does not take ownership of newcrl */
int wolfSSL_X509_STORE_add_crl(WOLFSSL_X509_STORE *store, WOLFSSL_X509_CRL *newcrl)
{
    CRL_Entry   *crle;
    WOLFSSL_X509_CRL *crl;

    WOLFSSL_ENTER("wolfSSL_X509_STORE_add_crl");
    if (store == NULL || newcrl == NULL || store->cm == NULL)
        return BAD_FUNC_ARG;

    if (store->cm->crl == NULL) {
        crl = wolfSSL_X509_crl_new(store->cm);
        if (crl == NULL) {
            return WOLFSSL_FAILURE;
        }
        if (DupX509_CRL(crl, newcrl) != 0) {
            if (crl != NULL)
                FreeCRL(crl, 1);
            return WOLFSSL_FAILURE;
        }
        store->crl = store->cm->crl = crl;
        if (wolfSSL_CertManagerEnableCRL(store->cm, WOLFSSL_CRL_CHECKALL)
                != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("wolfSSL_CertManagerEnableCRL error");
            return WOLFSSL_FAILURE;
        }
        return WOLFSSL_SUCCESS;
    }

    /* find tail of current list and add new list */
    crl  = store->cm->crl;
    crle = crl->crlList;
    if (newcrl->crlList != NULL) {
        CRL_Entry *tail = crle;
        CRL_Entry *toAdd;

        if (wc_LockMutex(&crl->crlLock) != 0)
        {
            WOLFSSL_MSG("wc_LockMutex failed");
            return BAD_MUTEX_E;
        }

        toAdd = DupCRL_list(newcrl->crlList, crl->heap);
        if (tail == NULL) {
            crl->crlList = toAdd;
        }
        else {
            while (tail->next != NULL) tail = tail->next;
            tail->next = toAdd;
        }
        wc_UnLockMutex(&crl->crlLock);
    }

    if (wolfSSL_CertManagerEnableCRL(store->cm, WOLFSSL_CRL_CHECKALL)
            != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("wolfSSL_CertManagerEnableCRL error");
        return WOLFSSL_FAILURE;
    }

    WOLFSSL_LEAVE("wolfSSL_X509_STORE_add_crl", WOLFSSL_SUCCESS);

    return WOLFSSL_SUCCESS;
}
#endif

#ifdef HAVE_CRL_MONITOR


/* Signal Monitor thread is setup, save status to setup flag, 0 on success */
static int SignalSetup(WOLFSSL_CRL* crl, int status)
{
    int ret;

    /* signal to calling thread we're setup */
    if (wc_LockMutex(&crl->crlLock) != 0) {
        WOLFSSL_MSG("wc_LockMutex crlLock failed");
        return BAD_MUTEX_E;
    }
    crl->setup = status;
    if (wc_UnLockMutex(&crl->crlLock) != 0) {
        WOLFSSL_MSG("wc_UnLockMutex crlLock failed");
        return BAD_MUTEX_E;
    }

    ret = wolfSSL_CondSignal(&crl->cond);
    if (ret != 0)
        return BAD_COND_E;

    return 0;
}


/* read in new CRL entries and save new list */
static int SwapLists(WOLFSSL_CRL* crl)
{
    int        ret;
    CRL_Entry* newList;
#ifdef WOLFSSL_SMALL_STACK
    WOLFSSL_CRL* tmp;
#else
    WOLFSSL_CRL tmp[1];
#endif

#ifdef WOLFSSL_SMALL_STACK
    tmp = (WOLFSSL_CRL*)XMALLOC(sizeof(WOLFSSL_CRL), NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (tmp == NULL)
        return MEMORY_E;
#endif

    if (InitCRL(tmp, crl->cm) < 0) {
        WOLFSSL_MSG("Init tmp CRL failed");
#ifdef WOLFSSL_SMALL_STACK
        XFREE(tmp, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return -1;
    }

    if (crl->monitors[0].path) {
        ret = LoadCRL(tmp, crl->monitors[0].path, WOLFSSL_FILETYPE_PEM, 0);
        if (ret != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("PEM LoadCRL on dir change failed");
            FreeCRL(tmp, 0);
#ifdef WOLFSSL_SMALL_STACK
            XFREE(tmp, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
            return -1;
        }
    }

    if (crl->monitors[1].path) {
        ret = LoadCRL(tmp, crl->monitors[1].path, WOLFSSL_FILETYPE_ASN1, 0);
        if (ret != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("DER LoadCRL on dir change failed");
            FreeCRL(tmp, 0);
#ifdef WOLFSSL_SMALL_STACK
            XFREE(tmp, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
            return -1;
        }
    }

    if (wc_LockMutex(&crl->crlLock) != 0) {
        WOLFSSL_MSG("wc_LockMutex failed");
        FreeCRL(tmp, 0);
#ifdef WOLFSSL_SMALL_STACK
        XFREE(tmp, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return -1;
    }

    newList = tmp->crlList;

    /* swap lists */
    tmp->crlList  = crl->crlList;
    crl->crlList = newList;

    wc_UnLockMutex(&crl->crlLock);

    FreeCRL(tmp, 0);

#ifdef WOLFSSL_SMALL_STACK
    XFREE(tmp, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return 0;
}


#if (defined(__MACH__) || defined(__FreeBSD__))

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>

#ifdef __MACH__
    #define XEVENT_MODE O_EVTONLY
#elif defined(__FreeBSD__)
    #define XEVENT_MODE O_RDONLY
#endif


/* we need a unique kqueue user filter fd for crl in case user is doing custom
 * events too */
#ifndef CRL_CUSTOM_FD
    #define CRL_CUSTOM_FD 123456
#endif


/* shutdown monitor thread, 0 on success */
static int StopMonitor(wolfSSL_CRL_mfd_t mfd)
{
    struct kevent change;

    /* trigger custom shutdown */
    EV_SET(&change, CRL_CUSTOM_FD, EVFILT_USER, 0, NOTE_TRIGGER, 0, NULL);
    if (kevent(mfd, &change, 1, NULL, 0, NULL) < 0) {
        WOLFSSL_MSG("kevent trigger customer event failed");
        return -1;
    }

    return 0;
}


/* OS X  monitoring */
static THREAD_RETURN WOLFSSL_THREAD DoMonitor(void* arg)
{
    int fPEM, fDER;
    struct kevent change;

    WOLFSSL_CRL* crl = (WOLFSSL_CRL*)arg;

    WOLFSSL_ENTER("DoMonitor");

    crl->mfd = kqueue();
    if (crl->mfd == -1) {
        WOLFSSL_MSG("kqueue failed");
        SignalSetup(crl, MONITOR_SETUP_E);
        return NULL;
    }

    /* listen for custom shutdown event */
    EV_SET(&change, CRL_CUSTOM_FD, EVFILT_USER, EV_ADD, 0, 0, NULL);
    if (kevent(crl->mfd, &change, 1, NULL, 0, NULL) < 0) {
        WOLFSSL_MSG("kevent monitor customer event failed");
        SignalSetup(crl, MONITOR_SETUP_E);
        (void)close(crl->mfd);
        return NULL;
    }

    fPEM = -1;
    fDER = -1;

    if (crl->monitors[0].path) {
        fPEM = open(crl->monitors[0].path, XEVENT_MODE);
        if (fPEM == -1) {
            WOLFSSL_MSG("PEM event dir open failed");
            SignalSetup(crl, MONITOR_SETUP_E);
            (void)close(crl->mfd);
            return NULL;
        }
    }

    if (crl->monitors[1].path) {
        fDER = open(crl->monitors[1].path, XEVENT_MODE);
        if (fDER == -1) {
            WOLFSSL_MSG("DER event dir open failed");
            if (fPEM != -1)
                (void)close(fPEM);
            (void)close(crl->mfd);
            SignalSetup(crl, MONITOR_SETUP_E);
            return NULL;
        }
    }

    if (fPEM != -1)
        EV_SET(&change, fPEM, EVFILT_VNODE, EV_ADD | EV_ENABLE | EV_CLEAR,
                NOTE_DELETE | NOTE_EXTEND | NOTE_WRITE | NOTE_ATTRIB, 0, 0);

    if (fDER != -1)
        EV_SET(&change, fDER, EVFILT_VNODE, EV_ADD | EV_ENABLE | EV_CLEAR,
                NOTE_DELETE | NOTE_EXTEND | NOTE_WRITE | NOTE_ATTRIB, 0, 0);

    /* signal to calling thread we're setup */
    if (SignalSetup(crl, 1) != 0) {
        if (fPEM != -1)
            (void)close(fPEM);
        if (fDER != -1)
            (void)close(fDER);
        (void)close(crl->mfd);
        return NULL;
    }

    for (;;) {
        struct kevent event;
        int           numEvents = kevent(crl->mfd, &change, 1, &event, 1, NULL);

        WOLFSSL_MSG("Got kevent");

        if (numEvents == -1) {
            WOLFSSL_MSG("kevent problem, continue");
            continue;
        }

        if (event.filter == EVFILT_USER) {
            WOLFSSL_MSG("Got user shutdown event, breaking out");
            break;
        }

        if (SwapLists(crl) < 0) {
            WOLFSSL_MSG("SwapLists problem, continue");
        }
    }

    if (fPEM != -1)
        (void)close(fPEM);
    if (fDER != -1)
        (void)close(fDER);

    (void)close(crl->mfd);

    return NULL;
}


#elif defined(__linux__)

#include <sys/types.h>
#include <sys/inotify.h>
#include <sys/eventfd.h>
#include <unistd.h>


#ifndef max
    static WC_INLINE int max(int a, int b)
    {
        return a > b ? a : b;
    }
#endif /* max */


/* shutdown monitor thread, 0 on success */
static int StopMonitor(wolfSSL_CRL_mfd_t mfd)
{
    word64 w64 = 1;

    /* write to our custom event */
    if (write(mfd, &w64, sizeof(w64)) < 0) {
        WOLFSSL_MSG("StopMonitor write failed");
        return -1;
    }

    return 0;
}


/* linux monitoring */
static THREAD_RETURN WOLFSSL_THREAD DoMonitor(void* arg)
{
    int         notifyFd;
    int         wd  = -1;
    WOLFSSL_CRL* crl = (WOLFSSL_CRL*)arg;
#ifdef WOLFSSL_SMALL_STACK
    char*       buff;
#else
    char        buff[8192];
#endif

    WOLFSSL_ENTER("DoMonitor");

    crl->mfd = eventfd(0, 0);  /* our custom shutdown event */
    if (crl->mfd < 0) {
        WOLFSSL_MSG("eventfd failed");
        SignalSetup(crl, MONITOR_SETUP_E);
        return NULL;
    }

    notifyFd = inotify_init();
    if (notifyFd < 0) {
        WOLFSSL_MSG("inotify failed");
        (void)close(crl->mfd);
        SignalSetup(crl, MONITOR_SETUP_E);
        return NULL;
    }

    if (crl->monitors[0].path) {
        wd = inotify_add_watch(notifyFd, crl->monitors[0].path, IN_CLOSE_WRITE |
                                                                IN_DELETE);
        if (wd < 0) {
            WOLFSSL_MSG("PEM notify add watch failed");
            (void)close(crl->mfd);
            (void)close(notifyFd);
            SignalSetup(crl, MONITOR_SETUP_E);
            return NULL;
        }
    }

    if (crl->monitors[1].path) {
        wd = inotify_add_watch(notifyFd, crl->monitors[1].path, IN_CLOSE_WRITE |
                                                                IN_DELETE);
        if (wd < 0) {
            WOLFSSL_MSG("DER notify add watch failed");
            (void)close(crl->mfd);
            (void)close(notifyFd);
            SignalSetup(crl, MONITOR_SETUP_E);
            return NULL;
        }
    }

#ifdef WOLFSSL_SMALL_STACK
    buff = (char*)XMALLOC(8192, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (buff == NULL)
        return NULL;
#endif

    /* signal to calling thread we're setup */
    if (SignalSetup(crl, 1) != 0) {
        #ifdef WOLFSSL_SMALL_STACK
            XFREE(buff, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        #endif

        if (wd > 0) {
            if (inotify_rm_watch(notifyFd, wd) < 0)
                WOLFSSL_MSG("inotify_rm_watch #1 failed in DoMonitor");
        }
        (void)close(crl->mfd);
        (void)close(notifyFd);
        return NULL;
    }

    for (;;) {
        fd_set readfds;
        int    result;
        int    length;

        FD_ZERO(&readfds);
        FD_SET(notifyFd, &readfds);
        FD_SET(crl->mfd, &readfds);

        result = select(max(notifyFd, crl->mfd) + 1, &readfds, NULL, NULL,NULL);

        WOLFSSL_MSG("Got notify event");

        if (result < 0) {
            WOLFSSL_MSG("select problem, continue");
            continue;
        }

        if (FD_ISSET(crl->mfd, &readfds)) {
            word64 r64;
            int    rlen;

            WOLFSSL_MSG("got custom shutdown event, breaking out");

            /* read out the bytes written to the event to clean up */
            rlen = (int) read(crl->mfd, &r64, sizeof(r64));
            if (rlen < 0) {
                WOLFSSL_MSG("read custom event failure");
            }

            break;
        }

        length = (int) read(notifyFd, buff, 8192);
        if (length < 0) {
            WOLFSSL_MSG("notify read problem, continue");
            continue;
        }

        if (SwapLists(crl) < 0) {
            WOLFSSL_MSG("SwapLists problem, continue");
        }
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(buff, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    if (wd > 0) {
        if (inotify_rm_watch(notifyFd, wd) < 0)
            WOLFSSL_MSG("inotify_rm_watch #2 failed in DoMonitor");
    }
    (void)close(crl->mfd);
    (void)close(notifyFd);

    return NULL;
}

#elif defined(_MSC_VER)

/* shutdown monitor thread, 0 on success */
static int StopMonitor(wolfSSL_CRL_mfd_t mfd)
{
    if (SetEvent(mfd) == 0) {
        WOLFSSL_MSG("SetEvent custom event trigger failed");
        return -1;
    }
    return 0;
}

#ifdef DEBUG_WOLFSSL
#define SHOW_WINDOWS_ERROR() do {                               \
    LPVOID lpMsgBuf;                                            \
    DWORD dw = GetLastError();                                  \
    FormatMessageA(                                             \
        FORMAT_MESSAGE_ALLOCATE_BUFFER |                        \
        FORMAT_MESSAGE_FROM_SYSTEM |                            \
        FORMAT_MESSAGE_IGNORE_INSERTS,                          \
        NULL,                                                   \
        dw,                                                     \
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),              \
        (LPSTR) &lpMsgBuf,                                      \
        0, NULL );                                              \
    WOLFSSL_MSG_EX("DoMonitor failed with error %d: %s\n",      \
        dw, lpMsgBuf);                                          \
    LocalFree(lpMsgBuf);                                        \
} while(0)
#else
#define SHOW_WINDOWS_ERROR() WC_DO_NOTHING
#endif

#define DM_ERROR() do {                                         \
    SHOW_WINDOWS_ERROR();                                       \
    status = MONITOR_SETUP_E;                                   \
    goto cleanup;                                               \
} while(0)

/* windows monitoring
 * Tested initially by hand by running
 * .\server.exe -A certs/ca-cert.pem -i -x
 * and connecting to with
 * .\client.exe -C -c certs/server-cert.pem -k certs/server-key.pem
 * This connection succeeds by default. By deleting all files from certs/crl
 * except for crl.revoked we disallow the client to connect. Deleting files
 * is done while the server is running to show that the monitor reacts to
 * changes in the crl directory. */
static THREAD_RETURN WOLFSSL_THREAD DoMonitor(void* arg)
{
    WOLFSSL_CRL* crl = (WOLFSSL_CRL*)arg;
    int status = 0;
    HANDLE handles[WOLFSSL_CRL_MONITORS_LEN + 1];
    DWORD handlesLen = 0;
    int i;

    WOLFSSL_ENTER("DoMonitor");

    handles[0] = crl->mfd = CreateEventA(NULL, FALSE, FALSE, NULL);
    if (crl->mfd == NULL) {
        WOLFSSL_MSG("CreateEventA failed");
        DM_ERROR();
    }
    handlesLen++;

    for (i = 0; i < WOLFSSL_CRL_MONITORS_LEN; i++) {
        if (crl->monitors[i].path) {
            handles[handlesLen] = FindFirstChangeNotificationA(
                crl->monitors[i].path, TRUE,
                /* Watch for any changes that may affect what CRL's we load.
                 * This may trigger on the same file multiple times but this
                 * way we are certain that we have the most up to date and
                 * accurate set of CRL's. We don't expect this to trigger
                 * often enough for it to be a bottleneck. */
                FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_ATTRIBUTES |
                FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE |
                FILE_NOTIFY_CHANGE_SECURITY);
            if (handles[handlesLen] == INVALID_HANDLE_VALUE) {
                WOLFSSL_MSG("FindFirstChangeNotificationA failed");
                DM_ERROR();
            }
            handlesLen++;
        }
    }

    if (handlesLen == 1) {
        WOLFSSL_MSG("Nothing to watch. Only custom event handle set.");
        DM_ERROR();
    }

    if (SignalSetup(crl, 1) != 0) {
        WOLFSSL_MSG("Call to SignalSetup failed");
        DM_ERROR();
    }

    for (;;) {
        DWORD waitRet = WaitForMultipleObjects(handlesLen, handles, FALSE,
                                               INFINITE);
        WOLFSSL_MSG("Got notify event");

        if (waitRet >= WAIT_OBJECT_0 && waitRet < WAIT_OBJECT_0 + handlesLen) {
            if (waitRet == WAIT_OBJECT_0) {
                WOLFSSL_MSG("got custom shutdown event, breaking out");
                break;
            }
            else if (SwapLists(crl) < 0) {
                WOLFSSL_MSG("SwapLists problem, continue");
            }
        }
        else {
            WOLFSSL_MSG("Unexpected WaitForMultipleObjects return. Continue.");
        }

        for (i = 1; i < (int)handlesLen; i++) {
            if (FindNextChangeNotification(handles[i]) == 0) {
                WOLFSSL_MSG("FindNextChangeNotification failed");
                DM_ERROR();
            }
        }
    }

cleanup:
    if (status != 0)
        SignalSetup(crl, status);
    for (i = 0; i < (int)handlesLen; i++) {
        BOOL closeRet;
        if (i == 0) /* First handle is our custom event */
            closeRet = CloseHandle(handles[i]);
        else
            closeRet = FindCloseChangeNotification(handles[i]);
        if (closeRet == 0) {
            WOLFSSL_MSG("Failed to close handle");
        }
    }
    crl->mfd = INVALID_HANDLE_VALUE;
    return 0;
}

#endif /* MACH or linux or windows */


/* Start Monitoring the CRL path(s) in a thread */
static int StartMonitorCRL(WOLFSSL_CRL* crl)
{
    int ret = WOLFSSL_SUCCESS;

    WOLFSSL_ENTER("StartMonitorCRL");

    if (crl == NULL)
        return BAD_FUNC_ARG;

    if (crl->tid != INVALID_THREAD_VAL) {
        WOLFSSL_MSG("Monitor thread already running");
        return ret;  /* that's ok, someone already started */
    }

    if (wolfSSL_NewThread(&crl->tid, DoMonitor, crl) != 0) {
        WOLFSSL_MSG("Thread creation error");
        return THREAD_CREATE_E;
    }

    /* wait for setup to complete */
    if (wc_LockMutex(&crl->crlLock) != 0) {
        WOLFSSL_MSG("wc_LockMutex crlLock failed");
        return BAD_MUTEX_E;
    }
    while (crl->setup == 0) {
        int condRet;
        if (wc_UnLockMutex(&crl->crlLock) != 0) {
            WOLFSSL_MSG("wc_UnLockMutex crlLock failed");
            return BAD_MUTEX_E;
        }
        condRet = wolfSSL_CondWait(&crl->cond);
        if (wc_LockMutex(&crl->crlLock) != 0) {
            WOLFSSL_MSG("wc_LockMutex crlLock failed");
            return BAD_MUTEX_E;
        }
        if (condRet != 0) {
            ret = BAD_COND_E;
            break;
        }
    }
    if (ret >= 0 && crl->setup < 0)
        ret = crl->setup;  /* store setup error */

    if (ret < 0) {
        WOLFSSL_MSG("DoMonitor setup failure");
        crl->tid = INVALID_THREAD_VAL;  /* thread already done */
    }
    if (wc_UnLockMutex(&crl->crlLock) != 0) {
        WOLFSSL_MSG("wc_UnLockMutex crlLock failed");
        return BAD_MUTEX_E;
    }

    return ret;
}

#endif  /* HAVE_CRL_MONITOR */

#if !defined(NO_FILESYSTEM) && !defined(NO_WOLFSSL_DIR)

/* Load CRL path files of type, WOLFSSL_SUCCESS on ok */
int LoadCRL(WOLFSSL_CRL* crl, const char* path, int type, int monitor)
{
    int         ret = WOLFSSL_SUCCESS;
    char*       name = NULL;
#ifdef WOLFSSL_SMALL_STACK
    ReadDirCtx* readCtx = NULL;
#else
    ReadDirCtx  readCtx[1];
#endif

    WOLFSSL_ENTER("LoadCRL");
    if (crl == NULL)
        return BAD_FUNC_ARG;

#ifdef WOLFSSL_SMALL_STACK
    readCtx = (ReadDirCtx*)XMALLOC(sizeof(ReadDirCtx), crl->heap,
                                                       DYNAMIC_TYPE_TMP_BUFFER);
    if (readCtx == NULL)
        return MEMORY_E;
#endif

    /* try to load each regular file in path */
    ret = wc_ReadDirFirst(readCtx, path, &name);
    while (ret == 0 && name) {
        int skip = 0;
        if (type == WOLFSSL_FILETYPE_PEM) {
            if (XSTRSTR(name, ".pem") == NULL) {
                WOLFSSL_MSG("not .pem file, skipping");
                skip = 1;
            }
        }
        else {
            if (XSTRSTR(name, ".der") == NULL &&
                XSTRSTR(name, ".crl") == NULL)
            {
                WOLFSSL_MSG("not .der or .crl file, skipping");
                skip = 1;
            }
        }

#ifndef CRL_REPORT_LOAD_ERRORS
        if (!skip && ProcessFile(NULL, name, type, CRL_TYPE, NULL, 0, crl,
                                 VERIFY) != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("CRL file load failed, continuing");
        }
#else
        if (!skip) {
            ret = ProcessFile(NULL, name, type, CRL_TYPE, NULL, 0, crl, VERIFY);
            if (ret != WOLFSSL_SUCCESS) {
                WOLFSSL_MSG("CRL file load failed");
                return ret;
            }
        }
#endif

        ret = wc_ReadDirNext(readCtx, path, &name);
    }
    wc_ReadDirClose(readCtx);

    /* load failures not reported, for backwards compat */
    ret = WOLFSSL_SUCCESS;

#ifdef WOLFSSL_SMALL_STACK
    XFREE(readCtx, crl->heap, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    if (monitor & WOLFSSL_CRL_MONITOR) {
#ifdef HAVE_CRL_MONITOR
        word32 pathLen;
        char* pathBuf;

        WOLFSSL_MSG("monitor path requested");

        pathLen = (word32)XSTRLEN(path);
        pathBuf = (char*)XMALLOC(pathLen+1, crl->heap, DYNAMIC_TYPE_CRL_MONITOR);
        if (pathBuf) {
            XMEMCPY(pathBuf, path, pathLen+1);

            if (type == WOLFSSL_FILETYPE_PEM) {
                /* free old path before setting a new one */
                if (crl->monitors[0].path) {
                    XFREE(crl->monitors[0].path, crl->heap,
                            DYNAMIC_TYPE_CRL_MONITOR);
                }
                crl->monitors[0].path = pathBuf;
                crl->monitors[0].type = WOLFSSL_FILETYPE_PEM;
            } else {
                /* free old path before setting a new one */
                if (crl->monitors[1].path) {
                    XFREE(crl->monitors[1].path, crl->heap,
                            DYNAMIC_TYPE_CRL_MONITOR);
                }
                crl->monitors[1].path = pathBuf;
                crl->monitors[1].type = WOLFSSL_FILETYPE_ASN1;
            }

            if (monitor & WOLFSSL_CRL_START_MON) {
                WOLFSSL_MSG("start monitoring requested");

                ret = StartMonitorCRL(crl);
            }
        }
        else {
            ret = MEMORY_E;
        }
#else
        WOLFSSL_MSG("CRL monitoring requested but not compiled in");
        ret = NOT_COMPILED_IN;
#endif
    }

    return ret;
}

#else
int LoadCRL(WOLFSSL_CRL* crl, const char* path, int type, int monitor)
{
    (void)crl;
    (void)path;
    (void)type;
    (void)monitor;

    /* stub for scenario where file system is not supported */
    return NOT_COMPILED_IN;
}
#endif /* !NO_FILESYSTEM && !NO_WOLFSSL_DIR */

#endif /* HAVE_CRL */
#endif /* !WOLFCRYPT_ONLY */
