/* dtls.c
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
 * WOLFSSL_DTLS_NO_HVR_ON_RESUME
 *     If defined, a DTLS server will not do a cookie exchange on successful
 *     client resumption: the resumption will be faster (one RTT less) and
 *     will consume less bandwidth (one ClientHello and one HelloVerifyRequest
 *     less). On the other hand, if a valid SessionID is collected, forged
 *     clientHello messages will consume resources on the server.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#ifndef WOLFCRYPT_ONLY

#include <wolfssl/error-ssl.h>
#include <wolfssl/internal.h>
#include <wolfssl/ssl.h>
#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

#define ERROR_OUT(err, eLabel) { ret = (err); goto eLabel; }

#ifdef WOLFSSL_DTLS

void DtlsResetState(WOLFSSL* ssl)
{
    /* Reset the state so that we can statelessly await the
     * ClientHello that contains the cookie. Don't gate on IsAtLeastTLSv1_3
     * to handle the edge case when the peer wants a lower version. */

    /* Reset DTLS window */
#ifdef WOLFSSL_DTLS13
    w64Zero(&ssl->dtls13Epochs[0].nextSeqNumber);
    w64Zero(&ssl->dtls13Epochs[0].nextPeerSeqNumber);
    XMEMSET(ssl->dtls13Epochs[0].window, 0,
        sizeof(ssl->dtls13Epochs[0].window));
    Dtls13FreeFsmResources(ssl);
#endif
    ssl->keys.dtls_expected_peer_handshake_number = 0;
    ssl->keys.dtls_handshake_number = 0;
    ssl->keys.dtls_sequence_number_hi = 0;
    ssl->keys.dtls_sequence_number_lo = 0;

    /* Reset states */
    ssl->options.serverState = NULL_STATE;
    ssl->options.clientState = NULL_STATE;
    ssl->options.connectState = CONNECT_BEGIN;
    ssl->options.acceptState = ACCEPT_BEGIN;
    ssl->options.handShakeState = NULL_STATE;
    ssl->msgsReceived.got_client_hello = 0;
    ssl->keys.dtls_handshake_number = 0;
    ssl->keys.dtls_expected_peer_handshake_number = 0;
    XMEMSET(ssl->keys.peerSeq, 0, sizeof(ssl->keys.peerSeq));
    ssl->options.tls = 0;
    ssl->options.tls1_1 = 0;
    ssl->options.tls1_3 = 0;
}

int DtlsIgnoreError(int err)
{
    /* Whitelist of errors not to ignore */
    switch (err) {
    case MEMORY_E:
    case MEMORY_ERROR:
    case ASYNC_INIT_E:
    case ASYNC_OP_E:
    case SOCKET_ERROR_E:
    case WANT_READ:
    case WANT_WRITE:
        return 0;
    default:
        return 1;
    }
}

void DtlsSetSeqNumForReply(WOLFSSL* ssl)
{
    /* We cover both DTLS 1.2 and 1.3 cases because we may be negotiating
     * protocols. */
    /* We should continue with the same sequence number as the
     * Client Hello. */
    ssl->keys.dtls_sequence_number_hi = ssl->keys.curSeq_hi;
    ssl->keys.dtls_sequence_number_lo = ssl->keys.curSeq_lo;
#ifdef WOLFSSL_DTLS13
    if (ssl->dtls13EncryptEpoch != NULL) {
        ssl->dtls13EncryptEpoch->nextSeqNumber =
                w64From32(ssl->keys.curSeq_hi, ssl->keys.curSeq_lo);
    }
#endif
    /* We should continue with the same handshake number as the
     * Client Hello. */
    ssl->keys.dtls_handshake_number =
            ssl->keys.dtls_peer_handshake_number;
}

#if !defined(NO_WOLFSSL_SERVER)

#if defined(NO_SHA) && defined(NO_SHA256)
#error "DTLS needs either SHA or SHA-256"
#endif /* NO_SHA && NO_SHA256 */

#if !defined(NO_SHA) && defined(NO_SHA256)
#define DTLS_COOKIE_TYPE WC_SHA
#define DTLS_COOKIE_SZ WC_SHA_DIGEST_SIZE
#endif /* !NO_SHA && NO_SHA256 */

#ifndef NO_SHA256
#define DTLS_COOKIE_TYPE WC_SHA256
#define DTLS_COOKIE_SZ WC_SHA256_DIGEST_SIZE
#endif /* !NO_SHA256 */

#if defined(WOLFSSL_DTLS13) && (defined(HAVE_SESSION_TICKET) || \
                                !defined(NO_PSK))
typedef struct PskInfo {
    byte cipherSuite0;
    byte cipherSuite;
    byte isValid:1;
} PskInfo;
#endif

typedef struct WolfSSL_ConstVector {
    word32 size;
    const byte* elements;
} WolfSSL_ConstVector;

typedef struct WolfSSL_CH {
    ProtocolVersion* pv;
    const byte* random;
    WolfSSL_ConstVector sessionId;
    WolfSSL_ConstVector cookie;
    WolfSSL_ConstVector cipherSuite;
    WolfSSL_ConstVector compression;
    WolfSSL_ConstVector extension;
    WolfSSL_ConstVector cookieExt;
    const byte* raw;
    word32 length;
    /* Store the DTLS 1.2 cookie since we can just compute it once in dtls.c */
    byte dtls12cookie[DTLS_COOKIE_SZ];
    byte dtls12cookieSet:1;
} WolfSSL_CH;

static int ReadVector8(const byte* input, WolfSSL_ConstVector* v)
{
    v->size = *input;
    v->elements = input + OPAQUE8_LEN;
    return v->size + OPAQUE8_LEN;
}

