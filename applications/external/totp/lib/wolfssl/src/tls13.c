/* tls13.c
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
 * BUILD_GCM
 *    Enables AES-GCM ciphersuites.
 * HAVE_AESCCM
 *    Enables AES-CCM ciphersuites.
 * HAVE_SESSION_TICKET
 *    Enables session tickets - required for TLS 1.3 resumption.
 * NO_PSK
 *    Do not enable Pre-Shared Keys.
 * HAVE_KEYING_MATERIAL
 *    Enables exporting keying material based on section 7.5 of RFC 8446.
 * WOLFSSL_ASYNC_CRYPT
 *    Enables the use of asynchronous cryptographic operations.
 *    This is available for ciphers and certificates.
 * HAVE_CHACHA && HAVE_POLY1305
 *    Enables use of CHACHA20-POLY1305 ciphersuites.
 * WOLFSSL_DEBUG_TLS
 *    Writes out details of TLS 1.3 protocol including handshake message buffers
 *    and key generation input and output.
 * WOLFSSL_EARLY_DATA
 *    Allow 0-RTT Handshake using Early Data extensions and handshake message
 * WOLFSSL_EARLY_DATA_GROUP
 *    Group EarlyData message with ClientHello when sending
 * WOLFSSL_NO_SERVER_GROUPS_EXT
 *    Do not send the server's groups in an extension when the server's top
 *    preference is not in client's list.
 * WOLFSSL_POST_HANDSHAKE_AUTH
 *    Allow TLS v1.3 code to perform post-handshake authentication of the
 *    client.
 * WOLFSSL_SEND_HRR_COOKIE
 *    Send a cookie in hello_retry_request message to enable stateless tracking
 *    of ClientHello replies.
 * WOLFSSL_TLS13
 *    Enable TLS 1.3 protocol implementation.
 * WOLFSSL_TLS13_MIDDLEBOX_COMPAT
 *    Enable middlebox compatibility in the TLS 1.3 handshake.
 *    This includes sending ChangeCipherSpec before encrypted messages and
 *    including a session id.
 * WOLFSSL_TLS13_SHA512
 *    Allow generation of SHA-512 digests in handshake - no ciphersuite
 *    requires SHA-512 at this time.
 * WOLFSSL_TLS13_TICKET_BEFORE_FINISHED
 *    Allow a NewSessionTicket message to be sent by server before Client's
 *    Finished message.
 *    See TLS v1.3 specification, Section 4.6.1, Paragraph 4 (Note).
 * WOLFSSL_PSK_ONE_ID
 *    When only one PSK ID is used and only one call to the PSK callback can
 *    be made per connect.
 *    You cannot use wc_psk_client_cs_callback type callback on client.
 * WOLFSSL_PRIORITIZE_PSK
 *    During a handshake, prioritize PSK order instead of ciphersuite order.
 * WOLFSSL_CHECK_ALERT_ON_ERR
 *    Check for alerts during the handshake in the event of an error.
 * WOLFSSL_NO_CLIENT_CERT_ERROR
 *    Requires client to set a client certificate
 * WOLFSSL_PSK_MULTI_ID_PER_CS
 *    When multiple PSK identities are available for the same cipher suite.
 *    Sets the first byte of the client identity to the count of identities
 *    that have been seen so far for the cipher suite.
 * WOLFSSL_CHECK_SIG_FAULTS
 *    Verifies the ECC signature after signing in case of faults in the
 *    calculation of the signature. Useful when signature fault injection is a
 *    possible attack.
 * WOLFSSL_32BIT_MILLI_TIME
 *    Function TimeNowInMilliseconds() returns an unsigned 32-bit value.
 *    Default behavior is to return a signed 64-bit value.
 */

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#ifdef WOLFSSL_TLS13
#ifdef HAVE_SESSION_TICKET
    #include <wolfssl/wolfcrypt/wc_port.h>
#endif

#ifndef WOLFCRYPT_ONLY

#ifdef HAVE_ERRNO_H
    #include <errno.h>
#endif

#if defined(__MACH__) || defined(__FreeBSD__) || \
    defined(__INCLUDE_NUTTX_CONFIG_H) || defined(WOLFSSL_RIOT_OS)
#include <sys/time.h>
#endif /* __MACH__ || __FreeBSD__ ||
          __INCLUDE_NUTTX_CONFIG_H || WOLFSSL_RIOT_OS */


#include <wolfssl/internal.h>
#include <wolfssl/error-ssl.h>
#include <wolfssl/wolfcrypt/asn.h>
#include <wolfssl/wolfcrypt/dh.h>
#include <wolfssl/wolfcrypt/kdf.h>
#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

#ifdef __sun
    #include <sys/filio.h>
#endif

#ifndef TRUE
    #define TRUE  1
#endif
#ifndef FALSE
    #define FALSE 0
#endif

#ifndef HAVE_HKDF
    #ifndef _MSC_VER
        #error "The build option HAVE_HKDF is required for TLS 1.3"
    #else
        #pragma message("error: The build option HAVE_HKDF is required for TLS 1.3")
    #endif
#endif

#ifndef HAVE_TLS_EXTENSIONS
    #ifndef _MSC_VER
        #error "The build option HAVE_TLS_EXTENSIONS is required for TLS 1.3"
    #else
        #pragma message("error: The build option HAVE_TLS_EXTENSIONS is required for TLS 1.3")
    #endif
#endif


/* Set ret to error value and jump to label.
 *
 * err     The error value to set.
 * eLabel  The label to jump to.
 */
#define ERROR_OUT(err, eLabel) { ret = (err); goto eLabel; }

/* Size of the TLS v1.3 label use when deriving keys. */
#define TLS13_PROTOCOL_LABEL_SZ    6
/* The protocol label for TLS v1.3. */
static const byte tls13ProtocolLabel[TLS13_PROTOCOL_LABEL_SZ + 1] = "tls13 ";

#ifdef WOLFSSL_DTLS13
#define DTLS13_PROTOCOL_LABEL_SZ    6
static const byte dtls13ProtocolLabel[DTLS13_PROTOCOL_LABEL_SZ + 1] = "dtls13";
#endif /* WOLFSSL_DTLS13 */

#if defined(HAVE_ECH)
#define ECH_ACCEPT_CONFIRMATION_SZ 8
#define ECH_ACCEPT_CONFIRMATION_LABEL_SZ 23
static const byte
    echAcceptConfirmationLabel[ECH_ACCEPT_CONFIRMATION_LABEL_SZ + 1] =
    "ech accept confirmation";
#endif

#ifndef NO_CERTS
#if !defined(NO_RSA) || defined(HAVE_ECC) || defined(HAVE_ED25519) || \
    defined(HAVE_ED448) || defined(HAVE_PQC)

static WC_INLINE int GetMsgHash(WOLFSSL* ssl, byte* hash);

#endif
#endif

/* Expand data using HMAC, salt and label and info.
 * TLS v1.3 defines this function. Use callback if available.
 *
 * ssl          The SSL/TLS object.
 * okm          The generated pseudorandom key - output key material.
 * okmLen       The length of generated pseudorandom key -
 *              output key material.
 * prk          The salt - pseudo-random key.
 * prkLen       The length of the salt - pseudo-random key.
 * protocol     The TLS protocol label.
 * protocolLen  The length of the TLS protocol label.
 * info         The information to expand.
 * infoLen      The length of the information.
 * digest       The type of digest to use.
 * returns 0 on success, otherwise failure.
 */
static int Tls13HKDFExpandLabel(WOLFSSL* ssl, byte* okm, word32 okmLen,
                                const byte* prk, word32 prkLen,
                                const byte* protocol, word32 protocolLen,
                                const byte* label, word32 labelLen,
                                const byte* info, word32 infoLen,
                                int digest)
{
    int ret = NOT_COMPILED_IN;

#if defined(HAVE_PK_CALLBACKS)
    if (ssl->ctx && ssl->ctx->HKDFExpandLabelCb) {
        ret = ssl->ctx->HKDFExpandLabelCb(okm, okmLen, prk, prkLen,
                                          protocol, protocolLen,
                                          label, labelLen,
                                          info, infoLen, digest,
                                          WOLFSSL_CLIENT_END /* ignored */);
    }

    if (ret != NOT_COMPILED_IN)
        return ret;
#endif
    (void)ssl;
    PRIVATE_KEY_UNLOCK();
    ret = wc_Tls13_HKDF_Expand_Label(okm, okmLen, prk, prkLen,
                                     protocol, protocolLen,
                                     label, labelLen,
                                     info, infoLen, digest);
    PRIVATE_KEY_LOCK();
    return ret;
}

#if !defined(HAVE_FIPS) || !defined(wc_Tls13_HKDF_Expand_Label)
/* Same as above, but pass in the side we are expanding for.
 *
 * side      The side (WOLFSSL_CLIENT_END or WOLFSSL_SERVER_END).
 */
static int Tls13HKDFExpandKeyLabel(WOLFSSL* ssl, byte* okm, word32 okmLen,
                                   const byte* prk, word32 prkLen,
                                   const byte* protocol, word32 protocolLen,
                                   const byte* label, word32 labelLen,
                                   const byte* info, word32 infoLen,
                                   int digest, int side)
{
#if defined(HAVE_PK_CALLBACKS)
    int ret = NOT_COMPILED_IN;
    if (ssl->ctx && ssl->ctx->HKDFExpandLabelCb) {
        ret = ssl->ctx->HKDFExpandLabelCb(okm, okmLen, prk, prkLen,
                                         protocol, protocolLen,
                                         label, labelLen,
                                         info, infoLen,
                                         digest, side);
    }

    if (ret != NOT_COMPILED_IN)
        return ret;
#endif

/* hash buffer may not be fully initialized, but the sending length won't
 * extend beyond the initialized span.
 */
PRAGMA_GCC_DIAG_PUSH
PRAGMA_GCC("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
    (void)ssl;
    (void)side;
    return wc_Tls13_HKDF_Expand_Label(okm, okmLen, prk, prkLen,
                                      protocol, protocolLen,
                                      label, labelLen,
                                      info, infoLen, digest);
PRAGMA_GCC_DIAG_POP
}
#endif /* !HAVE_FIPS || !wc_Tls13_HKDF_Expand_Label */

/* Derive a key from a message.
 *
 * ssl        The SSL/TLS object.
 * output     The buffer to hold the derived key.
 * outputLen  The length of the derived key.
 * secret     The secret used to derive the key (HMAC secret).
 * label      The label used to distinguish the context.
 * labelLen   The length of the label.
 * msg        The message data to derive key from.
 * msgLen     The length of the message data to derive key from.
 * hashAlgo   The hash algorithm to use in the HMAC.
 * returns 0 on success, otherwise failure.
 */
static int DeriveKeyMsg(WOLFSSL* ssl, byte* output, int outputLen,
                        const byte* secret, const byte* label, word32 labelLen,
                        byte* msg, int msgLen, int hashAlgo)
{
    byte        hash[WC_MAX_DIGEST_SIZE];
    Digest      digest;
    word32      hashSz = 0;
    const byte* protocol;
    word32      protocolLen;
    int         digestAlg = -1;
    int         ret = BAD_FUNC_ARG;

    switch (hashAlgo) {
#ifndef NO_WOLFSSL_SHA256
        case sha256_mac:
            ret = wc_InitSha256_ex(&digest.sha256, ssl->heap, INVALID_DEVID);
            if (ret == 0) {
                    ret = wc_Sha256Update(&digest.sha256, msg, msgLen);
                if (ret == 0)
                    ret = wc_Sha256Final(&digest.sha256, hash);
                wc_Sha256Free(&digest.sha256);
            }
            hashSz = WC_SHA256_DIGEST_SIZE;
            digestAlg = WC_SHA256;
            break;
#endif
#ifdef WOLFSSL_SHA384
        case sha384_mac:
            ret = wc_InitSha384_ex(&digest.sha384, ssl->heap, INVALID_DEVID);
            if (ret == 0) {
                ret = wc_Sha384Update(&digest.sha384, msg, msgLen);
                if (ret == 0)
                    ret = wc_Sha384Final(&digest.sha384, hash);
                wc_Sha384Free(&digest.sha384);
            }
            hashSz = WC_SHA384_DIGEST_SIZE;
            digestAlg = WC_SHA384;
            break;
#endif
#ifdef WOLFSSL_TLS13_SHA512
        case sha512_mac:
            ret = wc_InitSha512_ex(&digest.sha512, ssl->heap, INVALID_DEVID);
            if (ret == 0) {
                ret = wc_Sha512Update(&digest.sha512, msg, msgLen);
                if (ret == 0)
                    ret = wc_Sha512Final(&digest.sha512, hash);
                wc_Sha512Free(&digest.sha512);
            }
            hashSz = WC_SHA512_DIGEST_SIZE;
            digestAlg = WC_SHA512;
            break;
#endif
#ifdef WOLFSSL_SM3
        case sm3_mac:
            ret = wc_InitSm3(&digest.sm3, ssl->heap, INVALID_DEVID);
            if (ret == 0) {
                ret = wc_Sm3Update(&digest.sm3, msg, msgLen);
                if (ret == 0)
                    ret = wc_Sm3Final(&digest.sm3, hash);
                wc_Sm3Free(&digest.sm3);
            }
            hashSz = WC_SM3_DIGEST_SIZE;
            digestAlg = WC_SM3;
            break;
#endif
        default:
            digestAlg = -1;
            break;
    }

    if (digestAlg < 0)
        return HASH_TYPE_E;

    if (ret != 0)
        return ret;

    switch (ssl->version.minor) {
        case TLSv1_3_MINOR:
            protocol = tls13ProtocolLabel;
            protocolLen = TLS13_PROTOCOL_LABEL_SZ;
            break;
#ifdef WOLFSSL_DTLS13
        case DTLSv1_3_MINOR:
            if (!ssl->options.dtls)
                return VERSION_ERROR;

            protocol = dtls13ProtocolLabel;
            protocolLen = DTLS13_PROTOCOL_LABEL_SZ;
            break;
#endif /* WOLFSSL_DTLS13 */
        default:
            return VERSION_ERROR;
    }
    if (outputLen == -1)
        outputLen = hashSz;

    ret = Tls13HKDFExpandLabel(ssl, output, outputLen, secret, hashSz,
                               protocol, protocolLen, label, labelLen,
                               hash, hashSz, digestAlg);
    return ret;
}

/* Derive a key.
 *
 * ssl          The SSL/TLS object.
 * output       The buffer to hold the derived key.
 * outputLen    The length of the derived key.
 * secret       The secret used to derive the key (HMAC secret).
 * label        The label used to distinguish the context.
 * labelLen     The length of the label.
 * hashAlgo     The hash algorithm to use in the HMAC.
 * includeMsgs  Whether to include a hash of the handshake messages so far.
 * side         The side that we are deriving the secret for.
 * returns 0 on success, otherwise failure.
 */
int Tls13DeriveKey(WOLFSSL* ssl, byte* output, int outputLen,
                   const byte* secret, const byte* label, word32 labelLen,
                   int hashAlgo, int includeMsgs, int side)
{
    int         ret = 0;
    byte        hash[WC_MAX_DIGEST_SIZE];
    word32      hashSz = 0;
    word32      hashOutSz = 0;
    const byte* protocol;
    word32      protocolLen;
    int         digestAlg = 0;

    switch (hashAlgo) {
    #ifndef NO_SHA256
        case sha256_mac:
            hashSz    = WC_SHA256_DIGEST_SIZE;
            digestAlg = WC_SHA256;
            if (includeMsgs)
                ret = wc_Sha256GetHash(&ssl->hsHashes->hashSha256, hash);
            break;
    #endif

    #ifdef WOLFSSL_SHA384
        case sha384_mac:
            hashSz    = WC_SHA384_DIGEST_SIZE;
            digestAlg = WC_SHA384;
            if (includeMsgs)
                ret = wc_Sha384GetHash(&ssl->hsHashes->hashSha384, hash);
            break;
    #endif

    #ifdef WOLFSSL_TLS13_SHA512
        case sha512_mac:
            hashSz    = WC_SHA512_DIGEST_SIZE;
            digestAlg = WC_SHA512;
            if (includeMsgs)
                ret = wc_Sha512GetHash(&ssl->hsHashes->hashSha512, hash);
            break;
    #endif

    #ifdef WOLFSSL_SM3
        case sm3_mac:
            hashSz    = WC_SM3_DIGEST_SIZE;
            digestAlg = WC_SM3;
            if (includeMsgs)
                ret = wc_Sm3GetHash(&ssl->hsHashes->hashSm3, hash);
            break;
    #endif

        default:
            ret = HASH_TYPE_E;
            break;
    }
    if (ret != 0)
        return ret;

    protocol = tls13ProtocolLabel;
    protocolLen = TLS13_PROTOCOL_LABEL_SZ;

#ifdef WOLFSSL_DTLS13
    if (ssl->options.dtls) {
         protocol = dtls13ProtocolLabel;
         protocolLen = DTLS13_PROTOCOL_LABEL_SZ;
    }
#endif /* WOLFSSL_DTLS13 */

    if (outputLen == -1)
        outputLen = hashSz;
    if (includeMsgs)
        hashOutSz = hashSz;

    /* hash buffer may not be fully initialized, but the sending length won't
     * extend beyond the initialized span.
     */
    PRAGMA_GCC_DIAG_PUSH
    PRAGMA_GCC("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
    PRIVATE_KEY_UNLOCK();
    #if defined(HAVE_FIPS) && defined(wc_Tls13_HKDF_Expand_Label)
    (void)side;
    ret = wc_Tls13_HKDF_Expand_Label_fips(output, outputLen, secret, hashSz,
                             protocol, protocolLen, label, labelLen,
                             hash, hashOutSz, digestAlg);
    #else
    ret = Tls13HKDFExpandKeyLabel(ssl, output, outputLen, secret, hashSz,
                                  protocol, protocolLen, label, labelLen,
                                  hash, hashOutSz, digestAlg, side);
    #endif
    PRIVATE_KEY_LOCK();

#ifdef WOLFSSL_CHECK_MEM_ZERO
    wc_MemZero_Add("TLS 1.3 derived key", output, outputLen);
#endif
    return ret;
    PRAGMA_GCC_DIAG_POP
}

/* Convert TLS mac ID to a hash algorithm ID
 *
 * mac Mac ID to convert
 * returns hash ID on success, or the NONE type.
 */
static WC_INLINE int mac2hash(int mac)
{
    int hash;
    switch (mac) {
        #ifndef NO_SHA256
        case sha256_mac:
            hash = WC_SHA256;
            break;
        #endif

        #ifdef WOLFSSL_SHA384
        case sha384_mac:
            hash = WC_SHA384;
            break;
        #endif

        #ifdef WOLFSSL_TLS13_SHA512
        case sha512_mac:
            hash = WC_SHA512;
            break;
        #endif

        #ifdef WOLFSSL_SM3
        case sm3_mac:
            hash = WC_SM3;
            break;
        #endif

    default:
        hash = WC_HASH_TYPE_NONE;
    }
    return hash;
}

#ifndef NO_PSK
/* The length of the binder key label. */
#define BINDER_KEY_LABEL_SZ         10
/* The binder key label. */
static const byte binderKeyLabel[BINDER_KEY_LABEL_SZ + 1] =
    "ext binder";

/* Derive the binder key.
 *
 * ssl  The SSL/TLS object.
 * key  The derived key.
 * returns 0 on success, otherwise failure.
 */
static int DeriveBinderKey(WOLFSSL* ssl, byte* key)
{
    WOLFSSL_MSG("Derive Binder Key");
    if (ssl == NULL || ssl->arrays == NULL) {
        return BAD_FUNC_ARG;
    }
    return DeriveKeyMsg(ssl, key, -1, ssl->arrays->secret,
                        binderKeyLabel, BINDER_KEY_LABEL_SZ,
                        NULL, 0, ssl->specs.mac_algorithm);
}
#endif /* !NO_PSK */

#if defined(HAVE_SESSION_TICKET) && \
    (!defined(NO_WOLFSSL_CLIENT) || !defined(NO_WOLFSSL_SERVER))
/* The length of the binder key resume label. */
#define BINDER_KEY_RESUME_LABEL_SZ  10
/* The binder key resume label. */
static const byte binderKeyResumeLabel[BINDER_KEY_RESUME_LABEL_SZ + 1] =
    "res binder";

/* Derive the binder resumption key.
 *
 * ssl  The SSL/TLS object.
 * key  The derived key.
 * returns 0 on success, otherwise failure.
 */
static int DeriveBinderKeyResume(WOLFSSL* ssl, byte* key)
{
    WOLFSSL_MSG("Derive Binder Key - Resumption");
    if (ssl == NULL || ssl->arrays == NULL) {
        return BAD_FUNC_ARG;
    }
    return DeriveKeyMsg(ssl, key, -1, ssl->arrays->secret,
                        binderKeyResumeLabel, BINDER_KEY_RESUME_LABEL_SZ,
                        NULL, 0, ssl->specs.mac_algorithm);
}
#endif /* HAVE_SESSION_TICKET && (!NO_WOLFSSL_CLIENT || !NO_WOLFSSL_SERVER) */

#ifdef WOLFSSL_EARLY_DATA

/* The length of the early traffic label. */
#define EARLY_TRAFFIC_LABEL_SZ      11
/* The early traffic label. */
static const byte earlyTrafficLabel[EARLY_TRAFFIC_LABEL_SZ + 1] =
    "c e traffic";

/* Derive the early traffic key.
 *
 * ssl  The SSL/TLS object.
 * key  The derived key.
 * side The side that we are deriving the secret for.
 * returns 0 on success, otherwise failure.
 */
static int DeriveEarlyTrafficSecret(WOLFSSL* ssl, byte* key, int side)
{
    int ret;
    WOLFSSL_MSG("Derive Early Traffic Secret");
    if (ssl == NULL || ssl->arrays == NULL) {
        return BAD_FUNC_ARG;
    }
    ret = Tls13DeriveKey(ssl, key, -1, ssl->arrays->secret,
                    earlyTrafficLabel, EARLY_TRAFFIC_LABEL_SZ,
                    ssl->specs.mac_algorithm, 1, side);
#ifdef HAVE_SECRET_CALLBACK
    if (ret == 0 && ssl->tls13SecretCb != NULL) {
        ret = ssl->tls13SecretCb(ssl, CLIENT_EARLY_TRAFFIC_SECRET, key,
                                 ssl->specs.hash_size, ssl->tls13SecretCtx);
        if (ret != 0) {
            WOLFSSL_ERROR_VERBOSE(TLS13_SECRET_CB_E);
            return TLS13_SECRET_CB_E;
        }
    }
#ifdef OPENSSL_EXTRA
    if (ret == 0 && ssl->tls13KeyLogCb != NULL) {
        ret = ssl->tls13KeyLogCb(ssl, CLIENT_EARLY_TRAFFIC_SECRET, key,
                                ssl->specs.hash_size, NULL);
        if (ret != 0) {
            WOLFSSL_ERROR_VERBOSE(TLS13_SECRET_CB_E);
            return TLS13_SECRET_CB_E;
        }
    }
#endif /* OPENSSL_EXTRA */
#endif /* HAVE_SECRET_CALLBACK */
    return ret;
}

#endif

/* The length of the client handshake label. */
#define CLIENT_HANDSHAKE_LABEL_SZ   12
/* The client handshake label. */
static const byte clientHandshakeLabel[CLIENT_HANDSHAKE_LABEL_SZ + 1] =
    "c hs traffic";

/* Derive the client handshake key.
 *
 * ssl  The SSL/TLS object.
 * key  The derived key.
 * returns 0 on success, otherwise failure.
 */
static int DeriveClientHandshakeSecret(WOLFSSL* ssl, byte* key)
{
    int ret;
    WOLFSSL_MSG("Derive Client Handshake Secret");
    if (ssl == NULL || ssl->arrays == NULL) {
        return BAD_FUNC_ARG;
    }

    ret = Tls13DeriveKey(ssl, key, -1, ssl->arrays->preMasterSecret,
                    clientHandshakeLabel, CLIENT_HANDSHAKE_LABEL_SZ,
                    ssl->specs.mac_algorithm, 1, WOLFSSL_CLIENT_END);
#ifdef HAVE_SECRET_CALLBACK
    if (ret == 0 && ssl->tls13SecretCb != NULL) {
        ret = ssl->tls13SecretCb(ssl, CLIENT_HANDSHAKE_TRAFFIC_SECRET, key,
                                 ssl->specs.hash_size, ssl->tls13SecretCtx);
        if (ret != 0) {
            WOLFSSL_ERROR_VERBOSE(TLS13_SECRET_CB_E);
            return TLS13_SECRET_CB_E;
        }
    }
#ifdef OPENSSL_EXTRA
    if (ret == 0 && ssl->tls13KeyLogCb != NULL) {
        ret = ssl->tls13KeyLogCb(ssl, CLIENT_HANDSHAKE_TRAFFIC_SECRET, key,
                                ssl->specs.hash_size, NULL);
        if (ret != 0) {
            WOLFSSL_ERROR_VERBOSE(TLS13_SECRET_CB_E);
            return TLS13_SECRET_CB_E;
        }
    }
#endif /* OPENSSL_EXTRA */
#endif /* HAVE_SECRET_CALLBACK */
    return ret;
}

/* The length of the server handshake label. */
#define SERVER_HANDSHAKE_LABEL_SZ   12
/* The server handshake label. */
static const byte serverHandshakeLabel[SERVER_HANDSHAKE_LABEL_SZ + 1] =
    "s hs traffic";

/* Derive the server handshake key.
 *
 * ssl  The SSL/TLS object.
 * key  The derived key.
 * returns 0 on success, otherwise failure.
 */
static int DeriveServerHandshakeSecret(WOLFSSL* ssl, byte* key)
{
    int ret;
    WOLFSSL_MSG("Derive Server Handshake Secret");
    if (ssl == NULL || ssl->arrays == NULL) {
        return BAD_FUNC_ARG;
    }
    ret = Tls13DeriveKey(ssl, key, -1, ssl->arrays->preMasterSecret,
                    serverHandshakeLabel, SERVER_HANDSHAKE_LABEL_SZ,
                    ssl->specs.mac_algorithm, 1, WOLFSSL_SERVER_END);
#ifdef HAVE_SECRET_CALLBACK
    if (ret == 0 && ssl->tls13SecretCb != NULL) {
        ret = ssl->tls13SecretCb(ssl, SERVER_HANDSHAKE_TRAFFIC_SECRET, key,
                                 ssl->specs.hash_size, ssl->tls13SecretCtx);
        if (ret != 0) {
            WOLFSSL_ERROR_VERBOSE(TLS13_SECRET_CB_E);
            return TLS13_SECRET_CB_E;
        }
    }
#ifdef OPENSSL_EXTRA
    if (ret == 0 && ssl->tls13KeyLogCb != NULL) {
        ret = ssl->tls13KeyLogCb(ssl, SERVER_HANDSHAKE_TRAFFIC_SECRET, key,
                                ssl->specs.hash_size, NULL);
        if (ret != 0) {
            WOLFSSL_ERROR_VERBOSE(TLS13_SECRET_CB_E);
            return TLS13_SECRET_CB_E;
        }
    }
#endif /* OPENSSL_EXTRA */
#endif /* HAVE_SECRET_CALLBACK */
    return ret;
}

/* The length of the client application traffic label. */
#define CLIENT_APP_LABEL_SZ         12
/* The client application traffic label. */
static const byte clientAppLabel[CLIENT_APP_LABEL_SZ + 1] =
    "c ap traffic";

/* Derive the client application traffic key.
 *
 * ssl  The SSL/TLS object.
 * key  The derived key.
 * returns 0 on success, otherwise failure.
 */
static int DeriveClientTrafficSecret(WOLFSSL* ssl, byte* key)
{
    int ret;
    WOLFSSL_MSG("Derive Client Traffic Secret");
    if (ssl == NULL || ssl->arrays == NULL) {
        return BAD_FUNC_ARG;
    }
    ret = Tls13DeriveKey(ssl, key, -1, ssl->arrays->masterSecret,
                    clientAppLabel, CLIENT_APP_LABEL_SZ,
                    ssl->specs.mac_algorithm, 1, WOLFSSL_CLIENT_END);
#ifdef HAVE_SECRET_CALLBACK
    if (ret == 0 && ssl->tls13SecretCb != NULL) {
        ret = ssl->tls13SecretCb(ssl, CLIENT_TRAFFIC_SECRET, key,
                                 ssl->specs.hash_size, ssl->tls13SecretCtx);
        if (ret != 0) {
            WOLFSSL_ERROR_VERBOSE(TLS13_SECRET_CB_E);
            return TLS13_SECRET_CB_E;
        }
    }
#ifdef OPENSSL_EXTRA
    if (ret == 0 && ssl->tls13KeyLogCb != NULL) {
        ret = ssl->tls13KeyLogCb(ssl, CLIENT_TRAFFIC_SECRET, key,
                                ssl->specs.hash_size, NULL);
        if (ret != 0) {
            WOLFSSL_ERROR_VERBOSE(TLS13_SECRET_CB_E);
            return TLS13_SECRET_CB_E;
        }
    }
#endif /* OPENSSL_EXTRA */
#endif /* HAVE_SECRET_CALLBACK */
    return ret;
}

/* The length of the server application traffic label. */
#define SERVER_APP_LABEL_SZ         12
/* The  server application traffic label. */
static const byte serverAppLabel[SERVER_APP_LABEL_SZ + 1] =
    "s ap traffic";

/* Derive the server application traffic key.
 *
 * ssl  The SSL/TLS object.
 * key  The derived key.
 * returns 0 on success, otherwise failure.
 */
static int DeriveServerTrafficSecret(WOLFSSL* ssl, byte* key)
{
    int ret;
    WOLFSSL_MSG("Derive Server Traffic Secret");
    if (ssl == NULL || ssl->arrays == NULL) {
        return BAD_FUNC_ARG;
    }
    ret = Tls13DeriveKey(ssl, key, -1, ssl->arrays->masterSecret,
                    serverAppLabel, SERVER_APP_LABEL_SZ,
                    ssl->specs.mac_algorithm, 1, WOLFSSL_SERVER_END);
#ifdef HAVE_SECRET_CALLBACK
    if (ret == 0 && ssl->tls13SecretCb != NULL) {
        ret = ssl->tls13SecretCb(ssl, SERVER_TRAFFIC_SECRET, key,
                                 ssl->specs.hash_size, ssl->tls13SecretCtx);
        if (ret != 0) {
            WOLFSSL_ERROR_VERBOSE(TLS13_SECRET_CB_E);
            return TLS13_SECRET_CB_E;
        }
    }
#ifdef OPENSSL_EXTRA
    if (ret == 0 && ssl->tls13KeyLogCb != NULL) {
        ret = ssl->tls13KeyLogCb(ssl, SERVER_TRAFFIC_SECRET, key,
                                ssl->specs.hash_size, NULL);
        if (ret != 0) {
            WOLFSSL_ERROR_VERBOSE(TLS13_SECRET_CB_E);
            return TLS13_SECRET_CB_E;
        }
    }
#endif /* OPENSSL_EXTRA */
#endif /* HAVE_SECRET_CALLBACK */
    return ret;
}

#ifdef HAVE_KEYING_MATERIAL
/* The length of the exporter master secret label. */
#define EXPORTER_MASTER_LABEL_SZ    10
/* The exporter master secret label. */
static const byte exporterMasterLabel[EXPORTER_MASTER_LABEL_SZ + 1] =
    "exp master";

/* Derive the exporter secret.
 *
 * ssl  The SSL/TLS object.
 * key  The derived key.
 * returns 0 on success, otherwise failure.
 */
static int DeriveExporterSecret(WOLFSSL* ssl, byte* key)
{
    int ret;
    WOLFSSL_ENTER("Derive Exporter Secret");
    if (ssl == NULL || ssl->arrays == NULL) {
        return BAD_FUNC_ARG;
    }
    ret = Tls13DeriveKey(ssl, key, -1, ssl->arrays->masterSecret,
                        exporterMasterLabel, EXPORTER_MASTER_LABEL_SZ,
                        ssl->specs.mac_algorithm, 1, 0 /* Unused */);
#ifdef HAVE_SECRET_CALLBACK
    if (ret == 0 && ssl->tls13SecretCb != NULL) {
        ret = ssl->tls13SecretCb(ssl, EXPORTER_SECRET, key,
                                 ssl->specs.hash_size, ssl->tls13SecretCtx);
        if (ret != 0) {
            WOLFSSL_ERROR_VERBOSE(TLS13_SECRET_CB_E);
            return TLS13_SECRET_CB_E;
        }
    }
#ifdef OPENSSL_EXTRA
    if (ret == 0 && ssl->tls13KeyLogCb != NULL) {
        ret = ssl->tls13KeyLogCb(ssl, EXPORTER_SECRET, key,
                                ssl->specs.hash_size, NULL);
        if (ret != 0) {
            WOLFSSL_ERROR_VERBOSE(TLS13_SECRET_CB_E);
            return TLS13_SECRET_CB_E;
        }
    }
#endif /* OPENSSL_EXTRA */
#endif /* HAVE_SECRET_CALLBACK */
    return ret;
}

/* The length of the exporter label. */
#define EXPORTER_LABEL_SZ    8
/* The exporter label. */
static const byte exporterLabel[EXPORTER_LABEL_SZ + 1] =
    "exporter";
/* Hash("") */
#ifndef NO_SHA256
static const byte emptySHA256Hash[] = {
    0xE3, 0xB0, 0xC4, 0x42, 0x98, 0xFC, 0x1C, 0x14, 0x9A, 0xFB, 0xF4, 0xC8,
    0x99, 0x6F, 0xB9, 0x24, 0x27, 0xAE, 0x41, 0xE4, 0x64, 0x9B, 0x93, 0x4C,
    0xA4, 0x95, 0x99, 0x1B, 0x78, 0x52, 0xB8, 0x55
};
#endif
#ifdef WOLFSSL_SHA384
static const byte emptySHA384Hash[] = {
    0x38, 0xB0, 0x60, 0xA7, 0x51, 0xAC, 0x96, 0x38, 0x4C, 0xD9, 0x32, 0x7E,
    0xB1, 0xB1, 0xE3, 0x6A, 0x21, 0xFD, 0xB7, 0x11, 0x14, 0xBE, 0x07, 0x43,
    0x4C, 0x0C, 0xC7, 0xBF, 0x63, 0xF6, 0xE1, 0xDA, 0x27, 0x4E, 0xDE, 0xBF,
    0xE7, 0x6F, 0x65, 0xFB, 0xD5, 0x1A, 0xD2, 0xF1, 0x48, 0x98, 0xB9, 0x5B
};
#endif
#ifdef WOLFSSL_TLS13_SHA512
static const byte emptySHA512Hash[] = {
    0xCF, 0x83, 0xE1, 0x35, 0x7E, 0xEF, 0xB8, 0xBD, 0xF1, 0x54, 0x28, 0x50,
    0xD6, 0x6D, 0x80, 0x07, 0xD6, 0x20, 0xE4, 0x05, 0x0B, 0x57, 0x15, 0xDC,
    0x83, 0xF4, 0xA9, 0x21, 0xD3, 0x6C, 0xE9, 0xCE, 0x47, 0xD0, 0xD1, 0x3C,
    0x5D, 0x85, 0xF2, 0xB0, 0xFF, 0x83, 0x18, 0xD2, 0x87, 0x7E, 0xEC, 0x2F,
    0x63, 0xB9, 0x31, 0xBD, 0x47, 0x41, 0x7A, 0x81, 0xA5, 0x38, 0x32, 0x7A,
    0xF9, 0x27, 0xDA, 0x3E
};
#endif
/**
 * Implement section 7.5 of RFC 8446
 * @return  0 on success
 *         <0 on failure
 */
int Tls13_Exporter(WOLFSSL* ssl, unsigned char *out, size_t outLen,
        const char *label, size_t labelLen,
        const unsigned char *context, size_t contextLen)
{
    int                 ret;
    enum wc_HashType    hashType = WC_HASH_TYPE_NONE;
    int                 hashLen = 0;
    byte                hashOut[WC_MAX_DIGEST_SIZE];
    const byte*         emptyHash = NULL;
    byte                firstExpand[WC_MAX_DIGEST_SIZE];
    const byte*         protocol = tls13ProtocolLabel;
    word32              protocolLen = TLS13_PROTOCOL_LABEL_SZ;

    if (ssl->options.dtls && ssl->version.minor != DTLSv1_3_MINOR)
        return VERSION_ERROR;

    if (!ssl->options.dtls && ssl->version.minor != TLSv1_3_MINOR)
        return VERSION_ERROR;

#ifdef WOLFSSL_DTLS13
    if (ssl->options.dtls) {
        protocol = dtls13ProtocolLabel;
        protocolLen = DTLS13_PROTOCOL_LABEL_SZ;
    }
#endif /* WOLFSSL_DTLS13 */

    switch (ssl->specs.mac_algorithm) {
        #ifndef NO_SHA256
        case sha256_mac:
            hashType  = WC_HASH_TYPE_SHA256;
            hashLen   = WC_SHA256_DIGEST_SIZE;
            emptyHash = emptySHA256Hash;
            break;
        #endif

        #ifdef WOLFSSL_SHA384
        case sha384_mac:
            hashType  = WC_HASH_TYPE_SHA384;
            hashLen   = WC_SHA384_DIGEST_SIZE;
            emptyHash = emptySHA384Hash;
            break;
        #endif

        #ifdef WOLFSSL_TLS13_SHA512
        case sha512_mac:
            hashType  = WC_HASH_TYPE_SHA512;
            hashLen   = WC_SHA512_DIGEST_SIZE;
            emptyHash = emptySHA512Hash;
            break;
        #endif
    }

    /* Derive-Secret(Secret, label, "") */
    ret = Tls13HKDFExpandLabel(ssl, firstExpand, hashLen,
            ssl->arrays->exporterSecret, hashLen,
            protocol, protocolLen, (byte*)label, (word32)labelLen,
            emptyHash, hashLen, hashType);
    if (ret != 0)
        return ret;

    /* Hash(context_value) */
    ret = wc_Hash(hashType, context, (word32)contextLen, hashOut, WC_MAX_DIGEST_SIZE);
    if (ret != 0)
        return ret;

    ret = Tls13HKDFExpandLabel(ssl, out, (word32)outLen, firstExpand, hashLen,
            protocol, protocolLen, exporterLabel, EXPORTER_LABEL_SZ,
            hashOut, hashLen, hashType);

    return ret;
}
#endif

#if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
/* The length of the resumption master secret label. */
#define RESUME_MASTER_LABEL_SZ      10
/* The resumption master secret label. */
static const byte resumeMasterLabel[RESUME_MASTER_LABEL_SZ + 1] =
    "res master";

/* Derive the resumption secret.
 *
 * ssl  The SSL/TLS object.
 * key  The derived key.
 * returns 0 on success, otherwise failure.
 */
int DeriveResumptionSecret(WOLFSSL* ssl, byte* key)
{
    byte* masterSecret;

    WOLFSSL_MSG("Derive Resumption Secret");
    if (ssl == NULL) {
        return BAD_FUNC_ARG;
    }
    if (ssl->arrays != NULL) {
        masterSecret = ssl->arrays->masterSecret;
    }
    else {
        masterSecret = ssl->session->masterSecret;
    }
    return Tls13DeriveKey(ssl, key, -1, masterSecret, resumeMasterLabel,
                     RESUME_MASTER_LABEL_SZ, ssl->specs.mac_algorithm, 1,
                     0 /* Unused */);
}
#endif

/* Length of the finished label. */
#define FINISHED_LABEL_SZ           8
/* Finished label for generating finished key. */
static const byte finishedLabel[FINISHED_LABEL_SZ+1] = "finished";
/* Derive the finished secret.
 *
 * ssl     The SSL/TLS object.
 * key     The key to use with the HMAC.
 * secret  The derived secret.
 * side    The side that we are deriving the secret for.
 * returns 0 on success, otherwise failure.
 */
static int DeriveFinishedSecret(WOLFSSL* ssl, byte* key, byte* secret,
                                int side)
{
    WOLFSSL_MSG("Derive Finished Secret");
    return Tls13DeriveKey(ssl, secret, -1, key, finishedLabel,
                          FINISHED_LABEL_SZ,  ssl->specs.mac_algorithm, 0,
                          side);
}

/* The length of the application traffic label. */
#define APP_TRAFFIC_LABEL_SZ        11
/* The application traffic label. */
static const byte appTrafficLabel[APP_TRAFFIC_LABEL_SZ + 1] =
    "traffic upd";

/* Update the traffic secret.
 *
 * ssl     The SSL/TLS object.
 * secret  The previous secret and derived secret.
 * side    The side that we are deriving the secret for.
 * returns 0 on success, otherwise failure.
 */
static int DeriveTrafficSecret(WOLFSSL* ssl, byte* secret, int side)
{
    WOLFSSL_MSG("Derive New Application Traffic Secret");
    return Tls13DeriveKey(ssl, secret, -1, secret,
                     appTrafficLabel, APP_TRAFFIC_LABEL_SZ,
                     ssl->specs.mac_algorithm, 0, side);
}


static int Tls13_HKDF_Extract(WOLFSSL *ssl, byte* prk, const byte* salt, int saltLen,
                                 byte* ikm, int ikmLen, int digest)
{
    int ret;
#ifdef HAVE_PK_CALLBACKS
    void *cb_ctx = ssl->HkdfExtractCtx;
    CallbackHKDFExtract cb = ssl->ctx->HkdfExtractCb;
    if (cb != NULL) {
        ret = cb(prk, salt, saltLen, ikm, ikmLen, digest, cb_ctx);
    }
    else
#endif
    {
        (void)ssl;
        ret = wc_Tls13_HKDF_Extract(prk, salt, saltLen, ikm, ikmLen, digest);
    }
    return ret;
}

/* Derive the early secret using HKDF Extract.
 *
 * ssl  The SSL/TLS object.
 */
int DeriveEarlySecret(WOLFSSL* ssl)
{
    int ret;

    WOLFSSL_MSG("Derive Early Secret");
    if (ssl == NULL || ssl->arrays == NULL) {
        return BAD_FUNC_ARG;
    }
#if defined(WOLFSSL_RENESAS_TSIP_TLS)
    ret = tsip_Tls13DeriveEarlySecret(ssl);
    if (ret != CRYPTOCB_UNAVAILABLE)
        return ret;
#endif
    PRIVATE_KEY_UNLOCK();
#if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
    ret = Tls13_HKDF_Extract(ssl, ssl->arrays->secret, NULL, 0,
            ssl->arrays->psk_key, ssl->arrays->psk_keySz,
            mac2hash(ssl->specs.mac_algorithm));
#else
    ret = Tls13_HKDF_Extract(ssl, ssl->arrays->secret, NULL, 0,
            ssl->arrays->masterSecret, 0, mac2hash(ssl->specs.mac_algorithm));
#endif
    PRIVATE_KEY_LOCK();
    return ret;
}

/* The length of the derived label. */
#define DERIVED_LABEL_SZ        7
/* The derived label. */
static const byte derivedLabel[DERIVED_LABEL_SZ + 1] =
    "derived";

/* Derive the handshake secret using HKDF Extract.
 *
 * ssl  The SSL/TLS object.
 */
int DeriveHandshakeSecret(WOLFSSL* ssl)
{
    byte key[WC_MAX_DIGEST_SIZE];
    int ret;
    WOLFSSL_MSG("Derive Handshake Secret");
    if (ssl == NULL || ssl->arrays == NULL) {
        return BAD_FUNC_ARG;
    }
#if defined(WOLFSSL_RENESAS_TSIP_TLS)
    ret = tsip_Tls13DeriveHandshakeSecret(ssl);
    if (ret != CRYPTOCB_UNAVAILABLE)
        return ret;
#endif

    ret = DeriveKeyMsg(ssl, key, -1, ssl->arrays->secret,
                        derivedLabel, DERIVED_LABEL_SZ,
                        NULL, 0, ssl->specs.mac_algorithm);
    if (ret != 0)
        return ret;

    PRIVATE_KEY_UNLOCK();
    ret = Tls13_HKDF_Extract(ssl, ssl->arrays->preMasterSecret,
            key, ssl->specs.hash_size,
            ssl->arrays->preMasterSecret, ssl->arrays->preMasterSz,
            mac2hash(ssl->specs.mac_algorithm));
    PRIVATE_KEY_LOCK();

    return ret;
}

/* Derive the master secret using HKDF Extract.
 *
 * ssl  The SSL/TLS object.
 */
int DeriveMasterSecret(WOLFSSL* ssl)
{
    byte key[WC_MAX_DIGEST_SIZE];
    int ret;
    WOLFSSL_MSG("Derive Master Secret");
    if (ssl == NULL || ssl->arrays == NULL) {
        return BAD_FUNC_ARG;
    }

#if defined(WOLFSSL_RENESAS_TSIP_TLS)
    ret = tsip_Tls13DeriveMasterSecret(ssl);
    if (ret != CRYPTOCB_UNAVAILABLE)
        return ret;
#endif

    ret = DeriveKeyMsg(ssl, key, -1, ssl->arrays->preMasterSecret,
                        derivedLabel, DERIVED_LABEL_SZ,
                        NULL, 0, ssl->specs.mac_algorithm);
    if (ret != 0)
        return ret;

    PRIVATE_KEY_UNLOCK();
    ret = Tls13_HKDF_Extract(ssl, ssl->arrays->masterSecret,
            key, ssl->specs.hash_size,
            ssl->arrays->masterSecret, 0, mac2hash(ssl->specs.mac_algorithm));
    PRIVATE_KEY_LOCK();

#ifdef HAVE_KEYING_MATERIAL
    if (ret != 0)
        return ret;
    /* Calculate exporter secret only when saving arrays */
    if (ssl->options.saveArrays)
        ret = DeriveExporterSecret(ssl, ssl->arrays->exporterSecret);
#endif

    return ret;
}

#if defined(HAVE_SESSION_TICKET)
/* Length of the resumption label. */
#define RESUMPTION_LABEL_SZ         10
/* Resumption label for generating PSK associated with the ticket. */
static const byte resumptionLabel[RESUMPTION_LABEL_SZ+1] = "resumption";

/* Derive the PSK associated with the ticket.
 *
 * ssl       The SSL/TLS object.
 * nonce     The nonce to derive with.
 * nonceLen  The length of the nonce to derive with.
 * secret    The derived secret.
 * returns 0 on success, otherwise failure.
 */
int DeriveResumptionPSK(WOLFSSL* ssl, byte* nonce, byte nonceLen, byte* secret)
{
    int         digestAlg;
    /* Only one protocol version defined at this time. */
    const byte* protocol    = tls13ProtocolLabel;
    word32      protocolLen = TLS13_PROTOCOL_LABEL_SZ;
    int         ret;

    WOLFSSL_MSG("Derive Resumption PSK");

#ifdef WOLFSSL_DTLS13
    if (ssl->options.dtls) {
        protocol = dtls13ProtocolLabel;
        protocolLen = DTLS13_PROTOCOL_LABEL_SZ;
    }
#endif /* WOLFSSL_DTLS13 */

    switch (ssl->specs.mac_algorithm) {
        #ifndef NO_SHA256
        case sha256_mac:
            digestAlg = WC_SHA256;
            break;
        #endif

        #ifdef WOLFSSL_SHA384
        case sha384_mac:
            digestAlg = WC_SHA384;
            break;
        #endif

        #ifdef WOLFSSL_TLS13_SHA512
        case sha512_mac:
            digestAlg = WC_SHA512;
            break;
        #endif

        #ifdef WOLFSSL_SM3
        case sm3_mac:
            digestAlg = WC_SM3;
            break;
        #endif

        default:
            return BAD_FUNC_ARG;
    }

#if defined(WOLFSSL_TICKET_NONCE_MALLOC) &&                                    \
    (!defined(HAVE_FIPS) || (defined(FIPS_VERSION_GE) && FIPS_VERSION_GE(5,3)))
    PRIVATE_KEY_UNLOCK();
    ret = wc_Tls13_HKDF_Expand_Label_Alloc(secret, ssl->specs.hash_size,
        ssl->session->masterSecret, ssl->specs.hash_size, protocol, protocolLen,
        resumptionLabel, RESUMPTION_LABEL_SZ, nonce, nonceLen, digestAlg,
        ssl->heap);
    PRIVATE_KEY_LOCK();
#else
    ret = Tls13HKDFExpandLabel(ssl, secret, ssl->specs.hash_size,
                               ssl->session->masterSecret, ssl->specs.hash_size,
                               protocol, protocolLen, resumptionLabel,
                               RESUMPTION_LABEL_SZ, nonce, nonceLen, digestAlg);
#endif /* !defined(HAVE_FIPS) || FIPS_VERSION_GE(5,3) */
    return ret;
}
#endif /* HAVE_SESSION_TICKET */


/* Calculate the HMAC of message data to this point.
 *
 * ssl   The SSL/TLS object.
 * key   The HMAC key.
 * hash  The hash result - verify data.
 * returns length of verify data generated.
 */
static int BuildTls13HandshakeHmac(WOLFSSL* ssl, byte* key, byte* hash,
    word32* pHashSz)
{
#ifdef WOLFSSL_SMALL_STACK
    Hmac* verifyHmac;
#else
    Hmac  verifyHmac[1];
#endif
    int  hashType = WC_SHA256;
    int  hashSz = WC_SHA256_DIGEST_SIZE;
    int  ret = BAD_FUNC_ARG;

    if (ssl == NULL || key == NULL || hash == NULL) {
        return BAD_FUNC_ARG;
    }

    /* Get the hash of the previous handshake messages. */
    switch (ssl->specs.mac_algorithm) {
    #ifndef NO_SHA256
        case sha256_mac:
            hashType = WC_SHA256;
            hashSz = WC_SHA256_DIGEST_SIZE;
            ret = wc_Sha256GetHash(&ssl->hsHashes->hashSha256, hash);
            break;
    #endif /* !NO_SHA256 */
    #ifdef WOLFSSL_SHA384
        case sha384_mac:
            hashType = WC_SHA384;
            hashSz = WC_SHA384_DIGEST_SIZE;
            ret = wc_Sha384GetHash(&ssl->hsHashes->hashSha384, hash);
            break;
    #endif /* WOLFSSL_SHA384 */
    #ifdef WOLFSSL_TLS13_SHA512
        case sha512_mac:
            hashType = WC_SHA512;
            hashSz = WC_SHA512_DIGEST_SIZE;
            ret = wc_Sha512GetHash(&ssl->hsHashes->hashSha512, hash);
            break;
    #endif /* WOLFSSL_TLS13_SHA512 */
    #ifdef WOLFSSL_SM3
        case sm3_mac:
            hashType = WC_SM3;
            hashSz = WC_SM3_DIGEST_SIZE;
            ret = wc_Sm3GetHash(&ssl->hsHashes->hashSm3, hash);
            break;
    #endif /* WOLFSSL_SM3 */
        default:
            break;
    }
    if (ret != 0)
        return ret;

#ifdef WOLFSSL_DEBUG_TLS
    WOLFSSL_MSG("  Key");
    WOLFSSL_BUFFER(key, ssl->specs.hash_size);
    WOLFSSL_MSG("  Msg Hash");
    WOLFSSL_BUFFER(hash, hashSz);
#endif

#ifdef WOLFSSL_SMALL_STACK
    verifyHmac = (Hmac*)XMALLOC(sizeof(Hmac), NULL, DYNAMIC_TYPE_HMAC);
    if (verifyHmac == NULL) {
        return MEMORY_E;
    }
#endif

    /* Calculate the verify data. */
    ret = wc_HmacInit(verifyHmac, ssl->heap, ssl->devId);
    if (ret == 0) {
        ret = wc_HmacSetKey(verifyHmac, hashType, key, ssl->specs.hash_size);
        if (ret == 0)
            ret = wc_HmacUpdate(verifyHmac, hash, hashSz);
        if (ret == 0)
            ret = wc_HmacFinal(verifyHmac, hash);
        wc_HmacFree(verifyHmac);
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(verifyHmac, NULL, DYNAMIC_TYPE_HMAC);
#endif

#ifdef WOLFSSL_DEBUG_TLS
    WOLFSSL_MSG("  Hash");
    WOLFSSL_BUFFER(hash, hashSz);
#endif

    if (pHashSz)
        *pHashSz = hashSz;

    return ret;
}

/* The length of the label to use when deriving keys. */
#define WRITE_KEY_LABEL_SZ     3
/* The length of the label to use when deriving IVs. */
#define WRITE_IV_LABEL_SZ      2
/* The label to use when deriving keys. */
static const byte writeKeyLabel[WRITE_KEY_LABEL_SZ+1] = "key";
/* The label to use when deriving IVs. */
static const byte writeIVLabel[WRITE_IV_LABEL_SZ+1]   = "iv";

/* Derive the keys and IVs for TLS v1.3.
 *
 * ssl      The SSL/TLS object.
 * secret   early_data_key when deriving the key and IV for encrypting early
 *          data application data and end_of_early_data messages.
 *          handshake_key when deriving keys and IVs for encrypting handshake
 *          messages.
 *          traffic_key when deriving first keys and IVs for encrypting
 *          traffic messages.
 *          update_traffic_key when deriving next keys and IVs for encrypting
 *          traffic messages.
 * side     ENCRYPT_SIDE_ONLY when only encryption secret needs to be derived.
 *          DECRYPT_SIDE_ONLY when only decryption secret needs to be derived.
 *          ENCRYPT_AND_DECRYPT_SIDE when both secret needs to be derived.
 * store    1 indicates to derive the keys and IVs from derived secret and
 *          store ready for provisioning.
 * returns 0 on success, otherwise failure.
 */
int DeriveTls13Keys(WOLFSSL* ssl, int secret, int side, int store)
{
    int   ret = BAD_FUNC_ARG; /* Assume failure */
    int   i = 0;
#ifdef WOLFSSL_SMALL_STACK
    byte* key_dig;
#else
    byte  key_dig[MAX_PRF_DIG];
#endif
    int   provision;

#if defined(WOLFSSL_RENESAS_TSIP_TLS)
    ret = tsip_Tls13DeriveKeys(ssl, secret, side);
    if (ret != CRYPTOCB_UNAVAILABLE) {
        return ret;
    }
    ret = BAD_FUNC_ARG; /* Assume failure */
#endif

#ifdef WOLFSSL_SMALL_STACK
    key_dig = (byte*)XMALLOC(MAX_PRF_DIG, ssl->heap, DYNAMIC_TYPE_DIGEST);
    if (key_dig == NULL)
        return MEMORY_E;
#endif

    if (side == ENCRYPT_AND_DECRYPT_SIDE) {
        provision = PROVISION_CLIENT_SERVER;
    }
    else {
        provision = ((ssl->options.side != WOLFSSL_CLIENT_END) ^
                     (side == ENCRYPT_SIDE_ONLY)) ? PROVISION_CLIENT :
                                                    PROVISION_SERVER;
    }

    /* Derive the appropriate secret to use in the HKDF. */
    switch (secret) {
#ifdef WOLFSSL_EARLY_DATA
        case early_data_key:
            ret = DeriveEarlyTrafficSecret(ssl, ssl->clientSecret,
                                           WOLFSSL_CLIENT_END);
            if (ret != 0)
                goto end;
            break;
#endif

        case handshake_key:
            if (provision & PROVISION_CLIENT) {
                ret = DeriveClientHandshakeSecret(ssl,
                                                  ssl->clientSecret);
                if (ret != 0)
                    goto end;
            }
            if (provision & PROVISION_SERVER) {
                ret = DeriveServerHandshakeSecret(ssl,
                                                  ssl->serverSecret);
                if (ret != 0)
                    goto end;
            }
            break;

        case traffic_key:
            if (provision & PROVISION_CLIENT) {
                ret = DeriveClientTrafficSecret(ssl, ssl->clientSecret);
                if (ret != 0)
                    goto end;
            }
            if (provision & PROVISION_SERVER) {
                ret = DeriveServerTrafficSecret(ssl, ssl->serverSecret);
                if (ret != 0)
                    goto end;
            }
            break;

        case update_traffic_key:
            if (provision & PROVISION_CLIENT) {
                ret = DeriveTrafficSecret(ssl, ssl->clientSecret,
                                          WOLFSSL_CLIENT_END);
                if (ret != 0)
                    goto end;
            }
            if (provision & PROVISION_SERVER) {
                ret = DeriveTrafficSecret(ssl, ssl->serverSecret,
                                          WOLFSSL_SERVER_END);
                if (ret != 0)
                    goto end;
            }
            break;

        default:
            break;
    }

#ifdef WOLFSSL_QUIC
    if (WOLFSSL_IS_QUIC(ssl)) {
        ret = wolfSSL_quic_forward_secrets(ssl, secret, side);
        if (ret != 0)
            goto end;
    }
#endif /* WOLFSSL_QUIC */

    if (!store)
        goto end;

    /* Key data = client key | server key | client IV | server IV */

    if (provision & PROVISION_CLIENT) {
        /* Derive the client key.  */
        WOLFSSL_MSG("Derive Client Key");
        ret = Tls13DeriveKey(ssl, &key_dig[i], ssl->specs.key_size,
                        ssl->clientSecret, writeKeyLabel,
                        WRITE_KEY_LABEL_SZ, ssl->specs.mac_algorithm, 0,
                        WOLFSSL_CLIENT_END);
        if (ret != 0)
            goto end;
        i += ssl->specs.key_size;
    }

    if (provision & PROVISION_SERVER) {
        /* Derive the server key.  */
        WOLFSSL_MSG("Derive Server Key");
        ret = Tls13DeriveKey(ssl, &key_dig[i], ssl->specs.key_size,
                        ssl->serverSecret, writeKeyLabel,
                        WRITE_KEY_LABEL_SZ, ssl->specs.mac_algorithm, 0,
                        WOLFSSL_SERVER_END);
        if (ret != 0)
            goto end;
        i += ssl->specs.key_size;
    }

    if (provision & PROVISION_CLIENT) {
        /* Derive the client IV.  */
        WOLFSSL_MSG("Derive Client IV");
        ret = Tls13DeriveKey(ssl, &key_dig[i], ssl->specs.iv_size,
                        ssl->clientSecret, writeIVLabel,
                        WRITE_IV_LABEL_SZ, ssl->specs.mac_algorithm, 0,
                        WOLFSSL_CLIENT_END);
        if (ret != 0)
            goto end;
        i += ssl->specs.iv_size;
    }

    if (provision & PROVISION_SERVER) {
        /* Derive the server IV.  */
        WOLFSSL_MSG("Derive Server IV");
        ret = Tls13DeriveKey(ssl, &key_dig[i], ssl->specs.iv_size,
                        ssl->serverSecret, writeIVLabel,
                        WRITE_IV_LABEL_SZ, ssl->specs.mac_algorithm, 0,
                        WOLFSSL_SERVER_END);
        if (ret != 0)
            goto end;
        i += ssl->specs.iv_size;
    }

    /* Store keys and IVs but don't activate them. */
    ret = StoreKeys(ssl, key_dig, provision);

#ifdef WOLFSSL_DTLS13
    if (ret != 0)
      goto end;

    if (ssl->options.dtls) {
        ret = Dtls13DeriveSnKeys(ssl, provision);
        if (ret != 0)
            return ret;
    }

#endif /* WOLFSSL_DTLS13 */

end:
    ForceZero(key_dig, i);
#ifdef WOLFSSL_SMALL_STACK
    XFREE(key_dig, ssl->heap, DYNAMIC_TYPE_DIGEST);
#elif defined(WOLFSSL_CHECK_MEM_ZERO)
    wc_MemZero_Check(key_dig, MAX_PRF_DIG);
#endif

    if (ret != 0) {
        WOLFSSL_ERROR_VERBOSE(ret);
    }

    return ret;
}

#if (defined(HAVE_SESSION_TICKET) || !defined(NO_PSK))
#ifdef WOLFSSL_32BIT_MILLI_TIME
#ifndef NO_ASN_TIME
#if defined(USER_TICKS)
#if 0
    word32 TimeNowInMilliseconds(void)
    {
        /*
        write your own clock tick function if don't want gettimeofday()
        needs millisecond accuracy but doesn't have to correlated to EPOCH
        */
    }
#endif

#elif defined(TIME_OVERRIDES)
#if !defined(NO_ASN) && !defined(NO_ASN_TIME)
    word32 TimeNowInMilliseconds(void)
    {
        return (word32) wc_Time(0) * 1000;
    }
#else
    #ifndef HAVE_TIME_T_TYPE
        typedef long time_t;
    #endif
    extern time_t XTIME(time_t * timer);

    /* The time in milliseconds.
     * Used for tickets to represent difference between when first seen and when
     * sending.
     *
     * returns the time in milliseconds as a 32-bit value.
     */
    word32 TimeNowInMilliseconds(void)
    {
        return (word32) XTIME(0) * 1000;
    }
#endif

#elif defined(XTIME_MS)
    word32 TimeNowInMilliseconds(void)
    {
        return (word32)XTIME_MS(0);
    }

#elif defined(USE_WINDOWS_API)
    /* The time in milliseconds.
     * Used for tickets to represent difference between when first seen and when
     * sending.
     *
     * returns the time in milliseconds as a 32-bit value.
     */
    word32 TimeNowInMilliseconds(void)
    {
        static int           init = 0;
        static LARGE_INTEGER freq;
        LARGE_INTEGER        count;

        if (!init) {
            QueryPerformanceFrequency(&freq);
            init = 1;
        }

        QueryPerformanceCounter(&count);

        return (word32)(count.QuadPart / (freq.QuadPart / 1000));
    }

#elif defined(HAVE_RTP_SYS)
    #include "rtptime.h"

    /* The time in milliseconds.
     * Used for tickets to represent difference between when first seen and when
     * sending.
     *
     * returns the time in milliseconds as a 32-bit value.
     */
    word32 TimeNowInMilliseconds(void)
    {
        return (word32)rtp_get_system_sec() * 1000;
    }
#elif defined(WOLFSSL_DEOS)
    word32 TimeNowInMilliseconds(void)
    {
        const word32 systemTickTimeInHz = 1000000 / systemTickInMicroseconds();
        word32 *systemTickPtr = systemTickPointer();

        return (word32) (*systemTickPtr/systemTickTimeInHz) * 1000;
    }
#elif defined(MICRIUM)
    /* The time in milliseconds.
     * Used for tickets to represent difference between when first seen and when
     * sending.
     *
     * returns the time in milliseconds as a 32-bit value.
     */
    word32 TimeNowInMilliseconds(void)
    {
        OS_TICK ticks = 0;
        OS_ERR  err;

        ticks = OSTimeGet(&err);

        return (word32) (ticks / OSCfg_TickRate_Hz) * 1000;
    }
#elif defined(MICROCHIP_TCPIP_V5)
    /* The time in milliseconds.
     * Used for tickets to represent difference between when first seen and when
     * sending.
     *
     * returns the time in milliseconds as a 32-bit value.
     */
    word32 TimeNowInMilliseconds(void)
    {
        return (word32) (TickGet() / (TICKS_PER_SECOND / 1000));
    }
#elif defined(MICROCHIP_TCPIP)
    #if defined(MICROCHIP_MPLAB_HARMONY)
        #include <system/tmr/sys_tmr.h>

    /* The time in milliseconds.
     * Used for tickets to represent difference between when first seen and when
     * sending.
     *
     * returns the time in milliseconds as a 32-bit value.
     */
    word32 TimeNowInMilliseconds(void)
    {
        return (word32)(SYS_TMR_TickCountGet() /
                        (SYS_TMR_TickCounterFrequencyGet() / 1000));
    }
    #else
    /* The time in milliseconds.
     * Used for tickets to represent difference between when first seen and when
     * sending.
     *
     * returns the time in milliseconds as a 32-bit value.
     */
    word32 TimeNowInMilliseconds(void)
    {
        return (word32)(SYS_TICK_Get() / (SYS_TICK_TicksPerSecondGet() / 1000));
    }

    #endif

#elif defined(FREESCALE_MQX) || defined(FREESCALE_KSDK_MQX)
    /* The time in milliseconds.
     * Used for tickets to represent difference between when first seen and when
     * sending.
     *
     * returns the time in milliseconds as a 32-bit value.
     */
    word32 TimeNowInMilliseconds(void)
    {
        TIME_STRUCT mqxTime;

        _time_get_elapsed(&mqxTime);

        return (word32) mqxTime.SECONDS * 1000;
    }
#elif defined(FREESCALE_FREE_RTOS) || defined(FREESCALE_KSDK_FREERTOS)
    #include "include/task.h"

    /* The time in milliseconds.
     * Used for tickets to represent difference between when first seen and when
     * sending.
     *
     * returns the time in milliseconds as a 32-bit value.
     */
    word32 TimeNowInMilliseconds(void)
    {
        return (unsigned int)(((float)xTaskGetTickCount()) /
                              (configTICK_RATE_HZ / 1000));
    }
#elif defined(FREESCALE_KSDK_BM)
    #include "lwip/sys.h" /* lwIP */

    /* The time in milliseconds.
     * Used for tickets to represent difference between when first seen and when
     * sending.
     *
     * returns the time in milliseconds as a 32-bit value.
     */
    word32 TimeNowInMilliseconds(void)
    {
        return sys_now();
    }

#elif defined(WOLFSSL_CMSIS_RTOS) || defined(WOLFSSL_CMSIS_RTOSv2)

    word32 TimeNowInMilliseconds(void)
    {
        return (word32)osKernelGetTickCount();
    }

#elif defined(WOLFSSL_TIRTOS)
    /* The time in milliseconds.
     * Used for tickets to represent difference between when first seen and when
     * sending.
     *
     * returns the time in milliseconds as a 32-bit value.
     */
    word32 TimeNowInMilliseconds(void)
    {
        return (word32) Seconds_get() * 1000;
    }
#elif defined(WOLFSSL_UTASKER)
    /* The time in milliseconds.
     * Used for tickets to represent difference between when first seen and when
     * sending.
     *
     * returns the time in milliseconds as a 32-bit value.
     */
    word32 TimeNowInMilliseconds(void)
    {
        return (word32)(uTaskerSystemTick / (TICK_RESOLUTION / 1000));
    }
#elif defined(WOLFSSL_LINUXKM)
    word32 TimeNowInMilliseconds(void)
    {
        s64 t;
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 0, 0)
        struct timespec ts;
        getnstimeofday(&ts);
        t = ts.tv_sec * (s64)1000;
        t += ts.tv_nsec / (s64)1000000;
#else
        struct timespec64 ts;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 0, 0)
        ts = current_kernel_time64();
#else
        ktime_get_coarse_real_ts64(&ts);
#endif
        t = ts.tv_sec * 1000L;
        t += ts.tv_nsec / 1000000L;
#endif
        return (word32)t;
    }
#elif defined(WOLFSSL_QNX_CAAM)
    word32 TimeNowInMilliseconds(void)
    {
        struct timespec now;
        clock_gettime(CLOCK_REALTIME, &now);
        return (word32)(now.tv_sec * 1000 + now.tv_nsec / 1000000);
    }
#elif defined(FUSION_RTOS)
    /* The time in milliseconds.
     * Used for tickets to represent difference between when first seen and when
     * sending.
     *
     * returns the time in milliseconds as a 32-bit value.
     */
    word32 TimeNowInMilliseconds(void)
    {
        struct timeval now;
        if (FCL_GETTIMEOFDAY(&now, 0) < 0)
            return 0;

        /* Convert to milliseconds number. */
        return (word32)(now.tv_sec * 1000 + now.tv_usec / 1000);
    }
#elif defined(WOLFSSL_ZEPHYR)
    word32 TimeNowInMilliseconds(void)
    {
    #if defined(CONFIG_ARCH_POSIX)
        k_cpu_idle();
    #endif
        return (word32)k_uptime_get() / 1000;
    }

#else
    /* The time in milliseconds.
     * Used for tickets to represent difference between when first seen and when
     * sending.
     *
     * returns the time in milliseconds as a 32-bit value.
     */
    word32 TimeNowInMilliseconds(void)
    {
        struct timeval now;

        if (gettimeofday(&now, 0) < 0)
            return 0;

        /* Convert to milliseconds number. */
        return (word32)(now.tv_sec * 1000 + now.tv_usec / 1000);
    }
#endif
#else
    /* user must supply time in milliseconds function:
     *   word32 TimeNowInMilliseconds(void);
     * The response is milliseconds elapsed
     */
#endif /* !NO_ASN_TIME */
#else
#ifndef NO_ASN_TIME
#if defined(USER_TICKS)
#if 0
    sword64 TimeNowInMilliseconds(void)
    {
        /*
        write your own clock tick function if don't want gettimeofday()
        needs millisecond accuracy but doesn't have to correlated to EPOCH
        */
    }
#endif

#elif defined(TIME_OVERRIDES)
#if !defined(NO_ASN) && !defined(NO_ASN_TIME)
    sword64 TimeNowInMilliseconds(void)
    {
        return (sword64) wc_Time(0) * 1000;
    }
#else
    #ifndef HAVE_TIME_T_TYPE
        typedef long time_t;
    #endif
    extern time_t XTIME(time_t * timer);

    /* The time in milliseconds.
     * Used for tickets to represent difference between when first seen and when
     * sending.
     *
     * returns the time in milliseconds as a 32-bit value.
     */
    sword64 TimeNowInMilliseconds(void)
    {
        return (sword64) XTIME(0) * 1000;
    }
#endif

#elif defined(XTIME_MS)
    sword64 TimeNowInMilliseconds(void)
    {
        return (sword64)XTIME_MS(0);
    }

#elif defined(USE_WINDOWS_API)
    /* The time in milliseconds.
     * Used for tickets to represent difference between when first seen and when
     * sending.
     *
     * returns the time in milliseconds as a 64-bit value.
     */
    sword64 TimeNowInMilliseconds(void)
    {
        static int           init = 0;
        static LARGE_INTEGER freq;
        LARGE_INTEGER        count;

        if (!init) {
            QueryPerformanceFrequency(&freq);
            init = 1;
        }

        QueryPerformanceCounter(&count);

        return (sword64)(count.QuadPart / (freq.QuadPart / 1000));
    }

#elif defined(HAVE_RTP_SYS)
    #include "rtptime.h"

    /* The time in milliseconds.
     * Used for tickets to represent difference between when first seen and when
     * sending.
     *
     * returns the time in milliseconds as a 64-bit value.
     */
    sword64 TimeNowInMilliseconds(void)
    {
        return (sword64)rtp_get_system_sec() * 1000;
    }
#elif defined(WOLFSSL_DEOS)
    sword64 TimeNowInMilliseconds(void)
    {
        const word32 systemTickTimeInHz = 1000000 / systemTickInMicroseconds();
        word32 *systemTickPtr = systemTickPointer();

        return (sword64) (*systemTickPtr/systemTickTimeInHz) * 1000;
    }
#elif defined(MICRIUM)
    /* The time in milliseconds.
     * Used for tickets to represent difference between when first seen and when
     * sending.
     *
     * returns the time in milliseconds as a 64-bit value.
     */
    sword64 TimeNowInMilliseconds(void)
    {
        OS_TICK ticks = 0;
        OS_ERR  err;

        ticks = OSTimeGet(&err);

        return (sword64) (ticks / OSCfg_TickRate_Hz) * 1000;
    }
#elif defined(MICROCHIP_TCPIP_V5)
    /* The time in milliseconds.
     * Used for tickets to represent difference between when first seen and when
     * sending.
     *
     * returns the time in milliseconds as a 64-bit value.
     */
    sword64 TimeNowInMilliseconds(void)
    {
        return (sword64) (TickGet() / (TICKS_PER_SECOND / 1000));
    }
#elif defined(MICROCHIP_TCPIP)
    #if defined(MICROCHIP_MPLAB_HARMONY)
        #include <system/tmr/sys_tmr.h>

    /* The time in milliseconds.
     * Used for tickets to represent difference between when first seen and when
     * sending.
     *
     * returns the time in milliseconds as a 64-bit value.
     */
    sword64 TimeNowInMilliseconds(void)
    {
        return (sword64)SYS_TMR_TickCountGet() /
                        (SYS_TMR_TickCounterFrequencyGet() / 1000);
    }
    #else
    /* The time in milliseconds.
     * Used for tickets to represent difference between when first seen and when
     * sending.
     *
     * returns the time in milliseconds as a 64-bit value.
     */
    sword64 TimeNowInMilliseconds(void)
    {
        return (sword64)SYS_TICK_Get() / (SYS_TICK_TicksPerSecondGet() / 1000);
    }

    #endif

#elif defined(FREESCALE_MQX) || defined(FREESCALE_KSDK_MQX)
    /* The time in milliseconds.
     * Used for tickets to represent difference between when first seen and when
     * sending.
     *
     * returns the time in milliseconds as a 64-bit value.
     */
    sword64 TimeNowInMilliseconds(void)
    {
        TIME_STRUCT mqxTime;

        _time_get_elapsed(&mqxTime);

        return (sword64) mqxTime.SECONDS * 1000;
    }
#elif defined(FREESCALE_FREE_RTOS) || defined(FREESCALE_KSDK_FREERTOS)
    #include "include/task.h"

    /* The time in milliseconds.
     * Used for tickets to represent difference between when first seen and when
     * sending.
     *
     * returns the time in milliseconds as a 64-bit value.
     */
    sword64 TimeNowInMilliseconds(void)
    {
        return (sword64)xTaskGetTickCount() / (configTICK_RATE_HZ / 1000);
    }
#elif defined(FREESCALE_KSDK_BM)
    #include "lwip/sys.h" /* lwIP */

    /* The time in milliseconds.
     * Used for tickets to represent difference between when first seen and when
     * sending.
     *
     * returns the time in milliseconds as a 64-bit value.
     */
    sword64 TimeNowInMilliseconds(void)
    {
        return sys_now();
    }

#elif defined(WOLFSSL_CMSIS_RTOS) || defined(WOLFSSL_CMSIS_RTOSv2)

    sword64 TimeNowInMilliseconds(void)
    {
        return (sword64)osKernelGetTickCount();
    }

#elif defined(WOLFSSL_TIRTOS)
    /* The time in milliseconds.
     * Used for tickets to represent difference between when first seen and when
     * sending.
     *
     * returns the time in milliseconds as a 64-bit value.
     */
    sword64 TimeNowInMilliseconds(void)
    {
        return (sword64) Seconds_get() * 1000;
    }
#elif defined(WOLFSSL_UTASKER)
    /* The time in milliseconds.
     * Used for tickets to represent difference between when first seen and when
     * sending.
     *
     * returns the time in milliseconds as a 64-bit value.
     */
    sword64 TimeNowInMilliseconds(void)
    {
        return (sword64)(uTaskerSystemTick / (TICK_RESOLUTION / 1000));
    }
#elif defined(WOLFSSL_LINUXKM)
    sword64 TimeNowInMilliseconds(void)
    {
        s64 t;
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 0, 0)
        struct timespec ts;
        getnstimeofday(&ts);
        t = ts.tv_sec * (s64)1000;
        t += ts.tv_nsec / (s64)1000000;
#else
        struct timespec64 ts;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 0, 0)
        ts = current_kernel_time64();
#else
        ktime_get_coarse_real_ts64(&ts);
#endif
        t = ts.tv_sec * 1000L;
        t += ts.tv_nsec / 1000000L;
#endif
        return (sword64)t;
    }
#elif defined(WOLFSSL_QNX_CAAM)
    sword64 TimeNowInMilliseconds(void)
    {
        struct timespec now;
        clock_gettime(CLOCK_REALTIME, &now);
        return (sword64)(now.tv_sec * 1000 + now.tv_nsec / 1000000);
    }
#elif defined(FUSION_RTOS)
    /* The time in milliseconds.
     * Used for tickets to represent difference between when first seen and when
     * sending.
     *
     * returns the time in milliseconds as a 64-bit value.
     */
    sword64 TimeNowInMilliseconds(void)
    {
        struct timeval now;
        if (FCL_GETTIMEOFDAY(&now, 0) < 0)
            return 0;

        /* Convert to milliseconds number. */
        return (sword64)now.tv_sec * 1000 + now.tv_usec / 1000;
    }
#elif defined(WOLFSSL_ZEPHYR)
    sword64 TimeNowInMilliseconds(void)
    {
    #if defined(CONFIG_ARCH_POSIX)
        k_cpu_idle();
    #endif
        return (sword64)k_uptime_get() / 1000;
    }

#else
    /* The time in milliseconds.
     * Used for tickets to represent difference between when first seen and when
     * sending.
     *
     * returns the time in milliseconds as a 64-bit value.
     */
    sword64 TimeNowInMilliseconds(void)
    {
        struct timeval now;

        if (gettimeofday(&now, 0) < 0)
            return 0;

        /* Convert to milliseconds number. */
        return (sword64)now.tv_sec * 1000 + now.tv_usec / 1000;
    }
#endif
#else
    /* user must supply time in milliseconds function:
     *   sword64 TimeNowInMilliseconds(void);
     * The response is milliseconds elapsed
     */
#endif /* !NO_ASN_TIME */
#endif /* WOLFSSL_32BIT_MILLI_TIME */
#endif /* HAVE_SESSION_TICKET || !NO_PSK */


/* Extract the handshake header information.
 *
 * ssl       The SSL/TLS object.
 * input     The buffer holding the message data.
 * inOutIdx  On entry, the index into the buffer of the handshake data.
 *           On exit, the start of the handshake data.
 * type      Type of handshake message.
 * size      The length of the handshake message data.
 * totalSz   The total size of data in the buffer.
 * returns BUFFER_E if there is not enough input data and 0 on success.
 */
static int GetHandshakeHeader(WOLFSSL* ssl, const byte* input, word32* inOutIdx,
                              byte* type, word32* size, word32 totalSz)
{
    const byte* ptr = input + *inOutIdx;
    (void)ssl;

    *inOutIdx += HANDSHAKE_HEADER_SZ;
    if (*inOutIdx > totalSz)
        return BUFFER_E;

    *type = ptr[0];
    c24to32(&ptr[1], size);

    return 0;
}

/* Add record layer header to message.
 *
 * output  The buffer to write the record layer header into.
 * length  The length of the record data.
 * type    The type of record message.
 * ssl     The SSL/TLS object.
 */
static void AddTls13RecordHeader(byte* output, word32 length, byte type,
                                 WOLFSSL* ssl)
{
    RecordLayerHeader* rl;

    rl = (RecordLayerHeader*)output;
    rl->type    = type;
    rl->pvMajor = ssl->version.major;
    /* NOTE: May be TLSv1_MINOR when sending first ClientHello. */
    rl->pvMinor = TLSv1_2_MINOR;
    c16toa((word16)length, rl->length);
}

/* Add handshake header to message.
 *
 * output      The buffer to write the handshake header into.
 * length      The length of the handshake data.
 * fragOffset  The offset of the fragment data. (DTLS)
 * fragLength  The length of the fragment data. (DTLS)
 * type        The type of handshake message.
 * ssl         The SSL/TLS object. (DTLS)
 */
static void AddTls13HandShakeHeader(byte* output, word32 length,
                                    word32 fragOffset, word32 fragLength,
                                    byte type, WOLFSSL* ssl)
{
    HandShakeHeader* hs;
    (void)fragOffset;
    (void)fragLength;
    (void)ssl;

#ifdef WOLFSSL_DTLS13
    /* message_hash type is used for a synthetic message that replaces the first
       ClientHello in the hash transcript when using HelloRetryRequest. It will
       never be transmitted and, as the DTLS-only fields must not be considered
       when computing the hash transcript, we can avoid to use the DTLS
       handshake header. */
    if (ssl->options.dtls && type != message_hash) {
        Dtls13HandshakeAddHeader(ssl, output, (enum HandShakeType)type, length);
        return;
    }
#endif /* WOLFSSL_DTLS13 */

    /* handshake header */
    hs = (HandShakeHeader*)output;
    hs->type = type;
    c32to24(length, hs->length);
}


/* Add both record layer and handshake header to message.
 *
 * output      The buffer to write the headers into.
 * length      The length of the handshake data.
 * type        The type of record layer message.
 * ssl         The SSL/TLS object. (DTLS)
 */
static void AddTls13Headers(byte* output, word32 length, byte type,
                            WOLFSSL* ssl)
{
    word32 lengthAdj = HANDSHAKE_HEADER_SZ;
    word32 outputAdj = RECORD_HEADER_SZ;

#ifdef WOLFSSL_DTLS13
    if (ssl->options.dtls) {
        Dtls13AddHeaders(output, length, (enum HandShakeType)type, ssl);
        return;
    }
#endif /* WOLFSSL_DTLS13 */

    AddTls13RecordHeader(output, length + lengthAdj, handshake, ssl);
    AddTls13HandShakeHeader(output + outputAdj, length, 0, length, type, ssl);
}

#if (!defined(NO_WOLFSSL_CLIENT) || !defined(NO_WOLFSSL_SERVER)) \
    && !defined(NO_CERTS)
/* Add both record layer and fragment handshake header to message.
 *
 * output      The buffer to write the headers into.
 * fragOffset  The offset of the fragment data. (DTLS)
 * fragLength  The length of the fragment data. (DTLS)
 * length      The length of the handshake data.
 * type        The type of record layer message.
 * ssl         The SSL/TLS object. (DTLS)
 */
static void AddTls13FragHeaders(byte* output, word32 fragSz, word32 fragOffset,
                                word32 length, byte type, WOLFSSL* ssl)
{
    word32 lengthAdj = HANDSHAKE_HEADER_SZ;
    word32 outputAdj = RECORD_HEADER_SZ;
    (void)fragSz;

#ifdef WOLFSSL_DTLS13
    /* we ignore fragmentation fields here because fragmentation logic for
       DTLS1.3 is inside dtls13_handshake_send(). */
    if (ssl->options.dtls) {
        Dtls13AddHeaders(output, length, (enum HandShakeType)type, ssl);
        return;
    }
#endif /* WOLFSSL_DTLS13 */

    AddTls13RecordHeader(output, fragSz + lengthAdj, handshake, ssl);
    AddTls13HandShakeHeader(output + outputAdj, length, fragOffset, fragSz,
                            type, ssl);
}
#endif /* (!NO_WOLFSSL_CLIENT || !NO_WOLFSSL_SERVER) && !NO_CERTS */

/* Write the sequence number into the buffer.
 * No DTLS v1.3 support.
 *
 * ssl          The SSL/TLS object.
 * verifyOrder  Which set of sequence numbers to use.
 * out          The buffer to write into.
 */
static WC_INLINE void WriteSEQTls13(WOLFSSL* ssl, int verifyOrder, byte* out)
{
    word32 seq[2] = {0, 0};

    if (ssl->options.dtls) {
#ifdef WOLFSSL_DTLS13
        Dtls13GetSeq(ssl, verifyOrder, seq, 1);
#endif /* WOLFSSL_DTLS13 */
    }
    else if (verifyOrder == PEER_ORDER) {
        seq[0] = ssl->keys.peer_sequence_number_hi;
        seq[1] = ssl->keys.peer_sequence_number_lo++;
        /* handle rollover */
        if (seq[1] > ssl->keys.peer_sequence_number_lo)
            ssl->keys.peer_sequence_number_hi++;
    }
    else {
        seq[0] = ssl->keys.sequence_number_hi;
        seq[1] = ssl->keys.sequence_number_lo++;
        /* handle rollover */
        if (seq[1] > ssl->keys.sequence_number_lo)
            ssl->keys.sequence_number_hi++;
    }

    c32toa(seq[0], out);
    c32toa(seq[1], out + OPAQUE32_LEN);
}

/* Build the nonce for TLS v1.3 encryption and decryption.
 *
 * ssl    The SSL/TLS object.
 * nonce  The nonce data to use when encrypting or decrypting.
 * iv     The derived IV.
 * order  The side on which the message is to be or was sent.
 */
static WC_INLINE void BuildTls13Nonce(WOLFSSL* ssl, byte* nonce, const byte* iv,
                                   int order)
{
    int  i;

    /* The nonce is the IV with the sequence XORed into the last bytes. */
    WriteSEQTls13(ssl, order, nonce + AEAD_NONCE_SZ - SEQ_SZ);
    for (i = 0; i < AEAD_NONCE_SZ - SEQ_SZ; i++)
        nonce[i] = iv[i];
    for (; i < AEAD_NONCE_SZ; i++)
        nonce[i] ^= iv[i];
}

#if defined(HAVE_CHACHA) && defined(HAVE_POLY1305)
/* Encrypt with ChaCha20 and create authentication tag with Poly1305.
 *
 * ssl     The SSL/TLS object.
 * output  The buffer to write encrypted data and authentication tag into.
 *         May be the same pointer as input.
 * input   The data to encrypt.
 * sz      The number of bytes to encrypt.
 * nonce   The nonce to use with ChaCha20.
 * aad     The additional authentication data.
 * aadSz   The size of the addition authentication data.
 * tag     The authentication tag buffer.
 * returns 0 on success, otherwise failure.
 */
static int ChaCha20Poly1305_Encrypt(WOLFSSL* ssl, byte* output,
                                    const byte* input, word16 sz, byte* nonce,
                                    const byte* aad, word16 aadSz, byte* tag)
{
    int    ret    = 0;
    byte   poly[CHACHA20_256_KEY_SIZE];

    /* Poly1305 key is 256 bits of zero encrypted with ChaCha20. */
    XMEMSET(poly, 0, sizeof(poly));

    /* Set the nonce for ChaCha and get Poly1305 key. */
    ret = wc_Chacha_SetIV(ssl->encrypt.chacha, nonce, 0);
    if (ret != 0)
        return ret;
    /* Create Poly1305 key using ChaCha20 keystream. */
    ret = wc_Chacha_Process(ssl->encrypt.chacha, poly, poly, sizeof(poly));
    if (ret != 0)
        return ret;
#ifdef WOLFSSL_CHECK_MEM_ZERO
    wc_MemZero_Add("ChaCha20Poly1305_Encrypt poly", poly, sizeof(poly));
#endif
    ret = wc_Chacha_SetIV(ssl->encrypt.chacha, nonce, 1);
    if (ret != 0)
        return ret;
    /* Encrypt the plain text. */
    ret = wc_Chacha_Process(ssl->encrypt.chacha, output, input, sz);
    if (ret != 0) {
        ForceZero(poly, sizeof(poly));
    #ifdef WOLFSSL_CHECK_MEM_ZERO
        wc_MemZero_Check(poly, sizeof(poly));
    #endif
        return ret;
    }

    /* Set key for Poly1305. */
    ret = wc_Poly1305SetKey(ssl->auth.poly1305, poly, sizeof(poly));
    ForceZero(poly, sizeof(poly)); /* done with poly1305 key, clear it */
#ifdef WOLFSSL_CHECK_MEM_ZERO
    wc_MemZero_Check(poly, sizeof(poly));
#endif
    if (ret != 0)
        return ret;
    /* Add authentication code of encrypted data to end. */
    ret = wc_Poly1305_MAC(ssl->auth.poly1305, aad, aadSz, output, sz, tag,
                                                              POLY1305_AUTH_SZ);

    return ret;
}
#endif

#ifdef HAVE_NULL_CIPHER
/* Create authentication tag and copy data over input.
 *
 * ssl     The SSL/TLS object.
 * output  The buffer to copy data into.
 *         May be the same pointer as input.
 * input   The data.
 * sz      The number of bytes of data.
 * nonce   The nonce to use with authentication.
 * aad     The additional authentication data.
 * aadSz   The size of the addition authentication data.
 * tag     The authentication tag buffer.
 * returns 0 on success, otherwise failure.
 */
static int Tls13IntegrityOnly_Encrypt(WOLFSSL* ssl, byte* output,
                                      const byte* input, word16 sz,
                                      const byte* nonce,
                                      const byte* aad, word16 aadSz, byte* tag)
{
    int ret;

    /* HMAC: nonce | aad | input  */
    ret = wc_HmacUpdate(ssl->encrypt.hmac, nonce, HMAC_NONCE_SZ);
    if (ret == 0)
        ret = wc_HmacUpdate(ssl->encrypt.hmac, aad, aadSz);
    if (ret == 0)
        ret = wc_HmacUpdate(ssl->encrypt.hmac, input, sz);
    if (ret == 0)
        ret = wc_HmacFinal(ssl->encrypt.hmac, tag);
    /* Copy the input to output if not the same buffer */
    if (ret == 0 && output != input)
        XMEMCPY(output, input, sz);

    return ret;
}
#endif

/* Encrypt data for TLS v1.3.
 *
 * ssl     The SSL/TLS object.
 * output  The buffer to write encrypted data and authentication tag into.
 *         May be the same pointer as input.
 * input   The record header and data to encrypt.
 * sz      The number of bytes to encrypt.
 * aad     The additional authentication data.
 * aadSz   The size of the addition authentication data.
 * asyncOkay If non-zero can return WC_PENDING_E, otherwise blocks on crypto
 * returns 0 on success, otherwise failure.
 */
static int EncryptTls13(WOLFSSL* ssl, byte* output, const byte* input,
                        word16 sz, const byte* aad, word16 aadSz, int asyncOkay)
{
    int    ret    = 0;
    word16 dataSz = sz - ssl->specs.aead_mac_size;
    word16 macSz  = ssl->specs.aead_mac_size;
    word32 nonceSz = 0;
#ifdef WOLFSSL_ASYNC_CRYPT
    WC_ASYNC_DEV* asyncDev = NULL;
    word32 event_flags = WC_ASYNC_FLAG_CALL_AGAIN;
#endif

    WOLFSSL_ENTER("EncryptTls13");

    (void)output;
    (void)input;
    (void)sz;
    (void)dataSz;
    (void)macSz;
    (void)asyncOkay;
    (void)nonceSz;

#ifdef WOLFSSL_ASYNC_CRYPT
    if (ssl->error == WC_PENDING_E) {
        ssl->error = 0; /* clear async */
    }
#endif
#if defined(WOLFSSL_RENESAS_TSIP_TLS)
    ret = tsip_Tls13AesEncrypt(ssl, output, input, dataSz);
    if (ret != CRYPTOCB_UNAVAILABLE) {
        if (ret > 0) {
            ret = 0; /* tsip_Tls13AesEncrypt returns output size */
        }
        return ret;
    }
    ret = 0;
#endif /* WOLFSSL_RENESAS_TSIP_TLS */

    switch (ssl->encrypt.state) {
        case CIPHER_STATE_BEGIN:
        {
        #ifdef WOLFSSL_DEBUG_TLS
            WOLFSSL_MSG("Data to encrypt");
            WOLFSSL_BUFFER(input, dataSz);
            WOLFSSL_MSG("Additional Authentication Data");
            WOLFSSL_BUFFER(aad, aadSz);
        #endif

        #ifdef WOLFSSL_CIPHER_TEXT_CHECK
            if (ssl->specs.bulk_cipher_algorithm != wolfssl_cipher_null) {
                XMEMCPY(ssl->encrypt.sanityCheck, input,
                    min(dataSz, sizeof(ssl->encrypt.sanityCheck)));
            }
        #endif

        #ifdef CIPHER_NONCE
            if (ssl->encrypt.nonce == NULL) {
                ssl->encrypt.nonce = (byte*)XMALLOC(AEAD_NONCE_SZ,
                                            ssl->heap, DYNAMIC_TYPE_AES_BUFFER);
            #ifdef WOLFSSL_CHECK_MEM_ZERO
                if (ssl->encrypt.nonce != NULL) {
                    wc_MemZero_Add("EncryptTls13 nonce", ssl->encrypt.nonce,
                        AEAD_NONCE_SZ);
                }
            #endif
            }
            if (ssl->encrypt.nonce == NULL)
                return MEMORY_E;

            BuildTls13Nonce(ssl, ssl->encrypt.nonce, ssl->keys.aead_enc_imp_IV,
                            CUR_ORDER);
        #endif

            /* Advance state and proceed */
            ssl->encrypt.state = CIPHER_STATE_DO;
        }
        FALL_THROUGH;

        case CIPHER_STATE_DO:
        {
            switch (ssl->specs.bulk_cipher_algorithm) {
            #ifdef BUILD_AESGCM
                case wolfssl_aes_gcm:
                #ifdef WOLFSSL_ASYNC_CRYPT
                    /* initialize event */
                    asyncDev = &ssl->encrypt.aes->asyncDev;
                    ret = wolfSSL_AsyncInit(ssl, asyncDev, event_flags);
                    if (ret != 0)
                        break;
                #endif

                    nonceSz = AESGCM_NONCE_SZ;

                #if defined(HAVE_PK_CALLBACKS)
                    ret = NOT_COMPILED_IN;
                    if (ssl->ctx && ssl->ctx->PerformTlsRecordProcessingCb) {
                        ret = ssl->ctx->PerformTlsRecordProcessingCb(ssl, 1,
                                  output, input, dataSz,
                                  ssl->encrypt.nonce, nonceSz,
                                  output + dataSz, macSz,
                                  aad, aadSz);
                    }
                    if (ret == NOT_COMPILED_IN)
                #endif
                    {

                #if ((defined(HAVE_FIPS) || defined(HAVE_SELFTEST)) && \
                    (!defined(HAVE_FIPS_VERSION) || (HAVE_FIPS_VERSION < 2)))
                        ret = wc_AesGcmEncrypt(ssl->encrypt.aes, output, input,
                            dataSz, ssl->encrypt.nonce, nonceSz,
                            output + dataSz, macSz, aad, aadSz);
                #else
                        ret = wc_AesGcmSetExtIV(ssl->encrypt.aes,
                                ssl->encrypt.nonce, nonceSz);
                        if (ret == 0) {
                            ret = wc_AesGcmEncrypt_ex(ssl->encrypt.aes, output,
                                    input, dataSz, ssl->encrypt.nonce, nonceSz,
                                    output + dataSz, macSz, aad, aadSz);
                        }
                #endif
                    }
                    break;
            #endif

            #ifdef HAVE_AESCCM
                case wolfssl_aes_ccm:
                #ifdef WOLFSSL_ASYNC_CRYPT
                    /* initialize event */
                    asyncDev = &ssl->encrypt.aes->asyncDev;
                    ret = wolfSSL_AsyncInit(ssl, asyncDev, event_flags);
                    if (ret != 0)
                        break;
                #endif

                    nonceSz = AESCCM_NONCE_SZ;
                #if defined(HAVE_PK_CALLBACKS)
                    ret = NOT_COMPILED_IN;
                    if (ssl->ctx && ssl->ctx->PerformTlsRecordProcessingCb) {
                        ret = ssl->ctx->PerformTlsRecordProcessingCb(ssl, 1,
                                  output, input, dataSz,
                                  ssl->encrypt.nonce, nonceSz,
                                  output + dataSz, macSz,
                                  aad, aadSz);
                    }
                    if (ret == NOT_COMPILED_IN)
                #endif
                    {
                #if ((defined(HAVE_FIPS) || defined(HAVE_SELFTEST)) && \
                    (!defined(HAVE_FIPS_VERSION) || (HAVE_FIPS_VERSION < 2)))
                        ret = wc_AesCcmEncrypt(ssl->encrypt.aes, output, input,
                            dataSz, ssl->encrypt.nonce, nonceSz,
                            output + dataSz, macSz, aad, aadSz);
                #else
                        ret = wc_AesCcmSetNonce(ssl->encrypt.aes,
                                ssl->encrypt.nonce, nonceSz);
                        if (ret == 0) {
                            ret = wc_AesCcmEncrypt_ex(ssl->encrypt.aes, output,
                                    input, dataSz, ssl->encrypt.nonce, nonceSz,
                                    output + dataSz, macSz, aad, aadSz);
                        }
                #endif
                    }
                    break;
            #endif

            #if defined(HAVE_CHACHA) && defined(HAVE_POLY1305)
                case wolfssl_chacha:
                    ret = ChaCha20Poly1305_Encrypt(ssl, output, input, dataSz,
                        ssl->encrypt.nonce, aad, aadSz, output + dataSz);
                    break;
            #endif

            #ifdef WOLFSSL_SM4_GCM
                case wolfssl_sm4_gcm:
                    nonceSz = SM4_GCM_NONCE_SZ;
                    ret = wc_Sm4GcmEncrypt(ssl->encrypt.sm4, output, input,
                        dataSz, ssl->encrypt.nonce, nonceSz, output + dataSz,
                        macSz, aad, aadSz);
                    break;
            #endif

            #ifdef WOLFSSL_SM4_CCM
                case wolfssl_sm4_ccm:
                    nonceSz = SM4_CCM_NONCE_SZ;
                    ret = wc_Sm4CcmEncrypt(ssl->encrypt.sm4, output, input,
                        dataSz, ssl->encrypt.nonce, nonceSz, output + dataSz,
                        macSz, aad, aadSz);
                    break;
            #endif

            #ifdef HAVE_NULL_CIPHER
                case wolfssl_cipher_null:
                    ret = Tls13IntegrityOnly_Encrypt(ssl, output, input, dataSz,
                        ssl->encrypt.nonce, aad, aadSz, output + dataSz);
                    break;
            #endif

                default:
                    WOLFSSL_MSG("wolfSSL Encrypt programming error");
                    return ENCRYPT_ERROR;
            }

            /* Advance state */
            ssl->encrypt.state = CIPHER_STATE_END;

        #ifdef WOLFSSL_ASYNC_CRYPT
            if (ret == WC_PENDING_E) {
                /* if async is not okay, then block */
                if (!asyncOkay) {
                    ret = wc_AsyncWait(ret, asyncDev, event_flags);
                }
                else {
                    /* If pending, then leave and return will resume below */
                    return wolfSSL_AsyncPush(ssl, asyncDev);
                }
            }
        #endif
        }
        FALL_THROUGH;

        case CIPHER_STATE_END:
        {
        #ifdef WOLFSSL_DEBUG_TLS
            #ifdef CIPHER_NONCE
                WOLFSSL_MSG("Nonce");
                WOLFSSL_BUFFER(ssl->encrypt.nonce, ssl->specs.iv_size);
            #endif
                WOLFSSL_MSG("Encrypted data");
                WOLFSSL_BUFFER(output, dataSz);
                WOLFSSL_MSG("Authentication Tag");
                WOLFSSL_BUFFER(output + dataSz, macSz);
        #endif

        #ifdef WOLFSSL_CIPHER_TEXT_CHECK
            if (ssl->specs.bulk_cipher_algorithm != wolfssl_cipher_null &&
                XMEMCMP(output, ssl->encrypt.sanityCheck,
                    min(dataSz, sizeof(ssl->encrypt.sanityCheck))) == 0) {

                WOLFSSL_MSG("EncryptTls13 sanity check failed! Glitch?");
                return ENCRYPT_ERROR;
            }
            ForceZero(ssl->encrypt.sanityCheck,
                sizeof(ssl->encrypt.sanityCheck));
        #endif
        #ifdef WOLFSSL_CHECK_MEM_ZERO
            if ((ssl->specs.bulk_cipher_algorithm != wolfssl_cipher_null) &&
                    (output != input) && (ret == 0)) {
                wc_MemZero_Add("TLS 1.3 Encrypt plaintext", input, sz);
            }
        #endif

        #ifdef CIPHER_NONCE
            ForceZero(ssl->encrypt.nonce, AEAD_NONCE_SZ);
        #endif

            break;
        }

        default:
            break;
    }


    /* Reset state */
    ssl->encrypt.state = CIPHER_STATE_BEGIN;

    return ret;
}

#if defined(HAVE_CHACHA) && defined(HAVE_POLY1305)
/* Decrypt with ChaCha20 and check authentication tag with Poly1305.
 *
 * ssl     The SSL/TLS object.
 * output  The buffer to write decrypted data into.
 *         May be the same pointer as input.
 * input   The data to decrypt.
 * sz      The number of bytes to decrypt.
 * nonce   The nonce to use with ChaCha20.
 * aad     The additional authentication data.
 * aadSz   The size of the addition authentication data.
 * tagIn   The authentication tag data from packet.
 * returns 0 on success, otherwise failure.
 */
static int ChaCha20Poly1305_Decrypt(WOLFSSL* ssl, byte* output,
                                    const byte* input, word16 sz, byte* nonce,
                                    const byte* aad, word16 aadSz,
                                    const byte* tagIn)
{
    int ret;
    byte tag[POLY1305_AUTH_SZ];
    byte poly[CHACHA20_256_KEY_SIZE]; /* generated key for mac */

    /* Poly1305 key is 256 bits of zero encrypted with ChaCha20. */
    XMEMSET(poly, 0, sizeof(poly));

    /* Set nonce and get Poly1305 key. */
    ret = wc_Chacha_SetIV(ssl->decrypt.chacha, nonce, 0);
    if (ret != 0)
        return ret;
    /* Use ChaCha20 keystream to get Poly1305 key for tag. */
    ret = wc_Chacha_Process(ssl->decrypt.chacha, poly, poly, sizeof(poly));
    if (ret != 0)
        return ret;
#ifdef WOLFSSL_CHECK_MEM_ZERO
    wc_MemZero_Add("ChaCha20Poly1305_Decrypt poly", poly, sizeof(poly));
#endif
    ret = wc_Chacha_SetIV(ssl->decrypt.chacha, nonce, 1);
    if (ret != 0) {
        ForceZero(poly, sizeof(poly)); /* done with poly1305 key, clear it */
    #ifdef WOLFSSL_CHECK_MEM_ZERO
        wc_MemZero_Check(poly, sizeof(poly));
    #endif
        return ret;
    }

    /* Set key for Poly1305. */
    ret = wc_Poly1305SetKey(ssl->auth.poly1305, poly, sizeof(poly));
    ForceZero(poly, sizeof(poly)); /* done with poly1305 key, clear it */
#ifdef WOLFSSL_CHECK_MEM_ZERO
    wc_MemZero_Check(poly, sizeof(poly));
#endif
    if (ret != 0)
        return ret;
    /* Generate authentication tag for encrypted data. */
    if ((ret = wc_Poly1305_MAC(ssl->auth.poly1305, aad, aadSz, input, sz, tag,
                                                           sizeof(tag))) != 0) {
        return ret;
    }

    /* Check tag sent along with packet. */
    if (ConstantCompare(tagIn, tag, POLY1305_AUTH_SZ) != 0) {
        WOLFSSL_MSG("MAC did not match");
        return VERIFY_MAC_ERROR;
    }

    /* If the tag was good decrypt message. */
    ret = wc_Chacha_Process(ssl->decrypt.chacha, output, input, sz);

    return ret;
}
#endif

#ifdef HAVE_NULL_CIPHER
/* Check HMAC tag and copy over input.
 *
 * ssl     The SSL/TLS object.
 * output  The buffer to copy data into.
 *         May be the same pointer as input.
 * input   The data.
 * sz      The number of bytes of data.
 * nonce   The nonce to use with authentication.
 * aad     The additional authentication data.
 * aadSz   The size of the addition authentication data.
 * tagIn   The authentication tag data from packet.
 * returns 0 on success, otherwise failure.
 */
static int Tls13IntegrityOnly_Decrypt(WOLFSSL* ssl, byte* output,
                                      const byte* input, word16 sz,
                                      const byte* nonce,
                                      const byte* aad, word16 aadSz,
                                      const byte* tagIn)
{
    int ret;
    byte hmac[WC_MAX_DIGEST_SIZE];

    /* HMAC: nonce | aad | input  */
    ret = wc_HmacUpdate(ssl->decrypt.hmac, nonce, HMAC_NONCE_SZ);
    if (ret == 0)
        ret = wc_HmacUpdate(ssl->decrypt.hmac, aad, aadSz);
    if (ret == 0)
        ret = wc_HmacUpdate(ssl->decrypt.hmac, input, sz);
    if (ret == 0)
        ret = wc_HmacFinal(ssl->decrypt.hmac, hmac);
    /* Check authentication tag matches */
    if (ret == 0 && ConstantCompare(tagIn, hmac, ssl->specs.hash_size) != 0)
        ret = DECRYPT_ERROR;
    /* Copy the input to output if not the same buffer */
    if (ret == 0 && output != input)
        XMEMCPY(output, input, sz);

    return ret;
}
#endif

/* Decrypt data for TLS v1.3.
 *
 * ssl     The SSL/TLS object.
 * output  The buffer to write decrypted data into.
 *         May be the same pointer as input.
 * input   The data to decrypt and authentication tag.
 * sz      The length of the encrypted data plus authentication tag.
 * aad     The additional authentication data.
 * aadSz   The size of the addition authentication data.
 * returns 0 on success, otherwise failure.
 */
int DecryptTls13(WOLFSSL* ssl, byte* output, const byte* input, word16 sz,
                 const byte* aad, word16 aadSz)
{
    int    ret    = 0;
    word16 dataSz = sz - ssl->specs.aead_mac_size;
    word16 macSz  = ssl->specs.aead_mac_size;
    word32 nonceSz = 0;

    WOLFSSL_ENTER("DecryptTls13");

#if defined(WOLFSSL_RENESAS_TSIP_TLS)
    ret = tsip_Tls13AesDecrypt(ssl, output, input, sz);

    if (ret != CRYPTOCB_UNAVAILABLE) {
        #ifndef WOLFSSL_EARLY_DATA
        if (ret < 0) {
            ret = VERIFY_MAC_ERROR;
            WOLFSSL_ERROR_VERBOSE(ret);
        }
        #endif
        return ret;
    }
#endif

#ifdef WOLFSSL_ASYNC_CRYPT
    ret = wolfSSL_AsyncPop(ssl, &ssl->decrypt.state);
    if (ret != WC_NOT_PENDING_E) {
        /* check for still pending */
        if (ret == WC_PENDING_E)
            return ret;

        ssl->error = 0; /* clear async */

        /* let failures through so CIPHER_STATE_END logic is run */
    }
    else
#endif
    {
        /* Reset state */
        ret = 0;
        ssl->decrypt.state = CIPHER_STATE_BEGIN;
    }

    (void)output;
    (void)input;
    (void)sz;
    (void)dataSz;
    (void)macSz;
    (void)nonceSz;

    switch (ssl->decrypt.state) {
        case CIPHER_STATE_BEGIN:
        {
        #ifdef WOLFSSL_DEBUG_TLS
            WOLFSSL_MSG("Data to decrypt");
            WOLFSSL_BUFFER(input, dataSz);
            WOLFSSL_MSG("Additional Authentication Data");
            WOLFSSL_BUFFER(aad, aadSz);
            WOLFSSL_MSG("Authentication tag");
            WOLFSSL_BUFFER(input + dataSz, macSz);
        #endif

        #ifdef CIPHER_NONCE
            if (ssl->decrypt.nonce == NULL) {
                ssl->decrypt.nonce = (byte*)XMALLOC(AEAD_NONCE_SZ,
                                            ssl->heap, DYNAMIC_TYPE_AES_BUFFER);
            #ifdef WOLFSSL_CHECK_MEM_ZERO
                if (ssl->decrypt.nonce != NULL) {
                    wc_MemZero_Add("DecryptTls13 nonce", ssl->decrypt.nonce,
                        AEAD_NONCE_SZ);
                }
            #endif
            }
            if (ssl->decrypt.nonce == NULL)
                return MEMORY_E;

            BuildTls13Nonce(ssl, ssl->decrypt.nonce, ssl->keys.aead_dec_imp_IV,
                            PEER_ORDER);
        #endif

            /* Advance state and proceed */
            ssl->decrypt.state = CIPHER_STATE_DO;
        }
        FALL_THROUGH;

        case CIPHER_STATE_DO:
        {
            switch (ssl->specs.bulk_cipher_algorithm) {
            #ifdef BUILD_AESGCM
                case wolfssl_aes_gcm:
                #ifdef WOLFSSL_ASYNC_CRYPT
                    /* initialize event */
                    ret = wolfSSL_AsyncInit(ssl, &ssl->decrypt.aes->asyncDev,
                        WC_ASYNC_FLAG_NONE);
                    if (ret != 0)
                        break;
                #endif

                    nonceSz = AESGCM_NONCE_SZ;

                #if defined(HAVE_PK_CALLBACKS)
                    ret = NOT_COMPILED_IN;
                    if (ssl->ctx && ssl->ctx->PerformTlsRecordProcessingCb) {
                        ret = ssl->ctx->PerformTlsRecordProcessingCb(ssl, 0,
                                  output, input, dataSz,
                                  ssl->decrypt.nonce, nonceSz,
                                  (byte *)(input + dataSz), macSz,
                                  aad, aadSz);
                    }
                    if (ret == NOT_COMPILED_IN)
                #endif
                    {

                        ret = wc_AesGcmDecrypt(ssl->decrypt.aes, output, input,
                            dataSz, ssl->decrypt.nonce, nonceSz,
                            input + dataSz, macSz, aad, aadSz);

                #ifdef WOLFSSL_ASYNC_CRYPT
                        if (ret == WC_PENDING_E) {
                            ret = wolfSSL_AsyncPush(ssl,
                                &ssl->decrypt.aes->asyncDev);
                        }
                #endif

                    }
                    break;
            #endif

            #ifdef HAVE_AESCCM
                case wolfssl_aes_ccm:
                #ifdef WOLFSSL_ASYNC_CRYPT
                    /* initialize event */
                    ret = wolfSSL_AsyncInit(ssl, &ssl->decrypt.aes->asyncDev,
                        WC_ASYNC_FLAG_NONE);
                    if (ret != 0)
                        break;
                #endif

                    nonceSz = AESCCM_NONCE_SZ;
                #if defined(HAVE_PK_CALLBACKS)
                    ret = NOT_COMPILED_IN;
                    if (ssl->ctx && ssl->ctx->PerformTlsRecordProcessingCb) {
                        ret = ssl->ctx->PerformTlsRecordProcessingCb(ssl, 0,
                                  output, input, dataSz,
                                  ssl->decrypt.nonce, nonceSz,
                                  (byte *)(input + dataSz), macSz,
                                  aad, aadSz);
                    }
                    if (ret == NOT_COMPILED_IN)
                #endif
                    {
                        ret = wc_AesCcmDecrypt(ssl->decrypt.aes, output, input,
                            dataSz, ssl->decrypt.nonce, nonceSz,
                            input + dataSz, macSz, aad, aadSz);
                #ifdef WOLFSSL_ASYNC_CRYPT
                        if (ret == WC_PENDING_E) {
                            ret = wolfSSL_AsyncPush(ssl,
                                &ssl->decrypt.aes->asyncDev);
                        }
                #endif
                    }
                    break;
            #endif

            #if defined(HAVE_CHACHA) && defined(HAVE_POLY1305)
                case wolfssl_chacha:
                    ret = ChaCha20Poly1305_Decrypt(ssl, output, input, dataSz,
                        ssl->decrypt.nonce, aad, aadSz, input + dataSz);
                    break;
            #endif

            #ifdef WOLFSSL_SM4_GCM
                case wolfssl_sm4_gcm:
                    nonceSz = SM4_GCM_NONCE_SZ;
                    ret = wc_Sm4GcmDecrypt(ssl->decrypt.sm4, output, input,
                        dataSz, ssl->decrypt.nonce, nonceSz, output + dataSz,
                        macSz, aad, aadSz);
                    break;
            #endif

            #ifdef WOLFSSL_SM4_CCM
                case wolfssl_sm4_ccm:
                    nonceSz = SM4_CCM_NONCE_SZ;
                    ret = wc_Sm4CcmDecrypt(ssl->decrypt.sm4, output, input,
                        dataSz, ssl->decrypt.nonce, nonceSz, output + dataSz,
                        macSz, aad, aadSz);
                    break;
            #endif

            #ifdef HAVE_NULL_CIPHER
                case wolfssl_cipher_null:
                    ret = Tls13IntegrityOnly_Decrypt(ssl, output, input, dataSz,
                        ssl->decrypt.nonce, aad, aadSz, input + dataSz);
                    break;
            #endif
                default:
                    WOLFSSL_MSG("wolfSSL Decrypt programming error");
                    return DECRYPT_ERROR;
            }

            /* Advance state */
            ssl->decrypt.state = CIPHER_STATE_END;

        #ifdef WOLFSSL_ASYNC_CRYPT
            /* If pending, leave now */
            if (ret == WC_PENDING_E) {
                return ret;
            }
        #endif
        }
        FALL_THROUGH;

        case CIPHER_STATE_END:
        {
        #ifdef WOLFSSL_DEBUG_TLS
            #ifdef CIPHER_NONCE
                WOLFSSL_MSG("Nonce");
                WOLFSSL_BUFFER(ssl->decrypt.nonce, ssl->specs.iv_size);
            #endif
                WOLFSSL_MSG("Decrypted data");
                WOLFSSL_BUFFER(output, dataSz);
        #endif
        #ifdef WOLFSSL_CHECK_MEM_ZERO
            if ((ssl->specs.bulk_cipher_algorithm != wolfssl_cipher_null) &&
                    (ret == 0)) {
                wc_MemZero_Add("TLS 1.3 Decrypted data", output, sz);
            }
        #endif

        #ifdef CIPHER_NONCE
            ForceZero(ssl->decrypt.nonce, AEAD_NONCE_SZ);
        #endif

            break;
        }

       default:
            break;
    }

    if (ret < 0) {
        WOLFSSL_ERROR_VERBOSE(ret);
    }

    return ret;
}

/* Persistable BuildTls13Message arguments */
typedef struct BuildMsg13Args {
    word32 sz;
    word32 idx;
    word32 headerSz;
    word16 size;
} BuildMsg13Args;

static void FreeBuildMsg13Args(WOLFSSL* ssl, void* pArgs)
{
    BuildMsg13Args* args = (BuildMsg13Args*)pArgs;

    (void)ssl;
    (void)args;

    /* no allocations in BuildTls13Message */
}

/* Build SSL Message, encrypted.
 * TLS v1.3 encryption is AEAD only.
 *
 * ssl         The SSL/TLS object.
 * output      The buffer to write record message to.
 * outSz       Size of the buffer being written into.
 * input       The record data to encrypt (excluding record header).
 * inSz        The size of the record data.
 * type        The recorder header content type.
 * hashOutput  Whether to hash the unencrypted record data.
 * sizeOnly    Only want the size of the record message.
 * asyncOkay   If non-zero can return WC_PENDING_E, otherwise blocks on crypto
 * returns the size of the encrypted record message or negative value on error.
 */
int BuildTls13Message(WOLFSSL* ssl, byte* output, int outSz, const byte* input,
                int inSz, int type, int hashOutput, int sizeOnly, int asyncOkay)
{
    int ret;
    BuildMsg13Args* args;
    BuildMsg13Args  lcl_args;

    WOLFSSL_ENTER("BuildTls13Message");

#ifdef WOLFSSL_ASYNC_CRYPT
    ret = WC_NOT_PENDING_E;
    if (asyncOkay) {
        WOLFSSL_ASSERT_SIZEOF_GE(ssl->async->args, *args);

        if (ssl->async == NULL) {
            ssl->async = (struct WOLFSSL_ASYNC*)
                    XMALLOC(sizeof(struct WOLFSSL_ASYNC), ssl->heap,
                            DYNAMIC_TYPE_ASYNC);
            if (ssl->async == NULL)
                return MEMORY_E;
        }
        args = (BuildMsg13Args*)ssl->async->args;

        ret = wolfSSL_AsyncPop(ssl, &ssl->options.buildMsgState);
        if (ret != WC_NOT_PENDING_E) {
            /* Check for error */
            if (ret < 0)
                goto exit_buildmsg;
        }
    }
    else
#endif
    {
        args = &lcl_args;
    }

    /* Reset state */
#ifdef WOLFSSL_ASYNC_CRYPT
    if (ret == WC_NOT_PENDING_E)
#endif
    {
        ret = 0;
        ssl->options.buildMsgState = BUILD_MSG_BEGIN;
        XMEMSET(args, 0, sizeof(BuildMsg13Args));

        args->headerSz = RECORD_HEADER_SZ;
#ifdef WOLFSSL_DTLS13
        if (ssl->options.dtls)
            args->headerSz = Dtls13GetRlHeaderLength(ssl, 1);
#endif /* WOLFSSL_DTLS13 */

        args->sz = args->headerSz + inSz;
        args->idx  = args->headerSz;

    #ifdef WOLFSSL_ASYNC_CRYPT
        if (asyncOkay)
            ssl->async->freeArgs = FreeBuildMsg13Args;
    #endif
    }

    switch (ssl->options.buildMsgState) {
        case BUILD_MSG_BEGIN:
        {
           /* catch mistaken sizeOnly parameter */
            if (sizeOnly) {
                if (output || input) {
                    WOLFSSL_MSG("BuildTls13Message with sizeOnly "
                                "doesn't need input or output");
                    return BAD_FUNC_ARG;
                }
            }
            else if (output == NULL || input == NULL) {
                return BAD_FUNC_ARG;
            }

            /* Record layer content type at the end of record data. */
            args->sz++;
            /* Authentication data at the end. */
            args->sz += ssl->specs.aead_mac_size;

            if (sizeOnly)
                return args->sz;

            if (args->sz > (word32)outSz) {
                WOLFSSL_MSG("Oops, want to write past output buffer size");
                return BUFFER_E;
            }

            /* Record data length. */
            args->size = (word16)(args->sz - args->headerSz);
            /* Write/update the record header with the new size.
             * Always have the content type as application data for encrypted
             * messages in TLS v1.3.
             */

            if (ssl->options.dtls) {
#ifdef WOLFSSL_DTLS13
                Dtls13RlAddCiphertextHeader(ssl, output, args->size);
#endif /* WOLFSSL_DTLS13 */
            }
            else {
                AddTls13RecordHeader(output, args->size, application_data, ssl);
            }

            /* TLS v1.3 can do in place encryption. */
            if (input != output + args->idx)
                XMEMCPY(output + args->idx, input, inSz);
            args->idx += inSz;

            ssl->options.buildMsgState = BUILD_MSG_HASH;
        }
        FALL_THROUGH;

        case BUILD_MSG_HASH:
        {
            if (hashOutput) {
                ret = HashOutput(ssl, output, args->headerSz + inSz, 0);
                if (ret != 0)
                    goto exit_buildmsg;
            }

            /* The real record content type goes at the end of the data. */
            output[args->idx++] = (byte)type;

            ssl->options.buildMsgState = BUILD_MSG_ENCRYPT;
        }
        FALL_THROUGH;

        case BUILD_MSG_ENCRYPT:
        {
#ifdef WOLFSSL_QUIC
            if (WOLFSSL_IS_QUIC(ssl)) {
                /* QUIC does not use encryption of the TLS Record Layer.
                 * Return the original length + added headers
                 * and restore it in the record header. */
                AddTls13RecordHeader(output, inSz, type, ssl);
                ret = args->headerSz + inSz;
                goto exit_buildmsg;
            }
#endif
        #ifdef ATOMIC_USER
            if (ssl->ctx->MacEncryptCb) {
                /* User Record Layer Callback handling */
                byte* mac = output + args->idx;
                output += args->headerSz;

                ret = ssl->ctx->MacEncryptCb(ssl, mac, output, inSz, type, 0,
                        output, output, args->size, ssl->MacEncryptCtx);
            }
            else
        #endif
            {
                const byte* aad = output;
                output += args->headerSz;
                ret = EncryptTls13(ssl, output, output, args->size, aad,
                                   (word16)args->headerSz, asyncOkay);
                if (ret != 0) {
                #ifdef WOLFSSL_ASYNC_CRYPT
                    if (ret != WC_PENDING_E)
                #endif
                    {
                        /* Zeroize plaintext. */
                        ForceZero(output, args->size);
                    }
                }
#ifdef WOLFSSL_DTLS13
                if (ret == 0 && ssl->options.dtls) {
                    /* AAD points to the header. Reuse the variable  */
                    ret = Dtls13EncryptRecordNumber(ssl, (byte*)aad, (word16)args->sz);
                }
#endif /* WOLFSSL_DTLS13 */
            }
            break;
        }

        default:
            break;
    }

exit_buildmsg:

    WOLFSSL_LEAVE("BuildTls13Message", ret);

#ifdef WOLFSSL_ASYNC_CRYPT
    if (ret == WC_PENDING_E) {
        return ret;
    }
#endif

    /* make sure build message state is reset */
    ssl->options.buildMsgState = BUILD_MSG_BEGIN;

    /* return sz on success */
    if (ret == 0) {
        ret = args->sz;
    }
    else {
        WOLFSSL_ERROR_VERBOSE(ret);
    }

    /* Final cleanup */
#ifdef WOLFSSL_ASYNC_CRYPT
    if (asyncOkay)
        FreeAsyncCtx(ssl, 0);
    else
#endif
        FreeBuildMsg13Args(ssl, args);

    return ret;
}

#if !defined(NO_WOLFSSL_CLIENT) || (!defined(NO_WOLFSSL_SERVER) && \
    (defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)) && \
    (defined(WOLFSSL_PSK_ONE_ID) || defined(WOLFSSL_PRIORITIZE_PSK)))
/* Find the cipher suite in the suites set in the SSL.
 *
 * ssl    SSL/TLS object.
 * suite  Cipher suite to look for.
 * returns 1 when suite is found in SSL/TLS object's list and 0 otherwise.
 */
int FindSuiteSSL(const WOLFSSL* ssl, byte* suite)
{
    word16 i;
    const Suites* suites = WOLFSSL_SUITES(ssl);

    for (i = 0; i < suites->suiteSz; i += 2) {
        if (suites->suites[i+0] == suite[0] &&
                suites->suites[i+1] == suite[1]) {
            return 1;
        }
    }

    return 0;
}
#endif

#ifndef NO_PSK
/* Get the MAC algorithm for the TLS 1.3 cipher suite.
 *
 * @param [in] suite.
 * @return  A value from wc_MACAlgorithm enumeration.
 */
byte SuiteMac(const byte* suite)
{
    byte mac = no_mac;

    if (suite[0] == TLS13_BYTE) {
        switch (suite[1]) {
        #ifdef BUILD_TLS_AES_128_GCM_SHA256
            case TLS_AES_128_GCM_SHA256:
                mac = sha256_mac;
                break;
        #endif
        #ifdef BUILD_TLS_CHACHA20_POLY1305_SHA256
            case TLS_CHACHA20_POLY1305_SHA256:
                mac = sha256_mac;
                break;
        #endif
        #ifdef BUILD_TLS_AES_128_CCM_SHA256
            case TLS_AES_128_CCM_SHA256:
                mac = sha256_mac;
                break;
        #endif
        #ifdef BUILD_TLS_AES_128_CCM_8_SHA256
            case TLS_AES_128_CCM_8_SHA256:
                mac = sha256_mac;
                break;
        #endif
        #ifdef BUILD_TLS_AES_256_GCM_SHA384
            case TLS_AES_256_GCM_SHA384:
                mac = sha384_mac;
                break;
        #endif
            default:
                break;
        }
    }
#if (defined(WOLFSSL_SM4_GCM) || defined(WOLFSSL_SM4_CCM)) && \
     defined(WOLFSSL_SM3)
    else if (suite[0] == CIPHER_BYTE) {
        switch (suite[1]) {
        #ifdef BUILD_TLS_SM4_GCM_SM3
            case TLS_SM4_GCM_SM3:
                mac = sm3_mac;
                break;
        #endif
        #ifdef BUILD_TLS_SM4_CCM_SM3
            case TLS_SM4_CCM_SM3:
                mac = sm3_mac;
                break;
        #endif
            default:
                break;
        }
    }
#endif
#ifdef HAVE_NULL_CIPHER
    else if (suite[0] == ECC_BYTE) {
        switch (suite[1]) {
        #ifdef BUILD_TLS_SHA256_SHA256
            case TLS_SHA256_SHA256:
                mac = sha256_mac;
                break;
        #endif
        #ifdef BUILD_TLS_SHA384_SHA384
            case TLS_SHA384_SHA384:
                mac = sha384_mac;
                break;
        #endif
            default:
                break;
        }
    }
#endif

    return mac;
}
#endif

#if defined(WOLFSSL_SEND_HRR_COOKIE) && !defined(NO_WOLFSSL_SERVER)
/* Create Cookie extension using the hash of the first ClientHello.
 *
 * ssl     SSL/TLS object.
 * hash    The hash data.
 * hashSz  The size of the hash data in bytes.
 * returns 0 on success, otherwise failure.
 */
int CreateCookieExt(const WOLFSSL* ssl, byte* hash, word16 hashSz,
                    TLSX** exts, byte cipherSuite0, byte cipherSuite)
{
    int  ret;
    byte mac[WC_MAX_DIGEST_SIZE] = {0};
    Hmac cookieHmac;
    byte cookieType = 0;
    byte macSz = 0;
    byte cookie[OPAQUE8_LEN + WC_MAX_DIGEST_SIZE + OPAQUE16_LEN * 2];
    TLSX* ext;
    word16 cookieSz = 0;

    if (hash == NULL || hashSz == 0) {
        return BAD_FUNC_ARG;
    }

    /* Cookie Data = Hash Len | Hash | CS | KeyShare Group */
    cookie[cookieSz++] = (byte)hashSz;
    XMEMCPY(cookie + cookieSz, hash, hashSz);
    cookieSz += hashSz;
    cookie[cookieSz++] = cipherSuite0;
    cookie[cookieSz++] = cipherSuite;
    if ((ext = TLSX_Find(*exts, TLSX_KEY_SHARE)) != NULL) {
        KeyShareEntry* kse = (KeyShareEntry*)ext->data;
        if (kse == NULL) {
            WOLFSSL_MSG("KeyShareEntry can't be empty when negotiating "
                        "parameters");
            return BAD_STATE_E;
        }
        c16toa(kse->group, cookie + cookieSz);
        cookieSz += OPAQUE16_LEN;
    }

#if !defined(NO_SHA) && defined(NO_SHA256)
    cookieType = SHA;
    macSz = WC_SHA_DIGEST_SIZE;
#endif /* NO_SHA */
#ifndef NO_SHA256
    cookieType = WC_SHA256;
    macSz = WC_SHA256_DIGEST_SIZE;
#endif /* NO_SHA256 */

    ret = wc_HmacInit(&cookieHmac, ssl->heap, INVALID_DEVID);
    if (ret == 0) {
        ret = wc_HmacSetKey(&cookieHmac, cookieType,
                            ssl->buffers.tls13CookieSecret.buffer,
                            ssl->buffers.tls13CookieSecret.length);
    }
    if (ret == 0)
        ret = wc_HmacUpdate(&cookieHmac, cookie, cookieSz);
#ifdef WOLFSSL_DTLS13
    /* Tie cookie to peer address */
    if (ret == 0) {
        if (ssl->options.dtls && ssl->buffers.dtlsCtx.peer.sz > 0) {
            ret = wc_HmacUpdate(&cookieHmac,
                (byte*)ssl->buffers.dtlsCtx.peer.sa,
                ssl->buffers.dtlsCtx.peer.sz);
        }
    }
#endif
    if (ret == 0)
        ret = wc_HmacFinal(&cookieHmac, mac);

    wc_HmacFree(&cookieHmac);
    if (ret != 0)
        return ret;

    /* The cookie data is the hash and the integrity check. */
    return TLSX_Cookie_Use(ssl, cookie, cookieSz, mac, macSz, 1, exts);
}
#endif

#ifdef WOLFSSL_DTLS13
#define HRR_MAX_HS_HEADER_SZ DTLS_HANDSHAKE_HEADER_SZ
#else
#define HRR_MAX_HS_HEADER_SZ HANDSHAKE_HEADER_SZ
#endif /* WOLFSSL_DTLS13 */

static int CreateCookie(const WOLFSSL* ssl, byte** hash, byte* hashSz,
                            Hashes* hashes, TLSX** exts)
{
    int    ret = 0;

    (void)exts;

    *hash = NULL;
    switch (ssl->specs.mac_algorithm) {
    #ifndef NO_SHA256
        case sha256_mac:
            *hash = hashes->sha256;
            break;
    #endif
    #ifdef WOLFSSL_SHA384
        case sha384_mac:
            *hash = hashes->sha384;
            break;
    #endif
    #ifdef WOLFSSL_TLS13_SHA512
        case sha512_mac:
            *hash = hashes->sha512;
            break;
    #endif
    #ifdef WOLFSSL_SM3
        case sm3_mac:
            *hash = hashes->sm3;
            break;
    #endif
    }
    *hashSz = ssl->specs.hash_size;

    /* check hash */
    if (*hash == NULL && *hashSz > 0)
        return BAD_FUNC_ARG;

#if defined(WOLFSSL_SEND_HRR_COOKIE) && !defined(NO_WOLFSSL_SERVER)
    if (ssl->options.sendCookie && ssl->options.side == WOLFSSL_SERVER_END)
        ret = CreateCookieExt(ssl, *hash, *hashSz, exts,
                ssl->options.cipherSuite0, ssl->options.cipherSuite);
#endif
    return ret;
}

/* Restart the handshake hash with a hash of the previous messages.
 *
 * ssl The SSL/TLS object.
 * returns 0 on success, otherwise failure.
 */
int RestartHandshakeHash(WOLFSSL* ssl)
{
    int    ret;
    byte   header[HANDSHAKE_HEADER_SZ] = {0};
    Hashes hashes;
    byte*  hash = NULL;
    byte   hashSz = 0;

    ret = BuildCertHashes(ssl, &hashes);
    if (ret != 0)
        return ret;
    ret = CreateCookie(ssl, &hash, &hashSz, &hashes, &ssl->extensions);
    if (ret != 0)
        return ret;
#if defined(WOLFSSL_SEND_HRR_COOKIE) && !defined(NO_WOLFSSL_SERVER)
    if (ssl->options.sendCookie && ssl->options.side == WOLFSSL_SERVER_END)
        return 0;
#endif

    AddTls13HandShakeHeader(header, hashSz, 0, 0, message_hash, ssl);

#ifdef WOLFSSL_DEBUG_TLS
    WOLFSSL_MSG("Restart Hash");
    WOLFSSL_BUFFER(hash, hashSz);
#endif

    ret = InitHandshakeHashes(ssl);
    if (ret != 0)
        return ret;
    ret = HashRaw(ssl, header, sizeof(header));
    if (ret != 0)
        return ret;
    return HashRaw(ssl, hash, hashSz);
}

#if !defined(NO_WOLFSSL_CLIENT) || !defined(NO_WOLFSSL_SERVER)
/* The value in the random field of a ServerHello to indicate
 * HelloRetryRequest.
 */
static byte helloRetryRequestRandom[] = {
    0xCF, 0x21, 0xAD, 0x74, 0xE5, 0x9A, 0x61, 0x11,
    0xBE, 0x1D, 0x8C, 0x02, 0x1E, 0x65, 0xB8, 0x91,
    0xC2, 0xA2, 0x11, 0x16, 0x7A, 0xBB, 0x8C, 0x5E,
    0x07, 0x9E, 0x09, 0xE2, 0xC8, 0xA8, 0x33, 0x9C
};
#endif

#ifndef NO_WOLFSSL_CLIENT
#if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
#if defined(OPENSSL_EXTRA) && !defined(WOLFSSL_PSK_ONE_ID) && \
    !defined(NO_PSK)
/**
* convert mac algorithm to WOLFSSL_EVP_MD
* @param mac_alg mac algorithm
* @return const WOLFSSL_EVP_MD on successful, otherwise NULL
*/
static const WOLFSSL_EVP_MD* ssl_handshake_md(const byte mac_alg)
{
    switch(mac_alg) {
        case no_mac:
    #ifndef NO_MD5
        case md5_mac:
            return wolfSSL_EVP_md5();
    #endif
    #ifndef NO_SHA
        case sha_mac:
            return wolfSSL_EVP_sha1();
    #endif
    #ifdef WOLFSSL_SHA224
        case sha224_mac:
            return wolfSSL_EVP_sha224();
    #endif
        case sha256_mac:
            return wolfSSL_EVP_sha256();
    #ifdef WOLFSSL_SHA384
        case sha384_mac:
            return wolfSSL_EVP_sha384();
    #endif
    #ifdef WOLFSSL_SHA512
        case sha512_mac:
            return wolfSSL_EVP_sha512();
    #endif
        case rmd_mac:
        case blake2b_mac:
            WOLFSSL_MSG("no suitable EVP_MD");
            return NULL;
        default:
            WOLFSSL_MSG("Unknown mac algorithm");
            return NULL;
    }
}
#endif
/* Setup pre-shared key based on the details in the extension data.
 *
 * ssl          SSL/TLS object.
 * psk          Pre-shared key extension data.
 * clientHello  Whether called from client_hello construction.
 * returns 0 on success, PSK_KEY_ERROR when the client PSK callback fails and
 * other negative value on failure.
 */
static int SetupPskKey(WOLFSSL* ssl, PreSharedKey* psk, int clientHello)
{
#if defined(HAVE_SESSION_TICKET) || !defined(WOLFSSL_PSK_ONE_ID)
    int ret;
#endif
    byte suite[2];

    if (psk == NULL)
        return BAD_FUNC_ARG;

    suite[0] = ssl->options.cipherSuite0;
    suite[1] = ssl->options.cipherSuite;

#ifdef HAVE_SESSION_TICKET
    if (psk->resumption) {
        if (clientHello) {
            suite[0] = psk->cipherSuite0;
            suite[1] = psk->cipherSuite;

            /* Ensure cipher suite is supported or changed suite to one with
             * the same MAC algorithm. */
            if (!FindSuiteSSL(ssl, suite)) {
                WOLFSSL_ERROR_VERBOSE(PSK_KEY_ERROR);
                return PSK_KEY_ERROR;
            }

            ssl->options.cipherSuite0 = suite[0];
            ssl->options.cipherSuite = suite[1];

            /* Setting mac for binder and keys for deriving EarlyData. */
            ret = SetCipherSpecs(ssl);
            if (ret != 0)
                return ret;
        }

    #ifdef WOLFSSL_EARLY_DATA
        if (ssl->session->maxEarlyDataSz == 0)
            ssl->earlyData = no_early_data;
    #endif
        /* Resumption PSK is master secret. */
        ssl->arrays->psk_keySz = ssl->specs.hash_size;
        if ((ret = DeriveResumptionPSK(ssl, ssl->session->ticketNonce.data,
                   ssl->session->ticketNonce.len, ssl->arrays->psk_key)) != 0) {
            return ret;
        }
        if (!clientHello) {
            /* CLIENT: using secret in ticket for peer authentication. */
            ssl->options.peerAuthGood = 1;
        }
    }
#endif
#ifndef NO_PSK
    if (!psk->resumption) {
        /* Get the pre-shared key. */
#ifndef WOLFSSL_PSK_ONE_ID
        const char* cipherName = NULL;
    #ifdef OPENSSL_EXTRA
        WOLFSSL_SESSION* psksession = NULL;
    #endif

        /* Set the client identity to use. */
        XMEMSET(ssl->arrays->client_identity, 0,
            sizeof(ssl->arrays->client_identity));
        XMEMCPY(ssl->arrays->client_identity, psk->identity, psk->identityLen);

    #ifdef WOLFSSL_DEBUG_TLS
        WOLFSSL_MSG("PSK cipher suite:");
        WOLFSSL_MSG(GetCipherNameInternal(psk->cipherSuite0, psk->cipherSuite));
    #endif

        /* Get the pre-shared key. */
    #ifdef OPENSSL_EXTRA
        if (ssl->options.session_psk_cb != NULL) {
            const unsigned char* id = NULL;
            size_t idlen = 0;
            const WOLFSSL_EVP_MD* handshake_md = NULL;

            if (ssl->msgsReceived.got_hello_retry_request >= 1) {
                handshake_md = ssl_handshake_md(ssl->specs.mac_algorithm);
            }
            /* OpenSSL compatible callback that gets cached session. */
            if (ssl->options.session_psk_cb(ssl, handshake_md, &id, &idlen,
                                                            &psksession) == 0) {
                wolfSSL_FreeSession(ssl->ctx, psksession);
                WOLFSSL_MSG("psk session callback failed");
                return PSK_KEY_ERROR;
            }
            if (psksession != NULL) {
                if (idlen > MAX_PSK_KEY_LEN) {
                    wolfSSL_FreeSession(ssl->ctx, psksession);
                    WOLFSSL_MSG("psk key length is too long");
                    WOLFSSL_ERROR_VERBOSE(PSK_KEY_ERROR);
                    return PSK_KEY_ERROR;
                }

                ssl->arrays->psk_keySz = (word32)idlen;
                XMEMCPY(ssl->arrays->psk_key, id, idlen);
                suite[0] = psksession->cipherSuite0;
                suite[1] = psksession->cipherSuite;
                /* Not needed anymore. */
                wolfSSL_FreeSession(ssl->ctx, psksession);
                /* Leave pointer not NULL to indicate success with callback. */
            }
        }
        if (psksession != NULL) {
            /* Don't try other callbacks - we have an answer. */
        }
        else
    #endif /* OPENSSL_EXTRA */
        if (ssl->options.client_psk_cs_cb != NULL) {
        #ifdef WOLFSSL_PSK_MULTI_ID_PER_CS
            ssl->arrays->client_identity[0] = 0;
        #endif
            /* Lookup key again for next identity. */
            ssl->arrays->psk_keySz = ssl->options.client_psk_cs_cb(
                ssl, ssl->arrays->server_hint,
                ssl->arrays->client_identity, MAX_PSK_ID_LEN,
                ssl->arrays->psk_key, MAX_PSK_KEY_LEN,
                GetCipherNameInternal(psk->cipherSuite0, psk->cipherSuite));
            if (clientHello) {
                /* Use PSK cipher suite. */
                ssl->options.cipherSuite0 = psk->cipherSuite0;
                ssl->options.cipherSuite  = psk->cipherSuite;
            }
            else {
                byte pskCS[2];
                pskCS[0] = psk->cipherSuite0;
                pskCS[1] = psk->cipherSuite;

                /* Ensure PSK and negotiated cipher suites have same hash. */
                if (SuiteMac(pskCS) != SuiteMac(suite)) {
                    WOLFSSL_ERROR_VERBOSE(PSK_KEY_ERROR);
                    return PSK_KEY_ERROR;
                }
                /* Negotiated cipher suite is to be used - update PSK. */
                psk->cipherSuite0 = suite[0];
                psk->cipherSuite  = suite[1];
            }
        }
        else if (ssl->options.client_psk_tls13_cb != NULL) {
            byte cipherSuite0;
            byte cipherSuite;
            int cipherSuiteFlags = WOLFSSL_CIPHER_SUITE_FLAG_NONE;

            ssl->arrays->psk_keySz = ssl->options.client_psk_tls13_cb(ssl,
                    ssl->arrays->server_hint, ssl->arrays->client_identity,
                    MAX_PSK_ID_LEN, ssl->arrays->psk_key, MAX_PSK_KEY_LEN,
                    &cipherName);
            if (GetCipherSuiteFromName(cipherName, &cipherSuite0,
                                       &cipherSuite, &cipherSuiteFlags) != 0) {
                WOLFSSL_ERROR_VERBOSE(PSK_KEY_ERROR);
                return PSK_KEY_ERROR;
            }
            ssl->options.cipherSuite0 = cipherSuite0;
            ssl->options.cipherSuite  = cipherSuite;
            (void)cipherSuiteFlags;
        }
        else {
            ssl->arrays->psk_keySz = ssl->options.client_psk_cb(ssl,
                    ssl->arrays->server_hint, ssl->arrays->client_identity,
                    MAX_PSK_ID_LEN, ssl->arrays->psk_key, MAX_PSK_KEY_LEN);
            ssl->options.cipherSuite0 = TLS13_BYTE;
            ssl->options.cipherSuite  = WOLFSSL_DEF_PSK_CIPHER;
        }
        if (ssl->arrays->psk_keySz == 0 ||
                                     ssl->arrays->psk_keySz > MAX_PSK_KEY_LEN) {
            WOLFSSL_ERROR_VERBOSE(PSK_KEY_ERROR);
            return PSK_KEY_ERROR;
        }

        ret = SetCipherSpecs(ssl);
        if (ret != 0)
            return ret;
#else
        /* PSK information loaded during setting of default TLS extensions. */
#endif /* !WOLFSSL_PSK_ONE_ID */

        if (!clientHello && (psk->cipherSuite0 != suite[0] ||
                                                psk->cipherSuite != suite[1])) {
            WOLFSSL_ERROR_VERBOSE(PSK_KEY_ERROR);
            return PSK_KEY_ERROR;
        }

        if (!clientHello) {
            /* CLIENT: using PSK for peer authentication. */
            ssl->options.peerAuthGood = 1;
        }
    }
#endif

    if (ssl->options.noPskDheKe) {
        ssl->arrays->preMasterSz = 0;
    }

    /* Derive the early secret using the PSK. */
    return DeriveEarlySecret(ssl);
}

/* Derive and write the binders into the ClientHello in space left when
 * writing the Pre-Shared Key extension.
 *
 * ssl     The SSL/TLS object.
 * output  The buffer containing the ClientHello.
 * idx     The index at the end of the completed ClientHello.
 * returns 0 on success and otherwise failure.
 */
static int WritePSKBinders(WOLFSSL* ssl, byte* output, word32 idx)
{
    int           ret;
    TLSX*         ext;
    PreSharedKey* current;
    byte          binderKey[WC_MAX_DIGEST_SIZE];
    word16        len;

    WOLFSSL_ENTER("WritePSKBinders");

    ext = TLSX_Find(ssl->extensions, TLSX_PRE_SHARED_KEY);
    if (ext == NULL)
        return SANITY_MSG_E;

    /* Get the size of the binders to determine where to write binders. */
    ret = TLSX_PreSharedKey_GetSizeBinders((PreSharedKey*)ext->data,
                                                            client_hello, &len);
    if (ret < 0)
        return ret;
    idx -= len;

    /* Hash truncated ClientHello - up to binders. */
#ifdef WOLFSSL_DTLS13
    if (ssl->options.dtls)
        ret = Dtls13HashHandshake(ssl, output + Dtls13GetRlHeaderLength(ssl, 0),
            idx - Dtls13GetRlHeaderLength(ssl, 0));
    else
#endif /* WOLFSSL_DTLS13 */
        ret = HashOutput(ssl, output, idx, 0);

    if (ret != 0)
        return ret;

    current = (PreSharedKey*)ext->data;
#ifdef WOLFSSL_CHECK_MEM_ZERO
    if (current != NULL) {
        wc_MemZero_Add("WritePSKBinders binderKey", binderKey,
            sizeof(binderKey));
    }
#endif
    /* Calculate the binder for each identity based on previous handshake data.
     */
    while (current != NULL) {
        if ((ret = SetupPskKey(ssl, current, 1)) != 0)
            break;

    #ifdef HAVE_SESSION_TICKET
        if (current->resumption)
            ret = DeriveBinderKeyResume(ssl, binderKey);
    #endif
    #ifndef NO_PSK
        if (!current->resumption)
            ret = DeriveBinderKey(ssl, binderKey);
    #endif
        if (ret != 0)
            break;

        /* Derive the Finished message secret. */
        ret = DeriveFinishedSecret(ssl, binderKey,
                                   ssl->keys.client_write_MAC_secret,
                                   0 /* neither end */);
        if (ret != 0)
            break;

        /* Build the HMAC of the handshake message data = binder. */
        ret = BuildTls13HandshakeHmac(ssl, ssl->keys.client_write_MAC_secret,
            current->binder, &current->binderLen);
        if (ret != 0)
            break;

        current = current->next;
    }

    ForceZero(binderKey, sizeof(binderKey));
#ifdef WOLFSSL_CHECK_MEM_ZERO
    wc_MemZero_Check(binderKey, sizeof(binderKey));
#endif
    if (ret != 0)
        return ret;

    /* Data entered into extension, now write to message. */
    ret = TLSX_PreSharedKey_WriteBinders((PreSharedKey*)ext->data, output + idx,
                                                            client_hello, &len);
    if (ret < 0)
        return ret;

    /* Hash binders to complete the hash of the ClientHello. */
    ret = HashRaw(ssl, output + idx, len);
    if (ret < 0)
        return ret;

    #ifdef WOLFSSL_EARLY_DATA
    if (ssl->earlyData != no_early_data) {
        if ((ret = SetupPskKey(ssl, (PreSharedKey*)ext->data, 1)) != 0)
            return ret;

        /* Derive early data encryption key. */
        ret = DeriveTls13Keys(ssl, early_data_key, ENCRYPT_SIDE_ONLY, 1);
        if (ret != 0)
            return ret;
        if ((ret = SetKeysSide(ssl, ENCRYPT_SIDE_ONLY)) != 0)
            return ret;

#ifdef WOLFSSL_DTLS13
        if (ssl->options.dtls) {
            ret = Dtls13NewEpoch(
                ssl, w64From32(0x0, DTLS13_EPOCH_EARLYDATA), ENCRYPT_SIDE_ONLY);
            if (ret != 0)
                return ret;
        }
#endif /* WOLFSSL_DTLS13 */

    }
    #endif

    WOLFSSL_LEAVE("WritePSKBinders", ret);

    return ret;
}
#endif

#if defined(HAVE_ECH)
/* returns the index of the first supported cipher suite, -1 if none */
int EchConfigGetSupportedCipherSuite(WOLFSSL_EchConfig* config)
{
    int i, j, supported = 0;

    for (i = 0; i < config->numCipherSuites; i++) {
        supported = 0;

        for (j = 0; j < HPKE_SUPPORTED_KDF_LEN; j++) {
            if (config->cipherSuites[i].kdfId == hpkeSupportedKdf[j])
                break;
        }

        if (j < HPKE_SUPPORTED_KDF_LEN)
            for (j = 0; j < HPKE_SUPPORTED_AEAD_LEN; j++) {
                if (config->cipherSuites[i].aeadId == hpkeSupportedAead[j]) {
                    supported = 1;
                    break;
                }
            }

        if (supported)
            return i;
    }

    return -1;
}

/* returns status after we hash the ech inner */
static int EchHashHelloInner(WOLFSSL* ssl, WOLFSSL_ECH* ech)
{
    int ret;
    HS_Hashes* tmpHashes;
    byte falseHeader[HANDSHAKE_HEADER_SZ];

    if (ssl == NULL || ech == NULL)
        return BAD_FUNC_ARG;

    /* switch hsHashes to the ech version */
    InitHandshakeHashesAndCopy(ssl, ssl->hsHashes, &ssl->hsHashesEch);

    /* swap hsHashes so the regular hash functions work */
    tmpHashes = ssl->hsHashes;
    ssl->hsHashes = ssl->hsHashesEch;

    /* do the handshake header then the body */
    AddTls13HandShakeHeader(falseHeader,
        ech->innerClientHelloLen - ech->paddingLen - ech->hpke->Nt, 0, 0,
        client_hello, ssl);
    ret = HashRaw(ssl, falseHeader, HANDSHAKE_HEADER_SZ);

    /* hash the body */
    if (ret == 0) {
        ret = HashRaw(ssl, ech->innerClientHello,
              ech->innerClientHelloLen - ech->paddingLen - ech->hpke->Nt);
    }

    /* swap hsHashes back */
    ssl->hsHashes = tmpHashes;

    return ret;
}
#endif

static void GetTls13SessionId(WOLFSSL* ssl, byte* output, word32* idx)
{
    if (ssl->session->sessionIDSz > 0) {
        /* Session resumption for old versions of protocol. */
        if (ssl->session->sessionIDSz <= ID_LEN) {
            if (output != NULL)
                output[*idx] = ssl->session->sessionIDSz;
            (*idx)++;
            if (output != NULL) {
                XMEMCPY(output + *idx, ssl->session->sessionID,
                    ssl->session->sessionIDSz);
            }
            *idx += ssl->session->sessionIDSz;
        }
        else {
            /* Invalid session ID length. Reset it. */
            ssl->session->sessionIDSz = 0;
            if (output != NULL)
                output[*idx] = 0;
            (*idx)++;
        }
    }
    else {
    #ifdef WOLFSSL_TLS13_MIDDLEBOX_COMPAT
        if (ssl->options.tls13MiddleBoxCompat) {
            if (output != NULL)
                output[*idx] = ID_LEN;
            (*idx)++;
            if (output != NULL)
                XMEMCPY(output + *idx, ssl->arrays->clientRandom, ID_LEN);
            *idx += ID_LEN;
        }
        else
    #endif /* WOLFSSL_TLS13_MIDDLEBOX_COMPAT */
        {
            /* TLS v1.3 does not use session id - 0 length. */
            if (output != NULL)
                output[*idx] = 0;
            (*idx)++;
        }
    }
}

/* handle generation of TLS 1.3 client_hello (1) */
/* Send a ClientHello message to the server.
 * Include the information required to start a handshake with servers using
 * protocol versions less than TLS v1.3.
 * Only a client will send this message.
 *
 * ssl  The SSL/TLS object.
 * returns 0 on success and otherwise failure.
 */

typedef struct Sch13Args {
    byte*  output;
    word32 idx;
    int    sendSz;
    word16 length;
#if defined(HAVE_ECH)
    int clientRandomOffset;
    int preXLength;
    WOLFSSL_ECH* ech;
#endif
} Sch13Args;

int SendTls13ClientHello(WOLFSSL* ssl)
{
    int ret;
#ifdef WOLFSSL_ASYNC_CRYPT
    Sch13Args* args = NULL;
    WOLFSSL_ASSERT_SIZEOF_GE(ssl->async->args, *args);
#else
    Sch13Args  args[1];
#endif
    byte major, tls12minor;
    const Suites* suites;

    WOLFSSL_START(WC_FUNC_CLIENT_HELLO_SEND);
    WOLFSSL_ENTER("SendTls13ClientHello");

    if (ssl == NULL) {
        return BAD_FUNC_ARG;
    }

    ssl->options.buildingMsg = 1;
    major = SSLv3_MAJOR;
    tls12minor = TLSv1_2_MINOR;

#ifdef WOLFSSL_DTLS13
    if (ssl->options.dtls) {
        major = DTLS_MAJOR;
        tls12minor = DTLSv1_2_MINOR;
    }
#endif /* WOLFSSL_DTLS */

#ifdef HAVE_SESSION_TICKET
    if (ssl->options.resuming &&
            (ssl->session->version.major != ssl->version.major ||
             ssl->session->version.minor != ssl->version.minor)) {
    #ifndef WOLFSSL_NO_TLS12
        if (ssl->session->version.major == ssl->version.major &&
            ssl->session->version.minor < ssl->version.minor) {
            /* Cannot resume with a different protocol version. */
            ssl->options.resuming = 0;
            ssl->version.major = ssl->session->version.major;
            ssl->version.minor = ssl->session->version.minor;
            return SendClientHello(ssl);
        }
        else
    #endif
        {
            WOLFSSL_ERROR_VERBOSE(VERSION_ERROR);
            return VERSION_ERROR;
        }
    }
#endif

    suites = WOLFSSL_SUITES(ssl);
    if (suites == NULL) {
        WOLFSSL_MSG("Bad suites pointer in SendTls13ClientHello");
        return SUITES_ERROR;
    }

#ifdef WOLFSSL_ASYNC_CRYPT
    if (ssl->async == NULL) {
        ssl->async = (struct WOLFSSL_ASYNC*)
                XMALLOC(sizeof(struct WOLFSSL_ASYNC), ssl->heap,
                        DYNAMIC_TYPE_ASYNC);
        if (ssl->async == NULL)
            return MEMORY_E;
        ssl->async->freeArgs = NULL;
    }
    args = (Sch13Args*)ssl->async->args;

    ret = wolfSSL_AsyncPop(ssl, &ssl->options.asyncState);
    if (ret != WC_NOT_PENDING_E) {
        /* Check for error */
        if (ret < 0)
            return ret;
    }
    else
#endif
    {
        /* Reset state */
        ssl->options.asyncState = TLS_ASYNC_BEGIN;
        XMEMSET(args, 0, sizeof(Sch13Args));
    }

    switch (ssl->options.asyncState) {
    case TLS_ASYNC_BEGIN:
    {
    word32 sessIdSz = 0;

    args->idx = RECORD_HEADER_SZ + HANDSHAKE_HEADER_SZ;

#ifdef WOLFSSL_DTLS13
    if (ssl->options.dtls)
        args->idx += DTLS_RECORD_EXTRA + DTLS_HANDSHAKE_EXTRA;
#endif /* WOLFSSL_DTLS13 */

    /* Version | Random | Cipher Suites | Compression */
    args->length = VERSION_SZ + RAN_LEN + suites->suiteSz +
            SUITE_LEN + COMP_LEN + ENUM_LEN;
#ifdef WOLFSSL_QUIC
    if (WOLFSSL_IS_QUIC(ssl)) {
        /* RFC 9001 ch. 8.4 sessionID in ClientHello MUST be 0 length */
        ssl->session->sessionIDSz = 0;
        ssl->options.tls13MiddleBoxCompat = 0;
    }
#endif
    GetTls13SessionId(ssl, NULL, &sessIdSz);
    args->length += (word16)sessIdSz;

#ifdef WOLFSSL_DTLS13
    if (ssl->options.dtls) {
        /* legacy_cookie_id len */
        args->length += ENUM_LEN;

        /* server sent us an HelloVerifyRequest and we allow downgrade  */
        if (ssl->arrays->cookieSz > 0 && ssl->options.downgrade)
            args->length += ssl->arrays->cookieSz;
    }
#endif /* WOLFSSL_DTLS13 */

    /* Advance state and proceed */
    ssl->options.asyncState = TLS_ASYNC_BUILD;
    } /* case TLS_ASYNC_BEGIN */
    FALL_THROUGH;

    case TLS_ASYNC_BUILD:
    case TLS_ASYNC_DO:
    {
    /* Auto populate extensions supported unless user defined. */
    if ((ret = TLSX_PopulateExtensions(ssl, 0)) != 0)
        return ret;

    /* Advance state and proceed */
    ssl->options.asyncState = TLS_ASYNC_FINALIZE;
    } /* case TLS_ASYNC_BUILD */
    FALL_THROUGH;

    case TLS_ASYNC_FINALIZE:
    {
#ifdef WOLFSSL_EARLY_DATA
    #ifndef NO_PSK
        if (!ssl->options.resuming &&
                                     ssl->options.client_psk_tls13_cb == NULL &&
                                     ssl->options.client_psk_cb == NULL)
    #else
        if (!ssl->options.resuming)
    #endif
            ssl->earlyData = no_early_data;
    if (ssl->options.serverState == SERVER_HELLO_RETRY_REQUEST_COMPLETE)
        ssl->earlyData = no_early_data;
    if (ssl->earlyData == no_early_data)
        TLSX_Remove(&ssl->extensions, TLSX_EARLY_DATA, ssl->heap);
    if (ssl->earlyData != no_early_data &&
        (ret = TLSX_EarlyData_Use(ssl, 0, 0)) < 0) {
        return ret;
    }
#endif
#ifdef WOLFSSL_QUIC
    if (WOLFSSL_IS_QUIC(ssl) && IsAtLeastTLSv1_3(ssl->version)) {
        ret = wolfSSL_quic_add_transport_extensions(ssl, client_hello);
        if (ret != 0)
            return ret;
    }
#endif

    /* find length of outer and inner */
#if defined(HAVE_ECH)
    if (ssl->options.useEch == 1) {
        TLSX* echX = TLSX_Find(ssl->extensions, TLSX_ECH);
        if (echX == NULL)
            return -1;

        args->ech = (WOLFSSL_ECH*)echX->data;
        if (args->ech == NULL)
            return -1;

        /* set the type to inner */
        args->ech->type = ECH_TYPE_INNER;
        args->preXLength = args->length;

        /* get size for inner */
        ret = TLSX_GetRequestSize(ssl, client_hello, &args->length);
        if (ret != 0)
            return ret;

        /* set the type to outer */
        args->ech->type = 0;
        /* set innerClientHelloLen to ClientHelloInner + padding + tag */
        args->ech->paddingLen = 31 - ((args->length - 1) % 32);
        args->ech->innerClientHelloLen = args->length +
            args->ech->paddingLen + args->ech->hpke->Nt;
        /* set the length back to before we computed ClientHelloInner size */
        args->length = args->preXLength;
    }
#endif

    /* Include length of TLS extensions. */
    ret = TLSX_GetRequestSize(ssl, client_hello, &args->length);
    if (ret != 0)
        return ret;

    /* Total message size. */
    args->sendSz = args->length + HANDSHAKE_HEADER_SZ + RECORD_HEADER_SZ;

#ifdef WOLFSSL_DTLS13
    if (ssl->options.dtls)
        args->sendSz += DTLS_RECORD_EXTRA + DTLS_HANDSHAKE_EXTRA;
#endif /* WOLFSSL_DTLS13 */

    /* Check buffers are big enough and grow if needed. */
    if ((ret = CheckAvailableSize(ssl, args->sendSz)) != 0)
        return ret;

    /* Get position in output buffer to write new message to. */
    args->output = GetOutputBuffer(ssl);

    /* Put the record and handshake headers on. */
    AddTls13Headers(args->output, args->length, client_hello, ssl);

    /* Protocol version - negotiation now in extension: supported_versions. */
    args->output[args->idx++] = major;
    args->output[args->idx++] = tls12minor;

    /* Keep for downgrade. */
    ssl->chVersion = ssl->version;

    if (ssl->arrays == NULL) {
        return BAD_FUNC_ARG;
    }
    /* Client Random */
    if (ssl->options.connectState == CONNECT_BEGIN) {
        ret = wc_RNG_GenerateBlock(ssl->rng, args->output + args->idx, RAN_LEN);
        if (ret != 0)
            return ret;

        /* Store random for possible second ClientHello. */
        XMEMCPY(ssl->arrays->clientRandom, args->output + args->idx, RAN_LEN);
    }
    else
        XMEMCPY(args->output + args->idx, ssl->arrays->clientRandom, RAN_LEN);

#if defined(HAVE_ECH)
    args->clientRandomOffset = args->idx;
#endif

    args->idx += RAN_LEN;

    GetTls13SessionId(ssl, args->output, &args->idx);

#ifdef WOLFSSL_DTLS13
    if (ssl->options.dtls) {
        args->output[args->idx++] = ssl->arrays->cookieSz;

        if (ssl->arrays->cookieSz > 0) {
            /* We have a cookie saved, so the server sent us an
             * HelloVerifyRequest, it means it is a v1.2 server */
            if (!ssl->options.downgrade)
                return VERSION_ERROR;
            XMEMCPY(args->output + args->idx, ssl->arrays->cookie,
                ssl->arrays->cookieSz);
            args->idx += ssl->arrays->cookieSz;
        }
    }
#endif /* WOLFSSL_DTLS13 */

    /* Cipher suites */
    c16toa(suites->suiteSz, args->output + args->idx);
    args->idx += OPAQUE16_LEN;
    XMEMCPY(args->output + args->idx, &suites->suites,
        suites->suiteSz);
    args->idx += suites->suiteSz;
#ifdef WOLFSSL_DEBUG_TLS
    {
        int ii;
        WOLFSSL_MSG("Ciphers:");
        for (ii = 0 ; ii < suites->suiteSz; ii += 2) {
            WOLFSSL_MSG(GetCipherNameInternal(suites->suites[ii+0],
                                              suites->suites[ii+1]));
        }
    }
#endif

    /* Compression not supported in TLS v1.3. */
    args->output[args->idx++] = COMP_LEN;
    args->output[args->idx++] = NO_COMPRESSION;

#if defined(HAVE_ECH)
    /* write inner then outer */
    if (ssl->options.useEch == 1) {
        /* set the type to inner */
        args->ech->type = ECH_TYPE_INNER;

        /* allocate the inner */
        args->ech->innerClientHello =
            (byte*)XMALLOC(args->ech->innerClientHelloLen - args->ech->hpke->Nt,
            ssl->heap, DYNAMIC_TYPE_TMP_BUFFER);
        if (args->ech->innerClientHello == NULL)
            return MEMORY_E;

        /* set the padding bytes to 0 */
        XMEMSET(args->ech->innerClientHello + args->ech->innerClientHelloLen -
            args->ech->hpke->Nt - args->ech->paddingLen, 0,
            args->ech->paddingLen);

        /* copy the client hello to the ech innerClientHello, exclude record */
        /* and handshake headers */
        XMEMCPY(args->ech->innerClientHello,
            args->output + RECORD_HEADER_SZ + HANDSHAKE_HEADER_SZ,
            args->idx - (RECORD_HEADER_SZ + HANDSHAKE_HEADER_SZ));

        /* copy the client random to inner */
        XMEMCPY(ssl->arrays->clientRandomInner, ssl->arrays->clientRandom,
            RAN_LEN);

        /* change the outer client random */
        ret = wc_RNG_GenerateBlock(ssl->rng, args->output +
            args->clientRandomOffset, RAN_LEN);
        if (ret != 0)
            return ret;

        /* copy the new client random */
        XMEMCPY(ssl->arrays->clientRandom, args->output +
            args->clientRandomOffset, RAN_LEN);

        /* write the extensions for inner */
        args->length = 0;
        ret = TLSX_WriteRequest(ssl, args->ech->innerClientHello + args->idx -
            (RECORD_HEADER_SZ + HANDSHAKE_HEADER_SZ), client_hello,
            &args->length);
        if (ret != 0)
            return ret;

        /* set the type to outer */
        args->ech->type = 0;
    }
#endif

    /* Write out extensions for a request. */
    args->length = 0;
    ret = TLSX_WriteRequest(ssl, args->output + args->idx, client_hello,
        &args->length);
    if (ret != 0)
        return ret;

    args->idx += args->length;

#if defined(HAVE_ECH)
    /* encrypt and pack the ech innerClientHello */
    if (ssl->options.useEch == 1) {
        ret = TLSX_FinalizeEch(args->ech,
            args->output + RECORD_HEADER_SZ + HANDSHAKE_HEADER_SZ,
            args->sendSz - (RECORD_HEADER_SZ + HANDSHAKE_HEADER_SZ));

        if (ret != 0)
            return ret;
    }
#endif

#if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
    /* Resumption has a specific set of extensions and binder is calculated
     * for each identity.
     */
    if (TLSX_Find(ssl->extensions, TLSX_PRE_SHARED_KEY)) {
        ret = WritePSKBinders(ssl, args->output, args->idx);
    }
    else
#endif
    {
#ifdef WOLFSSL_DTLS13
        if (ssl->options.dtls)
            ret = Dtls13HashHandshake(ssl,
                args->output + Dtls13GetRlHeaderLength(ssl, 0),
                (word16)args->idx - Dtls13GetRlHeaderLength(ssl, 0));
        else
#endif /* WOLFSSL_DTLS13 */
        {
#if defined(HAVE_ECH)
            /* compute the inner hash */
            if (ssl->options.useEch == 1) {
                ret = EchHashHelloInner(ssl, args->ech);
            }
#endif

            /* compute the outer hash */
            if (ret == 0)
                ret = HashOutput(ssl, args->output, args->idx, 0);
        }
    }
    if (ret != 0)
        return ret;

    ssl->options.clientState = CLIENT_HELLO_COMPLETE;

#if defined(WOLFSSL_CALLBACKS) || defined(OPENSSL_EXTRA)
    if (ssl->hsInfoOn) AddPacketName(ssl, "ClientHello");
    if (ssl->toInfoOn) {
        ret = AddPacketInfo(ssl, "ClientHello", handshake, args->output,
                      args->sendSz, WRITE_PROTO, 0, ssl->heap);
        if (ret != 0)
            return ret;
    }
#endif

    ssl->options.buildingMsg = 0;
#ifdef WOLFSSL_DTLS13
    if (ssl->options.dtls) {
        ret = Dtls13HandshakeSend(ssl, args->output, (word16)args->sendSz,
                                  (word16)args->idx, client_hello, 0);
        break;
    }
#endif /* WOLFSSL_DTLS13 */

    ssl->buffers.outputBuffer.length += args->sendSz;

    /* Advance state and proceed */
    ssl->options.asyncState = TLS_ASYNC_END;
    }
    /* case TLS_ASYNC_BUILD */
    FALL_THROUGH;

    case TLS_ASYNC_END:
    {
#ifdef WOLFSSL_EARLY_DATA_GROUP
    /* QUIC needs to forward records at their encryption level
     * and is therefore unable to group here */
    if (ssl->earlyData == no_early_data || WOLFSSL_IS_QUIC(ssl))
#endif
        ret = SendBuffered(ssl);

    break;
    }
    default:
        ret = INPUT_CASE_ERROR;
    } /* switch (ssl->options.asyncState) */

#ifdef WOLFSSL_ASYNC_CRYPT
    if (ret == 0)
        FreeAsyncCtx(ssl, 0);
#endif

    WOLFSSL_LEAVE("SendTls13ClientHello", ret);
    WOLFSSL_END(WC_FUNC_CLIENT_HELLO_SEND);

    return ret;
}

#if defined(WOLFSSL_DTLS13) && !defined(WOLFSSL_NO_CLIENT)
static int Dtls13DoDowngrade(WOLFSSL* ssl)
{
    int ret;
    if (ssl->dtls13ClientHello == NULL)
        return BAD_STATE_E;

    /* v1.3 and v1.2 hash messages to compute the transcript hash. When we are
     * using DTLSv1.3 we hash the first clientHello following v1.3 but the
     * server can negotiate a lower version. So we need to re-hash the
     * clientHello to adhere to DTLS <= v1.2 rules. */
    ret = InitHandshakeHashes(ssl);
    if (ret != 0)
        return ret;
    ret = HashRaw(ssl, ssl->dtls13ClientHello, ssl->dtls13ClientHelloSz);
    XFREE(ssl->dtls13ClientHello, ssl->heap, DYNAMIC_TYPE_DTLS_MSG);
    ssl->dtls13ClientHello = NULL;
    ssl->dtls13ClientHelloSz = 0;
    ssl->keys.dtls_sequence_number_hi =
        (word16)w64GetHigh32(ssl->dtls13EncryptEpoch->nextSeqNumber);
    ssl->keys.dtls_sequence_number_lo =
        w64GetLow32(ssl->dtls13EncryptEpoch->nextSeqNumber);
    return ret;
}
#endif /* WOLFSSL_DTLS13 && !WOLFSSL_NO_CLIENT*/

#if defined(HAVE_ECH)
/* check if the server accepted ech or not */
static int EchCheckAcceptance(WOLFSSL* ssl, const byte* input,
    int serverRandomOffset, int helloSz)
{
    int ret = 0;
    int digestType;
    int digestSize;
    HS_Hashes* tmpHashes;
    HS_Hashes* acceptHashes;
    byte zeros[WC_MAX_DIGEST_SIZE] = {0};
    byte transcriptEchConf[WC_MAX_DIGEST_SIZE];
    byte expandLabelPrk[WC_MAX_DIGEST_SIZE];
    byte acceptConfirmation[ECH_ACCEPT_CONFIRMATION_SZ];

    /* copy ech hashes to accept */
    ret = InitHandshakeHashesAndCopy(ssl, ssl->hsHashesEch, &acceptHashes);

    /* swap hsHashes to acceptHashes */
    tmpHashes = ssl->hsHashes;
    ssl->hsHashes = acceptHashes;

    /* hash up to the last 8 bytes */
    if (ret == 0)
        ret = HashRaw(ssl, input, serverRandomOffset + RAN_LEN -
            ECH_ACCEPT_CONFIRMATION_SZ);

    /* hash 8 zeros */
    if (ret == 0)
        ret = HashRaw(ssl, zeros, ECH_ACCEPT_CONFIRMATION_SZ);

    /* hash the rest of the hello */
    if (ret == 0)
        ret = HashRaw(ssl, input + serverRandomOffset + RAN_LEN,
            helloSz + HANDSHAKE_HEADER_SZ - (serverRandomOffset + RAN_LEN));

    /* get the modified transcript hash */
    if (ret == 0)
        ret = GetMsgHash(ssl, transcriptEchConf);

    if (ret > 0)
        ret = 0;

    /* pick the right type and size based on mac_algorithm */
    if (ret == 0)
        switch (ssl->specs.mac_algorithm) {
#ifndef NO_SHA256
            case sha256_mac:
                digestType = WC_SHA256;
                digestSize = WC_SHA256_DIGEST_SIZE;
                break;
#endif /* !NO_SHA256 */
#ifdef WOLFSSL_SHA384
            case sha384_mac:
                digestType = WC_SHA384;
                digestSize = WC_SHA384_DIGEST_SIZE;
                break;
#endif /* WOLFSSL_SHA384 */
#ifdef WOLFSSL_TLS13_SHA512
            case sha512_mac:
                digestType = WC_SHA512;
                digestSize = WC_SHA512_DIGEST_SIZE;
                break;
#endif /* WOLFSSL_TLS13_SHA512 */
#ifdef WOLFSSL_SM3
            case sm3_mac:
                digestType = WC_SM3;
                digestSize = WC_SM3_DIGEST_SIZE;
                break;
#endif /* WOLFSSL_SM3 */
            default:
                ret = -1;
                break;
        }

    /* extract clientRandomInner with a key of all zeros */
    if (ret == 0)
        ret = wc_HKDF_Extract(digestType, zeros, digestSize,
            ssl->arrays->clientRandomInner, RAN_LEN, expandLabelPrk);

    /* tls expand with the confirmation label */
    if (ret == 0)
        ret = wc_Tls13_HKDF_Expand_Label(acceptConfirmation,
            ECH_ACCEPT_CONFIRMATION_SZ,
            expandLabelPrk, digestSize, tls13ProtocolLabel,
            TLS13_PROTOCOL_LABEL_SZ, echAcceptConfirmationLabel,
            ECH_ACCEPT_CONFIRMATION_LABEL_SZ, transcriptEchConf, digestSize,
            digestType);

    if (ret == 0) {
        /* last 8 bytes should match our expand output */
        ret = XMEMCMP(acceptConfirmation,
            ssl->arrays->serverRandom + RAN_LEN - ECH_ACCEPT_CONFIRMATION_SZ,
            ECH_ACCEPT_CONFIRMATION_SZ);

        /* ech accepted */
        if (ret == 0) {
            /* use the inner random for client random */
            XMEMCPY(ssl->arrays->clientRandom, ssl->arrays->clientRandomInner,
              RAN_LEN);

            /* switch back to original hsHashes */
            ssl->hsHashes = tmpHashes;

            /* free hsHashes */
            FreeHandshakeHashes(ssl);

            /* set the final hsHashes to the ech hashes */
            tmpHashes = ssl->hsHashesEch;

            /* set hsHashesEch to NULL to avoid double free */
            ssl->hsHashesEch = NULL;
        }
        /* ech rejected */
        else {
            /* switch to hsHashesEch */
            ssl->hsHashes = ssl->hsHashesEch;

            /* free ech hashes */
            FreeHandshakeHashes(ssl);
        }

        /* continue with outer if we failed to verify ech was accepted */
        ret = 0;
    }

    /* switch to acceptHashes */
    ssl->hsHashes = acceptHashes;

    /* free acceptHashes */
    FreeHandshakeHashes(ssl);

    ssl->hsHashes = tmpHashes;

    return ret;
}

/* replace the last 8 bytes of the server random with the ech acceptance
 * parameter, return status */
static int EchWriteAcceptance(WOLFSSL* ssl, byte* output,
  int serverRandomOffset, int helloSz)
{
    int ret = 0;
    int digestType;
    int digestSize;
    HS_Hashes* tmpHashes;
    HS_Hashes* acceptHashes;
    byte zeros[WC_MAX_DIGEST_SIZE] = {0};
    byte transcriptEchConf[WC_MAX_DIGEST_SIZE];
    byte expandLabelPrk[WC_MAX_DIGEST_SIZE];

    /* copy ech hashes to accept */
    ret = InitHandshakeHashesAndCopy(ssl, ssl->hsHashes, &acceptHashes);

    /* swap hsHashes to acceptHashes */
    tmpHashes = ssl->hsHashes;
    ssl->hsHashes = acceptHashes;

    /* hash up to the last 8 bytes */
    if (ret == 0)
        ret = HashRaw(ssl, output, serverRandomOffset + RAN_LEN -
            ECH_ACCEPT_CONFIRMATION_SZ);

    /* hash 8 zeros */
    if (ret == 0)
        ret = HashRaw(ssl, zeros, ECH_ACCEPT_CONFIRMATION_SZ);

    /* hash the rest of the hello */
    if (ret == 0)
        ret = HashRaw(ssl, output + serverRandomOffset + RAN_LEN,
            helloSz - (serverRandomOffset + RAN_LEN));

    /* get the modified transcript hash */
    if (ret == 0)
        ret = GetMsgHash(ssl, transcriptEchConf);

    if (ret > 0)
        ret = 0;

    /* pick the right type and size based on mac_algorithm */
    if (ret == 0)
        switch (ssl->specs.mac_algorithm) {
#ifndef NO_SHA256
            case sha256_mac:
                digestType = WC_SHA256;
                digestSize = WC_SHA256_DIGEST_SIZE;
                break;
#endif /* !NO_SHA256 */
#ifdef WOLFSSL_SHA384
            case sha384_mac:
                digestType = WC_SHA384;
                digestSize = WC_SHA384_DIGEST_SIZE;
                break;
#endif /* WOLFSSL_SHA384 */
#ifdef WOLFSSL_TLS13_SHA512
            case sha512_mac:
                digestType = WC_SHA512;
                digestSize = WC_SHA512_DIGEST_SIZE;
                break;
#endif /* WOLFSSL_TLS13_SHA512 */
#ifdef WOLFSSL_SM3
            case sm3_mac:
                digestType = WC_SM3;
                digestSize = WC_SM3_DIGEST_SIZE;
                break;
#endif /* WOLFSSL_SM3 */
            default:
                ret = -1;
                break;
        }

    /* extract clientRandom with a key of all zeros */
    if (ret == 0) {
        PRIVATE_KEY_UNLOCK();
        ret = wc_HKDF_Extract(digestType, zeros, digestSize,
            ssl->arrays->clientRandom, RAN_LEN, expandLabelPrk);
        PRIVATE_KEY_LOCK();
    }

    /* tls expand with the confirmation label */
    if (ret == 0) {
        PRIVATE_KEY_UNLOCK();
        ret = wc_Tls13_HKDF_Expand_Label(
            output + serverRandomOffset + RAN_LEN - ECH_ACCEPT_CONFIRMATION_SZ,
            ECH_ACCEPT_CONFIRMATION_SZ,
            expandLabelPrk, digestSize, tls13ProtocolLabel,
            TLS13_PROTOCOL_LABEL_SZ, echAcceptConfirmationLabel,
            ECH_ACCEPT_CONFIRMATION_LABEL_SZ, transcriptEchConf, digestSize,
            digestType);
        PRIVATE_KEY_LOCK();
    }

    if (ret == 0)
        XMEMCPY(ssl->arrays->serverRandom, output + serverRandomOffset,
            RAN_LEN);

    /* free acceptHashes */
    FreeHandshakeHashes(ssl);

    ssl->hsHashes = tmpHashes;

    return ret;
}
#endif

/* handle processing of TLS 1.3 server_hello (2) and hello_retry_request (6) */
/* Handle the ServerHello message from the server.
 * Only a client will receive this message.
 *
 * ssl       The SSL/TLS object.
 * input     The message buffer.
 * inOutIdx  On entry, the index into the message buffer of ServerHello.
 *           On exit, the index of byte after the ServerHello message.
 * helloSz   The length of the current handshake message.
 * returns 0 on success and otherwise failure.
 */

typedef struct Dsh13Args {
    ProtocolVersion pv;
    word32          idx;
    word32          begin;
    const byte*     sessId;
    word16          totalExtSz;
    byte            sessIdSz;
    byte            extMsgType;
#if defined(HAVE_ECH)
    int             serverRandomOffset;
#endif
} Dsh13Args;

int DoTls13ServerHello(WOLFSSL* ssl, const byte* input, word32* inOutIdx,
                       word32 helloSz, byte* extMsgType)
{
    int ret;
    byte suite[2];
    byte tls12minor;
#ifdef WOLFSSL_ASYNC_CRYPT
    Dsh13Args* args = NULL;
    WOLFSSL_ASSERT_SIZEOF_GE(ssl->async->args, *args);
#else
    Dsh13Args  args[1];
#endif

    WOLFSSL_START(WC_FUNC_SERVER_HELLO_DO);
    WOLFSSL_ENTER("DoTls13ServerHello");

    tls12minor = TLSv1_2_MINOR;

#ifdef WOLFSSL_DTLS13
    if (ssl->options.dtls)
        tls12minor = DTLSv1_2_MINOR;
#endif /*  WOLFSSL_DTLS13 */


    if (ssl == NULL || ssl->arrays == NULL)
        return BAD_FUNC_ARG;

#ifdef WOLFSSL_ASYNC_CRYPT
    if (ssl->async == NULL) {
        ssl->async = (struct WOLFSSL_ASYNC*)
                XMALLOC(sizeof(struct WOLFSSL_ASYNC), ssl->heap,
                        DYNAMIC_TYPE_ASYNC);
        if (ssl->async == NULL)
            return MEMORY_E;
        ssl->async->freeArgs = NULL;
    }
    args = (Dsh13Args*)ssl->async->args;

    ret = wolfSSL_AsyncPop(ssl, &ssl->options.asyncState);
    if (ret != WC_NOT_PENDING_E) {
        /* Check for error */
        if (ret < 0) {
            if (ret == WC_PENDING_E) {
                /* Mark message as not received so it can process again */
                ssl->msgsReceived.got_server_hello = 0;
            }
            return ret;
        }
    }
    else
#endif
    {
        /* Reset state */
        ssl->options.asyncState = TLS_ASYNC_BEGIN;
        XMEMSET(args, 0, sizeof(Dsh13Args));
    }

    switch (ssl->options.asyncState) {
    case TLS_ASYNC_BEGIN:
    {
    byte b;
#ifdef WOLFSSL_CALLBACKS
    if (ssl->hsInfoOn) AddPacketName(ssl, "ServerHello");
    if (ssl->toInfoOn) AddLateName("ServerHello", &ssl->timeoutInfo);
#endif

    /* Protocol version length check. */
    if (helloSz < OPAQUE16_LEN)
        return BUFFER_ERROR;

    args->idx = *inOutIdx;
    args->begin = args->idx;

    /* Protocol version */
    XMEMCPY(&args->pv, input + args->idx, OPAQUE16_LEN);
    args->idx += OPAQUE16_LEN;

#ifdef WOLFSSL_DTLS
    if (ssl->options.dtls &&
        (args->pv.major != DTLS_MAJOR || args->pv.minor == DTLS_BOGUS_MINOR))
        return VERSION_ERROR;
#endif /* WOLFSSL_DTLS */

#ifndef WOLFSSL_NO_TLS12
    {
        byte wantDowngrade;

        wantDowngrade = args->pv.major == ssl->version.major &&
            args->pv.minor < TLSv1_2_MINOR;

#ifdef WOLFSSL_DTLS13
        if (ssl->options.dtls)
            wantDowngrade = args->pv.major == ssl->version.major &&
                args->pv.minor > DTLSv1_2_MINOR;
#endif /* WOLFSSL_DTLS13 */

        if (wantDowngrade && ssl->options.downgrade) {
            /* Force client hello version 1.2 to work for static RSA. */
            ssl->chVersion.minor = TLSv1_2_MINOR;
            ssl->version.minor = TLSv1_2_MINOR;

#ifdef WOLFSSL_DTLS13
            if (ssl->options.dtls) {
                ssl->chVersion.minor = DTLSv1_2_MINOR;
                ssl->version.minor = DTLSv1_2_MINOR;
                ret = Dtls13DoDowngrade(ssl);
                if (ret != 0)
                    return ret;
            }
#endif /* WOLFSSL_DTLS13 */

            return DoServerHello(ssl, input, inOutIdx, helloSz);
        }
    }
#endif

    if (args->pv.major != ssl->version.major ||
        args->pv.minor != tls12minor) {
        SendAlert(ssl, alert_fatal, wolfssl_alert_protocol_version);
        WOLFSSL_ERROR_VERBOSE(VERSION_ERROR);
        return VERSION_ERROR;
    }

    /* Random and session id length check */
    if ((args->idx - args->begin) + RAN_LEN + ENUM_LEN > helloSz)
        return BUFFER_ERROR;

    /* Check if hello retry request */
    if (XMEMCMP(input + args->idx, helloRetryRequestRandom, RAN_LEN) == 0) {
        WOLFSSL_MSG("HelloRetryRequest format");
        *extMsgType = hello_retry_request;

        /* A HelloRetryRequest comes in as an ServerHello for MiddleBox compat.
         * Found message to be a HelloRetryRequest.
         * Don't allow more than one HelloRetryRequest or ServerHello.
         */
        if (ssl->msgsReceived.got_hello_retry_request) {
            WOLFSSL_ERROR_VERBOSE(DUPLICATE_MSG_E);
            return DUPLICATE_MSG_E;
        }
    }
    args->extMsgType = *extMsgType;

    /* Server random - keep for debugging. */
    XMEMCPY(ssl->arrays->serverRandom, input + args->idx, RAN_LEN);
#if defined(HAVE_ECH)
    args->serverRandomOffset = args->idx;
#endif
    args->idx += RAN_LEN;

    /* Session id */
    args->sessIdSz = input[args->idx++];
    if ((args->idx - args->begin) + args->sessIdSz > helloSz)
        return BUFFER_ERROR;
    args->sessId = input + args->idx;
    args->idx += args->sessIdSz;

    ssl->options.haveSessionId = 1;

    /* Ciphersuite and compression check */
    if ((args->idx - args->begin) + OPAQUE16_LEN + OPAQUE8_LEN > helloSz)
        return BUFFER_ERROR;

    /* Set the cipher suite from the message. */
    ssl->options.cipherSuite0 = input[args->idx++];
    ssl->options.cipherSuite  = input[args->idx++];
#ifdef WOLFSSL_DEBUG_TLS
    WOLFSSL_MSG("Chosen cipher suite:");
    WOLFSSL_MSG(GetCipherNameInternal(ssl->options.cipherSuite0,
                                      ssl->options.cipherSuite));
#endif

    /* Compression */
    b = input[args->idx++];
    if (b != 0) {
        WOLFSSL_MSG("Must be no compression types in list");
        WOLFSSL_ERROR_VERBOSE(INVALID_PARAMETER);
        return INVALID_PARAMETER;
    }

    if ((args->idx - args->begin) + OPAQUE16_LEN > helloSz) {
        if (!ssl->options.downgrade)
            return BUFFER_ERROR;
#ifndef WOLFSSL_NO_TLS12
        /* Force client hello version 1.2 to work for static RSA. */
        ssl->chVersion.minor = TLSv1_2_MINOR;
        ssl->version.minor = TLSv1_2_MINOR;

#ifdef WOLFSSL_DTLS13
        if (ssl->options.dtls) {
            ssl->chVersion.minor = DTLSv1_2_MINOR;
            ssl->version.minor = DTLSv1_2_MINOR;
            ret = Dtls13DoDowngrade(ssl);
            if (ret != 0)
                return ret;
        }
#endif /* WOLFSSL_DTLS13 */

#endif
        ssl->options.haveEMS = 0;
        if (args->pv.minor < ssl->options.minDowngrade) {
            SendAlert(ssl, alert_fatal, wolfssl_alert_protocol_version);
            return VERSION_ERROR;
        }
#ifndef WOLFSSL_NO_TLS12
        return DoServerHello(ssl, input, inOutIdx, helloSz);
#else
        SendAlert(ssl, alert_fatal, wolfssl_alert_protocol_version);
        return VERSION_ERROR;
#endif
    }

    if ((args->idx - args->begin) < helloSz) {
        int foundVersion;

        /* Get extension length and length check. */
        if ((args->idx - args->begin) + OPAQUE16_LEN > helloSz)
            return BUFFER_ERROR;
        ato16(&input[args->idx], &args->totalExtSz);
        args->idx += OPAQUE16_LEN;
        if ((args->idx - args->begin) + args->totalExtSz > helloSz)
            return BUFFER_ERROR;

        /* Need to negotiate version first. */
        if ((ret = TLSX_ParseVersion(ssl, input + args->idx,
            args->totalExtSz, *extMsgType, &foundVersion))) {
            return ret;
        }
        if (!foundVersion) {
            if (!ssl->options.downgrade) {
                WOLFSSL_MSG("Server trying to downgrade to version less than "
                            "TLS v1.3");
                SendAlert(ssl, alert_fatal, wolfssl_alert_protocol_version);
                WOLFSSL_ERROR_VERBOSE(VERSION_ERROR);
                return VERSION_ERROR;
            }
#if defined(OPENSSL_EXTRA) || defined(HAVE_WEBSERVER) || \
    defined(WOLFSSL_WPAS_SMALL)
            /* Check if client has disabled TLS 1.2 */
            if (args->pv.minor == TLSv1_2_MINOR &&
                (ssl->options.mask & SSL_OP_NO_TLSv1_2) == SSL_OP_NO_TLSv1_2) {
                WOLFSSL_MSG("\tOption set to not allow TLSv1.2");
                WOLFSSL_ERROR_VERBOSE(VERSION_ERROR);
                return VERSION_ERROR;
            }
#endif

            if (!ssl->options.dtls &&
                args->pv.minor < ssl->options.minDowngrade) {
                SendAlert(ssl, alert_fatal, wolfssl_alert_protocol_version);
                WOLFSSL_ERROR_VERBOSE(VERSION_ERROR);
                return VERSION_ERROR;
            }

            if (ssl->options.dtls &&
                args->pv.minor > ssl->options.minDowngrade) {
                SendAlert(ssl, alert_fatal, wolfssl_alert_protocol_version);
                WOLFSSL_ERROR_VERBOSE(VERSION_ERROR);
                return VERSION_ERROR;
            }

            ssl->version.minor = args->pv.minor;

#ifdef WOLFSSL_DTLS13
            if (ssl->options.dtls) {
                ret = Dtls13DoDowngrade(ssl);
                if (ret != 0)
                    return ret;
            }
#endif /* WOLFSSL_DTLS13 */
        }
    }

#ifdef WOLFSSL_DTLS13
    /* we are sure that version is >= v1.3 now, we can get rid of buffered
     * ClientHello that was buffered to re-compute the hash in case of
     * downgrade */
    if (ssl->options.dtls && ssl->dtls13ClientHello != NULL) {
        XFREE(ssl->dtls13ClientHello, ssl->heap, DYNAMIC_TYPE_DTLS_MSG);
        ssl->dtls13ClientHello = NULL;
        ssl->dtls13ClientHelloSz = 0;
    }
#endif /* WOLFSSL_DTLS13 */

    /* Advance state and proceed */
    ssl->options.asyncState = TLS_ASYNC_BUILD;
    } /* case TLS_ASYNC_BEGIN */
    FALL_THROUGH;

    case TLS_ASYNC_BUILD:
    case TLS_ASYNC_DO:
    {
    /* restore message type */
    *extMsgType = args->extMsgType;

    if (args->totalExtSz > 0) {
        /* Parse and handle extensions. */
        ret = TLSX_Parse(ssl, input + args->idx, args->totalExtSz,
            *extMsgType, NULL);
        if (ret != 0) {
        #ifdef WOLFSSL_ASYNC_CRYPT
            /* Handle async operation */
            if (ret == WC_PENDING_E) {
                /* Mark message as not received so it can process again */
                ssl->msgsReceived.got_server_hello = 0;
            }
        #endif
            return ret;
        }

        if (*extMsgType == hello_retry_request) {
            /* Update counts to reflect change of message type. */
            ssl->msgsReceived.got_hello_retry_request = 1;
            ssl->msgsReceived.got_server_hello = 0;
        }

        args->idx += args->totalExtSz;
    }

#ifdef WOLFSSL_DTLS_CID
    if (ssl->options.useDtlsCID && *extMsgType == server_hello)
        DtlsCIDOnExtensionsParsed(ssl);
#endif /* WOLFSSL_DTLS_CID */

    *inOutIdx = args->idx;

    ssl->options.serverState = SERVER_HELLO_COMPLETE;

#ifdef HAVE_SECRET_CALLBACK
    if (ssl->sessionSecretCb != NULL
#ifdef HAVE_SESSION_TICKET
            && ssl->session->ticketLen > 0
#endif
            ) {
        int secretSz = SECRET_LEN;
        ret = ssl->sessionSecretCb(ssl, ssl->session->masterSecret,
                                   &secretSz, ssl->sessionSecretCtx);
        if (ret != 0 || secretSz != SECRET_LEN) {
            WOLFSSL_ERROR_VERBOSE(SESSION_SECRET_CB_E);
            return SESSION_SECRET_CB_E;
        }
    }
#endif /* HAVE_SECRET_CALLBACK */

    /* Version only negotiated in extensions for TLS v1.3.
     * Only now do we know how to deal with session id.
     */
    if (!IsAtLeastTLSv1_3(ssl->version)) {
#ifndef WOLFSSL_NO_TLS12
        ssl->arrays->sessionIDSz = args->sessIdSz;

        if (ssl->arrays->sessionIDSz > ID_LEN) {
            WOLFSSL_MSG("Invalid session ID size");
            ssl->arrays->sessionIDSz = 0;
            return BUFFER_ERROR;
        }
        else if (ssl->arrays->sessionIDSz) {
            XMEMCPY(ssl->arrays->sessionID, args->sessId,
                ssl->arrays->sessionIDSz);
            ssl->options.haveSessionId = 1;
        }

        /* Force client hello version 1.2 to work for static RSA. */
        ssl->chVersion.minor = TLSv1_2_MINOR;
        /* Complete TLS v1.2 processing of ServerHello. */
        ret = CompleteServerHello(ssl);
#else
        WOLFSSL_MSG("Client using higher version, fatal error");
        WOLFSSL_ERROR_VERBOSE(VERSION_ERROR);
        ret = VERSION_ERROR;
#endif

        WOLFSSL_LEAVE("DoTls13ServerHello", ret);

        return ret;
    }

    /* Advance state and proceed */
    ssl->options.asyncState = TLS_ASYNC_FINALIZE;
    } /* case TLS_ASYNC_BUILD || TLS_ASYNC_DO */
    FALL_THROUGH;

    case TLS_ASYNC_FINALIZE:
    {
#ifdef WOLFSSL_TLS13_MIDDLEBOX_COMPAT
    if (ssl->options.tls13MiddleBoxCompat) {
        if (args->sessIdSz == 0) {
            WOLFSSL_MSG("args->sessIdSz == 0");
            WOLFSSL_ERROR_VERBOSE(INVALID_PARAMETER);
            return INVALID_PARAMETER;
        }
        if (ssl->session->sessionIDSz != 0) {
            if (ssl->session->sessionIDSz != args->sessIdSz ||
                XMEMCMP(ssl->session->sessionID, args->sessId,
                    args->sessIdSz) != 0) {
                WOLFSSL_MSG("session id doesn't match");
                WOLFSSL_ERROR_VERBOSE(INVALID_PARAMETER);
                return INVALID_PARAMETER;
            }
        }
        else if (XMEMCMP(ssl->arrays->clientRandom, args->sessId,
                args->sessIdSz) != 0) {
            WOLFSSL_MSG("session id doesn't match client random");
            WOLFSSL_ERROR_VERBOSE(INVALID_PARAMETER);
            return INVALID_PARAMETER;
        }
    }
    else
#endif /* WOLFSSL_TLS13_MIDDLEBOX_COMPAT */
#ifdef WOLFSSL_QUIC
    if (WOLFSSL_IS_QUIC(ssl)) {
        if (args->sessIdSz != 0) {
            WOLFSSL_MSG("args->sessIdSz != 0");
            WOLFSSL_ERROR_VERBOSE(INVALID_PARAMETER);
            return INVALID_PARAMETER;
        }
    }
    else
#endif /* WOLFSSL_QUIC */
    if (args->sessIdSz != ssl->session->sessionIDSz || (args->sessIdSz > 0 &&
        XMEMCMP(ssl->session->sessionID, args->sessId, args->sessIdSz) != 0))
    {
        WOLFSSL_MSG("Server sent different session id");
        WOLFSSL_ERROR_VERBOSE(INVALID_PARAMETER);
        return INVALID_PARAMETER;
    }

    ret = SetCipherSpecs(ssl);
    if (ret != 0)
        return ret;

#if defined(HAVE_ECH)
    /* check for acceptConfirmation and HashInput with 8 0 bytes */
    if (ssl->options.useEch == 1) {
        ret = EchCheckAcceptance(ssl, input, args->serverRandomOffset, helloSz);
        if (ret != 0)
            return ret;
    }
#endif

#ifdef HAVE_NULL_CIPHER
    if (ssl->options.cipherSuite0 == ECC_BYTE &&
                              (ssl->options.cipherSuite == TLS_SHA256_SHA256 ||
                               ssl->options.cipherSuite == TLS_SHA384_SHA384)) {
        ;
    }
    else
#endif
#if defined(WOLFSSL_SM4_GCM) && defined(WOLFSSL_SM3)
    if (ssl->options.cipherSuite0 == CIPHER_BYTE &&
            ssl->options.cipherSuite == TLS_SM4_GCM_SM3) {
        ; /* Do nothing. */
    }
    else
#endif
#if defined(WOLFSSL_SM4_CCM) && defined(WOLFSSL_SM3)
    if (ssl->options.cipherSuite0 == CIPHER_BYTE &&
            ssl->options.cipherSuite == TLS_SM4_CCM_SM3) {
        ; /* Do nothing. */
    }
    else
#endif
    /* Check that the negotiated ciphersuite matches protocol version. */
    if (ssl->options.cipherSuite0 != TLS13_BYTE) {
        WOLFSSL_MSG("Server sent non-TLS13 cipher suite in TLS 1.3 packet");
        WOLFSSL_ERROR_VERBOSE(INVALID_PARAMETER);
        return INVALID_PARAMETER;
    }

    suite[0] = ssl->options.cipherSuite0;
    suite[1] = ssl->options.cipherSuite;
    if (!FindSuiteSSL(ssl, suite)) {
        WOLFSSL_MSG("Cipher suite not supported on client");
        WOLFSSL_ERROR_VERBOSE(MATCH_SUITE_ERROR);
        return MATCH_SUITE_ERROR;
    }

    if (*extMsgType == server_hello) {
#if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
        PreSharedKey* psk = NULL;
        TLSX* ext = TLSX_Find(ssl->extensions, TLSX_PRE_SHARED_KEY);
        if (ext != NULL)
            psk = (PreSharedKey*)ext->data;
        while (psk != NULL && !psk->chosen)
            psk = psk->next;
        if (psk == NULL) {
            ssl->options.resuming = 0;
            ssl->arrays->psk_keySz = 0;
            XMEMSET(ssl->arrays->psk_key, 0, MAX_PSK_KEY_LEN);
        }
        else {
            if ((ret = SetupPskKey(ssl, psk, 0)) != 0)
                return ret;
            ssl->options.pskNegotiated = 1;
        }
#endif

        /* sanity check on PSK / KSE */
        if (
    #if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
            ssl->options.pskNegotiated == 0 &&
    #endif
            ssl->session->namedGroup == 0) {
            return EXT_MISSING;
        }

        ssl->keys.encryptionOn = 1;
        ssl->options.serverState = SERVER_HELLO_COMPLETE;

    }
    else {
        ssl->options.tls1_3 = 1;
        ssl->options.serverState = SERVER_HELLO_RETRY_REQUEST_COMPLETE;

        ret = RestartHandshakeHash(ssl);
    }

    break;
    } /* case TLS_ASYNC_FINALIZE */
    default:
        ret = INPUT_CASE_ERROR;
    } /* switch (ssl->options.asyncState) */

#ifdef WOLFSSL_ASYNC_CRYPT
    if (ret == 0)
        FreeAsyncCtx(ssl, 0);
#endif

    WOLFSSL_LEAVE("DoTls13ServerHello", ret);
    WOLFSSL_END(WC_FUNC_SERVER_HELLO_DO);

    return ret;
}

/* handle processing TLS 1.3 encrypted_extensions (8) */
/* Parse and handle an EncryptedExtensions message.
 * Only a client will receive this message.
 *
 * ssl       The SSL/TLS object.
 * input     The message buffer.
 * inOutIdx  On entry, the index into the message buffer of
 *           EncryptedExtensions.
 *           On exit, the index of byte after the EncryptedExtensions
 *           message.
 * totalSz   The length of the current handshake message.
 * returns 0 on success and otherwise failure.
 */
static int DoTls13EncryptedExtensions(WOLFSSL* ssl, const byte* input,
                                      word32* inOutIdx, word32 totalSz)
{
    int    ret;
    word32 begin = *inOutIdx;
    word32 i = begin;
    word16 totalExtSz;

    WOLFSSL_START(WC_FUNC_ENCRYPTED_EXTENSIONS_DO);
    WOLFSSL_ENTER("DoTls13EncryptedExtensions");

#ifdef WOLFSSL_CALLBACKS
    if (ssl->hsInfoOn) AddPacketName(ssl, "EncryptedExtensions");
    if (ssl->toInfoOn) AddLateName("EncryptedExtensions", &ssl->timeoutInfo);
#endif

    /* Length field of extension data. */
    if (totalSz < OPAQUE16_LEN)
        return BUFFER_ERROR;
    ato16(&input[i], &totalExtSz);
    i += OPAQUE16_LEN;

    /* Extension data. */
    if (i - begin + totalExtSz > totalSz)
        return BUFFER_ERROR;
    if ((ret = TLSX_Parse(ssl, input + i, totalExtSz, encrypted_extensions,
                                                                       NULL))) {
        return ret;
    }

    /* Move index to byte after message. */
    *inOutIdx = i + totalExtSz;

    /* Always encrypted. */
    *inOutIdx += ssl->keys.padSz;

#ifdef WOLFSSL_EARLY_DATA
    if (ssl->earlyData != no_early_data) {
        TLSX* ext = TLSX_Find(ssl->extensions, TLSX_EARLY_DATA);
        if (ext == NULL || !ext->val)
            ssl->earlyData = no_early_data;
    }
#endif

#ifdef WOLFSSL_EARLY_DATA
    if (ssl->earlyData == no_early_data) {
        ret = SetKeysSide(ssl, ENCRYPT_SIDE_ONLY);
        if (ret != 0)
            return ret;
    }
#endif

    ssl->options.serverState = SERVER_ENCRYPTED_EXTENSIONS_COMPLETE;

    WOLFSSL_LEAVE("DoTls13EncryptedExtensions", ret);
    WOLFSSL_END(WC_FUNC_ENCRYPTED_EXTENSIONS_DO);

    return ret;
}

#ifndef NO_CERTS
/* handle processing TLS v1.3 certificate_request (13) */
/* Handle a TLS v1.3 CertificateRequest message.
 * This message is always encrypted.
 * Only a client will receive this message.
 *
 * ssl       The SSL/TLS object.
 * input     The message buffer.
 * inOutIdx  On entry, the index into the message buffer of CertificateRequest.
 *           On exit, the index of byte after the CertificateRequest message.
 * size      The length of the current handshake message.
 * returns 0 on success and otherwise failure.
 */
static int DoTls13CertificateRequest(WOLFSSL* ssl, const byte* input,
                                     word32* inOutIdx, word32 size)
{
    word16      len;
    word32      begin = *inOutIdx;
    int         ret = 0;
    Suites      peerSuites;
#ifdef WOLFSSL_POST_HANDSHAKE_AUTH
    CertReqCtx* certReqCtx;
#endif

    WOLFSSL_START(WC_FUNC_CERTIFICATE_REQUEST_DO);
    WOLFSSL_ENTER("DoTls13CertificateRequest");

    XMEMSET(&peerSuites, 0, sizeof(Suites));

#ifdef WOLFSSL_CALLBACKS
    if (ssl->hsInfoOn) AddPacketName(ssl, "CertificateRequest");
    if (ssl->toInfoOn) AddLateName("CertificateRequest", &ssl->timeoutInfo);
#endif

    if (OPAQUE8_LEN > size)
        return BUFFER_ERROR;

    /* Length of the request context. */
    len = input[(*inOutIdx)++];
    if ((*inOutIdx - begin) + len > size)
        return BUFFER_ERROR;
    if (ssl->options.connectState < FINISHED_DONE && len > 0)
        return BUFFER_ERROR;

#ifdef WOLFSSL_POST_HANDSHAKE_AUTH
    /* CertReqCtx has one byte at end for context value.
     * Increase size to handle other implementations sending more than one byte.
     * That is, allocate extra space, over one byte, to hold the context value.
     */
    certReqCtx = (CertReqCtx*)XMALLOC(sizeof(CertReqCtx) + len - 1, ssl->heap,
                                                       DYNAMIC_TYPE_TMP_BUFFER);
    if (certReqCtx == NULL)
        return MEMORY_E;
    certReqCtx->next = ssl->certReqCtx;
    certReqCtx->len = len;
    XMEMCPY(&certReqCtx->ctx, input + *inOutIdx, len);
    ssl->certReqCtx = certReqCtx;
#endif
    *inOutIdx += len;

    /* TODO: Add support for more extensions:
     *   signed_certificate_timestamp, certificate_authorities, oid_filters.
     */
    /* Certificate extensions */
    if ((*inOutIdx - begin) + OPAQUE16_LEN > size)
        return BUFFER_ERROR;
    ato16(input + *inOutIdx, &len);
    *inOutIdx += OPAQUE16_LEN;
    if ((*inOutIdx - begin) + len > size)
        return BUFFER_ERROR;
    if (len == 0)
        return INVALID_PARAMETER;
    if ((ret = TLSX_Parse(ssl, input + *inOutIdx, len, certificate_request,
                                                                &peerSuites))) {
        return ret;
    }
    *inOutIdx += len;

    if ((ssl->buffers.certificate && ssl->buffers.certificate->buffer &&
        ((ssl->buffers.key && ssl->buffers.key->buffer)
        #ifdef HAVE_PK_CALLBACKS
            || wolfSSL_CTX_IsPrivatePkSet(ssl->ctx)
        #endif
    ))
        #ifdef OPENSSL_EXTRA
            || ssl->ctx->certSetupCb != NULL
        #endif
            ) {
        if (PickHashSigAlgo(ssl, peerSuites.hashSigAlgo,
                                               peerSuites.hashSigAlgoSz) != 0) {
            WOLFSSL_ERROR_VERBOSE(INVALID_PARAMETER);
            return INVALID_PARAMETER;
        }
        ssl->options.sendVerify = SEND_CERT;
    }
    else {
#ifndef WOLFSSL_NO_CLIENT_CERT_ERROR
        ssl->options.sendVerify = SEND_BLANK_CERT;
#else
        WOLFSSL_MSG("Certificate required but none set on client");
        SendAlert(ssl, alert_fatal, illegal_parameter);
        WOLFSSL_ERROR_VERBOSE(NO_CERT_ERROR);
        return NO_CERT_ERROR;
#endif
    }

    /* This message is always encrypted so add encryption padding. */
    *inOutIdx += ssl->keys.padSz;

    WOLFSSL_LEAVE("DoTls13CertificateRequest", ret);
    WOLFSSL_END(WC_FUNC_CERTIFICATE_REQUEST_DO);

    return ret;
}
#endif /* !NO_CERTS */
#endif /* !NO_WOLFSSL_CLIENT */

#ifndef NO_WOLFSSL_SERVER
#if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
/* Refine list of supported cipher suites to those common to server and client.
 *
 * ssl         SSL/TLS object.
 * peerSuites  The peer's advertised list of supported cipher suites.
 */
static void RefineSuites(WOLFSSL* ssl, Suites* peerSuites)
{
    byte   suites[WOLFSSL_MAX_SUITE_SZ];
    word16 suiteSz = 0;
    word16 i;
    word16 j;

    if (AllocateSuites(ssl) != 0)
        return;

    XMEMSET(suites, 0, WOLFSSL_MAX_SUITE_SZ);

    if (!ssl->options.useClientOrder) {
        /* Server order refining. */
        for (i = 0; i < ssl->suites->suiteSz; i += 2) {
            for (j = 0; j < peerSuites->suiteSz; j += 2) {
                if ((ssl->suites->suites[i+0] == peerSuites->suites[j+0]) &&
                    (ssl->suites->suites[i+1] == peerSuites->suites[j+1])) {
                    suites[suiteSz++] = peerSuites->suites[j+0];
                    suites[suiteSz++] = peerSuites->suites[j+1];
                    break;
                }
            }
            if (suiteSz == WOLFSSL_MAX_SUITE_SZ)
                break;
        }
    }
    else {
        /* Client order refining. */
        for (j = 0; j < peerSuites->suiteSz; j += 2) {
            for (i = 0; i < ssl->suites->suiteSz; i += 2) {
                if ((ssl->suites->suites[i+0] == peerSuites->suites[j+0]) &&
                    (ssl->suites->suites[i+1] == peerSuites->suites[j+1])) {
                    suites[suiteSz++] = peerSuites->suites[j+0];
                    suites[suiteSz++] = peerSuites->suites[j+1];
                    break;
                }
            }
            if (suiteSz == WOLFSSL_MAX_SUITE_SZ)
                break;
        }
    }

    ssl->suites->suiteSz = suiteSz;
    XMEMCPY(ssl->suites->suites, &suites, sizeof(suites));
#ifdef WOLFSSL_DEBUG_TLS
    {
        int ii;
        WOLFSSL_MSG("Refined Ciphers:");
        for (ii = 0 ; ii < ssl->suites->suiteSz; ii += 2) {
            WOLFSSL_MSG(GetCipherNameInternal(ssl->suites->suites[ii+0],
                                              ssl->suites->suites[ii+1]));
        }
    }
#endif
}


#ifndef NO_PSK
int FindPskSuite(const WOLFSSL* ssl, PreSharedKey* psk, byte* psk_key,
        word32* psk_keySz, const byte* suite, int* found, byte* foundSuite)
{
    const char* cipherName = NULL;
    byte        cipherSuite0 = TLS13_BYTE;
    byte        cipherSuite  = WOLFSSL_DEF_PSK_CIPHER;
    int         ret = 0;

    *found = 0;
    (void)suite;

    if (ssl->options.server_psk_tls13_cb != NULL) {
         *psk_keySz = ssl->options.server_psk_tls13_cb((WOLFSSL*)ssl,
             (char*)psk->identity, psk_key, MAX_PSK_KEY_LEN, &cipherName);
         if (*psk_keySz != 0) {
             int cipherSuiteFlags = WOLFSSL_CIPHER_SUITE_FLAG_NONE;
             *found = (GetCipherSuiteFromName(cipherName, &cipherSuite0,
                 &cipherSuite, &cipherSuiteFlags) == 0);
             (void)cipherSuiteFlags;
         }
    }
    if (*found == 0 && (ssl->options.server_psk_cb != NULL)) {
         *psk_keySz = ssl->options.server_psk_cb((WOLFSSL*)ssl,
                             (char*)psk->identity, psk_key,
                             MAX_PSK_KEY_LEN);
         *found = (*psk_keySz != 0);
    }
    if (*found) {
        if (*psk_keySz > MAX_PSK_KEY_LEN) {
            WOLFSSL_MSG("Key len too long in FindPsk()");
            ret = PSK_KEY_ERROR;
            WOLFSSL_ERROR_VERBOSE(ret);
            *found = 0;
        }
        if (ret == 0) {
        #if !defined(WOLFSSL_PSK_ONE_ID) && !defined(WOLFSSL_PRIORITIZE_PSK)
            /* Check whether PSK ciphersuite is in SSL. */
            *found = (suite[0] == cipherSuite0) && (suite[1] == cipherSuite);
        #else
            (void)suite;
            /* Check whether PSK ciphersuite is in SSL. */
            {
                byte s[2] = {
                    cipherSuite0,
                    cipherSuite,
                };
                *found = FindSuiteSSL(ssl, s);
            }
        #endif
        }
    }
    if (*found && foundSuite != NULL) {
        foundSuite[0] = cipherSuite0;
        foundSuite[1] = cipherSuite;
    }

    return ret;
}

/* Attempt to find the PSK (not session ticket) that matches.
 *
 * @param [in, out] ssl    The SSL/TLS object.
 * @param [in]      psk    A pre-shared key from the extension.
 * @param [out]     suite  Cipher suite to use with PSK.
 * @param [out]     err    Error code.
 *                         PSK_KEY_ERROR when key is too big or ticket age is
 *                         invalid,
 *                         UNSUPPORTED_SUITE on invalid suite.
 *                         Other error when attempting to derive early secret.
 * @return  1 when a match found - but check error code.
 * @return  0 when no match found.
 */
static int FindPsk(WOLFSSL* ssl, PreSharedKey* psk, const byte* suite, int* err)
{
    int         ret = 0;
    int         found = 0;
    byte        foundSuite[SUITE_LEN];

    WOLFSSL_ENTER("FindPsk");

    ret = FindPskSuite(ssl, psk, ssl->arrays->psk_key, &ssl->arrays->psk_keySz,
                       suite, &found, foundSuite);
    if (ret == 0 && found) {
        if ((ret == 0) && found) {
            /* Default to ciphersuite if cb doesn't specify. */
            ssl->options.resuming = 0;
            /* Don't send certificate request when using PSK. */
            ssl->options.verifyPeer = 0;

            /* PSK age is always zero. */
            if (psk->ticketAge != 0) {
                ret = PSK_KEY_ERROR;
                WOLFSSL_ERROR_VERBOSE(ret);
            }
        }
        if ((ret == 0) && found) {
            /* Set PSK ciphersuite into SSL. */
            ssl->options.cipherSuite0 = foundSuite[0];
            ssl->options.cipherSuite  = foundSuite[1];
            ret = SetCipherSpecs(ssl);
        }
        if ((ret == 0) && found) {
            /* Derive the early secret using the PSK. */
            ret = DeriveEarlySecret(ssl);
        }
        if ((ret == 0) && found) {
            /* PSK negotiation has succeeded */
            ssl->options.isPSK = 1;
            /* SERVER: using PSK for peer authentication. */
            ssl->options.peerAuthGood = 1;
        }
    }

    *err = ret;
    WOLFSSL_LEAVE("FindPsk", found);
    WOLFSSL_LEAVE("FindPsk", ret);
    return found;
}
#endif /* !NO_PSK */

/* Handle any Pre-Shared Key (PSK) extension.
 * Find a PSK that supports the cipher suite passed in.
 *
 * ssl         SSL/TLS object.
 * suite       Cipher suite to find PSK for.
 * usingPSK    1=Indicates handshake is using Pre-Shared Keys (2=Ephemeral)
 * first       Set to 1 if first in extension
 * returns 0 on success and otherwise failure.
 */
static int DoPreSharedKeys(WOLFSSL* ssl, const byte* input, word32 inputSz,
    const byte* suite, int* usingPSK, int* first)
{
    int           ret = 0;
    TLSX*         ext;
    PreSharedKey* current;
    byte          binderKey[WC_MAX_DIGEST_SIZE];
    byte          binder[WC_MAX_DIGEST_SIZE];
    word32        binderLen;

    #ifdef NO_PSK
        (void) suite; /* to avoid unused var warning when not used */
    #endif

    WOLFSSL_ENTER("DoPreSharedKeys");

    (void)suite;

    ext = TLSX_Find(ssl->extensions, TLSX_PRE_SHARED_KEY);
    if (ext == NULL) {
        WOLFSSL_MSG("No pre shared extension keys found");
        return BAD_FUNC_ARG;
    }

    /* Look through all client's pre-shared keys for a match. */
    for (current = (PreSharedKey*)ext->data; current != NULL;
            current = current->next) {
    #ifndef NO_PSK
        if (current->identityLen > MAX_PSK_ID_LEN) {
            return BUFFER_ERROR;
        }
        XMEMCPY(ssl->arrays->client_identity, current->identity,
                current->identityLen);
        ssl->arrays->client_identity[current->identityLen] = '\0';
    #endif

    #ifdef HAVE_SESSION_TICKET
        /* Decode the identity. */
        switch (current->decryptRet) {
            case PSK_DECRYPT_NONE:
                ret = DoClientTicket_ex(ssl, current, 1);
                /* psk->sess may be set. Need to clean up later. */
                break;
            case PSK_DECRYPT_OK:
                ret = WOLFSSL_TICKET_RET_OK;
                break;
            case PSK_DECRYPT_CREATE:
                ret = WOLFSSL_TICKET_RET_CREATE;
                break;
            case PSK_DECRYPT_FAIL:
                ret = WOLFSSL_TICKET_RET_REJECT;
                break;
        }

        #ifdef WOLFSSL_ASYNC_CRYPT
        if (ret == WC_PENDING_E)
            return ret;
        #endif

        if (ret != WOLFSSL_TICKET_RET_OK && current->sess_free_cb != NULL) {
            current->sess_free_cb(ssl, current->sess,
                    &current->sess_free_cb_ctx);
            current->sess = NULL;
            XMEMSET(&current->sess_free_cb_ctx, 0,
                    sizeof(psk_sess_free_cb_ctx));
        }
        if (ret == WOLFSSL_TICKET_RET_OK) {
            ret = DoClientTicketCheck(ssl, current, ssl->timeout, suite);
            if (ret == 0)
                DoClientTicketFinalize(ssl, current->it, current->sess);
            if (current->sess_free_cb != NULL) {
                current->sess_free_cb(ssl, current->sess,
                        &current->sess_free_cb_ctx);
                current->sess = NULL;
                XMEMSET(&current->sess_free_cb_ctx, 0,
                        sizeof(psk_sess_free_cb_ctx));
            }
            if (ret != 0)
                continue;

            /* SERVER: using secret in session ticket for peer auth. */
            ssl->options.peerAuthGood = 1;

        #ifdef WOLFSSL_EARLY_DATA
            ssl->options.maxEarlyDataSz = ssl->session->maxEarlyDataSz;
        #endif
            /* Use the same cipher suite as before and set up for use. */
            ssl->options.cipherSuite0   = ssl->session->cipherSuite0;
            ssl->options.cipherSuite    = ssl->session->cipherSuite;
            ret = SetCipherSpecs(ssl);
            if (ret != 0)
                return ret;

            /* Resumption PSK is resumption master secret. */
            ssl->arrays->psk_keySz = ssl->specs.hash_size;
            if ((ret = DeriveResumptionPSK(ssl, ssl->session->ticketNonce.data,
                ssl->session->ticketNonce.len, ssl->arrays->psk_key)) != 0) {
                return ret;
            }

            /* Derive the early secret using the PSK. */
            ret = DeriveEarlySecret(ssl);
            if (ret != 0)
                return ret;

            /* Hash data up to binders for deriving binders in PSK extension. */
            ret = HashInput(ssl, input, inputSz);
            if (ret < 0)
                return ret;

            /* Derive the binder key to use with HMAC. */
            ret = DeriveBinderKeyResume(ssl, binderKey);
            if (ret != 0)
                return ret;
        }
        else
    #endif /* HAVE_SESSION_TICKET */
    #ifndef NO_PSK
        if (FindPsk(ssl, current, suite, &ret)) {
            if (ret != 0)
                return ret;

            ret = HashInput(ssl, input, inputSz);
            if (ret < 0)
                return ret;

            /* Derive the binder key to use with HMAC. */
            ret = DeriveBinderKey(ssl, binderKey);
            if (ret != 0)
                return ret;
        }
        else
    #endif
        {
            continue;
        }

        ssl->options.sendVerify = 0;

        /* Derive the Finished message secret. */
        ret = DeriveFinishedSecret(ssl, binderKey,
                                   ssl->keys.client_write_MAC_secret,
                                   0 /* neither end */);
        if (ret != 0)
            return ret;

        /* Derive the binder and compare with the one in the extension. */
        ret = BuildTls13HandshakeHmac(ssl,
                         ssl->keys.client_write_MAC_secret, binder, &binderLen);
        if (ret != 0)
            return ret;
        if (binderLen != current->binderLen ||
                             XMEMCMP(binder, current->binder, binderLen) != 0) {
            WOLFSSL_ERROR_VERBOSE(BAD_BINDER);
            return BAD_BINDER;
        }

        /* This PSK works, no need to try any more. */
        current->chosen = 1;
        ext->resp = 1;
        break;
    }

    if (current == NULL) {
#ifdef WOLFSSL_PSK_ID_PROTECTION
    #ifndef NO_CERTS
        if (ssl->buffers.certChainCnt != 0)
            return 0;
    #endif
        WOLFSSL_ERROR_VERBOSE(BAD_BINDER);
        return BAD_BINDER;
#else
        return 0;
#endif
    }

    *first = (current == ext->data);
    *usingPSK = 1;

    WOLFSSL_LEAVE("DoPreSharedKeys", ret);

    return ret;
}

/* Handle any Pre-Shared Key (PSK) extension.
 * Must do this in ClientHello as it requires a hash of the truncated message.
 * Don't know size of binders until Pre-Shared Key extension has been parsed.
 *
 * ssl         SSL/TLS object.
 * input       ClientHello message.
 * helloSz     Size of the ClientHello message (including binders if present).
 * clSuites    Client's cipher suite list.
 * usingPSK    Indicates handshake is using Pre-Shared Keys.
 */
static int CheckPreSharedKeys(WOLFSSL* ssl, const byte* input, word32 helloSz,
                              Suites* clSuites, int* usingPSK)
{
    int    ret;
    TLSX*  ext;
    word16 bindersLen;
    int    first = 0;
#ifndef WOLFSSL_PSK_ONE_ID
    int    i;
    const Suites* suites = WOLFSSL_SUITES(ssl);
#else
    byte   suite[2];
#endif

    WOLFSSL_ENTER("CheckPreSharedKeys");

    ext = TLSX_Find(ssl->extensions, TLSX_PRE_SHARED_KEY);
    if (ext == NULL) {
#ifdef WOLFSSL_EARLY_DATA
        ssl->earlyData = no_early_data;
#endif
        if (usingPSK)
            *usingPSK = 0;
        /* Hash data up to binders for deriving binders in PSK extension. */
        ret = HashInput(ssl, input,  helloSz);
        return ret;
    }

    /* Extensions pushed on stack/list and PSK must be last. */
    if (ssl->extensions != ext) {
        WOLFSSL_ERROR_VERBOSE(PSK_KEY_ERROR);
        return PSK_KEY_ERROR;
    }

    /* Assume we are going to resume with a pre-shared key. */
    ssl->options.resuming = 1;

    /* Find the pre-shared key extension and calculate hash of truncated
     * ClientHello for binders.
     */
    ret = TLSX_PreSharedKey_GetSizeBinders((PreSharedKey*)ext->data,
                                                     client_hello, &bindersLen);
    if (ret < 0)
        return ret;

    /* Refine list for PSK processing. */
    RefineSuites(ssl, clSuites);

#ifndef WOLFSSL_PSK_ONE_ID
    if (usingPSK == NULL)
        return BAD_FUNC_ARG;

    /* Server list has only common suites from refining in server or client
     * order. */
    for (i = 0; !(*usingPSK) && i < suites->suiteSz; i += 2) {
        ret = DoPreSharedKeys(ssl, input, helloSz - bindersLen,
                suites->suites + i, usingPSK, &first);
        if (ret != 0) {
#ifdef HAVE_SESSION_TICKET
#ifdef WOLFSSL_ASYNC_CRYPT
            if (ret != WC_PENDING_E)
#endif
                CleanupClientTickets((PreSharedKey*)ext->data);
#endif
            WOLFSSL_MSG_EX("DoPreSharedKeys: %d", ret);
            return ret;
        }
    }
#ifdef HAVE_SESSION_TICKET
    CleanupClientTickets((PreSharedKey*)ext->data);
#endif
#else
    ret = DoPreSharedKeys(ssl, input, helloSz - bindersLen, suite, usingPSK,
        &first);
    if (ret != 0) {
        WOLFSSL_MSG_EX("DoPreSharedKeys: %d", ret);
        return ret;
    }
#endif

    if (*usingPSK) {
        /* While verifying the selected PSK, we updated the
         * handshake hash up to the binder bytes in the PSK extensions.
         * Continuing, we need the rest of the ClientHello hashed as well.
         */
        ret = HashRaw(ssl, input + helloSz - bindersLen, bindersLen);
    }
    else {
        /* No suitable PSK found, Hash the complete ClientHello,
         * as caller expect it after we return */
        ret = HashInput(ssl, input,  helloSz);
    }
    if (ret != 0)
        return ret;

    if (*usingPSK != 0) {
        word16 modes;
    #ifdef WOLFSSL_EARLY_DATA
        TLSX*  extEarlyData;

        extEarlyData = TLSX_Find(ssl->extensions, TLSX_EARLY_DATA);
        if (extEarlyData != NULL) {
            /* Check if accepting early data and first PSK. */
            if (ssl->earlyData != no_early_data && first) {
                extEarlyData->resp = 1;

                /* Derive early data decryption key. */
                ret = DeriveTls13Keys(ssl, early_data_key, DECRYPT_SIDE_ONLY,
                                                                             1);
                if (ret != 0)
                    return ret;
                if ((ret = SetKeysSide(ssl, DECRYPT_SIDE_ONLY)) != 0)
                    return ret;

#ifdef WOLFSSL_DTLS13
                if (ssl->options.dtls) {
                    ret = Dtls13NewEpoch(ssl,
                        w64From32(0x0, DTLS13_EPOCH_EARLYDATA),
                        DECRYPT_SIDE_ONLY);
                    if (ret != 0)
                        return ret;
                }
#endif /* WOLFSSL_DTLS13 */

                ssl->earlyData = process_early_data;
            }
            else
                extEarlyData->resp = 0;
        }
    #endif

        /* Get the PSK key exchange modes the client wants to negotiate. */
        ext = TLSX_Find(ssl->extensions, TLSX_PSK_KEY_EXCHANGE_MODES);
        if (ext == NULL) {
            WOLFSSL_ERROR_VERBOSE(MISSING_HANDSHAKE_DATA);
            return MISSING_HANDSHAKE_DATA;
        }
        modes = ext->val;

    #ifdef HAVE_SUPPORTED_CURVES
        ext = TLSX_Find(ssl->extensions, TLSX_KEY_SHARE);
        /* Use (EC)DHE for forward-security if possible. */
        if ((modes & (1 << PSK_DHE_KE)) != 0 && !ssl->options.noPskDheKe &&
                                                                  ext != NULL) {
            /* Only use named group used in last session. */
            ssl->namedGroup = ssl->session->namedGroup;

            *usingPSK = 2; /* generate new ephemeral key */
        }
        else if (ssl->options.onlyPskDheKe) {
            return PSK_KEY_ERROR;
        }
        else
    #endif
        {
            if ((modes & (1 << PSK_KE)) == 0) {
                WOLFSSL_MSG("psk_ke mode does not allow key share");
                WOLFSSL_ERROR_VERBOSE(PSK_KEY_ERROR);
                return PSK_KEY_ERROR;
            }
            ssl->options.noPskDheKe = 1;
            ssl->arrays->preMasterSz = 0;

            *usingPSK = 1;
        }
    }
#ifdef WOLFSSL_PSK_ID_PROTECTION
    else {
    #ifndef NO_CERTS
        if (ssl->buffers.certChainCnt != 0)
            return 0;
    #endif
        WOLFSSL_ERROR_VERBOSE(BAD_BINDER);
        return BAD_BINDER;
    }
#endif

    WOLFSSL_LEAVE("CheckPreSharedKeys", ret);

    return 0;
}
#endif /* HAVE_SESSION_TICKET || !NO_PSK */

#if defined(WOLFSSL_SEND_HRR_COOKIE)
/* Check that the Cookie data's integrity.
 *
 * ssl       SSL/TLS object.
 * cookie    The cookie data - hash and MAC.
 * cookieSz  The length of the cookie data in bytes.
 * returns Length of the hash on success, otherwise failure.
 */
int TlsCheckCookie(const WOLFSSL* ssl, const byte* cookie, word16 cookieSz)
{
    int  ret;
    byte mac[WC_MAX_DIGEST_SIZE] = {0};
    Hmac cookieHmac;
    byte cookieType = 0;
    byte macSz = 0;

#if !defined(NO_SHA) && defined(NO_SHA256)
    cookieType = SHA;
    macSz = WC_SHA_DIGEST_SIZE;
#endif /* NO_SHA */
#ifndef NO_SHA256
    cookieType = WC_SHA256;
    macSz = WC_SHA256_DIGEST_SIZE;
#endif /* NO_SHA256 */

    if (cookieSz < ssl->specs.hash_size + macSz)
        return HRR_COOKIE_ERROR;
    cookieSz -= macSz;

    ret = wc_HmacInit(&cookieHmac, ssl->heap, INVALID_DEVID);
    if (ret == 0) {
        ret = wc_HmacSetKey(&cookieHmac, cookieType,
                            ssl->buffers.tls13CookieSecret.buffer,
                            ssl->buffers.tls13CookieSecret.length);
    }
    if (ret == 0)
        ret = wc_HmacUpdate(&cookieHmac, cookie, cookieSz);
#ifdef WOLFSSL_DTLS13
    /* Tie cookie to peer address */
    if (ret == 0) {
        if (ssl->options.dtls && ssl->buffers.dtlsCtx.peer.sz > 0) {
            ret = wc_HmacUpdate(&cookieHmac,
                (byte*)ssl->buffers.dtlsCtx.peer.sa,
                ssl->buffers.dtlsCtx.peer.sz);
        }
    }
#endif
    if (ret == 0)
        ret = wc_HmacFinal(&cookieHmac, mac);

    wc_HmacFree(&cookieHmac);
    if (ret != 0)
        return ret;

    if (ConstantCompare(cookie + cookieSz, mac, macSz) != 0) {
        WOLFSSL_ERROR_VERBOSE(HRR_COOKIE_ERROR);
        return HRR_COOKIE_ERROR;
    }
    return cookieSz;
}

/* Length of the KeyShare Extension */
#define HRR_KEY_SHARE_SZ   (OPAQUE16_LEN + OPAQUE16_LEN + OPAQUE16_LEN)
/* Length of the Supported Versions Extension */
#define HRR_VERSIONS_SZ    (OPAQUE16_LEN + OPAQUE16_LEN + OPAQUE16_LEN)
/* Length of the Cookie Extension excluding cookie data */
#define HRR_COOKIE_HDR_SZ  (OPAQUE16_LEN + OPAQUE16_LEN + OPAQUE16_LEN)
/* PV | Random | Session Id | CipherSuite | Compression | Ext Len */
#define HRR_BODY_SZ        (VERSION_SZ + RAN_LEN + ENUM_LEN + ID_LEN + \
                            SUITE_LEN + COMP_LEN + OPAQUE16_LEN)
/* HH | PV | CipherSuite | Ext Len | Key Share | Supported Version | Cookie */
#define MAX_HRR_SZ   (HRR_MAX_HS_HEADER_SZ   + \
                        HRR_BODY_SZ         + \
                          HRR_KEY_SHARE_SZ  + \
                          HRR_VERSIONS_SZ   + \
                          HRR_COOKIE_HDR_SZ)


/* Restart the handshake hash from the cookie value.
 *
 * ssl     SSL/TLS object.
 * cookie  Cookie data from client.
 * returns 0 on success, otherwise failure.
 */
static int RestartHandshakeHashWithCookie(WOLFSSL* ssl, Cookie* cookie)
{
    byte   header[HANDSHAKE_HEADER_SZ] = {0};
    byte   hrr[MAX_HRR_SZ] = {0};
    int    hrrIdx;
    word32 idx;
    byte   hashSz;
    byte*  cookieData;
    word16 cookieDataSz;
    word16 length;
    int    keyShareExt = 0;
    int    ret;

    ret = TlsCheckCookie(ssl, cookie->data, (byte)cookie->len);
    if (ret < 0)
        return ret;
    cookieDataSz = (word16)ret;
    hashSz = cookie->data[0];
    cookieData = cookie->data;
    idx = OPAQUE8_LEN;

    /* Restart handshake hash with synthetic message hash. */
    AddTls13HandShakeHeader(header, hashSz, 0, 0, message_hash, ssl);

    if ((ret = InitHandshakeHashes(ssl)) != 0)
        return ret;
    if ((ret = HashRaw(ssl, header, sizeof(header))) != 0)
        return ret;
#ifdef WOLFSSL_DEBUG_TLS
    WOLFSSL_MSG("Restart Hash from Cookie");
    WOLFSSL_BUFFER(cookieData + idx, hashSz);
#endif
    if ((ret = HashRaw(ssl, cookieData + idx, hashSz)) != 0)
        return ret;

    /* Reconstruct the HelloRetryMessage for handshake hash. */
    length = HRR_BODY_SZ - ID_LEN + ssl->session->sessionIDSz +
             HRR_COOKIE_HDR_SZ + cookie->len;
    length += HRR_VERSIONS_SZ;
    /* HashSz (1 byte) + Hash (HashSz bytes) + CipherSuite (2 bytes) */
    if (cookieDataSz > OPAQUE8_LEN + hashSz + OPAQUE16_LEN) {
        keyShareExt = 1;
        length += HRR_KEY_SHARE_SZ;
    }

    AddTls13HandShakeHeader(hrr, length, 0, 0, server_hello, ssl);

    idx += hashSz;
    hrrIdx = HANDSHAKE_HEADER_SZ;

#ifdef WOLFSSL_DTLS13
    if (ssl->options.dtls)
        hrrIdx += DTLS_HANDSHAKE_EXTRA;
#endif /* WOLFSSL_DTLS13 */

    /* The negotiated protocol version. */
    hrr[hrrIdx++] = ssl->version.major;
    hrr[hrrIdx++] = ssl->options.dtls ? DTLSv1_2_MINOR : TLSv1_2_MINOR;

    /* HelloRetryRequest message has fixed value for random. */
    XMEMCPY(hrr + hrrIdx, helloRetryRequestRandom, RAN_LEN);
    hrrIdx += RAN_LEN;

    hrr[hrrIdx++] = ssl->session->sessionIDSz;
    if (ssl->session->sessionIDSz > 0) {
        XMEMCPY(hrr + hrrIdx, ssl->session->sessionID, ssl->session->sessionIDSz);
        hrrIdx += ssl->session->sessionIDSz;
    }

    /* Cipher Suite */
    hrr[hrrIdx++] = cookieData[idx++];
    hrr[hrrIdx++] = cookieData[idx++];

    /* Compression not supported in TLS v1.3. */
    hrr[hrrIdx++] = 0;

    /* Extensions' length */
    length -= HRR_BODY_SZ - ID_LEN + ssl->session->sessionIDSz;
    c16toa(length, hrr + hrrIdx);
    hrrIdx += 2;

    /* Optional KeyShare Extension */
    if (keyShareExt) {
        c16toa(TLSX_KEY_SHARE, hrr + hrrIdx);
        hrrIdx += 2;
        c16toa(OPAQUE16_LEN, hrr + hrrIdx);
        hrrIdx += 2;
        hrr[hrrIdx++] = cookieData[idx++];
        hrr[hrrIdx++] = cookieData[idx++];
    }
    c16toa(TLSX_SUPPORTED_VERSIONS, hrr + hrrIdx);
    hrrIdx += 2;
    c16toa(OPAQUE16_LEN, hrr + hrrIdx);
    hrrIdx += 2;
    #ifdef WOLFSSL_TLS13_DRAFT
        hrr[hrrIdx++] = TLS_DRAFT_MAJOR;
        hrr[hrrIdx++] = TLS_DRAFT_MINOR;
    #else
        hrr[hrrIdx++] = ssl->version.major;
        hrr[hrrIdx++] = ssl->version.minor;
    #endif

    /* Mandatory Cookie Extension */
    c16toa(TLSX_COOKIE, hrr + hrrIdx);
    hrrIdx += 2;
    c16toa(cookie->len + OPAQUE16_LEN, hrr + hrrIdx);
    hrrIdx += 2;
    c16toa(cookie->len, hrr + hrrIdx);
    hrrIdx += 2;

#ifdef WOLFSSL_DEBUG_TLS
    WOLFSSL_MSG("Reconstructed HelloRetryRequest");
    WOLFSSL_BUFFER(hrr, hrrIdx);
    WOLFSSL_MSG("Cookie");
    WOLFSSL_BUFFER(cookieData, cookie->len);
#endif

#ifdef WOLFSSL_DTLS13
    if (ssl->options.dtls) {
        ret = Dtls13HashHandshake(ssl, hrr, (word16)hrrIdx);
    }
    else
#endif /* WOLFSSL_DTLS13 */
        {
            ret = HashRaw(ssl, hrr, hrrIdx);
        }

    if (ret != 0)
        return ret;

    return HashRaw(ssl, cookieData, cookie->len);
}
#endif

/* Do SupportedVersion extension for TLS v1.3+ otherwise it is not.
 *
 * ssl       The SSL/TLS object.
 * input     The message buffer.
 * i         The index into the message buffer of ClientHello.
 * helloSz   The length of the current handshake message.
 * returns 0 on success and otherwise failure.
 */
static int DoTls13SupportedVersions(WOLFSSL* ssl, const byte* input, word32 i,
                                    word32 helloSz, int* wantDowngrade)
{
    int    ret;
    byte   b;
    word16 suiteSz;
    word16 totalExtSz;
    int    foundVersion = 0;

    /* Client random */
    i += RAN_LEN;
    /* Session id - not used in TLS v1.3 */
    b = input[i++];
    if (i + b > helloSz) {
        return BUFFER_ERROR;
    }
    i += b;
#ifdef WOLFSSL_DTLS13
    if (ssl->options.dtls) {
        /* legacy_cookie - not used in DTLS v1.3 */
        b = input[i++];
        if (i + b > helloSz) {
            return BUFFER_ERROR;
        }
        i += b;
    }
#endif /* WOLFSSL_DTLS13 */
    /* Cipher suites */
    if (i + OPAQUE16_LEN > helloSz)
        return BUFFER_ERROR;
    ato16(input + i, &suiteSz);
    i += OPAQUE16_LEN;
    if (i + suiteSz + 1 > helloSz)
        return BUFFER_ERROR;
    i += suiteSz;
    /* Compression */
    b = input[i++];
    if (i + b > helloSz)
        return BUFFER_ERROR;
    i += b;

    /* TLS 1.3 must have extensions */
    if (i < helloSz) {
        if (i + OPAQUE16_LEN > helloSz)
            return BUFFER_ERROR;
        ato16(&input[i], &totalExtSz);
        i += OPAQUE16_LEN;
        if (totalExtSz != helloSz - i)
            return BUFFER_ERROR;

        /* Need to negotiate version first. */
        if ((ret = TLSX_ParseVersion(ssl, input + i, totalExtSz, client_hello,
                                                              &foundVersion))) {
            return ret;
        }
    }
    *wantDowngrade = !foundVersion || !IsAtLeastTLSv1_3(ssl->version);

    return 0;
}

/* Handle a ClientHello handshake message.
 * If the protocol version in the message is not TLS v1.3 or higher, use
 * DoClientHello()
 * Only a server will receive this message.
 *
 * ssl       The SSL/TLS object.
 * input     The message buffer.
 * inOutIdx  On entry, the index into the message buffer of ClientHello.
 *           On exit, the index of byte after the ClientHello message and
 *           padding.
 * helloSz   The length of the current handshake message.
 * returns 0 on success and otherwise failure.
 */

typedef struct Dch13Args {
    ProtocolVersion pv;
    Suites*         clSuites;
    word32          idx;
    word32          begin;
    int             usingPSK;
} Dch13Args;

static void FreeDch13Args(WOLFSSL* ssl, void* pArgs)
{
    Dch13Args* args = (Dch13Args*)pArgs;

    (void)ssl;

    if (args && args->clSuites) {
        XFREE(args->clSuites, ssl->heap, DYNAMIC_TYPE_SUITES);
        args->clSuites = NULL;
    }
}

int DoTls13ClientHello(WOLFSSL* ssl, const byte* input, word32* inOutIdx,
                       word32 helloSz)
{
    int ret;
#ifdef WOLFSSL_ASYNC_CRYPT
    Dch13Args* args = NULL;
    WOLFSSL_ASSERT_SIZEOF_GE(ssl->async->args, *args);
#else
    Dch13Args  args[1];
#endif
#if defined(HAVE_ECH)
    TLSX* echX = NULL;
#endif

    WOLFSSL_START(WC_FUNC_CLIENT_HELLO_DO);
    WOLFSSL_ENTER("DoTls13ClientHello");

#ifdef WOLFSSL_ASYNC_CRYPT
    if (ssl->async == NULL) {
        ssl->async = (struct WOLFSSL_ASYNC*)
                XMALLOC(sizeof(struct WOLFSSL_ASYNC), ssl->heap,
                        DYNAMIC_TYPE_ASYNC);
        if (ssl->async == NULL)
            ERROR_OUT(MEMORY_E, exit_dch);
    }
    args = (Dch13Args*)ssl->async->args;

    ret = wolfSSL_AsyncPop(ssl, &ssl->options.asyncState);
    if (ret != WC_NOT_PENDING_E) {
        /* Check for error */
        if (ret < 0) {
            goto exit_dch;
        }
    }
    else
#endif
    {
        /* Reset state */
        ret = VERSION_ERROR;
        ssl->options.asyncState = TLS_ASYNC_BEGIN;
        XMEMSET(args, 0, sizeof(Dch13Args));
    #ifdef WOLFSSL_ASYNC_CRYPT
        ssl->async->freeArgs = FreeDch13Args;
    #endif
    }

    switch (ssl->options.asyncState) {
    case TLS_ASYNC_BEGIN:
    {
    byte b;
    byte sessIdSz;
    int wantDowngrade = 0;
    word16 totalExtSz = 0;

#ifdef WOLFSSL_CALLBACKS
    if (ssl->hsInfoOn) AddPacketName(ssl, "ClientHello");
    if (ssl->toInfoOn) AddLateName("ClientHello", &ssl->timeoutInfo);
#endif

    /* do not change state in the SSL object before the next region of code
     * to be able to statelessly compute a DTLS cookie */
#if defined(WOLFSSL_DTLS13) && defined(WOLFSSL_SEND_HRR_COOKIE)
    /* Update the ssl->options.dtlsStateful setting `if` statement in
     * wolfSSL_accept_TLSv13 when changing this one. */
    if (IsDtlsNotSctpMode(ssl) && ssl->options.sendCookie) {
        ret = DoClientHelloStateless(ssl, input, inOutIdx, helloSz);
        if (ret != 0 || !ssl->options.dtlsStateful) {
            *inOutIdx += helloSz;
            goto exit_dch;
        }
    }
    ssl->options.dtlsStateful = 1;
#endif /* WOLFSSL_DTLS */

    args->idx = *inOutIdx;
    args->begin = args->idx;

    /* protocol version, random and session id length check */
    if (OPAQUE16_LEN + RAN_LEN + OPAQUE8_LEN > helloSz) {
        ERROR_OUT(BUFFER_ERROR, exit_dch);
    }

    /* Protocol version */
    XMEMCPY(&args->pv, input + args->idx, OPAQUE16_LEN);
    ssl->chVersion = args->pv;   /* store */
    args->idx += OPAQUE16_LEN;


    /* this check pass for DTLS Major (0xff) */
    if (args->pv.major < SSLv3_MAJOR) {
        WOLFSSL_MSG("Legacy version field contains unsupported value");
        ERROR_OUT(VERSION_ERROR, exit_dch);
    }

#ifdef WOLFSSL_DTLS13
    if (ssl->options.dtls &&
        args->pv.major == DTLS_MAJOR && args->pv.minor > DTLSv1_2_MINOR) {
        wantDowngrade = 1;
        ssl->version.minor = args->pv.minor;
    }
#endif /* WOLFSSL_DTLS13 */

    if (!ssl->options.dtls) {
        /* Legacy protocol version cannot negotiate TLS 1.3 or higher. */
        if (args->pv.major > SSLv3_MAJOR || (args->pv.major == SSLv3_MAJOR &&
                                             args->pv.minor >= TLSv1_3_MINOR)) {
            args->pv.major = SSLv3_MAJOR;
            args->pv.minor = TLSv1_2_MINOR;
            wantDowngrade = 1;
            ssl->version.minor = args->pv.minor;
        }
        /* Legacy version must be [ SSLv3_MAJOR, TLSv1_2_MINOR ] for TLS v1.3 */
        else if (args->pv.major == SSLv3_MAJOR &&
                 args->pv.minor < TLSv1_2_MINOR) {
            wantDowngrade = 1;
            ssl->version.minor = args->pv.minor;
        }
    }

    if (!wantDowngrade) {
        ret = DoTls13SupportedVersions(ssl, input + args->begin,
            args->idx - args->begin, helloSz, &wantDowngrade);
        if (ret < 0)
            goto exit_dch;
    }

    if (wantDowngrade) {
#ifndef WOLFSSL_NO_TLS12
        byte realMinor;
        if (!ssl->options.downgrade) {
            WOLFSSL_MSG("Client trying to connect with lesser version than "
                        "TLS v1.3");
            ERROR_OUT(VERSION_ERROR, exit_dch);
        }

        if ((!ssl->options.dtls
                 && args->pv.minor < ssl->options.minDowngrade) ||
            (ssl->options.dtls && args->pv.minor > ssl->options.minDowngrade)) {
            WOLFSSL_MSG("\tversion below minimum allowed, fatal error");
            ERROR_OUT(VERSION_ERROR, exit_dch);
        }

        realMinor = ssl->version.minor;
        ssl->version.minor = args->pv.minor;
        ret = HashInput(ssl, input + args->begin, helloSz);
        ssl->version.minor = realMinor;
        if (ret == 0) {
            ret = DoClientHello(ssl, input, inOutIdx, helloSz);
        }
        goto exit_dch;
#else
        WOLFSSL_MSG("Client trying to connect with lesser version than "
                    "TLS v1.3");
        ERROR_OUT(VERSION_ERROR, exit_dch);
#endif
    }

    /* From here on we are a TLS 1.3 ClientHello. */

    /* Client random */
    XMEMCPY(ssl->arrays->clientRandom, input + args->idx, RAN_LEN);
    args->idx += RAN_LEN;

#ifdef WOLFSSL_DEBUG_TLS
    WOLFSSL_MSG("client random");
    WOLFSSL_BUFFER(ssl->arrays->clientRandom, RAN_LEN);
#endif

    sessIdSz = input[args->idx++];
#ifndef WOLFSSL_TLS13_MIDDLEBOX_COMPAT
    if (sessIdSz > ID_LEN)
#else
    if (sessIdSz != ID_LEN && sessIdSz != 0)
#endif
    {
        ERROR_OUT(INVALID_PARAMETER, exit_dch);
    }

    if (sessIdSz + args->idx > helloSz)
        ERROR_OUT(BUFFER_ERROR, exit_dch);

    ssl->session->sessionIDSz = sessIdSz;
    if (sessIdSz > 0)
        XMEMCPY(ssl->session->sessionID, input + args->idx, sessIdSz);
    args->idx += sessIdSz;

#ifdef WOLFSSL_DTLS13
    /* legacy_cookie */
    if (ssl->options.dtls) {
        /* https://www.rfc-editor.org/rfc/rfc9147.html#section-5.3 */
        byte cookieLen = input[args->idx++];
        if (cookieLen != 0) {
            ERROR_OUT(INVALID_PARAMETER, exit_dch);
        }
    }
#endif /* WOLFSSL_DTLS13 */

    args->clSuites = (Suites*)XMALLOC(sizeof(Suites), ssl->heap,
        DYNAMIC_TYPE_SUITES);
    if (args->clSuites == NULL) {
        ERROR_OUT(MEMORY_E, exit_dch);
    }

    /* Cipher suites */
    if ((args->idx - args->begin) + OPAQUE16_LEN > helloSz)
        ERROR_OUT(BUFFER_ERROR, exit_dch);
    ato16(&input[args->idx], &args->clSuites->suiteSz);
    args->idx += OPAQUE16_LEN;
    if ((args->clSuites->suiteSz % 2) != 0) {
        ERROR_OUT(INVALID_PARAMETER, exit_dch);
    }
    /* suites and compression length check */
    if ((args->idx - args->begin) + args->clSuites->suiteSz + OPAQUE8_LEN > helloSz)
        ERROR_OUT(BUFFER_ERROR, exit_dch);
    if (args->clSuites->suiteSz > WOLFSSL_MAX_SUITE_SZ)
        ERROR_OUT(BUFFER_ERROR, exit_dch);
    XMEMCPY(args->clSuites->suites, input + args->idx, args->clSuites->suiteSz);
    args->idx += args->clSuites->suiteSz;
    args->clSuites->hashSigAlgoSz = 0;

    /* Compression */
    b = input[args->idx++];
    if ((args->idx - args->begin) + b > helloSz)
        ERROR_OUT(BUFFER_ERROR, exit_dch);
    if (b != COMP_LEN) {
        WOLFSSL_MSG("Must be one compression type in list");
        ERROR_OUT(INVALID_PARAMETER, exit_dch);
    }
    b = input[args->idx++];
    if (b != NO_COMPRESSION) {
        WOLFSSL_MSG("Must be no compression type in list");
        ERROR_OUT(INVALID_PARAMETER, exit_dch);
    }

    /* Extensions */
    if ((args->idx - args->begin) == helloSz)
        ERROR_OUT(BUFFER_ERROR, exit_dch);
    if ((args->idx - args->begin) + OPAQUE16_LEN > helloSz)
        ERROR_OUT(BUFFER_ERROR, exit_dch);

    ato16(&input[args->idx], &totalExtSz);
    args->idx += OPAQUE16_LEN;
    if ((args->idx - args->begin) + totalExtSz > helloSz)
        ERROR_OUT(BUFFER_ERROR, exit_dch);

    /* Auto populate extensions supported unless user defined. */
    if ((ret = TLSX_PopulateExtensions(ssl, 1)) != 0)
        goto exit_dch;

#if defined(HAVE_ECH)
    if (ssl->ctx->echConfigs != NULL) {
        /* save the start of the buffer so we can use it when parsing ech */
        echX = TLSX_Find(ssl->extensions, TLSX_ECH);

        if (echX == NULL)
            return -1;

        ((WOLFSSL_ECH*)echX->data)->aad = input + HANDSHAKE_HEADER_SZ;
        ((WOLFSSL_ECH*)echX->data)->aadLen = helloSz;
    }
#endif

    /* Parse extensions */
    if ((ret = TLSX_Parse(ssl, input + args->idx, totalExtSz, client_hello,
                                                            args->clSuites))) {
        goto exit_dch;
    }

#if defined(HAVE_ECH)
    /* jump to the end to clean things up */
    if (echX != NULL && ((WOLFSSL_ECH*)echX->data)->state == ECH_WRITE_NONE)
        goto exit_dch;
#endif

#ifdef HAVE_SNI
        if ((ret = SNI_Callback(ssl)) != 0)
            goto exit_dch;
        ssl->options.side = WOLFSSL_SERVER_END;
#endif

    args->idx += totalExtSz;
    ssl->options.haveSessionId = 1;
    ssl->options.sendVerify = SEND_CERT;

#if defined(WOLFSSL_SEND_HRR_COOKIE)
    ssl->options.cookieGood = 0;
    if (ssl->options.sendCookie &&
            (ssl->options.serverState == SERVER_HELLO_RETRY_REQUEST_COMPLETE
#ifdef WOLFSSL_DTLS13
                    /* Always check for a valid cookie since we may have already
                     * sent a HRR but we reset the state. */
                    || ssl->options.dtls
#endif
                    )) {
        TLSX* ext = TLSX_Find(ssl->extensions, TLSX_COOKIE);

        if (ext != NULL) {
            /* Ensure the cookie came from client and isn't the one in the
            * response - HelloRetryRequest.
            */
            if (ext->resp == 0) {
                ret = RestartHandshakeHashWithCookie(ssl, (Cookie*)ext->data);
                if (ret != 0)
                    goto exit_dch;
                /* Don't change state here as we may want to enter
                 * DoTls13ClientHello again. */
                ssl->options.cookieGood = 1;
            }
            else {
                ERROR_OUT(HRR_COOKIE_ERROR, exit_dch);
            }
        }
        else {
            ERROR_OUT(HRR_COOKIE_ERROR, exit_dch);
        }
    }
#endif

#if (defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)) && \
                                                    defined(HAVE_TLS_EXTENSIONS)
    ret = CheckPreSharedKeys(ssl, input + args->begin, helloSz, args->clSuites,
        &args->usingPSK);
    if (ret != 0)
        goto exit_dch;
#else
    if ((ret = HashInput(ssl, input + args->begin, helloSz)) != 0)
        goto exit_dch;
#endif

#if (defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)) && \
                                                    defined(HAVE_TLS_EXTENSIONS)
    if (!args->usingPSK)
#endif
    {
#if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
        /* Not using PSK so don't require no KE. */
        ssl->options.noPskDheKe = 0;
#endif

#ifndef NO_CERTS
        if (TLSX_Find(ssl->extensions, TLSX_KEY_SHARE) == NULL) {
            WOLFSSL_MSG("Client did not send a KeyShare extension");
            ERROR_OUT(INCOMPLETE_DATA, exit_dch);
        }
        if (TLSX_Find(ssl->extensions, TLSX_SIGNATURE_ALGORITHMS) == NULL) {
            WOLFSSL_MSG("Client did not send a SignatureAlgorithms extension");
            ERROR_OUT(INCOMPLETE_DATA, exit_dch);
        }
#else
        ERROR_OUT(INVALID_PARAMETER, exit_dch);
#endif
    }

#ifdef HAVE_ALPN
    /* With PSK and all other things validated, it's time to
     * select the ALPN protocol, if so requested */
    if ((ret = ALPN_Select(ssl)) != 0)
        goto exit_dch;
#endif
    } /* case TLS_ASYNC_BEGIN */
    FALL_THROUGH;

    case TLS_ASYNC_BUILD:
    /* Advance state and proceed */
    ssl->options.asyncState = TLS_ASYNC_DO;
    FALL_THROUGH;

    case TLS_ASYNC_DO:
    {
#ifndef NO_CERTS
    if (!args->usingPSK) {
        if ((ret = MatchSuite(ssl, args->clSuites)) < 0) {
        #ifdef WOLFSSL_ASYNC_CRYPT
            if (ret != WC_PENDING_E)
        #endif
                WOLFSSL_MSG("Unsupported cipher suite, ClientHello 1.3");
            goto exit_dch;
        }
    }
    else
#endif
#ifdef HAVE_SUPPORTED_CURVES
    if (args->usingPSK == 2) {
        /* Pick key share and Generate a new key if not present. */
        int doHelloRetry = 0;
        ret = TLSX_KeyShare_Establish(ssl, &doHelloRetry);
        if (doHelloRetry) {
            ssl->options.serverState = SERVER_HELLO_RETRY_REQUEST_COMPLETE;
            if (ret != WC_PENDING_E)
                ret = 0; /* for hello_retry return 0 */
        }
        if (ret != 0)
            goto exit_dch;
    }
#endif

    /* Advance state and proceed */
    ssl->options.asyncState = TLS_ASYNC_VERIFY;
    } /* case TLS_ASYNC_BUILD || TLS_ASYNC_DO */
    FALL_THROUGH;

    case TLS_ASYNC_VERIFY:
    {
#if defined(WOLFSSL_ASYNC_CRYPT) && defined(HAVE_SUPPORTED_CURVES)
    /* Check if the KeyShare calculations from the previous state are complete.
     * wolfSSL_AsyncPop advances ssl->options.asyncState so we may end up here
     * with a pending calculation. */
    TLSX* extension = TLSX_Find(ssl->extensions, TLSX_KEY_SHARE);
    if (extension != NULL && extension->resp == 1) {
        KeyShareEntry* serverKSE = (KeyShareEntry*)extension->data;
        if (serverKSE != NULL && serverKSE->lastRet == WC_PENDING_E) {
            ret = TLSX_KeyShare_GenKey(ssl, serverKSE);
            if (ret != 0)
                goto exit_dch;
        }
    }
#endif
    /* Advance state and proceed */
    ssl->options.asyncState = TLS_ASYNC_FINALIZE;
    }
    FALL_THROUGH;

    case TLS_ASYNC_FINALIZE:
    {
    *inOutIdx = args->idx;
    ssl->options.clientState = CLIENT_HELLO_COMPLETE;
#if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
    ssl->options.pskNegotiated = (args->usingPSK != 0);
#endif

    if (!args->usingPSK) {
#ifndef NO_CERTS
        /* Check that the negotiated ciphersuite matches protocol version. */
    #ifdef HAVE_NULL_CIPHER
        if (ssl->options.cipherSuite0 == ECC_BYTE &&
                              (ssl->options.cipherSuite == TLS_SHA256_SHA256 ||
                               ssl->options.cipherSuite == TLS_SHA384_SHA384)) {
            ;
        }
        else
    #endif
    #if defined(WOLFSSL_SM4_GCM) && defined(WOLFSSL_SM3)
        if (ssl->options.cipherSuite0 == CIPHER_BYTE &&
                ssl->options.cipherSuite == TLS_SM4_GCM_SM3) {
            ; /* Do nothing. */
        }
        else
    #endif
    #if defined(WOLFSSL_SM4_CCM) && defined(WOLFSSL_SM3)
        if (ssl->options.cipherSuite0 == CIPHER_BYTE &&
                ssl->options.cipherSuite == TLS_SM4_CCM_SM3) {
            ; /* Do nothing. */
        }
        else
    #endif
        if (ssl->options.cipherSuite0 != TLS13_BYTE) {
            WOLFSSL_MSG("Negotiated ciphersuite from lesser version than "
                        "TLS v1.3");
            ERROR_OUT(MATCH_SUITE_ERROR, exit_dch);
        }

    #ifdef HAVE_SESSION_TICKET
        if (ssl->options.resuming) {
            ssl->options.resuming = 0;
            XMEMSET(ssl->arrays->psk_key, 0, ssl->specs.hash_size);
        }
    #endif

        /* Derive early secret for handshake secret. */
        if ((ret = DeriveEarlySecret(ssl)) != 0)
            goto exit_dch;
#endif /* !NO_CERTS */
    }
    break;
    } /* case TLS_ASYNC_FINALIZE */
    default:
        ret = INPUT_CASE_ERROR;
    } /* switch (ssl->options.asyncState) */

#if defined(WOLFSSL_SEND_HRR_COOKIE)
    if (ret == 0 && ssl->options.sendCookie && ssl->options.cookieGood &&
            (ssl->options.serverState == SERVER_HELLO_RETRY_REQUEST_COMPLETE
#ifdef WOLFSSL_DTLS13
                    /* DTLS cookie exchange should be done in stateless code in
                     * DoClientHelloStateless. If we verified the cookie then
                     * always advance the state. */
                    || ssl->options.dtls
#endif
                    ))
        ssl->options.serverState = SERVER_HELLO_COMPLETE;
#endif

#if defined(WOLFSSL_DTLS13) && defined(WOLFSSL_SEND_HRR_COOKIE)
    if (ret == 0 && ssl->options.dtls && ssl->options.sendCookie &&
        ssl->options.serverState <= SERVER_HELLO_RETRY_REQUEST_COMPLETE) {
        /* Cookie and key share negotiation should be handled in
         * DoClientHelloStateless. If we enter here then something went wrong
         * in our logic. */
        ERROR_OUT(BAD_HELLO, exit_dch);
    }
#endif /* WOLFSSL_DTLS13 */

#ifdef WOLFSSL_DTLS_CID
    /* do not modify CID state if we are sending an HRR  */
    if (ssl->options.useDtlsCID &&
            ssl->options.serverState != SERVER_HELLO_RETRY_REQUEST_COMPLETE)
        DtlsCIDOnExtensionsParsed(ssl);
#endif /* WOLFSSL_DTLS_CID */



exit_dch:

    WOLFSSL_LEAVE("DoTls13ClientHello", ret);

#ifdef WOLFSSL_ASYNC_CRYPT
    if (ret == WC_PENDING_E) {
        ssl->msgsReceived.got_client_hello = 0;
        return ret;
    }
#endif

    FreeDch13Args(ssl, args);
#ifdef WOLFSSL_ASYNC_CRYPT
    FreeAsyncCtx(ssl, 0);
#endif
    WOLFSSL_END(WC_FUNC_CLIENT_HELLO_DO);

    if (ret != 0) {
        WOLFSSL_ERROR_VERBOSE(ret);
    }

#if defined(HAVE_ECH)
    if (ret == 0 && echX != NULL &&
        ((WOLFSSL_ECH*)echX->data)->state == ECH_WRITE_NONE) {

        /* add the header to the inner hello */
        AddTls13HandShakeHeader(((WOLFSSL_ECH*)echX->data)->innerClientHello,
            ((WOLFSSL_ECH*)echX->data)->innerClientHelloLen, 0, 0,
            client_hello, ssl);
    }
#endif

    return ret;
}

/* Send TLS v1.3 ServerHello message to client.
 * Only a server will send this message.
 *
 * ssl  The SSL/TLS object.
 * returns 0 on success, otherwise failure.
 */
/* handle generation of TLS 1.3 server_hello (2) */
int SendTls13ServerHello(WOLFSSL* ssl, byte extMsgType)
{
    int    ret;
    byte*  output;
    word16 length;
    word32 idx = RECORD_HEADER_SZ + HANDSHAKE_HEADER_SZ;
    int    sendSz;
#if defined(HAVE_ECH)
    TLSX* echX = NULL;
    word32 serverRandomOffset;
#endif

    WOLFSSL_START(WC_FUNC_SERVER_HELLO_SEND);
    WOLFSSL_ENTER("SendTls13ServerHello");

    /* When ssl->options.dtlsStateful is not set then cookie is calculated in
     * dtls.c */
    if (extMsgType == hello_retry_request
#ifdef WOLFSSL_DTLS13
            && (!ssl->options.dtls || ssl->options.dtlsStateful)
#endif
            ) {
        WOLFSSL_MSG("wolfSSL Sending HelloRetryRequest");
        if ((ret = RestartHandshakeHash(ssl)) < 0)
            return ret;
    }

    ssl->options.buildingMsg = 1;
#ifdef WOLFSSL_DTLS13
    if (ssl->options.dtls)
        idx = DTLS_RECORD_HEADER_SZ + DTLS_HANDSHAKE_HEADER_SZ;
#endif /* WOLFSSL_DTLS13 */

    /* Protocol version, server random, session id, cipher suite, compression
     * and extensions.
     */
    length = VERSION_SZ + RAN_LEN + ENUM_LEN + ssl->session->sessionIDSz +
             SUITE_LEN + COMP_LEN;
    ret = TLSX_GetResponseSize(ssl, extMsgType, &length);
    if (ret != 0)
        return ret;
    sendSz = idx + length;

    /* Check buffers are big enough and grow if needed. */
    if ((ret = CheckAvailableSize(ssl, sendSz)) != 0)
        return ret;

    /* Get position in output buffer to write new message to. */
    output = GetOutputBuffer(ssl);

    /* Put the record and handshake headers on. */
    AddTls13Headers(output, length, server_hello, ssl);

    /* The protocol version must be TLS v1.2 for middleboxes. */
    output[idx++] = ssl->version.major;
    output[idx++] = ssl->options.dtls ? DTLSv1_2_MINOR : TLSv1_2_MINOR;

    if (extMsgType == server_hello) {
        /* Generate server random. */
        if ((ret = wc_RNG_GenerateBlock(ssl->rng, output + idx, RAN_LEN)) != 0)
            return ret;
    }
    else {
        /* HelloRetryRequest message has fixed value for random. */
        XMEMCPY(output + idx, helloRetryRequestRandom, RAN_LEN);
    }

#if defined(HAVE_ECH)
    serverRandomOffset = idx;
#endif

    /* Store in SSL for debugging. */
    XMEMCPY(ssl->arrays->serverRandom, output + idx, RAN_LEN);
    idx += RAN_LEN;

#ifdef WOLFSSL_DEBUG_TLS
    WOLFSSL_MSG("Server random");
    WOLFSSL_BUFFER(ssl->arrays->serverRandom, RAN_LEN);
#endif

    output[idx++] = ssl->session->sessionIDSz;
    if (ssl->session->sessionIDSz > 0) {
        XMEMCPY(output + idx, ssl->session->sessionID, ssl->session->sessionIDSz);
        idx += ssl->session->sessionIDSz;
    }

    /* Chosen cipher suite */
    output[idx++] = ssl->options.cipherSuite0;
    output[idx++] = ssl->options.cipherSuite;
#ifdef WOLFSSL_DEBUG_TLS
    WOLFSSL_MSG("Chosen cipher suite:");
    WOLFSSL_MSG(GetCipherNameInternal(ssl->options.cipherSuite0,
                                      ssl->options.cipherSuite));
#endif

    /* Compression not supported in TLS v1.3. */
    output[idx++] = 0;

    /* Extensions */
    ret = TLSX_WriteResponse(ssl, output + idx, extMsgType, NULL);
    if (ret != 0)
        return ret;

#ifdef WOLFSSL_SEND_HRR_COOKIE
    if (ssl->options.sendCookie && extMsgType == hello_retry_request) {
        /* Reset the hashes from here. We will be able to restart the hashes
         * from the cookie in RestartHandshakeHashWithCookie */
#ifdef WOLFSSL_DTLS13
        /* When ssl->options.dtlsStateful is not set then cookie is calculated
         * in dtls.c */
        if (ssl->options.dtls && !ssl->options.dtlsStateful)
            ret = 0;
        else
#endif
            ret = InitHandshakeHashes(ssl);
    }
    else
#endif
    {
#ifdef WOLFSSL_DTLS13
        if (ssl->options.dtls) {
            ret = Dtls13HashHandshake(
                ssl,
                output + Dtls13GetRlHeaderLength(ssl, 0) ,
                (word16)sendSz - Dtls13GetRlHeaderLength(ssl, 0));
        }
        else
#endif /* WOLFSSL_DTLS13 */
        {
#if defined(HAVE_ECH)
            if (ssl->ctx->echConfigs != NULL) {
                echX = TLSX_Find(ssl->extensions, TLSX_ECH);

                if (echX == NULL)
                    return -1;

                /* replace the last 8 bytes of server random with the accept */
                if (((WOLFSSL_ECH*)echX->data)->state == ECH_PARSED_INTERNAL) {
                    ret = EchWriteAcceptance(ssl, output + RECORD_HEADER_SZ,
                        serverRandomOffset - RECORD_HEADER_SZ,
                        sendSz - RECORD_HEADER_SZ);

                    /* remove ech so we don't keep sending it in write */
                    TLSX_Remove(&ssl->extensions, TLSX_ECH, ssl->heap);
                }
            }
#endif
            if (ret == 0)
                ret = HashOutput(ssl, output, sendSz, 0);
        }
    }

    if (ret != 0)
        return ret;

#if defined(WOLFSSL_CALLBACKS) || defined(OPENSSL_EXTRA)
    if (ssl->hsInfoOn)
        AddPacketName(ssl, "ServerHello");
    if (ssl->toInfoOn) {
        ret = AddPacketInfo(ssl, "ServerHello", handshake, output, sendSz,
                      WRITE_PROTO, 0, ssl->heap);
        if (ret != 0)
            return ret;
    }
    #endif

    if (extMsgType == server_hello)
        ssl->options.serverState = SERVER_HELLO_COMPLETE;

    ssl->options.buildingMsg = 0;
#ifdef WOLFSSL_DTLS13
    if (ssl->options.dtls) {
        ret = Dtls13HandshakeSend(ssl, output, (word16)sendSz, (word16)sendSz,
            (enum HandShakeType)extMsgType, 0);

        WOLFSSL_LEAVE("SendTls13ServerHello", ret);
        WOLFSSL_END(WC_FUNC_SERVER_HELLO_SEND);
        return ret;
    }
#endif /* WOLFSSL_DTLS13 */

    ssl->buffers.outputBuffer.length += sendSz;

    if (!ssl->options.groupMessages || extMsgType != server_hello)
        ret = SendBuffered(ssl);

    WOLFSSL_LEAVE("SendTls13ServerHello", ret);
    WOLFSSL_END(WC_FUNC_SERVER_HELLO_SEND);

    return ret;
}

/* handle generation of TLS 1.3 encrypted_extensions (8) */
/* Send the rest of the extensions encrypted under the handshake key.
 * This message is always encrypted in TLS v1.3.
 * Only a server will send this message.
 *
 * ssl  The SSL/TLS object.
 * returns 0 on success, otherwise failure.
 */
static int SendTls13EncryptedExtensions(WOLFSSL* ssl)
{
    int    ret;
    byte*  output;
    word16 length = 0;
    word32 idx;
    int    sendSz;

    WOLFSSL_START(WC_FUNC_ENCRYPTED_EXTENSIONS_SEND);
    WOLFSSL_ENTER("SendTls13EncryptedExtensions");

    ssl->options.buildingMsg = 1;
    ssl->keys.encryptionOn = 1;

#ifdef WOLFSSL_DTLS13
    if (ssl->options.dtls) {
        idx = Dtls13GetHeadersLength(ssl, encrypted_extensions);
    }
    else
#endif /* WOLFSSL_DTLS13 */
    {
        idx = RECORD_HEADER_SZ + HANDSHAKE_HEADER_SZ;
    }

#if defined(HAVE_SUPPORTED_CURVES) && !defined(WOLFSSL_NO_SERVER_GROUPS_EXT)
    if ((ret = TLSX_SupportedCurve_CheckPriority(ssl)) != 0)
        return ret;
#endif

    /* Derive the handshake secret now that we are at first message to be
     * encrypted under the keys.
     */
    if ((ret = DeriveHandshakeSecret(ssl)) != 0)
        return ret;
    if ((ret = DeriveTls13Keys(ssl, handshake_key,
                               ENCRYPT_AND_DECRYPT_SIDE, 1)) != 0)
        return ret;

    /* Setup encrypt/decrypt keys for following messages. */
#ifdef WOLFSSL_EARLY_DATA
    if ((ret = SetKeysSide(ssl, ENCRYPT_SIDE_ONLY)) != 0)
        return ret;
    if (ssl->earlyData != process_early_data) {
        if ((ret = SetKeysSide(ssl, DECRYPT_SIDE_ONLY)) != 0)
            return ret;
    }
#else
    if ((ret = SetKeysSide(ssl, ENCRYPT_AND_DECRYPT_SIDE)) != 0)
        return ret;
#endif
#ifdef WOLFSSL_QUIC
    if (IsAtLeastTLSv1_3(ssl->version) && WOLFSSL_IS_QUIC(ssl)) {
        ret = wolfSSL_quic_add_transport_extensions(ssl, encrypted_extensions);
        if (ret != 0)
            return ret;
    }
#endif

#ifdef WOLFSSL_DTLS13
    if (ssl->options.dtls) {
        w64wrapper epochHandshake = w64From32(0, DTLS13_EPOCH_HANDSHAKE);
        ssl->dtls13Epoch = epochHandshake;

        ret = Dtls13NewEpoch(
            ssl, epochHandshake, ENCRYPT_AND_DECRYPT_SIDE);
        if (ret != 0)
            return ret;

        ret = Dtls13SetEpochKeys(
            ssl, epochHandshake, ENCRYPT_AND_DECRYPT_SIDE);
        if (ret != 0)
            return ret;

    }
#endif /* WOLFSSL_DTLS13 */

    ret = TLSX_GetResponseSize(ssl, encrypted_extensions, &length);
    if (ret != 0)
        return ret;

    sendSz = idx + length;
    /* Encryption always on. */
    sendSz += MAX_MSG_EXTRA;

    /* Check buffers are big enough and grow if needed. */
    ret = CheckAvailableSize(ssl, sendSz);
    if (ret != 0)
        return ret;

    /* Get position in output buffer to write new message to. */
    output = GetOutputBuffer(ssl);

    /* Put the record and handshake headers on. */
    AddTls13Headers(output, length, encrypted_extensions, ssl);

    ret = TLSX_WriteResponse(ssl, output + idx, encrypted_extensions, NULL);
    if (ret != 0)
        return ret;
    idx += length;

#if defined(WOLFSSL_CALLBACKS) || defined(OPENSSL_EXTRA)
    if (ssl->hsInfoOn)
        AddPacketName(ssl, "EncryptedExtensions");
    if (ssl->toInfoOn) {
        ret = AddPacketInfo(ssl, "EncryptedExtensions", handshake, output,
                      sendSz, WRITE_PROTO, 0, ssl->heap);
        if (ret != 0)
            return ret;
    }
#endif

#ifdef WOLFSSL_DTLS13
    if (ssl->options.dtls) {
        ssl->options.buildingMsg = 0;
        ret = Dtls13HandshakeSend(ssl, output, (word16)sendSz, (word16)idx,
                                  encrypted_extensions, 1);

        if (ret == 0)
            ssl->options.serverState = SERVER_ENCRYPTED_EXTENSIONS_COMPLETE;

        WOLFSSL_LEAVE("SendTls13EncryptedExtensions", ret);
        WOLFSSL_END(WC_FUNC_ENCRYPTED_EXTENSIONS_SEND);

        return ret;
    }
#endif /* WOLFSSL_DTLS13 */

    /* This handshake message is always encrypted. */
    sendSz = BuildTls13Message(ssl, output, sendSz, output + RECORD_HEADER_SZ,
                               idx - RECORD_HEADER_SZ, handshake, 1, 0, 0);
    if (sendSz < 0)
        return sendSz;

    ssl->buffers.outputBuffer.length += sendSz;
    ssl->options.buildingMsg = 0;
    ssl->options.serverState = SERVER_ENCRYPTED_EXTENSIONS_COMPLETE;

    if (!ssl->options.groupMessages)
        ret = SendBuffered(ssl);


    WOLFSSL_LEAVE("SendTls13EncryptedExtensions", ret);
    WOLFSSL_END(WC_FUNC_ENCRYPTED_EXTENSIONS_SEND);

    return ret;
}

#ifndef NO_CERTS
/* handle generation TLS v1.3 certificate_request (13) */
/* Send the TLS v1.3 CertificateRequest message.
 * This message is always encrypted in TLS v1.3.
 * Only a server will send this message.
 *
 * ssl        SSL/TLS object.
 * reqCtx     Request context.
 * reqCtxLen  Length of context. 0 when sending as part of handshake.
 * returns 0 on success, otherwise failure.
 */
static int SendTls13CertificateRequest(WOLFSSL* ssl, byte* reqCtx,
                                       int reqCtxLen)
{
    byte*   output;
    int    ret;
    int    sendSz;
    word32 i;
    word16 reqSz;
    word16 hashSigAlgoSz = 0;
    SignatureAlgorithms* sa;
    int haveSig = SIG_RSA | SIG_ECDSA | SIG_FALCON | SIG_DILITHIUM;
#if defined(WOLFSSL_SM2) && defined(WOLFSSL_SM3)
    haveSig |= SIG_SM2;
#endif

    WOLFSSL_START(WC_FUNC_CERTIFICATE_REQUEST_SEND);
    WOLFSSL_ENTER("SendTls13CertificateRequest");

    ssl->options.buildingMsg = 1;

    if (ssl->options.side != WOLFSSL_SERVER_END)
        return SIDE_ERROR;

    /* Get the length of the hashSigAlgo buffer */
    InitSuitesHashSigAlgo_ex2(NULL, haveSig, 1, ssl->buffers.keySz,
        &hashSigAlgoSz);
    sa = TLSX_SignatureAlgorithms_New(ssl, hashSigAlgoSz, ssl->heap);
    if (sa == NULL)
        return MEMORY_ERROR;
    InitSuitesHashSigAlgo_ex2(sa->hashSigAlgo, haveSig, 1, ssl->buffers.keySz,
        &hashSigAlgoSz);
    ret = TLSX_Push(&ssl->extensions, TLSX_SIGNATURE_ALGORITHMS, sa, ssl->heap);
    if (ret != 0) {
        TLSX_SignatureAlgorithms_FreeAll(sa, ssl->heap);
        return ret;
    }

    i = RECORD_HEADER_SZ + HANDSHAKE_HEADER_SZ;
#ifdef WOLFSSL_DTLS13
    if (ssl->options.dtls)
        i = Dtls13GetRlHeaderLength(ssl, 1) + DTLS_HANDSHAKE_HEADER_SZ;
#endif /* WOLFSSL_DTLS13 */

    reqSz = (word16)(OPAQUE8_LEN + reqCtxLen);
    ret = TLSX_GetRequestSize(ssl, certificate_request, &reqSz);
    if (ret != 0)
        return ret;

    sendSz = i + reqSz;
    /* Always encrypted and make room for padding. */
    sendSz += MAX_MSG_EXTRA;

    /* Check buffers are big enough and grow if needed. */
    if ((ret = CheckAvailableSize(ssl, sendSz)) != 0)
        return ret;

    /* Get position in output buffer to write new message to. */
    output = GetOutputBuffer(ssl);

    /* Put the record and handshake headers on. */
    AddTls13Headers(output, reqSz, certificate_request, ssl);

    /* Certificate request context. */
    output[i++] = (byte)reqCtxLen;
    if (reqCtxLen != 0) {
        XMEMCPY(output + i, reqCtx, reqCtxLen);
        i += reqCtxLen;
    }

    /* Certificate extensions. */
    reqSz = 0;
    ret = TLSX_WriteRequest(ssl, output + i, certificate_request, &reqSz);
    if (ret != 0)
        return ret;
    i += reqSz;

#ifdef WOLFSSL_DTLS13
    if (ssl->options.dtls) {
        ssl->options.buildingMsg = 0;
        ret =
            Dtls13HandshakeSend(ssl, output, (word16)sendSz, (word16)i,
                                certificate_request, 1);

        WOLFSSL_LEAVE("SendTls13CertificateRequest", ret);
        WOLFSSL_END(WC_FUNC_CERTIFICATE_REQUEST_SEND);

        return ret;

    }
#endif /* WOLFSSL_DTLS13 */

    /* Always encrypted. */
    sendSz = BuildTls13Message(ssl, output, sendSz, output + RECORD_HEADER_SZ,
                               i - RECORD_HEADER_SZ, handshake, 1, 0, 0);
    if (sendSz < 0)
        return sendSz;

    #if defined(WOLFSSL_CALLBACKS) || defined(OPENSSL_EXTRA)
        if (ssl->hsInfoOn)
            AddPacketName(ssl, "CertificateRequest");
        if (ssl->toInfoOn) {
            ret = AddPacketInfo(ssl, "CertificateRequest", handshake, output,
                          sendSz, WRITE_PROTO, 0, ssl->heap);
            if (ret != 0)
                return ret;
        }
    #endif

    ssl->buffers.outputBuffer.length += sendSz;
    ssl->options.buildingMsg = 0;
    if (!ssl->options.groupMessages)
        ret = SendBuffered(ssl);

    WOLFSSL_LEAVE("SendTls13CertificateRequest", ret);
    WOLFSSL_END(WC_FUNC_CERTIFICATE_REQUEST_SEND);

    return ret;
}
#endif /* NO_CERTS */
#endif /* NO_WOLFSSL_SERVER */

#ifndef NO_CERTS
#if !defined(NO_RSA) || defined(HAVE_ECC) || defined(HAVE_ED25519) || \
    defined(HAVE_ED448) || defined(HAVE_PQC)
/* Encode the signature algorithm into buffer.
 *
 * hashalgo  The hash algorithm.
 * hsType   The signature type.
 * output    The buffer to encode into.
 */
static WC_INLINE void EncodeSigAlg(byte hashAlgo, byte hsType, byte* output)
{
    switch (hsType) {
#ifdef HAVE_ECC
        case ecc_dsa_sa_algo:
            output[0] = hashAlgo;
            output[1] = ecc_dsa_sa_algo;
            break;
#endif
#if defined(WOLFSSL_SM2) && defined(WOLFSSL_SM3)
        case sm2_sa_algo:
            output[0] = SM2_SA_MAJOR;
            output[1] = SM2_SA_MINOR;
            break;
#endif
#ifdef HAVE_ED25519
        /* ED25519: 0x0807 */
        case ed25519_sa_algo:
            output[0] = ED25519_SA_MAJOR;
            output[1] = ED25519_SA_MINOR;
            (void)hashAlgo;
            break;
#endif
#ifdef HAVE_ED448
        /* ED448: 0x0808 */
        case ed448_sa_algo:
            output[0] = ED448_SA_MAJOR;
            output[1] = ED448_SA_MINOR;
            (void)hashAlgo;
            break;
#endif
#ifndef NO_RSA
        /* PSS signatures: 0x080[4-6] */
        case rsa_pss_sa_algo:
            output[0] = rsa_pss_sa_algo;
            output[1] = hashAlgo;
            break;
#endif
#ifdef HAVE_PQC
        #ifdef HAVE_FALCON
        case falcon_level1_sa_algo:
            output[0] = FALCON_LEVEL1_SA_MAJOR;
            output[1] = FALCON_LEVEL1_SA_MINOR;
            break;
        case falcon_level5_sa_algo:
            output[0] = FALCON_LEVEL5_SA_MAJOR;
            output[1] = FALCON_LEVEL5_SA_MINOR;
            break;
        #endif
        #ifdef HAVE_DILITHIUM
        case dilithium_level2_sa_algo:
            output[0] = DILITHIUM_LEVEL2_SA_MAJOR;
            output[1] = DILITHIUM_LEVEL2_SA_MINOR;
            break;
        case dilithium_level3_sa_algo:
            output[0] = DILITHIUM_LEVEL3_SA_MAJOR;
            output[1] = DILITHIUM_LEVEL3_SA_MINOR;
            break;
        case dilithium_level5_sa_algo:
            output[0] = DILITHIUM_LEVEL5_SA_MAJOR;
            output[1] = DILITHIUM_LEVEL5_SA_MINOR;
            break;
        #endif
#endif
        default:
            break;
    }
}

/* Decode the signature algorithm.
 *
 * input     The encoded signature algorithm.
 * hashalgo  The hash algorithm.
 * hsType    The signature type.
 * returns INVALID_PARAMETER if not recognized and 0 otherwise.
 */
static WC_INLINE int DecodeTls13SigAlg(byte* input, byte* hashAlgo,
                                       byte* hsType)
{
    int ret = 0;

    switch (input[0]) {
    #if defined(WOLFSSL_SM2) && defined(WOLFSSL_SM3)
        case SM2_SA_MAJOR:
            if (input[1] == SM2_SA_MINOR) {
                *hsType = sm2_sa_algo;
                *hashAlgo = sm3_mac;
            }
            else
                ret = INVALID_PARAMETER;
            break;
    #endif
        case NEW_SA_MAJOR:
            /* PSS signatures: 0x080[4-6] */
            if (input[1] >= sha256_mac && input[1] <= sha512_mac) {
                *hsType   = input[0];
                *hashAlgo = input[1];
            }
    #ifdef HAVE_ED25519
            /* ED25519: 0x0807 */
            else if (input[1] == ED25519_SA_MINOR) {
                *hsType = ed25519_sa_algo;
                /* Hash performed as part of sign/verify operation. */
                *hashAlgo = sha512_mac;
            }
    #endif
    #ifdef HAVE_ED448
            /* ED448: 0x0808 */
            else if (input[1] == ED448_SA_MINOR) {
                *hsType = ed448_sa_algo;
                /* Hash performed as part of sign/verify operation. */
                *hashAlgo = sha512_mac;
            }
    #endif
            else
                ret = INVALID_PARAMETER;
            break;
#ifdef HAVE_PQC
        case PQC_SA_MAJOR:
#if defined(HAVE_FALCON)
            if (input[1] == FALCON_LEVEL1_SA_MINOR) {
                *hsType = falcon_level1_sa_algo;
                /* Hash performed as part of sign/verify operation. */
                *hashAlgo = sha512_mac;
            } else if (input[1] == FALCON_LEVEL5_SA_MINOR) {
                *hsType = falcon_level5_sa_algo;
                /* Hash performed as part of sign/verify operation. */
                *hashAlgo = sha512_mac;
            }
            else
#endif /* HAVE_FALCON */
#if defined(HAVE_DILITHIUM)
            if (input[1] == DILITHIUM_LEVEL2_SA_MINOR) {
                *hsType = dilithium_level2_sa_algo;
                /* Hash performed as part of sign/verify operation. */
                *hashAlgo = sha512_mac;
            } else if (input[1] == DILITHIUM_LEVEL3_SA_MINOR) {
                *hsType = dilithium_level3_sa_algo;
                /* Hash performed as part of sign/verify operation. */
                *hashAlgo = sha512_mac;
            } else if (input[1] == DILITHIUM_LEVEL5_SA_MINOR) {
                *hsType = dilithium_level5_sa_algo;
                /* Hash performed as part of sign/verify operation. */
                *hashAlgo = sha512_mac;
            }
            else
#endif /* HAVE_DILITHIUM */
            {
                ret = INVALID_PARAMETER;
            }
            break;
#endif
        default:
            *hashAlgo = input[0];
            *hsType   = input[1];
            break;
    }

    return ret;
}

/* Get the hash of the messages so far.
 *
 * ssl   The SSL/TLS object.
 * hash  The buffer to write the hash to.
 * returns the length of the hash.
 */
static WC_INLINE int GetMsgHash(WOLFSSL* ssl, byte* hash)
{
    int ret = 0;
    switch (ssl->specs.mac_algorithm) {
    #ifndef NO_SHA256
        case sha256_mac:
            ret = wc_Sha256GetHash(&ssl->hsHashes->hashSha256, hash);
            if (ret == 0)
                ret = WC_SHA256_DIGEST_SIZE;
            break;
    #endif /* !NO_SHA256 */
    #ifdef WOLFSSL_SHA384
        case sha384_mac:
            ret = wc_Sha384GetHash(&ssl->hsHashes->hashSha384, hash);
            if (ret == 0)
                ret = WC_SHA384_DIGEST_SIZE;
            break;
    #endif /* WOLFSSL_SHA384 */
    #ifdef WOLFSSL_TLS13_SHA512
        case sha512_mac:
            ret = wc_Sha512GetHash(&ssl->hsHashes->hashSha512, hash);
            if (ret == 0)
                ret = WC_SHA512_DIGEST_SIZE;
            break;
    #endif /* WOLFSSL_TLS13_SHA512 */
    #ifdef WOLFSSL_SM3
        case sm3_mac:
            ret = wc_Sm3GetHash(&ssl->hsHashes->hashSm3, hash);
            if (ret == 0)
                ret = WC_SM3_DIGEST_SIZE;
            break;
    #endif /* WOLFSSL_SM3 */
        default:
            break;
    }
    return ret;
}

/* The server certificate verification label. */
static const byte serverCertVfyLabel[CERT_VFY_LABEL_SZ] =
    "TLS 1.3, server CertificateVerify";
/* The client certificate verification label. */
static const byte clientCertVfyLabel[CERT_VFY_LABEL_SZ] =
    "TLS 1.3, client CertificateVerify";
/* The prefix byte in the signature data. */
#define SIGNING_DATA_PREFIX_BYTE   0x20

/* Create the signature data for TLS v1.3 certificate verification.
 *
 * ssl        The SSL/TLS object.
 * sigData    The signature data.
 * sigDataSz  The length of the signature data.
 * check      Indicates this is a check not create.
 */
int CreateSigData(WOLFSSL* ssl, byte* sigData, word16* sigDataSz,
                  int check)
{
    word16 idx;
    int side = ssl->options.side;
    int ret;

    /* Signature Data = Prefix | Label | Handshake Hash */
    XMEMSET(sigData, SIGNING_DATA_PREFIX_BYTE, SIGNING_DATA_PREFIX_SZ);
    idx = SIGNING_DATA_PREFIX_SZ;

    if ((side == WOLFSSL_SERVER_END && check) ||
        (side == WOLFSSL_CLIENT_END && !check)) {
        XMEMCPY(&sigData[idx], clientCertVfyLabel, CERT_VFY_LABEL_SZ);
    }
    if ((side == WOLFSSL_CLIENT_END && check) ||
        (side == WOLFSSL_SERVER_END && !check)) {
        XMEMCPY(&sigData[idx], serverCertVfyLabel, CERT_VFY_LABEL_SZ);
    }
    idx += CERT_VFY_LABEL_SZ;

    ret = GetMsgHash(ssl, &sigData[idx]);
    if (ret < 0)
        return ret;

    *sigDataSz = (word16)(idx + ret);
    ret = 0;

    return ret;
}

#ifndef NO_RSA
/* Encode the PKCS #1.5 RSA signature.
 *
 * sig        The buffer to place the encoded signature into.
 * sigData    The data to be signed.
 * sigDataSz  The size of the data to be signed.
 * hashAlgo   The hash algorithm to use when signing.
 * returns the length of the encoded signature or negative on error.
 */
int CreateRSAEncodedSig(byte* sig, byte* sigData, int sigDataSz,
                        int sigAlgo, int hashAlgo)
{
    Digest digest;
    int    hashSz = 0;
    int    ret = BAD_FUNC_ARG;
    byte*  hash;

    (void)sigAlgo;

    hash = sig;

    /* Digest the signature data. */
    switch (hashAlgo) {
#ifndef NO_WOLFSSL_SHA256
        case sha256_mac:
            ret = wc_InitSha256(&digest.sha256);
            if (ret == 0) {
                ret = wc_Sha256Update(&digest.sha256, sigData, sigDataSz);
                if (ret == 0)
                    ret = wc_Sha256Final(&digest.sha256, hash);
                wc_Sha256Free(&digest.sha256);
            }
            hashSz = WC_SHA256_DIGEST_SIZE;
            break;
#endif
#ifdef WOLFSSL_SHA384
        case sha384_mac:
            ret = wc_InitSha384(&digest.sha384);
            if (ret == 0) {
                ret = wc_Sha384Update(&digest.sha384, sigData, sigDataSz);
                if (ret == 0)
                    ret = wc_Sha384Final(&digest.sha384, hash);
                wc_Sha384Free(&digest.sha384);
            }
            hashSz = WC_SHA384_DIGEST_SIZE;
            break;
#endif
#ifdef WOLFSSL_SHA512
        case sha512_mac:
            ret = wc_InitSha512(&digest.sha512);
            if (ret == 0) {
                ret = wc_Sha512Update(&digest.sha512, sigData, sigDataSz);
                if (ret == 0)
                    ret = wc_Sha512Final(&digest.sha512, hash);
                wc_Sha512Free(&digest.sha512);
            }
            hashSz = WC_SHA512_DIGEST_SIZE;
            break;
#endif
    }

    if (ret != 0)
        return ret;

    return hashSz;
}
#endif /* !NO_RSA */

#ifdef HAVE_ECC
/* Encode the ECC signature.
 *
 * sigData    The data to be signed.
 * sigDataSz  The size of the data to be signed.
 * hashAlgo   The hash algorithm to use when signing.
 * returns the length of the encoded signature or negative on error.
 */
static int CreateECCEncodedSig(byte* sigData, int sigDataSz, int hashAlgo)
{
    Digest digest;
    int    hashSz = 0;
    int    ret = BAD_FUNC_ARG;

    /* Digest the signature data. */
    switch (hashAlgo) {
#ifndef NO_WOLFSSL_SHA256
        case sha256_mac:
            ret = wc_InitSha256(&digest.sha256);
            if (ret == 0) {
                ret = wc_Sha256Update(&digest.sha256, sigData, sigDataSz);
                if (ret == 0)
                    ret = wc_Sha256Final(&digest.sha256, sigData);
                wc_Sha256Free(&digest.sha256);
            }
            hashSz = WC_SHA256_DIGEST_SIZE;
            break;
#endif
#ifdef WOLFSSL_SHA384
        case sha384_mac:
            ret = wc_InitSha384(&digest.sha384);
            if (ret == 0) {
                ret = wc_Sha384Update(&digest.sha384, sigData, sigDataSz);
                if (ret == 0)
                    ret = wc_Sha384Final(&digest.sha384, sigData);
                wc_Sha384Free(&digest.sha384);
            }
            hashSz = WC_SHA384_DIGEST_SIZE;
            break;
#endif
#ifdef WOLFSSL_SHA512
        case sha512_mac:
            ret = wc_InitSha512(&digest.sha512);
            if (ret == 0) {
                ret = wc_Sha512Update(&digest.sha512, sigData, sigDataSz);
                if (ret == 0)
                    ret = wc_Sha512Final(&digest.sha512, sigData);
                wc_Sha512Free(&digest.sha512);
            }
            hashSz = WC_SHA512_DIGEST_SIZE;
            break;
#endif
        default:
            break;
    }

    if (ret != 0)
        return ret;

    return hashSz;
}
#endif /* HAVE_ECC */

#if !defined(NO_RSA) && defined(WC_RSA_PSS)
/* Check that the decrypted signature matches the encoded signature
 * based on the digest of the signature data.
 *
 * ssl       The SSL/TLS object.
 * sigAlgo   The signature algorithm used to generate signature.
 * hashAlgo  The hash algorithm used to generate signature.
 * decSig    The decrypted signature.
 * decSigSz  The size of the decrypted signature.
 * returns 0 on success, otherwise failure.
 */
static int CheckRSASignature(WOLFSSL* ssl, int sigAlgo, int hashAlgo,
                             byte* decSig, word32 decSigSz)
{
    int    ret = 0;
    byte   sigData[MAX_SIG_DATA_SZ];
    word16 sigDataSz;

    ret = CreateSigData(ssl, sigData, &sigDataSz, 1);
    if (ret != 0)
        return ret;

    if (sigAlgo == rsa_pss_sa_algo) {
        enum wc_HashType hashType = WC_HASH_TYPE_NONE;
        word32 sigSz;

        ret = ConvertHashPss(hashAlgo, &hashType, NULL);
        if (ret < 0)
            return ret;

        /* PSS signature can be done in-place */
        ret = CreateRSAEncodedSig(sigData, sigData, sigDataSz,
                                  sigAlgo, hashAlgo);
        if (ret < 0)
            return ret;
        sigSz = ret;

        ret = wc_RsaPSS_CheckPadding(sigData, sigSz, decSig, decSigSz,
                                     hashType);
    }

    return ret;
}
#endif /* !NO_RSA && WC_RSA_PSS */
#endif /* !NO_RSA || HAVE_ECC */

#if !defined(NO_WOLFSSL_CLIENT) || !defined(NO_WOLFSSL_SERVER)
/* Get the next certificate from the list for writing into the TLS v1.3
 * Certificate message.
 *
 * data    The certificate list.
 * length  The length of the certificate data in the list.
 * idx     The index of the next certificate.
 * returns the length of the certificate data. 0 indicates no more certificates
 * in the list.
 */
static word32 NextCert(byte* data, word32 length, word32* idx)
{
    word32 len;

    /* Is index at end of list. */
    if (*idx == length)
        return 0;

    /* Length of the current ASN.1 encoded certificate. */
    c24to32(data + *idx, &len);
    /* Include the length field. */
    len += 3;

    /* Move index to next certificate and return the current certificate's
     * length.
     */
    *idx += len;
    return len;
}

/* Add certificate data and empty extension to output up to the fragment size.
 *
 * ssl     SSL/TLS object.
 * cert    The certificate data to write out.
 * len     The length of the certificate data.
 * extSz   Length of the extension data with the certificate.
 * idx     The start of the certificate data to write out.
 * fragSz  The maximum size of this fragment.
 * output  The buffer to write to.
 * returns the number of bytes written.
 */
static word32 AddCertExt(WOLFSSL* ssl, byte* cert, word32 len, word16 extSz,
                         word32 idx, word32 fragSz, byte* output)
{
    word32 i = 0;
    word32 copySz = min(len - idx, fragSz);

    if (idx < len) {
        XMEMCPY(output, cert + idx, copySz);
        i = copySz;
        if (copySz == fragSz)
            return i;
    }
    copySz = len + extSz - idx - i;

    if (extSz == OPAQUE16_LEN) {
        if (copySz <= fragSz) {
            /* Empty extension */
            output[i++] = 0;
            output[i++] = 0;
        }
    }
    else {
        byte* certExts = ssl->buffers.certExts->buffer + idx + i - len;
        /* Put out as much of the extensions' data as will fit in fragment. */
        if (copySz > fragSz - i)
            copySz = fragSz - i;
        XMEMCPY(output + i, certExts, copySz);
        i += copySz;
    }

    return i;
}

/* handle generation TLS v1.3 certificate (11) */
/* Send the certificate for this end and any CAs that help with validation.
 * This message is always encrypted in TLS v1.3.
 *
 * ssl  The SSL/TLS object.
 * returns 0 on success, otherwise failure.
 */
static int SendTls13Certificate(WOLFSSL* ssl)
{
    int    ret = 0;
    word32 certSz, certChainSz, headerSz, listSz, payloadSz;
    word16 extSz = 0;
    word32 length, maxFragment;
    word32 len = 0;
    word32 idx = 0;
    word32 offset = OPAQUE16_LEN;
    byte*  p = NULL;
    byte   certReqCtxLen = 0;
#ifdef WOLFSSL_POST_HANDSHAKE_AUTH
    byte*  certReqCtx = NULL;
#endif

#ifdef OPENSSL_EXTRA
    WOLFSSL_X509* x509 = NULL;
    WOLFSSL_EVP_PKEY* pkey = NULL;
#endif

    WOLFSSL_START(WC_FUNC_CERTIFICATE_SEND);
    WOLFSSL_ENTER("SendTls13Certificate");

    ssl->options.buildingMsg = 1;

#ifdef WOLFSSL_POST_HANDSHAKE_AUTH
    if (ssl->options.side == WOLFSSL_CLIENT_END && ssl->certReqCtx != NULL) {
        certReqCtxLen = ssl->certReqCtx->len;
        certReqCtx = &ssl->certReqCtx->ctx;
    }
#endif

#ifdef OPENSSL_EXTRA
    /* call client cert callback if no cert has been loaded */
    if ((ssl->ctx->CBClientCert != NULL) &&
        (!ssl->buffers.certificate || !ssl->buffers.certificate->buffer)) {
        ret = ssl->ctx->CBClientCert(ssl, &x509, &pkey);
        if (ret == 1) {
            if ((wolfSSL_CTX_use_certificate(ssl->ctx, x509) == WOLFSSL_SUCCESS) &&
                (wolfSSL_CTX_use_PrivateKey(ssl->ctx, pkey) == WOLFSSL_SUCCESS)) {
                ssl->options.sendVerify = SEND_CERT;
            }
            wolfSSL_X509_free(x509);
            wolfSSL_EVP_PKEY_free(pkey);
        }
    }
#endif

    if (ssl->options.sendVerify == SEND_BLANK_CERT) {
        certSz = 0;
        certChainSz = 0;
        headerSz = OPAQUE8_LEN + certReqCtxLen + CERT_HEADER_SZ;
        length = headerSz;
        listSz = 0;
    }
    else {
#ifdef OPENSSL_EXTRA
        if ((ret = CertSetupCbWrapper(ssl)) != 0)
            return ret;
#endif

        if (!ssl->buffers.certificate) {
            WOLFSSL_MSG("Send Cert missing certificate buffer");
            return BUFFER_ERROR;
        }
        /* Certificate Data */
        certSz = ssl->buffers.certificate->length;
        /* Cert Req Ctx Len | Cert Req Ctx | Cert List Len | Cert Data Len */
        headerSz = OPAQUE8_LEN + certReqCtxLen + CERT_HEADER_SZ +
                   CERT_HEADER_SZ;

        ret = TLSX_GetResponseSize(ssl, certificate, &extSz);
        if (ret < 0)
            return ret;

        /* Create extensions' data if none already present. */
        if (extSz > OPAQUE16_LEN && ssl->buffers.certExts == NULL) {
            ret = AllocDer(&ssl->buffers.certExts, extSz, CERT_TYPE, ssl->heap);
            if (ret < 0)
                return ret;

            extSz = 0;
            ret = TLSX_WriteResponse(ssl, ssl->buffers.certExts->buffer,
                                                           certificate, &extSz);
            if (ret < 0)
                return ret;
        }

        /* Length of message data with one certificate and extensions. */
        length = headerSz + certSz + extSz;
        /* Length of list data with one certificate and extensions. */
        listSz = CERT_HEADER_SZ + certSz + extSz;

        /* Send rest of chain if sending cert (chain has leading size/s). */
        if (certSz > 0 && ssl->buffers.certChainCnt > 0) {
            p = ssl->buffers.certChain->buffer;
            /* Chain length including extensions. */
            certChainSz = ssl->buffers.certChain->length +
                          OPAQUE16_LEN * ssl->buffers.certChainCnt;
            length += certChainSz;
            listSz += certChainSz;
        }
        else
            certChainSz = 0;
    }

    payloadSz = length;

    if (ssl->fragOffset != 0)
        length -= (ssl->fragOffset + headerSz);

    maxFragment = wolfSSL_GetMaxFragSize(ssl, MAX_RECORD_SIZE);

    while (length > 0 && ret == 0) {
        byte*  output = NULL;
        word32 fragSz = 0;
        word32 i = RECORD_HEADER_SZ;
        int    sendSz = RECORD_HEADER_SZ;

#ifdef WOLFSSL_DTLS13
        if (ssl->options.dtls) {
            i = Dtls13GetRlHeaderLength(ssl, 1);
            sendSz = (int)i;
        }
#endif /* WOLFSSL_DTLS13 */

        if (ssl->fragOffset == 0) {
            if (headerSz + certSz + extSz + certChainSz <=
                                            maxFragment - HANDSHAKE_HEADER_SZ) {
                fragSz = headerSz + certSz + extSz + certChainSz;
            }
#ifdef WOLFSSL_DTLS13
            else if (ssl->options.dtls){
                /* short-circuit the fragmentation logic here. DTLS
                   fragmentation will be done in dtls13HandshakeSend() */
                fragSz = headerSz + certSz + extSz + certChainSz;
            }
#endif /* WOLFSSL_DTLS13 */
            else {
                fragSz = maxFragment - HANDSHAKE_HEADER_SZ;
            }

            sendSz += fragSz + HANDSHAKE_HEADER_SZ;
            i += HANDSHAKE_HEADER_SZ;
#ifdef WOLFSSL_DTLS13
            if (ssl->options.dtls) {
                sendSz += DTLS_HANDSHAKE_EXTRA;
                i += DTLS_HANDSHAKE_EXTRA;
            }
#endif /* WOLFSSL_DTLS13 */
        }
        else {
            fragSz = min(length, maxFragment);
            sendSz += fragSz;
        }

        sendSz += MAX_MSG_EXTRA;

        /* Check buffers are big enough and grow if needed. */
        if ((ret = CheckAvailableSize(ssl, sendSz)) != 0)
            return ret;

        /* Get position in output buffer to write new message to. */
        output = GetOutputBuffer(ssl);

        if (ssl->fragOffset == 0) {
            AddTls13FragHeaders(output, fragSz, 0, payloadSz, certificate, ssl);

            /* Request context. */
            output[i++] = certReqCtxLen;
        #ifdef WOLFSSL_POST_HANDSHAKE_AUTH
            if (certReqCtxLen > 0) {
                XMEMCPY(output + i, certReqCtx, certReqCtxLen);
                i += certReqCtxLen;
            }
        #endif
            length -= OPAQUE8_LEN + certReqCtxLen;
            fragSz -= OPAQUE8_LEN + certReqCtxLen;
            /* Certificate list length. */
            c32to24(listSz, output + i);
            i += CERT_HEADER_SZ;
            length -= CERT_HEADER_SZ;
            fragSz -= CERT_HEADER_SZ;
            /* Leaf certificate data length. */
            if (certSz > 0) {
                c32to24(certSz, output + i);
                i += CERT_HEADER_SZ;
                length -= CERT_HEADER_SZ;
                fragSz -= CERT_HEADER_SZ;
            }
        }
        else
            AddTls13RecordHeader(output, fragSz, handshake, ssl);

        if (certSz > 0 && ssl->fragOffset < certSz + extSz) {
            /* Put in the leaf certificate with extensions. */
            word32 copySz = AddCertExt(ssl, ssl->buffers.certificate->buffer,
                            certSz, extSz, ssl->fragOffset, fragSz, output + i);
            i += copySz;
            ssl->fragOffset += copySz;
            length -= copySz;
            fragSz -= copySz;
            if (ssl->fragOffset == certSz + extSz)
                FreeDer(&ssl->buffers.certExts);
        }
        if (certChainSz > 0 && fragSz > 0) {
            /* Put in the CA certificates with empty extensions. */
            while (fragSz > 0) {
                word32 l;

                if (offset == len + OPAQUE16_LEN) {
                    /* Find next CA certificate to write out. */
                    offset = 0;
                    /* Point to the start of current cert in chain buffer. */
                    p = ssl->buffers.certChain->buffer + idx;
                    len = NextCert(ssl->buffers.certChain->buffer,
                                   ssl->buffers.certChain->length, &idx);
                    if (len == 0)
                        break;
                }

                /* Write out certificate and empty extension. */
                l = AddCertExt(ssl, p, len, OPAQUE16_LEN, offset, fragSz,
                                                                    output + i);
                i += l;
                ssl->fragOffset += l;
                length -= l;
                fragSz -= l;
                offset += l;
            }
        }

        if ((int)i - RECORD_HEADER_SZ < 0) {
            WOLFSSL_MSG("Send Cert bad inputSz");
            return BUFFER_E;
        }

#ifdef WOLFSSL_DTLS13
        if (ssl->options.dtls) {
            /* DTLS1.3 uses a separate variable and logic for fragments */
            ssl->options.buildingMsg = 0;
            ssl->fragOffset = 0;
            ret = Dtls13HandshakeSend(ssl, output, (word16)sendSz, (word16)i,
                                      certificate, 1);
        }
        else
#endif /* WOLFSSL_DTLS13 */
        {
            /* This message is always encrypted. */
            sendSz = BuildTls13Message(ssl, output, sendSz,
                output + RECORD_HEADER_SZ, i - RECORD_HEADER_SZ, handshake, 1,
                0, 0);
            if (sendSz < 0)
                return sendSz;

#if defined(WOLFSSL_CALLBACKS) || defined(OPENSSL_EXTRA)
            if (ssl->hsInfoOn)
                AddPacketName(ssl, "Certificate");
            if (ssl->toInfoOn) {
                ret = AddPacketInfo(ssl, "Certificate", handshake, output,
                              sendSz, WRITE_PROTO, 0, ssl->heap);
                if (ret != 0)
                    return ret;
            }
#endif

            ssl->buffers.outputBuffer.length += sendSz;
            ssl->options.buildingMsg = 0;
            if (!ssl->options.groupMessages)
                ret = SendBuffered(ssl);
        }
    }

    if (ret != WANT_WRITE) {
        /* Clean up the fragment offset. */
        ssl->options.buildingMsg = 0;
        ssl->fragOffset = 0;
        if (ssl->options.side == WOLFSSL_SERVER_END)
            ssl->options.serverState = SERVER_CERT_COMPLETE;
    }

#ifdef WOLFSSL_POST_HANDSHAKE_AUTH
    if (ssl->options.side == WOLFSSL_CLIENT_END && ssl->certReqCtx != NULL) {
        CertReqCtx* ctx = ssl->certReqCtx;
        ssl->certReqCtx = ssl->certReqCtx->next;
        XFREE(ctx, ssl->heap, DYNAMIC_TYPE_TMP_BUFFER);
    }
#endif

    WOLFSSL_LEAVE("SendTls13Certificate", ret);
    WOLFSSL_END(WC_FUNC_CERTIFICATE_SEND);

    return ret;
}

#if (!defined(NO_RSA) || defined(HAVE_ECC) || defined(HAVE_ED25519) || \
     defined(HAVE_ED448) || defined(HAVE_PQC)) && \
    (!defined(NO_WOLFSSL_SERVER) || !defined(WOLFSSL_NO_CLIENT_AUTH))
typedef struct Scv13Args {
    byte*  output; /* not allocated */
    byte*  verify; /* not allocated */
    word32 idx;
    word32 sigLen;
    int    sendSz;
    word16 length;

    byte   sigAlgo;
    byte*  sigData;
    word16 sigDataSz;
} Scv13Args;

static void FreeScv13Args(WOLFSSL* ssl, void* pArgs)
{
    Scv13Args* args = (Scv13Args*)pArgs;

    (void)ssl;

    if (args && args->sigData) {
        XFREE(args->sigData, ssl->heap, DYNAMIC_TYPE_SIGNATURE);
        args->sigData = NULL;
    }
}

/* handle generation TLS v1.3 certificate_verify (15) */
/* Send the TLS v1.3 CertificateVerify message.
 * A hash of all the message so far is used.
 * The signed data is:
 *     0x20 * 64 | context string | 0x00 | hash of messages
 * This message is always encrypted in TLS v1.3.
 *
 * ssl  The SSL/TLS object.
 * returns 0 on success, otherwise failure.
 */
static int SendTls13CertificateVerify(WOLFSSL* ssl)
{
    int ret = 0;
    buffer* sig = &ssl->buffers.sig;
#ifdef WOLFSSL_ASYNC_CRYPT
    Scv13Args* args = NULL;
    WOLFSSL_ASSERT_SIZEOF_GE(ssl->async->args, *args);
#else
    Scv13Args  args[1];
#endif

#ifdef WOLFSSL_DTLS13
    int recordLayerHdrExtra;
#endif /* WOLFSSL_DTLS13 */

    WOLFSSL_START(WC_FUNC_CERTIFICATE_VERIFY_SEND);
    WOLFSSL_ENTER("SendTls13CertificateVerify");

    ssl->options.buildingMsg = 1;

#if defined(WOLFSSL_RENESAS_TSIP_TLS)
    ret = tsip_Tls13SendCertVerify(ssl);
    if (ret != CRYPTOCB_UNAVAILABLE) {
        goto exit_scv;
    }
    ret = 0;
#endif /* WOLFSSL_RENESAS_TSIP_TLS */

#ifdef WOLFSSL_DTLS13
    /* can be negative */
    if (ssl->options.dtls)
        recordLayerHdrExtra = Dtls13GetRlHeaderLength(ssl, 1) - RECORD_HEADER_SZ;
    else
        recordLayerHdrExtra = 0;

#endif /* WOLFSSL_DTLS13 */

#ifdef WOLFSSL_ASYNC_CRYPT
    if (ssl->async == NULL) {
        ssl->async = (struct WOLFSSL_ASYNC*)
                XMALLOC(sizeof(struct WOLFSSL_ASYNC), ssl->heap,
                        DYNAMIC_TYPE_ASYNC);
        if (ssl->async == NULL)
            ERROR_OUT(MEMORY_E, exit_scv);
    }
    args = (Scv13Args*)ssl->async->args;

    ret = wolfSSL_AsyncPop(ssl, &ssl->options.asyncState);
    if (ret != WC_NOT_PENDING_E) {
        /* Check for error */
        if (ret < 0)
            goto exit_scv;
    }
    else
#endif
    {
        /* Reset state */
        ret = 0;
        ssl->options.asyncState = TLS_ASYNC_BEGIN;
        XMEMSET(args, 0, sizeof(Scv13Args));
    #ifdef WOLFSSL_ASYNC_CRYPT
        ssl->async->freeArgs = FreeScv13Args;
    #endif
    }

    switch(ssl->options.asyncState)
    {
        case TLS_ASYNC_BEGIN:
        {
            if (ssl->options.sendVerify == SEND_BLANK_CERT) {
                return 0;  /* sent blank cert, can't verify */
            }

            args->sendSz = MAX_CERT_VERIFY_SZ + MAX_MSG_EXTRA;
            /* Always encrypted.  */
            args->sendSz += MAX_MSG_EXTRA;

            /* check for available size */
            if ((ret = CheckAvailableSize(ssl, args->sendSz)) != 0) {
                goto exit_scv;
            }

            /* get output buffer */
            args->output = GetOutputBuffer(ssl);

            /* Advance state and proceed */
            ssl->options.asyncState = TLS_ASYNC_BUILD;
        } /* case TLS_ASYNC_BEGIN */
        FALL_THROUGH;

        case TLS_ASYNC_BUILD:
        {
            int rem = ssl->buffers.outputBuffer.bufferSize
              - ssl->buffers.outputBuffer.length
              - RECORD_HEADER_SZ - HANDSHAKE_HEADER_SZ;

            /* idx is used to track verify pointer offset to output */
            args->idx = RECORD_HEADER_SZ + HANDSHAKE_HEADER_SZ;
            args->verify =
                          &args->output[RECORD_HEADER_SZ + HANDSHAKE_HEADER_SZ];

#ifdef WOLFSSL_DTLS13
            if (ssl->options.dtls) {
                rem -= recordLayerHdrExtra + DTLS_HANDSHAKE_EXTRA;
                args->idx += recordLayerHdrExtra + DTLS_HANDSHAKE_EXTRA;
                args->verify += recordLayerHdrExtra + DTLS_HANDSHAKE_EXTRA;
            }
#endif /* WOLFSSL_DTLS13 */

            if (ssl->buffers.key == NULL) {
            #ifdef HAVE_PK_CALLBACKS
                if (wolfSSL_CTX_IsPrivatePkSet(ssl->ctx))
                    args->length = GetPrivateKeySigSize(ssl);
                else
            #endif
                    ERROR_OUT(NO_PRIVATE_KEY, exit_scv);
            }
            else {
                ret = DecodePrivateKey(ssl, &args->length);
                if (ret != 0)
                    goto exit_scv;
            }

            if (rem < 0 || args->length > rem) {
                ERROR_OUT(BUFFER_E, exit_scv);
            }

            if (args->length == 0) {
                ERROR_OUT(NO_PRIVATE_KEY, exit_scv);
            }

            /* Add signature algorithm. */
            if (ssl->hsType == DYNAMIC_TYPE_RSA)
                args->sigAlgo = rsa_pss_sa_algo;
        #ifdef HAVE_ECC
            else if (ssl->hsType == DYNAMIC_TYPE_ECC) {
        #if defined(WOLFSSL_SM2) && defined(WOLFSSL_SM3)
                if (ssl->buffers.keyType == sm2_sa_algo) {
                    args->sigAlgo = sm2_sa_algo;
                }
                else
        #endif
                {
                    args->sigAlgo = ecc_dsa_sa_algo;
                }
            }
        #endif
        #ifdef HAVE_ED25519
            else if (ssl->hsType == DYNAMIC_TYPE_ED25519)
                args->sigAlgo = ed25519_sa_algo;
        #endif
        #ifdef HAVE_ED448
            else if (ssl->hsType == DYNAMIC_TYPE_ED448)
                args->sigAlgo = ed448_sa_algo;
        #endif
        #if defined(HAVE_PQC)
            #if defined(HAVE_FALCON)
            else if (ssl->hsType == DYNAMIC_TYPE_FALCON) {
                falcon_key* fkey = (falcon_key*)ssl->hsKey;
                byte level = 0;
                if (wc_falcon_get_level(fkey, &level) != 0) {
                    ERROR_OUT(ALGO_ID_E, exit_scv);
                }
                if (level == 1) {
                    args->sigAlgo = falcon_level1_sa_algo;
                }
                else if (level == 5) {
                    args->sigAlgo = falcon_level5_sa_algo;
                }
                else {
                    ERROR_OUT(ALGO_ID_E, exit_scv);
                }
            }
            #endif /* HAVE_FALCON */
            #if defined(HAVE_DILITHIUM)
            else if (ssl->hsType == DYNAMIC_TYPE_DILITHIUM) {
                dilithium_key* fkey = (dilithium_key*)ssl->hsKey;
                byte level = 0;
                if (wc_dilithium_get_level(fkey, &level) != 0) {
                    ERROR_OUT(ALGO_ID_E, exit_scv);
                }
                if (level == 2) {
                    args->sigAlgo = dilithium_level2_sa_algo;
                }
                else if (level == 3) {
                    args->sigAlgo = dilithium_level3_sa_algo;
                }
                else if (level == 5) {
                    args->sigAlgo = dilithium_level5_sa_algo;
                }
                else {
                    ERROR_OUT(ALGO_ID_E, exit_scv);
                }
            }
            #endif /* HAVE_DILITHIUM */
        #endif /* HAVE_PQC */
            else {
                ERROR_OUT(ALGO_ID_E, exit_scv);
            }
            EncodeSigAlg(ssl->options.hashAlgo, args->sigAlgo, args->verify);

            if (ssl->hsType == DYNAMIC_TYPE_RSA) {
                int sigLen = MAX_SIG_DATA_SZ;
                if (args->length > MAX_SIG_DATA_SZ)
                    sigLen = args->length;
                args->sigData = (byte*)XMALLOC(sigLen, ssl->heap,
                                                        DYNAMIC_TYPE_SIGNATURE);
            }
            else {
                args->sigData = (byte*)XMALLOC(MAX_SIG_DATA_SZ, ssl->heap,
                                                        DYNAMIC_TYPE_SIGNATURE);
            }
            if (args->sigData == NULL) {
                ERROR_OUT(MEMORY_E, exit_scv);
            }

            /* Create the data to be signed. */
            ret = CreateSigData(ssl, args->sigData, &args->sigDataSz, 0);
            if (ret != 0)
                goto exit_scv;

        #ifndef NO_RSA
            if (ssl->hsType == DYNAMIC_TYPE_RSA) {
                /* build encoded signature buffer */
                sig->length = WC_MAX_DIGEST_SIZE;
                sig->buffer = (byte*)XMALLOC(sig->length, ssl->heap,
                                                        DYNAMIC_TYPE_SIGNATURE);
                if (sig->buffer == NULL) {
                    ERROR_OUT(MEMORY_E, exit_scv);
                }

                ret = CreateRSAEncodedSig(sig->buffer, args->sigData,
                    args->sigDataSz, args->sigAlgo, ssl->options.hashAlgo);
                if (ret < 0)
                    goto exit_scv;
                sig->length = ret;
                ret = 0;

                /* Maximum size of RSA Signature. */
                args->sigLen = args->length;
            }
        #endif /* !NO_RSA */
        #ifdef HAVE_ECC
            if (ssl->hsType == DYNAMIC_TYPE_ECC) {
                sig->length = args->sendSz - args->idx - HASH_SIG_SIZE -
                              VERIFY_HEADER;
            #if defined(WOLFSSL_SM2) && defined(WOLFSSL_SM3)
                if (ssl->buffers.keyType != sm2_sa_algo)
            #endif
                {
                    ret = CreateECCEncodedSig(args->sigData,
                        args->sigDataSz, ssl->options.hashAlgo);
                    if (ret < 0)
                        goto exit_scv;
                    args->sigDataSz = (word16)ret;
                    ret = 0;
                }
            }
        #endif /* HAVE_ECC */
        #ifdef HAVE_ED25519
            if (ssl->hsType == DYNAMIC_TYPE_ED25519) {
                ret = Ed25519CheckPubKey(ssl);
                if (ret < 0) {
                    ERROR_OUT(ret, exit_scv);
                }
                sig->length = ED25519_SIG_SIZE;
            }
        #endif /* HAVE_ED25519 */
        #ifdef HAVE_ED448
            if (ssl->hsType == DYNAMIC_TYPE_ED448) {
                ret = Ed448CheckPubKey(ssl);
                if (ret < 0) {
                    ERROR_OUT(ret, exit_scv);
                }
                sig->length = ED448_SIG_SIZE;
            }
        #endif /* HAVE_ED448 */
        #if defined(HAVE_PQC)
            #if defined(HAVE_FALCON)
            if (ssl->hsType == DYNAMIC_TYPE_FALCON) {
                sig->length = FALCON_MAX_SIG_SIZE;
            }
            #endif
            #if defined(HAVE_DILITHIUM)
            if (ssl->hsType == DYNAMIC_TYPE_DILITHIUM) {
                sig->length = DILITHIUM_MAX_SIG_SIZE;
            }
            #endif
        #endif /* HAVE_PQC */

            /* Advance state and proceed */
            ssl->options.asyncState = TLS_ASYNC_DO;
        } /* case TLS_ASYNC_BUILD */
        FALL_THROUGH;

        case TLS_ASYNC_DO:
        {
        #ifdef HAVE_ECC
            if (ssl->hsType == DYNAMIC_TYPE_ECC) {
            #if defined(WOLFSSL_SM2) && defined(WOLFSSL_SM3)
                if (ssl->buffers.keyType == sm2_sa_algo) {
                    ret = Sm2wSm3Sign(ssl, TLS13_SM2_SIG_ID,
                        TLS13_SM2_SIG_ID_SZ, args->sigData, args->sigDataSz,
                        args->verify + HASH_SIG_SIZE + VERIFY_HEADER,
                        (word32*)&sig->length, (ecc_key*)ssl->hsKey, NULL);
                }
                else
            #endif
                {
                    ret = EccSign(ssl, args->sigData, args->sigDataSz,
                        args->verify + HASH_SIG_SIZE + VERIFY_HEADER,
                        (word32*)&sig->length, (ecc_key*)ssl->hsKey,
                #ifdef HAVE_PK_CALLBACKS
                        ssl->buffers.key
                #else
                        NULL
                #endif
                    );
                }
                args->length = (word16)sig->length;
            }
        #endif /* HAVE_ECC */
        #ifdef HAVE_ED25519
            if (ssl->hsType == DYNAMIC_TYPE_ED25519) {
                ret = Ed25519Sign(ssl, args->sigData, args->sigDataSz,
                    args->verify + HASH_SIG_SIZE + VERIFY_HEADER,
                    (word32*)&sig->length, (ed25519_key*)ssl->hsKey,
            #ifdef HAVE_PK_CALLBACKS
                    ssl->buffers.key
            #else
                    NULL
            #endif
                );
                args->length = (word16)sig->length;
            }
        #endif
        #ifdef HAVE_ED448
            if (ssl->hsType == DYNAMIC_TYPE_ED448) {
                ret = Ed448Sign(ssl, args->sigData, args->sigDataSz,
                    args->verify + HASH_SIG_SIZE + VERIFY_HEADER,
                    (word32*)&sig->length, (ed448_key*)ssl->hsKey,
            #ifdef HAVE_PK_CALLBACKS
                    ssl->buffers.key
            #else
                    NULL
            #endif
                );
                args->length = (word16)sig->length;
            }
        #endif
        #if defined(HAVE_PQC)
            #if defined(HAVE_FALCON)
            if (ssl->hsType == DYNAMIC_TYPE_FALCON) {
                ret = wc_falcon_sign_msg(args->sigData, args->sigDataSz,
                                         args->verify + HASH_SIG_SIZE +
                                         VERIFY_HEADER, (word32*)&sig->length,
                                         (falcon_key*)ssl->hsKey);
                args->length = (word16)sig->length;
            }
            #endif
            #if defined(HAVE_DILITHIUM)
            if (ssl->hsType == DYNAMIC_TYPE_DILITHIUM) {
                ret = wc_dilithium_sign_msg(args->sigData, args->sigDataSz,
                                         args->verify + HASH_SIG_SIZE +
                                         VERIFY_HEADER, (word32*)&sig->length,
                                         (dilithium_key*)ssl->hsKey);
                args->length = (word16)sig->length;
            }
            #endif
        #endif /* HAVE_PQC */
        #ifndef NO_RSA
            if (ssl->hsType == DYNAMIC_TYPE_RSA) {
                ret = RsaSign(ssl, sig->buffer, (word32)sig->length,
                    args->verify + HASH_SIG_SIZE + VERIFY_HEADER, &args->sigLen,
                    args->sigAlgo, ssl->options.hashAlgo,
                    (RsaKey*)ssl->hsKey,
                    ssl->buffers.key
                );
                if (ret == 0) {
                    args->length = (word16)args->sigLen;

                    XMEMCPY(args->sigData,
                        args->verify + HASH_SIG_SIZE + VERIFY_HEADER,
                        args->sigLen);
                }
            }
        #endif /* !NO_RSA */

            /* Check for error */
            if (ret != 0) {
                goto exit_scv;
            }

            /* Add signature length. */
            c16toa(args->length, args->verify + HASH_SIG_SIZE);

            /* Advance state and proceed */
            ssl->options.asyncState = TLS_ASYNC_VERIFY;
        } /* case TLS_ASYNC_DO */
        FALL_THROUGH;

        case TLS_ASYNC_VERIFY:
        {
        #ifndef NO_RSA
            if (ssl->hsType == DYNAMIC_TYPE_RSA) {
                /* check for signature faults */
                ret = VerifyRsaSign(ssl, args->sigData, args->sigLen,
                    sig->buffer, (word32)sig->length, args->sigAlgo,
                    ssl->options.hashAlgo, (RsaKey*)ssl->hsKey,
                    ssl->buffers.key
                );
            }
        #endif /* !NO_RSA */
        #if defined(HAVE_ECC) && defined(WOLFSSL_CHECK_SIG_FAULTS)
            if (ssl->hsType == DYNAMIC_TYPE_ECC) {
            #if defined(WOLFSSL_SM2) && defined(WOLFSSL_SM3)
                if (ssl->buffers.keyType == sm2_sa_algo) {
                    ret = Sm2wSm3Verify(ssl, TLS13_SM2_SIG_ID,
                        TLS13_SM2_SIG_ID_SZ,
                        args->verify + HASH_SIG_SIZE + VERIFY_HEADER,
                        sig->length, args->sigData, args->sigDataSz,
                        (ecc_key*)ssl->hsKey, NULL);
                }
                else
            #endif
                {
                    ret = EccVerify(ssl,
                        args->verify + HASH_SIG_SIZE + VERIFY_HEADER,
                        sig->length, args->sigData, args->sigDataSz,
                        (ecc_key*)ssl->hsKey,
                #ifdef HAVE_PK_CALLBACKS
                        ssl->buffers.key
                #else
                        NULL
                #endif
                    );
                }
            }
        #endif

            /* Check for error */
            if (ret != 0) {
                goto exit_scv;
            }

            /* Advance state and proceed */
            ssl->options.asyncState = TLS_ASYNC_FINALIZE;
        } /* case TLS_ASYNC_VERIFY */
        FALL_THROUGH;

        case TLS_ASYNC_FINALIZE:
        {
            /* Put the record and handshake headers on. */
            AddTls13Headers(args->output, args->length + HASH_SIG_SIZE +
                            VERIFY_HEADER, certificate_verify, ssl);

            args->sendSz = RECORD_HEADER_SZ + HANDSHAKE_HEADER_SZ +
                                   args->length + HASH_SIG_SIZE + VERIFY_HEADER;
#ifdef WOLFSSL_DTLS13
            if (ssl->options.dtls)
                args->sendSz += recordLayerHdrExtra + DTLS_HANDSHAKE_EXTRA;

#endif /* WOLFSSL_DTLS13 */
            /* Advance state and proceed */
            ssl->options.asyncState = TLS_ASYNC_END;
        } /* case TLS_ASYNC_FINALIZE */
        FALL_THROUGH;

        case TLS_ASYNC_END:
        {
#ifdef WOLFSSL_DTLS13
            if (ssl->options.dtls) {
                ssl->options.buildingMsg = 0;
                ret = Dtls13HandshakeSend(ssl, args->output,
                    MAX_CERT_VERIFY_SZ + MAX_MSG_EXTRA + MAX_MSG_EXTRA,
                    (word16)args->sendSz, certificate_verify, 1);
                if (ret != 0)
                    goto exit_scv;

                break;
            }
#endif /* WOLFSSL_DTLS13 */

            /* This message is always encrypted. */
            ret = BuildTls13Message(ssl, args->output,
                                    MAX_CERT_VERIFY_SZ + MAX_MSG_EXTRA,
                                    args->output + RECORD_HEADER_SZ,
                                    args->sendSz - RECORD_HEADER_SZ, handshake,
                                    1, 0, 0);

            if (ret < 0) {
                goto exit_scv;
            }
            else {
                args->sendSz = ret;
                ret = 0;
            }

        #if defined(WOLFSSL_CALLBACKS) || defined(OPENSSL_EXTRA)
            if (ssl->hsInfoOn)
                AddPacketName(ssl, "CertificateVerify");
            if (ssl->toInfoOn) {
                ret = AddPacketInfo(ssl, "CertificateVerify", handshake,
                            args->output, args->sendSz, WRITE_PROTO, 0,
                            ssl->heap);
                if (ret != 0)
                    goto exit_scv;
            }
        #endif

            ssl->buffers.outputBuffer.length += args->sendSz;
            ssl->options.buildingMsg = 0;
            if (!ssl->options.groupMessages)
                ret = SendBuffered(ssl);
            break;
        }
        default:
            ret = INPUT_CASE_ERROR;
    } /* switch(ssl->options.asyncState) */

exit_scv:

    WOLFSSL_LEAVE("SendTls13CertificateVerify", ret);
    WOLFSSL_END(WC_FUNC_CERTIFICATE_VERIFY_SEND);

#ifdef WOLFSSL_ASYNC_CRYPT
    /* Handle async operation */
    if (ret == WC_PENDING_E) {
        return ret;
    }
#endif /* WOLFSSL_ASYNC_CRYPT */

    /* Final cleanup */
    FreeScv13Args(ssl, args);
    FreeKeyExchange(ssl);
#ifdef WOLFSSL_ASYNC_IO
    /* Cleanup async */
    FreeAsyncCtx(ssl, 0);
#endif

    if (ret != 0) {
        WOLFSSL_ERROR_VERBOSE(ret);
    }

    return ret;
}
#endif
#endif /* !NO_WOLFSSL_CLIENT || !NO_WOLFSSL_SERVER */

#if !defined(NO_WOLFSSL_CLIENT) || !defined(WOLFSSL_NO_CLIENT_AUTH)
/* handle processing TLS v1.3 certificate (11) */
/* Parse and handle a TLS v1.3 Certificate message.
 *
 * ssl       The SSL/TLS object.
 * input     The message buffer.
 * inOutIdx  On entry, the index into the message buffer of Certificate.
 *           On exit, the index of byte after the Certificate message.
 * totalSz   The length of the current handshake message.
 * returns 0 on success and otherwise failure.
 */
static int DoTls13Certificate(WOLFSSL* ssl, byte* input, word32* inOutIdx,
                              word32 totalSz)
{
    int ret = 0;

    WOLFSSL_START(WC_FUNC_CERTIFICATE_DO);
    WOLFSSL_ENTER("DoTls13Certificate");

#ifdef WOLFSSL_DTLS13
    if (ssl->options.dtls && ssl->options.handShakeDone) {
        /* certificate needs some special care after the handshake */
        ret = Dtls13RtxProcessingCertificate(
            ssl, input + *inOutIdx, totalSz);
    }
#endif /* WOLFSSL_DTLS13 */

    if (ret == 0)
        ret = ProcessPeerCerts(ssl, input, inOutIdx, totalSz);
    if (ret == 0) {
#if !defined(NO_WOLFSSL_CLIENT)
        if (ssl->options.side == WOLFSSL_CLIENT_END)
            ssl->options.serverState = SERVER_CERT_COMPLETE;
#endif
#if !defined(NO_WOLFSSL_SERVER) && defined(WOLFSSL_POST_HANDSHAKE_AUTH)
        if (ssl->options.side == WOLFSSL_SERVER_END &&
                                ssl->options.handShakeState == HANDSHAKE_DONE) {
            /* reset handshake states */
            ssl->options.serverState = SERVER_FINISHED_COMPLETE;
            ssl->options.acceptState  = TICKET_SENT;
            ssl->options.handShakeState = SERVER_FINISHED_COMPLETE;
        }
#endif
    }

    WOLFSSL_LEAVE("DoTls13Certificate", ret);
    WOLFSSL_END(WC_FUNC_CERTIFICATE_DO);

    return ret;
}
#endif

#if !defined(NO_RSA) || defined(HAVE_ECC) || defined(HAVE_ED25519) || \
                                                             defined(HAVE_ED448)

typedef struct Dcv13Args {
    byte*  output; /* not allocated */
    word32 sendSz;
    word16 sz;
    word32 sigSz;
    word32 idx;
    word32 begin;
    byte   hashAlgo;
    byte   sigAlgo;

    byte*  sigData;
    word16 sigDataSz;
} Dcv13Args;

static void FreeDcv13Args(WOLFSSL* ssl, void* pArgs)
{
    Dcv13Args* args = (Dcv13Args*)pArgs;

    if (args && args->sigData != NULL) {
        XFREE(args->sigData, ssl->heap, DYNAMIC_TYPE_SIGNATURE);
        args->sigData = NULL;
    }

    (void)ssl;
}

/* handle processing TLS v1.3 certificate_verify (15) */
/* Parse and handle a TLS v1.3 CertificateVerify message.
 *
 * ssl       The SSL/TLS object.
 * input     The message buffer.
 * inOutIdx  On entry, the index into the message buffer of
 *           CertificateVerify.
 *           On exit, the index of byte after the CertificateVerify message.
 * totalSz   The length of the current handshake message.
 * returns 0 on success and otherwise failure.
 */
static int DoTls13CertificateVerify(WOLFSSL* ssl, byte* input,
                                    word32* inOutIdx, word32 totalSz)
{
    int         ret = 0;
    buffer*     sig = &ssl->buffers.sig;
#ifdef WOLFSSL_ASYNC_CRYPT
    Dcv13Args* args = NULL;
    WOLFSSL_ASSERT_SIZEOF_GE(ssl->async->args, *args);
#else
    Dcv13Args  args[1];
#endif

    WOLFSSL_START(WC_FUNC_CERTIFICATE_VERIFY_DO);
    WOLFSSL_ENTER("DoTls13CertificateVerify");

#if defined(WOLFSSL_RENESAS_TSIP_TLS)
    ret = tsip_Tls13CertificateVerify(ssl, input, inOutIdx, totalSz);
    if (ret != CRYPTOCB_UNAVAILABLE) {
        goto exit_dcv;
    }
    ret = 0;
#endif

#ifdef WOLFSSL_ASYNC_CRYPT
    if (ssl->async == NULL) {
        ssl->async = (struct WOLFSSL_ASYNC*)
                XMALLOC(sizeof(struct WOLFSSL_ASYNC), ssl->heap,
                        DYNAMIC_TYPE_ASYNC);
        if (ssl->async == NULL)
            ERROR_OUT(MEMORY_E, exit_dcv);
    }
    args = (Dcv13Args*)ssl->async->args;

    ret = wolfSSL_AsyncPop(ssl, &ssl->options.asyncState);
    if (ret != WC_NOT_PENDING_E) {
        /* Check for error */
        if (ret < 0)
            goto exit_dcv;
    }
    else
#endif
    {
        /* Reset state */
        ret = 0;
        ssl->options.asyncState = TLS_ASYNC_BEGIN;
        XMEMSET(args, 0, sizeof(Dcv13Args));
        args->hashAlgo = sha_mac;
        args->sigAlgo = anonymous_sa_algo;
        args->idx = *inOutIdx;
        args->begin = *inOutIdx;
    #ifdef WOLFSSL_ASYNC_CRYPT
        ssl->async->freeArgs = FreeDcv13Args;
    #endif
    }

    switch(ssl->options.asyncState)
    {
        case TLS_ASYNC_BEGIN:
        {
        #ifdef WOLFSSL_CALLBACKS
            if (ssl->hsInfoOn) AddPacketName(ssl, "CertificateVerify");
            if (ssl->toInfoOn) AddLateName("CertificateVerify",
                                           &ssl->timeoutInfo);
        #endif

            /* Advance state and proceed */
            ssl->options.asyncState = TLS_ASYNC_BUILD;
        } /* case TLS_ASYNC_BEGIN */
        FALL_THROUGH;

        case TLS_ASYNC_BUILD:
        {
            int validSigAlgo;

            /* Signature algorithm. */
            if ((args->idx - args->begin) + ENUM_LEN + ENUM_LEN > totalSz) {
                ERROR_OUT(BUFFER_ERROR, exit_dcv);
            }
            ret = DecodeTls13SigAlg(input + args->idx, &args->hashAlgo,
                                                                &args->sigAlgo);
            if (ret < 0)
                goto exit_dcv;
            args->idx += OPAQUE16_LEN;

            /* Signature length. */
            if ((args->idx - args->begin) + OPAQUE16_LEN > totalSz) {
                ERROR_OUT(BUFFER_ERROR, exit_dcv);
            }
            ato16(input + args->idx, &args->sz);
            args->idx += OPAQUE16_LEN;

            /* Signature data. */
            if ((args->idx - args->begin) + args->sz > totalSz ||
                                                       args->sz > ENCRYPT_LEN) {
                ERROR_OUT(BUFFER_ERROR, exit_dcv);
            }

            /* Check for public key of required type. */
            /* Assume invalid unless signature algo matches the key provided */
            validSigAlgo = 0;
        #ifdef HAVE_ED25519
            if (args->sigAlgo == ed25519_sa_algo) {
                WOLFSSL_MSG("Peer sent ED25519 sig");
                validSigAlgo = (ssl->peerEd25519Key != NULL) &&
                                                     ssl->peerEd25519KeyPresent;
            }
        #endif
        #ifdef HAVE_ED448
            if (args->sigAlgo == ed448_sa_algo) {
                WOLFSSL_MSG("Peer sent ED448 sig");
                validSigAlgo = (ssl->peerEd448Key != NULL) &&
                                                       ssl->peerEd448KeyPresent;
            }
        #endif
        #ifdef HAVE_ECC
            if (args->sigAlgo == ecc_dsa_sa_algo) {
                WOLFSSL_MSG("Peer sent ECC sig");
                validSigAlgo = (ssl->peerEccDsaKey != NULL) &&
                                                      ssl->peerEccDsaKeyPresent;
            }
        #endif
        #if defined(WOLFSSL_SM2) && defined(WOLFSSL_SM3)
            if (args->sigAlgo == sm2_sa_algo) {
                WOLFSSL_MSG("Peer sent SM2 sig");
                validSigAlgo = (ssl->peerEccDsaKey != NULL) &&
                                                      ssl->peerEccDsaKeyPresent;
            }
        #endif
        #ifdef HAVE_PQC
            if (args->sigAlgo == falcon_level1_sa_algo) {
                WOLFSSL_MSG("Peer sent Falcon Level 1 sig");
                validSigAlgo = (ssl->peerFalconKey != NULL) &&
                               ssl->peerFalconKeyPresent;
            }
            if (args->sigAlgo == falcon_level5_sa_algo) {
                WOLFSSL_MSG("Peer sent Falcon Level 5 sig");
                validSigAlgo = (ssl->peerFalconKey != NULL) &&
                               ssl->peerFalconKeyPresent;
            }
            if (args->sigAlgo == dilithium_level2_sa_algo) {
                WOLFSSL_MSG("Peer sent Dilithium Level 2 sig");
                validSigAlgo = (ssl->peerDilithiumKey != NULL) &&
                               ssl->peerDilithiumKeyPresent;
            }
            if (args->sigAlgo == dilithium_level3_sa_algo) {
                WOLFSSL_MSG("Peer sent Dilithium Level 3 sig");
                validSigAlgo = (ssl->peerDilithiumKey != NULL) &&
                               ssl->peerDilithiumKeyPresent;
            }
            if (args->sigAlgo == dilithium_level5_sa_algo) {
                WOLFSSL_MSG("Peer sent Dilithium Level 5 sig");
                validSigAlgo = (ssl->peerDilithiumKey != NULL) &&
                               ssl->peerDilithiumKeyPresent;
            }
        #endif
        #ifndef NO_RSA
            if (args->sigAlgo == rsa_sa_algo) {
                WOLFSSL_MSG("Peer sent PKCS#1.5 algo - not valid TLS 1.3");
                ERROR_OUT(INVALID_PARAMETER, exit_dcv);
            }
            if (args->sigAlgo == rsa_pss_sa_algo) {
                WOLFSSL_MSG("Peer sent RSA sig");
                validSigAlgo = (ssl->peerRsaKey != NULL) &&
                                                         ssl->peerRsaKeyPresent;
            }
        #endif
            if (!validSigAlgo) {
                WOLFSSL_MSG("Sig algo doesn't correspond to certificate");
                ERROR_OUT(SIG_VERIFY_E, exit_dcv);
            }

            sig->buffer = (byte*)XMALLOC(args->sz, ssl->heap,
                                         DYNAMIC_TYPE_SIGNATURE);
            if (sig->buffer == NULL) {
                ERROR_OUT(MEMORY_E, exit_dcv);
            }
            sig->length = args->sz;
            XMEMCPY(sig->buffer, input + args->idx, args->sz);

        #ifdef HAVE_ECC
            if (ssl->peerEccDsaKeyPresent) {
                WOLFSSL_MSG("Doing ECC peer cert verify");

                args->sigData = (byte*)XMALLOC(MAX_SIG_DATA_SZ, ssl->heap,
                                                        DYNAMIC_TYPE_SIGNATURE);
                if (args->sigData == NULL) {
                    ERROR_OUT(MEMORY_E, exit_dcv);
                }

                ret = CreateSigData(ssl, args->sigData, &args->sigDataSz, 1);
                if (ret != 0)
                    goto exit_dcv;
            #if defined(WOLFSSL_SM2) && defined(WOLFSSL_SM3)
                if (args->sigAlgo != sm2_sa_algo)
            #endif
                {
                    ret = CreateECCEncodedSig(args->sigData,
                        args->sigDataSz, args->hashAlgo);
                    if (ret < 0)
                        goto exit_dcv;
                    args->sigDataSz = (word16)ret;
                    ret = 0;
                }
            }
        #endif
        #ifdef HAVE_ED25519
            if (ssl->peerEd25519KeyPresent) {
                WOLFSSL_MSG("Doing ED25519 peer cert verify");

                args->sigData = (byte*)XMALLOC(MAX_SIG_DATA_SZ, ssl->heap,
                                                        DYNAMIC_TYPE_SIGNATURE);
                if (args->sigData == NULL) {
                    ERROR_OUT(MEMORY_E, exit_dcv);
                }

                CreateSigData(ssl, args->sigData, &args->sigDataSz, 1);
                ret = 0;
            }
        #endif
        #ifdef HAVE_ED448
            if (ssl->peerEd448KeyPresent) {
                WOLFSSL_MSG("Doing ED448 peer cert verify");

                args->sigData = (byte*)XMALLOC(MAX_SIG_DATA_SZ, ssl->heap,
                                                        DYNAMIC_TYPE_SIGNATURE);
                if (args->sigData == NULL) {
                    ERROR_OUT(MEMORY_E, exit_dcv);
                }

                CreateSigData(ssl, args->sigData, &args->sigDataSz, 1);
                ret = 0;
            }
       #endif
       #ifdef HAVE_PQC
            if (ssl->peerFalconKeyPresent) {
                WOLFSSL_MSG("Doing Falcon peer cert verify");

                args->sigData = (byte*)XMALLOC(MAX_SIG_DATA_SZ, ssl->heap,
                                                        DYNAMIC_TYPE_SIGNATURE);
                if (args->sigData == NULL) {
                    ERROR_OUT(MEMORY_E, exit_dcv);
                }

                CreateSigData(ssl, args->sigData, &args->sigDataSz, 1);
                ret = 0;
            }

            if (ssl->peerDilithiumKeyPresent) {
                WOLFSSL_MSG("Doing Dilithium peer cert verify");

                args->sigData = (byte*)XMALLOC(MAX_SIG_DATA_SZ, ssl->heap,
                                                        DYNAMIC_TYPE_SIGNATURE);
                if (args->sigData == NULL) {
                    ERROR_OUT(MEMORY_E, exit_dcv);
                }

                CreateSigData(ssl, args->sigData, &args->sigDataSz, 1);
                ret = 0;
            }
       #endif

            /* Advance state and proceed */
            ssl->options.asyncState = TLS_ASYNC_DO;
        } /* case TLS_ASYNC_BUILD */
        FALL_THROUGH;

        case TLS_ASYNC_DO:
        {
        #ifndef NO_RSA
            if (ssl->peerRsaKey != NULL && ssl->peerRsaKeyPresent != 0) {
                ret = RsaVerify(ssl, sig->buffer, (word32)sig->length, &args->output,
                    args->sigAlgo, args->hashAlgo, ssl->peerRsaKey,
                #ifdef HAVE_PK_CALLBACKS
                    &ssl->buffers.peerRsaKey
                #else
                    NULL
                #endif
                );
                if (ret >= 0) {
                    args->sendSz = ret;
                    ret = 0;
                }
            }
        #endif /* !NO_RSA */
        #ifdef HAVE_ECC
            if (ssl->peerEccDsaKeyPresent) {
            #if defined(WOLFSSL_SM2) && defined(WOLFSSL_SM3)
                if (args->sigAlgo == sm2_sa_algo) {
                    ret = Sm2wSm3Verify(ssl, TLS13_SM2_SIG_ID,
                        TLS13_SM2_SIG_ID_SZ, input + args->idx, args->sz,
                        args->sigData, args->sigDataSz,
                        ssl->peerEccDsaKey, NULL);
                }
                else
            #endif
                {
                    ret = EccVerify(ssl, input + args->idx, args->sz,
                        args->sigData, args->sigDataSz,
                        ssl->peerEccDsaKey,
                    #ifdef HAVE_PK_CALLBACKS
                        &ssl->buffers.peerEccDsaKey
                    #else
                        NULL
                    #endif
                    );
                }

                if (ret >= 0) {
                    /* CLIENT/SERVER: data verified with public key from
                     * certificate. */
                    ssl->options.peerAuthGood = 1;
                    FreeKey(ssl, DYNAMIC_TYPE_ECC, (void**)&ssl->peerEccDsaKey);
                    ssl->peerEccDsaKeyPresent = 0;
                }
            }
        #endif /* HAVE_ECC */
        #ifdef HAVE_ED25519
            if (ssl->peerEd25519KeyPresent) {
                ret = Ed25519Verify(ssl, input + args->idx, args->sz,
                    args->sigData, args->sigDataSz,
                    ssl->peerEd25519Key,
                #ifdef HAVE_PK_CALLBACKS
                    &ssl->buffers.peerEd25519Key
                #else
                    NULL
                #endif
                );

                if (ret >= 0) {
                    /* CLIENT/SERVER: data verified with public key from
                     * certificate. */
                    ssl->options.peerAuthGood = 1;
                    FreeKey(ssl, DYNAMIC_TYPE_ED25519,
                                                  (void**)&ssl->peerEd25519Key);
                    ssl->peerEd25519KeyPresent = 0;
                }
            }
        #endif
        #ifdef HAVE_ED448
            if (ssl->peerEd448KeyPresent) {
                ret = Ed448Verify(ssl, input + args->idx, args->sz,
                    args->sigData, args->sigDataSz,
                    ssl->peerEd448Key,
                #ifdef HAVE_PK_CALLBACKS
                    &ssl->buffers.peerEd448Key
                #else
                    NULL
                #endif
                );

                if (ret >= 0) {
                    /* CLIENT/SERVER: data verified with public key from
                     * certificate. */
                    ssl->options.peerAuthGood = 1;
                    FreeKey(ssl, DYNAMIC_TYPE_ED448,
                                                    (void**)&ssl->peerEd448Key);
                    ssl->peerEd448KeyPresent = 0;
                }
            }
        #endif
        #if defined(HAVE_PQC) && defined(HAVE_FALCON)
            if (ssl->peerFalconKeyPresent) {
                int res = 0;
                WOLFSSL_MSG("Doing Falcon peer cert verify");
                ret = wc_falcon_verify_msg(input + args->idx, args->sz,
                                    args->sigData, args->sigDataSz,
                                    &res, ssl->peerFalconKey);

                if ((ret >= 0) && (res == 1)) {
                    /* CLIENT/SERVER: data verified with public key from
                     * certificate. */
                    ssl->options.peerAuthGood = 1;
                    FreeKey(ssl, DYNAMIC_TYPE_FALCON,
                                                   (void**)&ssl->peerFalconKey);
                    ssl->peerFalconKeyPresent = 0;
                }
            }
        #endif /* HAVE_PQC && HAVE_FALCON */
        #if defined(HAVE_PQC) && defined(HAVE_DILITHIUM)
            if (ssl->peerDilithiumKeyPresent) {
                int res = 0;
                WOLFSSL_MSG("Doing Dilithium peer cert verify");
                ret = wc_dilithium_verify_msg(input + args->idx, args->sz,
                                              args->sigData, args->sigDataSz,
                                              &res, ssl->peerDilithiumKey);

                if ((ret >= 0) && (res == 1)) {
                    /* CLIENT/SERVER: data verified with public key from
                     * certificate. */
                    ssl->options.peerAuthGood = 1;
                    FreeKey(ssl, DYNAMIC_TYPE_DILITHIUM,
                            (void**)&ssl->peerDilithiumKey);
                    ssl->peerDilithiumKeyPresent = 0;
                }
            }
        #endif /* HAVE_PQC && HAVE_DILITHIUM */

            /* Check for error */
            if (ret != 0) {
                goto exit_dcv;
            }

            /* Advance state and proceed */
            ssl->options.asyncState = TLS_ASYNC_VERIFY;
        } /* case TLS_ASYNC_DO */
        FALL_THROUGH;

        case TLS_ASYNC_VERIFY:
        {
        #if !defined(NO_RSA) && defined(WC_RSA_PSS)
            if (ssl->peerRsaKey != NULL && ssl->peerRsaKeyPresent != 0) {
                ret = CheckRSASignature(ssl, args->sigAlgo, args->hashAlgo,
                                        args->output, args->sendSz);
                if (ret != 0)
                    goto exit_dcv;

                /* CLIENT/SERVER: data verified with public key from
                 * certificate. */
                ssl->peerRsaKeyPresent = 0;
                FreeKey(ssl, DYNAMIC_TYPE_RSA, (void**)&ssl->peerRsaKey);
                ssl->options.peerAuthGood = 1;
            }
        #endif /* !NO_RSA && WC_RSA_PSS */

            /* Advance state and proceed */
            ssl->options.asyncState = TLS_ASYNC_FINALIZE;
        } /* case TLS_ASYNC_VERIFY */
        FALL_THROUGH;

        case TLS_ASYNC_FINALIZE:
        {
            ssl->options.havePeerVerify = 1;

            /* Set final index */
            args->idx += args->sz;
            *inOutIdx = args->idx;

            /* Encryption is always on: add padding */
            *inOutIdx += ssl->keys.padSz;

            /* Advance state and proceed */
            ssl->options.asyncState = TLS_ASYNC_END;

        #if !defined(NO_WOLFSSL_CLIENT)
            if (ssl->options.side == WOLFSSL_CLIENT_END)
                ssl->options.serverState = SERVER_CERT_VERIFY_COMPLETE;
        #endif
        } /* case TLS_ASYNC_FINALIZE */
        FALL_THROUGH;

        case TLS_ASYNC_END:
        {
            break;
        }

        default:
            ret = INPUT_CASE_ERROR;
    } /* switch(ssl->options.asyncState) */

exit_dcv:

    WOLFSSL_LEAVE("DoTls13CertificateVerify", ret);
    WOLFSSL_END(WC_FUNC_CERTIFICATE_VERIFY_DO);

#ifdef WOLFSSL_ASYNC_CRYPT
    /* Handle async operation */
    if (ret == WC_PENDING_E) {
        /* Mark message as not received so it can process again */
        ssl->msgsReceived.got_certificate_verify = 0;

        return ret;
    }
    else
#endif /* WOLFSSL_ASYNC_CRYPT */
    if (ret != 0) {
        WOLFSSL_ERROR_VERBOSE(ret);

        if (ret != INVALID_PARAMETER) {
            SendAlert(ssl, alert_fatal, decrypt_error);
        }
    }

    /* Final cleanup */
    FreeDcv13Args(ssl, args);
    FreeKeyExchange(ssl);
#ifdef WOLFSSL_ASYNC_IO
    /* Cleanup async */
    FreeAsyncCtx(ssl, 0);
#endif

    return ret;
}
#endif /* !NO_RSA || HAVE_ECC */
#endif /* !NO_CERTS */

/* Parse and handle a TLS v1.3 Finished message.
 *
 * ssl       The SSL/TLS object.
 * input     The message buffer.
 * inOutIdx  On entry, the index into the message buffer of Finished.
 *           On exit, the index of byte after the Finished message and padding.
 * size      Length of message data.
 * totalSz   Length of remaining data in the message buffer.
 * sniff     Indicates whether we are sniffing packets.
 * returns 0 on success and otherwise failure.
 */
int DoTls13Finished(WOLFSSL* ssl, const byte* input, word32* inOutIdx,
                           word32 size, word32 totalSz, int sniff)
{
    int    ret;
    word32 finishedSz = 0;
    byte*  secret;
    byte   mac[WC_MAX_DIGEST_SIZE];

    WOLFSSL_START(WC_FUNC_FINISHED_DO);
    WOLFSSL_ENTER("DoTls13Finished");

#if !defined(NO_CERTS) && !defined(WOLFSSL_NO_CLIENT_AUTH)
    /* verify the client sent certificate if required */
    if (ssl->options.side == WOLFSSL_SERVER_END && !ssl->options.resuming &&
            (ssl->options.mutualAuth || ssl->options.failNoCert)) {
#ifdef OPENSSL_COMPATIBLE_DEFAULTS
        if (ssl->options.isPSK) {
            WOLFSSL_MSG("TLS v1.3 client used PSK but cert required. Allowing "
                        "for OpenSSL compatibility");
        }
        else
#endif
        if (
        #ifdef WOLFSSL_POST_HANDSHAKE_AUTH
            !ssl->options.verifyPostHandshake &&
        #endif
            (!ssl->options.havePeerCert || !ssl->options.havePeerVerify)) {
            ret = NO_PEER_CERT; /* NO_PEER_VERIFY */
            WOLFSSL_MSG("TLS v1.3 client did not present peer cert");
            DoCertFatalAlert(ssl, ret);
            return ret;
        }
    }
#endif

    /* check against totalSz */
    if (*inOutIdx + size > totalSz)
        return BUFFER_E;

#if defined(WOLFSSL_RENESAS_TSIP_TLS)
    ret = tsip_Tls13HandleFinished(ssl, input, inOutIdx, size, totalSz);
    if (ret == 0) {
        ssl->options.serverState = SERVER_FINISHED_COMPLETE;
        return ret;
    }
    if (ret == VERIFY_FINISHED_ERROR) {
        SendAlert(ssl, alert_fatal, decrypt_error);
        return ret;
    }
    if (ret != CRYPTOCB_UNAVAILABLE) {
        /* other errors */
        return ret;
    }
    ret = 0;
#endif /* WOLFSSL_RENESAS_TSIP_TLS */

    if (ssl->options.handShakeDone) {
        ret = DeriveFinishedSecret(ssl, ssl->clientSecret,
                                   ssl->keys.client_write_MAC_secret,
                                   WOLFSSL_CLIENT_END);
        if (ret != 0)
            return ret;

        secret = ssl->keys.client_write_MAC_secret;
    }
    else if (ssl->options.side == WOLFSSL_CLIENT_END) {
        /* All the handshake messages have been received to calculate
         * client and server finished keys.
         */
        ret = DeriveFinishedSecret(ssl, ssl->clientSecret,
                                   ssl->keys.client_write_MAC_secret,
                                   WOLFSSL_CLIENT_END);
        if (ret != 0)
            return ret;

        ret = DeriveFinishedSecret(ssl, ssl->serverSecret,
                                   ssl->keys.server_write_MAC_secret,
                                   WOLFSSL_SERVER_END);
        if (ret != 0)
            return ret;

        secret = ssl->keys.server_write_MAC_secret;
    }
    else {
        secret = ssl->keys.client_write_MAC_secret;
    }

    if (sniff == NO_SNIFF) {
        ret = BuildTls13HandshakeHmac(ssl, secret, mac, &finishedSz);
    #ifdef WOLFSSL_HAVE_TLS_UNIQUE
        if (ssl->options.side == WOLFSSL_CLIENT_END) {
            XMEMCPY(ssl->serverFinished, mac, finishedSz);
            ssl->serverFinished_len = finishedSz;
        }
        else {
            XMEMCPY(ssl->clientFinished, mac, finishedSz);
            ssl->clientFinished_len = finishedSz;
        }
    #endif /* WOLFSSL_HAVE_TLS_UNIQUE */
        if (ret != 0)
            return ret;
        if (size != finishedSz)
            return BUFFER_ERROR;
    }

#ifdef WOLFSSL_CALLBACKS
    if (ssl->hsInfoOn) AddPacketName(ssl, "Finished");
    if (ssl->toInfoOn) AddLateName("Finished", &ssl->timeoutInfo);
#endif

    if (sniff == NO_SNIFF) {
        /* Actually check verify data. */
        if (XMEMCMP(input + *inOutIdx, mac, size) != 0){
            WOLFSSL_MSG("Verify finished error on hashes");
            SendAlert(ssl, alert_fatal, decrypt_error);
            WOLFSSL_ERROR_VERBOSE(VERIFY_FINISHED_ERROR);
            return VERIFY_FINISHED_ERROR;
        }
    }

    /* Force input exhaustion at ProcessReply by consuming padSz. */
    *inOutIdx += size + ssl->keys.padSz;

    if (ssl->options.side == WOLFSSL_SERVER_END &&
                                                  !ssl->options.handShakeDone) {
#ifdef WOLFSSL_EARLY_DATA
        if (ssl->earlyData != no_early_data) {
            if ((ret = DeriveTls13Keys(ssl, no_key, DECRYPT_SIDE_ONLY, 1)) != 0)
                return ret;
        }
#endif
        /* Setup keys for application data messages from client. */
        if ((ret = SetKeysSide(ssl, DECRYPT_SIDE_ONLY)) != 0)
            return ret;
    }

#ifndef NO_WOLFSSL_CLIENT
    if (ssl->options.side == WOLFSSL_CLIENT_END)
        ssl->options.serverState = SERVER_FINISHED_COMPLETE;
#endif
#ifndef NO_WOLFSSL_SERVER
    if (ssl->options.side == WOLFSSL_SERVER_END) {
        ssl->options.clientState = CLIENT_FINISHED_COMPLETE;
        ssl->options.handShakeState = HANDSHAKE_DONE;
        ssl->options.handShakeDone  = 1;
    }
#endif

#if defined(WOLFSSL_DTLS13) && defined(WOLFSSL_EARLY_DATA)
    if (ssl->options.dtls && ssl->earlyData > early_data_ext) {
        /* DTLSv1.3 has no EndOfearlydata messages. We stop processing EarlyData
           as soon we receive the client's finished message */
        ssl->earlyData = done_early_data;
    }
#endif /* WOLFSSL_DTLS13 && WOLFSSL_EARLY_DATA */
#if defined(WOLFSSL_QUIC) && defined(WOLFSSL_EARLY_DATA)
    if (WOLFSSL_IS_QUIC(ssl) && ssl->earlyData > early_data_ext) {
        /* QUIC has no EndOfEarlyData messages. We stop processing EarlyData
           as soon we receive the client's finished message */
        ssl->earlyData = done_early_data;
    }
#endif /* WOLFSSL_QUIC && WOLFSSL_EARLY_DATA */

    WOLFSSL_LEAVE("DoTls13Finished", 0);
    WOLFSSL_END(WC_FUNC_FINISHED_DO);

    return 0;
}

#if !defined(NO_WOLFSSL_CLIENT) || !defined(NO_WOLFSSL_SERVER)
/* Send the TLS v1.3 Finished message.
 *
 * ssl  The SSL/TLS object.
 * returns 0 on success, otherwise failure.
 */
static int SendTls13Finished(WOLFSSL* ssl)
{
    int   finishedSz = ssl->specs.hash_size;
    byte* input;
    byte* output;
    int   ret;
    int   headerSz = HANDSHAKE_HEADER_SZ;
    int   outputSz;
    byte* secret;

#ifdef WOLFSSL_DTLS13
    int dtlsRet = 0, isDtls = 0;
#endif /* WOLFSSL_DTLS13 */

    WOLFSSL_START(WC_FUNC_FINISHED_SEND);
    WOLFSSL_ENTER("SendTls13Finished");

    ssl->options.buildingMsg = 1;
#ifdef WOLFSSL_DTLS13
    if (ssl->options.dtls) {
        headerSz = DTLS_HANDSHAKE_HEADER_SZ;
        /* using isDtls instead of ssl->options.dtls will abide clang static
           analyzer on using an uninitialized value */
        isDtls = 1;
    }
#endif /* WOLFSSL_DTLS13 */

    outputSz = WC_MAX_DIGEST_SIZE + DTLS_HANDSHAKE_HEADER_SZ + MAX_MSG_EXTRA;
    /* Check buffers are big enough and grow if needed. */
    if ((ret = CheckAvailableSize(ssl, outputSz)) != 0)
        return ret;

    /* get output buffer */
    output = GetOutputBuffer(ssl);
    input = output + RECORD_HEADER_SZ;

#ifdef WOLFSSL_DTLS13
    if (isDtls)
        input = output + Dtls13GetRlHeaderLength(ssl, 1);
#endif /* WOLFSSL_DTLS13 */

    AddTls13HandShakeHeader(input, finishedSz, 0, finishedSz, finished, ssl);

#if defined(WOLFSSL_RENESAS_TSIP_TLS)
    if (ssl->options.side == WOLFSSL_CLIENT_END) {
        ret = tsip_Tls13SendFinished(ssl, output, outputSz, input, 1);
        if (ret != CRYPTOCB_UNAVAILABLE) {
            return ret;
        }
        ret = 0;
    }
#endif /* WOLFSSL_RENESAS_TSIP_TLS */

    /* make finished hashes */
    if (ssl->options.handShakeDone) {
        ret = DeriveFinishedSecret(ssl, ssl->clientSecret,
                                   ssl->keys.client_write_MAC_secret,
                                   WOLFSSL_CLIENT_END);
        if (ret != 0)
            return ret;

        secret = ssl->keys.client_write_MAC_secret;
    }
    else if (ssl->options.side == WOLFSSL_CLIENT_END)
        secret = ssl->keys.client_write_MAC_secret;
    else {
        /* All the handshake messages have been done to calculate client and
         * server finished keys.
         */
        ret = DeriveFinishedSecret(ssl, ssl->clientSecret,
                                   ssl->keys.client_write_MAC_secret,
                                   WOLFSSL_SERVER_END);
        if (ret != 0)
            return ret;

        ret = DeriveFinishedSecret(ssl, ssl->serverSecret,
                                   ssl->keys.server_write_MAC_secret,
                                   WOLFSSL_CLIENT_END);
        if (ret != 0)
            return ret;

        secret = ssl->keys.server_write_MAC_secret;
    }
    ret = BuildTls13HandshakeHmac(ssl, secret, &input[headerSz], NULL);
    if (ret != 0)
        return ret;
    #ifdef WOLFSSL_HAVE_TLS_UNIQUE
        if (ssl->options.side == WOLFSSL_CLIENT_END) {
            XMEMCPY(ssl->clientFinished, &input[headerSz], finishedSz);
            ssl->clientFinished_len = finishedSz;
        }
        else {
            XMEMCPY(ssl->serverFinished, &input[headerSz], finishedSz);
            ssl->serverFinished_len = finishedSz;
        }
    #endif /* WOLFSSL_HAVE_TLS_UNIQUE */

#ifdef WOLFSSL_DTLS13
    if (isDtls) {
        dtlsRet = Dtls13HandshakeSend(ssl, output, (word16)outputSz,
            (word16)(Dtls13GetRlHeaderLength(ssl, 1) + headerSz + finishedSz), finished,
            1);
        if (dtlsRet != 0 && dtlsRet != WANT_WRITE)
            return ret;

    } else
#endif /* WOLFSSL_DTLS13 */
    {
        /* This message is always encrypted. */
        int sendSz = BuildTls13Message(ssl, output, outputSz, input,
                                   headerSz + finishedSz, handshake, 1, 0, 0);
        if (sendSz < 0) {
            WOLFSSL_ERROR_VERBOSE(BUILD_MSG_ERROR);
            return BUILD_MSG_ERROR;
        }

        #if defined(WOLFSSL_CALLBACKS) || defined(OPENSSL_EXTRA)
            if (ssl->hsInfoOn) AddPacketName(ssl, "Finished");
            if (ssl->toInfoOn) {
                ret = AddPacketInfo(ssl, "Finished", handshake, output, sendSz,
                              WRITE_PROTO, 0, ssl->heap);
                if (ret != 0)
                    return ret;
            }
        #endif

        ssl->buffers.outputBuffer.length += sendSz;
        ssl->options.buildingMsg = 0;
    }

    if (ssl->options.side == WOLFSSL_SERVER_END) {
#ifdef WOLFSSL_EARLY_DATA
        byte storeTrafficDecKeys = ssl->earlyData == no_early_data;
#endif
        /* Can send application data now. */
        if ((ret = DeriveMasterSecret(ssl)) != 0)
            return ret;
        /* Last use of preMasterSecret - zeroize as soon as possible. */
        ForceZero(ssl->arrays->preMasterSecret, ssl->arrays->preMasterSz);
#ifdef WOLFSSL_EARLY_DATA

#ifdef WOLFSSL_DTLS13
        /* DTLS13 dynamically change keys and it needs all
           the keys in ssl->keys to save the keying material */
        if (isDtls)
            storeTrafficDecKeys = 1;
#endif /* WOLFSSL_DTLS13 */

        if ((ret = DeriveTls13Keys(ssl, traffic_key, ENCRYPT_SIDE_ONLY, 1))
                                                                         != 0) {
            return ret;
        }
        if ((ret = DeriveTls13Keys(ssl, traffic_key, DECRYPT_SIDE_ONLY,
                                       storeTrafficDecKeys)) != 0) {
            return ret;
        }
#else
        if ((ret = DeriveTls13Keys(ssl, traffic_key, ENCRYPT_AND_DECRYPT_SIDE,
                                                                     1)) != 0) {
            return ret;
        }
#endif
        if ((ret = SetKeysSide(ssl, ENCRYPT_SIDE_ONLY)) != 0)
            return ret;

#ifdef WOLFSSL_DTLS13
        if (isDtls) {
            w64wrapper epochTraffic0;
            epochTraffic0 = w64From32(0, DTLS13_EPOCH_TRAFFIC0);
            ssl->dtls13Epoch = epochTraffic0;
            ssl->dtls13PeerEpoch = epochTraffic0;

            ret = Dtls13NewEpoch(
                ssl, epochTraffic0, ENCRYPT_AND_DECRYPT_SIDE);
            if (ret != 0)
                return ret;

            ret = Dtls13SetEpochKeys(
                ssl, epochTraffic0, ENCRYPT_AND_DECRYPT_SIDE);
            if (ret != 0)
                return ret;

        }
#endif /* WOLFSSL_DTLS13 */

    }

    if (ssl->options.side == WOLFSSL_CLIENT_END &&
                                                  !ssl->options.handShakeDone) {
#ifdef WOLFSSL_EARLY_DATA
        if (ssl->earlyData != no_early_data) {
            if ((ret = DeriveTls13Keys(ssl, no_key, ENCRYPT_AND_DECRYPT_SIDE,
                                                                     1)) != 0) {
                    return ret;
            }
        }
#endif
        /* Setup keys for application data messages. */
        if ((ret = SetKeysSide(ssl, ENCRYPT_AND_DECRYPT_SIDE)) != 0)
            return ret;

#if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
        ret = DeriveResumptionSecret(ssl, ssl->session->masterSecret);
        if (ret != 0)
            return ret;
#endif

#ifdef WOLFSSL_DTLS13
        if (isDtls) {
            w64wrapper epochTraffic0;
            epochTraffic0 = w64From32(0, DTLS13_EPOCH_TRAFFIC0);
            ssl->dtls13Epoch = epochTraffic0;
            ssl->dtls13PeerEpoch = epochTraffic0;

            ret = Dtls13NewEpoch(
                ssl, epochTraffic0, ENCRYPT_AND_DECRYPT_SIDE);
            if (ret != 0)
                return ret;

            ret = Dtls13SetEpochKeys(
                ssl, epochTraffic0, ENCRYPT_AND_DECRYPT_SIDE);
            if (ret != 0)
                return ret;

        }
#endif /* WOLFSSL_DTLS13 */
    }

#ifndef NO_WOLFSSL_CLIENT
    if (ssl->options.side == WOLFSSL_CLIENT_END) {
        ssl->options.clientState = CLIENT_FINISHED_COMPLETE;
        ssl->options.handShakeState = HANDSHAKE_DONE;
        ssl->options.handShakeDone  = 1;
    }
#endif
#ifndef NO_WOLFSSL_SERVER
    if (ssl->options.side == WOLFSSL_SERVER_END) {
        ssl->options.serverState = SERVER_FINISHED_COMPLETE;
    }
#endif

#ifdef WOLFSSL_DTLS13
    if (isDtls) {
        WOLFSSL_LEAVE("SendTls13Finished", ret);
        WOLFSSL_END(WC_FUNC_FINISHED_SEND);

        return dtlsRet;
    }
#endif /* WOLFSSL_DTLS13 */

    if ((ret = SendBuffered(ssl)) != 0)
        return ret;

    WOLFSSL_LEAVE("SendTls13Finished", ret);
    WOLFSSL_END(WC_FUNC_FINISHED_SEND);

    return ret;
}
#endif /* !NO_WOLFSSL_CLIENT || !NO_WOLFSSL_SERVER */

/* handle generation TLS v1.3 key_update (24) */
/* Send the TLS v1.3 KeyUpdate message.
 *
 * ssl  The SSL/TLS object.
 * returns 0 on success, otherwise failure.
 */
static int SendTls13KeyUpdate(WOLFSSL* ssl)
{
    byte*  input;
    byte*  output;
    int    ret;
    int    headerSz = HANDSHAKE_HEADER_SZ;
    int    outputSz;
    word32 i = RECORD_HEADER_SZ + HANDSHAKE_HEADER_SZ;

    WOLFSSL_START(WC_FUNC_KEY_UPDATE_SEND);
    WOLFSSL_ENTER("SendTls13KeyUpdate");

#ifdef WOLFSSL_DTLS13
    if (ssl->options.dtls)
        i = Dtls13GetRlHeaderLength(ssl, 1) + DTLS_HANDSHAKE_HEADER_SZ;
#endif /* WOLFSSL_DTLS13 */

    outputSz = OPAQUE8_LEN + MAX_MSG_EXTRA;
    /* Check buffers are big enough and grow if needed. */
    if ((ret = CheckAvailableSize(ssl, outputSz)) != 0)
        return ret;

    /* get output buffer */
    output = GetOutputBuffer(ssl);
    input = output + RECORD_HEADER_SZ;

#ifdef WOLFSSL_DTLS13
    if (ssl->options.dtls)
        input = output + Dtls13GetRlHeaderLength(ssl, 1);
#endif /* WOLFSSL_DTLS13 */

    AddTls13Headers(output, OPAQUE8_LEN, key_update, ssl);

    /* If:
     *   1. I haven't sent a KeyUpdate requesting a response and
     *   2. This isn't responding to peer KeyUpdate requiring a response then,
     * I want a response.
     */
    ssl->keys.updateResponseReq = output[i++] =
         !ssl->keys.updateResponseReq && !ssl->keys.keyUpdateRespond;
    /* Sent response, no longer need to respond. */
    ssl->keys.keyUpdateRespond = 0;

#ifdef WOLFSSL_DTLS13
    if (ssl->options.dtls) {
        ret = Dtls13HandshakeSend(ssl, output, (word16)outputSz,
            OPAQUE8_LEN + Dtls13GetRlHeaderLength(ssl, 1) +
                DTLS_HANDSHAKE_HEADER_SZ,
            key_update, 0);
    }
    else
#endif /* WOLFSSL_DTLS13 */
    {
        /* This message is always encrypted. */
        int sendSz = BuildTls13Message(ssl, output, outputSz, input,
                                   headerSz + OPAQUE8_LEN, handshake, 0, 0, 0);
        if (sendSz < 0)
            return BUILD_MSG_ERROR;

        #if defined(WOLFSSL_CALLBACKS) || defined(OPENSSL_EXTRA)
            if (ssl->hsInfoOn) AddPacketName(ssl, "KeyUpdate");
            if (ssl->toInfoOn) {
                ret = AddPacketInfo(ssl, "KeyUpdate", handshake, output, sendSz,
                              WRITE_PROTO, 0, ssl->heap);
                if (ret != 0)
                    return ret;
            }
        #endif

        ssl->buffers.outputBuffer.length += sendSz;

        ret = SendBuffered(ssl);


        if (ret != 0 && ret != WANT_WRITE)
            return ret;
    }

    /* In DTLS we must wait for the ack before setting up the new keys */
    if (!ssl->options.dtls) {

        /* Future traffic uses new encryption keys. */
        if ((ret = DeriveTls13Keys(
                       ssl, update_traffic_key, ENCRYPT_SIDE_ONLY, 1))
            != 0)
            return ret;
        if ((ret = SetKeysSide(ssl, ENCRYPT_SIDE_ONLY)) != 0)
            return ret;
    }


    WOLFSSL_LEAVE("SendTls13KeyUpdate", ret);
    WOLFSSL_END(WC_FUNC_KEY_UPDATE_SEND);

    return ret;
}

/* handle processing TLS v1.3 key_update (24) */
/* Parse and handle a TLS v1.3 KeyUpdate message.
 *
 * ssl       The SSL/TLS object.
 * input     The message buffer.
 * inOutIdx  On entry, the index into the message buffer of Finished.
 *           On exit, the index of byte after the Finished message and padding.
 * totalSz   The length of the current handshake message.
 * returns 0 on success and otherwise failure.
 */
static int DoTls13KeyUpdate(WOLFSSL* ssl, const byte* input, word32* inOutIdx,
                            word32 totalSz)
{
    int    ret;
    word32 i = *inOutIdx;

    WOLFSSL_START(WC_FUNC_KEY_UPDATE_DO);
    WOLFSSL_ENTER("DoTls13KeyUpdate");

    /* check against totalSz */
    if (OPAQUE8_LEN != totalSz)
        return BUFFER_E;

    switch (input[i]) {
        case update_not_requested:
            /* This message in response to any outstanding request. */
            ssl->keys.keyUpdateRespond = 0;
            ssl->keys.updateResponseReq = 0;
            break;
        case update_requested:
            /* New key update requiring a response. */
            ssl->keys.keyUpdateRespond = 1;
            break;
        default:
            WOLFSSL_ERROR_VERBOSE(INVALID_PARAMETER);
            return INVALID_PARAMETER;
    }

    /* Move index to byte after message. */
    *inOutIdx += totalSz;
    /* Always encrypted. */
    *inOutIdx += ssl->keys.padSz;

    /* Future traffic uses new decryption keys. */
    if ((ret = DeriveTls13Keys(ssl, update_traffic_key, DECRYPT_SIDE_ONLY, 1))
                                                                         != 0) {
        return ret;
    }
    if ((ret = SetKeysSide(ssl, DECRYPT_SIDE_ONLY)) != 0)
        return ret;

#ifdef WOLFSSL_DTLS13
    if (ssl->options.dtls) {
        w64Increment(&ssl->dtls13PeerEpoch);

        ret = Dtls13NewEpoch(ssl, ssl->dtls13PeerEpoch, DECRYPT_SIDE_ONLY);
        if (ret != 0)
            return ret;

        ret = Dtls13SetEpochKeys(ssl, ssl->dtls13PeerEpoch, DECRYPT_SIDE_ONLY);
        if (ret != 0)
            return ret;
    }
#endif /* WOLFSSL_DTLS13 */

    if (ssl->keys.keyUpdateRespond) {

#ifdef WOLFSSL_DTLS13
        /* we already sent a keyUpdate (either in response to a previous
           KeyUpdate or initiated by the application) and we are waiting for the
           ack. We can't send a new KeyUpdate right away but to honor the RFC we
           should send another KeyUpdate after the one in-flight is acked. We
           don't do that as it looks redundant, it will make the code more
           complex and I don't see a good use case for that. */
        if (ssl->options.dtls && ssl->dtls13WaitKeyUpdateAck) {
            ssl->keys.keyUpdateRespond = 0;
            return 0;
        }
#endif /* WOLFSSL_DTLS13 */

        return SendTls13KeyUpdate(ssl);
    }

    WOLFSSL_LEAVE("DoTls13KeyUpdate", ret);
    WOLFSSL_END(WC_FUNC_KEY_UPDATE_DO);

    return 0;
}

#ifdef WOLFSSL_EARLY_DATA
#ifndef NO_WOLFSSL_CLIENT
/* Send the TLS v1.3 EndOfEarlyData message to indicate that there will be no
 * more early application data.
 * The encryption key now changes to the pre-calculated handshake key.
 *
 * ssl  The SSL/TLS object.
 * returns 0 on success and otherwise failure.
 */
static int SendTls13EndOfEarlyData(WOLFSSL* ssl)
{
    byte*  output;
    int    ret;
    int    sendSz;
    word32 length;
    word32 idx = RECORD_HEADER_SZ + HANDSHAKE_HEADER_SZ;

    WOLFSSL_START(WC_FUNC_END_OF_EARLY_DATA_SEND);
    WOLFSSL_ENTER("SendTls13EndOfEarlyData");

    length = 0;
    sendSz = idx + length + MAX_MSG_EXTRA;
    ssl->options.buildingMsg = 1;

    /* Check buffers are big enough and grow if needed. */
    if ((ret = CheckAvailableSize(ssl, sendSz)) != 0)
        return ret;

    /* Get position in output buffer to write new message to. */
    output = GetOutputBuffer(ssl);

    /* Put the record and handshake headers on. */
    AddTls13Headers(output, length, end_of_early_data, ssl);

    /* This message is always encrypted. */
    sendSz = BuildTls13Message(ssl, output, sendSz, output + RECORD_HEADER_SZ,
                               idx - RECORD_HEADER_SZ, handshake, 1, 0, 0);
    if (sendSz < 0)
        return sendSz;

    ssl->buffers.outputBuffer.length += sendSz;

    if ((ret = SetKeysSide(ssl, ENCRYPT_SIDE_ONLY)) != 0)
        return ret;

    ssl->options.buildingMsg = 0;
    if (!ssl->options.groupMessages)
        ret = SendBuffered(ssl);

    WOLFSSL_LEAVE("SendTls13EndOfEarlyData", ret);
    WOLFSSL_END(WC_FUNC_END_OF_EARLY_DATA_SEND);

    return ret;
}
#endif /* !NO_WOLFSSL_CLIENT */

#ifndef NO_WOLFSSL_SERVER
/* handle processing of TLS 1.3 end_of_early_data (5) */
/* Parse the TLS v1.3 EndOfEarlyData message that indicates that there will be
 * no more early application data.
 * The decryption key now changes to the pre-calculated handshake key.
 *
 * ssl  The SSL/TLS object.
 * returns 0 on success and otherwise failure.
 */
static int DoTls13EndOfEarlyData(WOLFSSL* ssl, const byte* input,
                                 word32* inOutIdx, word32 size)
{
    int    ret;
    word32 begin = *inOutIdx;

    (void)input;

    WOLFSSL_START(WC_FUNC_END_OF_EARLY_DATA_DO);
    WOLFSSL_ENTER("DoTls13EndOfEarlyData");

    if ((*inOutIdx - begin) != size)
        return BUFFER_ERROR;

    if (ssl->earlyData == no_early_data) {
        WOLFSSL_MSG("EndOfEarlyData received unexpectedly");
        SendAlert(ssl, alert_fatal, unexpected_message);
        WOLFSSL_ERROR_VERBOSE(OUT_OF_ORDER_E);
        return OUT_OF_ORDER_E;
    }

    ssl->earlyData = done_early_data;

    /* Always encrypted. */
    *inOutIdx += ssl->keys.padSz;

    ret = SetKeysSide(ssl, DECRYPT_SIDE_ONLY);

    WOLFSSL_LEAVE("DoTls13EndOfEarlyData", ret);
    WOLFSSL_END(WC_FUNC_END_OF_EARLY_DATA_DO);

    return ret;
}
#endif /* !NO_WOLFSSL_SERVER */
#endif /* WOLFSSL_EARLY_DATA */

#if defined(HAVE_SESSION_TICKET) && defined(WOLFSSL_TICKET_NONCE_MALLOC) &&    \
    (!defined(HAVE_FIPS) || (defined(FIPS_VERSION_GE) && FIPS_VERSION_GE(5,3)))
int SessionTicketNoncePopulate(WOLFSSL_SESSION *session, const byte *nonce,
    byte len)
{
    if (session->ticketNonce.data
            != session->ticketNonce.dataStatic) {
         XFREE(session->ticketNonce.data, session->heap,
             DYNAMIC_TYPE_SESSION_TICK);
         session->ticketNonce.data = session->ticketNonce.dataStatic;
         session->ticketNonce.len = 0;
    }

    if (len > MAX_TICKET_NONCE_STATIC_SZ) {
        WOLFSSL_MSG("Using dynamic nonce buffer");
        session->ticketNonce.data = (byte*)XMALLOC(len,
            session->heap, DYNAMIC_TYPE_SESSION_TICK);
        if (session->ticketNonce.data == NULL)
            return MEMORY_ERROR;
    }
    XMEMCPY(session->ticketNonce.data, nonce, len);
    session->ticketNonce.len = len;
    return 0;
}
#endif
#ifndef NO_WOLFSSL_CLIENT
/* Handle a New Session Ticket handshake message.
 * Message contains the information required to perform resumption.
 *
 * ssl       The SSL/TLS object.
 * input     The message buffer.
 * inOutIdx  On entry, the index into the message buffer of Finished.
 *           On exit, the index of byte after the Finished message and padding.
 * size      The length of the current handshake message.
 * returns 0 on success, otherwise failure.
 */
static int DoTls13NewSessionTicket(WOLFSSL* ssl, const byte* input,
                                   word32* inOutIdx, word32 size)
{
#ifdef HAVE_SESSION_TICKET
    int    ret;
    word32 begin = *inOutIdx;
    word32 lifetime;
    word32 ageAdd;
    word16 length;
#ifdef WOLFSSL_32BIT_MILLI_TIME
    word32 now;
#else
    sword64 now;
#endif
    const byte* nonce;
    byte        nonceLength;

    WOLFSSL_START(WC_FUNC_NEW_SESSION_TICKET_DO);
    WOLFSSL_ENTER("DoTls13NewSessionTicket");

    /* Lifetime hint. */
    if ((*inOutIdx - begin) + SESSION_HINT_SZ > size)
        return BUFFER_ERROR;
    ato32(input + *inOutIdx, &lifetime);
    *inOutIdx += SESSION_HINT_SZ;
    if (lifetime > MAX_LIFETIME) {
        WOLFSSL_ERROR_VERBOSE(SERVER_HINT_ERROR);
        return SERVER_HINT_ERROR;
    }

    /* Age add. */
    if ((*inOutIdx - begin) + SESSION_ADD_SZ > size)
        return BUFFER_ERROR;
    ato32(input + *inOutIdx, &ageAdd);
    *inOutIdx += SESSION_ADD_SZ;

    /* Ticket nonce. */
    if ((*inOutIdx - begin) + 1 > size)
        return BUFFER_ERROR;
    nonceLength = input[*inOutIdx];
#if !defined(WOLFSSL_TICKET_NONCE_MALLOC) &&                                   \
    (!defined(HAVE_FIPS) || FIPS_VERSION_GE(5,3))
    if (nonceLength > MAX_TICKET_NONCE_STATIC_SZ) {
        WOLFSSL_MSG("Nonce length not supported");
        WOLFSSL_ERROR_VERBOSE(INVALID_PARAMETER);
        return INVALID_PARAMETER;
    }
#endif /* WOLFSSL_TICKET_NONCE_MALLOC && FIPS_VERSION_GE(5,3) */
    *inOutIdx += 1;
    if ((*inOutIdx - begin) + nonceLength > size)
        return BUFFER_ERROR;
    nonce = input + *inOutIdx;
    *inOutIdx += nonceLength;

    /* Ticket length. */
    if ((*inOutIdx - begin) + LENGTH_SZ > size)
        return BUFFER_ERROR;
    ato16(input + *inOutIdx, &length);
    *inOutIdx += LENGTH_SZ;
    if ((*inOutIdx - begin) + length > size)
        return BUFFER_ERROR;

    if ((ret = SetTicket(ssl, input + *inOutIdx, length)) != 0)
        return ret;
    *inOutIdx += length;

    now = TimeNowInMilliseconds();
    if (now == 0)
        return GETTIME_ERROR;
    /* Copy in ticket data (server identity). */
    ssl->timeout                  = lifetime;
    ssl->session->timeout         = lifetime;
    ssl->session->cipherSuite0    = ssl->options.cipherSuite0;
    ssl->session->cipherSuite     = ssl->options.cipherSuite;
    ssl->session->ticketSeen      = now;
    ssl->session->ticketAdd       = ageAdd;
    #ifdef WOLFSSL_EARLY_DATA
    ssl->session->maxEarlyDataSz  = ssl->options.maxEarlyDataSz;
    #endif

#if defined(WOLFSSL_TICKET_NONCE_MALLOC) &&                                    \
    (!defined(HAVE_FIPS) || (defined(FIPS_VERSION_GE) && FIPS_VERSION_GE(5,3)))
    ret = SessionTicketNoncePopulate(ssl->session, nonce, nonceLength);
    if (ret != 0)
        return ret;
#else
    ssl->session->ticketNonce.len = nonceLength;
    if (nonceLength > MAX_TICKET_NONCE_STATIC_SZ) {
        ret = BUFFER_ERROR;
        return ret;
    }
    if (nonceLength > 0)
        XMEMCPY(ssl->session->ticketNonce.data, nonce, nonceLength);
#endif /* defined(WOLFSSL_TICKET_NONCE_MALLOC) && FIPS_VERSION_GE(5,3) */

    ssl->session->namedGroup      = ssl->namedGroup;

    if ((*inOutIdx - begin) + EXTS_SZ > size)
        return BUFFER_ERROR;
    ato16(input + *inOutIdx, &length);
    *inOutIdx += EXTS_SZ;
    if ((*inOutIdx - begin) + length != size)
        return BUFFER_ERROR;
    #ifdef WOLFSSL_EARLY_DATA
    ret = TLSX_Parse(ssl, (byte *)input + (*inOutIdx), length, session_ticket,
                     NULL);
    if (ret != 0)
        return ret;
    #endif
    *inOutIdx += length;

    SetupSession(ssl);
    #ifndef NO_SESSION_CACHE
        AddSession(ssl);
    #endif

    /* Always encrypted. */
    *inOutIdx += ssl->keys.padSz;

    ssl->expect_session_ticket = 0;
#else
    (void)ssl;
    (void)input;

    WOLFSSL_ENTER("DoTls13NewSessionTicket");

    *inOutIdx += size + ssl->keys.padSz;
#endif /* HAVE_SESSION_TICKET */

    WOLFSSL_LEAVE("DoTls13NewSessionTicket", 0);
    WOLFSSL_END(WC_FUNC_NEW_SESSION_TICKET_DO);

    return 0;
}
#endif /* NO_WOLFSSL_CLIENT */

#ifndef NO_WOLFSSL_SERVER
    #ifdef HAVE_SESSION_TICKET

#ifdef WOLFSSL_TLS13_TICKET_BEFORE_FINISHED
/* Offset of the MAC size in the finished message. */
#define FINISHED_MSG_SIZE_OFFSET    3

/* Calculate the resumption secret which includes the unseen client finished
 * message.
 *
 * ssl  The SSL/TLS object.
 * returns 0 on success, otherwise failure.
 */
static int ExpectedResumptionSecret(WOLFSSL* ssl)
{
    int         ret;
    word32      finishedSz = 0;
    byte        mac[WC_MAX_DIGEST_SIZE];
    Digest      digest;
    static byte header[] = { 0x14, 0x00, 0x00, 0x00 };

    /* Copy the running hash so we can restore it after. */
    switch (ssl->specs.mac_algorithm) {
    #ifndef NO_SHA256
        case sha256_mac:
            ret = wc_Sha256Copy(&ssl->hsHashes->hashSha256, &digest.sha256);
            if (ret != 0)
                return ret;
            break;
    #endif
    #ifdef WOLFSSL_SHA384
        case sha384_mac:
            ret = wc_Sha384Copy(&ssl->hsHashes->hashSha384, &digest.sha384);
            if (ret != 0)
                return ret;
            break;
    #endif
    #ifdef WOLFSSL_TLS13_SHA512
        case sha512_mac:
            ret = wc_Sha512Copy(&ssl->hsHashes->hashSha512, &digest.sha512);
            if (ret != 0)
                return ret;
            break;
    #endif
    #ifdef WOLFSSL_SM3
        case sm3_mac:
            ret = wc_Sm3Copy(&ssl->hsHashes->hashSm3, &digest.sm3);
            if (ret != 0)
                return ret;
            break;
    #endif
    }

    /* Generate the Client's Finished message and hash it. */
    ret = BuildTls13HandshakeHmac(ssl, ssl->keys.client_write_MAC_secret, mac,
                                  &finishedSz);
    if (ret != 0)
        return ret;
    header[FINISHED_MSG_SIZE_OFFSET] = finishedSz;
#ifdef WOLFSSL_EARLY_DATA
    if (ssl->earlyData != no_early_data) {
        static byte endOfEarlyData[] = { 0x05, 0x00, 0x00, 0x00 };
        ret = HashRaw(ssl, endOfEarlyData, sizeof(endOfEarlyData));
        if (ret != 0)
            return ret;
    }
#endif
    if ((ret = HashRaw(ssl, header, sizeof(header))) != 0)
        return ret;
    if ((ret = HashRaw(ssl, mac, finishedSz)) != 0)
        return ret;

    if ((ret = DeriveResumptionSecret(ssl, ssl->session->masterSecret)) != 0)
        return ret;

    /* Restore the hash inline with currently seen messages. */
    switch (ssl->specs.mac_algorithm) {
    #ifndef NO_SHA256
        case sha256_mac:
            wc_Sha256Free(&ssl->hsHashes->hashSha256);
            ret = wc_Sha256Copy(&digest.sha256, &ssl->hsHashes->hashSha256);
            wc_Sha256Free(&digest.sha256);
            if (ret != 0)
                return ret;
            break;
    #endif
    #ifdef WOLFSSL_SHA384
        case sha384_mac:
            wc_Sha384Free(&ssl->hsHashes->hashSha384);
            ret = wc_Sha384Copy(&digest.sha384, &ssl->hsHashes->hashSha384);
            wc_Sha384Free(&digest.sha384);
            if (ret != 0)
                return ret;
            break;
    #endif
    #ifdef WOLFSSL_TLS13_SHA512
        case sha512_mac:
            wc_Sha512Free(&ssl->hsHashes->hashSha512);
            ret = wc_Sha512Copy(&digest.sha512, &ssl->hsHashes->hashSha512);
            wc_Sha512Free(&digest.sha512);
            if (ret != 0)
                return ret;
            break;
    #endif
    #ifdef WOLFSSL_SM3
        case sm3_mac:
            wc_Sm3Free(&ssl->hsHashes->hashSm3);
            ret = wc_Sm3Copy(&digest.sm3, &ssl->hsHashes->hashSm3);
            wc_Sm3Free(&digest.sm3);
            if (ret != 0)
                return ret;
            break;
    #endif
    }

    return ret;
}
#endif

/* Send New Session Ticket handshake message.
 * Message contains the information required to perform resumption.
 *
 * ssl  The SSL/TLS object.
 * returns 0 on success, otherwise failure.
 */
static int SendTls13NewSessionTicket(WOLFSSL* ssl)
{
    byte*  output;
    int    ret;
    int    sendSz;
    word16 extSz;
    word32 length;
    word32 idx = RECORD_HEADER_SZ + HANDSHAKE_HEADER_SZ;

    WOLFSSL_START(WC_FUNC_NEW_SESSION_TICKET_SEND);
    WOLFSSL_ENTER("SendTls13NewSessionTicket");

#ifdef WOLFSSL_DTLS13
    if (ssl->options.dtls)
        idx = Dtls13GetRlHeaderLength(ssl, 1) + DTLS_HANDSHAKE_HEADER_SZ;
#endif /* WOLFSSL_DTLS13 */

#ifdef WOLFSSL_TLS13_TICKET_BEFORE_FINISHED
    if (!ssl->msgsReceived.got_finished) {
        if ((ret = ExpectedResumptionSecret(ssl)) != 0)
            return ret;
    }
#endif

    /* Start ticket nonce at 0 and go up to 255. */
    if (ssl->session->ticketNonce.len == 0) {
        ssl->session->ticketNonce.len = DEF_TICKET_NONCE_SZ;
        ssl->session->ticketNonce.data[0] = 0;
    }
    else
    #ifdef WOLFSSL_ASYNC_CRYPT
        if (ssl->error != WC_PENDING_E)
    #endif
    {
            ssl->session->ticketNonce.data[0]++;
    }

    if ((ssl->options.mask & WOLFSSL_OP_NO_TICKET) != 0) {
        /* In this case we only send the ID as the ticket. Let's generate a new
         * ID for the new ticket so that we don't overwrite any old ones */
        ret = wc_RNG_GenerateBlock(ssl->rng, ssl->session->altSessionID,
                                   ID_LEN);
        if (ret != 0)
            return ret;
        ssl->session->haveAltSessionID = 1;
    }

    if (!ssl->options.noTicketTls13) {
        if ((ret = SetupTicket(ssl)) != 0)
            return ret;
        /* No need to create the ticket if we only send the ID */
        if ((ssl->options.mask & WOLFSSL_OP_NO_TICKET) == 0) {
            if ((ret = CreateTicket(ssl)) != 0)
                return ret;
        }
    }

#ifdef WOLFSSL_EARLY_DATA
    ssl->session->maxEarlyDataSz = ssl->options.maxEarlyDataSz;
    if (ssl->session->maxEarlyDataSz > 0)
        TLSX_EarlyData_Use(ssl, ssl->session->maxEarlyDataSz, 1);
    extSz = 0;
    ret = TLSX_GetResponseSize(ssl, session_ticket, &extSz);
    if (ret != 0)
        return ret;
#else
    extSz = EXTS_SZ;
#endif
    /* Lifetime | Age Add | Ticket session ID | Extensions */
    length = SESSION_HINT_SZ + SESSION_ADD_SZ + LENGTH_SZ;
    if ((ssl->options.mask & WOLFSSL_OP_NO_TICKET) != 0)
        length += ID_LEN + extSz;
    else
        length += ssl->session->ticketLen + extSz;
    /* Nonce */
    length += TICKET_NONCE_LEN_SZ + DEF_TICKET_NONCE_SZ;

    sendSz = idx + length + MAX_MSG_EXTRA;

    /* Check buffers are big enough and grow if needed. */
    if ((ret = CheckAvailableSize(ssl, sendSz)) != 0)
        return ret;

    /* Get position in output buffer to write new message to. */
    output = GetOutputBuffer(ssl);

    /* Put the record and handshake headers on. */
    AddTls13Headers(output, length, session_ticket, ssl);

    /* Lifetime hint */
    c32toa(ssl->ctx->ticketHint, output + idx);
    idx += SESSION_HINT_SZ;
    /* Age add - obfuscator */
    c32toa(ssl->session->ticketAdd, output + idx);
    idx += SESSION_ADD_SZ;

    output[idx++] = ssl->session->ticketNonce.len;
    output[idx++] = ssl->session->ticketNonce.data[0];

    /* length */
    if ((ssl->options.mask & WOLFSSL_OP_NO_TICKET) != 0) {
        c16toa(ID_LEN, output + idx);
    }
    else {
        c16toa(ssl->session->ticketLen, output + idx);
    }

    idx += LENGTH_SZ;
    /* ticket */
    if ((ssl->options.mask & WOLFSSL_OP_NO_TICKET) != 0) {
        if (ssl->session->haveAltSessionID)
            XMEMCPY(output + idx, ssl->session->altSessionID, ID_LEN);
        else
            return BAD_FUNC_ARG; /* Should not happen */
        idx += ID_LEN;
    }
    else {
        XMEMCPY(output + idx, ssl->session->ticket, ssl->session->ticketLen);
        idx += ssl->session->ticketLen;
    }

#ifdef WOLFSSL_EARLY_DATA
    extSz = 0;
    ret = TLSX_WriteResponse(ssl, output + idx, session_ticket, &extSz);
    if (ret != 0)
        return ret;
    idx += extSz;
#else
    /* No extension support - empty extensions. */
    c16toa(0, output + idx);
    idx += EXTS_SZ;
#endif

    ssl->options.haveSessionId = 1;

    SetupSession(ssl);
    /* Only add to cache when support built in and when the ticket contains
     * an ID. Otherwise we have no way to actually retrieve the ticket from the
     * cache. */
#if !defined(NO_SESSION_CACHE) && defined(WOLFSSL_TICKET_HAVE_ID)
    AddSession(ssl);
#endif

#ifdef WOLFSSL_DTLS13
    if (ssl->options.dtls)
        return Dtls13HandshakeSend(ssl, output, sendSz, idx, session_ticket, 0);
#endif /* WOLFSSL_DTLS13 */

    /* This message is always encrypted. */
    sendSz = BuildTls13Message(ssl, output, sendSz, output + RECORD_HEADER_SZ,
                               idx - RECORD_HEADER_SZ, handshake, 0, 0, 0);
    if (sendSz < 0)
        return sendSz;

    ssl->buffers.outputBuffer.length += sendSz;

    /* Always send as this is either directly after server's Finished or only
     * message after client's Finished.
     */
    ret = SendBuffered(ssl);

    WOLFSSL_LEAVE("SendTls13NewSessionTicket", 0);
    WOLFSSL_END(WC_FUNC_NEW_SESSION_TICKET_SEND);

    return ret;
}
    #endif /* HAVE_SESSION_TICKET */
#endif /* NO_WOLFSSL_SERVER */

/* Make sure no duplicates, no fast forward, or other problems
 *
 * ssl   The SSL/TLS object.
 * type  Type of handshake message received.
 * returns 0 on success, otherwise failure.
 */
static int SanityCheckTls13MsgReceived(WOLFSSL* ssl, byte type)
{
    /* verify not a duplicate, mark received, check state */
    switch (type) {

#ifndef NO_WOLFSSL_SERVER
        case client_hello:
        #ifndef NO_WOLFSSL_CLIENT
            /* Only valid when received on SERVER side. */
            if (ssl->options.side == WOLFSSL_CLIENT_END) {
                WOLFSSL_MSG("ClientHello received by client");
                WOLFSSL_ERROR_VERBOSE(SIDE_ERROR);
                return SIDE_ERROR;
            }
        #endif
            /* Check state. */
            if (ssl->options.clientState >= CLIENT_HELLO_COMPLETE) {
                WOLFSSL_MSG("ClientHello received out of order");
                WOLFSSL_ERROR_VERBOSE(OUT_OF_ORDER_E);
                return OUT_OF_ORDER_E;
            }
            /* Check previously seen. */
            /* Initial and after HelloRetryRequest - no more than 2. */
            if (ssl->msgsReceived.got_client_hello == 2) {
                WOLFSSL_MSG("Too many ClientHello received");
                WOLFSSL_ERROR_VERBOSE(DUPLICATE_MSG_E);
                return DUPLICATE_MSG_E;
            }
            /* Second only after HelloRetryRequest seen. */
            if (ssl->msgsReceived.got_client_hello == 1 &&
                ssl->options.serverState !=
                                          SERVER_HELLO_RETRY_REQUEST_COMPLETE) {
                WOLFSSL_MSG("Duplicate ClientHello received");
                WOLFSSL_ERROR_VERBOSE(DUPLICATE_MSG_E);
                return DUPLICATE_MSG_E;
            }
            ssl->msgsReceived.got_client_hello++;

            break;
#endif

#ifndef NO_WOLFSSL_CLIENT
        case server_hello:
        #ifndef NO_WOLFSSL_SERVER
            /* Only valid when received on CLIENT side. */
            if (ssl->options.side == WOLFSSL_SERVER_END) {
                WOLFSSL_MSG("ServerHello received by server");
                WOLFSSL_ERROR_VERBOSE(SIDE_ERROR);
                return SIDE_ERROR;
            }
        #endif
            /* Check state. */
            if (ssl->options.serverState >= SERVER_HELLO_COMPLETE) {
                WOLFSSL_MSG("ServerHello received out of order");
                WOLFSSL_ERROR_VERBOSE(OUT_OF_ORDER_E);
                return OUT_OF_ORDER_E;
            }
            /* Check previously seen. */
            /* Only once after ClientHello.
             * HelloRetryRequest has ServerHello type but count fixed up later
             * - see DoTls13ServerHello().
             */
            if (ssl->msgsReceived.got_server_hello) {
                WOLFSSL_MSG("Duplicate ServerHello received");
                WOLFSSL_ERROR_VERBOSE(DUPLICATE_MSG_E);
                return DUPLICATE_MSG_E;
            }
            ssl->msgsReceived.got_server_hello = 1;

            break;
#endif

#ifndef NO_WOLFSSL_CLIENT
        case session_ticket:
        #ifndef NO_WOLFSSL_SERVER
            /* Only valid when received on CLIENT side. */
            if (ssl->options.side == WOLFSSL_SERVER_END) {
                WOLFSSL_MSG("NewSessionTicket received by server");
                WOLFSSL_ERROR_VERBOSE(SIDE_ERROR);
                return SIDE_ERROR;
            }
        #endif
            /* Check state. */
        #ifdef WOLFSSL_TLS13_TICKET_BEFORE_FINISHED
            /* Only allowed after server's Finished message. */
            if (ssl->options.serverState < SERVER_FINISHED_COMPLETE) {
                WOLFSSL_MSG("NewSessionTicket received out of order");
                WOLFSSL_ERROR_VERBOSE(OUT_OF_ORDER_E);
                return OUT_OF_ORDER_E;
            }
        #else
            /* Only allowed after client's Finished message. */
            if (ssl->options.clientState < CLIENT_FINISHED_COMPLETE) {
                WOLFSSL_MSG("NewSessionTicket received out of order");
                WOLFSSL_ERROR_VERBOSE(OUT_OF_ORDER_E);
                return OUT_OF_ORDER_E;
            }
        #endif
            /* Many SessionTickets can be sent. */
            ssl->msgsReceived.got_session_ticket = 1;

            break;
#endif

#ifndef NO_WOLFSSL_SERVER
    #ifdef WOLFSSL_EARLY_DATA
        case end_of_early_data:
        #ifndef NO_WOLFSSL_CLIENT
            /* Only valid when received on SERVER side. */
            if (ssl->options.side == WOLFSSL_CLIENT_END) {
                WOLFSSL_MSG("EndOfEarlyData received by client");
                WOLFSSL_ERROR_VERBOSE(SIDE_ERROR);
                return SIDE_ERROR;
            }
        #endif
            /* Check state. */
            /* Only after server's Finished and before client's Finished. */
            if (ssl->options.serverState < SERVER_FINISHED_COMPLETE) {
                WOLFSSL_MSG("EndOfEarlyData received out of order");
                WOLFSSL_ERROR_VERBOSE(OUT_OF_ORDER_E);
                return OUT_OF_ORDER_E;
            }
            if (ssl->options.clientState >= CLIENT_FINISHED_COMPLETE) {
                WOLFSSL_MSG("EndOfEarlyData received out of order");
                WOLFSSL_ERROR_VERBOSE(OUT_OF_ORDER_E);
                return OUT_OF_ORDER_E;
            }
            /* Check previously seen. */
            if (ssl->msgsReceived.got_end_of_early_data) {
                WOLFSSL_MSG("Too many EndOfEarlyData received");
                WOLFSSL_ERROR_VERBOSE(DUPLICATE_MSG_E);
                return DUPLICATE_MSG_E;
            }
            ssl->msgsReceived.got_end_of_early_data = 1;

            break;
    #endif
#endif

#ifndef NO_WOLFSSL_CLIENT
        case encrypted_extensions:
        #ifndef NO_WOLFSSL_SERVER
            /* Only valid when received on CLIENT side. */
            if (ssl->options.side == WOLFSSL_SERVER_END) {
                WOLFSSL_MSG("EncryptedExtensions received by server");
                WOLFSSL_ERROR_VERBOSE(SIDE_ERROR);
                return SIDE_ERROR;
            }
        #endif
            /* Check state. */
            /* Must be received directly after ServerHello.
             * DoTls13EncryptedExtensions() changes state to:
             *   SERVER_ENCRYPTED_EXTENSIONS_COMPLETE.
             */
            if (ssl->options.serverState != SERVER_HELLO_COMPLETE) {
                WOLFSSL_MSG("EncryptedExtensions received out of order");
                WOLFSSL_ERROR_VERBOSE(OUT_OF_ORDER_E);
                return OUT_OF_ORDER_E;
            }
            /* Check previously seen. */
            if (ssl->msgsReceived.got_encrypted_extensions) {
                WOLFSSL_MSG("Duplicate EncryptedExtensions received");
                WOLFSSL_ERROR_VERBOSE(DUPLICATE_MSG_E);
                return DUPLICATE_MSG_E;
            }
            ssl->msgsReceived.got_encrypted_extensions = 1;

            break;
#endif

        case certificate:
            /* Valid on both sides. */
    #ifndef NO_WOLFSSL_CLIENT
            /* Check state. */
            /* On client, seen after EncryptedExtension and CertificateRequest
             * (if sent) and before CertificateVerify and Finished.
             * DoTls13Certificate() sets serverState to SERVER_CERT_COMPLETE.
             */
            if (ssl->options.side == WOLFSSL_CLIENT_END &&
                ssl->options.serverState !=
                                         SERVER_ENCRYPTED_EXTENSIONS_COMPLETE) {
                WOLFSSL_MSG("Certificate received out of order - Client");
                WOLFSSL_ERROR_VERBOSE(OUT_OF_ORDER_E);
                return OUT_OF_ORDER_E;
            }
        #if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
            /* Server's authenticating with PSK must not send this. */
            if (ssl->options.side == WOLFSSL_CLIENT_END &&
                             ssl->options.serverState == SERVER_CERT_COMPLETE &&
                             ssl->options.pskNegotiated) {
                WOLFSSL_MSG("Certificate received while using PSK");
                WOLFSSL_ERROR_VERBOSE(SANITY_MSG_E);
                return SANITY_MSG_E;
            }
        #endif
    #endif
    #ifndef NO_WOLFSSL_SERVER
            /* Check state. */
            /* On Server, valid after ClientHello received and ServerFinished
             * sent. */
            if (ssl->options.side == WOLFSSL_SERVER_END &&
                ssl->options.clientState != CLIENT_HELLO_COMPLETE &&
                ssl->options.serverState < SERVER_FINISHED_COMPLETE) {
                WOLFSSL_MSG("Certificate received out of order - Server");
                WOLFSSL_ERROR_VERBOSE(OUT_OF_ORDER_E);
                return OUT_OF_ORDER_E;
            }
    #endif
            /* Check previously seen. */
            if (ssl->msgsReceived.got_certificate) {
                WOLFSSL_MSG("Duplicate Certificate received");
                WOLFSSL_ERROR_VERBOSE(DUPLICATE_MSG_E);
                return DUPLICATE_MSG_E;
            }
            ssl->msgsReceived.got_certificate = 1;

            break;

#ifndef NO_WOLFSSL_CLIENT
        case certificate_request:
        #ifndef NO_WOLFSSL_SERVER
            /* Only valid when received on CLIENT side. */
            if (ssl->options.side == WOLFSSL_SERVER_END) {
                WOLFSSL_MSG("CertificateRequest received by server");
                WOLFSSL_ERROR_VERBOSE(SIDE_ERROR);
                return SIDE_ERROR;
            }
        #endif
            /* Check state. */
        #ifndef WOLFSSL_POST_HANDSHAKE_AUTH
            /* Only valid when sent after EncryptedExtensions and before
             * Certificate. */
            if (ssl->options.serverState !=
                                         SERVER_ENCRYPTED_EXTENSIONS_COMPLETE) {
                WOLFSSL_MSG("CertificateRequest received out of order");
                WOLFSSL_ERROR_VERBOSE(OUT_OF_ORDER_E);
                return OUT_OF_ORDER_E;
            }
        #else
            /* Valid when sent after EncryptedExtensions and before Certificate
             * and after both client and server have sent Finished (Post
             * Handshake Authentication). */
            if (ssl->options.serverState !=
                                         SERVER_ENCRYPTED_EXTENSIONS_COMPLETE &&
                       (ssl->options.serverState < SERVER_FINISHED_COMPLETE ||
                        ssl->options.clientState != CLIENT_FINISHED_COMPLETE)) {
                WOLFSSL_MSG("CertificateRequest received out of order");
                WOLFSSL_ERROR_VERBOSE(OUT_OF_ORDER_E);
                return OUT_OF_ORDER_E;
            }
        #endif
        #if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
            /* Server's authenticating with PSK must not send this. */
            if (ssl->options.pskNegotiated) {
                WOLFSSL_MSG("CertificateRequest received while using PSK");
                WOLFSSL_ERROR_VERBOSE(SANITY_MSG_E);
                return SANITY_MSG_E;
            }
        #endif
            /* Check previously seen. */
        #ifndef WOLFSSL_POST_HANDSHAKE_AUTH
            /* Only once during handshake. */
            if (ssl->msgsReceived.got_certificate_request) {
                WOLFSSL_MSG("Duplicate CertificateRequest received");
                WOLFSSL_ERROR_VERBOSE(DUPLICATE_MSG_E);
                return DUPLICATE_MSG_E;
            }
        #else
            /* Only once during handshake. */
            if (ssl->msgsReceived.got_certificate_request &&
                ssl->options.clientState != CLIENT_FINISHED_COMPLETE) {
                WOLFSSL_MSG("Duplicate CertificateRequest received");
                WOLFSSL_ERROR_VERBOSE(DUPLICATE_MSG_E);
                return DUPLICATE_MSG_E;
            }
        #endif
            ssl->msgsReceived.got_certificate_request = 1;

            break;
#endif

        case certificate_verify:
            /* Valid on both sides. */
    #ifndef NO_WOLFSSL_CLIENT
            /* Check state on client.
             * Valid only directly after a Certificate message. */
            if (ssl->options.side == WOLFSSL_CLIENT_END) {
                if (ssl->options.serverState != SERVER_CERT_COMPLETE) {
                    WOLFSSL_MSG("No Cert before CertVerify");
                    WOLFSSL_ERROR_VERBOSE(OUT_OF_ORDER_E);
                    return OUT_OF_ORDER_E;
                }
            #if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
                /* Server's authenticating with PSK must not send this. */
                if (ssl->options.pskNegotiated) {
                    WOLFSSL_MSG("CertificateVerify received while using PSK");
                    WOLFSSL_ERROR_VERBOSE(SANITY_MSG_E);
                    return SANITY_MSG_E;
                }
            #endif
            }
    #endif
    #ifndef NO_WOLFSSL_SERVER
            /* Check state on server. */
            if (ssl->options.side == WOLFSSL_SERVER_END) {
                /* Server must have sent Finished message. */
                if (ssl->options.serverState < SERVER_FINISHED_COMPLETE) {
                    WOLFSSL_MSG("CertificateVerify received out of order");
                    WOLFSSL_ERROR_VERBOSE(OUT_OF_ORDER_E);
                    return OUT_OF_ORDER_E;
                }
                /* Valid only directly after a Certificate message. */
                if (ssl->options.clientState < CLIENT_HELLO_COMPLETE) {
                    WOLFSSL_MSG("CertificateVerify before ClientHello done");
                    WOLFSSL_ERROR_VERBOSE(OUT_OF_ORDER_E);
                    return OUT_OF_ORDER_E;
                }
                if (!ssl->msgsReceived.got_certificate) {
                    WOLFSSL_MSG("No Cert before CertificateVerify");
                    WOLFSSL_ERROR_VERBOSE(OUT_OF_ORDER_E);
                    return OUT_OF_ORDER_E;
                }
            }
    #endif
            /* Check previously seen. */
            if (ssl->msgsReceived.got_certificate_verify) {
                WOLFSSL_MSG("Duplicate CertificateVerify received");
                WOLFSSL_ERROR_VERBOSE(DUPLICATE_MSG_E);
                return DUPLICATE_MSG_E;
            }
            ssl->msgsReceived.got_certificate_verify = 1;

            break;

        case finished:
            /* Valid on both sides. */
        #ifndef NO_WOLFSSL_CLIENT
            /* Check state on client. */
            if (ssl->options.side == WOLFSSL_CLIENT_END) {
                /* After sending ClientHello */
                if (ssl->options.clientState < CLIENT_HELLO_COMPLETE) {
                    WOLFSSL_MSG("Finished received out of order - clientState");
                    WOLFSSL_ERROR_VERBOSE(OUT_OF_ORDER_E);
                    return OUT_OF_ORDER_E;
                }
                /* Must have seen certificate and verify from server except when
                 * using PSK. */
            #if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
                if (ssl->options.pskNegotiated) {
                    if (ssl->options.serverState !=
                                         SERVER_ENCRYPTED_EXTENSIONS_COMPLETE) {
                        WOLFSSL_MSG("Finished received out of order - PSK");
                        WOLFSSL_ERROR_VERBOSE(OUT_OF_ORDER_E);
                        return OUT_OF_ORDER_E;
                    }
                }
                else
            #endif
                if (ssl->options.serverState != SERVER_CERT_VERIFY_COMPLETE) {
                    WOLFSSL_MSG("Finished received out of order - serverState");
                    WOLFSSL_ERROR_VERBOSE(OUT_OF_ORDER_E);
                    return OUT_OF_ORDER_E;
                }
            }
        #endif
        #ifndef NO_WOLFSSL_SERVER
            /* Check state on server. */
            if (ssl->options.side == WOLFSSL_SERVER_END) {
                if (ssl->options.serverState < SERVER_FINISHED_COMPLETE) {
                    WOLFSSL_MSG("Finished received out of order - serverState");
                    WOLFSSL_ERROR_VERBOSE(OUT_OF_ORDER_E);
                    return OUT_OF_ORDER_E;
                }
                if (ssl->options.clientState < CLIENT_HELLO_COMPLETE) {
                    WOLFSSL_MSG("Finished received out of order - clientState");
                    WOLFSSL_ERROR_VERBOSE(OUT_OF_ORDER_E);
                    return OUT_OF_ORDER_E;
                }
            #ifdef WOLFSSL_EARLY_DATA
                if (ssl->earlyData == process_early_data &&
                    /* early data may be lost when using DTLS */
                    !ssl->options.dtls
                    /* QUIC does not use EndOfEarlyData records */
                    && !WOLFSSL_IS_QUIC(ssl)) {
                    WOLFSSL_ERROR_VERBOSE(OUT_OF_ORDER_E);
                    return OUT_OF_ORDER_E;
                }
            #endif
            }
        #endif
        #if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
            if (!ssl->options.pskNegotiated)
        #endif
            {
                /* Must have received a Certificate message from client if
                 * verifying the peer. Empty certificate message indicates
                 * no certificate available.
                 */
                if (ssl->options.verifyPeer &&
                #ifdef WOLFSSL_POST_HANDSHAKE_AUTH
                    !ssl->options.verifyPostHandshake &&
                #endif
                                           !ssl->msgsReceived.got_certificate) {
                    WOLFSSL_MSG("Finished received out of order - "
                                "missing Certificate message");
                    WOLFSSL_ERROR_VERBOSE(OUT_OF_ORDER_E);
                    return OUT_OF_ORDER_E;
                }
                /* Mutual authentication on server requires a certificate from
                 * peer. Verify peer set on client side requires a certificate
                 * from peer as not doing PSK.
                 */
                if ((ssl->options.mutualAuth ||
                    (ssl->options.side == WOLFSSL_CLIENT_END &&
                     ssl->options.verifyPeer)) && !ssl->options.havePeerCert) {
                    WOLFSSL_MSG("Finished received out of order - "
                                "no valid certificate");
                    WOLFSSL_ERROR_VERBOSE(OUT_OF_ORDER_E);
                    return OUT_OF_ORDER_E;
                }
                /* Must have received a valid CertificateVerify if verifying
                 * peer and got a peer certificate.
                 */
                if ((ssl->options.mutualAuth || ssl->options.verifyPeer) &&
                    ssl->options.havePeerCert && !ssl->options.havePeerVerify) {
                    WOLFSSL_MSG("Finished received out of order - "
                                "Certificate message but no CertificateVerify");
                    WOLFSSL_ERROR_VERBOSE(OUT_OF_ORDER_E);
                    return OUT_OF_ORDER_E;
                }
            }
            /* Check previously seen. */
            if (ssl->msgsReceived.got_finished) {
                WOLFSSL_MSG("Duplicate Finished received");
                WOLFSSL_ERROR_VERBOSE(DUPLICATE_MSG_E);
                return DUPLICATE_MSG_E;
            }
            ssl->msgsReceived.got_finished = 1;

            break;

        case key_update:
            /* Valid on both sides. */
            /* Check state.
             * Client and server must have received finished message from other
             * side.
             */
            if (!ssl->msgsReceived.got_finished) {
                WOLFSSL_MSG("No KeyUpdate before Finished");
                WOLFSSL_ERROR_VERBOSE(OUT_OF_ORDER_E);
                return OUT_OF_ORDER_E;
            }
            /* Multiple KeyUpdates can be sent. */
            break;
#if defined(WOLFSSL_DTLS13) && !defined(WOLFSSL_NO_TLS12)
        case hello_verify_request:
            if (!ssl->options.dtls) {
                WOLFSSL_MSG("HelloVerifyRequest when not in DTLS");
                WOLFSSL_ERROR_VERBOSE(OUT_OF_ORDER_E);
                return OUT_OF_ORDER_E;
            }
            if (ssl->msgsReceived.got_hello_verify_request) {
                WOLFSSL_MSG("Duplicate HelloVerifyRequest received");
                WOLFSSL_ERROR_VERBOSE(DUPLICATE_MSG_E);
                return DUPLICATE_MSG_E;
            }
            ssl->msgsReceived.got_hello_verify_request = 1;
            if (ssl->msgsReceived.got_hello_retry_request) {
                WOLFSSL_MSG(
                    "Both HelloVerifyRequest and HelloRetryRequest received");
                WOLFSSL_ERROR_VERBOSE(DUPLICATE_MSG_E);
                return DUPLICATE_MSG_E;
            }
            if (ssl->options.serverState >=
                    SERVER_HELLO_RETRY_REQUEST_COMPLETE ||
                ssl->options.connectState != CLIENT_HELLO_SENT) {
                WOLFSSL_MSG("HelloVerifyRequest received out of order");
                WOLFSSL_ERROR_VERBOSE(OUT_OF_ORDER_E);
                return OUT_OF_ORDER_E;
            }
            if (ssl->options.side == WOLFSSL_SERVER_END) {
                WOLFSSL_MSG("HelloVerifyRequest received on the server");
                WOLFSSL_ERROR_VERBOSE(SIDE_ERROR);
                return SIDE_ERROR;
            }
            if (!ssl->options.downgrade ||
                ssl->options.minDowngrade < DTLSv1_2_MINOR) {
                WOLFSSL_MSG(
                    "HelloVerifyRequest received but not DTLSv1.2 allowed");
                WOLFSSL_ERROR_VERBOSE(VERSION_ERROR);
                return VERSION_ERROR;
            }
            break;
#endif /* WOLFSSL_DTLS13 && !WOLFSSL_NO_TLS12*/

        default:
            WOLFSSL_MSG("Unknown message type");
            WOLFSSL_ERROR_VERBOSE(SANITY_MSG_E);
            return SANITY_MSG_E;
    }

    return 0;
}

/* Handle a type of handshake message that has been received.
 *
 * ssl       The SSL/TLS object.
 * input     The message buffer.
 * inOutIdx  On entry, the index into the buffer of the current message.
 *           On exit, the index into the buffer of the next message.
 * size      The length of the current handshake message.
 * totalSz   Length of remaining data in the message buffer.
 * returns 0 on success and otherwise failure.
 */
int DoTls13HandShakeMsgType(WOLFSSL* ssl, byte* input, word32* inOutIdx,
                            byte type, word32 size, word32 totalSz)
{
    int ret = 0, tmp;
    word32 inIdx = *inOutIdx;
    int alertType = invalid_alert;
#if defined(HAVE_ECH)
    TLSX* echX = NULL;
    word32 echInOutIdx;
#endif

    (void)totalSz;

    WOLFSSL_ENTER("DoTls13HandShakeMsgType");

    /* make sure we can read the message */
    if (*inOutIdx + size > totalSz)
        return INCOMPLETE_DATA;

    /* sanity check msg received */
    if ((ret = SanityCheckTls13MsgReceived(ssl, type)) != 0) {
        WOLFSSL_MSG("Sanity Check on handshake message type received failed");
        if (ret == VERSION_ERROR)
            SendAlert(ssl, alert_fatal, wolfssl_alert_protocol_version);
        else
            SendAlert(ssl, alert_fatal, unexpected_message);
        return ret;
    }

#if defined(WOLFSSL_CALLBACKS)
    /* add name later, add on record and handshake header part back on */
    if (ssl->toInfoOn) {
        ret = AddPacketInfo(ssl, 0, handshake, input + *inOutIdx -
            HANDSHAKE_HEADER_SZ, size + HANDSHAKE_HEADER_SZ, READ_PROTO,
            RECORD_HEADER_SZ, ssl->heap);
        if (ret != 0)
            return ret;
        AddLateRecordHeader(&ssl->curRL, &ssl->timeoutInfo);
    }
#endif

    if (ssl->options.handShakeState == HANDSHAKE_DONE &&
            type != session_ticket && type != certificate_request &&
            type != certificate && type != key_update && type != finished) {
        WOLFSSL_MSG("HandShake message after handshake complete");
        SendAlert(ssl, alert_fatal, unexpected_message);
        WOLFSSL_ERROR_VERBOSE(OUT_OF_ORDER_E);
        return OUT_OF_ORDER_E;
    }

    if (ssl->options.side == WOLFSSL_CLIENT_END &&
               ssl->options.serverState == NULL_STATE &&
               type != server_hello && type != hello_retry_request
#if defined(WOLFSSL_DTLS13) && !defined(WOLFSSL_NO_TLS12)
        && (!ssl->options.dtls || type != hello_verify_request)
#endif /* defined(WOLFSSL_DTLS13) && !defined(WOLFSSL_NO_TLS12) */
        ) {
        WOLFSSL_MSG("First server message not server hello");
        SendAlert(ssl, alert_fatal, unexpected_message);
        WOLFSSL_ERROR_VERBOSE(OUT_OF_ORDER_E);
        return OUT_OF_ORDER_E;
    }

    if (ssl->options.side == WOLFSSL_SERVER_END &&
               ssl->options.clientState == NULL_STATE && type != client_hello) {
        WOLFSSL_MSG("First client message not client hello");
        SendAlert(ssl, alert_fatal, unexpected_message);
        WOLFSSL_ERROR_VERBOSE(OUT_OF_ORDER_E);
        return OUT_OF_ORDER_E;
    }

    /* above checks handshake state */
    switch (type) {
#ifndef NO_WOLFSSL_CLIENT
    /* Messages only received by client. */
    case server_hello:
        WOLFSSL_MSG("processing server hello");
        ret = DoTls13ServerHello(ssl, input, inOutIdx, size, &type);
    #if !defined(WOLFSSL_NO_CLIENT_AUTH) && \
               ((defined(HAVE_ED25519) && !defined(NO_ED25519_CLIENT_AUTH)) || \
                (defined(HAVE_ED448) && !defined(NO_ED448_CLIENT_AUTH)))
        if (ssl->options.resuming || !IsAtLeastTLSv1_2(ssl) ||
                                               IsAtLeastTLSv1_3(ssl->version)) {
            ssl->options.cacheMessages = 0;
            if ((ssl->hsHashes != NULL) && (ssl->hsHashes->messages != NULL)) {
                ForceZero(ssl->hsHashes->messages, ssl->hsHashes->length);
                XFREE(ssl->hsHashes->messages, ssl->heap, DYNAMIC_TYPE_HASHES);
                ssl->hsHashes->messages = NULL;
            }
        }
    #endif
        break;

    case encrypted_extensions:
        WOLFSSL_MSG("processing encrypted extensions");
        ret = DoTls13EncryptedExtensions(ssl, input, inOutIdx, size);
        break;

    #ifndef NO_CERTS
    case certificate_request:
        WOLFSSL_MSG("processing certificate request");
        ret = DoTls13CertificateRequest(ssl, input, inOutIdx, size);
        break;
    #endif

    case session_ticket:
        WOLFSSL_MSG("processing new session ticket");
        ret = DoTls13NewSessionTicket(ssl, input, inOutIdx, size);
        break;
#endif /* !NO_WOLFSSL_CLIENT */

#ifndef NO_WOLFSSL_SERVER
    /* Messages only received by server. */
    case client_hello:
        WOLFSSL_MSG("processing client hello");
#if defined(HAVE_ECH)
        /* keep the start idx so we can restore it for the inner call */
        echInOutIdx = *inOutIdx;
#endif
        ret = DoTls13ClientHello(ssl, input, inOutIdx, size);
    #if !defined(WOLFSSL_NO_CLIENT_AUTH) && \
               ((defined(HAVE_ED25519) && !defined(NO_ED25519_CLIENT_AUTH)) || \
                (defined(HAVE_ED448) && !defined(NO_ED448_CLIENT_AUTH)))
        if ((ssl->options.resuming || !ssl->options.verifyPeer ||
               !IsAtLeastTLSv1_2(ssl) || IsAtLeastTLSv1_3(ssl->version))
        #ifdef WOLFSSL_DTLS13
               && (!ssl->options.dtls)
        #endif
               ) {
        #if defined(WOLFSSL_ASYNC_CRYPT) || defined(WOLFSSL_NONBLOCK_OCSP)
            if (ret != WC_PENDING_E && ret != OCSP_WANT_READ)
        #endif
            {
                ssl->options.cacheMessages = 0;
                if ((ssl->hsHashes != NULL) &&
                        (ssl->hsHashes->messages != NULL)) {
                    ForceZero(ssl->hsHashes->messages, ssl->hsHashes->length);
                    XFREE(ssl->hsHashes->messages, ssl->heap,
                        DYNAMIC_TYPE_HASHES);
                    ssl->hsHashes->messages = NULL;
                }
            }
        }
    #endif
#if defined(HAVE_ECH)
        if (ret == 0) {
            echX = TLSX_Find(ssl->extensions, TLSX_ECH);

            if (echX != NULL &&
                ((WOLFSSL_ECH*)echX->data)->state == ECH_WRITE_NONE) {

                /* reset the inOutIdx to the outer start */
                *inOutIdx = echInOutIdx;

                /* call again with the inner hello */
                ret = DoTls13ClientHello(ssl,
                    ((WOLFSSL_ECH*)echX->data)->innerClientHello,
                    &echInOutIdx,
                    ((WOLFSSL_ECH*)echX->data)->innerClientHelloLen);

                /* if the inner ech parsed successfully we have successfully
                 * handled the hello and can skip the whole message */
                if (ret == 0)
                    *inOutIdx += size;
            }
        }
#endif /* HAVE_ECH */
        break;

    #ifdef WOLFSSL_EARLY_DATA
    case end_of_early_data:
        WOLFSSL_MSG("processing end of early data");
        ret = DoTls13EndOfEarlyData(ssl, input, inOutIdx, size);
        break;
    #endif
#endif /* !NO_WOLFSSL_SERVER */

    /* Messages received by both client and server. */
#if !defined(NO_CERTS) && (!defined(NO_WOLFSSL_CLIENT) || \
                           !defined(WOLFSSL_NO_CLIENT_AUTH))
    case certificate:
        WOLFSSL_MSG("processing certificate");
        ret = DoTls13Certificate(ssl, input, inOutIdx, size);
        break;
#endif

#if !defined(NO_RSA) || defined(HAVE_ECC) || defined(HAVE_ED25519) || \
    defined(HAVE_ED448) || defined(HAVE_PQC)
    case certificate_verify:
        WOLFSSL_MSG("processing certificate verify");
        ret = DoTls13CertificateVerify(ssl, input, inOutIdx, size);
        break;
#endif
    case finished:
        WOLFSSL_MSG("processing finished");
        ret = DoTls13Finished(ssl, input, inOutIdx, size, totalSz, NO_SNIFF);
        break;

    case key_update:
        WOLFSSL_MSG("processing key update");
        ret = DoTls13KeyUpdate(ssl, input, inOutIdx, size);
        break;

#if defined(WOLFSSL_DTLS13) && !defined(WOLFSSL_NO_TLS12) && \
    !defined(NO_WOLFSSL_CLIENT)
    case hello_verify_request:
        WOLFSSL_MSG("processing hello verify request");
        ret = DoHelloVerifyRequest(ssl, input, inOutIdx, size);
        break;
#endif
    default:
        WOLFSSL_MSG("Unknown handshake message type");
        ret = UNKNOWN_HANDSHAKE_TYPE;
        break;
    }

#if defined(WOLFSSL_ASYNC_CRYPT) || defined(WOLFSSL_ASYNC_IO)
    /* if async, offset index so this msg will be processed again */
    /* NOTE: check this now before other calls can overwrite ret */
    if ((ret == WC_PENDING_E || ret == OCSP_WANT_READ) && *inOutIdx > 0) {
        /* DTLS always stores a message in a buffer when async is enable, so we
         * don't need to adjust for the extra bytes here (*inOutIdx is always
         * == 0) */
        *inOutIdx -= HANDSHAKE_HEADER_SZ;
    }

    /* make sure async error is cleared */
    if (ret == 0 && (ssl->error == WC_PENDING_E || ssl->error == OCSP_WANT_READ)) {
        ssl->error = 0;
    }
#endif
    if (ret == 0 && type != client_hello && type != session_ticket &&
                                                           type != key_update) {
        ret = HashInput(ssl, input + inIdx, size);
    }

    alertType = TranslateErrorToAlert(ret);

    if (alertType != invalid_alert) {
#ifdef WOLFSSL_DTLS13
        if (type == client_hello && ssl->options.dtls)
            DtlsSetSeqNumForReply(ssl);
#endif
        tmp = SendAlert(ssl, alert_fatal, alertType);
        /* propagate socket error instead of tls error to be sure the error is
         * not ignored by DTLS code */
        if (tmp == SOCKET_ERROR_E)
            ret = SOCKET_ERROR_E;
    }

    if (ret == 0 && ssl->options.tls1_3) {
        /* Need to hash input message before deriving secrets. */
    #ifndef NO_WOLFSSL_CLIENT
        if (ssl->options.side == WOLFSSL_CLIENT_END) {
            if (type == server_hello) {
                if ((ret = DeriveEarlySecret(ssl)) != 0)
                    return ret;
                if ((ret = DeriveHandshakeSecret(ssl)) != 0)
                    return ret;

                if ((ret = DeriveTls13Keys(ssl, handshake_key,
                                        ENCRYPT_AND_DECRYPT_SIDE, 1)) != 0) {
                    return ret;
                }
        #ifdef WOLFSSL_EARLY_DATA
                if (ssl->earlyData != no_early_data) {
                    if ((ret = SetKeysSide(ssl, DECRYPT_SIDE_ONLY)) != 0)
                        return ret;
                }
                else
        #endif
                if ((ret = SetKeysSide(ssl, ENCRYPT_AND_DECRYPT_SIDE)) != 0)
                    return ret;

#ifdef WOLFSSL_DTLS13
                if (ssl->options.dtls) {
                    w64wrapper epochHandshake;
                    epochHandshake = w64From32(0, DTLS13_EPOCH_HANDSHAKE);
                    ssl->dtls13Epoch = epochHandshake;
                    ssl->dtls13PeerEpoch = epochHandshake;

                    ret = Dtls13NewEpoch(
                        ssl, epochHandshake, ENCRYPT_AND_DECRYPT_SIDE);
                    if (ret != 0)
                        return ret;

                    ret = Dtls13SetEpochKeys(
                        ssl, epochHandshake, ENCRYPT_AND_DECRYPT_SIDE);
                    if (ret != 0)
                        return ret;

                }
#endif /* WOLFSSL_DTLS13 */
            }

            if (type == finished) {
                if ((ret = DeriveMasterSecret(ssl)) != 0)
                    return ret;
                /* Last use of preMasterSecret - zeroize as soon as possible. */
                ForceZero(ssl->arrays->preMasterSecret,
                    ssl->arrays->preMasterSz);
        #ifdef WOLFSSL_EARLY_DATA
        #ifdef WOLFSSL_QUIC
                if (WOLFSSL_IS_QUIC(ssl) && ssl->earlyData != no_early_data) {
                    /* QUIC never sends/receives EndOfEarlyData, but having
                     * early data means the last encrpytion keys had not been
                     * set yet. */
                    if ((ret = SetKeysSide(ssl, ENCRYPT_SIDE_ONLY)) != 0)
                        return ret;
                }
        #endif
                if ((ret = DeriveTls13Keys(ssl, traffic_key,
                                    ENCRYPT_AND_DECRYPT_SIDE,
                                    ssl->earlyData == no_early_data)) != 0) {
                    return ret;
                }
        #else
                if ((ret = DeriveTls13Keys(ssl, traffic_key,
                                        ENCRYPT_AND_DECRYPT_SIDE, 1)) != 0) {
                    return ret;
                }
        #endif
            }
        #ifdef WOLFSSL_POST_HANDSHAKE_AUTH
            if (type == certificate_request &&
                                ssl->options.handShakeState == HANDSHAKE_DONE) {
                /* reset handshake states */
                ssl->options.clientState = CLIENT_HELLO_COMPLETE;
                ssl->options.connectState  = FIRST_REPLY_DONE;
                ssl->options.handShakeState = CLIENT_HELLO_COMPLETE;
                ssl->options.processReply = 0; /* doProcessInit */

                /*
                   DTLSv1.3 note: We can't reset serverState to
                   SERVER_FINISHED_COMPLETE with the goal that this connect
                   blocks until the cert/cert_verify/finished flight gets ACKed
                   by the server. The problem is that we will invoke
                   ProcessReplyEx() in that case, but we came here from
                   ProcessReplyEx() and it is not re-entrant safe (the input
                   buffer would still have the certificate_request message). */

                if (wolfSSL_connect_TLSv13(ssl) != WOLFSSL_SUCCESS) {
                    ret = ssl->error;
                    if (ret != WC_PENDING_E)
                        ret = POST_HAND_AUTH_ERROR;
                }
            }
        #endif
        }
    #endif /* NO_WOLFSSL_CLIENT */

#ifndef NO_WOLFSSL_SERVER
    #if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
        if (ssl->options.side == WOLFSSL_SERVER_END && type == finished) {
            ret = DeriveResumptionSecret(ssl, ssl->session->masterSecret);
            if (ret != 0)
                return ret;
        }
    #endif
#endif /* NO_WOLFSSL_SERVER */
    }

#ifdef WOLFSSL_DTLS13
    if (ssl->options.dtls && !ssl->options.dtlsStateful) {
        DtlsResetState(ssl);
        if (DtlsIgnoreError(ret))
            ret = 0;
    }
#endif

    WOLFSSL_LEAVE("DoTls13HandShakeMsgType()", ret);
    return ret;
}


/* Handle a handshake message that has been received.
 *
 * ssl       The SSL/TLS object.
 * input     The message buffer.
 * inOutIdx  On entry, the index into the buffer of the current message.
 *           On exit, the index into the buffer of the next message.
 * totalSz   Length of remaining data in the message buffer.
 * returns 0 on success and otherwise failure.
 */
int DoTls13HandShakeMsg(WOLFSSL* ssl, byte* input, word32* inOutIdx,
                        word32 totalSz)
{
    int    ret = 0;
    word32 inputLength;
    byte   type;
    word32 size = 0;

    WOLFSSL_ENTER("DoTls13HandShakeMsg");

    if (ssl->arrays == NULL) {


        if (GetHandshakeHeader(ssl, input, inOutIdx, &type, &size,
                                                                totalSz) != 0) {
            SendAlert(ssl, alert_fatal, unexpected_message);
            WOLFSSL_ERROR_VERBOSE(PARSE_ERROR);
            return PARSE_ERROR;
        }

        return DoTls13HandShakeMsgType(ssl, input, inOutIdx, type, size,
                                       totalSz);
    }

    inputLength = ssl->buffers.inputBuffer.length - *inOutIdx - ssl->keys.padSz;

    /* If there is a pending fragmented handshake message,
     * pending message size will be non-zero. */
    if (ssl->arrays->pendingMsgSz == 0) {

        if (GetHandshakeHeader(ssl, input, inOutIdx, &type, &size,
                               totalSz) != 0) {
            WOLFSSL_ERROR_VERBOSE(PARSE_ERROR);
            return PARSE_ERROR;
        }

        /* Cap the maximum size of a handshake message to something reasonable.
         * By default is the maximum size of a certificate message assuming
         * nine 2048-bit RSA certificates in the chain. */
        if (size > MAX_HANDSHAKE_SZ) {
            WOLFSSL_MSG("Handshake message too large");
            WOLFSSL_ERROR_VERBOSE(HANDSHAKE_SIZE_ERROR);
            return HANDSHAKE_SIZE_ERROR;
        }

        /* size is the size of the certificate message payload */
        if (inputLength - HANDSHAKE_HEADER_SZ < size) {
            ssl->arrays->pendingMsgType = type;
            ssl->arrays->pendingMsgSz = size + HANDSHAKE_HEADER_SZ;
            ssl->arrays->pendingMsg = (byte*)XMALLOC(size + HANDSHAKE_HEADER_SZ,
                                                     ssl->heap,
                                                     DYNAMIC_TYPE_ARRAYS);
            if (ssl->arrays->pendingMsg == NULL)
                return MEMORY_E;
            XMEMCPY(ssl->arrays->pendingMsg,
                    input + *inOutIdx - HANDSHAKE_HEADER_SZ,
                    inputLength);
            ssl->arrays->pendingMsgOffset = inputLength;
            *inOutIdx += inputLength + ssl->keys.padSz - HANDSHAKE_HEADER_SZ;
            return 0;
        }

        ret = DoTls13HandShakeMsgType(ssl, input, inOutIdx, type, size,
                                      totalSz);
    }
    else {
        if (inputLength + ssl->arrays->pendingMsgOffset >
                                                    ssl->arrays->pendingMsgSz) {
            inputLength = ssl->arrays->pendingMsgSz -
                                                  ssl->arrays->pendingMsgOffset;
        }
        XMEMCPY(ssl->arrays->pendingMsg + ssl->arrays->pendingMsgOffset,
                input + *inOutIdx, inputLength);
        ssl->arrays->pendingMsgOffset += inputLength;
        *inOutIdx += inputLength + ssl->keys.padSz;

        if (ssl->arrays->pendingMsgOffset == ssl->arrays->pendingMsgSz)
        {
            word32 idx = 0;
            ret = DoTls13HandShakeMsgType(ssl,
                                ssl->arrays->pendingMsg + HANDSHAKE_HEADER_SZ,
                                &idx, ssl->arrays->pendingMsgType,
                                ssl->arrays->pendingMsgSz - HANDSHAKE_HEADER_SZ,
                                ssl->arrays->pendingMsgSz);
        #ifdef WOLFSSL_ASYNC_CRYPT
            if (ret == WC_PENDING_E) {
                /* setup to process fragment again */
                ssl->arrays->pendingMsgOffset -= inputLength;
                *inOutIdx -= inputLength + ssl->keys.padSz;
            }
            else
        #endif
            {
                XFREE(ssl->arrays->pendingMsg, ssl->heap, DYNAMIC_TYPE_ARRAYS);
                ssl->arrays->pendingMsg = NULL;
                ssl->arrays->pendingMsgSz = 0;
            }
        }
    }

    WOLFSSL_LEAVE("DoTls13HandShakeMsg", ret);
    return ret;
}

#ifndef NO_WOLFSSL_CLIENT

/* The client connecting to the server.
 * The protocol version is expecting to be TLS v1.3.
 * If the server downgrades, and older versions of the protocol are compiled
 * in, the client will fallback to wolfSSL_connect().
 * Please see note at top of README if you get an error from connect.
 *
 * ssl  The SSL/TLS object.
 * returns WOLFSSL_SUCCESS on successful handshake, WOLFSSL_FATAL_ERROR when
 * unrecoverable error occurs and 0 otherwise.
 * For more error information use wolfSSL_get_error().
 */
int wolfSSL_connect_TLSv13(WOLFSSL* ssl)
{
    int advanceState;
    int ret = 0;

    WOLFSSL_ENTER("wolfSSL_connect_TLSv13");

#ifdef HAVE_ERRNO_H
    errno = 0;
#endif

    if (ssl == NULL)
        return BAD_FUNC_ARG;

    if (ssl->options.side != WOLFSSL_CLIENT_END) {
        ssl->error = SIDE_ERROR;
        WOLFSSL_ERROR(ssl->error);
        return WOLFSSL_FATAL_ERROR;
    }

    /* make sure this wolfSSL object has arrays and rng setup. Protects
     * case where the WOLFSSL object is re-used via wolfSSL_clear() */
    if ((ret = ReinitSSL(ssl, ssl->ctx, 0)) != 0) {
        return ret;
    }

#ifdef WOLFSSL_DTLS
    if (ssl->version.major == DTLS_MAJOR) {
        ssl->options.dtls   = 1;
        ssl->options.dtlsStateful = 1;
    }
#endif

#ifdef WOLFSSL_WOLFSENTRY_HOOKS
    if ((ssl->ConnectFilter != NULL) &&
        (ssl->options.connectState == CONNECT_BEGIN))
    {
        wolfSSL_netfilter_decision_t res;
        if ((ssl->ConnectFilter(ssl, ssl->ConnectFilter_arg, &res) ==
             WOLFSSL_SUCCESS) &&
            (res == WOLFSSL_NETFILTER_REJECT)) {
            ssl->error = SOCKET_FILTERED_E;
            WOLFSSL_ERROR(ssl->error);
            return WOLFSSL_FATAL_ERROR;
        }
    }
#endif /* WOLFSSL_WOLFSENTRY_HOOKS */

    /* fragOffset is non-zero when sending fragments. On the last
     * fragment, fragOffset is zero again, and the state can be
     * advanced. Also, only advance from states in which we send data */
    advanceState = (ssl->options.connectState == CONNECT_BEGIN ||
            ssl->options.connectState == HELLO_AGAIN ||
            (ssl->options.connectState >= FIRST_REPLY_DONE &&
             ssl->options.connectState <= FIRST_REPLY_FOURTH));

#ifdef WOLFSSL_DTLS13
    if (ssl->options.dtls)
        advanceState = advanceState && !ssl->dtls13SendingFragments
            && !ssl->dtls13SendingAckOrRtx;
#endif /* WOLFSSL_DTLS13 */

    if (ssl->buffers.outputBuffer.length > 0
    #ifdef WOLFSSL_ASYNC_CRYPT
        /* do not send buffered or advance state if last error was an
            async pending operation */
        && ssl->error != WC_PENDING_E
    #endif
    ) {
        if ((ssl->error = SendBuffered(ssl)) == 0) {
            if (ssl->fragOffset == 0 && !ssl->options.buildingMsg) {
                if (advanceState) {
#ifdef WOLFSSL_DTLS13
                    if (ssl->options.dtls && IsAtLeastTLSv1_3(ssl->version) &&
                        ssl->options.connectState == FIRST_REPLY_FOURTH) {
                    /* WAIT_FINISHED_ACK is a state added afterwards, but it
                       can't follow FIRST_REPLY_FOURTH in the enum order. Indeed
                       the value of the enum ConnectState is stored in
                       serialized session. This would make importing serialized
                       session from other wolfSSL version incompatible */
                        ssl->options.connectState = WAIT_FINISHED_ACK;
                    }
                    else
#endif /* WOLFSSL_DTLS13 */
                    {
                        ssl->options.connectState++;
                    }
                    WOLFSSL_MSG("connect state: "
                                "Advanced from last buffered fragment send");
#ifdef WOLFSSL_ASYNC_IO
                    FreeAsyncCtx(ssl, 0);
#endif

                }
            }
            else {
                WOLFSSL_MSG("connect state: "
                            "Not advanced, more fragments to send");
            }
#ifdef WOLFSSL_DTLS13
            if (ssl->options.dtls)
                ssl->dtls13SendingAckOrRtx = 0;
#endif /* WOLFSSL_DTLS13 */

        }
        else {
            ssl->error = ret;
            WOLFSSL_ERROR(ssl->error);
            return WOLFSSL_FATAL_ERROR;
        }
    }

    ret = RetrySendAlert(ssl);
    if (ret != 0) {
        ssl->error = ret;
        WOLFSSL_ERROR(ssl->error);
        return WOLFSSL_FATAL_ERROR;
    }

#ifdef WOLFSSL_DTLS13
    if (ssl->options.dtls && ssl->dtls13SendingFragments) {
        if ((ssl->error = Dtls13FragmentsContinue(ssl)) != 0) {
                WOLFSSL_ERROR(ssl->error);
                return WOLFSSL_FATAL_ERROR;
        }

        /* we sent all the fragments. Advance state. */
        ssl->options.connectState++;
    }
#endif /* WOLFSSL_DTLS13 */

    switch (ssl->options.connectState) {

        case CONNECT_BEGIN:
            /* Always send client hello first. */
            if ((ssl->error = SendTls13ClientHello(ssl)) != 0) {
                WOLFSSL_ERROR(ssl->error);
                return WOLFSSL_FATAL_ERROR;
            }

            ssl->options.connectState = CLIENT_HELLO_SENT;
            WOLFSSL_MSG("connect state: CLIENT_HELLO_SENT");
    #ifdef WOLFSSL_EARLY_DATA
            if (ssl->earlyData != no_early_data) {
        #if defined(WOLFSSL_TLS13_MIDDLEBOX_COMPAT)
                if (!ssl->options.dtls && ssl->options.tls13MiddleBoxCompat) {
                    if ((ssl->error = SendChangeCipher(ssl)) != 0) {
                        WOLFSSL_ERROR(ssl->error);
                        return WOLFSSL_FATAL_ERROR;
                    }
                    ssl->options.sentChangeCipher = 1;
                }
        #endif
            ssl->options.handShakeState = CLIENT_HELLO_COMPLETE;
            return WOLFSSL_SUCCESS;
            }
    #endif
            FALL_THROUGH;

        case CLIENT_HELLO_SENT:
            /* Get the response/s from the server. */
            while (ssl->options.serverState <
                    SERVER_HELLOVERIFYREQUEST_COMPLETE) {
                if ((ssl->error = ProcessReply(ssl)) < 0) {
                        WOLFSSL_ERROR(ssl->error);
                        return WOLFSSL_FATAL_ERROR;
                }

#ifdef WOLFSSL_DTLS13
                if (ssl->options.dtls) {
                    if ((ssl->error = Dtls13DoScheduledWork(ssl)) < 0) {
                        WOLFSSL_ERROR(ssl->error);
                        return WOLFSSL_FATAL_ERROR;
                    }
                }
#endif /* WOLFSSL_DTLS13 */
            }

            if (!ssl->options.tls1_3) {
    #ifndef WOLFSSL_NO_TLS12
                if (ssl->options.downgrade)
                    return wolfSSL_connect(ssl);
    #endif
                WOLFSSL_MSG("Client using higher version, fatal error");
                WOLFSSL_ERROR_VERBOSE(VERSION_ERROR);
                return VERSION_ERROR;
            }

            ssl->options.connectState = HELLO_AGAIN;
            WOLFSSL_MSG("connect state: HELLO_AGAIN");
            FALL_THROUGH;

        case HELLO_AGAIN:

            if (ssl->options.serverState ==
                                          SERVER_HELLO_RETRY_REQUEST_COMPLETE) {
        #if defined(WOLFSSL_TLS13_MIDDLEBOX_COMPAT)
                if (!ssl->options.dtls && !ssl->options.sentChangeCipher
                    && ssl->options.tls13MiddleBoxCompat) {
                    if ((ssl->error = SendChangeCipher(ssl)) != 0) {
                        WOLFSSL_ERROR(ssl->error);
                        return WOLFSSL_FATAL_ERROR;
                    }
                    ssl->options.sentChangeCipher = 1;
                }
        #endif
                /* Try again with different security parameters. */
                if ((ssl->error = SendTls13ClientHello(ssl)) != 0) {
                    WOLFSSL_ERROR(ssl->error);
                    return WOLFSSL_FATAL_ERROR;
                }
            }

            ssl->options.connectState = HELLO_AGAIN_REPLY;
            WOLFSSL_MSG("connect state: HELLO_AGAIN_REPLY");
            FALL_THROUGH;

        case HELLO_AGAIN_REPLY:
            /* Get the response/s from the server. */
            while (ssl->options.serverState < SERVER_FINISHED_COMPLETE) {
                if ((ssl->error = ProcessReply(ssl)) < 0) {
                        WOLFSSL_ERROR(ssl->error);
                        return WOLFSSL_FATAL_ERROR;
                }

#ifdef WOLFSSL_DTLS13
                if (ssl->options.dtls) {
                    if ((ssl->error = Dtls13DoScheduledWork(ssl)) < 0) {
                        WOLFSSL_ERROR(ssl->error);
                        return WOLFSSL_FATAL_ERROR;
                    }
                }
#endif /* WOLFSSL_DTLS13 */
            }

            ssl->options.connectState = FIRST_REPLY_DONE;
            WOLFSSL_MSG("connect state: FIRST_REPLY_DONE");
            FALL_THROUGH;

        case FIRST_REPLY_DONE:
            if (ssl->options.certOnly)
                return WOLFSSL_SUCCESS;
        #ifdef WOLFSSL_EARLY_DATA
            if (!ssl->options.dtls && ssl->earlyData != no_early_data
                && !WOLFSSL_IS_QUIC(ssl)) {
                if ((ssl->error = SendTls13EndOfEarlyData(ssl)) != 0) {
                    WOLFSSL_ERROR(ssl->error);
                    return WOLFSSL_FATAL_ERROR;
                }
                WOLFSSL_MSG("sent: end_of_early_data");
            }
        #endif

            ssl->options.connectState = FIRST_REPLY_FIRST;
            WOLFSSL_MSG("connect state: FIRST_REPLY_FIRST");
            FALL_THROUGH;

        case FIRST_REPLY_FIRST:
        #if defined(WOLFSSL_TLS13_MIDDLEBOX_COMPAT)
            if (!ssl->options.sentChangeCipher && !ssl->options.dtls
                && ssl->options.tls13MiddleBoxCompat) {
                if ((ssl->error = SendChangeCipher(ssl)) != 0) {
                    WOLFSSL_ERROR(ssl->error);
                    return WOLFSSL_FATAL_ERROR;
                }
                ssl->options.sentChangeCipher = 1;
            }
        #endif

            ssl->options.connectState = FIRST_REPLY_SECOND;
            WOLFSSL_MSG("connect state: FIRST_REPLY_SECOND");
            FALL_THROUGH;

        case FIRST_REPLY_SECOND:
            /* CLIENT: check peer authentication. */
            if (!ssl->options.peerAuthGood) {
                WOLFSSL_MSG("Server authentication did not happen");
                WOLFSSL_ERROR_VERBOSE(WOLFSSL_FATAL_ERROR);
                return WOLFSSL_FATAL_ERROR;
            }
        #ifndef NO_CERTS
            if (!ssl->options.resuming && ssl->options.sendVerify) {
                ssl->error = SendTls13Certificate(ssl);
                if (ssl->error != 0) {
                #ifdef WOLFSSL_CHECK_ALERT_ON_ERR
                    ProcessReplyEx(ssl, 1); /* See if an alert was sent. */
                #endif
                    WOLFSSL_ERROR(ssl->error);
                    return WOLFSSL_FATAL_ERROR;
                }
                WOLFSSL_MSG("sent: certificate");
            }
        #endif

            ssl->options.connectState = FIRST_REPLY_THIRD;
            WOLFSSL_MSG("connect state: FIRST_REPLY_THIRD");
            FALL_THROUGH;

        case FIRST_REPLY_THIRD:
        #if (!defined(NO_CERTS) && (!defined(NO_RSA) || defined(HAVE_ECC) || \
             defined(HAVE_ED25519) || defined(HAVE_ED448) || \
             defined(HAVE_PQC))) && (!defined(NO_WOLFSSL_SERVER) || \
             !defined(WOLFSSL_NO_CLIENT_AUTH))
            if (!ssl->options.resuming && ssl->options.sendVerify) {
                ssl->error = SendTls13CertificateVerify(ssl);
                if (ssl->error != 0) {
                #ifdef WOLFSSL_CHECK_ALERT_ON_ERR
                    ProcessReplyEx(ssl, 1); /* See if an alert was sent. */
                #endif
                    WOLFSSL_ERROR(ssl->error);
                    return WOLFSSL_FATAL_ERROR;
                }
                WOLFSSL_MSG("sent: certificate verify");
            }
        #endif

            ssl->options.connectState = FIRST_REPLY_FOURTH;
            WOLFSSL_MSG("connect state: FIRST_REPLY_FOURTH");
            FALL_THROUGH;

        case FIRST_REPLY_FOURTH:
            if ((ssl->error = SendTls13Finished(ssl)) != 0) {
            #ifdef WOLFSSL_CHECK_ALERT_ON_ERR
                ProcessReplyEx(ssl, 1); /* See if an alert was sent. */
            #endif
                WOLFSSL_ERROR(ssl->error);
                return WOLFSSL_FATAL_ERROR;
            }
            WOLFSSL_MSG("sent: finished");

#ifdef WOLFSSL_DTLS13
            ssl->options.connectState = WAIT_FINISHED_ACK;
            WOLFSSL_MSG("connect state: WAIT_FINISHED_ACK");
            FALL_THROUGH;

        case WAIT_FINISHED_ACK:
            if (ssl->options.dtls) {
                while (ssl->options.serverState != SERVER_FINISHED_ACKED) {
                    if ((ssl->error = ProcessReply(ssl)) < 0) {
                        WOLFSSL_ERROR(ssl->error);
                        return WOLFSSL_FATAL_ERROR;
                    }

                    if ((ssl->error = Dtls13DoScheduledWork(ssl)) < 0) {
                        WOLFSSL_ERROR(ssl->error);
                        return WOLFSSL_FATAL_ERROR;
                    }
                }
            }
#endif /* WOLFSSL_DTLS13 */
            ssl->options.connectState = FINISHED_DONE;
            WOLFSSL_MSG("connect state: FINISHED_DONE");
            FALL_THROUGH;

        case FINISHED_DONE:
        #ifndef NO_HANDSHAKE_DONE_CB
            if (ssl->hsDoneCb != NULL) {
                int cbret = ssl->hsDoneCb(ssl, ssl->hsDoneCtx);
                if (cbret < 0) {
                    ssl->error = cbret;
                    WOLFSSL_ERROR_VERBOSE(ssl->error);
                    WOLFSSL_MSG("HandShake Done Cb don't continue error");
                    return WOLFSSL_FATAL_ERROR;
                }
            }
        #endif /* NO_HANDSHAKE_DONE_CB */

            if (!ssl->options.keepResources) {
                FreeHandshakeResources(ssl);
            }
        #if defined(WOLFSSL_ASYNC_IO) && !defined(WOLFSSL_ASYNC_CRYPT)
            /* Free the remaining async context if not using it for crypto */
            FreeAsyncCtx(ssl, 1);
        #endif

            ssl->error = 0; /* clear the error */

            WOLFSSL_LEAVE("wolfSSL_connect_TLSv13", WOLFSSL_SUCCESS);
            return WOLFSSL_SUCCESS;

        default:
            WOLFSSL_MSG("Unknown connect state ERROR");
            return WOLFSSL_FATAL_ERROR; /* unknown connect state */
    }
}
#endif

#if defined(WOLFSSL_SEND_HRR_COOKIE)
/* Send a cookie with the HelloRetryRequest to avoid storing state.
 *
 * ssl       SSL/TLS object.
 * secret    Secret to use when generating integrity check for cookie.
 *           A value of NULL indicates to generate a new random secret.
 * secretSz  Size of secret data in bytes.
 *           Use a value of 0 to indicate use of default size.
 * returns BAD_FUNC_ARG when ssl is NULL or not using TLS v1.3, SIDE_ERROR when
 * called on a client; WOLFSSL_SUCCESS on success and otherwise failure.
 */
int wolfSSL_send_hrr_cookie(WOLFSSL* ssl, const unsigned char* secret,
                            unsigned int secretSz)
{
    int ret;

    if (ssl == NULL || !IsAtLeastTLSv1_3(ssl->version))
        return BAD_FUNC_ARG;
 #ifndef NO_WOLFSSL_SERVER
    if (ssl->options.side == WOLFSSL_CLIENT_END)
        return SIDE_ERROR;

    if (secretSz == 0) {
    #if !defined(NO_SHA) && defined(NO_SHA256)
        secretSz = WC_SHA_DIGEST_SIZE;
    #endif /* NO_SHA */
    #ifndef NO_SHA256
        secretSz = WC_SHA256_DIGEST_SIZE;
    #endif /* NO_SHA256 */
    }

    if (secretSz != ssl->buffers.tls13CookieSecret.length) {
        byte* newSecret;

        if (ssl->buffers.tls13CookieSecret.buffer != NULL) {
            ForceZero(ssl->buffers.tls13CookieSecret.buffer,
                      ssl->buffers.tls13CookieSecret.length);
            XFREE(ssl->buffers.tls13CookieSecret.buffer,
                  ssl->heap, DYNAMIC_TYPE_COOKIE_PWD);
        }

        newSecret = (byte*)XMALLOC(secretSz, ssl->heap,
                                   DYNAMIC_TYPE_COOKIE_PWD);
        if (newSecret == NULL) {
            ssl->buffers.tls13CookieSecret.buffer = NULL;
            ssl->buffers.tls13CookieSecret.length = 0;
            WOLFSSL_MSG("couldn't allocate new cookie secret");
            return MEMORY_ERROR;
        }
        ssl->buffers.tls13CookieSecret.buffer = newSecret;
        ssl->buffers.tls13CookieSecret.length = secretSz;
    #ifdef WOLFSSL_CHECK_MEM_ZERO
        wc_MemZero_Add("wolfSSL_send_hrr_cookie secret",
            ssl->buffers.tls13CookieSecret.buffer,
            ssl->buffers.tls13CookieSecret.length);
    #endif
    }

    /* If the supplied secret is NULL, randomly generate a new secret. */
    if (secret == NULL) {
        ret = wc_RNG_GenerateBlock(ssl->rng,
                               ssl->buffers.tls13CookieSecret.buffer, secretSz);
        if (ret < 0)
            return ret;
    }
    else
        XMEMCPY(ssl->buffers.tls13CookieSecret.buffer, secret, secretSz);

    ssl->options.sendCookie = 1;

    ret = WOLFSSL_SUCCESS;
#else
    (void)secret;
    (void)secretSz;

    ret = SIDE_ERROR;
#endif

    return ret;
}

int wolfSSL_disable_hrr_cookie(WOLFSSL* ssl)
{
    if (ssl == NULL || !IsAtLeastTLSv1_3(ssl->version))
        return BAD_FUNC_ARG;

#ifdef NO_WOLFSSL_SERVER
    return SIDE_ERROR;
#else
    if (ssl->options.side == WOLFSSL_CLIENT_END)
        return SIDE_ERROR;

    if (ssl->buffers.tls13CookieSecret.buffer != NULL) {
        ForceZero(ssl->buffers.tls13CookieSecret.buffer,
            ssl->buffers.tls13CookieSecret.length);
        XFREE(ssl->buffers.tls13CookieSecret.buffer, ssl->heap,
            DYNAMIC_TYPE_COOKIE_PWD);
        ssl->buffers.tls13CookieSecret.buffer = NULL;
        ssl->buffers.tls13CookieSecret.length = 0;
    }

    ssl->options.sendCookie = 0;
    return WOLFSSL_SUCCESS;
#endif /* NO_WOLFSSL_SERVER */
}

#endif /* defined(WOLFSSL_SEND_HRR_COOKIE) */

#ifdef HAVE_SUPPORTED_CURVES
/* Create a key share entry from group.
 * Generates a key pair.
 *
 * ssl    The SSL/TLS object.
 * group  The named group.
 * returns 0 on success, otherwise failure.
 *   for async can return WC_PENDING_E and should be called again
 */
int wolfSSL_UseKeyShare(WOLFSSL* ssl, word16 group)
{
    int ret;

    if (ssl == NULL)
        return BAD_FUNC_ARG;

#ifdef WOLFSSL_ASYNC_CRYPT
    ret = wolfSSL_AsyncPop(ssl, NULL);
    if (ret != WC_NOT_PENDING_E) {
        /* Check for error */
        if (ret < 0)
            return ret;
    }
#endif

#ifdef HAVE_PQC
    if (WOLFSSL_NAMED_GROUP_IS_PQC(group)) {

        if (ssl->ctx != NULL && ssl->ctx->method != NULL &&
            !IsAtLeastTLSv1_3(ssl->version)) {
            return BAD_FUNC_ARG;
        }

        if (ssl->options.side == WOLFSSL_SERVER_END) {
            /* If I am the server of a KEM connection, do not do keygen because I'm
             * going to encapsulate with the client's public key. Note that I might
             * be the client and ssl->option.side has not been properly set yet. In
             * that case the KeyGen operation will be deferred to connection time. */
            return WOLFSSL_SUCCESS;
        }
    }
#endif
#if defined(NO_TLS)
    (void)ret;
    (void)group;
#else
    ret = TLSX_KeyShare_Use(ssl, group, 0, NULL, NULL, &ssl->extensions);
    if (ret != 0)
        return ret;
#endif /* NO_TLS */
    return WOLFSSL_SUCCESS;
}

/* Send no key share entries - use HelloRetryRequest to negotiate shared group.
 *
 * ssl    The SSL/TLS object.
 * returns 0 on success, otherwise failure.
 */
int wolfSSL_NoKeyShares(WOLFSSL* ssl)
{
    int ret;

    if (ssl == NULL)
        return BAD_FUNC_ARG;
    if (ssl->options.side == WOLFSSL_SERVER_END)
        return SIDE_ERROR;
#if defined(NO_TLS)
    (void)ret;
#else
    ret = TLSX_KeyShare_Empty(ssl);
    if (ret != 0)
        return ret;
#endif /* NO_TLS */
    return WOLFSSL_SUCCESS;
}
#endif

/* Do not send a ticket after TLS v1.3 handshake for resumption.
 *
 * ctx  The SSL/TLS CTX object.
 * returns BAD_FUNC_ARG when ctx is NULL and 0 on success.
 */
int wolfSSL_CTX_no_ticket_TLSv13(WOLFSSL_CTX* ctx)
{
    if (ctx == NULL || !IsAtLeastTLSv1_3(ctx->method->version))
        return BAD_FUNC_ARG;
    if (ctx->method->side == WOLFSSL_CLIENT_END)
        return SIDE_ERROR;

#ifdef HAVE_SESSION_TICKET
    ctx->noTicketTls13 = 1;
#endif

    return 0;
}

/* Do not send a ticket after TLS v1.3 handshake for resumption.
 *
 * ssl  The SSL/TLS object.
 * returns BAD_FUNC_ARG when ssl is NULL, not using TLS v1.3, or called on
 * a client and 0 on success.
 */
int wolfSSL_no_ticket_TLSv13(WOLFSSL* ssl)
{
    if (ssl == NULL || !IsAtLeastTLSv1_3(ssl->version))
        return BAD_FUNC_ARG;
    if (ssl->options.side == WOLFSSL_CLIENT_END)
        return SIDE_ERROR;

#ifdef HAVE_SESSION_TICKET
    ssl->options.noTicketTls13 = 1;
#endif

    return 0;
}

/* Disallow (EC)DHE key exchange when using pre-shared keys.
 *
 * ctx  The SSL/TLS CTX object.
 * returns BAD_FUNC_ARG when ctx is NULL and 0 on success.
 */
int wolfSSL_CTX_no_dhe_psk(WOLFSSL_CTX* ctx)
{
    if (ctx == NULL || !IsAtLeastTLSv1_3(ctx->method->version))
        return BAD_FUNC_ARG;

#if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
    ctx->noPskDheKe = 1;
#endif

    return 0;
}

/* Disallow (EC)DHE key exchange when using pre-shared keys.
 *
 * ssl  The SSL/TLS object.
 * returns BAD_FUNC_ARG when ssl is NULL, or not using TLS v1.3 and 0 on
 * success.
 */
int wolfSSL_no_dhe_psk(WOLFSSL* ssl)
{
    if (ssl == NULL || !IsAtLeastTLSv1_3(ssl->version))
        return BAD_FUNC_ARG;

#if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
    ssl->options.noPskDheKe = 1;
#endif

    return 0;
}

#ifdef HAVE_SUPPORTED_CURVES
/* Only allow (EC)DHE key exchange when using pre-shared keys.
 *
 * ctx  The SSL/TLS CTX object.
 * returns BAD_FUNC_ARG when ctx is NULL and 0 on success.
 */
int wolfSSL_CTX_only_dhe_psk(WOLFSSL_CTX* ctx)
{
    if (ctx == NULL || !IsAtLeastTLSv1_3(ctx->method->version))
        return BAD_FUNC_ARG;

#if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
    ctx->onlyPskDheKe = 1;
#endif

    return 0;
}

/* Only allow (EC)DHE key exchange when using pre-shared keys.
 *
 * ssl  The SSL/TLS object.
 * returns BAD_FUNC_ARG when ssl is NULL, or not using TLS v1.3 and 0 on
 * success.
 */
int wolfSSL_only_dhe_psk(WOLFSSL* ssl)
{
    if (ssl == NULL || !IsAtLeastTLSv1_3(ssl->version))
        return BAD_FUNC_ARG;

#if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
    ssl->options.onlyPskDheKe = 1;
#endif

    return 0;
}
#endif /* HAVE_SUPPORTED_CURVES */

int Tls13UpdateKeys(WOLFSSL* ssl)
{
    if (ssl == NULL || !IsAtLeastTLSv1_3(ssl->version))
        return BAD_FUNC_ARG;

#ifdef WOLFSSL_DTLS13
    /* we are already waiting for the ack of a sent key update message. We can't
       send another one before receiving its ack. Either wolfSSL_update_keys()
       was invoked multiple times over a short period of time or we replied to a
       KeyUpdate with update request. We'll just ignore sending this
       KeyUpdate. */
    /* TODO: add WOLFSSL_ERROR_ALREADY_IN_PROGRESS type of error here */
    if (ssl->options.dtls && ssl->dtls13WaitKeyUpdateAck)
        return 0;
#endif /* WOLFSSL_DTLS13 */

    return SendTls13KeyUpdate(ssl);
}

/* Update the keys for encryption and decryption.
 * If using non-blocking I/O and WOLFSSL_ERROR_WANT_WRITE is returned then
 * calling wolfSSL_write() will have the message sent when ready.
 *
 * ssl  The SSL/TLS object.
 * returns BAD_FUNC_ARG when ssl is NULL, or not using TLS v1.3,
 * WOLFSSL_ERROR_WANT_WRITE when non-blocking I/O is not ready to write,
 * WOLFSSL_SUCCESS on success and otherwise failure.
 */
int wolfSSL_update_keys(WOLFSSL* ssl)
{
    int ret;
    ret = Tls13UpdateKeys(ssl);
    if (ret == WANT_WRITE)
        ret = WOLFSSL_ERROR_WANT_WRITE;
    else if (ret == 0)
        ret = WOLFSSL_SUCCESS;
    return ret;
}

/* Whether a response is waiting for key update request.
 *
 * ssl        The SSL/TLS object.
 * required   0 when no key update response required.
 *            1 when no key update response required.
 * return  0 on success.
 * return  BAD_FUNC_ARG when ssl is NULL or not using TLS v1.3
 */
int wolfSSL_key_update_response(WOLFSSL* ssl, int* required)
{
    if (required == NULL || ssl == NULL || !IsAtLeastTLSv1_3(ssl->version))
        return BAD_FUNC_ARG;

    *required = ssl->keys.updateResponseReq;

    return 0;
}

#if !defined(NO_CERTS) && defined(WOLFSSL_POST_HANDSHAKE_AUTH)
/* Allow post-handshake authentication in TLS v1.3 connections.
 *
 * ctx  The SSL/TLS CTX object.
 * returns BAD_FUNC_ARG when ctx is NULL, SIDE_ERROR when not a client and
 * 0 on success.
 */
int wolfSSL_CTX_allow_post_handshake_auth(WOLFSSL_CTX* ctx)
{
    if (ctx == NULL || !IsAtLeastTLSv1_3(ctx->method->version))
        return BAD_FUNC_ARG;
    if (ctx->method->side == WOLFSSL_SERVER_END)
        return SIDE_ERROR;

    ctx->postHandshakeAuth = 1;

    return 0;
}

/* Allow post-handshake authentication in TLS v1.3 connection.
 *
 * ssl  The SSL/TLS object.
 * returns BAD_FUNC_ARG when ssl is NULL, or not using TLS v1.3,
 * SIDE_ERROR when not a client and 0 on success.
 */
int wolfSSL_allow_post_handshake_auth(WOLFSSL* ssl)
{
    if (ssl == NULL || !IsAtLeastTLSv1_3(ssl->version))
        return BAD_FUNC_ARG;
    if (ssl->options.side == WOLFSSL_SERVER_END)
        return SIDE_ERROR;

    ssl->options.postHandshakeAuth = 1;

    return 0;
}

/* Request a certificate of the client.
 * Can be called any time after handshake completion.
 * A maximum of 256 requests can be sent on a connection.
 *
 * ssl  SSL/TLS object.
 */
int wolfSSL_request_certificate(WOLFSSL* ssl)
{
    int         ret;
#ifndef NO_WOLFSSL_SERVER
    CertReqCtx* certReqCtx;
#endif

    if (ssl == NULL || !IsAtLeastTLSv1_3(ssl->version))
        return BAD_FUNC_ARG;
#ifndef NO_WOLFSSL_SERVER
    if (ssl->options.side == WOLFSSL_CLIENT_END)
        return SIDE_ERROR;
    if (ssl->options.handShakeState != HANDSHAKE_DONE)
        return NOT_READY_ERROR;
    if (!ssl->options.postHandshakeAuth)
        return POST_HAND_AUTH_ERROR;

    certReqCtx = (CertReqCtx*)XMALLOC(sizeof(CertReqCtx), ssl->heap,
                                                       DYNAMIC_TYPE_TMP_BUFFER);
    if (certReqCtx == NULL)
        return MEMORY_E;
    XMEMSET(certReqCtx, 0, sizeof(CertReqCtx));
    certReqCtx->next = ssl->certReqCtx;
    certReqCtx->len = 1;
    if (certReqCtx->next != NULL)
        certReqCtx->ctx = certReqCtx->next->ctx + 1;
    ssl->certReqCtx = certReqCtx;

    ssl->msgsReceived.got_certificate = 0;
    ssl->msgsReceived.got_certificate_verify = 0;
    ssl->msgsReceived.got_finished = 0;

    ret = SendTls13CertificateRequest(ssl, &certReqCtx->ctx, certReqCtx->len);
    if (ret == WANT_WRITE)
        ret = WOLFSSL_ERROR_WANT_WRITE;
    else if (ret == 0)
        ret = WOLFSSL_SUCCESS;
#else
    ret = SIDE_ERROR;
#endif

    return ret;
}
#endif /* !NO_CERTS && WOLFSSL_POST_HANDSHAKE_AUTH */

#if !defined(WOLFSSL_NO_SERVER_GROUPS_EXT)
/* Get the preferred key exchange group.
 *
 * ssl  The SSL/TLS object.
 * returns BAD_FUNC_ARG when ssl is NULL or not using TLS v1.3,
 * SIDE_ERROR when not a client, NOT_READY_ERROR when handshake not complete
 * and group number on success.
 */
int wolfSSL_preferred_group(WOLFSSL* ssl)
{
    if (ssl == NULL || !IsAtLeastTLSv1_3(ssl->version))
        return BAD_FUNC_ARG;
#ifndef NO_WOLFSSL_CLIENT
    if (ssl->options.side == WOLFSSL_SERVER_END)
        return SIDE_ERROR;
    if (ssl->options.handShakeState != HANDSHAKE_DONE)
        return NOT_READY_ERROR;

#ifdef HAVE_SUPPORTED_CURVES
    /* Return supported groups only. */
    return TLSX_SupportedCurve_Preferred(ssl, 1);
#else
    return 0;
#endif
#else
    return SIDE_ERROR;
#endif
}
#endif

#if defined(HAVE_SUPPORTED_CURVES)
/* Sets the key exchange groups in rank order on a context.
 *
 * ctx     SSL/TLS context object.
 * groups  Array of groups.
 * count   Number of groups in array.
 * returns BAD_FUNC_ARG when ctx or groups is NULL, not using TLS v1.3 or
 * count is greater than WOLFSSL_MAX_GROUP_COUNT and WOLFSSL_SUCCESS on success.
 */
int wolfSSL_CTX_set_groups(WOLFSSL_CTX* ctx, int* groups, int count)
{
    int ret, i;

    WOLFSSL_ENTER("wolfSSL_CTX_set_groups");
    if (ctx == NULL || groups == NULL || count > WOLFSSL_MAX_GROUP_COUNT)
        return BAD_FUNC_ARG;
    if (!IsAtLeastTLSv1_3(ctx->method->version))
        return BAD_FUNC_ARG;

    ctx->numGroups = 0;
    #if !defined(NO_TLS)
    TLSX_Remove(&ctx->extensions, TLSX_SUPPORTED_GROUPS, ctx->heap);
    #endif /* !NO_TLS */
    for (i = 0; i < count; i++) {
        /* Call to wolfSSL_CTX_UseSupportedCurve also checks if input groups
         * are valid */
        if ((ret = wolfSSL_CTX_UseSupportedCurve(ctx, (word16)groups[i]))
                != WOLFSSL_SUCCESS) {
    #if !defined(NO_TLS)
            TLSX_Remove(&ctx->extensions, TLSX_SUPPORTED_GROUPS, ctx->heap);
    #endif /* !NO_TLS */
            return ret;
        }
        ctx->group[i] = (word16)groups[i];
    }
    ctx->numGroups = (byte)count;

    return WOLFSSL_SUCCESS;
}

/* Sets the key exchange groups in rank order.
 *
 * ssl     SSL/TLS object.
 * groups  Array of groups.
 * count   Number of groups in array.
 * returns BAD_FUNC_ARG when ssl or groups is NULL, not using TLS v1.3 or
 * count is greater than WOLFSSL_MAX_GROUP_COUNT and WOLFSSL_SUCCESS on success.
 */
int wolfSSL_set_groups(WOLFSSL* ssl, int* groups, int count)
{
    int ret, i;

    WOLFSSL_ENTER("wolfSSL_set_groups");
    if (ssl == NULL || groups == NULL || count > WOLFSSL_MAX_GROUP_COUNT)
        return BAD_FUNC_ARG;
    if (!IsAtLeastTLSv1_3(ssl->version))
        return BAD_FUNC_ARG;

    ssl->numGroups = 0;
    #if !defined(NO_TLS)
    TLSX_Remove(&ssl->extensions, TLSX_SUPPORTED_GROUPS, ssl->heap);
    #endif /* !NO_TLS */
    for (i = 0; i < count; i++) {
        /* Call to wolfSSL_UseSupportedCurve also checks if input groups
                 * are valid */
        if ((ret = wolfSSL_UseSupportedCurve(ssl, (word16)groups[i]))
                != WOLFSSL_SUCCESS) {
    #if !defined(NO_TLS)
            TLSX_Remove(&ssl->extensions, TLSX_SUPPORTED_GROUPS, ssl->heap);
    #endif /* !NO_TLS */
            return ret;
        }
        ssl->group[i] = (word16)groups[i];
    }
    ssl->numGroups = (byte)count;

    return WOLFSSL_SUCCESS;
}
#endif /* HAVE_SUPPORTED_CURVES */

#ifndef NO_PSK
/* Set the PSK callback, that is passed the cipher suite, for a client to use
 * against context object.
 *
 * @param [in, out] ctx  SSL/TLS context object.
 * @param [in]      cb   Client PSK callback passed a cipher suite.
 */
void wolfSSL_CTX_set_psk_client_cs_callback(WOLFSSL_CTX* ctx,
                                            wc_psk_client_cs_callback cb)
{
    WOLFSSL_ENTER("wolfSSL_CTX_set_psk_client_cs_callback");

    if (ctx == NULL)
        return;

    ctx->havePSK = 1;
    ctx->client_psk_cs_cb = cb;
}

/* Set the PSK callback, that is passed the cipher suite, for a client to use
 * against SSL object.
 *
 * @param [in, out] ssl  SSL/TLS object.
 * @param [in]      cb   Client PSK callback passed a cipher suite.
 */
void wolfSSL_set_psk_client_cs_callback(WOLFSSL* ssl,
                                        wc_psk_client_cs_callback cb)
{
    byte haveRSA = 1;
    int  keySz   = 0;

    WOLFSSL_ENTER("wolfSSL_set_psk_client_cs_callback");

    if (ssl == NULL)
        return;

    ssl->options.havePSK = 1;
    ssl->options.client_psk_cs_cb = cb;

    #ifdef NO_RSA
        haveRSA = 0;
    #endif
    #ifndef NO_CERTS
        keySz = ssl->buffers.keySz;
    #endif
    if (AllocateSuites(ssl) != 0)
        return;
    InitSuites(ssl->suites, ssl->version, keySz, haveRSA, TRUE,
               ssl->options.haveDH, ssl->options.haveECDSAsig,
               ssl->options.haveECC, TRUE, ssl->options.haveStaticECC,
               ssl->options.haveFalconSig, ssl->options.haveDilithiumSig,
               ssl->options.haveAnon, TRUE, ssl->options.side);
}

/* Set the PSK callback that returns the cipher suite for a client to use
 * against context object.
 *
 * @param [in, out] ctx  SSL/TLS context object.
 * @param [in]      cb   Client PSK callback returning cipher suite.
 */
void wolfSSL_CTX_set_psk_client_tls13_callback(WOLFSSL_CTX* ctx,
                                               wc_psk_client_tls13_callback cb)
{
    WOLFSSL_ENTER("wolfSSL_CTX_set_psk_client_tls13_callback");

    if (ctx == NULL)
        return;

    ctx->havePSK = 1;
    ctx->client_psk_tls13_cb = cb;
}

/* Set the PSK callback that returns the cipher suite for a client to use
 * against SSL object.
 *
 * @param [in, out] ssl  SSL/TLS object.
 * @param [in]      cb   Client PSK callback returning cipher suite.
 */
void wolfSSL_set_psk_client_tls13_callback(WOLFSSL* ssl,
                                           wc_psk_client_tls13_callback cb)
{
    byte haveRSA = 1;
    int  keySz   = 0;

    WOLFSSL_ENTER("wolfSSL_set_psk_client_tls13_callback");

    if (ssl == NULL)
        return;

    ssl->options.havePSK = 1;
    ssl->options.client_psk_tls13_cb = cb;

    #ifdef NO_RSA
        haveRSA = 0;
    #endif
    #ifndef NO_CERTS
        keySz = ssl->buffers.keySz;
    #endif
    if (AllocateSuites(ssl) != 0)
        return;
    InitSuites(ssl->suites, ssl->version, keySz, haveRSA, TRUE,
               ssl->options.haveDH, ssl->options.haveECDSAsig,
               ssl->options.haveECC, TRUE, ssl->options.haveStaticECC,
               ssl->options.haveFalconSig, ssl->options.haveDilithiumSig,
               ssl->options.haveAnon, TRUE, ssl->options.side);
}

/* Set the PSK callback that returns the cipher suite for a server to use
 * against context object.
 *
 * @param [in, out] ctx  SSL/TLS context object.
 * @param [in]      cb   Server PSK callback returning cipher suite.
 */
void wolfSSL_CTX_set_psk_server_tls13_callback(WOLFSSL_CTX* ctx,
                                               wc_psk_server_tls13_callback cb)
{
    WOLFSSL_ENTER("wolfSSL_CTX_set_psk_server_tls13_callback");
    if (ctx == NULL)
        return;
    ctx->havePSK = 1;
    ctx->server_psk_tls13_cb = cb;
}

/* Set the PSK callback that returns the cipher suite for a server to use
 * against SSL object.
 *
 * @param [in, out] ssl  SSL/TLS object.
 * @param [in]      cb   Server PSK callback returning cipher suite.
 */
void wolfSSL_set_psk_server_tls13_callback(WOLFSSL* ssl,
                                           wc_psk_server_tls13_callback cb)
{
    byte haveRSA = 1;
    int  keySz   = 0;

    WOLFSSL_ENTER("wolfSSL_set_psk_server_tls13_callback");
    if (ssl == NULL)
        return;

    ssl->options.havePSK = 1;
    ssl->options.server_psk_tls13_cb = cb;

    #ifdef NO_RSA
        haveRSA = 0;
    #endif
    #ifndef NO_CERTS
        keySz = ssl->buffers.keySz;
    #endif
    if (AllocateSuites(ssl) != 0)
        return;
    InitSuites(ssl->suites, ssl->version, keySz, haveRSA, TRUE,
               ssl->options.haveDH, ssl->options.haveECDSAsig,
               ssl->options.haveECC, TRUE, ssl->options.haveStaticECC,
               ssl->options.haveFalconSig, ssl->options.haveDilithiumSig,
               ssl->options.haveAnon, TRUE, ssl->options.side);
}

/* Get name of first supported cipher suite that uses the hash indicated.
 *
 * @param [in] ssl   SSL/TLS object.
 * @param [in] hash  Name of hash algorithm. e.g. "SHA256", "SHA384"
 * @return  Name of cipher suite.
 * @return  NULL on failure.
 */
const char* wolfSSL_get_cipher_name_by_hash(WOLFSSL* ssl, const char* hash)
{
    const char* name = NULL;
    byte mac = no_mac;
    int i;
    const Suites* suites = WOLFSSL_SUITES(ssl);

    if (XSTRCMP(hash, "SHA256") == 0) {
        mac = sha256_mac;
    }
    else if (XSTRCMP(hash, "SHA384") == 0) {
        mac = sha384_mac;
    }
    if (mac != no_mac) {
        for (i = 0; i < suites->suiteSz; i += 2) {
            if (SuiteMac(suites->suites + i) == mac) {
                name = GetCipherNameInternal(suites->suites[i + 0],
                                             suites->suites[i + 1]);
                break;
            }
        }
    }
    return name;
}
#endif /* !NO_PSK */


#ifndef NO_WOLFSSL_SERVER

/* The server accepting a connection from a client.
 * The protocol version is expecting to be TLS v1.3.
 * If the client downgrades, and older versions of the protocol are compiled
 * in, the server will fallback to wolfSSL_accept().
 * Please see note at top of README if you get an error from accept.
 *
 * ssl  The SSL/TLS object.
 * returns WOLFSSL_SUCCESS on successful handshake, WOLFSSL_FATAL_ERROR when
 * unrecoverable error occurs and 0 otherwise.
 * For more error information use wolfSSL_get_error().
 */
int wolfSSL_accept_TLSv13(WOLFSSL* ssl)
{
#if !defined(NO_CERTS) && (defined(HAVE_SESSION_TICKET) || !defined(NO_PSK))
    word16 havePSK = 0;
#endif
    int ret = 0;

    WOLFSSL_ENTER("wolfSSL_accept_TLSv13");

#ifdef HAVE_ERRNO_H
    errno = 0;
#endif

    if (ssl == NULL)
        return WOLFSSL_FATAL_ERROR;

#if !defined(NO_CERTS) && (defined(HAVE_SESSION_TICKET) || !defined(NO_PSK))
    havePSK = ssl->options.havePSK;
#endif

    if (ssl->options.side != WOLFSSL_SERVER_END) {
        ssl->error = SIDE_ERROR;
        WOLFSSL_ERROR(ssl->error);
        return WOLFSSL_FATAL_ERROR;
    }

    /* make sure this wolfSSL object has arrays and rng setup. Protects
     * case where the WOLFSSL object is re-used via wolfSSL_clear() */
    if ((ret = ReinitSSL(ssl, ssl->ctx, 0)) != 0) {
        return ret;
    }

#ifdef WOLFSSL_DTLS
    if (ssl->version.major == DTLS_MAJOR) {
        ssl->options.dtls   = 1;
        if (!IsDtlsNotSctpMode(ssl) || !ssl->options.sendCookie)
            ssl->options.dtlsStateful = 1;
    }
#endif

#ifdef WOLFSSL_WOLFSENTRY_HOOKS
    if ((ssl->AcceptFilter != NULL) &&
            ((ssl->options.acceptState == TLS13_ACCEPT_BEGIN)
#ifdef HAVE_SECURE_RENEGOTIATION
             || (ssl->options.acceptState == TLS13_ACCEPT_BEGIN_RENEG)
#endif
                ))
    {
        wolfSSL_netfilter_decision_t res;
        if ((ssl->AcceptFilter(ssl, ssl->AcceptFilter_arg, &res) ==
             WOLFSSL_SUCCESS) &&
            (res == WOLFSSL_NETFILTER_REJECT)) {
            ssl->error = SOCKET_FILTERED_E;
            WOLFSSL_ERROR(ssl->error);
            return WOLFSSL_FATAL_ERROR;
        }
    }
#endif /* WOLFSSL_WOLFSENTRY_HOOKS */

#ifndef NO_CERTS
#if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
    if (!havePSK)
#endif
    {
    #if defined(OPENSSL_ALL) || defined(OPENSSL_EXTRA) || \
        defined(WOLFSSL_NGINX) || defined (WOLFSSL_HAPROXY)
        if (ssl->ctx->certSetupCb != NULL) {
            WOLFSSL_MSG("CertSetupCb set. server cert and "
                        "key not checked");
        }
        else
    #endif
        {
            if (!ssl->buffers.certificate ||
                !ssl->buffers.certificate->buffer) {

                WOLFSSL_MSG("accept error: server cert required");
                ssl->error = NO_PRIVATE_KEY;
                WOLFSSL_ERROR(ssl->error);
                return WOLFSSL_FATAL_ERROR;
            }

            if (!ssl->buffers.key || !ssl->buffers.key->buffer) {
                /* allow no private key if using existing key */
            #ifdef WOLF_PRIVATE_KEY_ID
                if (ssl->devId != INVALID_DEVID
                #ifdef HAVE_PK_CALLBACKS
                    || wolfSSL_CTX_IsPrivatePkSet(ssl->ctx)
                #endif
                ) {
                    WOLFSSL_MSG("Allowing no server private key (external)");
                }
                else
            #endif
                {
                    WOLFSSL_MSG("accept error: server key required");
                    ssl->error = NO_PRIVATE_KEY;
                    WOLFSSL_ERROR(ssl->error);
                    return WOLFSSL_FATAL_ERROR;
                }
            }
        }
    }
#endif /* NO_CERTS */

    if (ssl->buffers.outputBuffer.length > 0
    #ifdef WOLFSSL_ASYNC_CRYPT
        /* do not send buffered or advance state if last error was an
            async pending operation */
        && ssl->error != WC_PENDING_E
    #endif
    ) {

        /* fragOffset is non-zero when sending fragments. On the last
         * fragment, fragOffset is zero again, and the state can be
         * advanced. */
        int advanceState =
            (ssl->options.acceptState == TLS13_ACCEPT_CLIENT_HELLO_DONE ||
                ssl->options.acceptState ==
                    TLS13_ACCEPT_HELLO_RETRY_REQUEST_DONE ||
                ssl->options.acceptState == TLS13_ACCEPT_SECOND_REPLY_DONE ||
                ssl->options.acceptState == TLS13_SERVER_HELLO_SENT ||
                ssl->options.acceptState == TLS13_ACCEPT_THIRD_REPLY_DONE ||
                ssl->options.acceptState == TLS13_SERVER_EXTENSIONS_SENT ||
                ssl->options.acceptState == TLS13_CERT_REQ_SENT ||
                ssl->options.acceptState == TLS13_CERT_SENT ||
                ssl->options.acceptState == TLS13_CERT_VERIFY_SENT ||
                ssl->options.acceptState == TLS13_ACCEPT_FINISHED_SENT ||
                ssl->options.acceptState == TLS13_ACCEPT_FINISHED_DONE);

#ifdef WOLFSSL_DTLS13
        if (ssl->options.dtls)
            advanceState = advanceState && !ssl->dtls13SendingFragments
                && !ssl->dtls13SendingAckOrRtx;
#endif /* WOLFSSL_DTLS13 */

        if ((ssl->error = SendBuffered(ssl)) == 0) {
            if (ssl->fragOffset == 0 && !ssl->options.buildingMsg) {
                if (advanceState) {
                    ssl->options.acceptState++;
                    WOLFSSL_MSG("accept state: "
                                "Advanced from last buffered fragment send");
#ifdef WOLFSSL_ASYNC_IO
                    FreeAsyncCtx(ssl, 0);
#endif
                }
            }
            else {
                WOLFSSL_MSG("accept state: "
                            "Not advanced, more fragments to send");
            }

#ifdef WOLFSSL_DTLS13
            if (ssl->options.dtls)
                ssl->dtls13SendingAckOrRtx = 0;
#endif /* WOLFSSL_DTLS13 */

        }
        else {
            ssl->error = ret;
            WOLFSSL_ERROR(ssl->error);
            return WOLFSSL_FATAL_ERROR;
        }
    }

    ret = RetrySendAlert(ssl);
    if (ret != 0) {
        ssl->error = ret;
        WOLFSSL_ERROR(ssl->error);
        return WOLFSSL_FATAL_ERROR;
    }
#ifdef WOLFSSL_DTLS13
    if (ssl->options.dtls && ssl->dtls13SendingFragments) {
        if ((ssl->error = Dtls13FragmentsContinue(ssl)) != 0) {
                WOLFSSL_ERROR(ssl->error);
                return WOLFSSL_FATAL_ERROR;
        }

        /* we sent all the fragments. Advance state. */
        ssl->options.acceptState++;
    }
#endif /* WOLFSSL_DTLS13 */

    switch (ssl->options.acceptState) {

#ifdef HAVE_SECURE_RENEGOTIATION
        case TLS13_ACCEPT_BEGIN_RENEG:
#endif
        case TLS13_ACCEPT_BEGIN :
            /* get client_hello */

            while (ssl->options.clientState < CLIENT_HELLO_COMPLETE) {
                if ((ssl->error = ProcessReply(ssl)) < 0) {
                    WOLFSSL_ERROR(ssl->error);
                    return WOLFSSL_FATAL_ERROR;
                }

#ifdef WOLFSSL_DTLS13
                if (ssl->options.dtls) {
                    if ((ssl->error = Dtls13DoScheduledWork(ssl)) < 0) {
                        WOLFSSL_ERROR(ssl->error);
                        return WOLFSSL_FATAL_ERROR;
                    }
                }
#endif /* WOLFSSL_DTLS13 */

            }

            ssl->options.acceptState = TLS13_ACCEPT_CLIENT_HELLO_DONE;
            WOLFSSL_MSG("accept state ACCEPT_CLIENT_HELLO_DONE");
            if (!IsAtLeastTLSv1_3(ssl->version))
                return wolfSSL_accept(ssl);
            FALL_THROUGH;

        case TLS13_ACCEPT_CLIENT_HELLO_DONE :
            if (ssl->options.serverState ==
                                          SERVER_HELLO_RETRY_REQUEST_COMPLETE) {
                if ((ssl->error = SendTls13ServerHello(ssl,
                                                   hello_retry_request)) != 0) {
                    WOLFSSL_ERROR(ssl->error);
                    return WOLFSSL_FATAL_ERROR;
                }
            }

            ssl->options.acceptState = TLS13_ACCEPT_HELLO_RETRY_REQUEST_DONE;
            WOLFSSL_MSG("accept state ACCEPT_HELLO_RETRY_REQUEST_DONE");
            FALL_THROUGH;

        case TLS13_ACCEPT_HELLO_RETRY_REQUEST_DONE :
    #ifdef WOLFSSL_TLS13_MIDDLEBOX_COMPAT
            if (!ssl->options.dtls && ssl->options.tls13MiddleBoxCompat
                && ssl->options.serverState ==
                                          SERVER_HELLO_RETRY_REQUEST_COMPLETE) {
                if ((ssl->error = SendChangeCipher(ssl)) != 0) {
                    WOLFSSL_ERROR(ssl->error);
                    return WOLFSSL_FATAL_ERROR;
                }
                ssl->options.sentChangeCipher = 1;
                ssl->options.serverState = SERVER_HELLO_RETRY_REQUEST_COMPLETE;
            }
    #endif
            ssl->options.acceptState = TLS13_ACCEPT_FIRST_REPLY_DONE;
            WOLFSSL_MSG("accept state ACCEPT_FIRST_REPLY_DONE");
            FALL_THROUGH;

        case TLS13_ACCEPT_FIRST_REPLY_DONE :
            if (ssl->options.serverState ==
                                          SERVER_HELLO_RETRY_REQUEST_COMPLETE) {
                ssl->options.clientState = CLIENT_HELLO_RETRY;
                while (ssl->options.clientState < CLIENT_HELLO_COMPLETE) {
                    if ((ssl->error = ProcessReply(ssl)) < 0) {
                        WOLFSSL_ERROR(ssl->error);
                        return WOLFSSL_FATAL_ERROR;
                    }

#ifdef WOLFSSL_DTLS13
                if (ssl->options.dtls) {
                    if ((ssl->error = Dtls13DoScheduledWork(ssl)) < 0) {
                        WOLFSSL_ERROR(ssl->error);
                        return WOLFSSL_FATAL_ERROR;
                    }
                }
#endif /* WOLFSSL_DTLS13 */

                }
            }

            ssl->options.acceptState = TLS13_ACCEPT_SECOND_REPLY_DONE;
            WOLFSSL_MSG("accept state ACCEPT_SECOND_REPLY_DONE");
            FALL_THROUGH;

        case TLS13_ACCEPT_SECOND_REPLY_DONE :

#ifdef WOLFSSL_DTLS
            if (ssl->chGoodCb != NULL) {
                int cbret = ssl->chGoodCb(ssl, ssl->chGoodCtx);
                if (cbret < 0) {
                    ssl->error = cbret;
                    WOLFSSL_MSG("ClientHello Good Cb don't continue error");
                    return WOLFSSL_FATAL_ERROR;
                }
            }
#endif

            if ((ssl->error = SendTls13ServerHello(ssl, server_hello)) != 0) {
                WOLFSSL_ERROR(ssl->error);
                return WOLFSSL_FATAL_ERROR;
            }
            ssl->options.acceptState = TLS13_SERVER_HELLO_SENT;
            WOLFSSL_MSG("accept state SERVER_HELLO_SENT");
            FALL_THROUGH;

        case TLS13_SERVER_HELLO_SENT :
    #if defined(WOLFSSL_TLS13_MIDDLEBOX_COMPAT)
            if (!ssl->options.dtls && ssl->options.tls13MiddleBoxCompat
                          && !ssl->options.sentChangeCipher && !ssl->options.dtls) {
                if ((ssl->error = SendChangeCipher(ssl)) != 0) {
                    WOLFSSL_ERROR(ssl->error);
                    return WOLFSSL_FATAL_ERROR;
                }
                ssl->options.sentChangeCipher = 1;
            }
    #endif

            ssl->options.acceptState = TLS13_ACCEPT_THIRD_REPLY_DONE;
            WOLFSSL_MSG("accept state ACCEPT_THIRD_REPLY_DONE");
            FALL_THROUGH;

        case TLS13_ACCEPT_THIRD_REPLY_DONE :
    #ifdef HAVE_SUPPORTED_CURVES
        #if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
            if (!ssl->options.noPskDheKe)
        #endif
            {
                ssl->error = TLSX_KeyShare_DeriveSecret(ssl);
                if (ssl->error != 0)
                    return WOLFSSL_FATAL_ERROR;
            }
    #endif

            if ((ssl->error = SendTls13EncryptedExtensions(ssl)) != 0) {
                WOLFSSL_ERROR(ssl->error);
                return WOLFSSL_FATAL_ERROR;
            }
            ssl->options.acceptState = TLS13_SERVER_EXTENSIONS_SENT;
            WOLFSSL_MSG("accept state SERVER_EXTENSIONS_SENT");
            FALL_THROUGH;

        case TLS13_SERVER_EXTENSIONS_SENT :
#ifndef NO_CERTS
            if (!ssl->options.resuming) {
                if (ssl->options.verifyPeer
    #ifdef WOLFSSL_POST_HANDSHAKE_AUTH
                    && !ssl->options.verifyPostHandshake
    #endif
                   ) {
                    ssl->error = SendTls13CertificateRequest(ssl, NULL, 0);
                    if (ssl->error != 0) {
                        WOLFSSL_ERROR(ssl->error);
                        return WOLFSSL_FATAL_ERROR;
                    }
                }
                else {
                    /* SERVER: Peer auth good if not verifying client. */
                    ssl->options.peerAuthGood = 1;
                }
            }
#endif
            ssl->options.acceptState = TLS13_CERT_REQ_SENT;
            WOLFSSL_MSG("accept state CERT_REQ_SENT");
            FALL_THROUGH;

        case TLS13_CERT_REQ_SENT :
#ifndef NO_CERTS
            if (!ssl->options.resuming && ssl->options.sendVerify) {
                if ((ssl->error = SendTls13Certificate(ssl)) != 0) {
                    WOLFSSL_ERROR(ssl->error);
                    return WOLFSSL_FATAL_ERROR;
                }
            }
#endif
            ssl->options.acceptState = TLS13_CERT_SENT;
            WOLFSSL_MSG("accept state CERT_SENT");
            FALL_THROUGH;

        case TLS13_CERT_SENT :
#if !defined(NO_CERTS) && (!defined(NO_RSA) || defined(HAVE_ECC) || \
     defined(HAVE_ED25519) || defined(HAVE_ED448) || defined(HAVE_PQC))
            if (!ssl->options.resuming && ssl->options.sendVerify) {
                if ((ssl->error = SendTls13CertificateVerify(ssl)) != 0) {
                    WOLFSSL_ERROR(ssl->error);
                    return WOLFSSL_FATAL_ERROR;
                }
            }
#endif
            ssl->options.acceptState = TLS13_CERT_VERIFY_SENT;
            WOLFSSL_MSG("accept state CERT_VERIFY_SENT");
            FALL_THROUGH;

        case TLS13_CERT_VERIFY_SENT :
            if ((ssl->error = SendTls13Finished(ssl)) != 0) {
                WOLFSSL_ERROR(ssl->error);
                return WOLFSSL_FATAL_ERROR;
            }

            ssl->options.acceptState = TLS13_ACCEPT_FINISHED_SENT;
            WOLFSSL_MSG("accept state ACCEPT_FINISHED_SENT");
#ifdef WOLFSSL_EARLY_DATA
            if (ssl->earlyData != no_early_data) {
                ssl->options.handShakeState = SERVER_FINISHED_COMPLETE;
                return WOLFSSL_SUCCESS;
            }
#endif
            FALL_THROUGH;

        case TLS13_ACCEPT_FINISHED_SENT :
#ifdef HAVE_SESSION_TICKET
    #ifdef WOLFSSL_TLS13_TICKET_BEFORE_FINISHED
            if (!ssl->options.verifyPeer && !ssl->options.noTicketTls13 &&
                                                ssl->ctx->ticketEncCb != NULL) {
                if ((ssl->error = SendTls13NewSessionTicket(ssl)) != 0) {
                    WOLFSSL_ERROR(ssl->error);
                    return WOLFSSL_FATAL_ERROR;
                }
                ssl->options.ticketsSent = 1;
            }
    #endif
#endif /* HAVE_SESSION_TICKET */
            ssl->options.acceptState = TLS13_PRE_TICKET_SENT;
            WOLFSSL_MSG("accept state  TICKET_SENT");
            FALL_THROUGH;

        case TLS13_PRE_TICKET_SENT :
            while (ssl->options.clientState < CLIENT_FINISHED_COMPLETE) {
                if ( (ssl->error = ProcessReply(ssl)) < 0) {
                        WOLFSSL_ERROR(ssl->error);
                        return WOLFSSL_FATAL_ERROR;
                    }

#ifdef WOLFSSL_DTLS13
                if (ssl->options.dtls) {
                    if ((ssl->error = Dtls13DoScheduledWork(ssl)) < 0) {
                        WOLFSSL_ERROR(ssl->error);
                        return WOLFSSL_FATAL_ERROR;
                    }
                }
#endif /* WOLFSSL_DTLS13 */
            }

            ssl->options.acceptState = TLS13_ACCEPT_FINISHED_DONE;
            WOLFSSL_MSG("accept state ACCEPT_FINISHED_DONE");
            FALL_THROUGH;

        case TLS13_ACCEPT_FINISHED_DONE :
            /* SERVER: When not resuming and verifying peer but no certificate
             * received and not failing when not received then peer auth good.
             */
            if (!ssl->options.resuming && ssl->options.verifyPeer &&
        #ifdef WOLFSSL_POST_HANDSHAKE_AUTH
                !ssl->options.verifyPostHandshake &&
        #endif
                !ssl->options.havePeerCert && !ssl->options.failNoCert) {
                ssl->options.peerAuthGood = 1;
            }
            /* SERVER: check peer authentication. */
            if (!ssl->options.peerAuthGood) {
                WOLFSSL_MSG("Client authentication did not happen");
                return WOLFSSL_FATAL_ERROR;
            }
#ifdef HAVE_SESSION_TICKET
            while (ssl->options.ticketsSent < ssl->options.maxTicketTls13) {
                if (!ssl->options.noTicketTls13 && ssl->ctx->ticketEncCb
                        != NULL) {
                    if ((ssl->error = SendTls13NewSessionTicket(ssl)) != 0) {
                        WOLFSSL_ERROR(ssl->error);
                        return WOLFSSL_FATAL_ERROR;
                    }
                }
                ssl->options.ticketsSent++;

                /* only one session ticket is sent on session resumption */
                if (ssl->options.resuming) {
                    break;
                }
            }
#endif /* HAVE_SESSION_TICKET */
            ssl->options.acceptState = TLS13_TICKET_SENT;
            WOLFSSL_MSG("accept state TICKET_SENT");
            FALL_THROUGH;

        case TLS13_TICKET_SENT :
#ifndef NO_HANDSHAKE_DONE_CB
            if (ssl->hsDoneCb) {
                int cbret = ssl->hsDoneCb(ssl, ssl->hsDoneCtx);
                if (cbret < 0) {
                    ssl->error = cbret;
                    WOLFSSL_MSG("HandShake Done Cb don't continue error");
                    return WOLFSSL_FATAL_ERROR;
                }
            }
#endif /* NO_HANDSHAKE_DONE_CB */

            if (!ssl->options.keepResources) {
                FreeHandshakeResources(ssl);
            }

#if defined(WOLFSSL_ASYNC_IO) && !defined(WOLFSSL_ASYNC_CRYPT)
            /* Free the remaining async context if not using it for crypto */
            FreeAsyncCtx(ssl, 1);
#endif

            ssl->error = 0; /* clear the error */

            WOLFSSL_LEAVE("wolfSSL_accept", WOLFSSL_SUCCESS);
            return WOLFSSL_SUCCESS;

        default :
            WOLFSSL_MSG("Unknown accept state ERROR");
            return WOLFSSL_FATAL_ERROR;
    }
}
#endif

#if !defined(NO_WOLFSSL_SERVER) && defined(HAVE_SESSION_TICKET)
/* Server sends a session ticket to the peer.
 *
 * RFC 8446, section 4.6.1, para 1.
 *
 * ssl  The SSL/TLS object.
 * returns BAD_FUNC_ARG when ssl is NULL, or not using TLS v1.3,
 *         SIDE_ERROR when not a server,
 *         NOT_READY_ERROR when handshake not complete,
 *         WOLFSSL_FATAL_ERROR when creating or sending message fails, and
 *         WOLFSSL_SUCCESS on success.
 */
int wolfSSL_send_SessionTicket(WOLFSSL* ssl)
{
    if (ssl == NULL || !IsAtLeastTLSv1_3(ssl->version))
        return BAD_FUNC_ARG;
    if (ssl->options.side == WOLFSSL_CLIENT_END)
        return SIDE_ERROR;
    if (ssl->options.handShakeState != HANDSHAKE_DONE)
        return NOT_READY_ERROR;

    if ((ssl->error = SendTls13NewSessionTicket(ssl)) != 0) {
        WOLFSSL_ERROR(ssl->error);
        return WOLFSSL_FATAL_ERROR;
    }
    ssl->options.ticketsSent++;

    return WOLFSSL_SUCCESS;
}
#endif

#ifdef WOLFSSL_EARLY_DATA
/* Sets the maximum amount of early data that can be seen by server when using
 * session tickets for resumption.
 * A value of zero indicates no early data is to be sent by client using session
 * tickets.
 *
 * ctx  The SSL/TLS CTX object.
 * sz   Maximum size of the early data.
 * returns BAD_FUNC_ARG when ctx is NULL, SIDE_ERROR when not a server and
 * 0 on success.
 */
int wolfSSL_CTX_set_max_early_data(WOLFSSL_CTX* ctx, unsigned int sz)
{
    if (ctx == NULL || !IsAtLeastTLSv1_3(ctx->method->version))
        return BAD_FUNC_ARG;
    if (ctx->method->side == WOLFSSL_CLIENT_END)
        return SIDE_ERROR;

    ctx->maxEarlyDataSz = sz;

#if defined(OPENSSL_EXTRA) || defined(WOLFSSL_ERROR_CODE_OPENSSL)
    /* 1 on success in OpenSSL*/
    return WOLFSSL_SUCCESS;
#else
    return 0;
#endif
}

/* Sets the maximum amount of early data that a client or server would like
 * to exchange. Servers will advertise this value in session tickets sent
 * to a client.
 * A value of zero indicates no early data will be sent by a client, or
 * no early data is accepted by a server (and announced as such in send out
 * session tickets).
 *
 * ssl  The SSL/TLS object.
 * sz   Maximum size of the early data.
 * returns BAD_FUNC_ARG when ssl is NULL, or not using TLS v1.3,
 * and 0 on success.
 */
int wolfSSL_set_max_early_data(WOLFSSL* ssl, unsigned int sz)
{
    if (ssl == NULL || !IsAtLeastTLSv1_3(ssl->version))
        return BAD_FUNC_ARG;

    ssl->options.maxEarlyDataSz = sz;
#if defined(OPENSSL_EXTRA) || defined(WOLFSSL_ERROR_CODE_OPENSSL)
    /* 1 on success in OpenSSL*/
    return WOLFSSL_SUCCESS;
#else
    return 0;
#endif
}

/* Gets the maximum amount of early data that can be seen by server when using
 * session tickets for resumption.
 * A value of zero indicates no early data is to be sent by client using session
 * tickets.
 *
 * ctx  The SSL/TLS CTX object.
 * returns BAD_FUNC_ARG when ctx is NULL, SIDE_ERROR when not a server and
 * returns the maximum amount of early data to be set
 */
int wolfSSL_CTX_get_max_early_data(WOLFSSL_CTX* ctx)
{
    if (ctx == NULL || !IsAtLeastTLSv1_3(ctx->method->version))
        return BAD_FUNC_ARG;
    if (ctx->method->side == WOLFSSL_CLIENT_END)
        return SIDE_ERROR;

    return ctx->maxEarlyDataSz;
}

/* Gets the maximum amount of early data that can be seen by server when using
 * session tickets for resumption.
 * A value of zero indicates no early data is to be sent by client using session
 * tickets.
 *
 * ssl  The SSL/TLS object.
 * returns BAD_FUNC_ARG when ssl is NULL, or not using TLS v1.3,
 * SIDE_ERROR when not a server and
 * returns the maximum amount of early data to be set
 */
int wolfSSL_get_max_early_data(WOLFSSL* ssl)
{
    if (ssl == NULL || !IsAtLeastTLSv1_3(ssl->version))
        return BAD_FUNC_ARG;

    return ssl->options.maxEarlyDataSz;
}

/* Write early data to the server.
 *
 * ssl    The SSL/TLS object.
 * data   Early data to write
 * sz     The size of the early data in bytes.
 * outSz  The number of early data bytes written.
 * returns BAD_FUNC_ARG when: ssl, data or outSz is NULL; sz is negative;
 * or not using TLS v1.3. SIDE ERROR when not a server. Otherwise the number of
 * early data bytes written.
 */
int wolfSSL_write_early_data(WOLFSSL* ssl, const void* data, int sz, int* outSz)
{
    int ret = 0;

    WOLFSSL_ENTER("wolfSSL_write_early_data");

    if (ssl == NULL || data == NULL || sz < 0 || outSz == NULL)
        return BAD_FUNC_ARG;
    if (!IsAtLeastTLSv1_3(ssl->version))
        return BAD_FUNC_ARG;

#ifndef NO_WOLFSSL_CLIENT
    if (ssl->options.side == WOLFSSL_SERVER_END)
        return SIDE_ERROR;

    if (ssl->options.handShakeState == NULL_STATE) {
        if (ssl->error != WC_PENDING_E)
            ssl->earlyData = expecting_early_data;
        ret = wolfSSL_connect_TLSv13(ssl);
        if (ret != WOLFSSL_SUCCESS)
            return WOLFSSL_FATAL_ERROR;
        /* on client side, status is set to rejected        */
        /* until sever accepts the early data extension.    */
        ssl->earlyDataStatus = WOLFSSL_EARLY_DATA_REJECTED;
    }
    if (ssl->options.handShakeState == CLIENT_HELLO_COMPLETE) {
#ifdef OPENSSL_EXTRA
        /* when processed early data exceeds max size */
        if (ssl->session->maxEarlyDataSz > 0 &&
            (ssl->earlyDataSz + sz > ssl->session->maxEarlyDataSz)) {
            ssl->error = TOO_MUCH_EARLY_DATA;
            return WOLFSSL_FATAL_ERROR;
        }
#endif
        ret = SendData(ssl, data, sz);
        if (ret > 0) {
            *outSz = ret;
            /* store amount of processed early data from client */
            ssl->earlyDataSz += ret;
        }
    }
#else
    return SIDE_ERROR;
#endif

    WOLFSSL_LEAVE("wolfSSL_write_early_data", ret);

    if (ret < 0)
        ret = WOLFSSL_FATAL_ERROR;
    return ret;
}

/* Read the any early data from the client.
 *
 * ssl    The SSL/TLS object.
 * data   Buffer to put the early data into.
 * sz     The size of the buffer in bytes.
 * outSz  The number of early data bytes read.
 * returns BAD_FUNC_ARG when: ssl, data or outSz is NULL; sz is negative;
 * or not using TLS v1.3. SIDE ERROR when not a server. Otherwise the number of
 * early data bytes read.
 */
int wolfSSL_read_early_data(WOLFSSL* ssl, void* data, int sz, int* outSz)
{
    int ret = 0;

    WOLFSSL_ENTER("wolfSSL_read_early_data");


    if (ssl == NULL || data == NULL || sz < 0 || outSz == NULL)
        return BAD_FUNC_ARG;
    if (!IsAtLeastTLSv1_3(ssl->version))
        return BAD_FUNC_ARG;

#ifndef NO_WOLFSSL_SERVER
    if (ssl->options.side == WOLFSSL_CLIENT_END)
        return SIDE_ERROR;

    if (ssl->options.handShakeState == NULL_STATE) {
        if (ssl->error != WC_PENDING_E)
            ssl->earlyData = expecting_early_data;
        /* this used to be: ret = wolfSSL_accept_TLSv13(ssl);
         * However, wolfSSL_accept_TLSv13() expects a certificate to
         * be installed already, which is not the case in servers
         * such as HAProxy. They do it after inspecting the ClientHello.
         * The common wolfssl_accept() allows that. */
        ret = wolfSSL_accept(ssl);
        if (ret <= 0)
            return WOLFSSL_FATAL_ERROR;
    }
    if (ssl->options.handShakeState == SERVER_FINISHED_COMPLETE) {
        ret = ReceiveData(ssl, (byte*)data, sz, FALSE);
        if (ret > 0)
            *outSz = ret;
        if (ssl->error == ZERO_RETURN) {
            ssl->error = WOLFSSL_ERROR_NONE;
#ifdef WOLFSSL_DTLS13
            if (ssl->options.dtls) {
                ret = Dtls13DoScheduledWork(ssl);
                if (ret  < 0) {
                    ssl->error = ret;
                    WOLFSSL_ERROR(ssl->error);
                    return WOLFSSL_FATAL_ERROR;
                }
            }
#endif /* WOLFSSL_DTLS13 */
        }
    }
    else
        ret = 0;
#else
    return SIDE_ERROR;
#endif

    WOLFSSL_LEAVE("wolfSSL_read_early_data", ret);

    if (ret < 0)
        ret = WOLFSSL_FATAL_ERROR;
    return ret;
}

/* Returns early data status
 *
 * ssl    The SSL/TLS object.
 * returns WOLFSSL_EARLY_DATA_ACCEPTED if the data was accepted
 *         WOLFSSL_EARLY_DATA_REJECTED if the data was rejected
 *         WOLFSSL_EARLY_DATA_NOT_SENT if no early data was sent
 */
int wolfSSL_get_early_data_status(const WOLFSSL* ssl)
{
    if (ssl == NULL || !IsAtLeastTLSv1_3(ssl->version))
        return BAD_FUNC_ARG;

    return ssl->earlyDataStatus;
}
#endif

#ifdef HAVE_SECRET_CALLBACK
int wolfSSL_set_tls13_secret_cb(WOLFSSL* ssl, Tls13SecretCb cb, void* ctx)
{
    WOLFSSL_ENTER("wolfSSL_set_tls13_secret_cb");
    if (ssl == NULL)
        return WOLFSSL_FATAL_ERROR;

    ssl->tls13SecretCb = cb;
    ssl->tls13SecretCtx = ctx;

    return WOLFSSL_SUCCESS;
}

#if defined(SHOW_SECRETS) && defined(WOLFSSL_SSLKEYLOGFILE)
int tls13ShowSecrets(WOLFSSL* ssl, int id, const unsigned char* secret,
    int secretSz, void* ctx)
{
    int i;
    const char* str = NULL;
    byte clientRandom[RAN_LEN];
    int clientRandomSz;
    XFILE fp;

    (void) ctx;
#ifdef WOLFSSL_SSLKEYLOGFILE_OUTPUT
    fp = XFOPEN(WOLFSSL_SSLKEYLOGFILE_OUTPUT, "ab");
    if (fp == XBADFILE) {
        return BAD_FUNC_ARG;
    }
#else
    fp = stderr;
#endif

    clientRandomSz = (int)wolfSSL_get_client_random(ssl, clientRandom,
        sizeof(clientRandom));

    if (clientRandomSz <= 0) {
        printf("Error getting server random %d\n", clientRandomSz);
    }

#if 0
    printf("TLS Server Secret CB: Rand %d, Secret %d\n",
        serverRandomSz, secretSz);
#endif

    switch (id) {
        case CLIENT_EARLY_TRAFFIC_SECRET:
            str = "CLIENT_EARLY_TRAFFIC_SECRET"; break;
        case EARLY_EXPORTER_SECRET:
            str = "EARLY_EXPORTER_SECRET"; break;
        case CLIENT_HANDSHAKE_TRAFFIC_SECRET:
            str = "CLIENT_HANDSHAKE_TRAFFIC_SECRET"; break;
        case SERVER_HANDSHAKE_TRAFFIC_SECRET:
            str = "SERVER_HANDSHAKE_TRAFFIC_SECRET"; break;
        case CLIENT_TRAFFIC_SECRET:
            str = "CLIENT_TRAFFIC_SECRET_0"; break;
        case SERVER_TRAFFIC_SECRET:
            str = "SERVER_TRAFFIC_SECRET_0"; break;
        case EXPORTER_SECRET:
            str = "EXPORTER_SECRET"; break;
    }

    fprintf(fp, "%s ", str);
    for (i = 0; i < (int)clientRandomSz; i++) {
        fprintf(fp, "%02x", clientRandom[i]);
    }
    fprintf(fp, " ");
    for (i = 0; i < secretSz; i++) {
        fprintf(fp, "%02x", secret[i]);
    }
    fprintf(fp, "\n");

#ifdef WOLFSSL_SSLKEYLOGFILE_OUTPUT
    XFCLOSE(fp);
#endif

    return 0;
}
#endif
#endif

#undef ERROR_OUT

#endif /* !WOLFCRYPT_ONLY */

#endif /* WOLFSSL_TLS13 */