static int ReadVector16(const byte* input, WolfSSL_ConstVector* v)
{
    word16 size16;
    ato16(input, &size16);
    v->size = (word32)size16;
    v->elements = input + OPAQUE16_LEN;
    return v->size + OPAQUE16_LEN;
}

static int CreateDtls12Cookie(const WOLFSSL* ssl, const WolfSSL_CH* ch,
                              byte* cookie)
{
    int ret;
    Hmac cookieHmac;
    ret = wc_HmacInit(&cookieHmac, ssl->heap, ssl->devId);
    if (ret == 0) {
        ret = wc_HmacSetKey(&cookieHmac, DTLS_COOKIE_TYPE,
            ssl->buffers.dtlsCookieSecret.buffer,
            ssl->buffers.dtlsCookieSecret.length);
        if (ret == 0) {
            ret = wc_HmacUpdate(&cookieHmac,
                   (const byte*)ssl->buffers.dtlsCtx.peer.sa,
                                ssl->buffers.dtlsCtx.peer.sz);
        }
        if (ret == 0)
            ret = wc_HmacUpdate(&cookieHmac, (byte*)ch->pv, OPAQUE16_LEN);
        if (ret == 0)
            ret = wc_HmacUpdate(&cookieHmac, (byte*)ch->random, RAN_LEN);
        if (ret == 0) {
            ret = wc_HmacUpdate(&cookieHmac, (byte*)ch->sessionId.elements,
                    ch->sessionId.size);
        }
        if (ret == 0) {
            ret = wc_HmacUpdate(&cookieHmac, (byte*)ch->cipherSuite.elements,
                ch->cipherSuite.size);
        }
        if (ret == 0) {
            ret = wc_HmacUpdate(&cookieHmac, (byte*)ch->compression.elements,
                ch->compression.size);
        }
        if (ret == 0)
            ret = wc_HmacFinal(&cookieHmac, cookie);
        wc_HmacFree(&cookieHmac);
    }

    return ret;
}

static int CheckDtlsCookie(const WOLFSSL* ssl, WolfSSL_CH* ch,
                           byte isTls13, byte* cookieGood)
{
    int ret = 0;

    (void)isTls13;

    *cookieGood = 0;
#ifdef WOLFSSL_DTLS13
    if (isTls13) {
        word16  len;
        if (ch->cookieExt.size < OPAQUE16_LEN + 1)
            return BUFFER_E;
        ato16(ch->cookieExt.elements, &len);
        if (ch->cookieExt.size - OPAQUE16_LEN != len)
            return BUFFER_E;
        ret = TlsCheckCookie(ssl, ch->cookieExt.elements + OPAQUE16_LEN,
                (word16)(ch->cookieExt.size - OPAQUE16_LEN));
        if (ret < 0 && ret != HRR_COOKIE_ERROR)
            return ret;
        *cookieGood = ret > 0;
        ret = 0;
    }
    else
#endif
    {
        if (ch->cookie.size != DTLS_COOKIE_SZ)
            return 0;
        if (!ch->dtls12cookieSet) {
            ret = CreateDtls12Cookie(ssl, ch, ch->dtls12cookie);
            if (ret != 0)
                return ret;
            ch->dtls12cookieSet = 1;
        }
        *cookieGood = ConstantCompare(ch->cookie.elements, ch->dtls12cookie,
                                      DTLS_COOKIE_SZ) == 0;
    }
    return ret;
}

static int ParseClientHello(const byte* input, word32 helloSz, WolfSSL_CH* ch)
{
    word32 idx = 0;

    /* protocol version, random and session id length check */
    if (OPAQUE16_LEN + RAN_LEN + OPAQUE8_LEN > helloSz)
        return BUFFER_ERROR;

    ch->raw = input;
    ch->pv = (ProtocolVersion*)(input + idx);
    idx += OPAQUE16_LEN;
    ch->random = (byte*)(input + idx);
    idx += RAN_LEN;
    idx += ReadVector8(input + idx, &ch->sessionId);
    if (idx > helloSz - OPAQUE8_LEN)
        return BUFFER_ERROR;
    idx += ReadVector8(input + idx, &ch->cookie);
    if (idx > helloSz - OPAQUE16_LEN)
        return BUFFER_ERROR;
    idx += ReadVector16(input + idx, &ch->cipherSuite);
    if (idx > helloSz - OPAQUE8_LEN)
        return BUFFER_ERROR;
    idx += ReadVector8(input + idx, &ch->compression);
    if (idx < helloSz - OPAQUE16_LEN) {
        /* Extensions are optional */
        idx += ReadVector16(input + idx, &ch->extension);
        if (idx > helloSz)
            return BUFFER_ERROR;
    }
    if (idx != helloSz)
        return BUFFER_ERROR;
    ch->length = idx;
    return 0;
}

#if (defined(WOLFSSL_DTLS_NO_HVR_ON_RESUME) && defined(HAVE_SESSION_TICKET)) \
    || defined(WOLFSSL_DTLS13)
static int FindExtByType(WolfSSL_ConstVector* ret, word16 extType,
    WolfSSL_ConstVector exts, int* tlsxFound)
{
    word32 len, idx = 0;
    word16 type;
    WolfSSL_ConstVector ext;

    XMEMSET(ret, 0, sizeof(*ret));
    len = exts.size;
    *tlsxFound = FALSE;
    /* type + len */
    while (len >= OPAQUE16_LEN + OPAQUE16_LEN) {
        ato16(exts.elements + idx, &type);
        idx += OPAQUE16_LEN;
        idx += ReadVector16(exts.elements + idx, &ext);
        if (idx > exts.size)
            return BUFFER_ERROR;
        if (type == extType) {
            XMEMCPY(ret, &ext, sizeof(ext));
            *tlsxFound = TRUE;
            return 0;
        }
        len = exts.size - idx;
    }
    return 0;
}
#endif

#if defined(WOLFSSL_DTLS_NO_HVR_ON_RESUME)
#ifdef HAVE_SESSION_TICKET
static int TlsTicketIsValid(const WOLFSSL* ssl, WolfSSL_ConstVector exts,
                            int* resume)
{
    WolfSSL_ConstVector tlsxSessionTicket;
    byte tempTicket[SESSION_TICKET_LEN];
    InternalTicket* it = NULL;
    int ret = 0;
    int tlsxFound;

    *resume = FALSE;

    ret = FindExtByType(&tlsxSessionTicket, TLSX_SESSION_TICKET, exts,
                         &tlsxFound);
    if (ret != 0)
        return ret;
    if (tlsxSessionTicket.size == 0)
        return 0;
    if (tlsxSessionTicket.size > SESSION_TICKET_LEN)
        return 0;
    XMEMCPY(tempTicket, tlsxSessionTicket.elements, tlsxSessionTicket.size);
    ret = DoDecryptTicket(ssl, tempTicket, (word32)tlsxSessionTicket.size, &it);
    if (ret == WOLFSSL_TICKET_RET_OK || ret == WOLFSSL_TICKET_RET_CREATE) {
        /* This logic is only for TLS <= 1.2 tickets. Don't accept TLS 1.3. */
        if (!IsAtLeastTLSv1_3(it->pv))
            *resume = TRUE;
    }
    if (it != NULL)
        ForceZero(it, sizeof(InternalTicket));
    return 0;
}
#endif /* HAVE_SESSION_TICKET */

static int TlsSessionIdIsValid(const WOLFSSL* ssl, WolfSSL_ConstVector sessionID,
                               int* resume)
{
    const WOLFSSL_SESSION* sess;
    word32 sessRow;
    int ret;
#ifdef HAVE_EXT_CACHE
    int copy;
#endif
    *resume = FALSE;

    if (ssl->options.sessionCacheOff)
        return 0;
    if (sessionID.size != ID_LEN)
        return 0;

#ifdef HAVE_EXT_CACHE
    if (ssl->ctx->get_sess_cb != NULL) {
        WOLFSSL_SESSION* extSess =
            ssl->ctx->get_sess_cb((WOLFSSL*)ssl, sessionID.elements, ID_LEN,
                                  &copy);
        if (extSess != NULL) {
#if defined(SESSION_CERTS) || (defined(WOLFSSL_TLS13) && \
                           defined(HAVE_SESSION_TICKET))
            /* This logic is only for TLS <= 1.2 tickets. Don't accept
             * TLS 1.3. */
            if (!IsAtLeastTLSv1_3(extSess->version))
#endif
                *resume = TRUE;
            if (!copy)
                wolfSSL_FreeSession(ssl->ctx, extSess);
            if (*resume)
                return 0;
        }
    }
    if (ssl->ctx->internalCacheLookupOff)
        return 0;
#endif


    ret = TlsSessionCacheGetAndRdLock(sessionID.elements, &sess, &sessRow,
            ssl->options.side);
    if (ret == 0 && sess != NULL) {
#if defined(SESSION_CERTS) || (defined(WOLFSSL_TLS13) && \
                               defined(HAVE_SESSION_TICKET))
        /* This logic is only for TLS <= 1.2 tickets. Don't accept
         * TLS 1.3. */
        if (!IsAtLeastTLSv1_3(sess->version))
#endif
            *resume = TRUE;
        TlsSessionCacheUnlockRow(sessRow);
    }

    return 0;
}

static int TlsResumptionIsValid(const WOLFSSL* ssl, WolfSSL_CH* ch,
                                int* resume)
{
    int ret;

#ifdef HAVE_SESSION_TICKET
    ret = TlsTicketIsValid(ssl, ch->extension, resume);
    if (ret != 0)
        return ret;
    if (*resume)
        return 0;
#endif /* HAVE_SESSION_TICKET */
    ret = TlsSessionIdIsValid(ssl, ch->sessionId, resume);
    return ret;
}
#endif /* WOLFSSL_DTLS13 || WOLFSSL_DTLS_NO_HVR_ON_RESUME */

#ifdef WOLFSSL_DTLS13
static int TlsCheckSupportedVersion(const WOLFSSL* ssl,
        WolfSSL_CH* ch, byte *isTls13)
{
    WolfSSL_ConstVector tlsxSupportedVersions;
    int ret;
    ProtocolVersion pv = ssl->version;
    int tlsxFound;

    ret = FindExtByType(&tlsxSupportedVersions, TLSX_SUPPORTED_VERSIONS,
                         ch->extension, &tlsxFound);
    if (ret != 0)
        return ret;
    if (!tlsxFound) {
        *isTls13 = 0;
        return 0;
    }
    ret = TLSX_SupportedVersions_Parse(ssl, tlsxSupportedVersions.elements,
            (word16)tlsxSupportedVersions.size, client_hello, &pv, NULL, NULL);
    if (ret != 0)
        return ret;
    if (IsAtLeastTLSv1_3(pv))
        *isTls13 = 1;
    else
        *isTls13 = 0;

    return 0;
}
#endif

#if defined(WOLFSSL_DTLS13) && \
        (!defined(NO_PSK) || defined(HAVE_SESSION_TICKET))
/* Very simplified version of CheckPreSharedKeys to find the current suite */
static void FindPskSuiteFromExt(const WOLFSSL* ssl, TLSX* extensions,
                                PskInfo* pskInfo, Suites* suites)
{
    TLSX* pskExt = TLSX_Find(extensions, TLSX_PRE_SHARED_KEY);
    PreSharedKey* current;
    int i;
    int ret;

    if (pskExt == NULL)
        return;

    for (i = 0; i < suites->suiteSz; i += 2) {
        for (current = (PreSharedKey*)pskExt->data; current != NULL;
                current = current->next) {
#ifdef HAVE_SESSION_TICKET
            {
                /* Decode the identity. */
                switch (current->decryptRet) {
                    case PSK_DECRYPT_NONE:
                        ret = DoClientTicket_ex(ssl, current, 0);
                        break;
                    case PSK_DECRYPT_OK:
                        ret = WOLFSSL_TICKET_RET_OK;
                        break;
                    case PSK_DECRYPT_CREATE:
                        ret = WOLFSSL_TICKET_RET_CREATE;
                        break;
                    case PSK_DECRYPT_FAIL:
                    default:
                        ret = WOLFSSL_TICKET_RET_REJECT;
                        break;
                }
                if (ret == WOLFSSL_TICKET_RET_OK) {
                    if (DoClientTicketCheck(ssl, current, ssl->timeout,
                            suites->suites + i) != 0) {
                        continue;
                    }

                    pskInfo->cipherSuite0 = current->it->suite[0];
                    pskInfo->cipherSuite  = current->it->suite[1];
                    pskInfo->isValid      = 1;
                    goto cleanup;
                }
            }
#endif
#ifndef NO_PSK
            {
                int found = 0;
                byte psk_key[MAX_PSK_KEY_LEN];
                word32 psk_keySz;
                byte foundSuite[SUITE_LEN];
                ret = FindPskSuite(ssl, current, psk_key, &psk_keySz,
                        suites->suites + i, &found, foundSuite);
                /* Clear the key just in case */
                ForceZero(psk_key, sizeof(psk_key));
                if (ret == 0 && found) {
                    pskInfo->cipherSuite0 = foundSuite[0];
                    pskInfo->cipherSuite  = foundSuite[1];
                    pskInfo->isValid      = 1;
                    goto cleanup;
                }
            }
#endif
        }
    }

    /* Empty return necessary so we can have both the label and macro guard */
cleanup:
#ifdef HAVE_SESSION_TICKET
    CleanupClientTickets((PreSharedKey*)pskExt->data);
#endif
    return;
}
#endif

#ifdef WOLFSSL_DTLS13

#ifndef WOLFSSL_SEND_HRR_COOKIE
#error "WOLFSSL_SEND_HRR_COOKIE has to be defined to use DTLS 1.3 server"
#endif

#ifdef WOLFSSL_PSK_ONE_ID
#error WOLFSSL_PSK_ONE_ID is not compatible with stateless DTLS 1.3 server. \
        wolfSSL needs to be able to make multiple calls for the same PSK.
#endif

static int SendStatelessReplyDtls13(const WOLFSSL* ssl, WolfSSL_CH* ch)
{
    int ret = -1;
    TLSX* parsedExts = NULL;
    WolfSSL_ConstVector tlsx;
    int tlsxFound;
    Suites suites;
    byte haveSA = 0;
    byte haveKS = 0;
    byte haveSG = 0;
#if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
    byte usePSK = 0;
    byte doKE = 0;
#endif
    CipherSuite cs;
    CipherSpecs specs;
    byte cookieHash[WC_MAX_DIGEST_SIZE];
    int cookieHashSz;
#if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
    PskInfo pskInfo;
    XMEMSET(&pskInfo, 0, sizeof(pskInfo));
#endif

#ifndef HAVE_SUPPORTED_CURVES
    (void)doKE;
#endif /* !HAVE_SUPPORTED_CURVES */

    XMEMSET(&cs, 0, sizeof(cs));

    /* We need to echo the session ID sent by the client */
    if (ch->sessionId.size > ID_LEN) {
        /* Too large. We can't echo this. */
        ERROR_OUT(INVALID_PARAMETER, dtls13_cleanup);
    }

    /* Populate the suites struct to find a common ciphersuite */
    XMEMSET(&suites, 0, sizeof(suites));
    suites.suiteSz = (word16)ch->cipherSuite.size;
    if ((suites.suiteSz % 2) != 0)
        ERROR_OUT(INVALID_PARAMETER, dtls13_cleanup);
    if (suites.suiteSz > WOLFSSL_MAX_SUITE_SZ)
        ERROR_OUT(BUFFER_ERROR, dtls13_cleanup);
    XMEMCPY(suites.suites, ch->cipherSuite.elements, suites.suiteSz);

    /* Populate extensions */

    /* Supported versions always need to be present. Has to appear after
     * key share as that is the order we reconstruct it in
     * RestartHandshakeHashWithCookie. */
    ret = TLSX_Push(&parsedExts,
              TLSX_SUPPORTED_VERSIONS, ssl, ssl->heap);
    if (ret != 0)
        goto dtls13_cleanup;
    /* Set that this is a response extension */
    parsedExts->resp = 1;

#if defined(HAVE_SUPPORTED_CURVES)
    ret = TLSX_SupportedCurve_Copy(ssl->extensions, &parsedExts, ssl->heap);
    if (ret != 0)
        goto dtls13_cleanup;
#endif

#if !defined(NO_CERTS)
    /* Signature algs */
    ret = FindExtByType(&tlsx, TLSX_SIGNATURE_ALGORITHMS,
                         ch->extension, &tlsxFound);
    if (ret != 0)
        goto dtls13_cleanup;
    if (tlsxFound) {
        WolfSSL_ConstVector sigAlgs;
        if (tlsx.size < OPAQUE16_LEN)
            ERROR_OUT(BUFFER_ERROR, dtls13_cleanup);
        ReadVector16(tlsx.elements, &sigAlgs);
        if (sigAlgs.size != tlsx.size - OPAQUE16_LEN)
            ERROR_OUT(BUFFER_ERROR, dtls13_cleanup);
        if ((sigAlgs.size % 2) != 0)
            ERROR_OUT(BUFFER_ERROR, dtls13_cleanup);
        suites.hashSigAlgoSz = (word16)sigAlgs.size;
        XMEMCPY(suites.hashSigAlgo, sigAlgs.elements, sigAlgs.size);
        haveSA = 1;
    }
#endif /* !defined(NO_CERTS) */

#ifdef HAVE_SUPPORTED_CURVES
    /* Supported groups */
    ret = FindExtByType(&tlsx, TLSX_SUPPORTED_GROUPS,
                         ch->extension, &tlsxFound);
    if (ret != 0)
        goto dtls13_cleanup;
    if (tlsxFound) {
        ret = TLSX_SupportedCurve_Parse(ssl, tlsx.elements,
                                     (word16)tlsx.size, 1, &parsedExts);
        if (ret != 0)
            goto dtls13_cleanup;
        haveSG = 1;
    }

    /* Key share */
    ret = FindExtByType(&tlsx, TLSX_KEY_SHARE,
                         ch->extension, &tlsxFound);
    if (ret != 0)
        goto dtls13_cleanup;
    if (tlsxFound) {
        ret = TLSX_KeyShare_Parse_ClientHello(ssl, tlsx.elements,
                                        (word16)tlsx.size, &parsedExts);
        if (ret != 0)
            goto dtls13_cleanup;
        haveKS = 1;
    }
#endif /* HAVE_SUPPORTED_CURVES */

#if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
    /* Pre-shared key */
    ret = FindExtByType(&tlsx, TLSX_PRE_SHARED_KEY, ch->extension, &tlsxFound);
    if (ret != 0)
        goto dtls13_cleanup;
    if (tlsxFound) {
        /* Let's just assume that the binders are correct here. We will
         * actually verify this in the stateful part of the processing
         * and if they don't match we will error out there anyway. */
        byte modes;

        /* Ask the user for the ciphersuite matching this identity */
        if (TLSX_PreSharedKey_Parse_ClientHello(&parsedExts,
                tlsx.elements, tlsx.size, ssl->heap) == 0)
            FindPskSuiteFromExt(ssl, parsedExts, &pskInfo, &suites);
        /* Revert to full handshake if PSK parsing failed */

        if (pskInfo.isValid) {
            ret = FindExtByType(&tlsx, TLSX_PSK_KEY_EXCHANGE_MODES,
                                 ch->extension, &tlsxFound);
            if (ret != 0)
                goto dtls13_cleanup;
            if (!tlsxFound)
                ERROR_OUT(PSK_KEY_ERROR, dtls13_cleanup);
            ret = TLSX_PskKeyModes_Parse_Modes(tlsx.elements, tlsx.size,
                                              client_hello, &modes);
            if (ret != 0)
                goto dtls13_cleanup;
            if ((modes & (1 << PSK_DHE_KE)) &&
                    !ssl->options.noPskDheKe) {
                if (!haveKS)
                    ERROR_OUT(PSK_KEY_ERROR, dtls13_cleanup);
                doKE = 1;
            }
            else if ((modes & (1 << PSK_KE)) == 0) {
                    ERROR_OUT(PSK_KEY_ERROR, dtls13_cleanup);
            }
            usePSK = 1;
        }
    }
#endif

#if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
    if (usePSK && pskInfo.isValid) {
        cs.cipherSuite0 = pskInfo.cipherSuite0;
        cs.cipherSuite = pskInfo.cipherSuite;

        /* https://datatracker.ietf.org/doc/html/rfc8446#section-9.2 */
        if (haveSG ^ haveKS) {
            WOLFSSL_MSG("Client needs to send both or none of KeyShare and "
                        "SupportedGroups");
            ERROR_OUT(INCOMPLETE_DATA, dtls13_cleanup);
        }

#ifdef HAVE_SUPPORTED_CURVES
        if (doKE) {
            byte searched = 0;
            ret = TLSX_KeyShare_Choose(ssl, parsedExts, cs.cipherSuite0,
                    cs.cipherSuite, &cs.clientKSE, &searched);
            if (ret != 0)
                goto dtls13_cleanup;
            if (cs.clientKSE == NULL && searched)
                cs.doHelloRetry = 1;
        }
#endif /* HAVE_SUPPORTED_CURVES */
    }
    else
#endif /* defined(HAVE_SESSION_TICKET) || !defined(NO_PSK) */
    {
        /* https://datatracker.ietf.org/doc/html/rfc8446#section-9.2 */
        if (!haveKS || !haveSA || !haveSG) {
            WOLFSSL_MSG("Client didn't send KeyShare or SigAlgs or "
                        "SupportedGroups.");
            ERROR_OUT(INCOMPLETE_DATA, dtls13_cleanup);
        }
        /* TLSX_KeyShare_Choose is done deep inside MatchSuite_ex */
        ret = MatchSuite_ex(ssl, &suites, &cs, parsedExts);
        if (ret < 0) {
            WOLFSSL_MSG("Unsupported cipher suite, ClientHello DTLS 1.3");
            ERROR_OUT(INCOMPLETE_DATA, dtls13_cleanup);
        }
    }

#ifdef HAVE_SUPPORTED_CURVES
    if (cs.doHelloRetry) {
        ret = TLSX_KeyShare_SetSupported(ssl, &parsedExts);
        if (ret != 0)
            goto dtls13_cleanup;
    }
    else {
        /* Need to remove the keyshare ext if we found a common group
         * and are not doing curve negotiation. */
        TLSX_Remove(&parsedExts, TLSX_KEY_SHARE, ssl->heap);
    }
#endif /* HAVE_SUPPORTED_CURVES */

    /* This is required to correctly generate the hash */
    ret = GetCipherSpec(WOLFSSL_SERVER_END, cs.cipherSuite0,
                         cs.cipherSuite, &specs, NULL);
    if (ret != 0)
        goto dtls13_cleanup;

    /* Calculate the cookie hash */
    ret = Dtls13HashClientHello(ssl, cookieHash, &cookieHashSz, ch->raw,
                                ch->length, &specs);
    if (ret != 0)
        goto dtls13_cleanup;

    /* Push the cookie to extensions */
    ret = CreateCookieExt(ssl, cookieHash, (word16)cookieHashSz,
                          &parsedExts, cs.cipherSuite0, cs.cipherSuite);
    if (ret != 0)
        goto dtls13_cleanup;

    {
        WOLFSSL* nonConstSSL = (WOLFSSL*)ssl;
        TLSX* sslExts = nonConstSSL->extensions;

        if (ret != 0)
            goto dtls13_cleanup;
        nonConstSSL->options.tls = 1;
        nonConstSSL->options.tls1_1 = 1;
        nonConstSSL->options.tls1_3 = 1;

        XMEMCPY(nonConstSSL->session->sessionID, ch->sessionId.elements,
                ch->sessionId.size);
        nonConstSSL->session->sessionIDSz = (byte)ch->sessionId.size;
        nonConstSSL->options.cipherSuite0 = cs.cipherSuite0;
        nonConstSSL->options.cipherSuite = cs.cipherSuite;
        nonConstSSL->extensions = parsedExts;

        ret = SendTls13ServerHello(nonConstSSL, hello_retry_request);

        /* Can be modified inside SendTls13ServerHello */
        parsedExts = nonConstSSL->extensions;

        nonConstSSL->session->sessionIDSz = 0;
        nonConstSSL->options.cipherSuite0 = 0;
        nonConstSSL->options.cipherSuite = 0;
        nonConstSSL->extensions = sslExts;

        nonConstSSL->options.tls = 0;
        nonConstSSL->options.tls1_1 = 0;
        nonConstSSL->options.tls1_3 = 0;
    }
dtls13_cleanup:
    TLSX_FreeAll(parsedExts, ssl->heap);
    return ret;
}
#endif

static int SendStatelessReply(const WOLFSSL* ssl, WolfSSL_CH* ch, byte isTls13)
{
    int ret;
    (void)isTls13;
#ifdef WOLFSSL_DTLS13
    if (isTls13) {
        ret = SendStatelessReplyDtls13(ssl, ch);
    }
    else
#endif
    {
        if (!ch->dtls12cookieSet) {
            ret = CreateDtls12Cookie(ssl, ch, ch->dtls12cookie);
            if (ret != 0)
                return ret;
            ch->dtls12cookieSet = 1;
        }
        ret = SendHelloVerifyRequest((WOLFSSL*)ssl, ch->dtls12cookie,
                DTLS_COOKIE_SZ);
    }
    return ret;
}

static int ClientHelloSanityCheck(WolfSSL_CH* ch, byte isTls13)
{
    /* Do basic checks on the basic fields */

    /* Check the protocol version */
    if (ch->pv->major != DTLS_MAJOR)
        return VERSION_ERROR;
    if (ch->pv->minor != DTLSv1_2_MINOR && ch->pv->minor != DTLS_MINOR)
        return VERSION_ERROR;
    if (isTls13) {
        if (ch->cookie.size != 0)
            return INVALID_PARAMETER;
        if (ch->compression.size != COMP_LEN)
            return INVALID_PARAMETER;
        if (ch->compression.elements[0] != NO_COMPRESSION)
            return INVALID_PARAMETER;
    }

    return 0;
}

int DoClientHelloStateless(WOLFSSL* ssl, const byte* input,
                           word32* inOutIdx, word32 helloSz)
{
    int ret;
    WolfSSL_CH ch;
    byte isTls13 = 0;

    XMEMSET(&ch, 0, sizeof(ch));

    ssl->options.dtlsStateful = 0;
    ret = ParseClientHello(input + *inOutIdx, helloSz, &ch);
    if (ret != 0)
        return ret;

#ifdef WOLFSSL_DTLS13
    if (IsAtLeastTLSv1_3(ssl->version)) {
        ret = TlsCheckSupportedVersion(ssl, &ch, &isTls13);
        if (ret != 0)
            return ret;
        if (isTls13) {
            int tlsxFound;
            ret = FindExtByType(&ch.cookieExt, TLSX_COOKIE, ch.extension,
                                 &tlsxFound);
            if (ret != 0)
                return ret;
        }
    }
#endif

    ret = ClientHelloSanityCheck(&ch, isTls13);
    if (ret != 0)
        return ret;

#ifdef WOLFSSL_DTLS_NO_HVR_ON_RESUME
    if (!isTls13) {
        int resume = FALSE;
        ret = TlsResumptionIsValid(ssl, &ch, &resume);
        if (ret != 0)
            return ret;
        if (resume) {
            ssl->options.dtlsStateful = 1;
            return 0;
        }
    }
#endif

    if (ch.cookie.size == 0 && ch.cookieExt.size == 0) {
        ret = SendStatelessReply((WOLFSSL*)ssl, &ch, isTls13);
    }
    else {
        byte cookieGood;
        ret = CheckDtlsCookie(ssl, &ch, isTls13, &cookieGood);
        if (ret != 0)
            return ret;
        if (!cookieGood) {
#ifdef WOLFSSL_DTLS13
            /* Invalid cookie for DTLS 1.3 results in an alert. Alert to be sent
             * in DoTls13ClientHello. */
            if (isTls13)
                ret = INVALID_PARAMETER;
            else
#endif
                ret = SendStatelessReply((WOLFSSL*)ssl, &ch, isTls13);
        }
        else
            ssl->options.dtlsStateful = 1;
    }

    return ret;
}
#endif /* !defined(NO_WOLFSSL_SERVER) */

#if defined(WOLFSSL_DTLS_CID)

typedef struct ConnectionID {
    byte length;
/* Ignore "nonstandard extension used : zero-sized array in struct/union"
 * MSVC warning */
#ifdef _MSC_VER
#pragma warning(disable: 4200)
#endif
    byte id[];
} ConnectionID;

typedef struct CIDInfo {
    ConnectionID* tx;
    ConnectionID* rx;
    byte negotiated : 1;
} CIDInfo;

static ConnectionID* DtlsCidNew(const byte* cid, byte size, void* heap)
{
    ConnectionID* ret;

    ret = (ConnectionID*)XMALLOC(sizeof(ConnectionID) + size, heap,
        DYNAMIC_TYPE_TLSX);
    if (ret == NULL)
        return NULL;

    ret->length = size;
    XMEMCPY(ret->id, cid, size);

    return ret;
}

static WC_INLINE CIDInfo* DtlsCidGetInfo(WOLFSSL* ssl)
{
    return ssl->dtlsCidInfo;
}

static int DtlsCidGetSize(WOLFSSL* ssl, unsigned int* size, int rx)
{
    ConnectionID* id;
    CIDInfo* info;

    if (ssl == NULL || size == NULL)
        return BAD_FUNC_ARG;

    info = DtlsCidGetInfo(ssl);
    if (info == NULL)
        return WOLFSSL_FAILURE;

    id = rx ? info->rx : info->tx;
    if (id == NULL) {
        *size = 0;
        return WOLFSSL_SUCCESS;
    }

    *size = id->length;
    return WOLFSSL_SUCCESS;
}

static int DtlsCidGet(WOLFSSL* ssl, unsigned char* buf, int bufferSz, int rx)
{
    ConnectionID* id;
    CIDInfo* info;

    if (ssl == NULL || buf == NULL)
        return BAD_FUNC_ARG;

    info = DtlsCidGetInfo(ssl);
    if (info == NULL)
        return WOLFSSL_FAILURE;

    id = rx ? info->rx : info->tx;
    if (id == NULL || id->length == 0)
        return WOLFSSL_SUCCESS;

    if (id->length > bufferSz)
        return LENGTH_ERROR;

    XMEMCPY(buf, id->id, id->length);
    return WOLFSSL_SUCCESS;
}

static CIDInfo* DtlsCidGetInfoFromExt(byte* ext)
{
    WOLFSSL** sslPtr;
    WOLFSSL* ssl;

    if (ext == NULL)
        return NULL;
    sslPtr = (WOLFSSL**)ext;
    ssl = *sslPtr;
    if (ssl == NULL)
        return NULL;
    return ssl->dtlsCidInfo;
}

static void DtlsCidUnsetInfoFromExt(byte* ext)
{
    WOLFSSL** sslPtr;
    WOLFSSL* ssl;

    if (ext == NULL)
        return;
    sslPtr = (WOLFSSL**)ext;
    ssl = *sslPtr;
    if (ssl == NULL)
        return;
    ssl->dtlsCidInfo = NULL;
}

void TLSX_ConnectionID_Free(byte* ext, void* heap)
{
    CIDInfo* info;
    (void)heap;

    info = DtlsCidGetInfoFromExt(ext);
    if (info == NULL)
        return;
    if (info->rx != NULL)
        XFREE(info->rx, heap, DYNAMIC_TYPE_TLSX);
    if (info->tx != NULL)
        XFREE(info->tx, heap, DYNAMIC_TYPE_TLSX);
    XFREE(info, heap, DYNAMIC_TYPE_TLSX);
    DtlsCidUnsetInfoFromExt(ext);
    XFREE(ext, heap, DYNAMIC_TYPE_TLSX);
}

word16 TLSX_ConnectionID_Write(byte* ext, byte* output)
{
    CIDInfo* info;

    info = DtlsCidGetInfoFromExt(ext);
    if (info == NULL)
        return 0;

    /* empty CID */
    if (info->rx == NULL) {
        *output = 0;
        return OPAQUE8_LEN;
    }

    *output = info->rx->length;
    XMEMCPY(output + OPAQUE8_LEN, info->rx->id, info->rx->length);
    return OPAQUE8_LEN + info->rx->length;
}

word16 TLSX_ConnectionID_GetSize(byte* ext)
{
    CIDInfo* info = DtlsCidGetInfoFromExt(ext);
    if (info == NULL)
        return 0;
    return info->rx == NULL ? OPAQUE8_LEN : OPAQUE8_LEN + info->rx->length;
}

int TLSX_ConnectionID_Use(WOLFSSL* ssl)
{
    CIDInfo* info;
    WOLFSSL** ext;
    int ret;

    ext = (WOLFSSL**)TLSX_Find(ssl->extensions, TLSX_CONNECTION_ID);
    if (ext != NULL)
        return 0;

    info = (CIDInfo*)XMALLOC(sizeof(CIDInfo), ssl->heap, DYNAMIC_TYPE_TLSX);
    if (info == NULL)
        return MEMORY_ERROR;
    ext = (WOLFSSL**)XMALLOC(sizeof(WOLFSSL**), ssl->heap, DYNAMIC_TYPE_TLSX);
    if (ext == NULL) {
        XFREE(info, ssl->heap, DYNAMIC_TYPE_TLSX);
        return MEMORY_ERROR;
    }
    XMEMSET(info, 0, sizeof(CIDInfo));
    /* CIDInfo needs to be accessed every time we send or receive a record. To
     * avoid the cost of the extension lookup save a pointer to the structure
     * inside the SSL object itself, and save a pointer to the SSL object in the
     * extension. The extension freeing routine uses the pointer to the SSL
     * object to find the structure and to set ssl->dtlsCidInfo pointer to NULL
     * after freeing the structure. */
    ssl->dtlsCidInfo = info;
    *ext = ssl;
    ret =
        TLSX_Push(&ssl->extensions, TLSX_CONNECTION_ID, (void*)ext, ssl->heap);
    if (ret != 0) {
        XFREE(info, ssl->heap, DYNAMIC_TYPE_TLSX);
        XFREE(ext, ssl->heap, DYNAMIC_TYPE_TLSX);
        ssl->dtlsCidInfo = NULL;
        return ret;
    }

    return 0;
}

int TLSX_ConnectionID_Parse(WOLFSSL* ssl, const byte* input, word16 length,
    byte isRequest)
{
    ConnectionID* id;
    CIDInfo* info;
    byte cidSize;
    TLSX* ext;

    ext = TLSX_Find(ssl->extensions, TLSX_CONNECTION_ID);
    if (ext == NULL) {
        /* CID not enabled */
        if (isRequest) {
            WOLFSSL_MSG("Received CID ext but it's not enabled, ignoring");
            return 0;
        }
        else {
            WOLFSSL_MSG("CID ext not requested by the Client, aborting");
            return UNSUPPORTED_EXTENSION;
        }
    }

    info = DtlsCidGetInfo(ssl);
    if (info == NULL)
        return BAD_STATE_E;

    /* it may happen if we process two ClientHello because the server sent an
     * HRR request */
    if (info->tx != NULL) {
        if (ssl->options.side != WOLFSSL_SERVER_END &&
            ssl->options.serverState != SERVER_HELLO_RETRY_REQUEST_COMPLETE)
            return BAD_STATE_E;

        XFREE(info->tx, ssl->heap, DYNAMIC_TYPE_TLSX);
        info->tx = NULL;
    }

    if (length < OPAQUE8_LEN)
        return BUFFER_ERROR;

    cidSize = *input;
    if (cidSize + OPAQUE8_LEN > length)
        return BUFFER_ERROR;

    if (cidSize > 0) {
        id = (ConnectionID*)XMALLOC(sizeof(*id) + cidSize, ssl->heap,
            DYNAMIC_TYPE_TLSX);
        if (id == NULL)
            return MEMORY_ERROR;
        XMEMCPY(id->id, input + OPAQUE8_LEN, cidSize);
        id->length = cidSize;
        info->tx = id;
    }

    info->negotiated = 1;
    if (isRequest)
        ext->resp = 1;

    return 0;
}

void DtlsCIDOnExtensionsParsed(WOLFSSL* ssl)
{
    CIDInfo* info;

    info = DtlsCidGetInfo(ssl);
    if (info == NULL)
        return;

    if (!info->negotiated) {
        TLSX_Remove(&ssl->extensions, TLSX_CONNECTION_ID, ssl->heap);
        return;
    }
}

byte DtlsCIDCheck(WOLFSSL* ssl, const byte* input, word16 inputSize)
{
    CIDInfo* info;
    info = DtlsCidGetInfo(ssl);
    if (info == NULL || info->rx == NULL || info->rx->length == 0)
        return 0;
    if (inputSize < info->rx->length)
        return 0;
    return XMEMCMP(input, info->rx->id, info->rx->length) == 0;
}

int wolfSSL_dtls_cid_use(WOLFSSL* ssl)
{
    int ret;

    /* CID is supported on DTLSv1.3 only */
    if (!IsAtLeastTLSv1_3(ssl->version))
        return WOLFSSL_FAILURE;

    ssl->options.useDtlsCID = 1;
    ret = TLSX_ConnectionID_Use(ssl);
    if (ret != 0)
        return ret;
    return WOLFSSL_SUCCESS;
}

int wolfSSL_dtls_cid_is_enabled(WOLFSSL* ssl)
{
    return DtlsCidGetInfo(ssl) != NULL;
}

int wolfSSL_dtls_cid_set(WOLFSSL* ssl, unsigned char* cid, unsigned int size)
{
    ConnectionID* newCid;
    CIDInfo* cidInfo;

    if (!ssl->options.useDtlsCID)
        return WOLFSSL_FAILURE;

    cidInfo = DtlsCidGetInfo(ssl);
    if (cidInfo == NULL)
        return WOLFSSL_FAILURE;

    if (cidInfo->rx != NULL) {
        XFREE(cidInfo->rx, ssl->heap, DYNAMIC_TYPE_TLSX);
        cidInfo->rx = NULL;
    }

    /* empty CID */
    if (size == 0)
        return WOLFSSL_SUCCESS;

    if (size > DTLS_CID_MAX_SIZE)
        return LENGTH_ERROR;

    newCid = DtlsCidNew(cid, (byte)size, ssl->heap);
    if (newCid == NULL)
        return MEMORY_ERROR;
    cidInfo->rx = newCid;
    return WOLFSSL_SUCCESS;
}

int wolfSSL_dtls_cid_get_rx_size(WOLFSSL* ssl, unsigned int* size)
{
    return DtlsCidGetSize(ssl, size, 1);
}

int wolfSSL_dtls_cid_get_rx(WOLFSSL* ssl, unsigned char* buf,
    unsigned int bufferSz)
{
    return DtlsCidGet(ssl, buf, bufferSz, 1);
}

int wolfSSL_dtls_cid_get_tx_size(WOLFSSL* ssl, unsigned int* size)
{
    return DtlsCidGetSize(ssl, size, 0);
}

int wolfSSL_dtls_cid_get_tx(WOLFSSL* ssl, unsigned char* buf,
    unsigned int bufferSz)
{
    return DtlsCidGet(ssl, buf, bufferSz, 0);
}

#endif /* WOLFSSL_DTLS_CID */
#endif /* WOLFSSL_DTLS */

#endif /* WOLFCRYPT_ONLY */
