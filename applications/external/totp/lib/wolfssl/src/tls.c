/* tls.c
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

#ifndef WOLFCRYPT_ONLY

#include <wolfssl/ssl.h>
#include <wolfssl/internal.h>
#include <wolfssl/error-ssl.h>
#include <wolfssl/wolfcrypt/hash.h>
#include <wolfssl/wolfcrypt/hmac.h>
#include <wolfssl/wolfcrypt/kdf.h>
#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

#ifdef HAVE_CURVE25519
    #include <wolfssl/wolfcrypt/curve25519.h>
#endif
#ifdef HAVE_CURVE448
    #include <wolfssl/wolfcrypt/curve448.h>
#endif
#ifdef HAVE_PQC
    #include <wolfssl/wolfcrypt/kyber.h>
#ifdef WOLFSSL_WC_KYBER
    #include <wolfssl/wolfcrypt/wc_kyber.h>
#elif defined(HAVE_LIBOQS) || defined(HAVE_PQM4)
    #include <wolfssl/wolfcrypt/ext_kyber.h>
#endif
#endif

#if defined(WOLFSSL_RENESAS_TSIP_TLS)
    #include <wolfssl/wolfcrypt/port/Renesas/renesas-tsip-crypt.h>
#endif

#include <wolfssl/wolfcrypt/hpke.h>

#ifndef NO_TLS

#if defined(WOLFSSL_TLS13) && defined(HAVE_SUPPORTED_CURVES)
static int TLSX_KeyShare_IsSupported(int namedGroup);
static void TLSX_KeyShare_FreeAll(KeyShareEntry* list, void* heap);
#endif

#ifdef HAVE_SUPPORTED_CURVES
static int TLSX_PopulateSupportedGroups(WOLFSSL* ssl, TLSX** extensions);
#endif

/* Digest enable checks */
#ifdef NO_OLD_TLS /* TLS 1.2 only */
    #if defined(NO_SHA256) && !defined(WOLFSSL_SHA384) && \
            !defined(WOLFSSL_SHA512)
        #error Must have SHA256, SHA384 or SHA512 enabled for TLS 1.2
    #endif
#else  /* TLS 1.1 or older */
    #if defined(NO_MD5) && defined(NO_SHA)
        #error Must have SHA1 and MD5 enabled for old TLS
    #endif
#endif

#ifdef WOLFSSL_TLS13
    #if !defined(NO_DH) && \
        !defined(HAVE_FFDHE_2048) && !defined(HAVE_FFDHE_3072) && \
        !defined(HAVE_FFDHE_4096) && !defined(HAVE_FFDHE_6144) && \
        !defined(HAVE_FFDHE_8192)
        #error Please configure your TLS 1.3 DH key size using either: HAVE_FFDHE_2048, HAVE_FFDHE_3072, HAVE_FFDHE_4096, HAVE_FFDHE_6144 or HAVE_FFDHE_8192
    #endif
    #if !defined(NO_RSA) && !defined(WC_RSA_PSS)
        #error The build option WC_RSA_PSS is required for TLS 1.3 with RSA
    #endif
    #ifndef HAVE_TLS_EXTENSIONS
        #ifndef _MSC_VER
            #error "The build option HAVE_TLS_EXTENSIONS is required for TLS 1.3"
        #else
            #pragma message("Error: The build option HAVE_TLS_EXTENSIONS is required for TLS 1.3")
        #endif
    #endif
#endif

/* Warn if secrets logging is enabled */
#if defined(SHOW_SECRETS) || defined(WOLFSSL_SSLKEYLOGFILE)
    #ifndef _MSC_VER
        #warning The SHOW_SECRETS and WOLFSSL_SSLKEYLOGFILE options should only be used for debugging and never in a production environment
    #else
        #pragma message("Warning: The SHOW_SECRETS and WOLFSSL_SSLKEYLOGFILE options should only be used for debugging and never in a production environment")
    #endif
#endif

#ifndef WOLFSSL_NO_TLS12

#ifdef WOLFSSL_SHA384
    #define HSHASH_SZ WC_SHA384_DIGEST_SIZE
#else
    #define HSHASH_SZ FINISHED_SZ
#endif

int BuildTlsHandshakeHash(WOLFSSL* ssl, byte* hash, word32* hashLen)
{
    int ret = 0;
    word32 hashSz = FINISHED_SZ;

    if (ssl == NULL || hash == NULL || hashLen == NULL || *hashLen < HSHASH_SZ)
        return BAD_FUNC_ARG;

    /* for constant timing perform these even if error */
#ifndef NO_OLD_TLS
    ret |= wc_Md5GetHash(&ssl->hsHashes->hashMd5, hash);
    ret |= wc_ShaGetHash(&ssl->hsHashes->hashSha, &hash[WC_MD5_DIGEST_SIZE]);
#endif

    if (IsAtLeastTLSv1_2(ssl)) {
#ifndef NO_SHA256
        if (ssl->specs.mac_algorithm <= sha256_mac ||
            ssl->specs.mac_algorithm == blake2b_mac) {
            ret |= wc_Sha256GetHash(&ssl->hsHashes->hashSha256, hash);
            hashSz = WC_SHA256_DIGEST_SIZE;
        }
#endif
#ifdef WOLFSSL_SHA384
        if (ssl->specs.mac_algorithm == sha384_mac) {
            ret |= wc_Sha384GetHash(&ssl->hsHashes->hashSha384, hash);
            hashSz = WC_SHA384_DIGEST_SIZE;
        }
#endif
#ifdef WOLFSSL_SM3
        if (ssl->specs.mac_algorithm == sm3_mac) {
            ret |= wc_Sm3GetHash(&ssl->hsHashes->hashSm3, hash);
            hashSz = WC_SM3_DIGEST_SIZE;
        }
#endif
    }

    *hashLen = hashSz;
#ifdef WOLFSSL_CHECK_MEM_ZERO
     wc_MemZero_Add("TLS handshake hash", hash, hashSz);
#endif

    if (ret != 0) {
        ret = BUILD_MSG_ERROR;
        WOLFSSL_ERROR_VERBOSE(ret);
    }

    return ret;
}


int BuildTlsFinished(WOLFSSL* ssl, Hashes* hashes, const byte* sender)
{
    int ret;
    const byte* side = NULL;
    word32 hashSz = HSHASH_SZ;
#if !defined(WOLFSSL_ASYNC_CRYPT) || defined(WC_ASYNC_NO_HASH)
    byte handshake_hash[HSHASH_SZ];
#else
    WC_DECLARE_VAR(handshake_hash, byte, HSHASH_SZ, ssl->heap);
    if (handshake_hash == NULL)
        return MEMORY_E;
#endif

    ret = BuildTlsHandshakeHash(ssl, handshake_hash, &hashSz);
    if (ret == 0) {
        if (XSTRNCMP((const char*)sender, (const char*)kTlsClientStr,
                                                          SIZEOF_SENDER) == 0) {
            side = kTlsClientFinStr;
        }
        else if (XSTRNCMP((const char*)sender, (const char*)kTlsServerStr,
                                                          SIZEOF_SENDER) == 0) {
            side = kTlsServerFinStr;
        }
        else {
            ret = BAD_FUNC_ARG;
            WOLFSSL_MSG("Unexpected sender value");
        }
    }

    if (ret == 0) {
#ifdef WOLFSSL_HAVE_PRF
#if !defined(NO_CERTS) && defined(HAVE_PK_CALLBACKS)
        if (ssl->ctx->TlsFinishedCb) {
            void* ctx = wolfSSL_GetTlsFinishedCtx(ssl);
            ret = ssl->ctx->TlsFinishedCb(ssl, side, handshake_hash, hashSz,
                                          (byte*)hashes, ctx);
        }
        if (!ssl->ctx->TlsFinishedCb || ret == PROTOCOLCB_UNAVAILABLE)
#endif
        {
            PRIVATE_KEY_UNLOCK();
            ret = wc_PRF_TLS((byte*)hashes, TLS_FINISHED_SZ,
                      ssl->arrays->masterSecret, SECRET_LEN, side,
                      FINISHED_LABEL_SZ, handshake_hash, hashSz,
                      IsAtLeastTLSv1_2(ssl), ssl->specs.mac_algorithm,
                      ssl->heap, ssl->devId);
            PRIVATE_KEY_LOCK();
        }
        ForceZero(handshake_hash, hashSz);
#else
        /* Pseudo random function must be enabled in the configuration. */
        ret = PRF_MISSING;
        WOLFSSL_ERROR_VERBOSE(ret);
        WOLFSSL_MSG("Pseudo-random function is not enabled");

        (void)side;
        (void)hashes;
#endif
    }

#if defined(WOLFSSL_ASYNC_CRYPT) && !defined(WC_ASYNC_NO_HASH)
    WC_FREE_VAR(handshake_hash, ssl->heap);
#elif defined(WOLFSSL_CHECK_MEM_ZERO)
    wc_MemZero_Check(handshake_hash, HSHASH_SZ);
#endif

    return ret;
}

#endif /* !WOLFSSL_NO_TLS12 */

#ifndef NO_OLD_TLS

#ifdef WOLFSSL_ALLOW_TLSV10
ProtocolVersion MakeTLSv1(void)
{
    ProtocolVersion pv;
    pv.major = SSLv3_MAJOR;
    pv.minor = TLSv1_MINOR;

    return pv;
}
#endif /* WOLFSSL_ALLOW_TLSV10 */


ProtocolVersion MakeTLSv1_1(void)
{
    ProtocolVersion pv;
    pv.major = SSLv3_MAJOR;
    pv.minor = TLSv1_1_MINOR;

    return pv;
}

#endif /* !NO_OLD_TLS */


#ifndef WOLFSSL_NO_TLS12

ProtocolVersion MakeTLSv1_2(void)
{
    ProtocolVersion pv;
    pv.major = SSLv3_MAJOR;
    pv.minor = TLSv1_2_MINOR;

    return pv;
}

#endif /* !WOLFSSL_NO_TLS12 */

#ifdef WOLFSSL_TLS13
/* The TLS v1.3 protocol version.
 *
 * returns the protocol version data for TLS v1.3.
 */
ProtocolVersion MakeTLSv1_3(void)
{
    ProtocolVersion pv;
    pv.major = SSLv3_MAJOR;
    pv.minor = TLSv1_3_MINOR;

    return pv;
}
#endif

#ifndef WOLFSSL_NO_TLS12

#ifdef HAVE_EXTENDED_MASTER
static const byte ext_master_label[EXT_MASTER_LABEL_SZ + 1] =
                                                      "extended master secret";
#endif
static const byte master_label[MASTER_LABEL_SZ + 1] = "master secret";
static const byte key_label   [KEY_LABEL_SZ + 1]    = "key expansion";

static int _DeriveTlsKeys(byte* key_dig, word32 key_dig_len,
                         const byte* ms, word32 msLen,
                         const byte* sr, const byte* cr,
                         int tls1_2, int hash_type,
                         void* heap, int devId)
{
    int ret;
#if defined(WOLFSSL_ASYNC_CRYPT) && !defined(WC_ASYNC_NO_HASH)
    WC_DECLARE_VAR(seed, byte, SEED_LEN, heap);
    if (seed == NULL)
        return MEMORY_E;
#else
    byte seed[SEED_LEN];
#endif

    XMEMCPY(seed,           sr, RAN_LEN);
    XMEMCPY(seed + RAN_LEN, cr, RAN_LEN);

#ifdef WOLFSSL_HAVE_PRF
    PRIVATE_KEY_UNLOCK();
    ret = wc_PRF_TLS(key_dig, key_dig_len, ms, msLen, key_label, KEY_LABEL_SZ,
               seed, SEED_LEN, tls1_2, hash_type, heap, devId);
    PRIVATE_KEY_LOCK();
#else
    /* Pseudo random function must be enabled in the configuration. */
    ret = PRF_MISSING;
    WOLFSSL_ERROR_VERBOSE(ret);
    WOLFSSL_MSG("Pseudo-random function is not enabled");

    (void)key_dig;
    (void)key_dig_len;
    (void)ms;
    (void)msLen;
    (void)tls1_2;
    (void)hash_type;
    (void)heap;
    (void)devId;
    (void)key_label;
    (void)master_label;
#ifdef HAVE_EXTENDED_MASTER
    (void)ext_master_label;
#endif
#endif

#if defined(WOLFSSL_ASYNC_CRYPT) && !defined(WC_ASYNC_NO_HASH)
    WC_FREE_VAR(seed, heap);
#endif

    return ret;
}

/* External facing wrapper so user can call as well, 0 on success */
int wolfSSL_DeriveTlsKeys(byte* key_dig, word32 key_dig_len,
                         const byte* ms, word32 msLen,
                         const byte* sr, const byte* cr,
                         int tls1_2, int hash_type)
{
    return _DeriveTlsKeys(key_dig, key_dig_len, ms, msLen, sr, cr, tls1_2,
        hash_type, NULL, INVALID_DEVID);
}


int DeriveTlsKeys(WOLFSSL* ssl)
{
    int   ret;
    int   key_dig_len = 2 * ssl->specs.hash_size +
                        2 * ssl->specs.key_size  +
                        2 * ssl->specs.iv_size;
#ifdef WOLFSSL_SMALL_STACK
    byte* key_dig;
#else
    byte  key_dig[MAX_PRF_DIG];
#endif

#ifdef WOLFSSL_SMALL_STACK
    key_dig = (byte*)XMALLOC(MAX_PRF_DIG, ssl->heap, DYNAMIC_TYPE_DIGEST);
    if (key_dig == NULL) {
        return MEMORY_E;
    }
#endif
#if !defined(NO_CERTS) && defined(HAVE_PK_CALLBACKS)
        ret = PROTOCOLCB_UNAVAILABLE;
        if (ssl->ctx->GenSessionKeyCb) {
            void* ctx = wolfSSL_GetGenSessionKeyCtx(ssl);
            ret = ssl->ctx->GenSessionKeyCb(ssl, ctx);
        }
        if (!ssl->ctx->GenSessionKeyCb || ret == PROTOCOLCB_UNAVAILABLE)
#endif
        ret = _DeriveTlsKeys(key_dig, key_dig_len,
                         ssl->arrays->masterSecret, SECRET_LEN,
                         ssl->arrays->serverRandom, ssl->arrays->clientRandom,
                         IsAtLeastTLSv1_2(ssl), ssl->specs.mac_algorithm,
                         ssl->heap, ssl->devId);
    if (ret == 0)
        ret = StoreKeys(ssl, key_dig, PROVISION_CLIENT_SERVER);

#ifdef WOLFSSL_SMALL_STACK
    XFREE(key_dig, ssl->heap, DYNAMIC_TYPE_DIGEST);
#endif

    return ret;
}

static int _MakeTlsMasterSecret(byte* ms, word32 msLen,
                               const byte* pms, word32 pmsLen,
                               const byte* cr, const byte* sr,
                               int tls1_2, int hash_type,
                               void* heap, int devId)
{
    int ret;
#if !defined(WOLFSSL_ASYNC_CRYPT) || defined(WC_ASYNC_NO_HASH)
    byte seed[SEED_LEN];
#else
    WC_DECLARE_VAR(seed, byte, SEED_LEN, heap);
    if (seed == NULL)
        return MEMORY_E;
#endif

    XMEMCPY(seed,           cr, RAN_LEN);
    XMEMCPY(seed + RAN_LEN, sr, RAN_LEN);

#ifdef WOLFSSL_HAVE_PRF
    PRIVATE_KEY_UNLOCK();
    ret = wc_PRF_TLS(ms, msLen, pms, pmsLen, master_label, MASTER_LABEL_SZ,
               seed, SEED_LEN, tls1_2, hash_type, heap, devId);
    PRIVATE_KEY_LOCK();
#else
    /* Pseudo random function must be enabled in the configuration. */
    ret = PRF_MISSING;
    WOLFSSL_MSG("Pseudo-random function is not enabled");

    (void)ms;
    (void)msLen;
    (void)pms;
    (void)pmsLen;
    (void)tls1_2;
    (void)hash_type;
    (void)heap;
    (void)devId;
#endif

#if defined(WOLFSSL_ASYNC_CRYPT) && !defined(WC_ASYNC_NO_HASH)
    WC_FREE_VAR(seed, heap);
#endif

    return ret;
}

/* External facing wrapper so user can call as well, 0 on success */
int wolfSSL_MakeTlsMasterSecret(byte* ms, word32 msLen,
                               const byte* pms, word32 pmsLen,
                               const byte* cr, const byte* sr,
                               int tls1_2, int hash_type)
{
    return _MakeTlsMasterSecret(ms, msLen, pms, pmsLen, cr, sr, tls1_2,
        hash_type, NULL, INVALID_DEVID);
}


#ifdef HAVE_EXTENDED_MASTER

static int _MakeTlsExtendedMasterSecret(byte* ms, word32 msLen,
                                        const byte* pms, word32 pmsLen,
                                        const byte* sHash, word32 sHashLen,
                                        int tls1_2, int hash_type,
                                        void* heap, int devId)
{
    int ret;

#ifdef WOLFSSL_HAVE_PRF
    PRIVATE_KEY_UNLOCK();
    ret = wc_PRF_TLS(ms, msLen, pms, pmsLen, ext_master_label, EXT_MASTER_LABEL_SZ,
               sHash, sHashLen, tls1_2, hash_type, heap, devId);
    PRIVATE_KEY_LOCK();
#else
    /* Pseudo random function must be enabled in the configuration. */
    ret = PRF_MISSING;
    WOLFSSL_MSG("Pseudo-random function is not enabled");

    (void)ms;
    (void)msLen;
    (void)pms;
    (void)pmsLen;
    (void)sHash;
    (void)sHashLen;
    (void)tls1_2;
    (void)hash_type;
    (void)heap;
    (void)devId;
#endif
    return ret;
}

/* External facing wrapper so user can call as well, 0 on success */
int wolfSSL_MakeTlsExtendedMasterSecret(byte* ms, word32 msLen,
                                        const byte* pms, word32 pmsLen,
                                        const byte* sHash, word32 sHashLen,
                                        int tls1_2, int hash_type)
{
    return _MakeTlsExtendedMasterSecret(ms, msLen, pms, pmsLen, sHash, sHashLen,
        tls1_2, hash_type, NULL, INVALID_DEVID);
}

#endif /* HAVE_EXTENDED_MASTER */


int MakeTlsMasterSecret(WOLFSSL* ssl)
{
    int ret;

#ifdef HAVE_EXTENDED_MASTER
    if (ssl->options.haveEMS) {
        word32 hashSz = HSHASH_SZ;
    #ifdef WOLFSSL_SMALL_STACK
        byte* handshake_hash = (byte*)XMALLOC(HSHASH_SZ, ssl->heap,
                                              DYNAMIC_TYPE_DIGEST);
        if (handshake_hash == NULL)
            return MEMORY_E;
    #else
        byte handshake_hash[HSHASH_SZ];
    #endif

        ret = BuildTlsHandshakeHash(ssl, handshake_hash, &hashSz);
        if (ret == 0) {
            ret = _MakeTlsExtendedMasterSecret(
                ssl->arrays->masterSecret, SECRET_LEN,
                ssl->arrays->preMasterSecret, ssl->arrays->preMasterSz,
                handshake_hash, hashSz,
                IsAtLeastTLSv1_2(ssl), ssl->specs.mac_algorithm,
                ssl->heap, ssl->devId);
            ForceZero(handshake_hash, hashSz);
        }

    #ifdef WOLFSSL_SMALL_STACK
        XFREE(handshake_hash, ssl->heap, DYNAMIC_TYPE_DIGEST);
    #elif defined(WOLFSSL_CHECK_MEM_ZERO)
        wc_MemZero_Check(handshake_hash, HSHASH_SZ);
    #endif
    }
    else
#endif /* HAVE_EXTENDED_MASTER */
    {

#if !defined(NO_CERTS) && defined(HAVE_PK_CALLBACKS)
        ret = PROTOCOLCB_UNAVAILABLE;
        if (ssl->ctx->GenMasterCb) {
            void* ctx = wolfSSL_GetGenMasterSecretCtx(ssl);
            ret = ssl->ctx->GenMasterCb(ssl, ctx);
        }
        if (!ssl->ctx->GenMasterCb || ret == PROTOCOLCB_UNAVAILABLE)
#endif
        {
            ret = _MakeTlsMasterSecret(ssl->arrays->masterSecret,
                      SECRET_LEN, ssl->arrays->preMasterSecret,
                      ssl->arrays->preMasterSz, ssl->arrays->clientRandom,
                      ssl->arrays->serverRandom, IsAtLeastTLSv1_2(ssl),
                      ssl->specs.mac_algorithm, ssl->heap, ssl->devId);
        }
    }
    if (ret == 0) {
    #ifdef SHOW_SECRETS
        /* Wireshark Pre-Master-Secret Format:
         *  CLIENT_RANDOM <clientrandom> <mastersecret>
         */
        const char* CLIENT_RANDOM_LABEL = "CLIENT_RANDOM";
        int i, pmsPos = 0;
        char pmsBuf[13 + 1 + 64 + 1 + 96 + 1 + 1];

        XSNPRINTF(&pmsBuf[pmsPos], sizeof(pmsBuf) - pmsPos, "%s ",
            CLIENT_RANDOM_LABEL);
        pmsPos += XSTRLEN(CLIENT_RANDOM_LABEL) + 1;
        for (i = 0; i < RAN_LEN; i++) {
            XSNPRINTF(&pmsBuf[pmsPos], sizeof(pmsBuf) - pmsPos, "%02x",
                ssl->arrays->clientRandom[i]);
            pmsPos += 2;
        }
        XSNPRINTF(&pmsBuf[pmsPos], sizeof(pmsBuf) - pmsPos, " ");
        pmsPos += 1;
        for (i = 0; i < SECRET_LEN; i++) {
            XSNPRINTF(&pmsBuf[pmsPos], sizeof(pmsBuf) - pmsPos, "%02x",
                ssl->arrays->masterSecret[i]);
            pmsPos += 2;
        }
        XSNPRINTF(&pmsBuf[pmsPos], sizeof(pmsBuf) - pmsPos, "\n");
        pmsPos += 1;

        /* print master secret */
        puts(pmsBuf);

        #if !defined(NO_FILESYSTEM) && defined(WOLFSSL_SSLKEYLOGFILE)
        {
            FILE* f = XFOPEN(WOLFSSL_SSLKEYLOGFILE_OUTPUT, "a");
            if (f != XBADFILE) {
                XFWRITE(pmsBuf, 1, pmsPos, f);
                XFCLOSE(f);
            }
        }
        #endif
    #endif /* SHOW_SECRETS */

        ret = DeriveTlsKeys(ssl);
    }

    return ret;
}


/* Used by EAP-TLS and EAP-TTLS to derive keying material from
 * the master_secret. */
int wolfSSL_make_eap_keys(WOLFSSL* ssl, void* msk, unsigned int len,
                                                              const char* label)
{
    int   ret;
#ifdef WOLFSSL_SMALL_STACK
    byte* seed;
#else
    byte  seed[SEED_LEN];
#endif

#ifdef WOLFSSL_SMALL_STACK
    seed = (byte*)XMALLOC(SEED_LEN, ssl->heap, DYNAMIC_TYPE_SEED);
    if (seed == NULL)
        return MEMORY_E;
#endif

    /*
     * As per RFC-5281, the order of the client and server randoms is reversed
     * from that used by the TLS protocol to derive keys.
     */
    XMEMCPY(seed,           ssl->arrays->clientRandom, RAN_LEN);
    XMEMCPY(seed + RAN_LEN, ssl->arrays->serverRandom, RAN_LEN);

#ifdef WOLFSSL_HAVE_PRF
    PRIVATE_KEY_UNLOCK();
    ret = wc_PRF_TLS((byte*)msk, len, ssl->arrays->masterSecret, SECRET_LEN,
              (const byte *)label, (word32)XSTRLEN(label), seed, SEED_LEN,
              IsAtLeastTLSv1_2(ssl), ssl->specs.mac_algorithm,
              ssl->heap, ssl->devId);
    PRIVATE_KEY_LOCK();
#else
    /* Pseudo random function must be enabled in the configuration. */
    ret = PRF_MISSING;
    WOLFSSL_MSG("Pseudo-random function is not enabled");

    (void)msk;
    (void)len;
    (void)label;
#endif

#ifdef WOLFSSL_SMALL_STACK
    XFREE(seed, ssl->heap, DYNAMIC_TYPE_SEED);
#endif

    return ret;
}

int wolfSSL_GetHmacType_ex(CipherSpecs* specs)
{
    if (specs == NULL)
        return BAD_FUNC_ARG;

    switch (specs->mac_algorithm) {
        #ifndef NO_MD5
        case md5_mac:
        {
            return WC_MD5;
        }
        #endif
        #ifndef NO_SHA256
        case sha256_mac:
        {
            return WC_SHA256;
        }
        #endif
        #ifdef WOLFSSL_SHA384
        case sha384_mac:
        {
            return WC_SHA384;
        }
        #endif
        #ifdef WOLFSSL_SM3
        case sm3_mac:
        {
            return WC_SM3;
        }
        #endif
        #ifndef NO_SHA
        case sha_mac:
        {
            return WC_SHA;
        }
        #endif
        #ifdef HAVE_BLAKE2
        case blake2b_mac:
        {
            return BLAKE2B_ID;
        }
        #endif
        default:
        {
            return WOLFSSL_FATAL_ERROR;
        }
    }
}

/* return HMAC digest type in wolfSSL format */
int wolfSSL_GetHmacType(WOLFSSL* ssl)
{
    if (ssl == NULL)
        return BAD_FUNC_ARG;

    return wolfSSL_GetHmacType_ex(&ssl->specs);
}


int wolfSSL_SetTlsHmacInner(WOLFSSL* ssl, byte* inner, word32 sz, int content,
                           int verify)
{
    if (ssl == NULL || inner == NULL)
        return BAD_FUNC_ARG;

    XMEMSET(inner, 0, WOLFSSL_TLS_HMAC_INNER_SZ);

    WriteSEQ(ssl, verify, inner);
    inner[SEQ_SZ] = (byte)content;
    inner[SEQ_SZ + ENUM_LEN]            = ssl->version.major;
    inner[SEQ_SZ + ENUM_LEN + ENUM_LEN] = ssl->version.minor;
    c16toa((word16)sz, inner + SEQ_SZ + ENUM_LEN + VERSION_SZ);

    return 0;
}


#ifndef WOLFSSL_AEAD_ONLY
#if !defined(WOLFSSL_NO_HASH_RAW) && !defined(HAVE_FIPS) && \
    !defined(HAVE_SELFTEST)

/* Update the hash in the HMAC.
 *
 * hmac  HMAC object.
 * data  Data to be hashed.
 * sz    Size of data to hash.
 * returns 0 on success, otherwise failure.
 */
static int Hmac_HashUpdate(Hmac* hmac, const byte* data, word32 sz)
{
    int ret = BAD_FUNC_ARG;

    switch (hmac->macType) {
    #ifndef NO_SHA
        case WC_SHA:
            ret = wc_ShaUpdate(&hmac->hash.sha, data, sz);
            break;
    #endif /* !NO_SHA */

    #ifndef NO_SHA256
        case WC_SHA256:
            ret = wc_Sha256Update(&hmac->hash.sha256, data, sz);
            break;
    #endif /* !NO_SHA256 */

    #ifdef WOLFSSL_SHA384
        case WC_SHA384:
            ret = wc_Sha384Update(&hmac->hash.sha384, data, sz);
            break;
    #endif /* WOLFSSL_SHA384 */

    #ifdef WOLFSSL_SHA512
        case WC_SHA512:
            ret = wc_Sha512Update(&hmac->hash.sha512, data, sz);
            break;
    #endif /* WOLFSSL_SHA512 */

    #ifdef WOLFSSL_SM3
        case WC_SM3:
            ret = wc_Sm3Update(&hmac->hash.sm3, data, sz);
            break;
    #endif /* WOLFSSL_SM3 */

        default:
            break;
    }

    return ret;
}

/* Finalize the hash but don't put the EOC, padding or length in.
 *
 * hmac  HMAC object.
 * hash  Hash result.
 * returns 0 on success, otherwise failure.
 */
static int Hmac_HashFinalRaw(Hmac* hmac, unsigned char* hash)
{
    int ret = BAD_FUNC_ARG;

    switch (hmac->macType) {
    #ifndef NO_SHA
        case WC_SHA:
            ret = wc_ShaFinalRaw(&hmac->hash.sha, hash);
            break;
    #endif /* !NO_SHA */

    #ifndef NO_SHA256
        case WC_SHA256:
            ret = wc_Sha256FinalRaw(&hmac->hash.sha256, hash);
            break;
    #endif /* !NO_SHA256 */

    #ifdef WOLFSSL_SHA384
        case WC_SHA384:
            ret = wc_Sha384FinalRaw(&hmac->hash.sha384, hash);
            break;
    #endif /* WOLFSSL_SHA384 */

    #ifdef WOLFSSL_SHA512
        case WC_SHA512:
            ret = wc_Sha512FinalRaw(&hmac->hash.sha512, hash);
            break;
    #endif /* WOLFSSL_SHA512 */

    #ifdef WOLFSSL_SM3
        case WC_SM3:
            ret = wc_Sm3FinalRaw(&hmac->hash.sm3, hash);
            break;
    #endif /* WOLFSSL_SM3 */

        default:
            break;
    }

    return ret;
}

/* Finalize the HMAC by performing outer hash.
 *
 * hmac  HMAC object.
 * mac   MAC result.
 * returns 0 on success, otherwise failure.
 */
static int Hmac_OuterHash(Hmac* hmac, unsigned char* mac)
{
    int ret = BAD_FUNC_ARG;
    wc_HashAlg hash;
    enum wc_HashType hashType = (enum wc_HashType)hmac->macType;
    int digestSz = wc_HashGetDigestSize(hashType);
    int blockSz = wc_HashGetBlockSize(hashType);

    if ((digestSz >= 0) && (blockSz >= 0)) {
        ret = wc_HashInit(&hash, hashType);
    }
    if (ret == 0) {
        ret = wc_HashUpdate(&hash, hashType, (byte*)hmac->opad,
            blockSz);
        if (ret == 0)
            ret = wc_HashUpdate(&hash, hashType, (byte*)hmac->innerHash,
                digestSz);
        if (ret == 0)
            ret = wc_HashFinal(&hash, hashType, mac);
        wc_HashFree(&hash, hashType);
    }

    return ret;
}

/* Calculate the HMAC of the header + message data.
 * Constant time implementation using wc_Sha*FinalRaw().
 *
 * hmac    HMAC object.
 * digest  MAC result.
 * in      Message data.
 * sz      Size of the message data.
 * header  Constructed record header with length of handshake data.
 * returns 0 on success, otherwise failure.
 */
static int Hmac_UpdateFinal_CT(Hmac* hmac, byte* digest, const byte* in,
                               word32 sz, int macLen, byte* header)
{
    byte         lenBytes[8];
    int          i, j;
    unsigned int k;
    int          blockBits, blockMask;
    int          lastBlockLen, extraLen, eocIndex;
    int          blocks, safeBlocks, lenBlock, eocBlock;
    unsigned int maxLen;
    int          blockSz, padSz;
    int          ret;
    word32       realLen;
    byte         extraBlock;

    switch (hmac->macType) {
    #ifndef NO_SHA
        case WC_SHA:
            blockSz = WC_SHA_BLOCK_SIZE;
            blockBits = 6;
            padSz = WC_SHA_BLOCK_SIZE - WC_SHA_PAD_SIZE + 1;
            break;
    #endif /* !NO_SHA */

    #ifndef NO_SHA256
        case WC_SHA256:
            blockSz = WC_SHA256_BLOCK_SIZE;
            blockBits = 6;
            padSz = WC_SHA256_BLOCK_SIZE - WC_SHA256_PAD_SIZE + 1;
            break;
    #endif /* !NO_SHA256 */

    #ifdef WOLFSSL_SHA384
        case WC_SHA384:
            blockSz = WC_SHA384_BLOCK_SIZE;
            blockBits = 7;
            padSz = WC_SHA384_BLOCK_SIZE - WC_SHA384_PAD_SIZE + 1;
            break;
    #endif /* WOLFSSL_SHA384 */

    #ifdef WOLFSSL_SHA512
        case WC_SHA512:
            blockSz = WC_SHA512_BLOCK_SIZE;
            blockBits = 7;
            padSz = WC_SHA512_BLOCK_SIZE - WC_SHA512_PAD_SIZE + 1;
            break;
    #endif /* WOLFSSL_SHA512 */

    #ifdef WOLFSSL_SM3
        case WC_SM3:
            blockSz = WC_SM3_BLOCK_SIZE;
            blockBits = 6;
            padSz = WC_SM3_BLOCK_SIZE - WC_SM3_PAD_SIZE + 1;
            break;
    #endif /* WOLFSSL_SM3 */

        default:
            return BAD_FUNC_ARG;
    }
    blockMask = blockSz - 1;

    /* Size of data to HMAC if padding length byte is zero. */
    maxLen = WOLFSSL_TLS_HMAC_INNER_SZ + sz - 1 - macLen;
    /* Complete data (including padding) has block for EOC and/or length. */
    extraBlock = ctSetLTE((maxLen + padSz) & blockMask, padSz);
    /* Total number of blocks for data including padding. */
    blocks = ((maxLen + blockSz - 1) >> blockBits) + extraBlock;
    /* Up to last 6 blocks can be hashed safely. */
    safeBlocks = blocks - 6;

    /* Length of message data. */
    realLen = maxLen - in[sz - 1];
    /* Number of message bytes in last block. */
    lastBlockLen = realLen & blockMask;
    /* Number of padding bytes in last block. */
    extraLen = ((blockSz * 2 - padSz - lastBlockLen) & blockMask) + 1;
    /* Number of blocks to create for hash. */
    lenBlock = (realLen + extraLen) >> blockBits;
    /* Block containing EOC byte. */
    eocBlock = realLen >> blockBits;
    /* Index of EOC byte in block. */
    eocIndex = realLen & blockMask;

    /* Add length of hmac's ipad to total length. */
    realLen += blockSz;
    /* Length as bits - 8 bytes bigendian. */
    c32toa(realLen >> ((sizeof(word32) * 8) - 3), lenBytes);
    c32toa(realLen << 3, lenBytes + sizeof(word32));

    ret = Hmac_HashUpdate(hmac, (unsigned char*)hmac->ipad, blockSz);
    if (ret != 0)
        return ret;

    XMEMSET(hmac->innerHash, 0, macLen);

    if (safeBlocks > 0) {
        ret = Hmac_HashUpdate(hmac, header, WOLFSSL_TLS_HMAC_INNER_SZ);
        if (ret != 0)
            return ret;
        ret = Hmac_HashUpdate(hmac, in, safeBlocks * blockSz -
                                                     WOLFSSL_TLS_HMAC_INNER_SZ);
        if (ret != 0)
            return ret;
    }
    else
        safeBlocks = 0;

    XMEMSET(digest, 0, macLen);
    k = safeBlocks * blockSz;
    for (i = safeBlocks; i < blocks; i++) {
        unsigned char hashBlock[WC_MAX_BLOCK_SIZE];
        unsigned char isEocBlock = ctMaskEq(i, eocBlock);
        unsigned char isOutBlock = ctMaskEq(i, lenBlock);

        for (j = 0; j < blockSz; j++) {
            unsigned char atEoc = ctMaskEq(j, eocIndex) & isEocBlock;
            unsigned char pastEoc = ctMaskGT(j, eocIndex) & isEocBlock;
            unsigned char b = 0;

            if (k < WOLFSSL_TLS_HMAC_INNER_SZ)
                b = header[k];
            else if (k < maxLen)
                b = in[k - WOLFSSL_TLS_HMAC_INNER_SZ];
            k++;

            b = ctMaskSel(atEoc, 0x80, b);
            b &= (unsigned char)~(word32)pastEoc;
            b &= ((unsigned char)~(word32)isOutBlock) | isEocBlock;

            if (j >= blockSz - 8) {
                b = ctMaskSel(isOutBlock, lenBytes[j - (blockSz - 8)], b);
            }

            hashBlock[j] = b;
        }

        ret = Hmac_HashUpdate(hmac, hashBlock, blockSz);
        if (ret != 0)
            return ret;
        ret = Hmac_HashFinalRaw(hmac, hashBlock);
        if (ret != 0)
            return ret;
        for (j = 0; j < macLen; j++)
            ((unsigned char*)hmac->innerHash)[j] |= hashBlock[j] & isOutBlock;
    }

    ret = Hmac_OuterHash(hmac, digest);

    return ret;
}

#endif

#if defined(WOLFSSL_NO_HASH_RAW) || defined(HAVE_FIPS) || \
    defined(HAVE_SELFTEST) || defined(HAVE_BLAKE2)

/* Calculate the HMAC of the header + message data.
 * Constant time implementation using normal hashing operations.
 * Update-Final need to be constant time.
 *
 * hmac    HMAC object.
 * digest  MAC result.
 * in      Message data.
 * sz      Size of the message data.
 * header  Constructed record header with length of handshake data.
 * returns 0 on success, otherwise failure.
 */
static int Hmac_UpdateFinal(Hmac* hmac, byte* digest, const byte* in,
                            word32 sz, byte* header)
{
    byte       dummy[WC_MAX_BLOCK_SIZE] = {0};
    int        ret = 0;
    word32     msgSz, blockSz, macSz, padSz, maxSz, realSz;
    word32     offset = 0;
    int        msgBlocks, blocks, blockBits;
    int        i;

    switch (hmac->macType) {
    #ifndef NO_SHA
        case WC_SHA:
            blockSz = WC_SHA_BLOCK_SIZE;
            blockBits = 6;
            macSz = WC_SHA_DIGEST_SIZE;
            padSz = WC_SHA_BLOCK_SIZE - WC_SHA_PAD_SIZE + 1;
            break;
    #endif /* !NO_SHA */

    #ifndef NO_SHA256
        case WC_SHA256:
            blockSz = WC_SHA256_BLOCK_SIZE;
            blockBits = 6;
            macSz = WC_SHA256_DIGEST_SIZE;
            padSz = WC_SHA256_BLOCK_SIZE - WC_SHA256_PAD_SIZE + 1;
            break;
    #endif /* !NO_SHA256 */

    #ifdef WOLFSSL_SHA384
        case WC_SHA384:
            blockSz = WC_SHA384_BLOCK_SIZE;
            blockBits = 7;
            macSz = WC_SHA384_DIGEST_SIZE;
            padSz = WC_SHA384_BLOCK_SIZE - WC_SHA384_PAD_SIZE + 1;
            break;
    #endif /* WOLFSSL_SHA384 */

    #ifdef WOLFSSL_SHA512
        case WC_SHA512:
            blockSz = WC_SHA512_BLOCK_SIZE;
            blockBits = 7;
            macSz = WC_SHA512_DIGEST_SIZE;
            padSz = WC_SHA512_BLOCK_SIZE - WC_SHA512_PAD_SIZE + 1;
            break;
    #endif /* WOLFSSL_SHA512 */

    #ifdef HAVE_BLAKE2
        case WC_HASH_TYPE_BLAKE2B:
            blockSz = BLAKE2B_BLOCKBYTES;
            blockBits = 7;
            macSz = BLAKE2B_256;
            padSz = 0;
            break;
    #endif /* HAVE_BLAKE2 */

    #ifdef WOLFSSL_SM3
        case WC_SM3:
            blockSz = WC_SM3_BLOCK_SIZE;
            blockBits = 6;
            macSz = WC_SM3_DIGEST_SIZE;
            padSz = WC_SM3_BLOCK_SIZE - WC_SM3_PAD_SIZE + 1;
            break;
    #endif

        default:
            WOLFSSL_MSG("ERROR: Hmac_UpdateFinal failed, no hmac->macType");
            return BAD_FUNC_ARG;
    }

    msgSz = sz - (1 + in[sz - 1] + macSz);
    /* Make negative result 0 */
    msgSz &= ~(0 - (msgSz >> 31));
    realSz = WOLFSSL_TLS_HMAC_INNER_SZ + msgSz;
    maxSz = WOLFSSL_TLS_HMAC_INNER_SZ + (sz - 1) - macSz;

    /* Calculate #blocks processed in HMAC for max and real data. */
    blocks      = maxSz >> blockBits;
    blocks     += ((maxSz + padSz) % blockSz) < padSz;
    msgBlocks   = realSz >> blockBits;
    /* #Extra blocks to process. */
    blocks -= msgBlocks + ((((realSz + padSz) % blockSz) < padSz) ? 1 : 0);
    /* Calculate whole blocks. */
    msgBlocks--;

    ret = wc_HmacUpdate(hmac, header, WOLFSSL_TLS_HMAC_INNER_SZ);
    if (ret == 0) {
        /* Fill the rest of the block with any available data. */
        word32 currSz = ctMaskLT(msgSz, blockSz) & msgSz;
        currSz |= ctMaskGTE(msgSz, blockSz) & blockSz;
        currSz -= WOLFSSL_TLS_HMAC_INNER_SZ;
        currSz &= ~(0 - (currSz >> 31));
        ret = wc_HmacUpdate(hmac, in, currSz);
        offset = currSz;
    }
    if (ret == 0) {
        /* Do the hash operations on a block basis. */
        for (i = 0; i < msgBlocks; i++, offset += blockSz) {
            ret = wc_HmacUpdate(hmac, in + offset, blockSz);
            if (ret != 0)
                break;
        }
    }
    if (ret == 0)
        ret = wc_HmacUpdate(hmac, in + offset, msgSz - offset);
    if (ret == 0)
        ret = wc_HmacFinal(hmac, digest);
    if (ret == 0) {
        /* Do the dummy hash operations. Do at least one. */
        for (i = 0; i < blocks + 1; i++) {
            ret = wc_HmacUpdate(hmac, dummy, blockSz);
            if (ret != 0)
                break;
        }
    }

    return ret;
}

#endif

int TLS_hmac(WOLFSSL* ssl, byte* digest, const byte* in, word32 sz, int padSz,
             int content, int verify, int epochOrder)
{
    Hmac   hmac;
    byte   myInner[WOLFSSL_TLS_HMAC_INNER_SZ];
    int    ret = 0;
    const byte* macSecret = NULL;
    word32 hashSz = 0;

    if (ssl == NULL)
        return BAD_FUNC_ARG;

#ifdef HAVE_TRUNCATED_HMAC
    hashSz = ssl->truncated_hmac ? (byte)TRUNCATED_HMAC_SZ
                                        : ssl->specs.hash_size;
#else
    hashSz = ssl->specs.hash_size;
#endif

#ifdef HAVE_FUZZER
    /* Fuzz "in" buffer with sz to be used in HMAC algorithm */
    if (ssl->fuzzerCb) {
        if (verify && padSz >= 0) {
            ssl->fuzzerCb(ssl, in, sz + hashSz + padSz + 1, FUZZ_HMAC,
                          ssl->fuzzerCtx);
        }
        else {
            ssl->fuzzerCb(ssl, in, sz, FUZZ_HMAC, ssl->fuzzerCtx);
        }
    }
#endif

    if (!ssl->options.dtls)
        wolfSSL_SetTlsHmacInner(ssl, myInner, sz, content, verify);
    else
        wolfSSL_SetTlsHmacInner(ssl, myInner, sz, content, epochOrder);

    ret = wc_HmacInit(&hmac, ssl->heap, ssl->devId);
    if (ret != 0)
        return ret;


#ifdef WOLFSSL_DTLS
    if (ssl->options.dtls)
        macSecret = wolfSSL_GetDtlsMacSecret(ssl, verify, epochOrder);
    else
        macSecret = wolfSSL_GetMacSecret(ssl, verify);
#else
    macSecret = wolfSSL_GetMacSecret(ssl, verify);
#endif
    ret = wc_HmacSetKey(&hmac, wolfSSL_GetHmacType(ssl),
                                              macSecret,
                                              ssl->specs.hash_size);

    if (ret == 0) {
        /* Constant time verification required. */
        if (verify && padSz >= 0) {
#if !defined(WOLFSSL_NO_HASH_RAW) && !defined(HAVE_FIPS) && \
    !defined(HAVE_SELFTEST)
    #ifdef HAVE_BLAKE2
            if (wolfSSL_GetHmacType(ssl) == WC_HASH_TYPE_BLAKE2B) {
                ret = Hmac_UpdateFinal(&hmac, digest, in,
                                              sz + hashSz + padSz + 1, myInner);
            }
            else
    #endif
            {
                ret = Hmac_UpdateFinal_CT(&hmac, digest, in,
                                      sz + hashSz + padSz + 1, hashSz, myInner);
            }
#else
            ret = Hmac_UpdateFinal(&hmac, digest, in, sz + hashSz + padSz + 1,
                                                                       myInner);
#endif
        }
        else {
            ret = wc_HmacUpdate(&hmac, myInner, sizeof(myInner));
            if (ret == 0)
                ret = wc_HmacUpdate(&hmac, in, sz);                /* content */
            if (ret == 0)
                ret = wc_HmacFinal(&hmac, digest);
        }
    }

    wc_HmacFree(&hmac);

    return ret;
}
#endif /* WOLFSSL_AEAD_ONLY */

#endif /* !WOLFSSL_NO_TLS12 */

#ifdef HAVE_TLS_EXTENSIONS

/**
 * The TLSX semaphore is used to calculate the size of the extensions to be sent
 * from one peer to another.
 */

/** Supports up to 72 flags. Increase as needed. */
#define SEMAPHORE_SIZE 9

/**
 * Converts the extension type (id) to an index in the semaphore.
 *
 * Official reference for TLS extension types:
 *   http://www.iana.org/assignments/tls-extensiontype-values/tls-extensiontype-values.xml
 *
 * Motivation:
 *   Previously, we used the extension type itself as the index of that
 *   extension in the semaphore as the extension types were declared
 *   sequentially, but maintain a semaphore as big as the number of available
 *   extensions is no longer an option since the release of renegotiation_info.
 *
 * How to update:
 *   Assign extension types that extrapolate the number of available semaphores
 *   to the first available index going backwards in the semaphore array.
 *   When adding a new extension type that don't extrapolate the number of
 *   available semaphores, check for a possible collision with with a
 *   'remapped' extension type.
 *
 * Update TLSX_Parse for duplicate detection if more added above 62.
 */
static WC_INLINE word16 TLSX_ToSemaphore(word16 type)
{
    switch (type) {

        case TLSX_RENEGOTIATION_INFO: /* 0xFF01 */
            return 63;
#ifdef WOLFSSL_QUIC
        case TLSX_KEY_QUIC_TP_PARAMS_DRAFT: /* 0xffa5 */
            return 64;
#endif
#if defined(WOLFSSL_TLS13) && defined(HAVE_ECH)
        case TLSX_ECH: /* 0xfe0d */
            return 65;
#endif
        default:
            if (type > 62) {
                /* This message SHOULD only happens during the adding of
                   new TLS extensions in which its IANA number overflows
                   the current semaphore's range, or if its number already
                   is assigned to be used by another extension.
                   Use this check value for the new extension and decrement
                   the check value by one. */
                WOLFSSL_MSG("### TLSX semaphore collision or overflow detected!");
            }
    }

    return type;
}

/** Checks if a specific light (tls extension) is not set in the semaphore. */
#define IS_OFF(semaphore, light) \
    (!(((semaphore)[(light) / 8] &  (byte) (0x01 << ((light) % 8)))))

/** Turn on a specific light (tls extension) in the semaphore. */
/* the semaphore marks the extensions already written to the message */
#define TURN_ON(semaphore, light) \
    ((semaphore)[(light) / 8] |= (byte) (0x01 << ((light) % 8)))

/** Turn off a specific light (tls extension) in the semaphore. */
#define TURN_OFF(semaphore, light) \
    ((semaphore)[(light) / 8] &= (byte) ~(0x01 << ((light) % 8)))

/** Creates a new extension. */
static TLSX* TLSX_New(TLSX_Type type, const void* data, void* heap)
{
    TLSX* extension = (TLSX*)XMALLOC(sizeof(TLSX), heap, DYNAMIC_TYPE_TLSX);

    (void)heap;

    if (extension) {
        extension->type = type;
        extension->data = (void*)data;
        extension->resp = 0;
        extension->next = NULL;
    }

    return extension;
}

/**
 * Creates a new extension and appends it to the provided list.
 * Checks for duplicate extensions, keeps the newest.
 */
int TLSX_Append(TLSX** list, TLSX_Type type, const void* data, void* heap)
{
    TLSX* extension = TLSX_New(type, data, heap);
    TLSX* cur;
    TLSX** prevNext = list;

    if (extension == NULL)
        return MEMORY_E;

    for (cur = *list; cur != NULL;) {
        if (cur->type == type) {
            *prevNext = cur->next;
            cur->next = NULL;
            TLSX_FreeAll(cur, heap);
            cur = *prevNext;
        }
        else {
            prevNext = &cur->next;
            cur = cur->next;
        }
    }

    /* Append the extension to the list */
    *prevNext = extension;

    return 0;
}

/**
 * Creates a new extension and pushes it to the provided list.
 * Checks for duplicate extensions, keeps the newest.
 */
int TLSX_Push(TLSX** list, TLSX_Type type, const void* data, void* heap)
{
    TLSX* extension = TLSX_New(type, data, heap);

    if (extension == NULL)
        return MEMORY_E;

    /* pushes the new extension on the list. */
    extension->next = *list;
    *list = extension;

    /* remove duplicate extensions, there should be only one of each type. */
    do {
        if (extension->next && extension->next->type == type) {
            TLSX *next = extension->next;

            extension->next = next->next;
            next->next = NULL;

            TLSX_FreeAll(next, heap);

            /* there is no way to occur more than
             * two extensions of the same type.
             */
            break;
        }
    } while ((extension = extension->next));

    return 0;
}

#ifndef NO_WOLFSSL_CLIENT

int TLSX_CheckUnsupportedExtension(WOLFSSL* ssl, TLSX_Type type);

int TLSX_CheckUnsupportedExtension(WOLFSSL* ssl, TLSX_Type type)
{
    TLSX *extension = TLSX_Find(ssl->extensions, type);

    if (!extension)
        extension = TLSX_Find(ssl->ctx->extensions, type);

    return extension == NULL;
}

int TLSX_HandleUnsupportedExtension(WOLFSSL* ssl);

int TLSX_HandleUnsupportedExtension(WOLFSSL* ssl)
{
    SendAlert(ssl, alert_fatal, unsupported_extension);
    WOLFSSL_ERROR_VERBOSE(UNSUPPORTED_EXTENSION);
    return UNSUPPORTED_EXTENSION;
}

#else

#define TLSX_CheckUnsupportedExtension(ssl, type) 0
#define TLSX_HandleUnsupportedExtension(ssl) 0

#endif

#if !defined(NO_WOLFSSL_SERVER) || defined(WOLFSSL_TLS13)
void TLSX_SetResponse(WOLFSSL* ssl, TLSX_Type type);
/** Mark an extension to be sent back to the client. */
void TLSX_SetResponse(WOLFSSL* ssl, TLSX_Type type)
{
    TLSX *extension = TLSX_Find(ssl->extensions, type);

    if (extension)
        extension->resp = 1;
}
#endif

/******************************************************************************/
/* Application-Layer Protocol Negotiation                                     */
/******************************************************************************/

#ifdef HAVE_ALPN
/** Creates a new ALPN object, providing protocol name to use. */
static ALPN* TLSX_ALPN_New(char *protocol_name, word16 protocol_nameSz,
                                                                     void* heap)
{
    ALPN *alpn;

    WOLFSSL_ENTER("TLSX_ALPN_New");

    if (protocol_name == NULL ||
        protocol_nameSz > WOLFSSL_MAX_ALPN_PROTO_NAME_LEN) {
        WOLFSSL_MSG("Invalid arguments");
        return NULL;
    }

    alpn = (ALPN*)XMALLOC(sizeof(ALPN), heap, DYNAMIC_TYPE_TLSX);
    if (alpn == NULL) {
        WOLFSSL_MSG("Memory failure");
        return NULL;
    }

    alpn->next = NULL;
    alpn->negotiated = 0;
    alpn->options = 0;

    alpn->protocol_name = (char*)XMALLOC(protocol_nameSz + 1,
                                         heap, DYNAMIC_TYPE_TLSX);
    if (alpn->protocol_name == NULL) {
        WOLFSSL_MSG("Memory failure");
        XFREE(alpn, heap, DYNAMIC_TYPE_TLSX);
        return NULL;
    }

    XMEMCPY(alpn->protocol_name, protocol_name, protocol_nameSz);
    alpn->protocol_name[protocol_nameSz] = 0;

    (void)heap;

    return alpn;
}

/** Releases an ALPN object. */
static void TLSX_ALPN_Free(ALPN *alpn, void* heap)
{
    (void)heap;

    if (alpn == NULL)
        return;

    XFREE(alpn->protocol_name, heap, DYNAMIC_TYPE_TLSX);
    XFREE(alpn, heap, DYNAMIC_TYPE_TLSX);
}

/** Releases all ALPN objects in the provided list. */
static void TLSX_ALPN_FreeAll(ALPN *list, void* heap)
{
    ALPN* alpn;

    while ((alpn = list)) {
        list = alpn->next;
        TLSX_ALPN_Free(alpn, heap);
    }
}

/** Tells the buffered size of the ALPN objects in a list. */
static word16 TLSX_ALPN_GetSize(ALPN *list)
{
    ALPN* alpn;
    word16 length = OPAQUE16_LEN; /* list length */

    while ((alpn = list)) {
        list = alpn->next;

        length++; /* protocol name length is on one byte */
        length += (word16)XSTRLEN(alpn->protocol_name);
    }

    return length;
}

/** Writes the ALPN objects of a list in a buffer. */
static word16 TLSX_ALPN_Write(ALPN *list, byte *output)
{
    ALPN* alpn;
    word16 length = 0;
    word16 offset = OPAQUE16_LEN; /* list length offset */

    while ((alpn = list)) {
        list = alpn->next;

        length = (word16)XSTRLEN(alpn->protocol_name);

        /* protocol name length */
        output[offset++] = (byte)length;

        /* protocol name value */
        XMEMCPY(output + offset, alpn->protocol_name, length);

        offset += length;
    }

    /* writing list length */
    c16toa(offset - OPAQUE16_LEN, output);

    return offset;
}

/** Finds a protocol name in the provided ALPN list */
static ALPN* TLSX_ALPN_Find(ALPN *list, char *protocol_name, word16 size)
{
    ALPN *alpn;

    if (list == NULL || protocol_name == NULL)
        return NULL;

    alpn = list;
    while (alpn != NULL && (
           (word16)XSTRLEN(alpn->protocol_name) != size ||
           XSTRNCMP(alpn->protocol_name, protocol_name, size)))
        alpn = alpn->next;

    return alpn;
}

/** Set the ALPN matching client and server requirements */
static int TLSX_SetALPN(TLSX** extensions, const void* data, word16 size,
                                                                     void* heap)
{
    ALPN *alpn;
    int  ret;

    if (extensions == NULL || data == NULL)
        return BAD_FUNC_ARG;

    alpn = TLSX_ALPN_New((char *)data, size, heap);
    if (alpn == NULL) {
        WOLFSSL_MSG("Memory failure");
        return MEMORY_E;
    }

    alpn->negotiated = 1;

    ret = TLSX_Push(extensions, TLSX_APPLICATION_LAYER_PROTOCOL, (void*)alpn,
                                                                          heap);
    if (ret != 0) {
        TLSX_ALPN_Free(alpn, heap);
        return ret;
    }

    return WOLFSSL_SUCCESS;
}

static int ALPN_find_match(WOLFSSL *ssl, TLSX **pextension,
                           const byte **psel, byte *psel_len,
                           const byte *alpn_val, word16 alpn_val_len)
{
    TLSX    *extension;
    ALPN    *alpn, *list;
    const byte *sel = NULL, *s;
    byte sel_len = 0, wlen;

    extension = TLSX_Find(ssl->extensions, TLSX_APPLICATION_LAYER_PROTOCOL);
    if (extension == NULL)
        extension = TLSX_Find(ssl->ctx->extensions,
                              TLSX_APPLICATION_LAYER_PROTOCOL);

    /* No ALPN configured here */
    if (extension == NULL || extension->data == NULL) {
        *pextension = NULL;
        *psel = NULL;
        *psel_len = 0;
        return 0;
    }

    list = (ALPN*)extension->data;
    for (s = alpn_val;
         (s - alpn_val) < alpn_val_len;
         s += wlen) {
        wlen = *s++; /* bounds already checked on save */
        alpn = TLSX_ALPN_Find(list, (char*)s, wlen);
        if (alpn != NULL) {
            WOLFSSL_MSG("ALPN protocol match");
            sel = s,
            sel_len = wlen;
            break;
        }
    }

    if (sel == NULL) {
        WOLFSSL_MSG("No ALPN protocol match");

        /* do nothing if no protocol match between client and server and option
         is set to continue (like OpenSSL) */
        if (list->options & WOLFSSL_ALPN_CONTINUE_ON_MISMATCH) {
            WOLFSSL_MSG("Continue on mismatch");
        }
        else {
            SendAlert(ssl, alert_fatal, no_application_protocol);
            WOLFSSL_ERROR_VERBOSE(UNKNOWN_ALPN_PROTOCOL_NAME_E);
            return UNKNOWN_ALPN_PROTOCOL_NAME_E;
        }
    }

    *pextension = extension;
    *psel = sel;
    *psel_len = sel_len;
    return 0;
}

int ALPN_Select(WOLFSSL *ssl)
{
    TLSX *extension;
    const byte *sel = NULL;
    byte sel_len = 0;
    int r = 0;

    WOLFSSL_ENTER("ALPN_Select");
    if (ssl->alpn_peer_requested == NULL)
        return 0;

#if defined(OPENSSL_ALL) || defined(WOLFSSL_NGINX) || defined(WOLFSSL_HAPROXY)
    if (ssl->alpnSelect != NULL && ssl->options.side == WOLFSSL_SERVER_END) {
        r = ssl->alpnSelect(ssl, &sel, &sel_len, ssl->alpn_peer_requested,
                ssl->alpn_peer_requested_length, ssl->alpnSelectArg);
        switch (r) {
            case SSL_TLSEXT_ERR_OK:
                WOLFSSL_MSG("ALPN protocol match");
                break;
            case SSL_TLSEXT_ERR_NOACK:
                WOLFSSL_MSG("ALPN cb no match but not fatal");
                sel = NULL;
                sel_len = 0;
                break;
            case SSL_TLSEXT_ERR_ALERT_FATAL:
            default:
                WOLFSSL_MSG("ALPN cb no match and fatal");
                SendAlert(ssl, alert_fatal, no_application_protocol);
                WOLFSSL_ERROR_VERBOSE(UNKNOWN_ALPN_PROTOCOL_NAME_E);
                return UNKNOWN_ALPN_PROTOCOL_NAME_E;
        }
    }
    else
#endif
    {
        r = ALPN_find_match(ssl, &extension, &sel, &sel_len,
                            ssl->alpn_peer_requested,
                            ssl->alpn_peer_requested_length);
        if (r != 0)
            return r;
    }

    if (sel != NULL) {
        /* set the matching negotiated protocol */
        r = TLSX_SetALPN(&ssl->extensions, sel, sel_len, ssl->heap);
        if (r != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("TLSX_SetALPN failed");
            return BUFFER_ERROR;
        }
        /* reply to ALPN extension sent from peer */
#ifndef NO_WOLFSSL_SERVER
        TLSX_SetResponse(ssl, TLSX_APPLICATION_LAYER_PROTOCOL);
#endif
    }
    return 0;
}

/** Parses a buffer of ALPN extensions and set the first one matching
 * client and server requirements */
static int TLSX_ALPN_ParseAndSet(WOLFSSL *ssl, const byte *input, word16 length,
                                 byte isRequest)
{
    word16  size = 0, offset = 0, wlen;
    int     r = BUFFER_ERROR;
    const byte *s;

    if (OPAQUE16_LEN > length)
        return BUFFER_ERROR;

    ato16(input, &size);
    offset += OPAQUE16_LEN;

    /* validating alpn list length */
    if (size == 0 || length != OPAQUE16_LEN + size)
        return BUFFER_ERROR;

    /* validating length of entries before accepting */
    for (s = input + offset; (s - input) < size; s += wlen) {
        wlen = *s++;
        if (wlen == 0 || (s + wlen - input) > length)
            return BUFFER_ERROR;
    }

    if (isRequest) {
        /* keep the list sent by peer, if this is from a request. We
         * use it later in ALPN_Select() for evaluation. */
        if (ssl->alpn_peer_requested != NULL) {
            XFREE(ssl->alpn_peer_requested, ssl->heap, DYNAMIC_TYPE_ALPN);
            ssl->alpn_peer_requested_length = 0;
        }
        ssl->alpn_peer_requested = (byte *)XMALLOC(size, ssl->heap,
                                                   DYNAMIC_TYPE_ALPN);
        if (ssl->alpn_peer_requested == NULL) {
            return MEMORY_ERROR;
        }
        ssl->alpn_peer_requested_length = size;
        XMEMCPY(ssl->alpn_peer_requested, (char*)input + offset, size);
    }
    else {
        /* a response, we should find the value in our config */
        const byte *sel = NULL;
        byte sel_len = 0;
        TLSX *extension = NULL;

        r = ALPN_find_match(ssl, &extension, &sel, &sel_len, input + offset, size);
        if (r != 0)
            return r;

        if (sel != NULL) {
            /* set the matching negotiated protocol */
            r = TLSX_SetALPN(&ssl->extensions, sel, sel_len, ssl->heap);
            if (r != WOLFSSL_SUCCESS) {
                WOLFSSL_MSG("TLSX_SetALPN failed");
                return BUFFER_ERROR;
            }
        }
        /* If we had nothing configured, the response is unexpected */
        else if (extension == NULL) {
            r = TLSX_HandleUnsupportedExtension(ssl);
            if (r != 0)
                return r;
        }
    }
    return 0;
}

/** Add a protocol name to the list of accepted usable ones */
int TLSX_UseALPN(TLSX** extensions, const void* data, word16 size, byte options,
                                                                     void* heap)
{
    ALPN *alpn;
    TLSX *extension;
    int  ret;

    if (extensions == NULL || data == NULL)
        return BAD_FUNC_ARG;

    alpn = TLSX_ALPN_New((char *)data, size, heap);
    if (alpn == NULL) {
        WOLFSSL_MSG("Memory failure");
        return MEMORY_E;
    }

    /* Set Options of ALPN */
    alpn->options = options;

    extension = TLSX_Find(*extensions, TLSX_APPLICATION_LAYER_PROTOCOL);
    if (extension == NULL) {
        ret = TLSX_Push(extensions, TLSX_APPLICATION_LAYER_PROTOCOL,
                                                             (void*)alpn, heap);
        if (ret != 0) {
            TLSX_ALPN_Free(alpn, heap);
            return ret;
        }
    }
    else {
        /* push new ALPN object to extension data. */
        alpn->next = (ALPN*)extension->data;
        extension->data = (void*)alpn;
    }

    return WOLFSSL_SUCCESS;
}

/** Get the protocol name set by the server */
int TLSX_ALPN_GetRequest(TLSX* extensions, void** data, word16 *dataSz)
{
    TLSX *extension;
    ALPN *alpn;

    if (extensions == NULL || data == NULL || dataSz == NULL)
        return BAD_FUNC_ARG;

    *data = NULL;
    *dataSz = 0;

    extension = TLSX_Find(extensions, TLSX_APPLICATION_LAYER_PROTOCOL);
    if (extension == NULL) {
        WOLFSSL_MSG("TLS extension not found");
        WOLFSSL_ERROR_VERBOSE(WOLFSSL_ALPN_NOT_FOUND);
        return WOLFSSL_ALPN_NOT_FOUND;
    }

    alpn = (ALPN *)extension->data;
    if (alpn == NULL) {
        WOLFSSL_MSG("ALPN extension not found");
        WOLFSSL_ERROR_VERBOSE(WOLFSSL_FATAL_ERROR);
        return WOLFSSL_FATAL_ERROR;
    }

    if (alpn->negotiated != 1) {

        /* consider as an error */
        if (alpn->options & WOLFSSL_ALPN_FAILED_ON_MISMATCH) {
            WOLFSSL_MSG("No protocol match with peer -> Failed");
            WOLFSSL_ERROR_VERBOSE(WOLFSSL_FATAL_ERROR);
            return WOLFSSL_FATAL_ERROR;
        }

        /* continue without negotiated protocol */
        WOLFSSL_MSG("No protocol match with peer -> Continue");
        WOLFSSL_ERROR_VERBOSE(WOLFSSL_ALPN_NOT_FOUND);
        return WOLFSSL_ALPN_NOT_FOUND;
    }

    if (alpn->next != NULL) {
        WOLFSSL_MSG("Only one protocol name must be accepted");
        WOLFSSL_ERROR_VERBOSE(WOLFSSL_FATAL_ERROR);
        return WOLFSSL_FATAL_ERROR;
    }

    *data = alpn->protocol_name;
    *dataSz = (word16)XSTRLEN((char*)*data);

    return WOLFSSL_SUCCESS;
}

#define ALPN_FREE_ALL     TLSX_ALPN_FreeAll
#define ALPN_GET_SIZE     TLSX_ALPN_GetSize
#define ALPN_WRITE        TLSX_ALPN_Write
#define ALPN_PARSE        TLSX_ALPN_ParseAndSet

#else /* HAVE_ALPN */

#define ALPN_FREE_ALL(list, heap) WC_DO_NOTHING
#define ALPN_GET_SIZE(list)     0
#define ALPN_WRITE(a, b)        0
#define ALPN_PARSE(a, b, c, d)  0

#endif /* HAVE_ALPN */

/******************************************************************************/
/* Server Name Indication                                                     */
/******************************************************************************/

#ifdef HAVE_SNI

/** Creates a new SNI object. */
static SNI* TLSX_SNI_New(byte type, const void* data, word16 size, void* heap)
{
    SNI* sni = (SNI*)XMALLOC(sizeof(SNI), heap, DYNAMIC_TYPE_TLSX);

    (void)heap;

    if (sni) {
        sni->type = type;
        sni->next = NULL;

    #ifndef NO_WOLFSSL_SERVER
        sni->options = 0;
        sni->status  = WOLFSSL_SNI_NO_MATCH;
    #endif

        switch (sni->type) {
            case WOLFSSL_SNI_HOST_NAME:
                sni->data.host_name = (char*)XMALLOC(size + 1, heap,
                                                     DYNAMIC_TYPE_TLSX);
                if (sni->data.host_name) {
                    XSTRNCPY(sni->data.host_name, (const char*)data, size);
                    sni->data.host_name[size] = '\0';
                } else {
                    XFREE(sni, heap, DYNAMIC_TYPE_TLSX);
                    sni = NULL;
                }
            break;

            default: /* invalid type */
                XFREE(sni, heap, DYNAMIC_TYPE_TLSX);
                sni = NULL;
        }
    }

    return sni;
}

/** Releases a SNI object. */
static void TLSX_SNI_Free(SNI* sni, void* heap)
{
    if (sni) {
        switch (sni->type) {
            case WOLFSSL_SNI_HOST_NAME:
                XFREE(sni->data.host_name, heap, DYNAMIC_TYPE_TLSX);
            break;
        }

        XFREE(sni, heap, DYNAMIC_TYPE_TLSX);
    }
    (void)heap;
}

/** Releases all SNI objects in the provided list. */
static void TLSX_SNI_FreeAll(SNI* list, void* heap)
{
    SNI* sni;

    while ((sni = list)) {
        list = sni->next;
        TLSX_SNI_Free(sni, heap);
    }
}

/** Tells the buffered size of the SNI objects in a list. */
static word16 TLSX_SNI_GetSize(SNI* list)
{
    SNI* sni;
    word16 length = OPAQUE16_LEN; /* list length */

    while ((sni = list)) {
        list = sni->next;

        length += ENUM_LEN + OPAQUE16_LEN; /* sni type + sni length */

        switch (sni->type) {
            case WOLFSSL_SNI_HOST_NAME:
                length += (word16)XSTRLEN((char*)sni->data.host_name);
            break;
        }
    }

    return length;
}

/** Writes the SNI objects of a list in a buffer. */
static word16 TLSX_SNI_Write(SNI* list, byte* output)
{
    SNI* sni;
    word16 length = 0;
    word16 offset = OPAQUE16_LEN; /* list length offset */

    while ((sni = list)) {
        list = sni->next;

        output[offset++] = sni->type; /* sni type */

        switch (sni->type) {
            case WOLFSSL_SNI_HOST_NAME:
                length = (word16)XSTRLEN((char*)sni->data.host_name);

                c16toa(length, output + offset); /* sni length */
                offset += OPAQUE16_LEN;

                XMEMCPY(output + offset, sni->data.host_name, length);

                offset += length;
            break;
        }
    }

    c16toa(offset - OPAQUE16_LEN, output); /* writing list length */

    return offset;
}

/** Finds a SNI object in the provided list. */
static SNI* TLSX_SNI_Find(SNI *list, byte type)
{
    SNI* sni = list;

    while (sni && sni->type != type)
        sni = sni->next;

    return sni;
}

#if (!defined(NO_WOLFSSL_CLIENT) || !defined(NO_WOLFSSL_SERVER))
/** Sets the status of a SNI object. */
static void TLSX_SNI_SetStatus(TLSX* extensions, byte type, byte status)
{
    TLSX* extension = TLSX_Find(extensions, TLSX_SERVER_NAME);
    SNI* sni = TLSX_SNI_Find(extension ? (SNI*)extension->data : NULL, type);

    if (sni)
        sni->status = status;
}
#endif

/** Gets the status of a SNI object. */
byte TLSX_SNI_Status(TLSX* extensions, byte type)
{
    TLSX* extension = TLSX_Find(extensions, TLSX_SERVER_NAME);
    SNI* sni = TLSX_SNI_Find(extension ? (SNI*)extension->data : NULL, type);

    if (sni)
        return sni->status;

    return 0;
}

/** Parses a buffer of SNI extensions. */
static int TLSX_SNI_Parse(WOLFSSL* ssl, const byte* input, word16 length,
                          byte isRequest)
{
#ifndef NO_WOLFSSL_SERVER
    word16 size = 0;
    word16 offset = 0;
    int cacheOnly = 0;
    SNI *sni = NULL;
    byte type;
    byte matched;
#if defined(WOLFSSL_TLS13) && defined(HAVE_ECH)
    WOLFSSL_ECH* ech = NULL;
    WOLFSSL_EchConfig* workingConfig;
    TLSX* echX;
#endif
#endif /* !NO_WOLFSSL_SERVER */
    TLSX *extension = TLSX_Find(ssl->extensions, TLSX_SERVER_NAME);

    if (!extension)
        extension = TLSX_Find(ssl->ctx->extensions, TLSX_SERVER_NAME);

    if (!isRequest) {
        #ifndef NO_WOLFSSL_CLIENT
            if (!extension || !extension->data)
                return TLSX_HandleUnsupportedExtension(ssl);

            if (length > 0)
                return BUFFER_ERROR; /* SNI response MUST be empty. */

            /* This call enables wolfSSL_SNI_GetRequest() to be called in the
             * client side to fetch the used SNI. It will only work if the SNI
             * was set at the SSL object level. Right now we only support one
             * name type, WOLFSSL_SNI_HOST_NAME, but in the future, the
             * inclusion of other name types will turn this method inaccurate,
             * as the extension response doesn't contains information of which
             * name was accepted.
             */
            TLSX_SNI_SetStatus(ssl->extensions, WOLFSSL_SNI_HOST_NAME,
                                                        WOLFSSL_SNI_REAL_MATCH);

            return 0;
        #endif
    }

#ifndef NO_WOLFSSL_SERVER
    if (!extension || !extension->data) {
        /* This will keep SNI even though TLSX_UseSNI has not been called.
         * Enable it so that the received sni is available to functions
         * that use a custom callback when SNI is received.
         */
    #ifdef WOLFSSL_ALWAYS_KEEP_SNI
        cacheOnly = 1;
    #endif
        if (ssl->ctx->sniRecvCb) {
            cacheOnly = 1;
        }

        if (cacheOnly) {
            WOLFSSL_MSG("Forcing SSL object to store SNI parameter");
        }
        else {
            /* Skipping, SNI not enabled at server side. */
            return 0;
        }
    }

    if (OPAQUE16_LEN > length)
        return BUFFER_ERROR;

    ato16(input, &size);
    offset += OPAQUE16_LEN;

    /* validating sni list length */
    if (length != OPAQUE16_LEN + size || size == 0)
        return BUFFER_ERROR;

    /* SNI was badly specified and only one type is now recognized and allowed.
     * Only one SNI value per type (RFC6066), so, no loop. */
    type = input[offset++];
    if (type != WOLFSSL_SNI_HOST_NAME)
        return BUFFER_ERROR;

    if (offset + OPAQUE16_LEN > length)
        return BUFFER_ERROR;
    ato16(input + offset, &size);
    offset += OPAQUE16_LEN;

    if (offset + size != length || size == 0)
        return BUFFER_ERROR;

    if (!cacheOnly && !(sni = TLSX_SNI_Find((SNI*)extension->data, type)))
        return 0; /* not using this type of SNI. */

#ifdef WOLFSSL_TLS13
    /* Don't process the second ClientHello SNI extension if there
     * was problems with the first.
     */
    if (!cacheOnly && sni->status != 0)
        return 0;
#endif
    matched = cacheOnly || (XSTRLEN(sni->data.host_name) == size &&
         XSTRNCMP(sni->data.host_name, (const char*)input + offset, size) == 0);

#if defined(WOLFSSL_TLS13) && defined(HAVE_ECH)
    echX = TLSX_Find(ssl->extensions, TLSX_ECH);
    if (echX != NULL)
        ech = (WOLFSSL_ECH*)(echX->data);

    if (!matched && ech != NULL) {
        workingConfig = ech->echConfig;

        while (workingConfig != NULL) {
            matched = XSTRLEN(workingConfig->publicName) == size &&
                XSTRNCMP(workingConfig->publicName,
                (const char*)input + offset, size) == 0;

            if (matched)
                break;

            workingConfig = workingConfig->next;
        }
    }
#endif

    if (matched || sni->options & WOLFSSL_SNI_ANSWER_ON_MISMATCH) {
        int matchStat;
        int r = TLSX_UseSNI(&ssl->extensions, type, input + offset, size,
                                                                     ssl->heap);
        if (r != WOLFSSL_SUCCESS)
            return r; /* throws error. */

        if (cacheOnly) {
            WOLFSSL_MSG("Forcing storage of SNI, Fake match");
            matchStat = WOLFSSL_SNI_FORCE_KEEP;
        }
        else if (matched) {
            WOLFSSL_MSG("SNI did match!");
            matchStat = WOLFSSL_SNI_REAL_MATCH;
        }
        else {
            WOLFSSL_MSG("fake SNI match from ANSWER_ON_MISMATCH");
            matchStat = WOLFSSL_SNI_FAKE_MATCH;
        }

        TLSX_SNI_SetStatus(ssl->extensions, type, (byte)matchStat);

        if (!cacheOnly)
            TLSX_SetResponse(ssl, TLSX_SERVER_NAME);
    }
    else if (!(sni->options & WOLFSSL_SNI_CONTINUE_ON_MISMATCH)) {
        SendAlert(ssl, alert_fatal, unrecognized_name);
        WOLFSSL_ERROR_VERBOSE(UNKNOWN_SNI_HOST_NAME_E);
        return UNKNOWN_SNI_HOST_NAME_E;
    }
#else
    (void)input;
#endif /* !NO_WOLFSSL_SERVER */

#if defined(NO_WOLFSSL_CLIENT) && defined(NO_WOLFSSL_SERVER)
    (void)length;
#endif

    return 0;
}

static int TLSX_SNI_VerifyParse(WOLFSSL* ssl,  byte isRequest)
{
    (void)ssl;

    if (isRequest) {
    #ifndef NO_WOLFSSL_SERVER
        TLSX* ctx_ext = TLSX_Find(ssl->ctx->extensions, TLSX_SERVER_NAME);
        TLSX* ssl_ext = TLSX_Find(ssl->extensions,      TLSX_SERVER_NAME);
        SNI* ctx_sni = ctx_ext ? (SNI*)ctx_ext->data : NULL;
        SNI* ssl_sni = ssl_ext ? (SNI*)ssl_ext->data : NULL;
        SNI* sni = NULL;

        for (; ctx_sni; ctx_sni = ctx_sni->next) {
            if (ctx_sni->options & WOLFSSL_SNI_ABORT_ON_ABSENCE) {
                sni = TLSX_SNI_Find(ssl_sni, ctx_sni->type);

                if (sni) {
                    if (sni->status != WOLFSSL_SNI_NO_MATCH)
                        continue;

                    /* if ssl level overrides ctx level, it is ok. */
                    if ((sni->options & WOLFSSL_SNI_ABORT_ON_ABSENCE) == 0)
                        continue;
                }

                SendAlert(ssl, alert_fatal, handshake_failure);
                WOLFSSL_ERROR_VERBOSE(SNI_ABSENT_ERROR);
                return SNI_ABSENT_ERROR;
            }
        }

        for (; ssl_sni; ssl_sni = ssl_sni->next) {
            if (ssl_sni->options & WOLFSSL_SNI_ABORT_ON_ABSENCE) {
                if (ssl_sni->status != WOLFSSL_SNI_NO_MATCH)
                    continue;

                SendAlert(ssl, alert_fatal, handshake_failure);
                WOLFSSL_ERROR_VERBOSE(SNI_ABSENT_ERROR);
                return SNI_ABSENT_ERROR;
            }
        }
    #endif /* NO_WOLFSSL_SERVER */
    }

    return 0;
}

int TLSX_UseSNI(TLSX** extensions, byte type, const void* data, word16 size,
                                                                     void* heap)
{
    TLSX* extension;
    SNI* sni = NULL;

    if (extensions == NULL || data == NULL)
        return BAD_FUNC_ARG;

    if ((sni = TLSX_SNI_New(type, data, size, heap)) == NULL)
        return MEMORY_E;

    extension = TLSX_Find(*extensions, TLSX_SERVER_NAME);
    if (!extension) {
        int ret = TLSX_Push(extensions, TLSX_SERVER_NAME, (void*)sni, heap);

        if (ret != 0) {
            TLSX_SNI_Free(sni, heap);
            return ret;
        }
    }
    else {
        /* push new SNI object to extension data. */
        sni->next = (SNI*)extension->data;
        extension->data = (void*)sni;

        /* remove duplicate SNI, there should be only one of each type. */
        do {
            if (sni->next && sni->next->type == type) {
                SNI* next = sni->next;

                sni->next = next->next;
                TLSX_SNI_Free(next, heap);

                /* there is no way to occur more than
                 * two SNIs of the same type.
                 */
                break;
            }
        } while ((sni = sni->next));
    }

    return WOLFSSL_SUCCESS;
}

#ifndef NO_WOLFSSL_SERVER

/** Tells the SNI requested by the client. */
word16 TLSX_SNI_GetRequest(TLSX* extensions, byte type, void** data)
{
    TLSX* extension = TLSX_Find(extensions, TLSX_SERVER_NAME);
    SNI* sni = TLSX_SNI_Find(extension ? (SNI*)extension->data : NULL, type);

    if (sni && sni->status != WOLFSSL_SNI_NO_MATCH) {
        switch (sni->type) {
            case WOLFSSL_SNI_HOST_NAME:
                if (data) {
                    *data = sni->data.host_name;
                    return (word16)XSTRLEN((char*)*data);
                }
        }
    }

    return 0;
}

/** Sets the options for a SNI object. */
void TLSX_SNI_SetOptions(TLSX* extensions, byte type, byte options)
{
    TLSX* extension = TLSX_Find(extensions, TLSX_SERVER_NAME);
    SNI* sni = TLSX_SNI_Find(extension ? (SNI*)extension->data : NULL, type);

    if (sni)
        sni->options = options;
}

/** Retrieves a SNI request from a client hello buffer. */
int TLSX_SNI_GetFromBuffer(const byte* clientHello, word32 helloSz,
                           byte type, byte* sni, word32* inOutSz)
{
    word32 offset = 0;
    word32 len32 = 0;
    word16 len16 = 0;

    if (helloSz < RECORD_HEADER_SZ + HANDSHAKE_HEADER_SZ + CLIENT_HELLO_FIRST)
        return INCOMPLETE_DATA;

    /* TLS record header */
    if ((enum ContentType) clientHello[offset++] != handshake) {

        /* checking for SSLv2.0 client hello according to: */
        /* http://tools.ietf.org/html/rfc4346#appendix-E.1 */
        if ((enum HandShakeType) clientHello[++offset] == client_hello) {
            offset += ENUM_LEN + VERSION_SZ; /* skip version */

            ato16(clientHello + offset, &len16);
            offset += OPAQUE16_LEN;

            if (len16 % 3) /* cipher_spec_length must be multiple of 3 */
                return BUFFER_ERROR;

            ato16(clientHello + offset, &len16);
            /* Returning SNI_UNSUPPORTED do not increment offset here */

            if (len16 != 0) /* session_id_length must be 0 */
                return BUFFER_ERROR;

            WOLFSSL_ERROR_VERBOSE(SNI_UNSUPPORTED);
            return SNI_UNSUPPORTED;
        }

        return BUFFER_ERROR;
    }

    if (clientHello[offset++] != SSLv3_MAJOR)
        return BUFFER_ERROR;

    if (clientHello[offset++] < TLSv1_MINOR) {
        WOLFSSL_ERROR_VERBOSE(SNI_UNSUPPORTED);
        return SNI_UNSUPPORTED;
    }

    ato16(clientHello + offset, &len16);
    offset += OPAQUE16_LEN;

    if (offset + len16 > helloSz)
        return INCOMPLETE_DATA;

    /* Handshake header */
    if ((enum HandShakeType) clientHello[offset] != client_hello)
        return BUFFER_ERROR;

    c24to32(clientHello + offset + 1, &len32);
    offset += HANDSHAKE_HEADER_SZ;

    if (offset + len32 > helloSz)
        return BUFFER_ERROR;

    /* client hello */
    offset += VERSION_SZ + RAN_LEN; /* version, random */

    if (helloSz < offset + clientHello[offset])
        return BUFFER_ERROR;

    offset += ENUM_LEN + clientHello[offset]; /* skip session id */

    /* cypher suites */
    if (helloSz < offset + OPAQUE16_LEN)
        return BUFFER_ERROR;

    ato16(clientHello + offset, &len16);
    offset += OPAQUE16_LEN;

    if (helloSz < offset + len16)
        return BUFFER_ERROR;

    offset += len16; /* skip cypher suites */

    /* compression methods */
    if (helloSz < offset + 1)
        return BUFFER_ERROR;

    if (helloSz < offset + clientHello[offset])
        return BUFFER_ERROR;

    offset += ENUM_LEN + clientHello[offset]; /* skip compression methods */

    /* extensions */
    if (helloSz < offset + OPAQUE16_LEN)
        return 0; /* no extensions in client hello. */

    ato16(clientHello + offset, &len16);
    offset += OPAQUE16_LEN;

    if (helloSz < offset + len16)
        return BUFFER_ERROR;

    while (len16 >= OPAQUE16_LEN + OPAQUE16_LEN) {
        word16 extType;
        word16 extLen;

        ato16(clientHello + offset, &extType);
        offset += OPAQUE16_LEN;

        ato16(clientHello + offset, &extLen);
        offset += OPAQUE16_LEN;

        if (helloSz < offset + extLen)
            return BUFFER_ERROR;

        if (extType != TLSX_SERVER_NAME) {
            offset += extLen; /* skip extension */
        } else {
            word16 listLen;

            ato16(clientHello + offset, &listLen);
            offset += OPAQUE16_LEN;

            if (helloSz < offset + listLen)
                return BUFFER_ERROR;

            while (listLen > ENUM_LEN + OPAQUE16_LEN) {
                byte   sniType = clientHello[offset++];
                word16 sniLen;

                ato16(clientHello + offset, &sniLen);
                offset += OPAQUE16_LEN;

                if (helloSz < offset + sniLen)
                    return BUFFER_ERROR;

                if (sniType != type) {
                    offset  += sniLen;
                    listLen -= min(ENUM_LEN + OPAQUE16_LEN + sniLen, listLen);
                    continue;
                }

                *inOutSz = min(sniLen, *inOutSz);
                XMEMCPY(sni, clientHello + offset, *inOutSz);

                return WOLFSSL_SUCCESS;
            }
        }

        len16 -= min(2 * OPAQUE16_LEN + extLen, len16);
    }

    return len16 ? BUFFER_ERROR : 0;
}

#endif

#define SNI_FREE_ALL     TLSX_SNI_FreeAll
#define SNI_GET_SIZE     TLSX_SNI_GetSize
#define SNI_WRITE        TLSX_SNI_Write
#define SNI_PARSE        TLSX_SNI_Parse
#define SNI_VERIFY_PARSE TLSX_SNI_VerifyParse

#else

#define SNI_FREE_ALL(list, heap) WC_DO_NOTHING
#define SNI_GET_SIZE(list)     0
#define SNI_WRITE(a, b)        0
#define SNI_PARSE(a, b, c, d)  0
#define SNI_VERIFY_PARSE(a, b) 0

#endif /* HAVE_SNI */

/******************************************************************************/
/* Trusted CA Key Indication                                                  */
/******************************************************************************/

#ifdef HAVE_TRUSTED_CA

/** Creates a new TCA object. */
static TCA* TLSX_TCA_New(byte type, const byte* id, word16 idSz, void* heap)
{
    TCA* tca = (TCA*)XMALLOC(sizeof(TCA), heap, DYNAMIC_TYPE_TLSX);

    if (tca) {
        XMEMSET(tca, 0, sizeof(TCA));
        tca->type = type;

        switch (type) {
            case WOLFSSL_TRUSTED_CA_PRE_AGREED:
                break;

            #ifndef NO_SHA
            case WOLFSSL_TRUSTED_CA_KEY_SHA1:
            case WOLFSSL_TRUSTED_CA_CERT_SHA1:
                if (idSz == WC_SHA_DIGEST_SIZE &&
                        (tca->id =
                            (byte*)XMALLOC(idSz, heap, DYNAMIC_TYPE_TLSX))) {
                    XMEMCPY(tca->id, id, idSz);
                    tca->idSz = idSz;
                }
                else {
                    XFREE(tca, heap, DYNAMIC_TYPE_TLSX);
                    tca = NULL;
                }
                break;
            #endif

            case WOLFSSL_TRUSTED_CA_X509_NAME:
                if (idSz > 0 &&
                        (tca->id =
                            (byte*)XMALLOC(idSz, heap, DYNAMIC_TYPE_TLSX))) {
                    XMEMCPY(tca->id, id, idSz);
                    tca->idSz = idSz;
                }
                else {
                    XFREE(tca, heap, DYNAMIC_TYPE_TLSX);
                    tca = NULL;
                }
                break;

            default: /* invalid type */
                XFREE(tca, heap, DYNAMIC_TYPE_TLSX);
                tca = NULL;
        }
    }

    (void)heap;

    return tca;
}

/** Releases a TCA object. */
static void TLSX_TCA_Free(TCA* tca, void* heap)
{
    (void)heap;

    if (tca) {
        if (tca->id)
            XFREE(tca->id, heap, DYNAMIC_TYPE_TLSX);
        XFREE(tca, heap, DYNAMIC_TYPE_TLSX);
    }
}

/** Releases all TCA objects in the provided list. */
static void TLSX_TCA_FreeAll(TCA* list, void* heap)
{
    TCA* tca;

    while ((tca = list)) {
        list = tca->next;
        TLSX_TCA_Free(tca, heap);
    }
}

/** Tells the buffered size of the TCA objects in a list. */
static word16 TLSX_TCA_GetSize(TCA* list)
{
    TCA* tca;
    word16 length = OPAQUE16_LEN; /* list length */

    while ((tca = list)) {
        list = tca->next;

        length += ENUM_LEN; /* tca type */

        switch (tca->type) {
            case WOLFSSL_TRUSTED_CA_PRE_AGREED:
                break;
            case WOLFSSL_TRUSTED_CA_KEY_SHA1:
            case WOLFSSL_TRUSTED_CA_CERT_SHA1:
                length += tca->idSz;
                break;
            case WOLFSSL_TRUSTED_CA_X509_NAME:
                length += OPAQUE16_LEN + tca->idSz;
                break;
        }
    }

    return length;
}

/** Writes the TCA objects of a list in a buffer. */
static word16 TLSX_TCA_Write(TCA* list, byte* output)
{
    TCA* tca;
    word16 offset = OPAQUE16_LEN; /* list length offset */

    while ((tca = list)) {
        list = tca->next;

        output[offset++] = tca->type; /* tca type */

        switch (tca->type) {
            case WOLFSSL_TRUSTED_CA_PRE_AGREED:
                break;
            #ifndef NO_SHA
            case WOLFSSL_TRUSTED_CA_KEY_SHA1:
            case WOLFSSL_TRUSTED_CA_CERT_SHA1:
                if (tca->id != NULL) {
                    XMEMCPY(output + offset, tca->id, tca->idSz);
                    offset += tca->idSz;
                }
                else {
                    /* ID missing. Set to an empty string. */
                    c16toa(0, output + offset);
                    offset += OPAQUE16_LEN;
                }
                break;
            #endif
            case WOLFSSL_TRUSTED_CA_X509_NAME:
                if (tca->id != NULL) {
                    c16toa(tca->idSz, output + offset); /* tca length */
                    offset += OPAQUE16_LEN;
                    XMEMCPY(output + offset, tca->id, tca->idSz);
                    offset += tca->idSz;
                }
                else {
                    /* ID missing. Set to an empty string. */
                    c16toa(0, output + offset);
                    offset += OPAQUE16_LEN;
                }
                break;
            default:
                /* ID unknown. Set to an empty string. */
                c16toa(0, output + offset);
                offset += OPAQUE16_LEN;
        }
    }

    c16toa(offset - OPAQUE16_LEN, output); /* writing list length */

    return offset;
}

#ifndef NO_WOLFSSL_SERVER
static TCA* TLSX_TCA_Find(TCA *list, byte type, const byte* id, word16 idSz)
{
    TCA* tca = list;

    while (tca && tca->type != type && type != WOLFSSL_TRUSTED_CA_PRE_AGREED &&
           idSz != tca->idSz && !XMEMCMP(id, tca->id, idSz))
        tca = tca->next;

    return tca;
}
#endif /* NO_WOLFSSL_SERVER */

/** Parses a buffer of TCA extensions. */
static int TLSX_TCA_Parse(WOLFSSL* ssl, const byte* input, word16 length,
                          byte isRequest)
{
#ifndef NO_WOLFSSL_SERVER
    word16 size = 0;
    word16 offset = 0;
#endif

    TLSX *extension = TLSX_Find(ssl->extensions, TLSX_TRUSTED_CA_KEYS);

    if (!extension)
        extension = TLSX_Find(ssl->ctx->extensions, TLSX_TRUSTED_CA_KEYS);

    if (!isRequest) {
        #ifndef NO_WOLFSSL_CLIENT
            if (!extension || !extension->data)
                return TLSX_HandleUnsupportedExtension(ssl);

            if (length > 0)
                return BUFFER_ERROR; /* TCA response MUST be empty. */

            /* Set the flag that we're good for keys */
            TLSX_SetResponse(ssl, TLSX_TRUSTED_CA_KEYS);

            return 0;
        #endif
    }

#ifndef NO_WOLFSSL_SERVER
    if (!extension || !extension->data) {
        /* Skipping, TCA not enabled at server side. */
        return 0;
    }

    if (OPAQUE16_LEN > length)
        return BUFFER_ERROR;

    ato16(input, &size);
    offset += OPAQUE16_LEN;

    /* validating tca list length */
    if (length != OPAQUE16_LEN + size)
        return BUFFER_ERROR;

    for (size = 0; offset < length; offset += size) {
        TCA *tca = NULL;
        byte type;
        const byte* id = NULL;
        word16 idSz = 0;

        if (offset + ENUM_LEN > length)
            return BUFFER_ERROR;

        type = input[offset++];

        switch (type) {
            case WOLFSSL_TRUSTED_CA_PRE_AGREED:
                break;
            #ifndef NO_SHA
            case WOLFSSL_TRUSTED_CA_KEY_SHA1:
            case WOLFSSL_TRUSTED_CA_CERT_SHA1:
                if (offset + WC_SHA_DIGEST_SIZE > length)
                    return BUFFER_ERROR;
                idSz = WC_SHA_DIGEST_SIZE;
                id = input + offset;
                offset += idSz;
                break;
            #endif
            case WOLFSSL_TRUSTED_CA_X509_NAME:
                if (offset + OPAQUE16_LEN > length)
                    return BUFFER_ERROR;
                ato16(input + offset, &idSz);
                offset += OPAQUE16_LEN;
                if ((offset > length) || (idSz > length - offset))
                    return BUFFER_ERROR;
                id = input + offset;
                offset += idSz;
                break;
            default:
                WOLFSSL_ERROR_VERBOSE(TCA_INVALID_ID_TYPE);
                return TCA_INVALID_ID_TYPE;
        }

        /* Find the type/ID in the TCA list. */
        tca = TLSX_TCA_Find((TCA*)extension->data, type, id, idSz);
        if (tca != NULL) {
            /* Found it. Set the response flag and break out of the loop. */
            TLSX_SetResponse(ssl, TLSX_TRUSTED_CA_KEYS);
            break;
        }
    }
#else
    (void)input;
#endif

    return 0;
}

/* Checks to see if the server sent a response for the TCA. */
static int TLSX_TCA_VerifyParse(WOLFSSL* ssl, byte isRequest)
{
    (void)ssl;

    if (!isRequest) {
    #ifndef NO_WOLFSSL_CLIENT
        TLSX* extension = TLSX_Find(ssl->extensions, TLSX_TRUSTED_CA_KEYS);

        if (extension && !extension->resp) {
            SendAlert(ssl, alert_fatal, handshake_failure);
            WOLFSSL_ERROR_VERBOSE(TCA_ABSENT_ERROR);
            return TCA_ABSENT_ERROR;
        }
    #endif /* NO_WOLFSSL_CLIENT */
    }

    return 0;
}

int TLSX_UseTrustedCA(TLSX** extensions, byte type,
                    const byte* id, word16 idSz, void* heap)
{
    TLSX* extension;
    TCA* tca = NULL;

    if (extensions == NULL)
        return BAD_FUNC_ARG;

    if ((tca = TLSX_TCA_New(type, id, idSz, heap)) == NULL)
        return MEMORY_E;

    extension = TLSX_Find(*extensions, TLSX_TRUSTED_CA_KEYS);
    if (!extension) {
        int ret = TLSX_Push(extensions, TLSX_TRUSTED_CA_KEYS, (void*)tca, heap);

        if (ret != 0) {
            TLSX_TCA_Free(tca, heap);
            return ret;
        }
    }
    else {
        /* push new TCA object to extension data. */
        tca->next = (TCA*)extension->data;
        extension->data = (void*)tca;
    }

    return WOLFSSL_SUCCESS;
}

#define TCA_FREE_ALL     TLSX_TCA_FreeAll
#define TCA_GET_SIZE     TLSX_TCA_GetSize
#define TCA_WRITE        TLSX_TCA_Write
#define TCA_PARSE        TLSX_TCA_Parse
#define TCA_VERIFY_PARSE TLSX_TCA_VerifyParse

#else /* HAVE_TRUSTED_CA */

#define TCA_FREE_ALL(list, heap) WC_DO_NOTHING
#define TCA_GET_SIZE(list)     0
#define TCA_WRITE(a, b)        0
#define TCA_PARSE(a, b, c, d)  0
#define TCA_VERIFY_PARSE(a, b) 0

#endif /* HAVE_TRUSTED_CA */

/******************************************************************************/
/* Max Fragment Length Negotiation                                            */
/******************************************************************************/

#ifdef HAVE_MAX_FRAGMENT

static word16 TLSX_MFL_Write(byte* data, byte* output)
{
    output[0] = data[0];

    return ENUM_LEN;
}

static int TLSX_MFL_Parse(WOLFSSL* ssl, const byte* input, word16 length,
                          byte isRequest)
{
    if (length != ENUM_LEN)
        return BUFFER_ERROR;

#ifdef WOLFSSL_OLD_UNSUPPORTED_EXTENSION
    (void) isRequest;
#else
    if (!isRequest)
        if (TLSX_CheckUnsupportedExtension(ssl, TLSX_MAX_FRAGMENT_LENGTH))
            return TLSX_HandleUnsupportedExtension(ssl);
#endif

    switch (*input) {
        case WOLFSSL_MFL_2_8 : ssl->max_fragment =  256; break;
        case WOLFSSL_MFL_2_9 : ssl->max_fragment =  512; break;
        case WOLFSSL_MFL_2_10: ssl->max_fragment = 1024; break;
        case WOLFSSL_MFL_2_11: ssl->max_fragment = 2048; break;
        case WOLFSSL_MFL_2_12: ssl->max_fragment = 4096; break;
        case WOLFSSL_MFL_2_13: ssl->max_fragment = 8192; break;

        default:
            SendAlert(ssl, alert_fatal, illegal_parameter);
            WOLFSSL_ERROR_VERBOSE(UNKNOWN_MAX_FRAG_LEN_E);
            return UNKNOWN_MAX_FRAG_LEN_E;
    }

#ifndef NO_WOLFSSL_SERVER
    if (isRequest) {
        int ret = TLSX_UseMaxFragment(&ssl->extensions, *input, ssl->heap);

        if (ret != WOLFSSL_SUCCESS)
            return ret; /* throw error */

        TLSX_SetResponse(ssl, TLSX_MAX_FRAGMENT_LENGTH);
    }
#endif

    return 0;
}

int TLSX_UseMaxFragment(TLSX** extensions, byte mfl, void* heap)
{
    byte* data = NULL;
    int ret = 0;

    if (extensions == NULL || mfl < WOLFSSL_MFL_MIN || mfl > WOLFSSL_MFL_MAX)
        return BAD_FUNC_ARG;

    data = (byte*)XMALLOC(ENUM_LEN, heap, DYNAMIC_TYPE_TLSX);
    if (data == NULL)
        return MEMORY_E;

    data[0] = mfl;

    ret = TLSX_Push(extensions, TLSX_MAX_FRAGMENT_LENGTH, data, heap);
    if (ret != 0) {
        XFREE(data, heap, DYNAMIC_TYPE_TLSX);
        return ret;
    }

    return WOLFSSL_SUCCESS;
}


#define MFL_FREE_ALL(data, heap) XFREE(data, (heap), DYNAMIC_TYPE_TLSX)
#define MFL_GET_SIZE(data) ENUM_LEN
#define MFL_WRITE          TLSX_MFL_Write
#define MFL_PARSE          TLSX_MFL_Parse

#else

#define MFL_FREE_ALL(a, b) WC_DO_NOTHING
#define MFL_GET_SIZE(a)       0
#define MFL_WRITE(a, b)       0
#define MFL_PARSE(a, b, c, d) 0

#endif /* HAVE_MAX_FRAGMENT */

/******************************************************************************/
/* Truncated HMAC                                                             */
/******************************************************************************/

#ifdef HAVE_TRUNCATED_HMAC

static int TLSX_THM_Parse(WOLFSSL* ssl, const byte* input, word16 length,
                          byte isRequest)
{
    if (length != 0 || input == NULL)
        return BUFFER_ERROR;

    if (!isRequest) {
    #ifndef WOLFSSL_OLD_UNSUPPORTED_EXTENSION
        if (TLSX_CheckUnsupportedExtension(ssl, TLSX_TRUNCATED_HMAC))
            return TLSX_HandleUnsupportedExtension(ssl);
    #endif
    }
    else {
        #ifndef NO_WOLFSSL_SERVER
            int ret = TLSX_UseTruncatedHMAC(&ssl->extensions, ssl->heap);

            if (ret != WOLFSSL_SUCCESS)
                return ret; /* throw error */

            TLSX_SetResponse(ssl, TLSX_TRUNCATED_HMAC);
        #endif
    }

    ssl->truncated_hmac = 1;

    return 0;
}

int TLSX_UseTruncatedHMAC(TLSX** extensions, void* heap)
{
    int ret = 0;

    if (extensions == NULL)
        return BAD_FUNC_ARG;

    ret = TLSX_Push(extensions, TLSX_TRUNCATED_HMAC, NULL, heap);
    if (ret != 0)
        return ret;

    return WOLFSSL_SUCCESS;
}

#define THM_PARSE TLSX_THM_Parse

#else

#define THM_PARSE(a, b, c, d) 0

#endif /* HAVE_TRUNCATED_HMAC */

/******************************************************************************/
/* Certificate Status Request                                                 */
/******************************************************************************/

#ifdef HAVE_CERTIFICATE_STATUS_REQUEST

static void TLSX_CSR_Free(CertificateStatusRequest* csr, void* heap)
{
    switch (csr->status_type) {
        case WOLFSSL_CSR_OCSP:
            FreeOcspRequest(&csr->request.ocsp);
        break;
    }

#ifdef WOLFSSL_TLS13
    if (csr->response.buffer != NULL) {
        XFREE(csr->response.buffer, csr->ssl->heap,
                DYNAMIC_TYPE_TMP_BUFFER);
    }
#endif
    XFREE(csr, heap, DYNAMIC_TYPE_TLSX);
    (void)heap;
}

static word16 TLSX_CSR_GetSize(CertificateStatusRequest* csr, byte isRequest)
{
    word16 size = 0;

    /* shut up compiler warnings */
    (void) csr; (void) isRequest;

#ifndef NO_WOLFSSL_CLIENT
    if (isRequest) {
        switch (csr->status_type) {
            case WOLFSSL_CSR_OCSP:
                size += ENUM_LEN + 2 * OPAQUE16_LEN;

                if (csr->request.ocsp.nonceSz)
                    size += OCSP_NONCE_EXT_SZ;
            break;
        }
    }
#endif
#if defined(WOLFSSL_TLS13) && !defined(NO_WOLFSSL_SERVER)
    if (!isRequest && csr->ssl->options.tls1_3)
        return OPAQUE8_LEN + OPAQUE24_LEN + csr->response.length;
#endif

    return size;
}

static word16 TLSX_CSR_Write(CertificateStatusRequest* csr, byte* output,
                                                                 byte isRequest)
{
    /* shut up compiler warnings */
    (void) csr; (void) output; (void) isRequest;

#ifndef NO_WOLFSSL_CLIENT
    if (isRequest) {
        word16 offset = 0;
        word16 length = 0;

        /* type */
        output[offset++] = csr->status_type;

        switch (csr->status_type) {
            case WOLFSSL_CSR_OCSP:
                /* responder id list */
                c16toa(0, output + offset);
                offset += OPAQUE16_LEN;

                /* request extensions */
                if (csr->request.ocsp.nonceSz)
                    length = (word16)EncodeOcspRequestExtensions(
                                                 &csr->request.ocsp,
                                                 output + offset + OPAQUE16_LEN,
                                                 OCSP_NONCE_EXT_SZ);

                c16toa(length, output + offset);
                offset += OPAQUE16_LEN + length;

            break;
        }

        return offset;
    }
#endif
#if defined(WOLFSSL_TLS13) && !defined(NO_WOLFSSL_SERVER)
    if (!isRequest && csr->ssl->options.tls1_3) {
        word16 offset = 0;
        output[offset++] = csr->status_type;
        c32to24(csr->response.length, output + offset);
        offset += OPAQUE24_LEN;
        XMEMCPY(output + offset, csr->response.buffer, csr->response.length);
        offset += csr->response.length;
        return offset;
    }
#endif

    return 0;
}

static int TLSX_CSR_Parse(WOLFSSL* ssl, const byte* input, word16 length,
                          byte isRequest)
{
    int ret;
#if !defined(NO_WOLFSSL_SERVER)
    byte status_type;
    word16 size = 0;
#if defined(WOLFSSL_TLS13)
    DecodedCert* cert;
#endif
#endif

#if !defined(NO_WOLFSSL_CLIENT) || !defined(NO_WOLFSSL_SERVER) \
    && defined(WOLFSSL_TLS13)
    OcspRequest* request;
    TLSX* extension;
    CertificateStatusRequest* csr;
#endif

#if !defined(NO_WOLFSSL_CLIENT) && defined(WOLFSSL_TLS13) \
 || !defined(NO_WOLFSSL_SERVER)
    word32 offset = 0;
#endif

#if !defined(NO_WOLFSSL_CLIENT) && defined(WOLFSSL_TLS13)
    word32 resp_length = 0;
#endif

    /* shut up compiler warnings */
    (void) ssl; (void) input;

    if (!isRequest) {
#ifndef NO_WOLFSSL_CLIENT
        extension = TLSX_Find(ssl->extensions, TLSX_STATUS_REQUEST);
        csr = extension ? (CertificateStatusRequest*)extension->data : NULL;

        if (!csr) {
            /* look at context level */
            extension = TLSX_Find(ssl->ctx->extensions, TLSX_STATUS_REQUEST);
            csr = extension ? (CertificateStatusRequest*)extension->data : NULL;

            if (!csr) /* unexpected extension */
                return TLSX_HandleUnsupportedExtension(ssl);

            /* enable extension at ssl level */
            ret = TLSX_UseCertificateStatusRequest(&ssl->extensions,
                                     csr->status_type, csr->options, ssl,
                                     ssl->heap, ssl->devId);
            if (ret != WOLFSSL_SUCCESS)
                return ret == 0 ? -1 : ret;

            switch (csr->status_type) {
                case WOLFSSL_CSR_OCSP:
                    /* propagate nonce */
                    if (csr->request.ocsp.nonceSz) {
                        request =
                            (OcspRequest*)TLSX_CSR_GetRequest(ssl->extensions);

                        if (request) {
                            XMEMCPY(request->nonce, csr->request.ocsp.nonce,
                                                    csr->request.ocsp.nonceSz);
                            request->nonceSz = csr->request.ocsp.nonceSz;
                        }
                    }
                break;
            }
        }

        ssl->status_request = 1;

    #ifdef WOLFSSL_TLS13
        if (ssl->options.tls1_3) {
            /* Get the new extension potentially created above. */
            extension = TLSX_Find(ssl->extensions, TLSX_STATUS_REQUEST);
            csr = extension ? (CertificateStatusRequest*)extension->data : NULL;
            if (csr == NULL)
                return MEMORY_ERROR;

            ret = 0;
            if (OPAQUE8_LEN + OPAQUE24_LEN > length)
                ret = BUFFER_ERROR;
            if (ret == 0 && input[offset++] != WOLFSSL_CSR_OCSP) {
                ret = BAD_CERTIFICATE_STATUS_ERROR;
                WOLFSSL_ERROR_VERBOSE(ret);
            }
            if (ret == 0) {
                c24to32(input + offset, &resp_length);
                offset += OPAQUE24_LEN;
                if (offset + resp_length != length)
                    ret = BUFFER_ERROR;
            }
            if (ret == 0) {
                csr->response.buffer = (byte*)XMALLOC(resp_length, ssl->heap,
                        DYNAMIC_TYPE_TMP_BUFFER);
                if (csr->response.buffer == NULL)
                    ret = MEMORY_ERROR;
            }
            if (ret == 0) {
                XMEMCPY(csr->response.buffer, input + offset, resp_length);
                csr->response.length = resp_length;
            }

            return ret;
        }
        else
    #endif
        {
            /* extension_data MUST be empty. */
            return length ? BUFFER_ERROR : 0;
        }
#endif
    }
    else {
#ifndef NO_WOLFSSL_SERVER
        if (length == 0)
            return 0;

        status_type = input[offset++];

        switch (status_type) {
            case WOLFSSL_CSR_OCSP: {

                /* skip responder_id_list */
                if ((int)(length - offset) < OPAQUE16_LEN)
                    return BUFFER_ERROR;

                ato16(input + offset, &size);
                offset += OPAQUE16_LEN + size;

                /* skip request_extensions */
                if ((int)(length - offset) < OPAQUE16_LEN)
                    return BUFFER_ERROR;

                ato16(input + offset, &size);
                offset += OPAQUE16_LEN + size;

                if (offset > length)
                    return BUFFER_ERROR;

                /* is able to send OCSP response? */
                if (SSL_CM(ssl) == NULL || !SSL_CM(ssl)->ocspStaplingEnabled)
                    return 0;
            }
            break;

            /* unknown status type */
            default:
                return 0;
        }

        /* if using status_request and already sending it, skip this one */
        #ifdef HAVE_CERTIFICATE_STATUS_REQUEST_V2
        if (ssl->status_request_v2)
            return 0;
        #endif

        /* accept the first good status_type and return */
        ret = TLSX_UseCertificateStatusRequest(&ssl->extensions, status_type,
                                                 0, ssl, ssl->heap, ssl->devId);
        if (ret != WOLFSSL_SUCCESS)
            return ret == 0 ? -1 : ret; /* throw error */

    #if defined(WOLFSSL_TLS13)
        if (ssl->options.tls1_3) {
            if (ssl->buffers.certificate == NULL) {
                WOLFSSL_MSG("Certificate buffer not set!");
                return BUFFER_ERROR;
            }
            cert = (DecodedCert*)XMALLOC(sizeof(DecodedCert), ssl->heap,
                                         DYNAMIC_TYPE_DCERT);
            if (cert == NULL) {
                return MEMORY_E;
            }
            InitDecodedCert(cert, ssl->buffers.certificate->buffer,
                            ssl->buffers.certificate->length, ssl->heap);
            ret = ParseCert(cert, CERT_TYPE, 1, SSL_CM(ssl));
            if (ret != 0 ) {
                FreeDecodedCert(cert);
                XFREE(cert, ssl->heap, DYNAMIC_TYPE_DCERT);
                return ret;
            }
            ret = TLSX_CSR_InitRequest(ssl->extensions, cert, ssl->heap);
            if (ret != 0 ) {
                FreeDecodedCert(cert);
                XFREE(cert, ssl->heap, DYNAMIC_TYPE_DCERT);
                return ret;
            }
            FreeDecodedCert(cert);
            XFREE(cert, ssl->heap, DYNAMIC_TYPE_DCERT);

            extension = TLSX_Find(ssl->extensions, TLSX_STATUS_REQUEST);
            csr = extension ?
                (CertificateStatusRequest*)extension->data : NULL;
            if (csr == NULL)
                return MEMORY_ERROR;

            request = &csr->request.ocsp;
            ret = CreateOcspResponse(ssl, &request, &csr->response);
            if (ret != 0)
                return ret;
            if (csr->response.buffer)
                TLSX_SetResponse(ssl, TLSX_STATUS_REQUEST);
        }
        else
    #endif
            TLSX_SetResponse(ssl, TLSX_STATUS_REQUEST);
        ssl->status_request = status_type;
#endif
    }

    return 0;
}

int TLSX_CSR_InitRequest(TLSX* extensions, DecodedCert* cert, void* heap)
{
    TLSX* extension = TLSX_Find(extensions, TLSX_STATUS_REQUEST);
    CertificateStatusRequest* csr = extension ?
        (CertificateStatusRequest*)extension->data : NULL;
    int ret = 0;

    if (csr) {
        switch (csr->status_type) {
            case WOLFSSL_CSR_OCSP: {
                byte nonce[MAX_OCSP_NONCE_SZ];
                int  nonceSz = csr->request.ocsp.nonceSz;

                /* preserve nonce */
                XMEMCPY(nonce, csr->request.ocsp.nonce, nonceSz);

                if ((ret = InitOcspRequest(&csr->request.ocsp, cert, 0, heap))
                                                                           != 0)
                    return ret;

                /* restore nonce */
                XMEMCPY(csr->request.ocsp.nonce, nonce, nonceSz);
                csr->request.ocsp.nonceSz = nonceSz;
            }
            break;
        }
    }

    return ret;
}

void* TLSX_CSR_GetRequest(TLSX* extensions)
{
    TLSX* extension = TLSX_Find(extensions, TLSX_STATUS_REQUEST);
    CertificateStatusRequest* csr = extension ?
                              (CertificateStatusRequest*)extension->data : NULL;

    if (csr) {
        switch (csr->status_type) {
            case WOLFSSL_CSR_OCSP:
                return &csr->request.ocsp;
        }
    }

    return NULL;
}

int TLSX_CSR_ForceRequest(WOLFSSL* ssl)
{
    TLSX* extension = TLSX_Find(ssl->extensions, TLSX_STATUS_REQUEST);
    CertificateStatusRequest* csr = extension ?
                              (CertificateStatusRequest*)extension->data : NULL;

    if (csr) {
        switch (csr->status_type) {
            case WOLFSSL_CSR_OCSP:
                if (SSL_CM(ssl)->ocspEnabled) {
                    csr->request.ocsp.ssl = ssl;
                    return CheckOcspRequest(SSL_CM(ssl)->ocsp,
                                              &csr->request.ocsp, NULL, NULL);
                }
                else {
                    WOLFSSL_ERROR_VERBOSE(OCSP_LOOKUP_FAIL);
                    return OCSP_LOOKUP_FAIL;
                }
        }
    }

    return 0;
}

int TLSX_UseCertificateStatusRequest(TLSX** extensions, byte status_type,
                                         byte options, WOLFSSL* ssl, void* heap,
                                                                      int devId)
{
    CertificateStatusRequest* csr = NULL;
    int ret = 0;

    if (!extensions || status_type != WOLFSSL_CSR_OCSP)
        return BAD_FUNC_ARG;

    csr = (CertificateStatusRequest*)
             XMALLOC(sizeof(CertificateStatusRequest), heap, DYNAMIC_TYPE_TLSX);
    if (!csr)
        return MEMORY_E;

    ForceZero(csr, sizeof(CertificateStatusRequest));

    csr->status_type = status_type;
    csr->options     = options;
    csr->ssl         = ssl;

    switch (csr->status_type) {
        case WOLFSSL_CSR_OCSP:
            if (options & WOLFSSL_CSR_OCSP_USE_NONCE) {
                WC_RNG rng;

            #ifndef HAVE_FIPS
                ret = wc_InitRng_ex(&rng, heap, devId);
            #else
                ret = wc_InitRng(&rng);
                (void)devId;
            #endif
                if (ret == 0) {
                    if (wc_RNG_GenerateBlock(&rng, csr->request.ocsp.nonce,
                                                        MAX_OCSP_NONCE_SZ) == 0)
                        csr->request.ocsp.nonceSz = MAX_OCSP_NONCE_SZ;

                    wc_FreeRng(&rng);
                }
            }
        break;
    }

    if ((ret = TLSX_Push(extensions, TLSX_STATUS_REQUEST, csr, heap)) != 0) {
        XFREE(csr, heap, DYNAMIC_TYPE_TLSX);
        return ret;
    }

    return WOLFSSL_SUCCESS;
}

#define CSR_FREE_ALL TLSX_CSR_Free
#define CSR_GET_SIZE TLSX_CSR_GetSize
#define CSR_WRITE    TLSX_CSR_Write
#define CSR_PARSE    TLSX_CSR_Parse

#else

#define CSR_FREE_ALL(data, heap) WC_DO_NOTHING
#define CSR_GET_SIZE(a, b)    0
#define CSR_WRITE(a, b, c)    0
#define CSR_PARSE(a, b, c, d) 0

#endif /* HAVE_CERTIFICATE_STATUS_REQUEST */

/******************************************************************************/
/* Certificate Status Request v2                                              */
/******************************************************************************/

#ifdef HAVE_CERTIFICATE_STATUS_REQUEST_V2

static void TLSX_CSR2_FreeAll(CertificateStatusRequestItemV2* csr2, void* heap)
{
    CertificateStatusRequestItemV2* next;

    for (; csr2; csr2 = next) {
        next = csr2->next;

        switch (csr2->status_type) {
            case WOLFSSL_CSR2_OCSP:
            case WOLFSSL_CSR2_OCSP_MULTI:
                while(csr2->requests--)
                    FreeOcspRequest(&csr2->request.ocsp[csr2->requests]);
            break;
        }

        XFREE(csr2, heap, DYNAMIC_TYPE_TLSX);
    }
    (void)heap;
}

static word16 TLSX_CSR2_GetSize(CertificateStatusRequestItemV2* csr2,
                                                                 byte isRequest)
{
    word16 size = 0;

    /* shut up compiler warnings */
    (void) csr2; (void) isRequest;

#ifndef NO_WOLFSSL_CLIENT
    if (isRequest) {
        CertificateStatusRequestItemV2* next;

        for (size = OPAQUE16_LEN; csr2; csr2 = next) {
            next = csr2->next;

            switch (csr2->status_type) {
                case WOLFSSL_CSR2_OCSP:
                case WOLFSSL_CSR2_OCSP_MULTI:
                    size += ENUM_LEN + 3 * OPAQUE16_LEN;

                    if (csr2->request.ocsp[0].nonceSz)
                        size += OCSP_NONCE_EXT_SZ;
                break;
            }
        }
    }
#endif

    return size;
}

static word16 TLSX_CSR2_Write(CertificateStatusRequestItemV2* csr2,
                                                   byte* output, byte isRequest)
{
    /* shut up compiler warnings */
    (void) csr2; (void) output; (void) isRequest;

#ifndef NO_WOLFSSL_CLIENT
    if (isRequest) {
        word16 offset;
        word16 length;

        for (offset = OPAQUE16_LEN; csr2 != NULL; csr2 = csr2->next) {
            /* status_type */
            output[offset++] = csr2->status_type;

            /* request */
            switch (csr2->status_type) {
                case WOLFSSL_CSR2_OCSP:
                case WOLFSSL_CSR2_OCSP_MULTI:
                    /* request_length */
                    length = 2 * OPAQUE16_LEN;

                    if (csr2->request.ocsp[0].nonceSz)
                        length += OCSP_NONCE_EXT_SZ;

                    c16toa(length, output + offset);
                    offset += OPAQUE16_LEN;

                    /* responder id list */
                    c16toa(0, output + offset);
                    offset += OPAQUE16_LEN;

                    /* request extensions */
                    length = 0;

                    if (csr2->request.ocsp[0].nonceSz)
                        length = (word16)EncodeOcspRequestExtensions(
                                                 &csr2->request.ocsp[0],
                                                 output + offset + OPAQUE16_LEN,
                                                 OCSP_NONCE_EXT_SZ);

                    c16toa(length, output + offset);
                    offset += OPAQUE16_LEN + length;
                break;
            }
        }

        /* list size */
        c16toa(offset - OPAQUE16_LEN, output);

        return offset;
    }
#endif

    return 0;
}

static int TLSX_CSR2_Parse(WOLFSSL* ssl, const byte* input, word16 length,
                           byte isRequest)
{
    int ret;

    /* shut up compiler warnings */
    (void) ssl; (void) input;

    if (!isRequest) {
#ifndef NO_WOLFSSL_CLIENT
        TLSX* extension = TLSX_Find(ssl->extensions, TLSX_STATUS_REQUEST_V2);
        CertificateStatusRequestItemV2* csr2 = extension ?
                        (CertificateStatusRequestItemV2*)extension->data : NULL;

        if (!csr2) {
            /* look at context level */
            extension = TLSX_Find(ssl->ctx->extensions, TLSX_STATUS_REQUEST_V2);
            csr2 = extension ?
                        (CertificateStatusRequestItemV2*)extension->data : NULL;

            if (!csr2) /* unexpected extension */
                return TLSX_HandleUnsupportedExtension(ssl);

            /* enable extension at ssl level */
            for (; csr2; csr2 = csr2->next) {
                ret = TLSX_UseCertificateStatusRequestV2(&ssl->extensions,
                                    csr2->status_type, csr2->options, ssl->heap,
                                                                    ssl->devId);
                if (ret != WOLFSSL_SUCCESS)
                    return ret;

                switch (csr2->status_type) {
                    case WOLFSSL_CSR2_OCSP:
                        /* followed by */
                    case WOLFSSL_CSR2_OCSP_MULTI:
                        /* propagate nonce */
                        if (csr2->request.ocsp[0].nonceSz) {
                            OcspRequest* request =
                             (OcspRequest*)TLSX_CSR2_GetRequest(ssl->extensions,
                                                          csr2->status_type, 0);

                            if (request) {
                                XMEMCPY(request->nonce,
                                        csr2->request.ocsp[0].nonce,
                                        csr2->request.ocsp[0].nonceSz);

                                request->nonceSz =
                                                  csr2->request.ocsp[0].nonceSz;
                            }
                        }
                    break;
                }
            }
        }

        ssl->status_request_v2 = 1;

        return length ? BUFFER_ERROR : 0; /* extension_data MUST be empty. */
#endif
    }
    else {
#ifndef NO_WOLFSSL_SERVER
        byte   status_type;
        word16 request_length;
        word16 offset = 0;
        word16 size = 0;

        /* list size */
        if (offset + OPAQUE16_LEN >= length) {
            return BUFFER_E;
        }

        ato16(input + offset, &request_length);
        offset += OPAQUE16_LEN;

        if (length - OPAQUE16_LEN != request_length)
            return BUFFER_ERROR;

        while (length > offset) {
            if ((int)(length - offset) < ENUM_LEN + OPAQUE16_LEN)
                return BUFFER_ERROR;

            status_type = input[offset++];

            ato16(input + offset, &request_length);
            offset += OPAQUE16_LEN;

            if (length - offset < request_length)
                return BUFFER_ERROR;

            switch (status_type) {
                case WOLFSSL_CSR2_OCSP:
                case WOLFSSL_CSR2_OCSP_MULTI:
                    /* skip responder_id_list */
                    if ((int)(length - offset) < OPAQUE16_LEN)
                        return BUFFER_ERROR;

                    ato16(input + offset, &size);
                    if (length - offset < size)
                        return BUFFER_ERROR;

                    offset += OPAQUE16_LEN + size;
                    /* skip request_extensions */
                    if ((int)(length - offset) < OPAQUE16_LEN)
                        return BUFFER_ERROR;

                    ato16(input + offset, &size);
                    if (length - offset < size)
                        return BUFFER_ERROR;

                    offset += OPAQUE16_LEN + size;
                    if (offset > length)
                        return BUFFER_ERROR;

                    /* is able to send OCSP response? */
                    if (SSL_CM(ssl) == NULL
                    || !SSL_CM(ssl)->ocspStaplingEnabled)
                        continue;
                break;

                default:
                    /* unknown status type, skipping! */
                    offset += request_length;
                    continue;
            }

            /* if using status_request and already sending it, remove it
             * and prefer to use the v2 version */
            #ifdef HAVE_CERTIFICATE_STATUS_REQUEST
            if (ssl->status_request) {
                ssl->status_request = 0;
                TLSX_Remove(&ssl->extensions, TLSX_STATUS_REQUEST, ssl->heap);
            }
            #endif

            /* TLS 1.3 servers MUST NOT act upon presence or information in
             * this extension (RFC 8448 Section 4.4.2.1).
             */
            if (!IsAtLeastTLSv1_3(ssl->version)) {
                /* accept the first good status_type and return */
                ret = TLSX_UseCertificateStatusRequestV2(&ssl->extensions,
                                         status_type, 0, ssl->heap, ssl->devId);
                if (ret != WOLFSSL_SUCCESS)
                    return ret; /* throw error */

                TLSX_SetResponse(ssl, TLSX_STATUS_REQUEST_V2);
                ssl->status_request_v2 = status_type;
            }

            return 0;
        }
#endif
    }

    return 0;
}

int TLSX_CSR2_InitRequests(TLSX* extensions, DecodedCert* cert, byte isPeer,
                                                                     void* heap)
{
    TLSX* extension = TLSX_Find(extensions, TLSX_STATUS_REQUEST_V2);
    CertificateStatusRequestItemV2* csr2 = extension ?
        (CertificateStatusRequestItemV2*)extension->data : NULL;
    int ret = 0;

    for (; csr2; csr2 = csr2->next) {
        switch (csr2->status_type) {
            case WOLFSSL_CSR2_OCSP:
                if (!isPeer || csr2->requests != 0)
                    break;

                FALL_THROUGH; /* followed by */

            case WOLFSSL_CSR2_OCSP_MULTI: {
                if (csr2->requests < 1 + MAX_CHAIN_DEPTH) {
                    byte nonce[MAX_OCSP_NONCE_SZ];
                    int  nonceSz = csr2->request.ocsp[0].nonceSz;

                    /* preserve nonce, replicating nonce of ocsp[0] */
                    XMEMCPY(nonce, csr2->request.ocsp[0].nonce, nonceSz);

                    if ((ret = InitOcspRequest(
                                      &csr2->request.ocsp[csr2->requests], cert,
                                                                 0, heap)) != 0)
                        return ret;

                    /* restore nonce */
                    XMEMCPY(csr2->request.ocsp[csr2->requests].nonce,
                                                                nonce, nonceSz);
                    csr2->request.ocsp[csr2->requests].nonceSz = nonceSz;
                    csr2->requests++;
                }
            }
            break;
        }
    }

    (void)cert;
    return ret;
}

void* TLSX_CSR2_GetRequest(TLSX* extensions, byte status_type, byte idx)
{
    TLSX* extension = TLSX_Find(extensions, TLSX_STATUS_REQUEST_V2);
    CertificateStatusRequestItemV2* csr2 = extension ?
                        (CertificateStatusRequestItemV2*)extension->data : NULL;

    for (; csr2; csr2 = csr2->next) {
        if (csr2->status_type == status_type) {
            switch (csr2->status_type) {
                case WOLFSSL_CSR2_OCSP:
                    /* followed by */

                case WOLFSSL_CSR2_OCSP_MULTI:
                    /* requests are initialized in the reverse order */
                    return idx < csr2->requests
                         ? &csr2->request.ocsp[csr2->requests - idx - 1]
                         : NULL;
            }
        }
    }

    return NULL;
}

int TLSX_CSR2_ForceRequest(WOLFSSL* ssl)
{
    TLSX* extension = TLSX_Find(ssl->extensions, TLSX_STATUS_REQUEST_V2);
    CertificateStatusRequestItemV2* csr2 = extension ?
                        (CertificateStatusRequestItemV2*)extension->data : NULL;

    /* forces only the first one */
    if (csr2) {
        switch (csr2->status_type) {
            case WOLFSSL_CSR2_OCSP:
                /* followed by */

            case WOLFSSL_CSR2_OCSP_MULTI:
                if (SSL_CM(ssl)->ocspEnabled) {
                    csr2->request.ocsp[0].ssl = ssl;
                    return CheckOcspRequest(SSL_CM(ssl)->ocsp,
                                          &csr2->request.ocsp[0], NULL, NULL);
                }
                else {
                    WOLFSSL_ERROR_VERBOSE(OCSP_LOOKUP_FAIL);
                    return OCSP_LOOKUP_FAIL;
                }
        }
    }

    return 0;
}

int TLSX_UseCertificateStatusRequestV2(TLSX** extensions, byte status_type,
                                           byte options, void* heap, int devId)
{
    TLSX* extension = NULL;
    CertificateStatusRequestItemV2* csr2 = NULL;
    int ret = 0;

    if (!extensions)
        return BAD_FUNC_ARG;

    if (status_type != WOLFSSL_CSR2_OCSP
    &&  status_type != WOLFSSL_CSR2_OCSP_MULTI)
        return BAD_FUNC_ARG;

    csr2 = (CertificateStatusRequestItemV2*)
       XMALLOC(sizeof(CertificateStatusRequestItemV2), heap, DYNAMIC_TYPE_TLSX);
    if (!csr2)
        return MEMORY_E;

    ForceZero(csr2, sizeof(CertificateStatusRequestItemV2));

    csr2->status_type = status_type;
    csr2->options     = options;
    csr2->next        = NULL;

    switch (csr2->status_type) {
        case WOLFSSL_CSR2_OCSP:
        case WOLFSSL_CSR2_OCSP_MULTI:
            if (options & WOLFSSL_CSR2_OCSP_USE_NONCE) {
                WC_RNG rng;

            #ifndef HAVE_FIPS
                ret = wc_InitRng_ex(&rng, heap, devId);
            #else
                ret = wc_InitRng(&rng);
                (void)devId;
            #endif
                if (ret == 0) {
                    if (wc_RNG_GenerateBlock(&rng, csr2->request.ocsp[0].nonce,
                                                        MAX_OCSP_NONCE_SZ) == 0)
                        csr2->request.ocsp[0].nonceSz = MAX_OCSP_NONCE_SZ;

                    wc_FreeRng(&rng);
                }
            }
        break;
    }

    /* append new item */
    if ((extension = TLSX_Find(*extensions, TLSX_STATUS_REQUEST_V2))) {
        CertificateStatusRequestItemV2* last =
                               (CertificateStatusRequestItemV2*)extension->data;

        for (; last->next; last = last->next);

        last->next = csr2;
    }
    else if ((ret = TLSX_Push(extensions, TLSX_STATUS_REQUEST_V2, csr2,heap))) {
        XFREE(csr2, heap, DYNAMIC_TYPE_TLSX);
        return ret;
    }

    return WOLFSSL_SUCCESS;
}

#define CSR2_FREE_ALL TLSX_CSR2_FreeAll
#define CSR2_GET_SIZE TLSX_CSR2_GetSize
#define CSR2_WRITE    TLSX_CSR2_Write
#define CSR2_PARSE    TLSX_CSR2_Parse

#else

#define CSR2_FREE_ALL(data, heap) WC_DO_NOTHING
#define CSR2_GET_SIZE(a, b)    0
#define CSR2_WRITE(a, b, c)    0
#define CSR2_PARSE(a, b, c, d) 0

#endif /* HAVE_CERTIFICATE_STATUS_REQUEST_V2 */

/******************************************************************************/
/* Supported Elliptic Curves                                                  */
/******************************************************************************/

#ifdef HAVE_SUPPORTED_CURVES

#if !defined(HAVE_ECC) && !defined(HAVE_CURVE25519) && !defined(HAVE_CURVE448) \
                       && !defined(HAVE_FFDHE) && !defined(HAVE_PQC)
#error Elliptic Curves Extension requires Elliptic Curve Cryptography or liboqs groups. \
       Use --enable-ecc and/or --enable-liboqs in the configure script or \
       define HAVE_ECC. Alternatively use FFDHE for DH cipher suites.
#endif

static int TLSX_SupportedCurve_New(SupportedCurve** curve, word16 name,
                                                                     void* heap)
{
    if (curve == NULL)
        return BAD_FUNC_ARG;

    (void)heap;

    *curve = (SupportedCurve*)XMALLOC(sizeof(SupportedCurve), heap,
                                                             DYNAMIC_TYPE_TLSX);
    if (*curve == NULL)
        return MEMORY_E;

    (*curve)->name = name;
    (*curve)->next = NULL;

    return 0;
}

static int TLSX_PointFormat_New(PointFormat** point, byte format, void* heap)
{
    if (point == NULL)
        return BAD_FUNC_ARG;

    (void)heap;

    *point = (PointFormat*)XMALLOC(sizeof(PointFormat), heap,
                                                             DYNAMIC_TYPE_TLSX);
    if (*point == NULL)
        return MEMORY_E;

    (*point)->format = format;
    (*point)->next = NULL;

    return 0;
}

static void TLSX_SupportedCurve_FreeAll(SupportedCurve* list, void* heap)
{
    SupportedCurve* curve;

    while ((curve = list)) {
        list = curve->next;
        XFREE(curve, heap, DYNAMIC_TYPE_TLSX);
    }
    (void)heap;
}

static void TLSX_PointFormat_FreeAll(PointFormat* list, void* heap)
{
    PointFormat* point;

    while ((point = list)) {
        list = point->next;
        XFREE(point, heap, DYNAMIC_TYPE_TLSX);
    }
    (void)heap;
}

static int TLSX_SupportedCurve_Append(SupportedCurve* list, word16 name,
                                                                     void* heap)
{
    int ret = BAD_FUNC_ARG;

    while (list) {
        if (list->name == name) {
            ret = 0; /* curve already in use */
            break;
        }

        if (list->next == NULL) {
            ret = TLSX_SupportedCurve_New(&list->next, name, heap);
            break;
        }

        list = list->next;
    }

    return ret;
}

static int TLSX_PointFormat_Append(PointFormat* list, byte format, void* heap)
{
    int ret = BAD_FUNC_ARG;

    while (list) {
        if (list->format == format) {
            ret = 0; /* format already in use */
            break;
        }

        if (list->next == NULL) {
            ret = TLSX_PointFormat_New(&list->next, format, heap);
            break;
        }

        list = list->next;
    }

    return ret;
}

#if defined(WOLFSSL_TLS13) || !defined(NO_WOLFSSL_CLIENT)

#if defined(HAVE_FFDHE) && (defined(HAVE_ECC) || defined(HAVE_CURVE25519) || \
                                                         defined(HAVE_CURVE448))
static void TLSX_SupportedCurve_ValidateRequest(const WOLFSSL* ssl,
                                                const byte* semaphore)
{
    /* If all pre-defined parameter types for key exchange are supported then
     * always send SupportedGroups extension.
     */
    (void)ssl;
    (void)semaphore;
}
#else
static void TLSX_SupportedCurve_ValidateRequest(WOLFSSL* ssl, byte* semaphore)
{
    word16 i;
    const Suites* suites = WOLFSSL_SUITES(ssl);

    for (i = 0; i < suites->suiteSz; i += 2) {
        if (suites->suites[i] == TLS13_BYTE)
            return;
    #ifdef BUILD_TLS_SM4_GCM_SM3
        if ((suites->suites[i] == CIPHER_BYTE) &&
            (suites->suites[i+1] == TLS_SM4_GCM_SM3))
            return;
    #endif
    #ifdef BUILD_TLS_SM4_CCM_SM3
        if ((suites->suites[i] == CIPHER_BYTE) &&
            (suites->suites[i+1] == TLS_SM4_CCM_SM3))
            return;
    #endif
    #ifdef BUILD_TLS_ECDHE_ECDSA_WITH_SM4_CBC_SM3
        if ((suites->suites[i] == SM_BYTE) &&
            (suites->suites[i+1] == TLS_ECDHE_ECDSA_WITH_SM4_CBC_SM3))
            return;
    #endif
        if ((suites->suites[i] == ECC_BYTE) ||
            (suites->suites[i] == ECDHE_PSK_BYTE) ||
            (suites->suites[i] == CHACHA_BYTE)) {
        #if defined(HAVE_ECC) || defined(HAVE_CURVE25519) || \
                                                          defined(HAVE_CURVE448)
            return;
        #endif
        }
        #ifdef HAVE_FFDHE
        else {
            return;
        }
        #endif
    }

    /* turns semaphore on to avoid sending this extension. */
    TURN_ON(semaphore, TLSX_ToSemaphore(TLSX_SUPPORTED_GROUPS));
}
#endif

/* Only send PointFormats if TLSv13, ECC or CHACHA cipher suite present.
 */
static void TLSX_PointFormat_ValidateRequest(WOLFSSL* ssl, byte* semaphore)
{
#ifdef HAVE_FFDHE
    (void)ssl;
    (void)semaphore;
#else
    word16 i;
    const Suites* suites = WOLFSSL_SUITES(ssl);

    if (suites == NULL)
        return;

    for (i = 0; i < suites->suiteSz; i += 2) {
        if (suites->suites[i] == TLS13_BYTE)
            return;
    #ifdef BUILD_TLS_SM4_GCM_SM3
        if ((suites->suites[i] == CIPHER_BYTE) &&
            (suites->suites[i+1] == TLS_SM4_GCM_SM3))
            return;
    #endif
    #ifdef BUILD_TLS_SM4_CCM_SM3
        if ((suites->suites[i] == CIPHER_BYTE) &&
            (suites->suites[i+1] == TLS_SM4_CCM_SM3))
            return;
    #endif
    #ifdef BUILD_TLS_ECDHE_ECDSA_WITH_SM4_CBC_SM3
        if ((suites->suites[i] == SM_BYTE) &&
            (suites->suites[i+1] == TLS_ECDHE_ECDSA_WITH_SM4_CBC_SM3))
            return;
    #endif
        if ((suites->suites[i] == ECC_BYTE) ||
            (suites->suites[i] == ECDHE_PSK_BYTE) ||
            (suites->suites[i] == CHACHA_BYTE)) {
        #if defined(HAVE_ECC) || defined(HAVE_CURVE25519) || \
                                                          defined(HAVE_CURVE448)
            return;
        #endif
        }
    }
   /* turns semaphore on to avoid sending this extension. */
   TURN_ON(semaphore, TLSX_ToSemaphore(TLSX_EC_POINT_FORMATS));
#endif
}

#endif /* WOLFSSL_TLS13 || !NO_WOLFSSL_CLIENT */

#ifndef NO_WOLFSSL_SERVER

static void TLSX_PointFormat_ValidateResponse(WOLFSSL* ssl, byte* semaphore)
{
#if defined(HAVE_FFDHE) || defined(HAVE_ECC) || defined(HAVE_CURVE25519) || \
                                                          defined(HAVE_CURVE448)
    (void)semaphore;
#endif

    if (ssl->options.cipherSuite0 == TLS13_BYTE)
        return;
#ifdef BUILD_TLS_SM4_GCM_SM3
    if ((ssl->options.cipherSuite0 == CIPHER_BYTE) &&
        (ssl->options.cipherSuite == TLS_SM4_GCM_SM3))
        return;
#endif
#ifdef BUILD_TLS_SM4_CCM_SM3
    if ((ssl->options.cipherSuite0 == CIPHER_BYTE) &&
        (ssl->options.cipherSuite == TLS_SM4_CCM_SM3))
        return;
#endif
#ifdef BUILD_TLS_ECDHE_ECDSA_WITH_SM4_CBC_SM3
    if ((ssl->options.cipherSuite0 == SM_BYTE) &&
        (ssl->options.cipherSuite == TLS_ECDHE_ECDSA_WITH_SM4_CBC_SM3))
        return;
#endif
#if defined(HAVE_ECC) || defined(HAVE_CURVE25519) || defined(HAVE_CURVE448)
    if (ssl->options.cipherSuite0 == ECC_BYTE ||
        ssl->options.cipherSuite0 == ECDHE_PSK_BYTE ||
        ssl->options.cipherSuite0 == CHACHA_BYTE) {
        return;
    }
#endif

    /* turns semaphore on to avoid sending this extension. */
    TURN_ON(semaphore, TLSX_ToSemaphore(TLSX_EC_POINT_FORMATS));
}

#endif /* !NO_WOLFSSL_SERVER */

#ifndef NO_WOLFSSL_CLIENT

static word16 TLSX_SupportedCurve_GetSize(SupportedCurve* list)
{
    SupportedCurve* curve;
    word16 length = OPAQUE16_LEN; /* list length */

    while ((curve = list)) {
        list = curve->next;
        length += OPAQUE16_LEN; /* curve length */
    }

    return length;
}

#endif

static word16 TLSX_PointFormat_GetSize(PointFormat* list)
{
    PointFormat* point;
    word16 length = ENUM_LEN; /* list length */

    while ((point = list)) {
        list = point->next;
        length += ENUM_LEN; /* format length */
    }

    return length;
}

#ifndef NO_WOLFSSL_CLIENT

static word16 TLSX_SupportedCurve_Write(SupportedCurve* list, byte* output)
{
    word16 offset = OPAQUE16_LEN;

    while (list) {
        c16toa(list->name, output + offset);
        offset += OPAQUE16_LEN;
        list = list->next;
    }

    c16toa(offset - OPAQUE16_LEN, output); /* writing list length */

    return offset;
}

#endif

static word16 TLSX_PointFormat_Write(PointFormat* list, byte* output)
{
    word16 offset = ENUM_LEN;

    while (list) {
        output[offset++] = list->format;
        list = list->next;
    }

    output[0] = (byte)(offset - ENUM_LEN);

    return offset;
}

#if !defined(NO_WOLFSSL_SERVER) || (defined(WOLFSSL_TLS13) && \
                                         !defined(WOLFSSL_NO_SERVER_GROUPS_EXT))

int TLSX_SupportedCurve_Parse(const WOLFSSL* ssl, const byte* input,
                              word16 length, byte isRequest, TLSX** extensions)
{
    word16 offset;
    word16 name;
    int ret;

    if(!isRequest && !IsAtLeastTLSv1_3(ssl->version)) {
#ifdef WOLFSSL_ALLOW_SERVER_SC_EXT
        return 0;
#else
        return BUFFER_ERROR; /* servers doesn't send this extension. */
#endif
    }

    if (OPAQUE16_LEN > length || length % OPAQUE16_LEN)
        return BUFFER_ERROR;

    ato16(input, &offset);

    /* validating curve list length */
    if (length != OPAQUE16_LEN + offset)
        return BUFFER_ERROR;

    offset = OPAQUE16_LEN;
    if (offset == length)
        return 0;

#if defined(WOLFSSL_TLS13) && !defined(WOLFSSL_NO_SERVER_GROUPS_EXT)
    if (!isRequest) {
        TLSX* extension;
        SupportedCurve* curve;

        extension = TLSX_Find(*extensions, TLSX_SUPPORTED_GROUPS);
        if (extension != NULL) {
            /* Replace client list with server list of supported groups. */
            curve = (SupportedCurve*)extension->data;
            extension->data = NULL;
            TLSX_SupportedCurve_FreeAll(curve, ssl->heap);

            ato16(input + offset, &name);
            offset += OPAQUE16_LEN;

            ret = TLSX_SupportedCurve_New(&curve, name, ssl->heap);
            if (ret != 0)
                return ret; /* throw error */
            extension->data = (void*)curve;
        }
    }
#endif

    for (; offset < length; offset += OPAQUE16_LEN) {
        ato16(input + offset, &name);

        ret = TLSX_UseSupportedCurve(extensions, name, ssl->heap);
        /* If it is BAD_FUNC_ARG then it is a group we do not support, but
         * that is fine. */
        if (ret != WOLFSSL_SUCCESS && ret != BAD_FUNC_ARG) {
            return ret;
        }
    }

    return 0;
}

#endif

#if !defined(NO_WOLFSSL_SERVER)

#if defined(WOLFSSL_TLS13) && !defined(WOLFSSL_NO_SERVER_GROUPS_EXT)

/* Checks the priority of the groups on the server and set the supported groups
 * response if there is a group not advertised by the client that is preferred.
 *
 * ssl  SSL/TLS object.
 * returns 0 on success, otherwise an error.
 */
int TLSX_SupportedCurve_CheckPriority(WOLFSSL* ssl)
{
    int ret;
    TLSX* extension;
    TLSX* priority = NULL;
    TLSX* ext = NULL;
    word16 name;
    SupportedCurve* curve;

    extension = TLSX_Find(ssl->extensions, TLSX_SUPPORTED_GROUPS);
    /* May be doing PSK with no key exchange. */
    if (extension == NULL)
        return 0;

    ret = TLSX_PopulateSupportedGroups(ssl, &priority);
    if (ret != WOLFSSL_SUCCESS) {
        TLSX_FreeAll(priority, ssl->heap);
        return ret;
    }

    ext = TLSX_Find(priority, TLSX_SUPPORTED_GROUPS);
    if (ext == NULL) {
        WOLFSSL_MSG("Could not find supported groups extension");
        TLSX_FreeAll(priority, ssl->heap);
        return 0;
    }

    curve = (SupportedCurve*)ext->data;
    name = curve->name;

    curve = (SupportedCurve*)extension->data;
    while (curve != NULL) {
        if (curve->name == name)
            break;
        curve = curve->next;
    }

    if (curve == NULL) {
        /* Couldn't find the preferred group in client list. */
        extension->resp = 1;

        /* Send server list back and free client list. */
        curve = (SupportedCurve*)extension->data;
        extension->data = ext->data;
        ext->data = curve;
    }

    TLSX_FreeAll(priority, ssl->heap);

    return 0;
}

#endif /* WOLFSSL_TLS13 && !WOLFSSL_NO_SERVER_GROUPS_EXT */

#if defined(HAVE_FFDHE) && !defined(WOLFSSL_NO_TLS12)
#ifdef HAVE_PUBLIC_FFDHE
static int tlsx_ffdhe_find_group(WOLFSSL* ssl, SupportedCurve* clientGroup,
    SupportedCurve* serverGroup)
{
    int ret = 0;
    SupportedCurve* group;
    const DhParams* params = NULL;

    for (; serverGroup != NULL; serverGroup = serverGroup->next) {
        if (!WOLFSSL_NAMED_GROUP_IS_FFHDE(serverGroup->name))
            continue;

        for (group = clientGroup; group != NULL; group = group->next) {
            if (serverGroup->name != group->name)
                continue;

            switch (serverGroup->name) {
            #ifdef HAVE_FFDHE_2048
                case WOLFSSL_FFDHE_2048:
                    params = wc_Dh_ffdhe2048_Get();
                    break;
            #endif
            #ifdef HAVE_FFDHE_3072
                case WOLFSSL_FFDHE_3072:
                    params = wc_Dh_ffdhe3072_Get();
                    break;
            #endif
            #ifdef HAVE_FFDHE_4096
                case WOLFSSL_FFDHE_4096:
                    params = wc_Dh_ffdhe4096_Get();
                    break;
            #endif
            #ifdef HAVE_FFDHE_6144
                case WOLFSSL_FFDHE_6144:
                    params = wc_Dh_ffdhe6144_Get();
                    break;
            #endif
            #ifdef HAVE_FFDHE_8192
                case WOLFSSL_FFDHE_8192:
                    params = wc_Dh_ffdhe8192_Get();
                    break;
            #endif
                default:
                    break;
            }
            if (params == NULL) {
                ret = BAD_FUNC_ARG;
                break;
            }
            if (params->p_len >= ssl->options.minDhKeySz &&
                                     params->p_len <= ssl->options.maxDhKeySz) {
                break;
            }
        }

        if (ret != 0)
            break;
        if ((group != NULL) && (serverGroup->name == group->name))
            break;
    }

    if ((ret == 0) && (serverGroup != NULL) && (params != NULL)) {
        ssl->buffers.serverDH_P.buffer = (unsigned char *)params->p;
        ssl->buffers.serverDH_P.length = params->p_len;
        ssl->buffers.serverDH_G.buffer = (unsigned char *)params->g;
        ssl->buffers.serverDH_G.length = params->g_len;

        ssl->namedGroup = serverGroup->name;
    #if !defined(WOLFSSL_OLD_PRIME_CHECK) && \
        !defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)
        ssl->options.dhDoKeyTest = 0;
    #endif
        ssl->options.haveDH = 1;
    }

    return ret;
}
#else
static int tlsx_ffdhe_find_group(WOLFSSL* ssl, SupportedCurve* clientGroup,
    SupportedCurve* serverGroup)
{
    int ret = 0;
    SupportedCurve* group;
    word32 p_len;

    for (; serverGroup != NULL; serverGroup = serverGroup->next) {
        if (!WOLFSSL_NAMED_GROUP_IS_FFHDE(serverGroup->name))
            continue;

        for (group = clientGroup; group != NULL; group = group->next) {
            if (serverGroup->name != group->name)
                continue;

            wc_DhGetNamedKeyParamSize(serverGroup->name, &p_len, NULL, NULL);
            if (p_len == 0) {
                ret = BAD_FUNC_ARG;
                break;
            }
            if (p_len >= ssl->options.minDhKeySz &&
                                             p_len <= ssl->options.maxDhKeySz) {
                break;
            }
        }

        if (ret != 0)
            break;
        if ((group != NULL) && (serverGroup->name == group->name))
            break;
    }

    if ((ret == 0) && (serverGroup != NULL)) {
        word32 pSz, gSz;

        ssl->buffers.serverDH_P.buffer = NULL;
        ssl->buffers.serverDH_G.buffer = NULL;
        ret = wc_DhGetNamedKeyParamSize(serverGroup->name, &pSz, &gSz, NULL);
        if (ret == 0) {
            ssl->buffers.serverDH_P.buffer =
                (byte*)XMALLOC(pSz, ssl->heap, DYNAMIC_TYPE_PUBLIC_KEY);
            if (ssl->buffers.serverDH_P.buffer == NULL)
                ret = MEMORY_E;
            else
                ssl->buffers.serverDH_P.length = pSz;
        }
        if (ret == 0) {
            ssl->buffers.serverDH_G.buffer =
                (byte*)XMALLOC(gSz, ssl->heap, DYNAMIC_TYPE_PUBLIC_KEY);
            if (ssl->buffers.serverDH_G.buffer == NULL) {
                ret = MEMORY_E;
            } else
                ssl->buffers.serverDH_G.length = gSz;
        }
        if (ret == 0) {
            ret = wc_DhCopyNamedKey(serverGroup->name,
                              ssl->buffers.serverDH_P.buffer, &pSz,
                              ssl->buffers.serverDH_G.buffer, &gSz,
                              NULL, NULL);
        }
        if (ret == 0) {
            ssl->buffers.weOwnDH = 1;

            ssl->namedGroup = serverGroup->name;
        #if !defined(WOLFSSL_OLD_PRIME_CHECK) && \
            !defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)
            ssl->options.dhDoKeyTest = 0;
        #endif
            ssl->options.haveDH = 1;
        }
        else {
            if (ssl->buffers.serverDH_P.buffer != NULL) {
                XFREE(ssl->buffers.serverDH_P.buffer, ssl->heap,
                    DYNAMIC_TYPE_PUBLIC_KEY);
                ssl->buffers.serverDH_P.length = 0;
                ssl->buffers.serverDH_P.buffer = NULL;
            }
            if (ssl->buffers.serverDH_G.buffer != NULL) {
                XFREE(ssl->buffers.serverDH_G.buffer, ssl->heap,
                    DYNAMIC_TYPE_PUBLIC_KEY);
                ssl->buffers.serverDH_G.length = 0;
                ssl->buffers.serverDH_G.buffer = NULL;
            }
        }
    }

    return ret;
}
#endif

/* Set the highest priority common FFDHE group on the server as compared to
 * client extensions.
 *
 * ssl    SSL/TLS object.
 * returns 0 on success, otherwise an error.
 */
int TLSX_SupportedFFDHE_Set(WOLFSSL* ssl)
{
    int ret;
    TLSX* priority = NULL;
    TLSX* ext = NULL;
    TLSX* extension;
    SupportedCurve* clientGroup;
    SupportedCurve* group;
    int found = 0;

    extension = TLSX_Find(ssl->extensions, TLSX_SUPPORTED_GROUPS);
    /* May be doing PSK with no key exchange. */
    if (extension == NULL)
        return 0;
    clientGroup = (SupportedCurve*)extension->data;
    for (group = clientGroup; group != NULL; group = group->next) {
        if (WOLFSSL_NAMED_GROUP_IS_FFHDE(group->name)) {
            found = 1;
            break;
        }
    }
    if (!found)
        return 0;

    if (ssl->buffers.serverDH_P.buffer && ssl->buffers.weOwnDH) {
        XFREE(ssl->buffers.serverDH_P.buffer, ssl->heap,
                                                       DYNAMIC_TYPE_PUBLIC_KEY);
    }
    if (ssl->buffers.serverDH_G.buffer && ssl->buffers.weOwnDH) {
        XFREE(ssl->buffers.serverDH_G.buffer, ssl->heap,
                                                       DYNAMIC_TYPE_PUBLIC_KEY);
    }
    ssl->buffers.serverDH_P.buffer = NULL;
    ssl->buffers.serverDH_G.buffer = NULL;
    ssl->buffers.weOwnDH = 0;
    ssl->options.haveDH = 0;

    ret = TLSX_PopulateSupportedGroups(ssl, &priority);
    if (ret == WOLFSSL_SUCCESS) {
        SupportedCurve* serverGroup;

        ext = TLSX_Find(priority, TLSX_SUPPORTED_GROUPS);
        serverGroup = (SupportedCurve*)ext->data;

        ret = tlsx_ffdhe_find_group(ssl, clientGroup, serverGroup);
    }

    TLSX_FreeAll(priority, ssl->heap);

    return ret;
}
#endif /* HAVE_FFDHE && !WOLFSSL_NO_TLS12 */

#endif /* !NO_WOLFSSL_SERVER */

#if defined(WOLFSSL_TLS13) && !defined(WOLFSSL_NO_SERVER_GROUPS_EXT)
/* Return the preferred group.
 *
 * ssl             SSL/TLS object.
 * checkSupported  Whether to check for the first supported group.
 * returns BAD_FUNC_ARG if no group found, otherwise the group.
 */
int TLSX_SupportedCurve_Preferred(WOLFSSL* ssl, int checkSupported)
{
    TLSX* extension;
    SupportedCurve* curve;

    extension = TLSX_Find(ssl->extensions, TLSX_SUPPORTED_GROUPS);
    if (extension == NULL)
        return BAD_FUNC_ARG;

    curve = (SupportedCurve*)extension->data;
    while (curve != NULL) {
        if (!checkSupported || TLSX_KeyShare_IsSupported(curve->name))
            return curve->name;
        curve = curve->next;
    }

    return BAD_FUNC_ARG;
}

#endif /* HAVE_SUPPORTED_CURVES */

#ifndef NO_WOLFSSL_SERVER

static int TLSX_PointFormat_Parse(WOLFSSL* ssl, const byte* input,
                                  word16 length, byte isRequest)
{
    int ret;

    /* validating formats list length */
    if (ENUM_LEN > length || length != (word16)ENUM_LEN + input[0])
        return BUFFER_ERROR;

    if (isRequest) {
        /* adding uncompressed point format to response */
        ret = TLSX_UsePointFormat(&ssl->extensions, WOLFSSL_EC_PF_UNCOMPRESSED,
                                                                     ssl->heap);
        if (ret != WOLFSSL_SUCCESS)
            return ret; /* throw error */

        TLSX_SetResponse(ssl, TLSX_EC_POINT_FORMATS);
    }

    return 0;
}

#if defined(HAVE_ECC) || defined(HAVE_CURVE25519) || defined(HAVE_CURVE448)
int TLSX_ValidateSupportedCurves(const WOLFSSL* ssl, byte first, byte second,
                                 word32* ecdhCurveOID) {
    TLSX*           extension = NULL;
    SupportedCurve* curve     = NULL;
    word32          oid       = 0;
    word32          defOid    = 0;
    word32          defSz     = 80; /* Maximum known curve size is 66. */
    word32          nextOid   = 0;
    word32          nextSz    = 80; /* Maximum known curve size is 66. */
    word32          currOid   = ssl->ecdhCurveOID;
    int             ephmSuite = 0;
    word16          octets    = 0; /* according to 'ecc_set_type ecc_sets[];' */
    int             key       = 0; /* validate key       */

    (void)oid;

    if (first == CHACHA_BYTE) {
        switch (second) {
            case TLS_DHE_RSA_WITH_CHACHA20_POLY1305_SHA256:
            case TLS_PSK_WITH_CHACHA20_POLY1305_SHA256:
            case TLS_DHE_PSK_WITH_CHACHA20_POLY1305_SHA256:
            case TLS_DHE_RSA_WITH_CHACHA20_OLD_POLY1305_SHA256:
                return 1; /* no suite restriction */
            case TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256:
            case TLS_ECDHE_RSA_WITH_CHACHA20_OLD_POLY1305_SHA256:
            case TLS_ECDHE_PSK_WITH_CHACHA20_POLY1305_SHA256:
                break;
        }
    }
    if (first == ECC_BYTE || first == ECDHE_PSK_BYTE || first == CHACHA_BYTE)
        extension = TLSX_Find(ssl->extensions, TLSX_SUPPORTED_GROUPS);
    if (!extension)
        return 1; /* no suite restriction */

    for (curve = (SupportedCurve*)extension->data;
         curve && !key;
         curve = curve->next) {

    #ifdef OPENSSL_EXTRA
        /* skip if name is not in supported ECC range
         * or disabled by user */
        if (curve->name > WOLFSSL_ECC_MAX ||
            wolfSSL_curve_is_disabled(ssl, curve->name))
            continue;
    #endif

        /* find supported curve */
        switch (curve->name) {
#ifdef HAVE_ECC
    #if (defined(HAVE_ECC160) || defined(HAVE_ALL_CURVES)) && ECC_MIN_KEY_SZ <= 160
        #ifndef NO_ECC_SECP
            case WOLFSSL_ECC_SECP160R1:
                oid = ECC_SECP160R1_OID;
                octets = 20;
                break;
        #endif /* !NO_ECC_SECP */
        #ifdef HAVE_ECC_SECPR2
            case WOLFSSL_ECC_SECP160R2:
                oid = ECC_SECP160R2_OID;
                octets = 20;
                break;
        #endif /* HAVE_ECC_SECPR2 */
        #ifdef HAVE_ECC_KOBLITZ
            case WOLFSSL_ECC_SECP160K1:
                oid = ECC_SECP160K1_OID;
                octets = 20;
                break;
        #endif /* HAVE_ECC_KOBLITZ */
        #endif
    #if (defined(HAVE_ECC192) || defined(HAVE_ALL_CURVES)) && ECC_MIN_KEY_SZ <= 192
        #ifndef NO_ECC_SECP
            case WOLFSSL_ECC_SECP192R1:
                oid = ECC_SECP192R1_OID;
                octets = 24;
                break;
        #endif /* !NO_ECC_SECP */
        #ifdef HAVE_ECC_KOBLITZ
            case WOLFSSL_ECC_SECP192K1:
                oid = ECC_SECP192K1_OID;
                octets = 24;
                break;
        #endif /* HAVE_ECC_KOBLITZ */
    #endif
    #if (defined(HAVE_ECC224) || defined(HAVE_ALL_CURVES)) && ECC_MIN_KEY_SZ <= 224
        #ifndef NO_ECC_SECP
            case WOLFSSL_ECC_SECP224R1:
                oid = ECC_SECP224R1_OID;
                octets = 28;
                break;
        #endif /* !NO_ECC_SECP */
        #ifdef HAVE_ECC_KOBLITZ
            case WOLFSSL_ECC_SECP224K1:
                oid = ECC_SECP224K1_OID;
                octets = 28;
                break;
        #endif /* HAVE_ECC_KOBLITZ */
    #endif
    #if (!defined(NO_ECC256) || defined(HAVE_ALL_CURVES)) && ECC_MIN_KEY_SZ <= 256
        #ifndef NO_ECC_SECP
            case WOLFSSL_ECC_SECP256R1:
                oid = ECC_SECP256R1_OID;
                octets = 32;
                break;
        #endif /* !NO_ECC_SECP */
    #endif /* !NO_ECC256 || HAVE_ALL_CURVES */
#endif
        #if (defined(HAVE_CURVE25519) || defined(HAVE_ED25519)) && ECC_MIN_KEY_SZ <= 256
            case WOLFSSL_ECC_X25519:
                oid = ECC_X25519_OID;
                octets = 32;
                break;
        #endif /* HAVE_CURVE25519 */
#ifdef HAVE_ECC
    #if (!defined(NO_ECC256) || defined(HAVE_ALL_CURVES)) && ECC_MIN_KEY_SZ <= 256
        #ifdef HAVE_ECC_KOBLITZ
            case WOLFSSL_ECC_SECP256K1:
                oid = ECC_SECP256K1_OID;
                octets = 32;
                break;
        #endif /* HAVE_ECC_KOBLITZ */
        #ifdef HAVE_ECC_BRAINPOOL
            case WOLFSSL_ECC_BRAINPOOLP256R1:
                oid = ECC_BRAINPOOLP256R1_OID;
                octets = 32;
                break;
        #endif /* HAVE_ECC_BRAINPOOL */
        #ifdef WOLFSSL_SM2
            case WOLFSSL_ECC_SM2P256V1:
                oid = ECC_SM2P256V1_OID;
                octets = 32;
                break;
        #endif /* WOLFSSL_SM2 */
    #endif
    #if (defined(HAVE_ECC384) || defined(HAVE_ALL_CURVES)) && ECC_MIN_KEY_SZ <= 384
        #ifndef NO_ECC_SECP
            case WOLFSSL_ECC_SECP384R1:
                oid = ECC_SECP384R1_OID;
                octets = 48;
                break;
        #endif /* !NO_ECC_SECP */
        #ifdef HAVE_ECC_BRAINPOOL
            case WOLFSSL_ECC_BRAINPOOLP384R1:
                oid = ECC_BRAINPOOLP384R1_OID;
                octets = 48;
                break;
        #endif /* HAVE_ECC_BRAINPOOL */
    #endif
#endif
        #if (defined(HAVE_CURVE448) || defined(HAVE_ED448)) && ECC_MIN_KEY_SZ <= 448
            case WOLFSSL_ECC_X448:
                oid = ECC_X448_OID;
                octets = 57;
                break;
        #endif /* HAVE_CURVE448 */
#ifdef HAVE_ECC
    #if (defined(HAVE_ECC512) || defined(HAVE_ALL_CURVES)) && ECC_MIN_KEY_SZ <= 512
        #ifdef HAVE_ECC_BRAINPOOL
            case WOLFSSL_ECC_BRAINPOOLP512R1:
                oid = ECC_BRAINPOOLP512R1_OID;
                octets = 64;
                break;
        #endif /* HAVE_ECC_BRAINPOOL */
    #endif
    #if (defined(HAVE_ECC521) || defined(HAVE_ALL_CURVES)) && ECC_MIN_KEY_SZ <= 521
        #ifndef NO_ECC_SECP
            case WOLFSSL_ECC_SECP521R1:
                oid = ECC_SECP521R1_OID;
                octets = 66;
                break;
        #endif /* !NO_ECC_SECP */
    #endif
#endif
            default: continue; /* unsupported curve */
        }

    #ifdef HAVE_ECC
        /* Set default Oid */
        if (defOid == 0 && ssl->eccTempKeySz <= octets && defSz > octets) {
            defOid = oid;
            defSz = octets;
        }

        /* The eccTempKeySz is the preferred ephemeral key size */
        if (currOid == 0 && ssl->eccTempKeySz == octets)
            currOid = oid;
        if ((nextOid == 0 || nextSz > octets) && ssl->eccTempKeySz <= octets) {
            nextOid = oid;
            nextSz  = octets;
        }
    #else
        if (defOid == 0 && defSz > octets) {
            defOid = oid;
            defSz = octets;
        }

        if (currOid == 0)
            currOid = oid;
        if (nextOid == 0 || nextSz > octets) {
            nextOid = oid;
            nextSz  = octets;
        }
    #endif

        if (first == ECC_BYTE) {
            switch (second) {
#if defined(HAVE_ECC) || defined(HAVE_ED25519) || defined(HAVE_ED448)
                /* ECDHE_ECDSA */
                case TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA:
                case TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA:
                case TLS_ECDHE_ECDSA_WITH_RC4_128_SHA:
                case TLS_ECDHE_ECDSA_WITH_3DES_EDE_CBC_SHA:
                case TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256:
                case TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384:
                case TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256:
                case TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384:
                case TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8:
                case TLS_ECDHE_ECDSA_WITH_AES_256_CCM_8:
                    key |= ssl->ecdhCurveOID == oid;
                    ephmSuite = 1;
                break;

    #ifdef WOLFSSL_STATIC_DH
                /* ECDH_ECDSA */
                case TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA:
                case TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA:
                case TLS_ECDH_ECDSA_WITH_RC4_128_SHA:
                case TLS_ECDH_ECDSA_WITH_3DES_EDE_CBC_SHA:
                case TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256:
                case TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA384:
                case TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256:
                case TLS_ECDH_ECDSA_WITH_AES_256_GCM_SHA384:
                    if (oid == ECC_X25519_OID && defOid == oid) {
                        defOid = 0;
                        defSz = 80;
                    }
                    if (oid == ECC_X448_OID && defOid == oid) {
                        defOid = 0;
                        defSz = 80;
                    }
                    key |= ssl->pkCurveOID == oid;
                break;
    #endif /* WOLFSSL_STATIC_DH */
#endif /* HAVE_ECC || HAVE_ED25519 || HAVE_ED448 */
#ifndef NO_RSA
                /* ECDHE_RSA */
                case TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA:
                case TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA:
                case TLS_ECDHE_RSA_WITH_RC4_128_SHA:
                case TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA:
                case TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256:
                case TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384:
                case TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256:
                case TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384:
                    key |= ssl->ecdhCurveOID == oid;
                    ephmSuite = 1;
                break;

    #if defined(HAVE_ECC) && defined(WOLFSSL_STATIC_DH)
                /* ECDH_RSA */
                case TLS_ECDH_RSA_WITH_AES_256_CBC_SHA:
                case TLS_ECDH_RSA_WITH_AES_128_CBC_SHA:
                case TLS_ECDH_RSA_WITH_RC4_128_SHA:
                case TLS_ECDH_RSA_WITH_3DES_EDE_CBC_SHA:
                case TLS_ECDH_RSA_WITH_AES_128_CBC_SHA256:
                case TLS_ECDH_RSA_WITH_AES_256_CBC_SHA384:
                case TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256:
                case TLS_ECDH_RSA_WITH_AES_256_GCM_SHA384:
                    if (oid == ECC_X25519_OID && defOid == oid) {
                        defOid = 0;
                        defSz = 80;
                    }
                    if (oid == ECC_X448_OID && defOid == oid) {
                        defOid = 0;
                        defSz = 80;
                    }
                    key |= ssl->pkCurveOID == oid;
                break;
    #endif /* HAVE_ECC && WOLFSSL_STATIC_DH */
#endif
                default:
                    if (oid == ECC_X25519_OID && defOid == oid) {
                        defOid = 0;
                        defSz = 80;
                    }
                    if (oid == ECC_X448_OID && defOid == oid) {
                        defOid = 0;
                        defSz = 80;
                    }
                    key = 1;
                break;
            }
        }

        /* ChaCha20-Poly1305 ECC cipher suites */
        if (first == CHACHA_BYTE) {
            switch (second) {
#if defined(HAVE_ECC) || defined(HAVE_ED25519) || defined(HAVE_ED448)
                /* ECDHE_ECDSA */
                case TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256 :
                case TLS_ECDHE_ECDSA_WITH_CHACHA20_OLD_POLY1305_SHA256 :
                    key |= ssl->ecdhCurveOID == oid;
                    ephmSuite = 1;
                break;
#endif /* HAVE_ECC || HAVE_ED25519 || HAVE_ED448 */
#ifndef NO_RSA
                /* ECDHE_RSA */
                case TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256 :
                case TLS_ECDHE_RSA_WITH_CHACHA20_OLD_POLY1305_SHA256 :
                    key |= ssl->ecdhCurveOID == oid;
                    ephmSuite = 1;
                break;
#endif
                default:
                    key = 1;
                break;
            }
        }
    }

    *ecdhCurveOID = ssl->ecdhCurveOID;
    /* Choose the default if it is at the required strength. */
#ifdef HAVE_ECC
    if (*ecdhCurveOID == 0 && defSz == ssl->eccTempKeySz)
#else
    if (*ecdhCurveOID == 0)
#endif
    {
        key = 1;
        *ecdhCurveOID = defOid;
    }
    /* Choose any curve at the required strength. */
    if (*ecdhCurveOID == 0) {
        key = 1;
        *ecdhCurveOID = currOid;
    }
    /* Choose the default if it is at the next highest strength. */
    if (*ecdhCurveOID == 0 && defSz == nextSz)
        *ecdhCurveOID = defOid;
    /* Choose any curve at the next highest strength. */
    if (*ecdhCurveOID == 0)
        *ecdhCurveOID = nextOid;
    /* No curve and ephemeral ECC suite requires a matching curve. */
    if (*ecdhCurveOID == 0 && ephmSuite)
        key = 0;

    return key;
}
#endif

#endif /* NO_WOLFSSL_SERVER */


int TLSX_SupportedCurve_Copy(TLSX* src, TLSX** dst, void* heap)
{
    TLSX* extension;
    int ret;

    extension = TLSX_Find(src, TLSX_SUPPORTED_GROUPS);
    if (extension != NULL) {
        SupportedCurve* curve;
        for (curve = (SupportedCurve*)extension->data; curve != NULL;
                curve = curve->next) {
            ret = TLSX_UseSupportedCurve(dst, curve->name, heap);
            if (ret != WOLFSSL_SUCCESS)
                return MEMORY_E;
        }
    }

    return 0;
}

int TLSX_UseSupportedCurve(TLSX** extensions, word16 name, void* heap)
{
    TLSX* extension = NULL;
    SupportedCurve* curve = NULL;
    int ret;

    if (extensions == NULL) {
        return BAD_FUNC_ARG;
    }

#ifdef WOLFSSL_TLS13
    if (! TLSX_KeyShare_IsSupported(name)) {
        return BAD_FUNC_ARG;
    }
#endif

    extension = TLSX_Find(*extensions, TLSX_SUPPORTED_GROUPS);

    if (!extension) {
        ret = TLSX_SupportedCurve_New(&curve, name, heap);
        if (ret != 0)
            return ret;

        ret = TLSX_Push(extensions, TLSX_SUPPORTED_GROUPS, curve, heap);
        if (ret != 0) {
            XFREE(curve, heap, DYNAMIC_TYPE_TLSX);
            return ret;
        }
    }
    else {
        ret = TLSX_SupportedCurve_Append((SupportedCurve*)extension->data, name,
                                                                          heap);
        if (ret != 0)
            return ret;
    }

    return WOLFSSL_SUCCESS;
}

int TLSX_UsePointFormat(TLSX** extensions, byte format, void* heap)
{
    TLSX* extension = NULL;
    PointFormat* point = NULL;
    int ret = 0;

    if (extensions == NULL)
        return BAD_FUNC_ARG;

    extension = TLSX_Find(*extensions, TLSX_EC_POINT_FORMATS);

    if (!extension) {
        ret = TLSX_PointFormat_New(&point, format, heap);
        if (ret != 0)
            return ret;

        ret = TLSX_Push(extensions, TLSX_EC_POINT_FORMATS, point, heap);
        if (ret != 0) {
            XFREE(point, heap, DYNAMIC_TYPE_TLSX);
            return ret;
        }
    }
    else {
        ret = TLSX_PointFormat_Append((PointFormat*)extension->data, format,
                                                                          heap);
        if (ret != 0)
            return ret;
    }

    return WOLFSSL_SUCCESS;
}

#define EC_FREE_ALL         TLSX_SupportedCurve_FreeAll
#define EC_VALIDATE_REQUEST TLSX_SupportedCurve_ValidateRequest

#ifndef NO_WOLFSSL_CLIENT
#define EC_GET_SIZE TLSX_SupportedCurve_GetSize
#define EC_WRITE    TLSX_SupportedCurve_Write
#else
#define EC_GET_SIZE(list)         0
#define EC_WRITE(a, b)            0
#endif

#if !defined(NO_WOLFSSL_SERVER) || (defined(WOLFSSL_TLS13) && \
                                         !defined(WOLFSSL_NO_SERVER_GROUPS_EXT))
#define EC_PARSE TLSX_SupportedCurve_Parse
#else
#define EC_PARSE(a, b, c, d, e)   0
#endif

#define PF_FREE_ALL          TLSX_PointFormat_FreeAll
#define PF_VALIDATE_REQUEST  TLSX_PointFormat_ValidateRequest
#define PF_VALIDATE_RESPONSE TLSX_PointFormat_ValidateResponse

#define PF_GET_SIZE TLSX_PointFormat_GetSize
#define PF_WRITE    TLSX_PointFormat_Write

#ifndef NO_WOLFSSL_SERVER
#define PF_PARSE TLSX_PointFormat_Parse
#else
#define PF_PARSE(a, b, c, d)      0
#endif

#else

#define EC_FREE_ALL(list, heap) WC_DO_NOTHING
#define EC_GET_SIZE(list)         0
#define EC_WRITE(a, b)            0
#define EC_PARSE(a, b, c, d, e)   0
#define EC_VALIDATE_REQUEST(a, b) WC_DO_NOTHING

#define PF_FREE_ALL(list, heap)   WC_DO_NOTHING
#define PF_GET_SIZE(list)         0
#define PF_WRITE(a, b)            0
#define PF_PARSE(a, b, c, d)      0
#define PF_VALIDATE_REQUEST(a, b) WC_DO_NOTHING
#define PF_VALIDATE_RESPONSE(a, b) WC_DO_NOTHING

#endif /* HAVE_SUPPORTED_CURVES */

/******************************************************************************/
/* Renegotiation Indication                                                   */
/******************************************************************************/

#if defined(HAVE_SECURE_RENEGOTIATION) \
 || defined(HAVE_SERVER_RENEGOTIATION_INFO)

static byte TLSX_SecureRenegotiation_GetSize(SecureRenegotiation* data,
                                                                  int isRequest)
{
    byte length = OPAQUE8_LEN; /* empty info length */

    /* data will be NULL for HAVE_SERVER_RENEGOTIATION_INFO only */
    if (data && data->enabled && data->verifySet) {
        /* client sends client_verify_data only */
        length += TLS_FINISHED_SZ;

        /* server also sends server_verify_data */
        if (!isRequest)
            length += TLS_FINISHED_SZ;
    }

    return length;
}

static word16 TLSX_SecureRenegotiation_Write(SecureRenegotiation* data,
                                                    byte* output, int isRequest)
{
    word16 offset = OPAQUE8_LEN; /* RenegotiationInfo length */
    if (data && data->enabled && data->verifySet) {
        /* client sends client_verify_data only */
        XMEMCPY(output + offset, data->client_verify_data, TLS_FINISHED_SZ);
        offset += TLS_FINISHED_SZ;

        /* server also sends server_verify_data */
        if (!isRequest) {
            XMEMCPY(output + offset, data->server_verify_data, TLS_FINISHED_SZ);
            offset += TLS_FINISHED_SZ;
        }
    }

    output[0] = (byte)(offset - 1);  /* info length - self */

    return offset;
}

static int TLSX_SecureRenegotiation_Parse(WOLFSSL* ssl, const byte* input,
                                          word16 length, byte isRequest)
{
    int ret = SECURE_RENEGOTIATION_E;

    if (length >= OPAQUE8_LEN) {
        if (isRequest) {
        #ifndef NO_WOLFSSL_SERVER
            if (ssl->secure_renegotiation == NULL) {
                ret = wolfSSL_UseSecureRenegotiation(ssl);
                if (ret == WOLFSSL_SUCCESS)
                    ret = 0;
            }
            if (ret != 0 && ret != SECURE_RENEGOTIATION_E) {
            }
            else if (ssl->secure_renegotiation == NULL) {
            }
            else if (!ssl->secure_renegotiation->enabled) {
                if (*input == 0) {
                    input++; /* get past size */

                    ssl->secure_renegotiation->enabled = 1;
                    TLSX_SetResponse(ssl, TLSX_RENEGOTIATION_INFO);
                    ret = 0;
                }
                else {
                    /* already in error state */
                    WOLFSSL_MSG("SCR client verify data present");
                }
            }
            else if (*input == TLS_FINISHED_SZ) {
                if (length < TLS_FINISHED_SZ + 1) {
                    WOLFSSL_MSG("SCR malformed buffer");
                    ret = BUFFER_E;
                }
                else {
                    input++; /* get past size */

                    /* validate client verify data */
                    if (XMEMCMP(input,
                            ssl->secure_renegotiation->client_verify_data,
                            TLS_FINISHED_SZ) == 0) {
                        WOLFSSL_MSG("SCR client verify data match");
                        TLSX_SetResponse(ssl, TLSX_RENEGOTIATION_INFO);
                        ret = 0;  /* verified */
                    }
                    else {
                        /* already in error state */
                        WOLFSSL_MSG("SCR client verify data Failure");
                    }
                }
            }
        #endif
        }
        else if (ssl->secure_renegotiation != NULL) {
        #ifndef NO_WOLFSSL_CLIENT
            if (!ssl->secure_renegotiation->enabled) {
                if (*input == 0) {
                    ssl->secure_renegotiation->enabled = 1;
                    ret = 0;
                }
            }
            else if (*input == 2 * TLS_FINISHED_SZ &&
                     length == 2 * TLS_FINISHED_SZ + OPAQUE8_LEN) {
                input++;  /* get past size */

                /* validate client and server verify data */
                if (XMEMCMP(input,
                            ssl->secure_renegotiation->client_verify_data,
                            TLS_FINISHED_SZ) == 0 &&
                    XMEMCMP(input + TLS_FINISHED_SZ,
                            ssl->secure_renegotiation->server_verify_data,
                            TLS_FINISHED_SZ) == 0) {
                    WOLFSSL_MSG("SCR client and server verify data match");
                    ret = 0;  /* verified */
                }
                else {
                    /* already in error state */
                    WOLFSSL_MSG("SCR client and server verify data Failure");
                }
            }
        #endif
        }
    }

    if (ret != 0) {
        WOLFSSL_ERROR_VERBOSE(ret);
        SendAlert(ssl, alert_fatal, handshake_failure);
    }

    return ret;
}

int TLSX_UseSecureRenegotiation(TLSX** extensions, void* heap)
{
    int ret = 0;
    SecureRenegotiation* data;

    data = (SecureRenegotiation*)XMALLOC(sizeof(SecureRenegotiation), heap,
                                                             DYNAMIC_TYPE_TLSX);
    if (data == NULL)
        return MEMORY_E;

    XMEMSET(data, 0, sizeof(SecureRenegotiation));

    ret = TLSX_Push(extensions, TLSX_RENEGOTIATION_INFO, data, heap);
    if (ret != 0) {
        XFREE(data, heap, DYNAMIC_TYPE_TLSX);
        return ret;
    }

    return WOLFSSL_SUCCESS;
}

#ifdef HAVE_SERVER_RENEGOTIATION_INFO

int TLSX_AddEmptyRenegotiationInfo(TLSX** extensions, void* heap)
{
    int ret;

    /* send empty renegotiation_info extension */
    TLSX* ext = TLSX_Find(*extensions, TLSX_RENEGOTIATION_INFO);
    if (ext == NULL) {
        ret = TLSX_UseSecureRenegotiation(extensions, heap);
        if (ret != WOLFSSL_SUCCESS)
            return ret;

        ext = TLSX_Find(*extensions, TLSX_RENEGOTIATION_INFO);
    }
    if (ext)
        ext->resp = 1;

    return WOLFSSL_SUCCESS;
}

#endif /* HAVE_SERVER_RENEGOTIATION_INFO */


#define SCR_FREE_ALL(data, heap) XFREE(data, (heap), DYNAMIC_TYPE_TLSX)
#define SCR_GET_SIZE       TLSX_SecureRenegotiation_GetSize
#define SCR_WRITE          TLSX_SecureRenegotiation_Write
#define SCR_PARSE          TLSX_SecureRenegotiation_Parse

#else

#define SCR_FREE_ALL(a, heap) WC_DO_NOTHING
#define SCR_GET_SIZE(a, b)    0
#define SCR_WRITE(a, b, c)    0
#define SCR_PARSE(a, b, c, d) 0

#endif /* HAVE_SECURE_RENEGOTIATION || HAVE_SERVER_RENEGOTIATION_INFO */

/******************************************************************************/
/* Session Tickets                                                            */
/******************************************************************************/

#ifdef HAVE_SESSION_TICKET

#if defined(WOLFSSL_TLS13) || !defined(NO_WOLFSSL_CLIENT)
static void TLSX_SessionTicket_ValidateRequest(WOLFSSL* ssl)
{
    TLSX*          extension = TLSX_Find(ssl->extensions, TLSX_SESSION_TICKET);
    SessionTicket* ticket    = extension ?
                                         (SessionTicket*)extension->data : NULL;

    if (ticket) {
        /* TODO validate ticket timeout here! */
        if (ticket->lifetime == 0xfffffff) {
            /* send empty ticket on timeout */
            TLSX_UseSessionTicket(&ssl->extensions, NULL, ssl->heap);
        }
    }
}
#endif /* WOLFSSL_TLS13 || !NO_WOLFSSL_CLIENT */


static word16 TLSX_SessionTicket_GetSize(SessionTicket* ticket, int isRequest)
{
    (void)isRequest;
    return ticket ? ticket->size : 0;
}

static word16 TLSX_SessionTicket_Write(SessionTicket* ticket, byte* output,
                                       int isRequest)
{
    word16 offset = 0; /* empty ticket */

    if (isRequest && ticket) {
        XMEMCPY(output + offset, ticket->data, ticket->size);
        offset += ticket->size;
    }

    return offset;
}


static int TLSX_SessionTicket_Parse(WOLFSSL* ssl, const byte* input,
                                    word16 length, byte isRequest)
{
    int ret = 0;

    (void) input; /* avoid unused parameter if NO_WOLFSSL_SERVER defined */

    if (!isRequest) {
        if (TLSX_CheckUnsupportedExtension(ssl, TLSX_SESSION_TICKET))
            return TLSX_HandleUnsupportedExtension(ssl);

        if (length != 0)
            return BUFFER_ERROR;

#ifndef NO_WOLFSSL_CLIENT
        ssl->expect_session_ticket = 1;
#endif
    }
#ifndef NO_WOLFSSL_SERVER
    else {
        /* server side */
        if (ssl->ctx->ticketEncCb == NULL) {
            WOLFSSL_MSG("Client sent session ticket, server has no callback");
            return 0;
        }

#ifdef HAVE_SECURE_RENEGOTIATION
        if (IsSCR(ssl)) {
            WOLFSSL_MSG("Client sent session ticket during SCR. Ignoring.");
            return 0;
        }
#endif

        if (length > SESSION_TICKET_LEN) {
            ret = BAD_TICKET_MSG_SZ;
            WOLFSSL_ERROR_VERBOSE(ret);
        } else if (IsAtLeastTLSv1_3(ssl->version)) {
            WOLFSSL_MSG("Process client ticket rejected, TLS 1.3 no support");
            ssl->options.rejectTicket = 1;
            ret = 0;  /* not fatal */
        } else if (ssl->options.noTicketTls12) {
            /* ignore ticket request */
        } else if (length == 0) {
            /* blank ticket */
            ret = TLSX_UseSessionTicket(&ssl->extensions, NULL, ssl->heap);
            if (ret == WOLFSSL_SUCCESS) {
                ret = 0;
                /* send blank ticket */
                TLSX_SetResponse(ssl, TLSX_SESSION_TICKET);
                ssl->options.createTicket = 1;  /* will send ticket msg */
                ssl->options.useTicket    = 1;
                ssl->options.resuming     = 0;  /* no standard resumption */
                ssl->arrays->sessionIDSz  = 0;  /* no echo on blank ticket */
            }
        } else {
            /* got actual ticket from client */
            ret = DoClientTicket(ssl, input, length);
            if (ret == WOLFSSL_TICKET_RET_OK) {    /* use ticket to resume */
                WOLFSSL_MSG("Using existing client ticket");
                ssl->options.useTicket    = 1;
                ssl->options.resuming     = 1;
                /* SERVER: ticket is peer auth. */
                ssl->options.peerAuthGood = 1;
            } else if (ret == WOLFSSL_TICKET_RET_CREATE) {
                WOLFSSL_MSG("Using existing client ticket, creating new one");
                ret = TLSX_UseSessionTicket(&ssl->extensions, NULL, ssl->heap);
                if (ret == WOLFSSL_SUCCESS) {
                    ret = 0;
                    TLSX_SetResponse(ssl, TLSX_SESSION_TICKET);
                                                    /* send blank ticket */
                    ssl->options.createTicket = 1;  /* will send ticket msg */
                    ssl->options.useTicket    = 1;
                    ssl->options.resuming     = 1;
                    /* SERVER: ticket is peer auth. */
                    ssl->options.peerAuthGood = 1;
                }
            } else if (ret == WOLFSSL_TICKET_RET_REJECT) {
                WOLFSSL_MSG("Process client ticket rejected, not using");
                ssl->options.rejectTicket = 1;
                ret = 0;  /* not fatal */
            } else if (ret == VERSION_ERROR) {
                WOLFSSL_MSG("Process client ticket rejected, bad TLS version");
                ssl->options.rejectTicket = 1;
                ret = 0;  /* not fatal */
            } else if (ret == WOLFSSL_TICKET_RET_FATAL) {
                WOLFSSL_MSG("Process client ticket fatal error, not using");
            } else if (ret < 0) {
                WOLFSSL_MSG("Process client ticket unknown error, not using");
            }
        }
    }
#endif /* NO_WOLFSSL_SERVER */

#if defined(NO_WOLFSSL_CLIENT) && defined(NO_WOLFSSL_SERVER)
    (void)ssl;
#endif

    return ret;
}

WOLFSSL_LOCAL SessionTicket* TLSX_SessionTicket_Create(word32 lifetime,
                                            byte* data, word16 size, void* heap)
{
    SessionTicket* ticket = (SessionTicket*)XMALLOC(sizeof(SessionTicket),
                                                       heap, DYNAMIC_TYPE_TLSX);
    if (ticket) {
        ticket->data = (byte*)XMALLOC(size, heap, DYNAMIC_TYPE_TLSX);
        if (ticket->data == NULL) {
            XFREE(ticket, heap, DYNAMIC_TYPE_TLSX);
            return NULL;
        }

        XMEMCPY(ticket->data, data, size);
        ticket->size     = size;
        ticket->lifetime = lifetime;
    }

    (void)heap;

    return ticket;
}
WOLFSSL_LOCAL void TLSX_SessionTicket_Free(SessionTicket* ticket, void* heap)
{
    if (ticket) {
        XFREE(ticket->data, heap, DYNAMIC_TYPE_TLSX);
        XFREE(ticket,       heap, DYNAMIC_TYPE_TLSX);
    }

    (void)heap;
}

int TLSX_UseSessionTicket(TLSX** extensions, SessionTicket* ticket, void* heap)
{
    int ret = 0;

    if (extensions == NULL)
        return BAD_FUNC_ARG;

    /* If the ticket is NULL, the client will request a new ticket from the
       server. Otherwise, the client will use it in the next client hello. */
    if ((ret = TLSX_Push(extensions, TLSX_SESSION_TICKET, (void*)ticket, heap))
                                                                           != 0)
        return ret;

    return WOLFSSL_SUCCESS;
}

#define WOLF_STK_VALIDATE_REQUEST TLSX_SessionTicket_ValidateRequest
#define WOLF_STK_GET_SIZE         TLSX_SessionTicket_GetSize
#define WOLF_STK_WRITE            TLSX_SessionTicket_Write
#define WOLF_STK_PARSE            TLSX_SessionTicket_Parse
#define WOLF_STK_FREE(stk, heap)  TLSX_SessionTicket_Free((SessionTicket*)(stk),(heap))

#else

#define WOLF_STK_FREE(a, b) WC_DO_NOTHING
#define WOLF_STK_VALIDATE_REQUEST(a) WC_DO_NOTHING
#define WOLF_STK_GET_SIZE(a, b)      0
#define WOLF_STK_WRITE(a, b, c)      0
#define WOLF_STK_PARSE(a, b, c, d)   0

#endif /* HAVE_SESSION_TICKET */

#if defined(HAVE_ENCRYPT_THEN_MAC) && !defined(WOLFSSL_AEAD_ONLY)
/******************************************************************************/
/* Encrypt-then-MAC                                                           */
/******************************************************************************/

#ifndef WOLFSSL_NO_TLS12
static int TLSX_EncryptThenMac_Use(WOLFSSL* ssl);

/**
 * Get the size of the Encrypt-Then-MAC extension.
 *
 * msgType  Type of message to put extension into.
 * pSz      Size of extension data.
 * return SANITY_MSG_E when the message is not allowed to have extension and
 *        0 otherwise.
 */
static int TLSX_EncryptThenMac_GetSize(byte msgType, word16* pSz)
{
    (void)pSz;

    if (msgType != client_hello && msgType != server_hello) {
        WOLFSSL_ERROR_VERBOSE(SANITY_MSG_E);
        return SANITY_MSG_E;
    }

    /* Empty extension */

    return 0;
}

/**
 * Write the Encrypt-Then-MAC extension.
 *
 * data     Unused
 * output   Extension data buffer. Unused.
 * msgType  Type of message to put extension into.
 * pSz      Size of extension data.
 * return SANITY_MSG_E when the message is not allowed to have extension and
 *        0 otherwise.
 */
static int TLSX_EncryptThenMac_Write(void* data, byte* output, byte msgType,
                                     word16* pSz)
{
    (void)data;
    (void)output;
    (void)pSz;

    if (msgType != client_hello && msgType != server_hello) {
        WOLFSSL_ERROR_VERBOSE(SANITY_MSG_E);
        return SANITY_MSG_E;
    }

    /* Empty extension */

    return 0;
}

/**
 * Parse the Encrypt-Then-MAC extension.
 *
 * ssl      SSL object
 * input    Extension data buffer.
 * length   Length of this extension's data.
 * msgType  Type of message to extension appeared in.
 * return SANITY_MSG_E when the message is not allowed to have extension,
 *        BUFFER_ERROR when the extension's data is invalid,
 *        MEMORY_E when unable to allocate memory and
 *        0 otherwise.
 */
static int TLSX_EncryptThenMac_Parse(WOLFSSL* ssl, const byte* input,
                                     word16 length, byte msgType)
{
    int ret;

    (void)input;

    if (msgType != client_hello && msgType != server_hello) {
        WOLFSSL_ERROR_VERBOSE(SANITY_MSG_E);
        return SANITY_MSG_E;
    }

    /* Empty extension */
    if (length != 0)
        return BUFFER_ERROR;

    if (msgType == client_hello) {
        /* Check the user hasn't disallowed use of Encrypt-Then-Mac. */
        if (!ssl->options.disallowEncThenMac) {
            ssl->options.encThenMac = 1;
            /* Set the extension reply. */
            ret = TLSX_EncryptThenMac_Use(ssl);
            if (ret != 0)
                return ret;
        }
        return 0;
    }

    /* Server Hello */
    if (ssl->options.disallowEncThenMac) {
        WOLFSSL_ERROR_VERBOSE(SANITY_MSG_E);
        return SANITY_MSG_E;
    }

    ssl->options.encThenMac = 1;
    return 0;

}

/**
 * Add the Encrypt-Then-MAC extension to list.
 *
 * ssl      SSL object
 * return MEMORY_E when unable to allocate memory and 0 otherwise.
 */
static int TLSX_EncryptThenMac_Use(WOLFSSL* ssl)
{
    int   ret = 0;
    TLSX* extension;

    /* Find the Encrypt-Then-Mac extension if it exists. */
    extension = TLSX_Find(ssl->extensions, TLSX_ENCRYPT_THEN_MAC);
    if (extension == NULL) {
        /* Push new Encrypt-Then-Mac extension. */
        ret = TLSX_Push(&ssl->extensions, TLSX_ENCRYPT_THEN_MAC, NULL,
            ssl->heap);
        if (ret != 0)
            return ret;
    }

    return 0;
}

/**
 * Set the Encrypt-Then-MAC extension as one to respond too.
 *
 * ssl      SSL object
 * return EXT_MISSING when EncryptThenMac extension not in list.
 */
int TLSX_EncryptThenMac_Respond(WOLFSSL* ssl)
{
    TLSX* extension;

    extension = TLSX_Find(ssl->extensions, TLSX_ENCRYPT_THEN_MAC);
    if (extension == NULL)
        return EXT_MISSING;
    extension->resp = 1;

    return 0;
}

#define ETM_GET_SIZE  TLSX_EncryptThenMac_GetSize
#define ETM_WRITE     TLSX_EncryptThenMac_Write
#define ETM_PARSE     TLSX_EncryptThenMac_Parse

#else

#define ETM_GET_SIZE(a, b)    0
#define ETM_WRITE(a, b, c, d) 0
#define ETM_PARSE(a, b, c, d) 0

#endif /* !WOLFSSL_NO_TLS12 */

#endif /* HAVE_ENCRYPT_THEN_MAC && !WOLFSSL_AEAD_ONLY */


#ifdef WOLFSSL_SRTP

/******************************************************************************/
/* DTLS SRTP (Secure Real-time Transport Protocol)                            */
/******************************************************************************/

/* Only support single SRTP profile */
typedef struct TlsxSrtp {
    word16 profileCount;
    word16 ids; /* selected bits */
} TlsxSrtp;

static int TLSX_UseSRTP_GetSize(TlsxSrtp *srtp)
{
    /*   SRTP Profile Len (2)
     *      SRTP Profiles (2)
     *   MKI (master key id) Length */
    return (OPAQUE16_LEN + (srtp->profileCount * OPAQUE16_LEN) + 1);
}

static TlsxSrtp* TLSX_UseSRTP_New(word16 ids, void* heap)
{
    TlsxSrtp* srtp;
    int i;

    srtp = (TlsxSrtp*)XMALLOC(sizeof(TlsxSrtp), heap, DYNAMIC_TYPE_TLSX);
    if (srtp == NULL) {
        WOLFSSL_MSG("TLSX SRTP Memory failure");
        return NULL;
    }

    /* count and test each bit set */
    srtp->profileCount = 0;
    for (i=0; i<16; i++) {
        if (ids & (1 << i)) {
            srtp->profileCount++;
        }
    }
    srtp->ids = ids;

    return srtp;
}

static void TLSX_UseSRTP_Free(TlsxSrtp *srtp, void* heap)
{
    if (srtp != NULL) {
        XFREE(srtp, heap, DYNAMIC_TYPE_TLSX);
    }
    (void)heap;
}

static int TLSX_UseSRTP_Parse(WOLFSSL* ssl, const byte* input, word16 length,
    byte isRequest)
{
    int ret = BAD_FUNC_ARG;
    word16 profile_len = 0;
    word16 profile_value = 0;
    word16 offset = 0;
#ifndef NO_WOLFSSL_SERVER
    int i;
    TlsxSrtp* srtp = NULL;
#endif

    if (length < OPAQUE16_LEN) {
        return BUFFER_ERROR;
    }

    /* reset selected DTLS SRTP profile ID */
    ssl->dtlsSrtpId = 0;

    /* total length, not include itself */
    ato16(input, &profile_len);
    offset += OPAQUE16_LEN;

    if (!isRequest) {
#ifndef NO_WOLFSSL_CLIENT
        if (length < offset + OPAQUE16_LEN)
            return BUFFER_ERROR;

        ato16(input + offset, &profile_value);

        /* check that the profile received was in the ones we support */
        if (profile_value < 16 &&
                               (ssl->dtlsSrtpProfiles & (1 << profile_value))) {
            ssl->dtlsSrtpId = profile_value;
            ret = 0; /* success */
        }
#endif
    }
#ifndef NO_WOLFSSL_SERVER
    else {
        /* parse remainder one profile at a time, looking for match in CTX */
        ret = 0;
        for (i=offset; i<length; i+=OPAQUE16_LEN) {
            ato16(input+i, &profile_value);
            /* find first match */
            if (profile_value < 16 &&
                                 ssl->dtlsSrtpProfiles & (1 << profile_value)) {
                ssl->dtlsSrtpId = profile_value;

                /* make sure we respond with selected SRTP id selected */
                srtp = TLSX_UseSRTP_New((1 << profile_value), ssl->heap);
                if (srtp != NULL) {
                    ret = TLSX_Push(&ssl->extensions, TLSX_USE_SRTP,
                        (void*)srtp, ssl->heap);
                    if (ret == 0) {
                        TLSX_SetResponse(ssl, TLSX_USE_SRTP);
                        /* successfully set extension */
                    }
                }
                else {
                    ret = MEMORY_E;
                }
                break;
            }
        }
    }

    if (ret == 0 && ssl->dtlsSrtpId == 0) {
        WOLFSSL_MSG("TLSX_UseSRTP_Parse profile not found!");
        /* not fatal */
    }
    else if (ret != 0) {
        ssl->dtlsSrtpId = 0;
        TLSX_UseSRTP_Free(srtp, ssl->heap);
    }
#endif
    (void)profile_len;

    return ret;
}

static word16 TLSX_UseSRTP_Write(TlsxSrtp* srtp, byte* output)
{
    word16 offset = 0;
    int i, j;

    c16toa(srtp->profileCount * 2, output + offset);
    offset += OPAQUE16_LEN;
    j = 0;
    for (i = 0; i < srtp->profileCount; i++) {
        for (; j < 16; j++) {
            if (srtp->ids & (1 << j)) {
                c16toa(j, output + offset);
                offset += OPAQUE16_LEN;
            }
        }
    }
    output[offset++] = 0x00; /* MKI Length */

    return offset;
}

static int TLSX_UseSRTP(TLSX** extensions, word16 profiles, void* heap)
{
    int ret = 0;
    TLSX* extension;

    if (extensions == NULL) {
        return BAD_FUNC_ARG;
    }

    extension = TLSX_Find(*extensions, TLSX_USE_SRTP);
    if (extension == NULL) {
        TlsxSrtp* srtp = TLSX_UseSRTP_New(profiles, heap);
        if (srtp == NULL) {
            return MEMORY_E;
        }

        ret = TLSX_Push(extensions, TLSX_USE_SRTP, (void*)srtp, heap);
        if (ret != 0) {
            TLSX_UseSRTP_Free(srtp, heap);
        }
    }

    return ret;
}

#ifndef NO_WOLFSSL_SERVER
    #define SRTP_FREE     TLSX_UseSRTP_Free
    #define SRTP_PARSE    TLSX_UseSRTP_Parse
    #define SRTP_WRITE    TLSX_UseSRTP_Write
    #define SRTP_GET_SIZE TLSX_UseSRTP_GetSize
#else
    #define SRTP_FREE(a, b) WC_DO_NOTHING
    #define SRTP_PARSE(a, b, c, d)      0
    #define SRTP_WRITE(a, b)            0
    #define SRTP_GET_SIZE(a)            0
#endif

#endif /* WOLFSSL_SRTP */


/******************************************************************************/
/* Supported Versions                                                         */
/******************************************************************************/

#ifdef WOLFSSL_TLS13
static WC_INLINE int versionIsGreater(byte isDtls, byte a, byte b)
{
    (void)isDtls;

#ifdef WOLFSSL_DTLS
    /* DTLS version increases backwards (-1,-2,-3,etc) */
    if (isDtls)
        return a < b;
#endif /* WOLFSSL_DTLS */

    return a > b;
}

static WC_INLINE int versionIsLesser(byte isDtls, byte a, byte b)
{
    (void)isDtls;

#ifdef WOLFSSL_DTLS
    /* DTLS version increases backwards (-1,-2,-3,etc) */
    if (isDtls)
        return a > b;
#endif /* WOLFSSL_DTLS */

    return a < b;
}

static WC_INLINE int versionIsAtLeast(byte isDtls, byte a, byte b)
{
    (void)isDtls;

#ifdef WOLFSSL_DTLS
    /* DTLS version increases backwards (-1,-2,-3,etc) */
    if (isDtls)
        return a <= b;
#endif /* WOLFSSL_DTLS */

    return a >= b;
}

static WC_INLINE int versionIsLessEqual(byte isDtls, byte a, byte b)
{
    (void)isDtls;

#ifdef WOLFSSL_DTLS
    /* DTLS version increases backwards (-1,-2,-3,etc) */
    if (isDtls)
        return a >= b;
#endif /* WOLFSSL_DTLS */

    return a <= b;
}

/* Return the size of the SupportedVersions extension's data.
 *
 * data       The SSL/TLS object.
 * msgType The type of the message this extension is being written into.
 * returns the length of data that will be in the extension.
 */
static int TLSX_SupportedVersions_GetSize(void* data, byte msgType, word16* pSz)
{
    WOLFSSL* ssl = (WOLFSSL*)data;
    byte tls13Minor, tls12Minor, tls11Minor, isDtls;

    isDtls = !!ssl->options.dtls;
    tls13Minor = (byte)(isDtls ? DTLSv1_3_MINOR : TLSv1_3_MINOR);
    tls12Minor = (byte)(isDtls ? DTLSv1_2_MINOR : TLSv1_2_MINOR);
    tls11Minor = (byte)(isDtls ? DTLS_MINOR : TLSv1_1_MINOR);

    /* unused on some configuration */
    (void)tls12Minor;
    (void)tls13Minor;
    (void)tls11Minor;

    if (msgType == client_hello) {
        /* TLS v1.2 and TLS v1.3  */
        int cnt = 0;

        if (versionIsLessEqual(isDtls, ssl->options.minDowngrade, tls13Minor)
        #if defined(OPENSSL_EXTRA) || defined(HAVE_WEBSERVER) || \
            defined(WOLFSSL_WPAS_SMALL)
            && (ssl->options.mask & SSL_OP_NO_TLSv1_3) == 0
        #endif
        ) {
            cnt++;
        }

        if (ssl->options.downgrade) {
    #ifndef WOLFSSL_NO_TLS12
            if (versionIsLessEqual(
                    isDtls, ssl->options.minDowngrade, tls12Minor)
#if defined(OPENSSL_EXTRA) || defined(HAVE_WEBSERVER) ||                       \
    defined(WOLFSSL_WPAS_SMALL)
                && (ssl->options.mask & SSL_OP_NO_TLSv1_2) == 0
#endif
            ) {
                cnt++;
            }
#endif
    #ifndef NO_OLD_TLS
            if (versionIsLessEqual(
                    isDtls, ssl->options.minDowngrade, tls11Minor)
            #if defined(OPENSSL_EXTRA) || defined(HAVE_WEBSERVER) || \
                defined(WOLFSSL_WPAS_SMALL)
                && (ssl->options.mask & SSL_OP_NO_TLSv1_1) == 0
            #endif
            ) {
                cnt++;
            }
        #ifdef WOLFSSL_ALLOW_TLSV10
            if (!ssl->options.dtls && (ssl->options.minDowngrade <= TLSv1_MINOR)
            #if defined(OPENSSL_EXTRA) || defined(HAVE_WEBSERVER) || \
                defined(WOLFSSL_WPAS_SMALL)
                && (ssl->options.mask & SSL_OP_NO_TLSv1) == 0
            #endif
            ) {
                cnt++;
            }
        #endif
    #endif
        }

        *pSz += (word16)(OPAQUE8_LEN + cnt * OPAQUE16_LEN);
    }
    else if (msgType == server_hello || msgType == hello_retry_request) {
        *pSz += OPAQUE16_LEN;
    }
    else {
        WOLFSSL_ERROR_VERBOSE(SANITY_MSG_E);
        return SANITY_MSG_E;
    }

    return 0;
}

/* Writes the SupportedVersions extension into the buffer.
 *
 * data    The SSL/TLS object.
 * output  The buffer to write the extension into.
 * msgType The type of the message this extension is being written into.
 * returns the length of data that was written.
 */
static int TLSX_SupportedVersions_Write(void* data, byte* output,
                                        byte msgType, word16* pSz)
{
    WOLFSSL* ssl = (WOLFSSL*)data;
    byte tls13minor, tls12minor, tls11minor, isDtls = 0;

    tls13minor = (byte)TLSv1_3_MINOR;
    tls12minor = (byte)TLSv1_2_MINOR;
    tls11minor = (byte)TLSv1_1_MINOR;

    /* unused in some configuration */
    (void)tls11minor;
    (void)tls12minor;

#ifdef WOLFSSL_DTLS13
    if (ssl->options.dtls) {
        tls13minor = (byte)DTLSv1_3_MINOR;
        tls12minor = (byte)DTLSv1_2_MINOR;
        tls11minor = (byte)DTLS_MINOR;
        isDtls = 1;
    }
#endif /* WOLFSSL_DTLS13 */

    if (msgType == client_hello) {
        byte major = ssl->ctx->method->version.major;

        byte* cnt = output++;
        *cnt = 0;

        if (versionIsLessEqual(isDtls, ssl->options.minDowngrade, tls13minor)
#if defined(OPENSSL_EXTRA) || defined(HAVE_WEBSERVER) ||                       \
    defined(WOLFSSL_WPAS_SMALL)
            && (ssl->options.mask & SSL_OP_NO_TLSv1_3) == 0
#endif
        ) {
            *cnt += OPAQUE16_LEN;
        #ifdef WOLFSSL_TLS13_DRAFT
            /* The TLS draft major number. */
            *(output++) = TLS_DRAFT_MAJOR;
            /* Version of draft supported. */
            *(output++) = TLS_DRAFT_MINOR;
        #else
            *(output++) = major;
            *(output++) = tls13minor;
        #endif
        }

        if (ssl->options.downgrade) {
        #ifndef WOLFSSL_NO_TLS12
            if (versionIsLessEqual(isDtls, ssl->options.minDowngrade, tls12minor)
#if defined(OPENSSL_EXTRA) || defined(HAVE_WEBSERVER) || \
                defined(WOLFSSL_WPAS_SMALL)
                && (ssl->options.mask & SSL_OP_NO_TLSv1_2) == 0
            #endif
            ) {
                *cnt += OPAQUE16_LEN;
                *(output++) = major;
                *(output++) = tls12minor;
            }
        #endif

    #ifndef NO_OLD_TLS
            if (versionIsLessEqual(isDtls, ssl->options.minDowngrade, tls11minor)
            #if defined(OPENSSL_EXTRA) || defined(HAVE_WEBSERVER) || \
                defined(WOLFSSL_WPAS_SMALL)
                && (ssl->options.mask & SSL_OP_NO_TLSv1_1) == 0
            #endif
            ) {
                *cnt += OPAQUE16_LEN;
                *(output++) = major;
                *(output++) = tls11minor;
            }
        #ifdef WOLFSSL_ALLOW_TLSV10
            if (!ssl->options.dtls && (ssl->options.minDowngrade <= TLSv1_MINOR)
            #if defined(OPENSSL_EXTRA) || defined(HAVE_WEBSERVER) || \
                defined(WOLFSSL_WPAS_SMALL)
                && (ssl->options.mask & SSL_OP_NO_TLSv1) == 0
            #endif
            ) {
                *cnt += OPAQUE16_LEN;
                *(output++) = major;
                *(output++) = (byte)TLSv1_MINOR;
            }
        #endif
    #endif
        }

        *pSz += (word16)(OPAQUE8_LEN + *cnt);
    }
    else if (msgType == server_hello || msgType == hello_retry_request) {
        output[0] = ssl->version.major;
        output[1] = ssl->version.minor;

        *pSz += OPAQUE16_LEN;
    }
    else {
        WOLFSSL_ERROR_VERBOSE(SANITY_MSG_E);
        return SANITY_MSG_E;
    }

    return 0;
}

/* Parse the SupportedVersions extension.
 *
 * ssl     The SSL/TLS object.
 * input   The buffer with the extension data.
 * length  The length of the extension data.
 * msgType The type of the message this extension is being parsed from.
 * pv      The output ProtocolVersion for the negotiated version
 * opts    The output options structure. Can be NULL.
 * exts    The output extensions list. Can be NULL.
 * returns 0 on success, otherwise failure.
 */
int TLSX_SupportedVersions_Parse(const WOLFSSL* ssl, const byte* input,
        word16 length, byte msgType, ProtocolVersion* pv, Options* opts,
        TLSX** exts)
{
    /* The client's greatest minor version that we support */
    byte clientGreatestMinor = SSLv3_MINOR;
    int ret;
    byte major, minor;
    byte tls13minor, tls12minor;
    byte isDtls;

    tls13minor = TLSv1_3_MINOR;
    tls12minor = TLSv1_2_MINOR;
    isDtls = ssl->options.dtls == 1;

#ifdef WOLFSSL_DTLS13
    if (ssl->options.dtls) {
        tls13minor = DTLSv1_3_MINOR;
        tls12minor = DTLSv1_2_MINOR;
        clientGreatestMinor = DTLS_MINOR;
    }
#endif /* WOLFSSL_DTLS13 */

    if (msgType == client_hello) {
        int i;
        int len;
        int set = 0;

        /* Must contain a length and at least one version. */
        if (length < OPAQUE8_LEN + OPAQUE16_LEN || (length & 1) != 1)
            return BUFFER_ERROR;

        len = *input;

        /* Protocol version array must fill rest of data. */
        if (length != (word16)OPAQUE8_LEN + len)
            return BUFFER_ERROR;

        input++;

        /* Find first match. */
        for (i = 0; i < len; i += OPAQUE16_LEN) {
            major = input[i];
            minor = input[i + OPAQUE8_LEN];

#ifdef WOLFSSL_TLS13_DRAFT
            if (major == TLS_DRAFT_MAJOR && minor == TLS_DRAFT_MINOR) {
                major = SSLv3_MAJOR;
                minor = TLSv1_3_MINOR;
            }
#else
            if (major == TLS_DRAFT_MAJOR)
                continue;
#endif

            if (major != ssl->ctx->method->version.major)
                continue;

            /* No upgrade allowed. */
            if (versionIsGreater(isDtls, minor, ssl->version.minor))
                continue;

            /* Check downgrade. */
            if (versionIsLesser(isDtls, minor, ssl->version.minor)) {
                if (!ssl->options.downgrade)
                    continue;

                if (versionIsLesser(isDtls, minor, ssl->options.minDowngrade))
                    continue;
            }
            if (versionIsGreater(isDtls, minor, clientGreatestMinor))
                clientGreatestMinor = minor;

            set = 1;
        }
        if (!set) {
            /* No common supported version was negotiated */
            SendAlert((WOLFSSL*)ssl, alert_fatal,
                      wolfssl_alert_protocol_version);
            WOLFSSL_ERROR_VERBOSE(VERSION_ERROR);
            return VERSION_ERROR;
        }
        pv->minor = clientGreatestMinor;
        if (versionIsAtLeast(isDtls, clientGreatestMinor, tls13minor)) {
            if (opts != NULL)
                opts->tls1_3 = 1;

            /* TLS v1.3 requires supported version extension */
            if (exts != NULL &&
                    TLSX_Find(*exts, TLSX_SUPPORTED_VERSIONS) == NULL) {
                ret = TLSX_Push(exts,
                          TLSX_SUPPORTED_VERSIONS, ssl, ssl->heap);
                if (ret != 0) {
                    return ret;
                }
                /* *exts should be pointing to the TLSX_SUPPORTED_VERSIONS
                 * ext in the list since it was pushed. */
                (*exts)->resp = 1;
            }
        }

    }
    else if (msgType == server_hello || msgType == hello_retry_request) {
        /* Must contain one version. */
        if (length != OPAQUE16_LEN)
            return BUFFER_ERROR;

        major = input[0];
        minor = input[OPAQUE8_LEN];

        if (major != ssl->ctx->method->version.major) {
            WOLFSSL_ERROR_VERBOSE(VERSION_ERROR);
            return VERSION_ERROR;
        }

        /* Can't downgrade with this extension below TLS v1.3. */
        if (versionIsLesser(isDtls, minor, tls13minor)) {
            WOLFSSL_ERROR_VERBOSE(VERSION_ERROR);
            return VERSION_ERROR;
        }

        /* Version is TLS v1.2 to handle downgrading from TLS v1.3+. */
        if (ssl->options.downgrade && ssl->version.minor == tls12minor) {
            /* Set minor version back to TLS v1.3+ */
            pv->minor = ssl->ctx->method->version.minor;
        }

        /* No upgrade allowed. */
        if (versionIsLesser(isDtls, ssl->version.minor, minor)) {
            WOLFSSL_ERROR_VERBOSE(VERSION_ERROR);
            return VERSION_ERROR;
        }

        /* Check downgrade. */
        if (versionIsGreater(isDtls, ssl->version.minor, minor)) {
            if (!ssl->options.downgrade) {
                WOLFSSL_ERROR_VERBOSE(VERSION_ERROR);
                return VERSION_ERROR;
            }

            if (versionIsLesser(
                    isDtls, minor, ssl->options.minDowngrade)) {
                WOLFSSL_ERROR_VERBOSE(VERSION_ERROR);
                return VERSION_ERROR;
            }

            /* Downgrade the version. */
            pv->minor = minor;
        }
    }
    else {
        WOLFSSL_ERROR_VERBOSE(SANITY_MSG_E);
        return SANITY_MSG_E;
    }

    return 0;
}

/* Sets a new SupportedVersions extension into the extension list.
 *
 * extensions  The list of extensions.
 * data        The extensions specific data.
 * heap        The heap used for allocation.
 * returns 0 on success, otherwise failure.
 */
static int TLSX_SetSupportedVersions(TLSX** extensions, const void* data,
                                     void* heap)
{
    if (extensions == NULL || data == NULL)
        return BAD_FUNC_ARG;

    return TLSX_Push(extensions, TLSX_SUPPORTED_VERSIONS, data, heap);
}

#define SV_GET_SIZE  TLSX_SupportedVersions_GetSize
#define SV_WRITE     TLSX_SupportedVersions_Write
#define SV_PARSE     TLSX_SupportedVersions_Parse

#else

#define SV_GET_SIZE(a, b, c) 0
#define SV_WRITE(a, b, c, d) 0
#define SV_PARSE(a, b, c, d, e, f, g) 0

#endif /* WOLFSSL_TLS13 */

#if defined(WOLFSSL_TLS13) && defined(WOLFSSL_SEND_HRR_COOKIE)

/******************************************************************************/
/* Cookie                                                                     */
/******************************************************************************/

/* Free the cookie data.
 *
 * cookie  Cookie data.
 * heap    The heap used for allocation.
 */
static void TLSX_Cookie_FreeAll(Cookie* cookie, void* heap)
{
    (void)heap;

    if (cookie != NULL)
        XFREE(cookie, heap, DYNAMIC_TYPE_TLSX);
}

/* Get the size of the encoded Cookie extension.
 * In messages: ClientHello and HelloRetryRequest.
 *
 * cookie   The cookie to write.
 * msgType  The type of the message this extension is being written into.
 * returns the number of bytes of the encoded Cookie extension.
 */
static int TLSX_Cookie_GetSize(Cookie* cookie, byte msgType, word16* pSz)
{
    if (msgType == client_hello || msgType == hello_retry_request) {
        *pSz += OPAQUE16_LEN + cookie->len;
    }
    else {
        WOLFSSL_ERROR_VERBOSE(SANITY_MSG_E);
        return SANITY_MSG_E;
    }
    return 0;
}

/* Writes the Cookie extension into the output buffer.
 * Assumes that the the output buffer is big enough to hold data.
 * In messages: ClientHello and HelloRetryRequest.
 *
 * cookie   The cookie to write.
 * output   The buffer to write into.
 * msgType  The type of the message this extension is being written into.
 * returns the number of bytes written into the buffer.
 */
static int TLSX_Cookie_Write(Cookie* cookie, byte* output, byte msgType,
                             word16* pSz)
{
    if (msgType == client_hello || msgType == hello_retry_request) {
        c16toa(cookie->len, output);
        output += OPAQUE16_LEN;
        XMEMCPY(output, cookie->data, cookie->len);
        *pSz += OPAQUE16_LEN + cookie->len;
    }
    else {
        WOLFSSL_ERROR_VERBOSE(SANITY_MSG_E);
        return SANITY_MSG_E;
    }
    return 0;
}

/* Parse the Cookie extension.
 * In messages: ClientHello and HelloRetryRequest.
 *
 * ssl      The SSL/TLS object.
 * input    The extension data.
 * length   The length of the extension data.
 * msgType  The type of the message this extension is being parsed from.
 * returns 0 on success and other values indicate failure.
 */
static int TLSX_Cookie_Parse(WOLFSSL* ssl, const byte* input, word16 length,
                             byte msgType)
{
    word16  len;
    word16  idx = 0;
    TLSX*   extension;
    Cookie* cookie;

    if (msgType != client_hello && msgType != hello_retry_request) {
        WOLFSSL_ERROR_VERBOSE(SANITY_MSG_E);
        return SANITY_MSG_E;
    }

    /* Message contains length and Cookie which must be at least one byte
     * in length.
     */
    if (length < OPAQUE16_LEN + 1)
        return BUFFER_E;
    ato16(input + idx, &len);
    idx += OPAQUE16_LEN;
    if (length - idx != len)
        return BUFFER_E;

    if (msgType == hello_retry_request)
        return TLSX_Cookie_Use(ssl, input + idx, len, NULL, 0, 0,
                               &ssl->extensions);

    /* client_hello */
    extension = TLSX_Find(ssl->extensions, TLSX_COOKIE);
    if (extension == NULL) {
#ifdef WOLFSSL_DTLS13
        if (ssl->options.dtls && IsAtLeastTLSv1_3(ssl->version))
            /* Allow a cookie extension with DTLS 1.3 because it is possible
             * that a different SSL instance sent the cookie but we are now
             * receiving it. */
            return TLSX_Cookie_Use(ssl, input + idx, len, NULL, 0, 0,
                                   &ssl->extensions);
        else
#endif
        {
            WOLFSSL_ERROR_VERBOSE(HRR_COOKIE_ERROR);
            return HRR_COOKIE_ERROR;
        }
    }

    cookie = (Cookie*)extension->data;
    if (cookie->len != len || XMEMCMP(cookie->data, input + idx, len) != 0) {
        WOLFSSL_ERROR_VERBOSE(HRR_COOKIE_ERROR);
        return HRR_COOKIE_ERROR;
    }

    /* Request seen. */
    extension->resp = 0;

    return 0;
}

/* Use the data to create a new Cookie object in the extensions.
 *
 * ssl    SSL/TLS object.
 * data   Cookie data.
 * len    Length of cookie data in bytes.
 * mac    MAC data.
 * macSz  Length of MAC data in bytes.
 * resp   Indicates the extension will go into a response (HelloRetryRequest).
 * returns 0 on success and other values indicate failure.
 */
int TLSX_Cookie_Use(const WOLFSSL* ssl, const byte* data, word16 len, byte* mac,
                    byte macSz, int resp, TLSX** exts)
{
    int     ret = 0;
    TLSX*   extension;
    Cookie* cookie;

    /* Find the cookie extension if it exists. */
    extension = TLSX_Find(*exts, TLSX_COOKIE);
    if (extension == NULL) {
        /* Push new cookie extension. */
        ret = TLSX_Push(exts, TLSX_COOKIE, NULL, ssl->heap);
        if (ret != 0)
            return ret;

        extension = TLSX_Find(*exts, TLSX_COOKIE);
        if (extension == NULL)
            return MEMORY_E;
    }

    cookie = (Cookie*)XMALLOC(sizeof(Cookie) + len + macSz, ssl->heap,
                              DYNAMIC_TYPE_TLSX);
    if (cookie == NULL)
        return MEMORY_E;

    cookie->len = len + macSz;
    XMEMCPY(cookie->data, data, len);
    if (mac != NULL)
        XMEMCPY(cookie->data + len, mac, macSz);

    if (extension->data != NULL)
        XFREE(extension->data, ssl->heap, DYNAMIC_TYPE_TLSX);

    extension->data = (void*)cookie;
    extension->resp = (byte)resp;

    return 0;
}

#define CKE_FREE_ALL  TLSX_Cookie_FreeAll
#define CKE_GET_SIZE  TLSX_Cookie_GetSize
#define CKE_WRITE     TLSX_Cookie_Write
#define CKE_PARSE     TLSX_Cookie_Parse

#else

#define CKE_FREE_ALL(a, b)    0
#define CKE_GET_SIZE(a, b, c) 0
#define CKE_WRITE(a, b, c, d) 0
#define CKE_PARSE(a, b, c, d) 0

#endif

#if defined(WOLFSSL_TLS13) && !defined(NO_CERTS) && \
    !defined(WOLFSSL_NO_CA_NAMES) && defined(OPENSSL_EXTRA)
/* Currently only settable through compatibility API */
/******************************************************************************/
/* Certificate Authorities                                                       */
/******************************************************************************/

static word16 TLSX_CA_Names_GetSize(void* data)
{
    WOLFSSL* ssl = (WOLFSSL*)data;
    WOLF_STACK_OF(WOLFSSL_X509_NAME)* names;
    word16 size = 0;

    if (ssl->options.side == WOLFSSL_CLIENT_END) {
        /* To add support use a different member like ssl->ca_names and
         * add accessor functions:
         * - *_set0_CA_list
         * - *_get0_CA_list */
        WOLFSSL_MSG("We don't currently support sending the client's list.");
        return 0;
    }

    /* Length of names */
    size += OPAQUE16_LEN;
    for (names = SSL_CA_NAMES(ssl); names != NULL; names = names->next) {
        byte seq[MAX_SEQ_SZ];
        WOLFSSL_X509_NAME* name = names->data.name;

        if (name != NULL) {
            /* 16-bit length | SEQ | Len | DER of name */
            size += (word16)(OPAQUE16_LEN + SetSequence(name->rawLen, seq) +
                             name->rawLen);
        }
    }
    return size;
}

static word16 TLSX_CA_Names_Write(void* data, byte* output)
{
    WOLFSSL* ssl = (WOLFSSL*)data;
    WOLF_STACK_OF(WOLFSSL_X509_NAME)* names;
    byte* len;

    if (ssl->options.side == WOLFSSL_CLIENT_END) {
        /* To add support use a different member like ssl->ca_names and
         * add accessor functions:
         * - *_set0_CA_list
         * - *_get0_CA_list */
        WOLFSSL_MSG("We don't currently support sending the client's list.");
        return 0;
    }

    /* Reserve space for the length value */
    len = output;
    output += OPAQUE16_LEN;
    for (names = SSL_CA_NAMES(ssl); names != NULL; names = names->next) {
        byte seq[MAX_SEQ_SZ];
        WOLFSSL_X509_NAME* name = names->data.name;

        if (name != NULL) {
            c16toa((word16)name->rawLen +
                   (word16)SetSequence(name->rawLen, seq), output);
            output += OPAQUE16_LEN;
            output += SetSequence(name->rawLen, output);
            XMEMCPY(output, name->raw, name->rawLen);
            output += name->rawLen;
        }
    }
    /* Write the total length */
    c16toa((word16)(output - len - OPAQUE16_LEN), len);
    return (word16)(output - len);
}

static int TLSX_CA_Names_Parse(WOLFSSL *ssl, const byte* input,
                                  word16 length, byte isRequest)
{
    word16 extLen;

    (void)isRequest;

    if (ssl->options.side == WOLFSSL_SERVER_END) {
        /* To add support use a different member like ssl->ca_names and
         * add accessor functions:
         * - *_set0_CA_list
         * - *_get0_CA_list */
        WOLFSSL_MSG("We don't currently support parsing the client's list.");
        return 0;
    }

    if (ssl->client_ca_names != ssl->ctx->client_ca_names)
        wolfSSL_sk_X509_NAME_pop_free(ssl->client_ca_names, NULL);
    ssl->client_ca_names = wolfSSL_sk_X509_NAME_new(NULL);
    if (ssl->client_ca_names == NULL)
        return MEMORY_ERROR;

    if (length < OPAQUE16_LEN)
        return BUFFER_ERROR;

    ato16(input, &extLen);
    input += OPAQUE16_LEN;
    length -= OPAQUE16_LEN;
    if (extLen != length)
        return BUFFER_ERROR;

    while (length) {
        word32 idx = 0;
        WOLFSSL_X509_NAME* name = NULL;
        int ret = 0;
        int didInit = FALSE;
        /* Use a DecodedCert struct to get access to GetName to
         * parse DN name */
#ifdef WOLFSSL_SMALL_STACK
        DecodedCert *cert = (DecodedCert *)XMALLOC(
            sizeof(*cert), ssl->heap, DYNAMIC_TYPE_DCERT);
        if (cert == NULL)
            return MEMORY_ERROR;
#else
        DecodedCert cert[1];
#endif

        if (length < OPAQUE16_LEN)
            return BUFFER_ERROR;
        ato16(input, &extLen);
        idx += OPAQUE16_LEN;

        if (extLen > length)
            ret = BUFFER_ERROR;

        if (ret == 0) {
            InitDecodedCert(cert, input + idx, extLen, ssl->heap);
            didInit = TRUE;
            idx += extLen;
            ret = GetName(cert, SUBJECT, extLen);
        }

        if (ret == 0 && (name = wolfSSL_X509_NAME_new()) == NULL)
            ret = MEMORY_ERROR;

        if (ret == 0) {
            CopyDecodedName(name, cert, SUBJECT);
            if (wolfSSL_sk_X509_NAME_push(ssl->client_ca_names, name)
                    == WOLFSSL_FAILURE)
                ret = MEMORY_ERROR;
        }

        if (didInit)
            FreeDecodedCert(cert);

#ifdef WOLFSSL_SMALL_STACK
        XFREE(cert, ssl->heap, DYNAMIC_TYPE_DCERT);
#endif
        if (ret != 0)
            return ret;

        input += idx;
        length -= (word16)idx;
    }
    return 0;
}

#define CAN_GET_SIZE  TLSX_CA_Names_GetSize
#define CAN_WRITE     TLSX_CA_Names_Write
#define CAN_PARSE     TLSX_CA_Names_Parse

#else

#define CAN_GET_SIZE(...)  0
#define CAN_WRITE(...)     0
#define CAN_PARSE(...)     0

#endif

#if !defined(NO_CERTS) && !defined(WOLFSSL_NO_SIGALG)
/******************************************************************************/
/* Signature Algorithms                                                       */
/******************************************************************************/

/* Return the size of the SignatureAlgorithms extension's data.
 *
 * data  Unused
 * returns the length of data that will be in the extension.
 */

static word16 TLSX_SignatureAlgorithms_GetSize(void* data)
{
    SignatureAlgorithms* sa = (SignatureAlgorithms*)data;

    if (sa->hashSigAlgoSz == 0)
        return OPAQUE16_LEN + WOLFSSL_SUITES(sa->ssl)->hashSigAlgoSz;
    else
        return OPAQUE16_LEN + sa->hashSigAlgoSz;
}

/* Creates a bit string of supported hash algorithms with RSA PSS.
 * The bit string is used when determining which signature algorithm to use
 * when creating the CertificateVerify message.
 * Note: Valid data has an even length as each signature algorithm is two bytes.
 *
 * ssl     The SSL/TLS object.
 * input   The buffer with the list of supported signature algorithms.
 * length  The length of the list in bytes.
 * returns 0 on success, BUFFER_ERROR when the length is not even.
 */
static int TLSX_SignatureAlgorithms_MapPss(WOLFSSL *ssl, const byte* input,
                                           word16 length)
{
    word16 i;

    if ((length & 1) == 1)
        return BUFFER_ERROR;

    ssl->pssAlgo = 0;
    for (i = 0; i < length; i += 2) {
        if (input[i] == rsa_pss_sa_algo && input[i + 1] <= sha512_mac)
            ssl->pssAlgo |= 1 << input[i + 1];
    #ifdef WOLFSSL_TLS13
        if (input[i] == rsa_pss_sa_algo && input[i + 1] >= pss_sha256 &&
                                                   input[i + 1] <= pss_sha512) {
            ssl->pssAlgo |= 1 << input[i + 1];
        }
    #endif
    }

    return 0;
}

/* Writes the SignatureAlgorithms extension into the buffer.
 *
 * data    Unused
 * output  The buffer to write the extension into.
 * returns the length of data that was written.
 */
static word16 TLSX_SignatureAlgorithms_Write(void* data, byte* output)
{
    SignatureAlgorithms* sa = (SignatureAlgorithms*)data;
    const Suites* suites = WOLFSSL_SUITES(sa->ssl);
    word16 hashSigAlgoSz;

    if (sa->hashSigAlgoSz == 0) {
        c16toa(suites->hashSigAlgoSz, output);
        XMEMCPY(output + OPAQUE16_LEN, suites->hashSigAlgo,
                suites->hashSigAlgoSz);
        hashSigAlgoSz = suites->hashSigAlgoSz;
    }
    else {
        c16toa(sa->hashSigAlgoSz, output);
        XMEMCPY(output + OPAQUE16_LEN, sa->hashSigAlgo,
                sa->hashSigAlgoSz);
        hashSigAlgoSz = sa->hashSigAlgoSz;
    }

#ifndef NO_RSA
    TLSX_SignatureAlgorithms_MapPss(sa->ssl, output + OPAQUE16_LEN,
            hashSigAlgoSz);
#endif

    return OPAQUE16_LEN + hashSigAlgoSz;
}

/* Parse the SignatureAlgorithms extension.
 *
 * ssl     The SSL/TLS object.
 * input   The buffer with the extension data.
 * length  The length of the extension data.
 * returns 0 on success, otherwise failure.
 */
static int TLSX_SignatureAlgorithms_Parse(WOLFSSL *ssl, const byte* input,
                                  word16 length, byte isRequest, Suites* suites)
{
    word16 len;

    if (!isRequest)
        return BUFFER_ERROR;

    /* Must contain a length and at least algorithm. */
    if (length < OPAQUE16_LEN + OPAQUE16_LEN || (length & 1) != 0)
        return BUFFER_ERROR;

    ato16(input, &len);
    input += OPAQUE16_LEN;

    /* Algorithm array must fill rest of data. */
    if (length != OPAQUE16_LEN + len)
        return BUFFER_ERROR;

    /* Sig Algo list size must be even. */
    if (suites->hashSigAlgoSz % 2 != 0)
        return BUFFER_ERROR;

    /* truncate hashSigAlgo list if too long */
    suites->hashSigAlgoSz = len;
    if (suites->hashSigAlgoSz > WOLFSSL_MAX_SIGALGO) {
        WOLFSSL_MSG("TLSX SigAlgo list exceeds max, truncating");
        suites->hashSigAlgoSz = WOLFSSL_MAX_SIGALGO;
    }
    XMEMCPY(suites->hashSigAlgo, input, suites->hashSigAlgoSz);

    return TLSX_SignatureAlgorithms_MapPss(ssl, input, len);
}

/* Sets a new SignatureAlgorithms extension into the extension list.
 *
 * extensions  The list of extensions.
 * data        The extensions specific data.
 * heap        The heap used for allocation.
 * returns 0 on success, otherwise failure.
 */
static int TLSX_SetSignatureAlgorithms(TLSX** extensions, WOLFSSL* ssl,
                                       void* heap)
{
    SignatureAlgorithms* sa;
    int ret;

    if (extensions == NULL)
        return BAD_FUNC_ARG;

    /* Already present */
    if (TLSX_Find(*extensions, TLSX_SIGNATURE_ALGORITHMS) != NULL)
        return 0;

    sa = TLSX_SignatureAlgorithms_New(ssl, 0, heap);
    if (sa == NULL)
        return MEMORY_ERROR;

    ret = TLSX_Push(extensions, TLSX_SIGNATURE_ALGORITHMS, sa, heap);
    if (ret != 0)
        TLSX_SignatureAlgorithms_FreeAll(sa, heap);
    return ret;
}

SignatureAlgorithms* TLSX_SignatureAlgorithms_New(WOLFSSL* ssl,
        word16 hashSigAlgoSz, void* heap)
{
    SignatureAlgorithms* sa;
    (void)heap;

    sa = (SignatureAlgorithms*)XMALLOC(sizeof(*sa) + hashSigAlgoSz, heap,
                                       DYNAMIC_TYPE_TLSX);
    if (sa != NULL) {
        XMEMSET(sa, 0, sizeof(*sa) + hashSigAlgoSz);
        sa->ssl = ssl;
        sa->hashSigAlgoSz = hashSigAlgoSz;
    }
    return sa;
}

void TLSX_SignatureAlgorithms_FreeAll(SignatureAlgorithms* sa,
                                             void* heap)
{
    XFREE(sa, heap, DYNAMIC_TYPE_TLSX);
    (void)heap;
}

#define SA_GET_SIZE  TLSX_SignatureAlgorithms_GetSize
#define SA_WRITE     TLSX_SignatureAlgorithms_Write
#define SA_PARSE     TLSX_SignatureAlgorithms_Parse
#define SA_FREE_ALL  TLSX_SignatureAlgorithms_FreeAll
#endif
/******************************************************************************/
/* Signature Algorithms Certificate                                           */
/******************************************************************************/

#if defined(WOLFSSL_TLS13) && !defined(NO_CERTS) && !defined(WOLFSSL_NO_SIGALG)
/* Return the size of the SignatureAlgorithms extension's data.
 *
 * data  Unused
 * returns the length of data that will be in the extension.
 */
static word16 TLSX_SignatureAlgorithmsCert_GetSize(void* data)
{
    WOLFSSL* ssl = (WOLFSSL*)data;

    return OPAQUE16_LEN + ssl->certHashSigAlgoSz;
}

/* Writes the SignatureAlgorithmsCert extension into the buffer.
 *
 * data    Unused
 * output  The buffer to write the extension into.
 * returns the length of data that was written.
 */
static word16 TLSX_SignatureAlgorithmsCert_Write(void* data, byte* output)
{
    WOLFSSL* ssl = (WOLFSSL*)data;

    c16toa(ssl->certHashSigAlgoSz, output);
    XMEMCPY(output + OPAQUE16_LEN, ssl->certHashSigAlgo,
            ssl->certHashSigAlgoSz);

    return OPAQUE16_LEN + ssl->certHashSigAlgoSz;
}

/* Parse the SignatureAlgorithmsCert extension.
 *
 * ssl     The SSL/TLS object.
 * input   The buffer with the extension data.
 * length  The length of the extension data.
 * returns 0 on success, otherwise failure.
 */
static int TLSX_SignatureAlgorithmsCert_Parse(WOLFSSL *ssl, const byte* input,
                                              word16 length, byte isRequest)
{
    word16 len;

    if (!isRequest)
        return BUFFER_ERROR;

    /* Must contain a length and at least algorithm. */
    if (length < OPAQUE16_LEN + OPAQUE16_LEN || (length & 1) != 0)
        return BUFFER_ERROR;

    ato16(input, &len);
    input += OPAQUE16_LEN;

    /* Algorithm array must fill rest of data. */
    if (length != OPAQUE16_LEN + len)
        return BUFFER_ERROR;

    /* truncate hashSigAlgo list if too long */
    ssl->certHashSigAlgoSz = len;
    if (ssl->certHashSigAlgoSz > WOLFSSL_MAX_SIGALGO) {
        WOLFSSL_MSG("TLSX SigAlgo list exceeds max, truncating");
        ssl->certHashSigAlgoSz = WOLFSSL_MAX_SIGALGO;
    }
    XMEMCPY(ssl->certHashSigAlgo, input, ssl->certHashSigAlgoSz);

    return 0;
}

/* Sets a new SignatureAlgorithmsCert extension into the extension list.
 *
 * extensions  The list of extensions.
 * data        The extensions specific data.
 * heap        The heap used for allocation.
 * returns 0 on success, otherwise failure.
 */
static int TLSX_SetSignatureAlgorithmsCert(TLSX** extensions,
        const WOLFSSL* data, void* heap)
{
    if (extensions == NULL)
        return BAD_FUNC_ARG;

    return TLSX_Push(extensions, TLSX_SIGNATURE_ALGORITHMS_CERT, data, heap);
}

#define SAC_GET_SIZE  TLSX_SignatureAlgorithmsCert_GetSize
#define SAC_WRITE     TLSX_SignatureAlgorithmsCert_Write
#define SAC_PARSE     TLSX_SignatureAlgorithmsCert_Parse
#endif /* WOLFSSL_TLS13 */


/******************************************************************************/
/* Key Share                                                                  */
/******************************************************************************/

#if defined(WOLFSSL_TLS13) && defined(HAVE_SUPPORTED_CURVES)
/* Create a key share entry using named Diffie-Hellman parameters group.
 * Generates a key pair.
 *
 * ssl   The SSL/TLS object.
 * kse   The key share entry object.
 * returns 0 on success, otherwise failure.
 */
static int TLSX_KeyShare_GenDhKey(WOLFSSL *ssl, KeyShareEntry* kse)
{
    int ret = 0;
#if !defined(NO_DH) && (!defined(NO_CERTS) || !defined(NO_PSK))
    word32 pSz = 0, pvtSz = 0;
    DhKey* dhKey = (DhKey*)kse->key;

    /* Pick the parameters from the named group. */
#ifdef HAVE_PUBLIC_FFDHE
    const DhParams* params = NULL;
    switch (kse->group) {
    #ifdef HAVE_FFDHE_2048
        case WOLFSSL_FFDHE_2048:
            params = wc_Dh_ffdhe2048_Get();
            pvtSz = 29;
            break;
    #endif
    #ifdef HAVE_FFDHE_3072
        case WOLFSSL_FFDHE_3072:
            params = wc_Dh_ffdhe3072_Get();
            pvtSz = 34;
            break;
    #endif
    #ifdef HAVE_FFDHE_4096
        case WOLFSSL_FFDHE_4096:
            params = wc_Dh_ffdhe4096_Get();
            pvtSz = 39;
            break;
    #endif
    #ifdef HAVE_FFDHE_6144
        case WOLFSSL_FFDHE_6144:
            params = wc_Dh_ffdhe6144_Get();
            pvtSz = 46;
            break;
    #endif
    #ifdef HAVE_FFDHE_8192
        case WOLFSSL_FFDHE_8192:
            params = wc_Dh_ffdhe8192_Get();
            pvtSz = 52;
            break;
    #endif
        default:
            break;
    }
    if (params == NULL)
        return BAD_FUNC_ARG;
    pSz = params->p_len;
#else
    pvtSz = wc_DhGetNamedKeyMinSize(kse->group);
    if (pvtSz == 0) {
        return BAD_FUNC_ARG;
    }
    ret = wc_DhGetNamedKeyParamSize(kse->group, &pSz, NULL, NULL);
    if (ret != 0) {
        return BAD_FUNC_ARG;
    }
#endif

    /* Trigger Key Generation */
    if (kse->pubKey == NULL || kse->privKey == NULL) {
        if (kse->key == NULL) {
            kse->key = (DhKey*)XMALLOC(sizeof(DhKey), ssl->heap,
                DYNAMIC_TYPE_DH);
            if (kse->key == NULL)
                return MEMORY_E;

            /* Setup Key */
            ret = wc_InitDhKey_ex((DhKey*)kse->key, ssl->heap, ssl->devId);
            if (ret == 0) {
                dhKey = (DhKey*)kse->key;
            #ifdef HAVE_PUBLIC_FFDHE
                ret = wc_DhSetKey(dhKey, params->p, params->p_len, params->g,
                                                                 params->g_len);
            #else
                ret = wc_DhSetNamedKey(dhKey, kse->group);
            #endif
            }
        }

        /* Allocate space for the private and public key */
        if (ret == 0 && kse->pubKey == NULL) {
            kse->pubKey = (byte*)XMALLOC(pSz, ssl->heap,
                DYNAMIC_TYPE_PUBLIC_KEY);
            if (kse->pubKey == NULL)
                ret = MEMORY_E;
        }

        if (ret == 0 && kse->privKey == NULL) {
            kse->privKey = (byte*)XMALLOC(pvtSz, ssl->heap,
                DYNAMIC_TYPE_PRIVATE_KEY);
            if (kse->privKey == NULL)
                ret = MEMORY_E;
        }

        if (ret == 0) {
        #if defined(WOLFSSL_STATIC_EPHEMERAL) && defined(WOLFSSL_DH_EXTRA)
            ret = wolfSSL_StaticEphemeralKeyLoad(ssl, WC_PK_TYPE_DH, kse->key);
            kse->pubKeyLen = pSz;
            kse->keyLen = pvtSz;
            if (ret == 0) {
                ret = wc_DhExportKeyPair(dhKey,
                    (byte*)kse->privKey, &kse->keyLen, /* private */
                    kse->pubKey, &kse->pubKeyLen /* public */
                );
            }
            else
        #endif
            {
                /* Generate a new key pair */
                /* For async this is called once and when event is done, the
                 *   provided buffers will be populated.
                 * Final processing is zero pad below. */
                kse->pubKeyLen = pSz;
                kse->keyLen = pvtSz;
                ret = DhGenKeyPair(ssl, dhKey,
                    (byte*)kse->privKey, &kse->keyLen, /* private */
                    kse->pubKey, &kse->pubKeyLen /* public */
                );
            #ifdef WOLFSSL_ASYNC_CRYPT
                if (ret == WC_PENDING_E) {
                    return ret;
                }
            #endif
            }
        }
    }

    if (ret == 0) {
        if (pSz != kse->pubKeyLen) {
            /* Zero pad the front of the public key to match prime "p" size */
            XMEMMOVE(kse->pubKey + pSz - kse->pubKeyLen, kse->pubKey,
                kse->pubKeyLen);
            XMEMSET(kse->pubKey, 0, pSz - kse->pubKeyLen);
            kse->pubKeyLen = pSz;
        }

        if (pvtSz != kse->keyLen) {
            /* Zero pad the front of the private key */
            XMEMMOVE(kse->privKey + pvtSz - kse->keyLen, kse->privKey,
                kse->keyLen);
            XMEMSET(kse->privKey, 0, pvtSz - kse->keyLen);
            kse->keyLen = pvtSz;
        }

    #ifdef WOLFSSL_DEBUG_TLS
        WOLFSSL_MSG("Public DH Key");
        WOLFSSL_BUFFER(kse->pubKey, kse->pubKeyLen);
    #endif
    }

    /* Always release the DH key to free up memory.
     * The DhKey will be setup again in TLSX_KeyShare_ProcessDh */
    if (dhKey != NULL)
        wc_FreeDhKey(dhKey);
    if (kse->key != NULL) {
        XFREE(kse->key, ssl->heap, DYNAMIC_TYPE_DH);
        kse->key = NULL;
    }

    if (ret != 0) {
        /* Cleanup on error, otherwise data owned by key share entry */
        if (kse->privKey != NULL) {
            XFREE(kse->privKey, ssl->heap, DYNAMIC_TYPE_PRIVATE_KEY);
            kse->privKey = NULL;
        }
        if (kse->pubKey != NULL) {
            XFREE(kse->pubKey, ssl->heap, DYNAMIC_TYPE_PUBLIC_KEY);
            kse->pubKey = NULL;
        }
    }
#else
    (void)ssl;
    (void)kse;

    ret = NOT_COMPILED_IN;
    WOLFSSL_ERROR_VERBOSE(ret);
#endif

    return ret;
}

/* Create a key share entry using X25519 parameters group.
 * Generates a key pair.
 *
 * ssl   The SSL/TLS object.
 * kse   The key share entry object.
 * returns 0 on success, otherwise failure.
 */
static int TLSX_KeyShare_GenX25519Key(WOLFSSL *ssl, KeyShareEntry* kse)
{
    int ret = 0;
#ifdef HAVE_CURVE25519
    curve25519_key* key = (curve25519_key*)kse->key;

    if (kse->key == NULL) {
        /* Allocate a Curve25519 key to hold private key. */
        kse->key = (curve25519_key*)XMALLOC(sizeof(curve25519_key), ssl->heap,
                                                      DYNAMIC_TYPE_PRIVATE_KEY);
        if (kse->key == NULL) {
            WOLFSSL_MSG("GenX25519Key memory error");
            return MEMORY_E;
        }

        /* Make an Curve25519 key. */
        ret = wc_curve25519_init_ex((curve25519_key*)kse->key, ssl->heap,
            INVALID_DEVID);
        if (ret == 0) {
            /* setting "key" means okay to call wc_curve25519_free */
            key = (curve25519_key*)kse->key;

        #ifdef WOLFSSL_STATIC_EPHEMERAL
            ret = wolfSSL_StaticEphemeralKeyLoad(ssl, WC_PK_TYPE_CURVE25519, kse->key);
            if (ret != 0)
        #endif
            {
                ret = wc_curve25519_make_key(ssl->rng, CURVE25519_KEYSIZE, key);
            }
        }
    }

    if (ret == 0 && kse->pubKey == NULL) {
        /* Allocate space for the public key. */
        kse->pubKey = (byte*)XMALLOC(CURVE25519_KEYSIZE, ssl->heap,
                                                       DYNAMIC_TYPE_PUBLIC_KEY);
        if (kse->pubKey == NULL) {
            WOLFSSL_MSG("GenX25519Key pub memory error");
            ret = MEMORY_E;
        }
    }

    if (ret == 0) {
        /* Export Curve25519 public key. */
        kse->pubKeyLen = CURVE25519_KEYSIZE;
        if (wc_curve25519_export_public_ex(key, kse->pubKey, &kse->pubKeyLen,
                                                  EC25519_LITTLE_ENDIAN) != 0) {
            ret = ECC_EXPORT_ERROR;
            WOLFSSL_ERROR_VERBOSE(ret);
        }
        kse->pubKeyLen = CURVE25519_KEYSIZE; /* always CURVE25519_KEYSIZE */
    }

#ifdef WOLFSSL_DEBUG_TLS
    if (ret == 0) {
        WOLFSSL_MSG("Public Curve25519 Key");
        WOLFSSL_BUFFER(kse->pubKey, kse->pubKeyLen);
    }
#endif

    if (ret != 0) {
        /* Data owned by key share entry otherwise. */
        if (kse->pubKey != NULL) {
            XFREE(kse->pubKey, ssl->heap, DYNAMIC_TYPE_PUBLIC_KEY);
            kse->pubKey = NULL;
        }
        if (key != NULL)
            wc_curve25519_free(key);
        if (kse->key != NULL) {
            XFREE(kse->key, ssl->heap, DYNAMIC_TYPE_PRIVATE_KEY);
            kse->key = NULL;
        }
    }
#else
    (void)ssl;
    (void)kse;

    ret = NOT_COMPILED_IN;
    WOLFSSL_ERROR_VERBOSE(ret);
#endif /* HAVE_CURVE25519 */

    return ret;
}

/* Create a key share entry using X448 parameters group.
 * Generates a key pair.
 *
 * ssl   The SSL/TLS object.
 * kse   The key share entry object.
 * returns 0 on success, otherwise failure.
 */
static int TLSX_KeyShare_GenX448Key(WOLFSSL *ssl, KeyShareEntry* kse)
{
    int ret = 0;
#ifdef HAVE_CURVE448
    curve448_key* key = (curve448_key*)kse->key;

    if (kse->key == NULL) {
        /* Allocate a Curve448 key to hold private key. */
        kse->key = (curve448_key*)XMALLOC(sizeof(curve448_key), ssl->heap,
                                                      DYNAMIC_TYPE_PRIVATE_KEY);
        if (kse->key == NULL) {
            WOLFSSL_MSG("GenX448Key memory error");
            return MEMORY_E;
        }

        /* Make an Curve448 key. */
        ret = wc_curve448_init((curve448_key*)kse->key);
        if (ret == 0) {
            key = (curve448_key*)kse->key;

            #ifdef WOLFSSL_STATIC_EPHEMERAL
            ret = wolfSSL_StaticEphemeralKeyLoad(ssl, WC_PK_TYPE_CURVE448, kse->key);
            if (ret != 0)
        #endif
            {
                ret = wc_curve448_make_key(ssl->rng, CURVE448_KEY_SIZE, key);
            }
        }
    }

    if (ret == 0 && kse->pubKey == NULL) {
        /* Allocate space for the public key. */
        kse->pubKey = (byte*)XMALLOC(CURVE448_KEY_SIZE, ssl->heap,
                                                       DYNAMIC_TYPE_PUBLIC_KEY);
        if (kse->pubKey == NULL) {
            WOLFSSL_MSG("GenX448Key pub memory error");
            ret = MEMORY_E;
        }
    }

    if (ret == 0) {
        /* Export Curve448 public key. */
        kse->pubKeyLen = CURVE448_KEY_SIZE;
        if (wc_curve448_export_public_ex(key, kse->pubKey, &kse->pubKeyLen,
                                                    EC448_LITTLE_ENDIAN) != 0) {
            ret = ECC_EXPORT_ERROR;
        }
        kse->pubKeyLen = CURVE448_KEY_SIZE; /* always CURVE448_KEY_SIZE */
    }

#ifdef WOLFSSL_DEBUG_TLS
    if (ret == 0) {
        WOLFSSL_MSG("Public Curve448 Key");
        WOLFSSL_BUFFER(kse->pubKey, kse->pubKeyLen);
    }
#endif

    if (ret != 0) {
        /* Data owned by key share entry otherwise. */
        if (kse->pubKey != NULL) {
            XFREE(kse->pubKey, ssl->heap, DYNAMIC_TYPE_PUBLIC_KEY);
            kse->pubKey = NULL;
        }
        if (key != NULL)
            wc_curve448_free(key);
        if (kse->key != NULL) {
            XFREE(kse->key, ssl->heap, DYNAMIC_TYPE_PRIVATE_KEY);
            kse->key = NULL;
        }
    }
#else
    (void)ssl;
    (void)kse;

    ret = NOT_COMPILED_IN;
    WOLFSSL_ERROR_VERBOSE(ret);
#endif /* HAVE_CURVE448 */

    return ret;
}

/* Create a key share entry using named elliptic curve parameters group.
 * Generates a key pair.
 *
 * ssl   The SSL/TLS object.
 * kse   The key share entry object.
 * returns 0 on success, otherwise failure.
 */
static int TLSX_KeyShare_GenEccKey(WOLFSSL *ssl, KeyShareEntry* kse)
{
    int ret = 0;
#if defined(HAVE_ECC) && defined(HAVE_ECC_KEY_EXPORT)
    word32 keySize = 0;
    word16 curveId = (word16) ECC_CURVE_INVALID;
    ecc_key* eccKey = (ecc_key*)kse->key;

    /* TODO: [TLS13] The key sizes should come from wolfcrypt. */
    /* Translate named group to a curve id. */
    switch (kse->group) {
    #if (!defined(NO_ECC256)  || defined(HAVE_ALL_CURVES)) && ECC_MIN_KEY_SZ <= 256
        #ifndef NO_ECC_SECP
        case WOLFSSL_ECC_SECP256R1:
            curveId = ECC_SECP256R1;
            keySize = 32;
            break;
        #endif /* !NO_ECC_SECP */
        #ifdef WOLFSSL_SM2
        case WOLFSSL_ECC_SM2P256V1:
            curveId = ECC_SM2P256V1;
            keySize = 32;
            break;
        #endif /* !NO_ECC_SECP */
    #endif
    #if (defined(HAVE_ECC384) || defined(HAVE_ALL_CURVES)) && ECC_MIN_KEY_SZ <= 384
        #ifndef NO_ECC_SECP
        case WOLFSSL_ECC_SECP384R1:
            curveId = ECC_SECP384R1;
            keySize = 48;
            break;
        #endif /* !NO_ECC_SECP */
    #endif
    #if (defined(HAVE_ECC521) || defined(HAVE_ALL_CURVES)) && ECC_MIN_KEY_SZ <= 521
        #ifndef NO_ECC_SECP
        case WOLFSSL_ECC_SECP521R1:
            curveId = ECC_SECP521R1;
            keySize = 66;
            break;
        #endif /* !NO_ECC_SECP */
    #endif
        default:
            WOLFSSL_ERROR_VERBOSE(BAD_FUNC_ARG);
            return BAD_FUNC_ARG;
    }

    if (kse->key == NULL) {
        kse->keyLen = keySize;
        kse->pubKeyLen = keySize * 2 + 1;

    #if defined(WOLFSSL_RENESAS_TSIP_TLS)
        ret = tsip_Tls13GenEccKeyPair(ssl, kse);
        if (ret != CRYPTOCB_UNAVAILABLE) {
            return ret;
        }
    #endif
        /* Allocate an ECC key to hold private key. */
        kse->key = (byte*)XMALLOC(sizeof(ecc_key), ssl->heap, DYNAMIC_TYPE_ECC);
        if (kse->key == NULL) {
            WOLFSSL_MSG("EccTempKey Memory error");
            return MEMORY_E;
        }

        /* Make an ECC key */
        ret = wc_ecc_init_ex((ecc_key*)kse->key, ssl->heap, ssl->devId);
        if (ret == 0) {
            /* setting eccKey means okay to call wc_ecc_free */
            eccKey = (ecc_key*)kse->key;

        #ifdef WOLFSSL_STATIC_EPHEMERAL
            ret = wolfSSL_StaticEphemeralKeyLoad(ssl, WC_PK_TYPE_ECDH, kse->key);
            if (ret != 0)
        #endif
            {
                /* set curve info for EccMakeKey "peer" info */
                ret = wc_ecc_set_curve(eccKey, kse->keyLen, curveId);
                if (ret == 0) {
                    /* Generate ephemeral ECC key */
                    /* For async this is called once and when event is done, the
                    *   provided buffers in key be populated.
                    * Final processing is x963 key export below. */
                    ret = EccMakeKey(ssl, eccKey, eccKey);
                }
            #ifdef WOLFSSL_ASYNC_CRYPT
                if (ret == WC_PENDING_E)
                    return ret;
            #endif
            }
        }
    }

    if (ret == 0 && kse->pubKey == NULL) {
        /* Allocate space for the public key */
        kse->pubKey = (byte*)XMALLOC(kse->pubKeyLen, ssl->heap,
            DYNAMIC_TYPE_PUBLIC_KEY);
        if (kse->pubKey == NULL) {
            WOLFSSL_MSG("Key data Memory error");
            ret = MEMORY_E;
        }
    }

    if (ret == 0) {
        XMEMSET(kse->pubKey, 0, kse->pubKeyLen);

        /* Export public key. */
        PRIVATE_KEY_UNLOCK();
        if (wc_ecc_export_x963(eccKey, kse->pubKey, &kse->pubKeyLen) != 0) {
            ret = ECC_EXPORT_ERROR;
            WOLFSSL_ERROR_VERBOSE(ret);
        }
        PRIVATE_KEY_LOCK();
    }
#ifdef WOLFSSL_DEBUG_TLS
    if (ret == 0) {
        WOLFSSL_MSG("Public ECC Key");
        WOLFSSL_BUFFER(kse->pubKey, kse->pubKeyLen);
    }
#endif

    if (ret != 0) {
        /* Cleanup on error, otherwise data owned by key share entry */
        if (kse->pubKey != NULL) {
            XFREE(kse->pubKey, ssl->heap, DYNAMIC_TYPE_PUBLIC_KEY);
            kse->pubKey = NULL;
        }
        if (eccKey != NULL)
            wc_ecc_free(eccKey);
        if (kse->key != NULL) {
            XFREE(kse->key, ssl->heap, DYNAMIC_TYPE_PRIVATE_KEY);
            kse->key = NULL;
        }
    }
#else
    (void)ssl;
    (void)kse;

    ret = NOT_COMPILED_IN;
    WOLFSSL_ERROR_VERBOSE(ret);
#endif /* HAVE_ECC && HAVE_ECC_KEY_EXPORT */

    return ret;
}

#ifdef HAVE_PQC
static int kyber_id2type(int id, int *type)
{
    int ret = 0;

    switch (id) {
    #ifdef WOLFSSL_KYBER512
        case WOLFSSL_KYBER_LEVEL1:
            *type = KYBER512;
            break;
    #endif
    #ifdef WOLFSSL_KYBER768
        case WOLFSSL_KYBER_LEVEL3:
            *type = KYBER768;
            break;
    #endif
    #ifdef WOLFSSL_KYBER1024
        case WOLFSSL_KYBER_LEVEL5:
            *type = KYBER1024;
            break;
    #endif
        default:
            ret = NOT_COMPILED_IN;
            break;
    }

    return ret;
}

typedef struct PqcHybridMapping {
    int hybrid;
    int ecc;
    int pqc;
} PqcHybridMapping;

static const PqcHybridMapping pqc_hybrid_mapping[] = {
    {.hybrid = WOLFSSL_P256_KYBER_LEVEL1,     .ecc = WOLFSSL_ECC_SECP256R1,
     .pqc = WOLFSSL_KYBER_LEVEL1},
    {.hybrid = WOLFSSL_P384_KYBER_LEVEL3,     .ecc = WOLFSSL_ECC_SECP384R1,
     .pqc = WOLFSSL_KYBER_LEVEL3},
    {.hybrid = WOLFSSL_P521_KYBER_LEVEL5,     .ecc = WOLFSSL_ECC_SECP521R1,
     .pqc = WOLFSSL_KYBER_LEVEL5},
    {.hybrid = 0, .ecc = 0, .pqc = 0}
};

/* This will map an ecc-pqs hybrid group into its ecc group and pqc kem group.
 * If it cannot find a mapping then *pqc is set to group. ecc is optional. */
static void findEccPqc(int *ecc, int *pqc, int group)
{
    int i;
    if (pqc == NULL) {
        return;
    }

    *pqc = 0;
    if (ecc != NULL) {
        *ecc = 0;
    }

    for (i = 0; pqc_hybrid_mapping[i].hybrid != 0; i++) {
        if (pqc_hybrid_mapping[i].hybrid == group) {
            *pqc = pqc_hybrid_mapping[i].pqc;
            if (ecc != NULL) {
                *ecc = pqc_hybrid_mapping[i].ecc;
            }
            break;
        }
    }

    if (*pqc == 0) {
        /* It is not a hybrid, so maybe its simple. */
        *pqc = group;
    }
}

/* Create a key share entry using liboqs parameters group.
 * Generates a key pair.
 *
 * ssl   The SSL/TLS object.
 * kse   The key share entry object.
 * returns 0 on success, otherwise failure.
 */
static int TLSX_KeyShare_GenPqcKey(WOLFSSL *ssl, KeyShareEntry* kse)
{
    int ret = 0;
    int type = 0;
    KyberKey kem[1];
    byte* pubKey = NULL;
    byte* privKey = NULL;
    KeyShareEntry *ecc_kse = NULL;
    int oqs_group = 0;
    int ecc_group = 0;
    word32 privSz = 0;
    word32 pubSz = 0;

    findEccPqc(&ecc_group, &oqs_group, kse->group);
    ret = kyber_id2type(oqs_group, &type);
    if (ret == NOT_COMPILED_IN) {
        WOLFSSL_MSG("Invalid Kyber algorithm specified.");
        ret = BAD_FUNC_ARG;
    }

    if (ret == 0) {
        ret = wc_KyberKey_Init(type, kem, ssl->heap, ssl->devId);
        if (ret != 0) {
            WOLFSSL_MSG("Failed to initialize Kyber Key.");
        }
    }

    if (ret == 0) {
        ecc_kse = (KeyShareEntry*)XMALLOC(sizeof(*ecc_kse), ssl->heap,
                   DYNAMIC_TYPE_TLSX);
        if (ecc_kse == NULL) {
            WOLFSSL_MSG("ecc_kse memory allocation failure");
            ret = MEMORY_ERROR;
        }
    }

    if (ret == 0) {
        XMEMSET(ecc_kse, 0, sizeof(*ecc_kse));

        ret = wc_KyberKey_PrivateKeySize(kem, &privSz);
    }
    if (ret == 0) {
        ret = wc_KyberKey_PublicKeySize(kem, &pubSz);
    }

    if (ret == 0 && ecc_group != 0) {
        ecc_kse->group = ecc_group;
        ret = TLSX_KeyShare_GenEccKey(ssl, ecc_kse);
        /* If fail, no error message,  TLSX_KeyShare_GenEccKey will do it. */
    }

    if (ret == 0) {
        pubKey = (byte*)XMALLOC(ecc_kse->pubKeyLen + pubSz, ssl->heap,
                                DYNAMIC_TYPE_PUBLIC_KEY);
        if (pubKey == NULL) {
            WOLFSSL_MSG("pubkey memory allocation failure");
            ret = MEMORY_ERROR;
        }
    }

    if (ret == 0) {
        privKey = (byte*)XMALLOC(privSz, ssl->heap, DYNAMIC_TYPE_PRIVATE_KEY);
        if (privKey == NULL) {
            WOLFSSL_MSG("privkey memory allocation failure");
            ret = MEMORY_ERROR;
        }
    }

    if (ret == 0) {
        ret = wc_KyberKey_MakeKey(kem, ssl->rng);
        if (ret != 0) {
            WOLFSSL_MSG("Kyber keygen failure");
        }
    }
    if (ret == 0) {
        ret = wc_KyberKey_EncodePublicKey(kem, pubKey + ecc_kse->pubKeyLen,
            pubSz);
    }
    if (ret == 0) {
        ret = wc_KyberKey_EncodePrivateKey(kem, privKey, privSz);
    }
    if (ret == 0) {
        XMEMCPY(pubKey, ecc_kse->pubKey, ecc_kse->pubKeyLen);
        kse->pubKey = pubKey;
        kse->pubKeyLen = ecc_kse->pubKeyLen + pubSz;
        pubKey = NULL;

        /* Note we are saving the OQS private key and ECC private key
         * separately. That's because the ECC private key is not simply a
         * buffer. Its is an ecc_key struct.
         */
        kse->privKey = privKey;
        privKey = NULL;

        kse->key = ecc_kse->key;
        ecc_kse->key = NULL;
    }

#ifdef WOLFSSL_DEBUG_TLS
    WOLFSSL_MSG("Public Kyber Key");
    WOLFSSL_BUFFER(kse->pubKey, kse->pubKeyLen );
#endif

    wc_KyberKey_Free(kem);
    TLSX_KeyShare_FreeAll(ecc_kse, ssl->heap);
    if (pubKey != NULL)
        XFREE(pubKey, ssl->heap, DYNAMIC_TYPE_PUBLIC_KEY);
    if (privKey != NULL)
        XFREE(privKey, ssl->heap, DYNAMIC_TYPE_PRIVATE_KEY);

    return ret;
}
#endif /* HAVE_PQC */

/* Generate a secret/key using the key share entry.
 *
 * ssl  The SSL/TLS object.
 * kse  The key share entry holding peer data.
 */
int TLSX_KeyShare_GenKey(WOLFSSL *ssl, KeyShareEntry *kse)
{
    int ret;
    /* Named FFDHE groups have a bit set to identify them. */
    if (WOLFSSL_NAMED_GROUP_IS_FFHDE(kse->group))
        ret = TLSX_KeyShare_GenDhKey(ssl, kse);
    else if (kse->group == WOLFSSL_ECC_X25519)
        ret = TLSX_KeyShare_GenX25519Key(ssl, kse);
    else if (kse->group == WOLFSSL_ECC_X448)
        ret = TLSX_KeyShare_GenX448Key(ssl, kse);
#ifdef HAVE_PQC
    else if (WOLFSSL_NAMED_GROUP_IS_PQC(kse->group))
        ret = TLSX_KeyShare_GenPqcKey(ssl, kse);
#endif
    else
        ret = TLSX_KeyShare_GenEccKey(ssl, kse);
#ifdef WOLFSSL_ASYNC_CRYPT
    kse->lastRet = ret;
#endif
    return ret;
}

/* Free the key share dynamic data.
 *
 * list  The linked list of key share entry objects.
 * heap  The heap used for allocation.
 */
static void TLSX_KeyShare_FreeAll(KeyShareEntry* list, void* heap)
{
    KeyShareEntry* current;

    while ((current = list) != NULL) {
        list = current->next;
        if (WOLFSSL_NAMED_GROUP_IS_FFHDE(current->group)) {
#ifndef NO_DH
            wc_FreeDhKey((DhKey*)current->key);
#endif
        }
        else if (current->group == WOLFSSL_ECC_X25519) {
#ifdef HAVE_CURVE25519
            wc_curve25519_free((curve25519_key*)current->key);
#endif
        }
        else if (current->group == WOLFSSL_ECC_X448) {
#ifdef HAVE_CURVE448
            wc_curve448_free((curve448_key*)current->key);
#endif
        }
#ifdef HAVE_PQC
        else if (WOLFSSL_NAMED_GROUP_IS_PQC(current->group) &&
                 current->key != NULL) {
            ForceZero((byte*)current->key, current->keyLen);
        }
#endif
        else {
#ifdef HAVE_ECC
            wc_ecc_free((ecc_key*)current->key);
#endif
        }
        XFREE(current->key, heap, DYNAMIC_TYPE_PRIVATE_KEY);
    #if !defined(NO_DH) && (!defined(NO_CERTS) || !defined(NO_PSK))
        XFREE(current->privKey, heap, DYNAMIC_TYPE_PRIVATE_KEY);
    #endif
        XFREE(current->pubKey, heap, DYNAMIC_TYPE_PUBLIC_KEY);
        XFREE(current->ke, heap, DYNAMIC_TYPE_PUBLIC_KEY);
        XFREE(current, heap, DYNAMIC_TYPE_TLSX);
    }

    (void)heap;
}

/* Get the size of the encoded key share extension.
 *
 * list     The linked list of key share extensions.
 * msgType  The type of the message this extension is being written into.
 * returns the number of bytes of the encoded key share extension.
 */
static word16 TLSX_KeyShare_GetSize(KeyShareEntry* list, byte msgType)
{
    word16         len = 0;
    byte           isRequest = (msgType == client_hello);
    KeyShareEntry* current;

    /* The named group the server wants to use. */
    if (msgType == hello_retry_request)
        return OPAQUE16_LEN;

    /* List of key exchange groups. */
    if (isRequest)
        len += OPAQUE16_LEN;
    while ((current = list) != NULL) {
        list = current->next;

        if (!isRequest && current->pubKey == NULL)
            continue;

        len += (word16)(KE_GROUP_LEN + OPAQUE16_LEN + current->pubKeyLen);
    }

    return len;
}

/* Writes the key share extension into the output buffer.
 * Assumes that the the output buffer is big enough to hold data.
 *
 * list     The linked list of key share entries.
 * output   The buffer to write into.
 * msgType  The type of the message this extension is being written into.
 * returns the number of bytes written into the buffer.
 */
static word16 TLSX_KeyShare_Write(KeyShareEntry* list, byte* output,
                                  byte msgType)
{
    word16         i = 0;
    byte           isRequest = (msgType == client_hello);
    KeyShareEntry* current;

    if (msgType == hello_retry_request) {
        c16toa(list->group, output);
        return OPAQUE16_LEN;
    }

    /* ClientHello has a list but ServerHello is only the chosen. */
    if (isRequest)
        i += OPAQUE16_LEN;

    /* Write out all in the list. */
    while ((current = list) != NULL) {
        list = current->next;

        if (!isRequest && current->pubKey == NULL)
            continue;

        c16toa(current->group, &output[i]);
        i += KE_GROUP_LEN;
        c16toa((word16)(current->pubKeyLen), &output[i]);
        i += OPAQUE16_LEN;
        XMEMCPY(&output[i], current->pubKey, current->pubKeyLen);
        i += (word16)current->pubKeyLen;
    }
    /* Write the length of the list if required. */
    if (isRequest)
        c16toa(i - OPAQUE16_LEN, output);

    return i;
}

/* Process the DH key share extension on the client side.
 *
 * ssl            The SSL/TLS object.
 * keyShareEntry  The key share entry object to use to calculate shared secret.
 * returns 0 on success and other values indicate failure.
 */
static int TLSX_KeyShare_ProcessDh(WOLFSSL* ssl, KeyShareEntry* keyShareEntry)
{
    int ret = 0;
#if !defined(NO_DH) && (!defined(NO_CERTS) || !defined(NO_PSK))
    word32 pSz = 0;
    DhKey* dhKey = (DhKey*)keyShareEntry->key;

#ifdef HAVE_PUBLIC_FFDHE
    const DhParams* params = NULL;
    switch (keyShareEntry->group) {
    #ifdef HAVE_FFDHE_2048
        case WOLFSSL_FFDHE_2048:
            params = wc_Dh_ffdhe2048_Get();
            break;
    #endif
    #ifdef HAVE_FFDHE_3072
        case WOLFSSL_FFDHE_3072:
            params = wc_Dh_ffdhe3072_Get();
            break;
    #endif
    #ifdef HAVE_FFDHE_4096
        case WOLFSSL_FFDHE_4096:
            params = wc_Dh_ffdhe4096_Get();
            break;
    #endif
    #ifdef HAVE_FFDHE_6144
        case WOLFSSL_FFDHE_6144:
            params = wc_Dh_ffdhe6144_Get();
            break;
    #endif
    #ifdef HAVE_FFDHE_8192
        case WOLFSSL_FFDHE_8192:
            params = wc_Dh_ffdhe8192_Get();
            break;
    #endif
        default:
            break;
    }
    if (params == NULL) {
        WOLFSSL_ERROR_VERBOSE(PEER_KEY_ERROR);
        return PEER_KEY_ERROR;
    }
    pSz = params->p_len;
#else
    ret = wc_DhGetNamedKeyParamSize(keyShareEntry->group, &pSz, NULL, NULL);
    if (ret != 0 || pSz == 0) {
        WOLFSSL_ERROR_VERBOSE(PEER_KEY_ERROR);
        return PEER_KEY_ERROR;
    }
#endif

    /* if DhKey is not setup, do it now */
    if (keyShareEntry->key == NULL) {
        keyShareEntry->key = (DhKey*)XMALLOC(sizeof(DhKey), ssl->heap,
            DYNAMIC_TYPE_DH);
        if (keyShareEntry->key == NULL)
            return MEMORY_E;

        /* Setup Key */
        ret = wc_InitDhKey_ex((DhKey*)keyShareEntry->key, ssl->heap, ssl->devId);
        if (ret == 0) {
            dhKey = (DhKey*)keyShareEntry->key;
        /* Set key */
        #ifdef HAVE_PUBLIC_FFDHE
            ret = wc_DhSetKey(dhKey, params->p, params->p_len, params->g,
                                                                params->g_len);
        #else
            ret = wc_DhSetNamedKey(dhKey, keyShareEntry->group);
        #endif
        }
    }

    if (ret == 0
    #ifdef WOLFSSL_ASYNC_CRYPT
        && keyShareEntry->lastRet == 0 /* don't enter here if WC_PENDING_E */
    #endif
    ) {
    #ifdef WOLFSSL_DEBUG_TLS
        WOLFSSL_MSG("Peer DH Key");
        WOLFSSL_BUFFER(keyShareEntry->ke, keyShareEntry->keLen);
    #endif

        ssl->options.dhKeySz = (word16)pSz;

        /* Derive secret from private key and peer's public key. */
        ret = DhAgree(ssl, dhKey,
            (const byte*)keyShareEntry->privKey, keyShareEntry->keyLen, /* our private */
            keyShareEntry->ke, keyShareEntry->keLen,                    /* peer's public key */
            ssl->arrays->preMasterSecret, &ssl->arrays->preMasterSz,    /* secret */
            NULL, 0
        );
    #ifdef WOLFSSL_ASYNC_CRYPT
        if (ret == WC_PENDING_E) {
            return ret;
        }
    #endif
    }

    /* RFC 8446 Section 7.4.1:
     *     ... left-padded with zeros up to the size of the prime. ...
     */
    if (ret == 0 && (word32)ssl->options.dhKeySz > ssl->arrays->preMasterSz) {
        word32 diff = (word32)ssl->options.dhKeySz - ssl->arrays->preMasterSz;
        XMEMMOVE(ssl->arrays->preMasterSecret + diff,
                        ssl->arrays->preMasterSecret, ssl->arrays->preMasterSz);
        XMEMSET(ssl->arrays->preMasterSecret, 0, diff);
        ssl->arrays->preMasterSz = ssl->options.dhKeySz;
    }

    /* done with key share, release resources */
    if (dhKey)
        wc_FreeDhKey(dhKey);
    if (keyShareEntry->key) {
        XFREE(keyShareEntry->key, ssl->heap, DYNAMIC_TYPE_DH);
        keyShareEntry->key = NULL;
    }
    if (keyShareEntry->privKey != NULL) {
        XFREE(keyShareEntry->privKey, ssl->heap, DYNAMIC_TYPE_PRIVATE_KEY);
        keyShareEntry->privKey = NULL;
    }
    if (keyShareEntry->pubKey != NULL) {
        XFREE(keyShareEntry->pubKey, ssl->heap, DYNAMIC_TYPE_PUBLIC_KEY);
        keyShareEntry->pubKey = NULL;
    }
    XFREE(keyShareEntry->ke, ssl->heap, DYNAMIC_TYPE_PUBLIC_KEY);
    keyShareEntry->ke = NULL;
#else
    (void)ssl;
    (void)keyShareEntry;
    ret = PEER_KEY_ERROR;
    WOLFSSL_ERROR_VERBOSE(ret);
#endif
    return ret;
}

/* Process the X25519 key share extension on the client side.
 *
 * ssl            The SSL/TLS object.
 * keyShareEntry  The key share entry object to use to calculate shared secret.
 * returns 0 on success and other values indicate failure.
 */
static int TLSX_KeyShare_ProcessX25519(WOLFSSL* ssl,
                                       KeyShareEntry* keyShareEntry)
{
    int ret;

#ifdef HAVE_CURVE25519
    curve25519_key* key = (curve25519_key*)keyShareEntry->key;
    curve25519_key* peerX25519Key;

#ifdef HAVE_ECC
    if (ssl->peerEccKey != NULL) {
        wc_ecc_free(ssl->peerEccKey);
        ssl->peerEccKey = NULL;
        ssl->peerEccKeyPresent = 0;
    }
#endif

    peerX25519Key = (curve25519_key*)XMALLOC(sizeof(curve25519_key), ssl->heap,
                                                             DYNAMIC_TYPE_TLSX);
    if (peerX25519Key == NULL) {
        WOLFSSL_MSG("PeerEccKey Memory error");
        return MEMORY_ERROR;
    }
    ret = wc_curve25519_init(peerX25519Key);
    if (ret != 0) {
        XFREE(peerX25519Key, ssl->heap, DYNAMIC_TYPE_TLSX);
        return ret;
    }
#ifdef WOLFSSL_DEBUG_TLS
    WOLFSSL_MSG("Peer Curve25519 Key");
    WOLFSSL_BUFFER(keyShareEntry->ke, keyShareEntry->keLen);
#endif

    if (wc_curve25519_check_public(keyShareEntry->ke, keyShareEntry->keLen,
                                                  EC25519_LITTLE_ENDIAN) != 0) {
        ret = ECC_PEERKEY_ERROR;
        WOLFSSL_ERROR_VERBOSE(ret);
    }

    if (ret == 0) {
        if (wc_curve25519_import_public_ex(keyShareEntry->ke,
                                            keyShareEntry->keLen, peerX25519Key,
                                            EC25519_LITTLE_ENDIAN) != 0) {
            ret = ECC_PEERKEY_ERROR;
            WOLFSSL_ERROR_VERBOSE(ret);
        }
    }

    if (ret == 0) {
        ssl->ecdhCurveOID = ECC_X25519_OID;

        ret = wc_curve25519_shared_secret_ex(key, peerX25519Key,
                                                   ssl->arrays->preMasterSecret,
                                                   &ssl->arrays->preMasterSz,
                                                   EC25519_LITTLE_ENDIAN);
    }

    wc_curve25519_free(peerX25519Key);
    XFREE(peerX25519Key, ssl->heap, DYNAMIC_TYPE_TLSX);
    wc_curve25519_free((curve25519_key*)keyShareEntry->key);
    if (keyShareEntry->key != NULL) {
        XFREE(keyShareEntry->key, ssl->heap, DYNAMIC_TYPE_PRIVATE_KEY);
        keyShareEntry->key = NULL;
    }
#else
    (void)ssl;
    (void)keyShareEntry;

    ret = PEER_KEY_ERROR;
    WOLFSSL_ERROR_VERBOSE(ret);
#endif /* HAVE_CURVE25519 */

    return ret;
}

/* Process the X448 key share extension on the client side.
 *
 * ssl            The SSL/TLS object.
 * keyShareEntry  The key share entry object to use to calculate shared secret.
 * returns 0 on success and other values indicate failure.
 */
static int TLSX_KeyShare_ProcessX448(WOLFSSL* ssl, KeyShareEntry* keyShareEntry)
{
    int ret;

#ifdef HAVE_CURVE448
    curve448_key* key = (curve448_key*)keyShareEntry->key;
    curve448_key* peerX448Key;

#ifdef HAVE_ECC
    if (ssl->peerEccKey != NULL) {
        wc_ecc_free(ssl->peerEccKey);
        ssl->peerEccKey = NULL;
        ssl->peerEccKeyPresent = 0;
    }
#endif

    peerX448Key = (curve448_key*)XMALLOC(sizeof(curve448_key), ssl->heap,
                                                             DYNAMIC_TYPE_TLSX);
    if (peerX448Key == NULL) {
        WOLFSSL_MSG("PeerEccKey Memory error");
        return MEMORY_ERROR;
    }
    ret = wc_curve448_init(peerX448Key);
    if (ret != 0) {
        XFREE(peerX448Key, ssl->heap, DYNAMIC_TYPE_TLSX);
        return ret;
    }
#ifdef WOLFSSL_DEBUG_TLS
    WOLFSSL_MSG("Peer Curve448 Key");
    WOLFSSL_BUFFER(keyShareEntry->ke, keyShareEntry->keLen);
#endif

    if (wc_curve448_check_public(keyShareEntry->ke, keyShareEntry->keLen,
                                                    EC448_LITTLE_ENDIAN) != 0) {
        ret = ECC_PEERKEY_ERROR;
        WOLFSSL_ERROR_VERBOSE(ret);
    }

    if (ret == 0) {
        if (wc_curve448_import_public_ex(keyShareEntry->ke,
                                              keyShareEntry->keLen, peerX448Key,
                                              EC448_LITTLE_ENDIAN) != 0) {
            ret = ECC_PEERKEY_ERROR;
            WOLFSSL_ERROR_VERBOSE(ret);
        }
    }

    if (ret == 0) {
        ssl->ecdhCurveOID = ECC_X448_OID;

        ret = wc_curve448_shared_secret_ex(key, peerX448Key,
                                                   ssl->arrays->preMasterSecret,
                                                   &ssl->arrays->preMasterSz,
                                                   EC448_LITTLE_ENDIAN);
    }

    wc_curve448_free(peerX448Key);
    XFREE(peerX448Key, ssl->heap, DYNAMIC_TYPE_TLSX);
    wc_curve448_free((curve448_key*)keyShareEntry->key);
    if (keyShareEntry->key != NULL) {
        XFREE(keyShareEntry->key, ssl->heap, DYNAMIC_TYPE_PRIVATE_KEY);
        keyShareEntry->key = NULL;
    }
#else
    (void)ssl;
    (void)keyShareEntry;

    ret = PEER_KEY_ERROR;
    WOLFSSL_ERROR_VERBOSE(ret);
#endif /* HAVE_CURVE448 */

    return ret;
}

/* Process the ECC key share extension on the client side.
 *
 * ssl            The SSL/TLS object.
 * keyShareEntry  The key share entry object to use to calculate shared secret.
 * returns 0 on success and other values indicate failure.
 */
static int TLSX_KeyShare_ProcessEcc(WOLFSSL* ssl, KeyShareEntry* keyShareEntry)
{
    int ret = 0;
#ifdef HAVE_ECC
    int curveId = ECC_CURVE_INVALID;
    ecc_key* eccKey = (ecc_key*)keyShareEntry->key;

    /* find supported curve */
    switch (keyShareEntry->group) {
    #if (!defined(NO_ECC256)  || defined(HAVE_ALL_CURVES)) && ECC_MIN_KEY_SZ <= 256
        #ifndef NO_ECC_SECP
        case WOLFSSL_ECC_SECP256R1:
            curveId = ECC_SECP256R1;
            break;
        #endif /* !NO_ECC_SECP */
        #ifdef WOLFSSL_SM2
        case WOLFSSL_ECC_SM2P256V1:
            curveId = ECC_SM2P256V1;
            break;
        #endif
    #endif
    #if (defined(HAVE_ECC384) || defined(HAVE_ALL_CURVES)) && ECC_MIN_KEY_SZ <= 384
        #ifndef NO_ECC_SECP
        case WOLFSSL_ECC_SECP384R1:
            curveId = ECC_SECP384R1;
            break;
        #endif /* !NO_ECC_SECP */
    #endif
    #if (defined(HAVE_ECC521) || defined(HAVE_ALL_CURVES)) && ECC_MIN_KEY_SZ <= 521
        #ifndef NO_ECC_SECP
        case WOLFSSL_ECC_SECP521R1:
            curveId = ECC_SECP521R1;
            break;
        #endif /* !NO_ECC_SECP */
    #endif
    #if defined(HAVE_X448) && ECC_MIN_KEY_SZ <= 448
        case WOLFSSL_ECC_X448:
            curveId = ECC_X448;
            break;
    #endif
        default:
            /* unsupported curve */
            WOLFSSL_ERROR_VERBOSE(ECC_PEERKEY_ERROR);
            return ECC_PEERKEY_ERROR;
    }

#ifdef WOLFSSL_ASYNC_CRYPT
    if (keyShareEntry->lastRet == 0) /* don't enter here if WC_PENDING_E */
#endif
    {
    #ifdef WOLFSSL_DEBUG_TLS
        WOLFSSL_MSG("Peer ECC Key");
        WOLFSSL_BUFFER(keyShareEntry->ke, keyShareEntry->keLen);
    #endif

        if (ssl->peerEccKey != NULL) {
            wc_ecc_free(ssl->peerEccKey);
            XFREE(ssl->peerEccKey, ssl->heap, DYNAMIC_TYPE_ECC);
            ssl->peerEccKeyPresent = 0;
        }
#if defined(WOLFSSL_RENESAS_TSIP_TLS)
        ret = tsip_Tls13GenSharedSecret(ssl, keyShareEntry);
        if (ret != CRYPTOCB_UNAVAILABLE) {
            return ret;
        }
        ret = 0;
#endif

        ssl->peerEccKey = (ecc_key*)XMALLOC(sizeof(ecc_key), ssl->heap,
                                            DYNAMIC_TYPE_ECC);
        if (ssl->peerEccKey == NULL) {
            WOLFSSL_MSG("PeerEccKey Memory error");
            ret = MEMORY_ERROR;
        }

        if (ret == 0) {
            ret = wc_ecc_init_ex(ssl->peerEccKey, ssl->heap, ssl->devId);
        }

        /* Point is validated by import function. */
        if (ret == 0) {
            ret = wc_ecc_import_x963_ex(keyShareEntry->ke, keyShareEntry->keLen,
                                ssl->peerEccKey, curveId);
            if (ret != 0) {
                ret = ECC_PEERKEY_ERROR;
                WOLFSSL_ERROR_VERBOSE(ret);
            }
        }

        if (ret == 0) {
            ssl->ecdhCurveOID = ssl->peerEccKey->dp->oidSum;
            ssl->peerEccKeyPresent = 1;
        }
    }

    if (ret == 0 && eccKey == NULL)
        ret = BAD_FUNC_ARG;
    if (ret == 0) {
        ret = EccSharedSecret(ssl, eccKey, ssl->peerEccKey,
            keyShareEntry->ke, &keyShareEntry->keLen,
            ssl->arrays->preMasterSecret, &ssl->arrays->preMasterSz,
            ssl->options.side
        );
    #ifdef WOLFSSL_ASYNC_CRYPT
        if (ret == WC_PENDING_E)
            return ret;
    #endif
    }

    /* done with key share, release resources */
    if (ssl->peerEccKey != NULL
    #ifdef HAVE_PK_CALLBACKS
        && ssl->ctx->EccSharedSecretCb == NULL
    #endif
    ) {
        wc_ecc_free(ssl->peerEccKey);
        XFREE(ssl->peerEccKey, ssl->heap, DYNAMIC_TYPE_ECC);
        ssl->peerEccKey = NULL;
        ssl->peerEccKeyPresent = 0;
    }
    if (keyShareEntry->key) {
        wc_ecc_free((ecc_key*)keyShareEntry->key);
        XFREE(keyShareEntry->key, ssl->heap, DYNAMIC_TYPE_ECC);
        keyShareEntry->key = NULL;
    }
    XFREE(keyShareEntry->ke, ssl->heap, DYNAMIC_TYPE_PUBLIC_KEY);
    keyShareEntry->ke = NULL;
#else
    (void)ssl;
    (void)keyShareEntry;

    ret = PEER_KEY_ERROR;
    WOLFSSL_ERROR_VERBOSE(ret);
#endif /* HAVE_ECC */

    return ret;
}

#ifdef HAVE_PQC
/* Process the Kyber key share extension on the client side.
 *
 * ssl            The SSL/TLS object.
 * keyShareEntry  The key share entry object to use to calculate shared secret.
 * returns 0 on success and other values indicate failure.
 */
static int TLSX_KeyShare_ProcessPqc(WOLFSSL* ssl, KeyShareEntry* keyShareEntry)
{
    int      ret = 0;
    int      type;
    KyberKey kem[1];
    byte*    sharedSecret = NULL;
    word32   sharedSecretLen = 0;
    int      oqs_group = 0;
    int      ecc_group = 0;
    ecc_key  eccpubkey;
    word32   outlen = 0;
    word32   privSz = 0;
    word32   ctSz = 0;
    word32   ssSz = 0;

    if (keyShareEntry->ke == NULL) {
        WOLFSSL_MSG("Invalid OQS algorithm specified.");
        return BAD_FUNC_ARG;
    }

    if (ssl->options.side == WOLFSSL_SERVER_END) {
        /* I am the server, the shared secret has already been generated and
         * is in keyShareEntry->ke; copy it to the pre-master secret
         * pre-allocated buffer. */
        if (keyShareEntry->keLen > ENCRYPT_LEN) {
            WOLFSSL_MSG("shared secret is too long.");
            return LENGTH_ERROR;
        }

        XMEMCPY(ssl->arrays->preMasterSecret, keyShareEntry->ke,
                keyShareEntry->keLen);
        ssl->arrays->preMasterSz = keyShareEntry->keLen;
        XFREE(keyShareEntry->ke, sl->heap, DYNAMIC_TYPE_SECRET)
        keyShareEntry->ke = NULL;
        keyShareEntry->keLen = 0;
        return 0;
    }

    /* I am the client, the ciphertext is in keyShareEntry->ke */
    findEccPqc(&ecc_group, &oqs_group, keyShareEntry->group);

    ret = kyber_id2type(oqs_group, &type);
    if (ret != 0) {
        WOLFSSL_MSG("Invalid OQS algorithm specified.");
        ret = BAD_FUNC_ARG;
    }
    if (ret == 0) {
        ret = wc_KyberKey_Init(type, kem, ssl->heap, INVALID_DEVID);
        if (ret != 0) {
            WOLFSSL_MSG("Error creating Kyber KEM");
        }
    }

    if (ret == 0) {
        ret = wc_KyberKey_SharedSecretSize(kem, &ssSz);
    }
    if (ret == 0) {
        sharedSecretLen = ssSz;
        switch (ecc_group) {
        case WOLFSSL_ECC_SECP256R1:
            sharedSecretLen += 32;
            outlen = 32;
            break;
        case WOLFSSL_ECC_SECP384R1:
            sharedSecretLen += 48;
            outlen = 48;
            break;
        case WOLFSSL_ECC_SECP521R1:
            sharedSecretLen += 66;
            outlen = 66;
            break;
        default:
            break;
        }

        ret = wc_ecc_init_ex(&eccpubkey, ssl->heap, ssl->devId);
        if (ret != 0) {
            WOLFSSL_MSG("Memory allocation error.");
            ret = MEMORY_E;
        }
    }
    if (ret == 0) {
        sharedSecret = (byte*)XMALLOC(sharedSecretLen, ssl->heap,
                                      DYNAMIC_TYPE_TLSX);
        if (sharedSecret == NULL) {
            WOLFSSL_MSG("Memory allocation error.");
            ret = MEMORY_E;
        }
    }
    if (ret == 0) {
        ret = wc_KyberKey_CipherTextSize(kem, &ctSz);
    }
    if (ret == 0) {
        ret = wc_KyberKey_PrivateKeySize(kem, &privSz);
    }
    if (ret == 0) {
        ret = wc_KyberKey_DecodePrivateKey(kem, keyShareEntry->privKey, privSz);
    }
    if (ret == 0) {
        ret = wc_KyberKey_Decapsulate(kem, sharedSecret + outlen,
            keyShareEntry->ke + keyShareEntry->keLen - ctSz, ctSz);
        if (ret != 0) {
            WOLFSSL_MSG("wc_KyberKey decapsulation failure.");
            ret = BAD_FUNC_ARG;
        }
    }

    if (ecc_group != 0) {
        if (ret == 0) {
            /* Point is validated by import function. */
            ret = wc_ecc_import_x963(keyShareEntry->ke,
                                     keyShareEntry->keLen - ctSz,
                                     &eccpubkey);
            if (ret != 0) {
                WOLFSSL_MSG("ECC Public key import error.");
            }
        }

#if defined(ECC_TIMING_RESISTANT) && (!defined(HAVE_FIPS) || \
    (!defined(HAVE_FIPS_VERSION) || (HAVE_FIPS_VERSION != 2))) && \
    !defined(HAVE_SELFTEST)
        if (ret == 0) {
            ret = wc_ecc_set_rng(keyShareEntry->key, ssl->rng);
            if (ret != 0) {
                WOLFSSL_MSG("Failure to set the ECC private key RNG.");
            }
        }
#endif

        if (ret == 0) {
            PRIVATE_KEY_UNLOCK();
            ret = wc_ecc_shared_secret(keyShareEntry->key, &eccpubkey,
                sharedSecret, &outlen);
            PRIVATE_KEY_LOCK();
            if (outlen != sharedSecretLen - ssSz) {
                WOLFSSL_MSG("ECC shared secret derivation error.");
                ret = BAD_FUNC_ARG;
            }
        }
    }
    if ((ret == 0) && (sharedSecretLen > ENCRYPT_LEN)) {
        WOLFSSL_MSG("shared secret is too long.");
        ret = LENGTH_ERROR;
    }

    if (ret == 0) {
         /* Copy the shared secret to the  pre-master secret pre-allocated
          * buffer. */
        XMEMCPY(ssl->arrays->preMasterSecret, sharedSecret, sharedSecretLen);
        ssl->arrays->preMasterSz = (word32) sharedSecretLen;
    }

    if (sharedSecret != NULL) {
        XFREE(sharedSecret, ssl->heap, DYNAMIC_TYPE_SECRET);
    }

    wc_ecc_free(&eccpubkey);
    wc_KyberKey_Free(kem);
    return ret;
}
#endif /* HAVE_PQC */

/* Process the key share extension on the client side.
 *
 * ssl            The SSL/TLS object.
 * keyShareEntry  The key share entry object to use to calculate shared secret.
 * returns 0 on success and other values indicate failure.
 */
static int TLSX_KeyShare_Process(WOLFSSL* ssl, KeyShareEntry* keyShareEntry)
{
    int ret;

#if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
    ssl->session->namedGroup = keyShareEntry->group;
#endif
    /* reset the pre master secret size */
    if (ssl->arrays->preMasterSz == 0)
        ssl->arrays->preMasterSz = ENCRYPT_LEN;

    /* Use Key Share Data from server. */
    if (WOLFSSL_NAMED_GROUP_IS_FFHDE(keyShareEntry->group))
        ret = TLSX_KeyShare_ProcessDh(ssl, keyShareEntry);
    else if (keyShareEntry->group == WOLFSSL_ECC_X25519)
        ret = TLSX_KeyShare_ProcessX25519(ssl, keyShareEntry);
    else if (keyShareEntry->group == WOLFSSL_ECC_X448)
        ret = TLSX_KeyShare_ProcessX448(ssl, keyShareEntry);
#ifdef HAVE_PQC
    else if (WOLFSSL_NAMED_GROUP_IS_PQC(keyShareEntry->group))
        ret = TLSX_KeyShare_ProcessPqc(ssl, keyShareEntry);
#endif
    else
        ret = TLSX_KeyShare_ProcessEcc(ssl, keyShareEntry);

#ifdef WOLFSSL_DEBUG_TLS
    if (ret == 0) {
        WOLFSSL_MSG("KE Secret");
        WOLFSSL_BUFFER(ssl->arrays->preMasterSecret, ssl->arrays->preMasterSz);
    }
#endif
#ifdef WOLFSSL_ASYNC_CRYPT
    keyShareEntry->lastRet = ret;
#endif

    return ret;
}

/* Parse an entry of the KeyShare extension.
 *
 * ssl     The SSL/TLS object.
 * input   The extension data.
 * length  The length of the extension data.
 * kse     The new key share entry object.
 * returns a positive number to indicate amount of data parsed and a negative
 * number on error.
 */
static int TLSX_KeyShareEntry_Parse(const WOLFSSL* ssl, const byte* input,
            word16 length, KeyShareEntry **kse, TLSX** extensions)
{
    int    ret;
    word16 group;
    word16 keLen;
    int    offset = 0;
    byte*  ke;

    if (length < OPAQUE16_LEN + OPAQUE16_LEN)
        return BUFFER_ERROR;
    /* Named group */
    ato16(&input[offset], &group);
    offset += OPAQUE16_LEN;
    /* Key exchange data - public key. */
    ato16(&input[offset], &keLen);
    offset += OPAQUE16_LEN;
    if (keLen == 0)
        return INVALID_PARAMETER;
    if (keLen > length - offset)
        return BUFFER_ERROR;

#ifdef HAVE_PQC
    if (WOLFSSL_NAMED_GROUP_IS_PQC(group) &&
        ssl->options.side == WOLFSSL_SERVER_END) {
        /* For KEMs, the public key is not stored. Casting away const because
         * we know for KEMs, it will be read-only.*/
        ke = (byte *)&input[offset];
    } else
#endif
    {
        /* Store a copy in the key share object. */
        ke = (byte*)XMALLOC(keLen, ssl->heap, DYNAMIC_TYPE_PUBLIC_KEY);
        if (ke == NULL)
            return MEMORY_E;
        XMEMCPY(ke, &input[offset], keLen);
    }

    /* Populate a key share object in the extension. */
    ret = TLSX_KeyShare_Use(ssl, group, keLen, ke, kse, extensions);
    if (ret != 0) {
        if (ke != &input[offset]) {
            XFREE(ke, ssl->heap, DYNAMIC_TYPE_PUBLIC_KEY);
        }
        return ret;
    }

    /* Total length of the parsed data. */
    return offset + keLen;
}

/* Searches the groups sent for the specified named group.
 *
 * ssl    SSL/TLS object.
 * name   Group name to match.
 * returns 1 when the extension has the group name and 0 otherwise.
 */
static int TLSX_KeyShare_Find(WOLFSSL* ssl, word16 group)
{
    TLSX*          extension;
    KeyShareEntry* list;

    extension = TLSX_Find(ssl->extensions, TLSX_KEY_SHARE);
    if (extension == NULL) {
        extension = TLSX_Find(ssl->ctx->extensions, TLSX_KEY_SHARE);
        if (extension == NULL)
            return 0;
    }

    list = (KeyShareEntry*)extension->data;
    while (list != NULL) {
        if (list->group == group)
            return 1;
        list = list->next;
    }

    return 0;
}


/* Searches the supported groups extension for the specified named group.
 *
 * ssl   The SSL/TLS object.
 * name  The group name to match.
 * returns 1 when the extension has the group name and 0 otherwise.
 */
static int TLSX_SupportedGroups_Find(const WOLFSSL* ssl, word16 name,
                                     TLSX* extensions)
{
#ifdef HAVE_SUPPORTED_CURVES
    TLSX*          extension;
    SupportedCurve* curve = NULL;

    if ((extension = TLSX_Find(extensions,
                                              TLSX_SUPPORTED_GROUPS)) == NULL) {
        if ((extension = TLSX_Find(ssl->ctx->extensions,
                                              TLSX_SUPPORTED_GROUPS)) == NULL) {
            return 0;
        }
    }

    for (curve = (SupportedCurve*)extension->data; curve; curve = curve->next) {
        if (curve->name == name)
            return 1;
    }
#endif

    (void)ssl;
    (void)name;

    return 0;
}

int TLSX_KeyShare_Parse_ClientHello(const WOLFSSL* ssl,
        const byte* input, word16 length, TLSX** extensions)
{
    int ret;
    int    offset = 0;
    word16 len;
    TLSX*  extension;

    /* Add a KeyShare extension if it doesn't exist even if peer sent no
     * entries. The presence of this extension signals that the peer can be
     * negotiated with. */
    extension = TLSX_Find(*extensions, TLSX_KEY_SHARE);
    if (extension == NULL) {
        /* Push new KeyShare extension. */
        ret = TLSX_Push(extensions, TLSX_KEY_SHARE, NULL, ssl->heap);
        if (ret != 0)
            return ret;
    }

    if (length < OPAQUE16_LEN)
        return BUFFER_ERROR;

    /* ClientHello contains zero or more key share entries. */
    ato16(input, &len);
    if (len != length - OPAQUE16_LEN)
        return BUFFER_ERROR;
    offset += OPAQUE16_LEN;

    while (offset < (int)length) {
        ret = TLSX_KeyShareEntry_Parse(ssl, &input[offset],
                length - (word16)offset, NULL, extensions);
        if (ret < 0)
            return ret;

        offset += ret;
    }

    return 0;
}

/* Parse the KeyShare extension.
 * Different formats in different messages.
 *
 * ssl      The SSL/TLS object.
 * input    The extension data.
 * length   The length of the extension data.
 * msgType  The type of the message this extension is being parsed from.
 * returns 0 on success and other values indicate failure.
 */
int TLSX_KeyShare_Parse(WOLFSSL* ssl, const byte* input, word16 length,
                               byte msgType)
{
    int ret;
    KeyShareEntry *keyShareEntry = NULL;
    word16 group;

    if (msgType == client_hello) {
        ret = TLSX_KeyShare_Parse_ClientHello(ssl, input, length,
                                              &ssl->extensions);
    }
    else if (msgType == server_hello) {
        int len;

        if (length < OPAQUE16_LEN)
            return BUFFER_ERROR;

        /* The data is the named group the server wants to use. */
        ato16(input, &group);

        /* Check the selected group was supported by ClientHello extensions. */
        if (!TLSX_SupportedGroups_Find(ssl, group, ssl->extensions)) {
            WOLFSSL_ERROR_VERBOSE(BAD_KEY_SHARE_DATA);
            return BAD_KEY_SHARE_DATA;
        }

        /* Check if the group was sent. */
        if (!TLSX_KeyShare_Find(ssl, group)) {
            WOLFSSL_ERROR_VERBOSE(BAD_KEY_SHARE_DATA);
            return BAD_KEY_SHARE_DATA;
        }

        /* ServerHello contains one key share entry. */
        len = TLSX_KeyShareEntry_Parse(ssl, input, length, &keyShareEntry,
                &ssl->extensions);
        if (len != (int)length)
            return BUFFER_ERROR;

        /* Not in list sent if there isn't a private key. */
        if (keyShareEntry == NULL || (keyShareEntry->key == NULL
        #if !defined(NO_DH) || defined(HAVE_PQC)
            && keyShareEntry->privKey == NULL
        #endif
        )) {
            WOLFSSL_ERROR_VERBOSE(BAD_KEY_SHARE_DATA);
            return BAD_KEY_SHARE_DATA;
        }

        /* Process the entry to calculate the secret. */
        ret = TLSX_KeyShare_Process(ssl, keyShareEntry);
        if (ret == 0)
            ssl->session->namedGroup = ssl->namedGroup = group;
    }
    else if (msgType == hello_retry_request) {
        if (length != OPAQUE16_LEN)
            return BUFFER_ERROR;

        /* The data is the named group the server wants to use. */
        ato16(input, &group);

    #ifdef WOLFSSL_ASYNC_CRYPT
        /* only perform find and clear TLSX if not returning from async */
        if (ssl->error != WC_PENDING_E)
    #endif
        {
            /* Check the selected group was supported by ClientHello extensions. */
            if (!TLSX_SupportedGroups_Find(ssl, group, ssl->extensions)) {
                WOLFSSL_ERROR_VERBOSE(BAD_KEY_SHARE_DATA);
                return BAD_KEY_SHARE_DATA;
            }

            /* Check if the group was sent. */
            if (TLSX_KeyShare_Find(ssl, group)) {
                WOLFSSL_ERROR_VERBOSE(BAD_KEY_SHARE_DATA);
                return BAD_KEY_SHARE_DATA;
            }

            /* Clear out unusable key shares. */
            ret = TLSX_KeyShare_Empty(ssl);
            if (ret != 0)
                return ret;
        }

#ifdef HAVE_PQC
        /* For post-quantum groups, do this in TLSX_PopulateExtensions(). */
        if (!WOLFSSL_NAMED_GROUP_IS_PQC(group))
#endif
            ret = TLSX_KeyShare_Use(ssl, group, 0, NULL, NULL, &ssl->extensions);

        if (ret == 0)
            ssl->session->namedGroup = ssl->namedGroup = group;
    }
    else {
        /* Not a message type that is allowed to have this extension. */
        WOLFSSL_ERROR_VERBOSE(SANITY_MSG_E);
        return SANITY_MSG_E;
    }

    return ret;
}

/* Create a new key share entry and put it into the list.
 *
 * list           The linked list of key share entries.
 * group          The named group.
 * heap           The memory to allocate with.
 * keyShareEntry  The new key share entry object.
 * returns 0 on success and other values indicate failure.
 */
static int TLSX_KeyShare_New(KeyShareEntry** list, int group, void *heap,
                             KeyShareEntry** keyShareEntry)
{
    KeyShareEntry* kse;
    KeyShareEntry** next;

    kse = (KeyShareEntry*)XMALLOC(sizeof(KeyShareEntry), heap,
                                  DYNAMIC_TYPE_TLSX);
    if (kse == NULL)
        return MEMORY_E;

    XMEMSET(kse, 0, sizeof(*kse));
    kse->group = (word16)group;

    /* Add it to the back and maintain the links. */
    while (*list != NULL) {
        /* Assign to temporary to work around compiler bug found by customer. */
        next = &((*list)->next);
        list = next;
    }
    *list = kse;
    *keyShareEntry = kse;

    (void)heap;

    return 0;
}

#ifdef HAVE_PQC
static int server_generate_pqc_ciphertext(WOLFSSL* ssl,
    KeyShareEntry* keyShareEntry, byte* data, word16 len)
{
    /* I am the server. The data parameter is the client's public key. I need
     * to generate the public information (AKA ciphertext) and shared secret
     * here. Note the "public information" is equivalent to a the public key in
     * key exchange parlance. That's why it is being assigned to pubKey.
     */
    int type;
    KyberKey kem[1];
    byte* sharedSecret = NULL;
    byte* ciphertext = NULL;
    int ret = 0;
    int oqs_group = 0;
    int ecc_group = 0;
    KeyShareEntry *ecc_kse = NULL;
    ecc_key eccpubkey;
    word32 outlen = 0;
    word32 pubSz = 0;
    word32 ctSz = 0;
    word32 ssSz = 0;

    findEccPqc(&ecc_group, &oqs_group, keyShareEntry->group);
    ret = kyber_id2type(oqs_group, &type);
    if (ret != 0) {
        WOLFSSL_MSG("Invalid Kyber algorithm specified.");
        ret = BAD_FUNC_ARG;
    }

    if (ret == 0) {
        ret = wc_ecc_init_ex(&eccpubkey, ssl->heap, ssl->devId);
        if (ret != 0) {
            WOLFSSL_MSG("Could not do ECC public key initialization.");
            ret = MEMORY_E;
        }
    }
    if (ret == 0) {
        ecc_kse = (KeyShareEntry*)XMALLOC(sizeof(*ecc_kse), ssl->heap,
            DYNAMIC_TYPE_TLSX);
        if (ecc_kse == NULL) {
            WOLFSSL_MSG("ecc_kse memory allocation failure");
            ret = MEMORY_ERROR;
        }
    }

    if (ret == 0) {
        XMEMSET(ecc_kse, 0, sizeof(*ecc_kse));
    }

    if (ret == 0 && ecc_group != 0) {
        ecc_kse->group = ecc_group;
        ret = TLSX_KeyShare_GenEccKey(ssl, ecc_kse);
        if (ret != 0) {
            /* No message, TLSX_KeyShare_GenEccKey() will do it. */
            return ret;
        }
        ret = 0;
    }

    if (ret == 0) {
        ret = wc_KyberKey_Init(type, kem, ssl->heap, INVALID_DEVID);
        if (ret != 0) {
            WOLFSSL_MSG("Error creating Kyber KEM");
        }
    }
    if (ret == 0) {
        ret = wc_KyberKey_PublicKeySize(kem, &pubSz);
    }
    if (ret == 0) {
        ret = wc_KyberKey_CipherTextSize(kem, &ctSz);
    }
    if (ret == 0) {
        ret = wc_KyberKey_SharedSecretSize(kem, &ssSz);
    }

    if (ret == 0 && len != pubSz + ecc_kse->pubKeyLen) {
        WOLFSSL_MSG("Invalid public key.");
        ret = BAD_FUNC_ARG;
    }

    if (ret == 0) {
        sharedSecret = (byte*)XMALLOC(ecc_kse->keyLen + ssSz, ssl->heap,
            DYNAMIC_TYPE_TLSX);
        ciphertext = (byte*)XMALLOC(ecc_kse->pubKeyLen + ctSz, ssl->heap,
            DYNAMIC_TYPE_TLSX);

        if (sharedSecret == NULL || ciphertext == NULL) {
            WOLFSSL_MSG("Ciphertext/shared secret memory allocation failure.");
            ret = MEMORY_E;
        }
    }

    if (ecc_group != 0) {
        if (ret == 0) {
            /* Point is validated by import function. */
            ret = wc_ecc_import_x963(data, len - pubSz, &eccpubkey);
            if (ret != 0) {
                WOLFSSL_MSG("Bad ECC public key.");
            }
        }

#if defined(ECC_TIMING_RESISTANT) && (!defined(HAVE_FIPS) || \
    (!defined(HAVE_FIPS_VERSION) || (HAVE_FIPS_VERSION != 2))) && \
    !defined(HAVE_SELFTEST)
        if (ret == 0) {
            ret = wc_ecc_set_rng(ecc_kse->key, ssl->rng);
        }
#endif

        if (ret == 0) {
            outlen = ecc_kse->keyLen;
            PRIVATE_KEY_UNLOCK();
            ret = wc_ecc_shared_secret(ecc_kse->key, &eccpubkey,
                                       sharedSecret,
                                       &outlen);
            PRIVATE_KEY_LOCK();
            if (outlen != ecc_kse->keyLen) {
                WOLFSSL_MSG("Data length mismatch.");
                ret = BAD_FUNC_ARG;
            }
        }
    }

    if (ret == 0) {
        ret = wc_KyberKey_DecodePublicKey(kem, data + ecc_kse->pubKeyLen,
            pubSz);
    }
    if (ret == 0) {
        ret = wc_KyberKey_Encapsulate(kem, ciphertext + ecc_kse->pubKeyLen,
            sharedSecret + outlen, ssl->rng);
        if (ret != 0) {
            WOLFSSL_MSG("wc_KyberKey encapsulation failure.");
        }
    }

    if (ret == 0) {
        if (keyShareEntry->ke != NULL) {
            XFREE(keyShareEntry->ke, ssl->heap, DYNAMIC_TYPE_PUBLIC_KEY);
        }

        keyShareEntry->ke = sharedSecret;
        keyShareEntry->keLen = outlen + ssSz;
        sharedSecret = NULL;

        XMEMCPY(ciphertext, ecc_kse->pubKey, ecc_kse->pubKeyLen);
        keyShareEntry->pubKey = ciphertext;
        keyShareEntry->pubKeyLen = (word32)(ecc_kse->pubKeyLen + ctSz);
        ciphertext = NULL;
    }

    TLSX_KeyShare_FreeAll(ecc_kse, ssl->heap);
    if (sharedSecret != NULL)
        XFREE(sharedSecret, ssl->heap, DYNAMIC_TYPE_TLSX);
    if (ciphertext != NULL)
        XFREE(ciphertext, ssl->heap, DYNAMIC_TYPE_TLSX);
    wc_ecc_free(&eccpubkey);
    wc_KyberKey_Free(kem);
    return ret;
}
#endif /* HAVE_PQC */

/* Use the data to create a new key share object in the extensions.
 *
 * ssl    The SSL/TLS object.
 * group  The named group.
 * len    The length of the public key data.
 * data   The public key data.
 * kse    The new key share entry object.
 * returns 0 on success and other values indicate failure.
 */
int TLSX_KeyShare_Use(const WOLFSSL* ssl, word16 group, word16 len, byte* data,
                      KeyShareEntry **kse, TLSX** extensions)
{
    int            ret = 0;
    TLSX*          extension;
    KeyShareEntry* keyShareEntry = NULL;

    /* Find the KeyShare extension if it exists. */
    extension = TLSX_Find(*extensions, TLSX_KEY_SHARE);
    if (extension == NULL) {
        /* Push new KeyShare extension. */
        ret = TLSX_Push(extensions, TLSX_KEY_SHARE, NULL, ssl->heap);
        if (ret != 0)
            return ret;

        extension = TLSX_Find(*extensions, TLSX_KEY_SHARE);
        if (extension == NULL)
            return MEMORY_E;
    }
    extension->resp = 0;

    /* Try to find the key share entry with this group. */
    keyShareEntry = (KeyShareEntry*)extension->data;
    while (keyShareEntry != NULL) {
        if (keyShareEntry->group == group)
            break;
        keyShareEntry = keyShareEntry->next;
    }

    /* Create a new key share entry if not found. */
    if (keyShareEntry == NULL) {
        ret = TLSX_KeyShare_New((KeyShareEntry**)&extension->data, group,
                                ssl->heap, &keyShareEntry);
        if (ret != 0)
            return ret;
    }


#ifdef HAVE_PQC
    if (WOLFSSL_NAMED_GROUP_IS_PQC(group) &&
        ssl->options.side == WOLFSSL_SERVER_END) {
        ret = server_generate_pqc_ciphertext((WOLFSSL*)ssl, keyShareEntry, data,
                                             len);
        if (ret != 0)
            return ret;
    }
    else
#endif
    if (data != NULL) {
        if (keyShareEntry->ke != NULL) {
            XFREE(keyShareEntry->ke, ssl->heap, DYNAMIC_TYPE_PUBLIC_KEY);
        }
        keyShareEntry->ke = data;
        keyShareEntry->keLen = len;
    }
    else {
        /* Generate a key pair. Casting to non-const since changes inside are
         * minimal but would require an extensive redesign to refactor. Also
         * this path shouldn't be taken when parsing a ClientHello in stateless
         * mode. */
        ret = TLSX_KeyShare_GenKey((WOLFSSL*)ssl, keyShareEntry);
        if (ret != 0)
            return ret;
    }

    if (kse != NULL)
        *kse = keyShareEntry;

    return 0;
}

/* Set an empty Key Share extension.
 *
 * ssl  The SSL/TLS object.
 * returns 0 on success and other values indicate failure.
 */
int TLSX_KeyShare_Empty(WOLFSSL* ssl)
{
    int   ret = 0;
    TLSX* extension;

    /* Find the KeyShare extension if it exists. */
    extension = TLSX_Find(ssl->extensions, TLSX_KEY_SHARE);
    if (extension == NULL) {
        /* Push new KeyShare extension. */
        ret = TLSX_Push(&ssl->extensions, TLSX_KEY_SHARE, NULL, ssl->heap);
    }
    else if (extension->data != NULL) {
        TLSX_KeyShare_FreeAll((KeyShareEntry*)extension->data, ssl->heap);
        extension->data = NULL;
    }

    return ret;
}

/* Returns whether this group is supported.
 *
 * namedGroup  The named group to check.
 * returns 1 when supported or 0 otherwise.
 */
static int TLSX_KeyShare_IsSupported(int namedGroup)
{
    switch (namedGroup) {
    #ifdef HAVE_FFDHE_2048
        case WOLFSSL_FFDHE_2048:
            break;
    #endif
    #ifdef HAVE_FFDHE_3072
        case WOLFSSL_FFDHE_3072:
            break;
    #endif
    #ifdef HAVE_FFDHE_4096
        case WOLFSSL_FFDHE_4096:
            break;
    #endif
    #ifdef HAVE_FFDHE_6144
        case WOLFSSL_FFDHE_6144:
            break;
    #endif
    #ifdef HAVE_FFDHE_8192
        case WOLFSSL_FFDHE_8192:
            break;
    #endif
    #if (!defined(NO_ECC256)  || defined(HAVE_ALL_CURVES)) && ECC_MIN_KEY_SZ <= 256
        #ifdef HAVE_ECC_KOBLITZ
        case WOLFSSL_ECC_SECP256K1:
            break;
        #endif
        #ifndef NO_ECC_SECP
        case WOLFSSL_ECC_SECP256R1:
            break;
        #endif /* !NO_ECC_SECP */
        #ifdef HAVE_ECC_BRAINPOOL
        case WOLFSSL_ECC_BRAINPOOLP256R1:
            break;
        #endif
        #ifdef WOLFSSL_SM2
        case WOLFSSL_ECC_SM2P256V1:
            break;
        #endif /* WOLFSSL_SM2 */
    #endif
    #if defined(HAVE_CURVE25519) && ECC_MIN_KEY_SZ <= 256
        case WOLFSSL_ECC_X25519:
            break;
    #endif
    #if defined(HAVE_CURVE448) && ECC_MIN_KEY_SZ <= 448
        case WOLFSSL_ECC_X448:
            break;
    #endif
    #if (defined(HAVE_ECC384) || defined(HAVE_ALL_CURVES)) && ECC_MIN_KEY_SZ <= 384
        #ifndef NO_ECC_SECP
        case WOLFSSL_ECC_SECP384R1:
            break;
        #endif /* !NO_ECC_SECP */
        #ifdef HAVE_ECC_BRAINPOOL
        case WOLFSSL_ECC_BRAINPOOLP384R1:
            break;
        #endif
    #endif
    #if (defined(HAVE_ECC521) || defined(HAVE_ALL_CURVES)) && ECC_MIN_KEY_SZ <= 521
        #ifndef NO_ECC_SECP
        case WOLFSSL_ECC_SECP521R1:
            break;
        #endif /* !NO_ECC_SECP */
    #endif
    #if (defined(HAVE_ECC160) || defined(HAVE_ALL_CURVES)) && ECC_MIN_KEY_SZ <= 160
        #ifdef HAVE_ECC_KOBLITZ
        case WOLFSSL_ECC_SECP160K1:
            break;
        #endif
        #ifndef NO_ECC_SECP
        case WOLFSSL_ECC_SECP160R1:
            break;
        #endif
        #ifdef HAVE_ECC_SECPR2
        case WOLFSSL_ECC_SECP160R2:
            break;
        #endif
    #endif
    #if (defined(HAVE_ECC192) || defined(HAVE_ALL_CURVES)) && ECC_MIN_KEY_SZ <= 192
        #ifdef HAVE_ECC_KOBLITZ
        case WOLFSSL_ECC_SECP192K1:
            break;
        #endif
        #ifndef NO_ECC_SECP
        case WOLFSSL_ECC_SECP192R1:
            break;
        #endif
    #endif
    #if (defined(HAVE_ECC224) || defined(HAVE_ALL_CURVES)) && ECC_MIN_KEY_SZ <= 224
        #ifdef HAVE_ECC_KOBLITZ
        case WOLFSSL_ECC_SECP224K1:
            break;
        #endif
        #ifndef NO_ECC_SECP
        case WOLFSSL_ECC_SECP224R1:
            break;
        #endif
    #endif
    #if (defined(HAVE_ECC512) || defined(HAVE_ALL_CURVES)) && ECC_MIN_KEY_SZ <= 512
        #ifdef HAVE_ECC_BRAINPOOL
        case WOLFSSL_ECC_BRAINPOOLP512R1:
            break;
        #endif
    #endif
    #ifdef HAVE_PQC
    #ifdef WOLFSSL_WC_KYBER
        #ifdef WOLFSSL_KYBER512
            case WOLFSSL_KYBER_LEVEL1:
        #endif
        #ifdef WOLFSSL_KYBER768
            case WOLFSSL_KYBER_LEVEL3:
        #endif
        #ifdef WOLFSSL_KYBER1024
            case WOLFSSL_KYBER_LEVEL5:
        #endif
                break;
    #elif defined(HAVE_LIBOQS)
        case WOLFSSL_KYBER_LEVEL1:
        case WOLFSSL_KYBER_LEVEL3:
        case WOLFSSL_KYBER_LEVEL5:
        case WOLFSSL_P256_KYBER_LEVEL1:
        case WOLFSSL_P384_KYBER_LEVEL3:
        case WOLFSSL_P521_KYBER_LEVEL5:
        {
            int ret;
            int id;
            findEccPqc(NULL, &namedGroup, namedGroup);
            ret = kyber_id2type(namedGroup, &id);
            if (ret == NOT_COMPILED_IN) {
                return 0;
            }

            if (! ext_kyber_enabled(id)) {
                return 0;
            }
            break;
        }
    #elif defined(HAVE_PQM4)
        case WOLFSSL_KYBER_LEVEL1:
            break;
    #endif
    #endif /* HAVE_PQC */
        default:
            return 0;
    }

    return 1;
}


static const word16 preferredGroup[] = {
#if defined(HAVE_ECC) && (!defined(NO_ECC256) || \
    defined(HAVE_ALL_CURVES)) && !defined(NO_ECC_SECP) && ECC_MIN_KEY_SZ <= 256
    WOLFSSL_ECC_SECP256R1,
#if !defined(HAVE_FIPS) && defined(WOLFSSL_SM2)
    WOLFSSL_ECC_SM2P256V1,
#endif
#endif
#if defined(HAVE_CURVE25519) && ECC_MIN_KEY_SZ <= 256
    WOLFSSL_ECC_X25519,
#endif
#if defined(HAVE_CURVE448) && ECC_MIN_KEY_SZ <= 448
    WOLFSSL_ECC_X448,
#endif
#if defined(HAVE_ECC) && (!defined(NO_ECC384) || \
    defined(HAVE_ALL_CURVES)) && !defined(NO_ECC_SECP) && ECC_MIN_KEY_SZ <= 384
    WOLFSSL_ECC_SECP384R1,
#endif
#if defined(HAVE_ECC) && (!defined(NO_ECC521) || \
    defined(HAVE_ALL_CURVES)) && !defined(NO_ECC_SECP) && ECC_MIN_KEY_SZ <= 521
    WOLFSSL_ECC_SECP521R1,
#endif
#if defined(HAVE_FFDHE_2048)
    WOLFSSL_FFDHE_2048,
#endif
#if defined(HAVE_FFDHE_3072)
    WOLFSSL_FFDHE_3072,
#endif
#if defined(HAVE_FFDHE_4096)
    WOLFSSL_FFDHE_4096,
#endif
#if defined(HAVE_FFDHE_6144)
    WOLFSSL_FFDHE_6144,
#endif
#if defined(HAVE_FFDHE_8192)
    WOLFSSL_FFDHE_8192,
#endif
#ifdef WOLFSSL_WC_KYBER
    #ifdef WOLFSSL_KYBER512
    WOLFSSL_KYBER_LEVEL1,
    #endif
    #ifdef WOLFSSL_KYBER768
    WOLFSSL_KYBER_LEVEL3,
    #endif
    #ifdef WOLFSSL_KYBER1024
    WOLFSSL_KYBER_LEVEL5,
    #endif
#elif defined(HAVE_LIBOQS)
    /* These require a runtime call to TLSX_KeyShare_IsSupported to use */
    WOLFSSL_KYBER_LEVEL1,
    WOLFSSL_KYBER_LEVEL3,
    WOLFSSL_KYBER_LEVEL5,
    WOLFSSL_P256_KYBER_LEVEL1,
    WOLFSSL_P384_KYBER_LEVEL3,
    WOLFSSL_P521_KYBER_LEVEL5,
#elif defined(HAVE_PQM4)
    WOLFSSL_KYBER_LEVEL1,
#endif
    WOLFSSL_NAMED_GROUP_INVALID
};

#define PREFERRED_GROUP_SZ \
    ((sizeof(preferredGroup)/sizeof(*preferredGroup)) - 1)
                                            /* -1 for the invalid group */

/* Examines the application specified group ranking and returns the rank of the
 * group.
 * If no group ranking set then all groups are rank 0 (highest).
 *
 * ssl    The SSL/TLS object.
 * group  The group to check ranking for.
 * returns ranking from 0 to MAX_GROUP_COUNT-1 or -1 when group not in list.
 */
static int TLSX_KeyShare_GroupRank(const WOLFSSL* ssl, int group)
{
    byte i;
    const word16* groups;
    byte numGroups;

    if (ssl->numGroups == 0) {
        groups = preferredGroup;
        numGroups = PREFERRED_GROUP_SZ;
    }
    else {
        groups = ssl->group;
        numGroups = ssl->numGroups;
    }

#ifdef HAVE_LIBOQS
      if (!TLSX_KeyShare_IsSupported(group))
          return -1;
#endif

    for (i = 0; i < numGroups; i++)
        if (groups[i] == (word16)group)
            return i;

    return -1;
}

/* Set a key share that is supported by the client into extensions.
 *
 * ssl  The SSL/TLS object.
 * returns BAD_KEY_SHARE_DATA if no supported group has a key share,
 * 0 if a supported group has a key share and other values indicate an error.
 */
int TLSX_KeyShare_SetSupported(const WOLFSSL* ssl, TLSX** extensions)
{
    int             ret;
#ifdef HAVE_SUPPORTED_CURVES
    TLSX*           extension;
    SupportedCurve* curve = NULL;
    SupportedCurve* preferredCurve = NULL;
    KeyShareEntry*  kse = NULL;
    int             preferredRank = WOLFSSL_MAX_GROUP_COUNT;
    int             rank;

    extension = TLSX_Find(*extensions, TLSX_SUPPORTED_GROUPS);
    if (extension != NULL)
        curve = (SupportedCurve*)extension->data;
    /* Use server's preference order. */
    for (; curve != NULL; curve = curve->next) {
        if (!TLSX_KeyShare_IsSupported(curve->name))
            continue;
        if (wolfSSL_curve_is_disabled(ssl, curve->name))
            continue;

        rank = TLSX_KeyShare_GroupRank(ssl, curve->name);
        if (rank == -1)
            continue;
        if (rank < preferredRank) {
            preferredCurve = curve;
            preferredRank = rank;
        }
    }
    curve = preferredCurve;

    if (curve == NULL) {
        WOLFSSL_ERROR_VERBOSE(BAD_KEY_SHARE_DATA);
        return BAD_KEY_SHARE_DATA;
    }

    #ifdef WOLFSSL_ASYNC_CRYPT
    /* Check the old key share data list. */
    extension = TLSX_Find(*extensions, TLSX_KEY_SHARE);
    if (extension != NULL) {
        kse = (KeyShareEntry*)extension->data;
        /* We should not be computing keys if we are only going to advertise
         * our choice here. */
        if (kse != NULL && kse->lastRet == WC_PENDING_E) {
            WOLFSSL_ERROR_VERBOSE(BAD_KEY_SHARE_DATA);
            return BAD_KEY_SHARE_DATA;
        }
    }
    #endif

    /* Push new KeyShare extension. This will also free the old one */
    ret = TLSX_Push(extensions, TLSX_KEY_SHARE, NULL, ssl->heap);
    if (ret != 0)
        return ret;
    /* Extension got pushed to head */
    extension = *extensions;
    /* Push the selected curve */
    ret = TLSX_KeyShare_New((KeyShareEntry**)&extension->data, curve->name,
                            ssl->heap, &kse);
    if (ret != 0)
        return ret;
    /* Set extension to be in response. */
    extension->resp = 1;
#else

    (void)ssl;

    WOLFSSL_ERROR_VERBOSE(NOT_COMPILED_IN);
    ret = NOT_COMPILED_IN;
#endif

    return ret;
}

/* Server side KSE processing */
int TLSX_KeyShare_Choose(const WOLFSSL *ssl, TLSX* extensions,
    byte cipherSuite0, byte cipherSuite, KeyShareEntry** kse, byte* searched)
{
    TLSX*          extension;
    KeyShareEntry* clientKSE = NULL;
    KeyShareEntry* list = NULL;
    KeyShareEntry* preferredKSE = NULL;
    int preferredRank = WOLFSSL_MAX_GROUP_COUNT;
    int rank;

    (void)cipherSuite0;
    (void)cipherSuite;

    if (ssl == NULL || ssl->options.side != WOLFSSL_SERVER_END)
        return BAD_FUNC_ARG;

    *searched = 0;

    /* Find the KeyShare extension if it exists. */
    extension = TLSX_Find(extensions, TLSX_KEY_SHARE);
    if (extension != NULL)
        list = (KeyShareEntry*)extension->data;

    if (extension && extension->resp == 1) {
        /* Outside of the async case this path should not be taken. */
        int ret = INCOMPLETE_DATA;
    #ifdef WOLFSSL_ASYNC_CRYPT
        /* in async case make sure key generation is finalized */
        KeyShareEntry* serverKSE = (KeyShareEntry*)extension->data;
        if (serverKSE && serverKSE->lastRet == WC_PENDING_E) {
            if (ssl->options.serverState == SERVER_HELLO_RETRY_REQUEST_COMPLETE)
                *searched = 1;
            ret = TLSX_KeyShare_GenKey((WOLFSSL*)ssl, serverKSE);
        }
    #endif
        return ret;
    }

    /* Use server's preference order. */
    for (clientKSE = list; clientKSE != NULL; clientKSE = clientKSE->next) {
        if (clientKSE->ke == NULL)
            continue;

#ifdef WOLFSSL_SM2
        if ((cipherSuite0 == CIPHER_BYTE) &&
            ((cipherSuite == TLS_SM4_GCM_SM3) ||
             (cipherSuite == TLS_SM4_CCM_SM3))) {
           if (clientKSE->group != WOLFSSL_ECC_SM2P256V1) {
               continue;
           }
        }
        else if (clientKSE->group == WOLFSSL_ECC_SM2P256V1) {
           continue;
        }
#endif

        /* Check consistency now - extensions in any order. */
        if (!TLSX_SupportedGroups_Find(ssl, clientKSE->group, extensions))
            continue;

        if (!WOLFSSL_NAMED_GROUP_IS_FFHDE(clientKSE->group)) {
            /* Check max value supported. */
            if (clientKSE->group > WOLFSSL_ECC_MAX) {
#ifdef HAVE_PQC
                if (!WOLFSSL_NAMED_GROUP_IS_PQC(clientKSE->group))
#endif
                    continue;
            }
            if (wolfSSL_curve_is_disabled(ssl, clientKSE->group))
                continue;
        }
        if (!TLSX_KeyShare_IsSupported(clientKSE->group))
            continue;

        rank = TLSX_KeyShare_GroupRank(ssl, clientKSE->group);
        if (rank == -1)
            continue;
        if (rank < preferredRank) {
            preferredKSE = clientKSE;
            preferredRank = rank;
        }
    }
    *kse = preferredKSE;
    *searched = 1;
    return 0;
}

/* Server side KSE processing */
int TLSX_KeyShare_Setup(WOLFSSL *ssl, KeyShareEntry* clientKSE)
{
    int            ret;
    TLSX*          extension;
    KeyShareEntry* serverKSE;
    KeyShareEntry* list = NULL;

    if (ssl == NULL || ssl->options.side != WOLFSSL_SERVER_END)
        return BAD_FUNC_ARG;

    extension = TLSX_Find(ssl->extensions, TLSX_KEY_SHARE);
    if (extension == NULL)
        return BAD_STATE_E;

    if (clientKSE == NULL) {
#ifdef WOLFSSL_ASYNC_CRYPT
        /* Not necessarily an error. The key may have already been setup. */
        if (extension != NULL && extension->resp == 1) {
            serverKSE = (KeyShareEntry*)extension->data;
            if (serverKSE != NULL) {
                /* in async case make sure key generation is finalized */
                if (serverKSE->lastRet == WC_PENDING_E)
                    return TLSX_KeyShare_GenKey((WOLFSSL*)ssl, serverKSE);
                else if (serverKSE->lastRet == 0)
                    return 0;
            }
        }
#endif
        return BAD_FUNC_ARG;
    }

    /* Generate a new key pair except in the case of OQS KEM because we
     * are going to encapsulate and that does not require us to generate a
     * key pair.
     */
    ret = TLSX_KeyShare_New(&list, clientKSE->group, ssl->heap, &serverKSE);
    if (ret != 0)
        return ret;

    if (clientKSE->key == NULL) {
#ifdef HAVE_PQC
        if (WOLFSSL_NAMED_GROUP_IS_PQC(clientKSE->group)) {
            /* Going to need the public key (AKA ciphertext). */
            serverKSE->pubKey = clientKSE->pubKey;
            clientKSE->pubKey = NULL;
            serverKSE->pubKeyLen = clientKSE->pubKeyLen;
            clientKSE->pubKeyLen = 0;
        }
        else
#endif
        {
            ret = TLSX_KeyShare_GenKey(ssl, serverKSE);
        }

        /* for async do setup of serverKSE below, but return WC_PENDING_E */
        if (ret != 0
        #ifdef WOLFSSL_ASYNC_CRYPT
            && ret != WC_PENDING_E
        #endif
        ) {
            TLSX_KeyShare_FreeAll(list, ssl->heap);
            return ret;
        }
    }
    else {
        /* transfer buffers to serverKSE */
        serverKSE->key = clientKSE->key;
        clientKSE->key = NULL;
        serverKSE->keyLen = clientKSE->keyLen;
        serverKSE->pubKey = clientKSE->pubKey;
        clientKSE->pubKey = NULL;
        serverKSE->pubKeyLen = clientKSE->pubKeyLen;
    #ifndef NO_DH
        serverKSE->privKey = clientKSE->privKey;
        clientKSE->privKey = NULL;
    #endif
    }
    serverKSE->ke = clientKSE->ke;
    serverKSE->keLen = clientKSE->keLen;
    clientKSE->ke = NULL;
    clientKSE->keLen = 0;

    TLSX_KeyShare_FreeAll((KeyShareEntry*)extension->data, ssl->heap);
    extension->data = (void *)serverKSE;

    extension->resp = 1;
    return ret;
}

/* Ensure there is a key pair that can be used for key exchange.
 *
 * ssl  The SSL/TLS object.
 * doHelloRetry If set to non-zero will do hello_retry
 * returns 0 on success and other values indicate failure.
 */
int TLSX_KeyShare_Establish(WOLFSSL *ssl, int* doHelloRetry)
{
    int            ret;
    KeyShareEntry* clientKSE = NULL;
    byte           searched = 0;

    *doHelloRetry = 0;

    ret = TLSX_KeyShare_Choose(ssl, ssl->extensions, ssl->cipher.cipherSuite0,
        ssl->cipher.cipherSuite, &clientKSE, &searched);
    if (ret != 0 || !searched)
        return ret;

    /* No supported group found - send HelloRetryRequest. */
    if (clientKSE == NULL) {
        /* Set KEY_SHARE_ERROR to indicate HelloRetryRequest required. */
        *doHelloRetry = 1;
        return TLSX_KeyShare_SetSupported(ssl, &ssl->extensions);
    }

    return TLSX_KeyShare_Setup(ssl, clientKSE);
}

/* Derive the shared secret of the key exchange.
 *
 * ssl  The SSL/TLS object.
 * returns 0 on success and other values indicate failure.
 */
int TLSX_KeyShare_DeriveSecret(WOLFSSL *ssl)
{
    int            ret;
    TLSX*          extension;
    KeyShareEntry* list = NULL;

#ifdef WOLFSSL_ASYNC_CRYPT
    ret = wolfSSL_AsyncPop(ssl, NULL);
    /* Check for error */
    if (ret != WC_NOT_PENDING_E && ret < 0) {
        return ret;
    }
#endif

    /* Find the KeyShare extension if it exists. */
    extension = TLSX_Find(ssl->extensions, TLSX_KEY_SHARE);
    if (extension != NULL)
        list = (KeyShareEntry*)extension->data;

    if (list == NULL)
        return KEY_SHARE_ERROR;

    /* Calculate secret. */
    ret = TLSX_KeyShare_Process(ssl, list);

    return ret;
}

#define KS_FREE_ALL  TLSX_KeyShare_FreeAll
#define KS_GET_SIZE  TLSX_KeyShare_GetSize
#define KS_WRITE     TLSX_KeyShare_Write
#define KS_PARSE     TLSX_KeyShare_Parse

#else

#define KS_FREE_ALL(a, b) WC_DO_NOTHING
#define KS_GET_SIZE(a, b)    0
#define KS_WRITE(a, b, c)    0
#define KS_PARSE(a, b, c, d) 0

#endif /* WOLFSSL_TLS13 */

/******************************************************************************/
/* Pre-Shared Key                                                             */
/******************************************************************************/

#if defined(WOLFSSL_TLS13) && (defined(HAVE_SESSION_TICKET) || !defined(NO_PSK))
/* Free the pre-shared key dynamic data.
 *
 * list  The linked list of key share entry objects.
 * heap  The heap used for allocation.
 */
static void TLSX_PreSharedKey_FreeAll(PreSharedKey* list, void* heap)
{
    PreSharedKey* current;

    while ((current = list) != NULL) {
        list = current->next;
        XFREE(current->identity, heap, DYNAMIC_TYPE_TLSX);
        XFREE(current, heap, DYNAMIC_TYPE_TLSX);
    }

    (void)heap;
}

/* Get the size of the encoded pre shared key extension.
 *
 * list     The linked list of pre-shared key extensions.
 * msgType  The type of the message this extension is being written into.
 * returns the number of bytes of the encoded pre-shared key extension or
 * SANITY_MSG_E to indicate invalid message type.
 */
static int TLSX_PreSharedKey_GetSize(PreSharedKey* list, byte msgType,
                                     word16* pSz)
{
    if (msgType == client_hello) {
        /* Length of identities + Length of binders. */
        word16 len = OPAQUE16_LEN + OPAQUE16_LEN;
        while (list != NULL) {
            /* Each entry has: identity, ticket age and binder. */
            len += OPAQUE16_LEN + list->identityLen + OPAQUE32_LEN +
                   OPAQUE8_LEN + (word16)list->binderLen;
            list = list->next;
        }
        *pSz += len;
        return 0;
    }

    if (msgType == server_hello) {
        *pSz += OPAQUE16_LEN;
        return 0;
    }

    WOLFSSL_ERROR_VERBOSE(SANITY_MSG_E);
    return SANITY_MSG_E;
}

/* The number of bytes to be written for the binders.
 *
 * list     The linked list of pre-shared key extensions.
 * msgType  The type of the message this extension is being written into.
 * returns the number of bytes of the encoded pre-shared key extension or
 * SANITY_MSG_E to indicate invalid message type.
 */
int TLSX_PreSharedKey_GetSizeBinders(PreSharedKey* list, byte msgType,
                                     word16* pSz)
{
    word16 len;

    if (msgType != client_hello) {
        WOLFSSL_ERROR_VERBOSE(SANITY_MSG_E);
        return SANITY_MSG_E;
    }

    /* Length of all binders. */
    len = OPAQUE16_LEN;
    while (list != NULL) {
        len += OPAQUE8_LEN + (word16)list->binderLen;
        list = list->next;
    }

    *pSz = len;
    return 0;
}

/* Writes the pre-shared key extension into the output buffer - binders only.
 * Assumes that the the output buffer is big enough to hold data.
 *
 * list     The linked list of key share entries.
 * output   The buffer to write into.
 * msgType  The type of the message this extension is being written into.
 * returns the number of bytes written into the buffer.
 */
int TLSX_PreSharedKey_WriteBinders(PreSharedKey* list, byte* output,
                                   byte msgType, word16* pSz)
{
    PreSharedKey* current = list;
    word16 idx = 0;
    word16 lenIdx;
    word16 len;

    if (msgType != client_hello) {
        WOLFSSL_ERROR_VERBOSE(SANITY_MSG_E);
        return SANITY_MSG_E;
    }

    /* Skip length of all binders. */
    lenIdx = idx;
    idx += OPAQUE16_LEN;
    while (current != NULL) {
        /* Binder data length. */
        output[idx++] = (byte)current->binderLen;
        /* Binder data. */
        XMEMCPY(output + idx, current->binder, current->binderLen);
        idx += (word16)current->binderLen;

        current = current->next;
    }
    /* Length of the binders. */
    len = idx - lenIdx - OPAQUE16_LEN;
    c16toa(len, output + lenIdx);

    *pSz = idx;
    return 0;
}


/* Writes the pre-shared key extension into the output buffer.
 * Assumes that the the output buffer is big enough to hold data.
 *
 * list     The linked list of key share entries.
 * output   The buffer to write into.
 * msgType  The type of the message this extension is being written into.
 * returns the number of bytes written into the buffer.
 */
static int TLSX_PreSharedKey_Write(PreSharedKey* list, byte* output,
                                   byte msgType, word16* pSz)
{
    if (msgType == client_hello) {
        PreSharedKey* current = list;
        word16 idx = 0;
        word16 lenIdx;
        word16 len;
        int ret;

        /* Write identities only. Binders after HMACing over this. */
        lenIdx = idx;
        idx += OPAQUE16_LEN;
        while (current != NULL) {
            /* Identity length */
            c16toa(current->identityLen, output + idx);
            idx += OPAQUE16_LEN;
            /* Identity data */
            XMEMCPY(output + idx, current->identity, current->identityLen);
            idx += current->identityLen;

            /* Obfuscated ticket age. */
            c32toa(current->ticketAge, output + idx);
            idx += OPAQUE32_LEN;

            current = current->next;
        }
        /* Length of the identities. */
        len = idx - lenIdx - OPAQUE16_LEN;
        c16toa(len, output + lenIdx);

        /* Don't include binders here.
         * The binders are based on the hash of all the ClientHello data up to
         * and include the identities written above.
         */
        ret = TLSX_PreSharedKey_GetSizeBinders(list, msgType, &len);
        if (ret < 0)
            return ret;
        *pSz += idx + len;
    }
    else if (msgType == server_hello) {
        word16 i;

        /* Find the index of the chosen identity. */
        for (i=0; list != NULL && !list->chosen; i++)
            list = list->next;
        if (list == NULL) {
            WOLFSSL_ERROR_VERBOSE(BUILD_MSG_ERROR);
            return BUILD_MSG_ERROR;
        }

        /* The index of the identity chosen by the server from the list supplied
         * by the client.
         */
        c16toa(i, output);
        *pSz += OPAQUE16_LEN;
    }
    else {
        WOLFSSL_ERROR_VERBOSE(SANITY_MSG_E);
        return SANITY_MSG_E;
    }

    return 0;
}

int TLSX_PreSharedKey_Parse_ClientHello(TLSX** extensions, const byte* input,
                                        word16 length, void* heap)
{

    int    ret;
    word16 len;
    word16 idx = 0;
    TLSX*         extension;
    PreSharedKey* list;

    TLSX_Remove(extensions, TLSX_PRE_SHARED_KEY, heap);

    /* Length of identities and of binders. */
    if ((int)(length - idx) < OPAQUE16_LEN + OPAQUE16_LEN)
        return BUFFER_E;

    /* Length of identities. */
    ato16(input + idx, &len);
    idx += OPAQUE16_LEN;
    if (len < MIN_PSK_ID_LEN || length - idx < len)
        return BUFFER_E;

    /* Create a pre-shared key object for each identity. */
    while (len > 0) {
        const byte* identity;
        word16      identityLen;
        word32      age;

        if (len < OPAQUE16_LEN)
            return BUFFER_E;

        /* Length of identity. */
        ato16(input + idx, &identityLen);
        idx += OPAQUE16_LEN;
        if (len < OPAQUE16_LEN + identityLen + OPAQUE32_LEN ||
                identityLen > MAX_PSK_ID_LEN)
            return BUFFER_E;
        /* Cache identity pointer. */
        identity = input + idx;
        idx += identityLen;
        /* Ticket age. */
        ato32(input + idx, &age);
        idx += OPAQUE32_LEN;

        ret = TLSX_PreSharedKey_Use(extensions, identity, identityLen, age, no_mac,
                                    0, 0, 1, NULL, heap);
        if (ret != 0)
            return ret;

        /* Done with this identity. */
        len -= OPAQUE16_LEN + identityLen + OPAQUE32_LEN;
    }

    /* Find the list of identities sent to server. */
    extension = TLSX_Find(*extensions, TLSX_PRE_SHARED_KEY);
    if (extension == NULL)
        return PSK_KEY_ERROR;
    list = (PreSharedKey*)extension->data;

    /* Length of binders. */
    if (idx + OPAQUE16_LEN > length)
        return BUFFER_E;
    ato16(input + idx, &len);
    idx += OPAQUE16_LEN;
    if (len < MIN_PSK_BINDERS_LEN || length - idx < len)
        return BUFFER_E;

    /* Set binder for each identity. */
    while (list != NULL && len > 0) {
        /* Length of binder */
        list->binderLen = input[idx++];
        if (list->binderLen < WC_SHA256_DIGEST_SIZE ||
                list->binderLen > WC_MAX_DIGEST_SIZE)
            return BUFFER_E;
        if (len < OPAQUE8_LEN + list->binderLen)
            return BUFFER_E;

        /* Copy binder into static buffer. */
        XMEMCPY(list->binder, input + idx, list->binderLen);
        idx += (word16)list->binderLen;

        /* Done with binder entry. */
        len -= OPAQUE8_LEN + (word16)list->binderLen;

        /* Next identity. */
        list = list->next;
    }
    if (list != NULL || len != 0)
        return BUFFER_E;

    return 0;

}

/* Parse the pre-shared key extension.
 * Different formats in different messages.
 *
 * ssl      The SSL/TLS object.
 * input    The extension data.
 * length   The length of the extension data.
 * msgType  The type of the message this extension is being parsed from.
 * returns 0 on success and other values indicate failure.
 */
static int TLSX_PreSharedKey_Parse(WOLFSSL* ssl, const byte* input,
                                   word16 length, byte msgType)
{

    if (msgType == client_hello) {
        return TLSX_PreSharedKey_Parse_ClientHello(&ssl->extensions, input,
                                                   length, ssl->heap);
    }

    if (msgType == server_hello) {
        word16 idx;
        PreSharedKey* list;
        TLSX*         extension;

        /* Index of identity chosen by server. */
        if (length != OPAQUE16_LEN)
            return BUFFER_E;
        ato16(input, &idx);

    #ifdef WOLFSSL_EARLY_DATA
        ssl->options.pskIdIndex = idx + 1;
    #endif

        /* Find the list of identities sent to server. */
        extension = TLSX_Find(ssl->extensions, TLSX_PRE_SHARED_KEY);
        if (extension == NULL)
            return PSK_KEY_ERROR;
        list = (PreSharedKey*)extension->data;

        /* Mark the identity as chosen. */
        for (; list != NULL && idx > 0; idx--)
            list = list->next;
        if (list == NULL) {
            WOLFSSL_ERROR_VERBOSE(PSK_KEY_ERROR);
            return PSK_KEY_ERROR;
        }
        list->chosen = 1;

    #ifdef HAVE_SESSION_TICKET
        if (list->resumption) {
           /* Check that the session's details are the same as the server's. */
           if (ssl->options.cipherSuite0  != ssl->session->cipherSuite0       ||
               ssl->options.cipherSuite   != ssl->session->cipherSuite        ||
               ssl->session->version.major != ssl->ctx->method->version.major ||
               ssl->session->version.minor != ssl->ctx->method->version.minor) {
                WOLFSSL_ERROR_VERBOSE(PSK_KEY_ERROR);
               return PSK_KEY_ERROR;
           }
        }
    #endif

        return 0;
    }

    WOLFSSL_ERROR_VERBOSE(SANITY_MSG_E);
    return SANITY_MSG_E;
}

/* Create a new pre-shared key and put it into the list.
 *
 * list          The linked list of pre-shared key.
 * identity      The identity.
 * len           The length of the identity data.
 * heap          The memory to allocate with.
 * preSharedKey  The new pre-shared key object.
 * returns 0 on success and other values indicate failure.
 */
static int TLSX_PreSharedKey_New(PreSharedKey** list, const byte* identity,
                                 word16 len, void *heap,
                                 PreSharedKey** preSharedKey)
{
    PreSharedKey* psk;
    PreSharedKey** next;

    psk = (PreSharedKey*)XMALLOC(sizeof(PreSharedKey), heap, DYNAMIC_TYPE_TLSX);
    if (psk == NULL)
        return MEMORY_E;
    XMEMSET(psk, 0, sizeof(*psk));

    /* Make a copy of the identity data. */
    psk->identity = (byte*)XMALLOC(len + NULL_TERM_LEN, heap,
                                   DYNAMIC_TYPE_TLSX);
    if (psk->identity == NULL) {
        XFREE(psk, heap, DYNAMIC_TYPE_TLSX);
        return MEMORY_E;
    }
    XMEMCPY(psk->identity, identity, len);
    psk->identityLen = len;
    /* Use a NULL terminator in case it is a C string */
    psk->identity[psk->identityLen] = '\0';

    /* Add it to the end and maintain the links. */
    while (*list != NULL) {
        /* Assign to temporary to work around compiler bug found by customer. */
        next = &((*list)->next);
        list = next;
    }
    *list = psk;
    *preSharedKey = psk;

    (void)heap;

    return 0;
}

static WC_INLINE byte GetHmacLength(int hmac)
{
    switch (hmac) {
    #ifndef NO_SHA256
        case sha256_mac:
            return WC_SHA256_DIGEST_SIZE;
    #endif
    #ifdef WOLFSSL_SHA384
        case sha384_mac:
            return WC_SHA384_DIGEST_SIZE;
    #endif
    #ifdef WOLFSSL_SHA512
        case sha512_mac:
            return WC_SHA512_DIGEST_SIZE;
    #endif
    #ifdef WOLFSSL_SM3
        case sm3_mac:
            return WC_SM3_DIGEST_SIZE;
    #endif
    }
    return 0;
}

/* Use the data to create a new pre-shared key object in the extensions.
 *
 * ssl           The SSL/TLS object.
 * identity      The identity.
 * len           The length of the identity data.
 * age           The age of the identity.
 * hmac          The HMAC algorithm.
 * cipherSuite0  The first byte of the cipher suite to use.
 * cipherSuite   The second byte of the cipher suite to use.
 * resumption    The PSK is for resumption of a session.
 * preSharedKey  The new pre-shared key object.
 * returns 0 on success and other values indicate failure.
 */
int TLSX_PreSharedKey_Use(TLSX** extensions, const byte* identity, word16 len,
                          word32 age, byte hmac, byte cipherSuite0,
                          byte cipherSuite, byte resumption,
                          PreSharedKey **preSharedKey, void* heap)
{
    int           ret = 0;
    TLSX*         extension;
    PreSharedKey* psk = NULL;

    /* Find the pre-shared key extension if it exists. */
    extension = TLSX_Find(*extensions, TLSX_PRE_SHARED_KEY);
    if (extension == NULL) {
        /* Push new pre-shared key extension. */
        ret = TLSX_Push(extensions, TLSX_PRE_SHARED_KEY, NULL, heap);
        if (ret != 0)
            return ret;

        extension = TLSX_Find(*extensions, TLSX_PRE_SHARED_KEY);
        if (extension == NULL)
            return MEMORY_E;
    }

    /* Try to find the pre-shared key with this identity. */
    psk = (PreSharedKey*)extension->data;
    while (psk != NULL) {
        if ((psk->identityLen == len) &&
               (XMEMCMP(psk->identity, identity, len) == 0)) {
            break;
        }
        psk = psk->next;
    }

    /* Create a new pre-shared key object if not found. */
    if (psk == NULL) {
        ret = TLSX_PreSharedKey_New((PreSharedKey**)&extension->data, identity,
                                    len, heap, &psk);
        if (ret != 0)
            return ret;
    }

    /* Update/set age and HMAC algorithm. */
    psk->ticketAge    = age;
    psk->hmac         = hmac;
    psk->cipherSuite0 = cipherSuite0;
    psk->cipherSuite  = cipherSuite;
    psk->resumption   = resumption;
    psk->binderLen    = GetHmacLength(psk->hmac);

    if (preSharedKey != NULL)
        *preSharedKey = psk;

    return 0;
}

#define PSK_FREE_ALL  TLSX_PreSharedKey_FreeAll
#define PSK_GET_SIZE  TLSX_PreSharedKey_GetSize
#define PSK_WRITE     TLSX_PreSharedKey_Write
#define PSK_PARSE     TLSX_PreSharedKey_Parse

#else

#define PSK_FREE_ALL(a, b) WC_DO_NOTHING
#define PSK_GET_SIZE(a, b, c) 0
#define PSK_WRITE(a, b, c, d) 0
#define PSK_PARSE(a, b, c, d) 0

#endif

/******************************************************************************/
/* PSK Key Exchange Modes                                                     */
/******************************************************************************/

#if defined(WOLFSSL_TLS13) && (defined(HAVE_SESSION_TICKET) || !defined(NO_PSK))
/* Get the size of the encoded PSK KE modes extension.
 * Only in ClientHello.
 *
 * modes    The PSK KE mode bit string.
 * msgType  The type of the message this extension is being written into.
 * returns the number of bytes of the encoded PSK KE mode extension.
 */
static int TLSX_PskKeModes_GetSize(byte modes, byte msgType, word16* pSz)
{
    if (msgType == client_hello) {
        /* Format: Len | Modes* */
        word16 len = OPAQUE8_LEN;
        /* Check whether each possible mode is to be written. */
        if (modes & (1 << PSK_KE))
            len += OPAQUE8_LEN;
        if (modes & (1 << PSK_DHE_KE))
            len += OPAQUE8_LEN;
        *pSz += len;
        return 0;
    }

    WOLFSSL_ERROR_VERBOSE(SANITY_MSG_E);
    return SANITY_MSG_E;
}

/* Writes the PSK KE modes extension into the output buffer.
 * Assumes that the the output buffer is big enough to hold data.
 * Only in ClientHello.
 *
 * modes    The PSK KE mode bit string.
 * output   The buffer to write into.
 * msgType  The type of the message this extension is being written into.
 * returns the number of bytes written into the buffer.
 */
static int TLSX_PskKeModes_Write(byte modes, byte* output, byte msgType,
                                 word16* pSz)
{
    if (msgType == client_hello) {
        /* Format: Len | Modes* */
        word16 idx = OPAQUE8_LEN;

        /* Write out each possible mode. */
        if (modes & (1 << PSK_KE))
            output[idx++] = PSK_KE;
        if (modes & (1 << PSK_DHE_KE))
            output[idx++] = PSK_DHE_KE;
        /* Write out length of mode list. */
        output[0] = (byte)(idx - OPAQUE8_LEN);

        *pSz += idx;
        return 0;
    }

    WOLFSSL_ERROR_VERBOSE(SANITY_MSG_E);
    return SANITY_MSG_E;
}

int TLSX_PskKeyModes_Parse_Modes(const byte* input, word16 length, byte msgType,
                                byte* modes)
{
    if (msgType == client_hello) {
        /* Format: Len | Modes* */
        int   idx = 0;
        word16 len;
        *modes = 0;

        /* Ensure length byte exists. */
        if (length < OPAQUE8_LEN)
            return BUFFER_E;

        /* Get length of mode list and ensure that is the only data. */
        len = input[0];
        if (length - OPAQUE8_LEN != len)
            return BUFFER_E;

        idx = OPAQUE8_LEN;
        /* Set a bit for each recognized modes. */
        while (len > 0) {
            /* Ignore unrecognized modes.  */
            if (input[idx] <= PSK_DHE_KE)
               *modes |= 1 << input[idx];
            idx++;
            len--;
        }
        return 0;
    }

    WOLFSSL_ERROR_VERBOSE(SANITY_MSG_E);
    return SANITY_MSG_E;
}

/* Parse the PSK KE modes extension.
 * Only in ClientHello.
 *
 * ssl      The SSL/TLS object.
 * input    The extension data.
 * length   The length of the extension data.
 * msgType  The type of the message this extension is being parsed from.
 * returns 0 on success and other values indicate failure.
 */
static int TLSX_PskKeModes_Parse(WOLFSSL* ssl, const byte* input, word16 length,
                                 byte msgType)
{
    int    ret;
    byte modes;

    ret = TLSX_PskKeyModes_Parse_Modes(input, length, msgType, &modes);
    if (ret == 0)
        ret = TLSX_PskKeyModes_Use(ssl, modes);

    if (ret != 0) {
        WOLFSSL_ERROR_VERBOSE(ret);
    }

    return ret;
}

/* Use the data to create a new PSK Key Exchange Modes object in the extensions.
 *
 * ssl    The SSL/TLS object.
 * modes  The PSK key exchange modes.
 * returns 0 on success and other values indicate failure.
 */
int TLSX_PskKeyModes_Use(WOLFSSL* ssl, byte modes)
{
    int           ret = 0;
    TLSX*         extension;

    /* Find the PSK key exchange modes extension if it exists. */
    extension = TLSX_Find(ssl->extensions, TLSX_PSK_KEY_EXCHANGE_MODES);
    if (extension == NULL) {
        /* Push new PSK key exchange modes extension. */
        ret = TLSX_Push(&ssl->extensions, TLSX_PSK_KEY_EXCHANGE_MODES, NULL,
            ssl->heap);
        if (ret != 0)
            return ret;

        extension = TLSX_Find(ssl->extensions, TLSX_PSK_KEY_EXCHANGE_MODES);
        if (extension == NULL)
            return MEMORY_E;
    }

    extension->val = modes;

    return 0;
}

#define PKM_GET_SIZE  TLSX_PskKeModes_GetSize
#define PKM_WRITE     TLSX_PskKeModes_Write
#define PKM_PARSE     TLSX_PskKeModes_Parse

#else

#define PKM_GET_SIZE(a, b, c) 0
#define PKM_WRITE(a, b, c, d) 0
#define PKM_PARSE(a, b, c, d) 0

#endif

/******************************************************************************/
/* Post-Handshake Authentication                                              */
/******************************************************************************/

#if defined(WOLFSSL_TLS13) && defined(WOLFSSL_POST_HANDSHAKE_AUTH)
/* Get the size of the encoded Post-Handshake Authentication extension.
 * Only in ClientHello.
 *
 * msgType  The type of the message this extension is being written into.
 * returns the number of bytes of the encoded Post-Handshake Authentication
 * extension.
 */
static int TLSX_PostHandAuth_GetSize(byte msgType, word16* pSz)
{
    if (msgType == client_hello) {
        *pSz += 0;
        return 0;
    }

    WOLFSSL_ERROR_VERBOSE(SANITY_MSG_E);
    return SANITY_MSG_E;
}

/* Writes the Post-Handshake Authentication extension into the output buffer.
 * Assumes that the the output buffer is big enough to hold data.
 * Only in ClientHello.
 *
 * output   The buffer to write into.
 * msgType  The type of the message this extension is being written into.
 * returns the number of bytes written into the buffer.
 */
static int TLSX_PostHandAuth_Write(byte* output, byte msgType, word16* pSz)
{
    (void)output;

    if (msgType == client_hello) {
        *pSz += 0;
        return 0;
    }

    WOLFSSL_ERROR_VERBOSE(SANITY_MSG_E);
    return SANITY_MSG_E;
}

/* Parse the Post-Handshake Authentication extension.
 * Only in ClientHello.
 *
 * ssl      The SSL/TLS object.
 * input    The extension data.
 * length   The length of the extension data.
 * msgType  The type of the message this extension is being parsed from.
 * returns 0 on success and other values indicate failure.
 */
static int TLSX_PostHandAuth_Parse(WOLFSSL* ssl, const byte* input,
                                   word16 length, byte msgType)
{
    (void)input;

    if (msgType == client_hello) {
        /* Ensure extension is empty. */
        if (length != 0)
            return BUFFER_E;

        ssl->options.postHandshakeAuth = 1;
        return 0;
    }

    WOLFSSL_ERROR_VERBOSE(SANITY_MSG_E);
    return SANITY_MSG_E;
}

/* Create a new Post-handshake authentication object in the extensions.
 *
 * ssl    The SSL/TLS object.
 * returns 0 on success and other values indicate failure.
 */
static int TLSX_PostHandAuth_Use(WOLFSSL* ssl)
{
    int   ret = 0;
    TLSX* extension;

    /* Find the PSK key exchange modes extension if it exists. */
    extension = TLSX_Find(ssl->extensions, TLSX_POST_HANDSHAKE_AUTH);
    if (extension == NULL) {
        /* Push new Post-handshake Authentication extension. */
        ret = TLSX_Push(&ssl->extensions, TLSX_POST_HANDSHAKE_AUTH, NULL,
            ssl->heap);
        if (ret != 0)
            return ret;
    }

    return 0;
}

#define PHA_GET_SIZE  TLSX_PostHandAuth_GetSize
#define PHA_WRITE     TLSX_PostHandAuth_Write
#define PHA_PARSE     TLSX_PostHandAuth_Parse

#else

#define PHA_GET_SIZE(a, b)    0
#define PHA_WRITE(a, b, c)    0
#define PHA_PARSE(a, b, c, d) 0

#endif

/******************************************************************************/
/* Early Data Indication                                                      */
/******************************************************************************/

#ifdef WOLFSSL_EARLY_DATA
/* Get the size of the encoded Early Data Indication extension.
 * In messages: ClientHello, EncryptedExtensions and NewSessionTicket.
 *
 * msgType  The type of the message this extension is being written into.
 * returns the number of bytes of the encoded Early Data Indication extension.
 */
static int TLSX_EarlyData_GetSize(byte msgType, word16* pSz)
{
    int ret = 0;

    if (msgType == client_hello || msgType == encrypted_extensions)
        *pSz += 0;
    else if (msgType == session_ticket)
        *pSz += OPAQUE32_LEN;
    else {
        ret = SANITY_MSG_E;
        WOLFSSL_ERROR_VERBOSE(ret);
    }

    return ret;
}

/* Writes the Early Data Indicator extension into the output buffer.
 * Assumes that the the output buffer is big enough to hold data.
 * In messages: ClientHello, EncryptedExtensions and NewSessionTicket.
 *
 * maxSz    The maximum early data size.
 * output   The buffer to write into.
 * msgType  The type of the message this extension is being written into.
 * returns the number of bytes written into the buffer.
 */
static int TLSX_EarlyData_Write(word32 maxSz, byte* output, byte msgType,
                                word16* pSz)
{
    if (msgType == client_hello || msgType == encrypted_extensions)
        return 0;
    else if (msgType == session_ticket) {
        c32toa(maxSz, output);
        *pSz += OPAQUE32_LEN;
        return 0;
    }

    WOLFSSL_ERROR_VERBOSE(SANITY_MSG_E);
    return SANITY_MSG_E;
}

/* Parse the Early Data Indicator extension.
 * In messages: ClientHello, EncryptedExtensions and NewSessionTicket.
 *
 * ssl      The SSL/TLS object.
 * input    The extension data.
 * length   The length of the extension data.
 * msgType  The type of the message this extension is being parsed from.
 * returns 0 on success and other values indicate failure.
 */
static int TLSX_EarlyData_Parse(WOLFSSL* ssl, const byte* input, word16 length,
                                 byte msgType)
{
    WOLFSSL_ENTER("TLSX_EarlyData_Parse");
    if (msgType == client_hello) {
        if (length != 0)
            return BUFFER_E;

        if (ssl->earlyData == expecting_early_data) {

            if (ssl->options.maxEarlyDataSz != 0)
                ssl->earlyDataStatus = WOLFSSL_EARLY_DATA_ACCEPTED;
            else
                ssl->earlyDataStatus = WOLFSSL_EARLY_DATA_REJECTED;

            return TLSX_EarlyData_Use(ssl, 0, 0);
        }
        ssl->earlyData = early_data_ext;

        return 0;
    }
    if (msgType == encrypted_extensions) {
        if (length != 0)
            return BUFFER_E;

        /* Ensure the index of PSK identity chosen by server is 0.
         * Index is plus one to handle 'not set' value of 0.
         */
        if (ssl->options.pskIdIndex != 1) {
            WOLFSSL_ERROR_VERBOSE(PSK_KEY_ERROR);
            return PSK_KEY_ERROR;
        }

        if (ssl->options.side == WOLFSSL_CLIENT_END) {
            /* the extension from server comes in */
            ssl->earlyDataStatus = WOLFSSL_EARLY_DATA_ACCEPTED;
        }

        return TLSX_EarlyData_Use(ssl, 1, 1);
    }
    if (msgType == session_ticket) {
        word32 maxSz;

        if (length != OPAQUE32_LEN)
            return BUFFER_E;
        ato32(input, &maxSz);

        ssl->session->maxEarlyDataSz = maxSz;
        return 0;
    }

    WOLFSSL_ERROR_VERBOSE(SANITY_MSG_E);
    return SANITY_MSG_E;
}

/* Use the data to create a new Early Data object in the extensions.
 *
 * ssl    The SSL/TLS object.
 * maxSz  The maximum early data size.
 * is_response   if this extension is part of a response
 * returns 0 on success and other values indicate failure.
 */
int TLSX_EarlyData_Use(WOLFSSL* ssl, word32 maxSz, int is_response)
{
    int   ret = 0;
    TLSX* extension;

    /* Find the early data extension if it exists. */
    extension = TLSX_Find(ssl->extensions, TLSX_EARLY_DATA);
    if (extension == NULL) {
        /* Push new early data extension. */
        ret = TLSX_Push(&ssl->extensions, TLSX_EARLY_DATA, NULL, ssl->heap);
        if (ret != 0)
            return ret;

        extension = TLSX_Find(ssl->extensions, TLSX_EARLY_DATA);
        if (extension == NULL)
            return MEMORY_E;
    }

    extension->resp = is_response;
    /* In QUIC, earlydata size is either 0 or 0xffffffff.
     * Override any size between, possibly left from our initial value */
    extension->val  = (WOLFSSL_IS_QUIC(ssl) && is_response && maxSz > 0) ?
                       WOLFSSL_MAX_32BIT : maxSz;

    return 0;
}

#define EDI_GET_SIZE  TLSX_EarlyData_GetSize
#define EDI_WRITE     TLSX_EarlyData_Write
#define EDI_PARSE     TLSX_EarlyData_Parse

#else

#define EDI_GET_SIZE(a, b)    0
#define EDI_WRITE(a, b, c, d) 0
#define EDI_PARSE(a, b, c, d) 0

#endif

/******************************************************************************/
/* QUIC transport parameter extension                                         */
/******************************************************************************/
#ifdef WOLFSSL_QUIC

static word16 TLSX_QuicTP_GetSize(TLSX* extension)
{
    const QuicTransportParam *tp = (QuicTransportParam*)extension->data;

    return tp ? tp->len : 0;
}

int TLSX_QuicTP_Use(WOLFSSL* ssl, TLSX_Type ext_type, int is_response)
{
    int ret = 0;
    TLSX* extension;

    WOLFSSL_ENTER("TLSX_QuicTP_Use");
    if (ssl->quic.transport_local == NULL) {
        /* RFC9000, ch 7.3: "An endpoint MUST treat the absence of [...]
         *     from either endpoint [...] as a connection error of type
         *     TRANSPORT_PARAMETER_ERROR."
         */
        ret = QUIC_TP_MISSING_E;
        goto cleanup;
    }

    extension = TLSX_Find(ssl->extensions, ext_type);
    if (extension == NULL) {
        ret = TLSX_Push(&ssl->extensions, ext_type, NULL, ssl->heap);
        if (ret != 0)
            goto cleanup;

        extension = TLSX_Find(ssl->extensions, ext_type);
        if (extension == NULL) {
            ret = MEMORY_E;
            goto cleanup;
        }
    }
    if (extension->data) {
        QuicTransportParam_free((QuicTransportParam*)extension->data, ssl->heap);
        extension->data = NULL;
    }
    extension->resp = is_response;
    extension->data = (void*)QuicTransportParam_dup(ssl->quic.transport_local, ssl->heap);
    if (!extension->data) {
        ret = MEMORY_E;
        goto cleanup;
    }

cleanup:
    WOLFSSL_LEAVE("TLSX_QuicTP_Use", ret);
    return ret;
}

static word16 TLSX_QuicTP_Write(QuicTransportParam *tp, byte* output)
{
    word16 len = 0;

    WOLFSSL_ENTER("TLSX_QuicTP_Write");
    if (tp && tp->len) {
        XMEMCPY(output, tp->data, tp->len);
        len = tp->len;
    }
    WOLFSSL_LEAVE("TLSX_QuicTP_Write", len);
    return len;
}

static int TLSX_QuicTP_Parse(WOLFSSL *ssl, const byte *input, size_t len, int ext_type, int msgType)
{
    const QuicTransportParam *tp, **ptp;

    (void)msgType;
    tp = QuicTransportParam_new(input, len, ssl->heap);
    if (!tp) {
        return MEMORY_E;
    }
    ptp = (ext_type == TLSX_KEY_QUIC_TP_PARAMS_DRAFT) ?
        &ssl->quic.transport_peer_draft : &ssl->quic.transport_peer;
    if (*ptp) {
        QTP_FREE(*ptp, ssl->heap);
    }
    *ptp = tp;
    return 0;
}

#define QTP_GET_SIZE    TLSX_QuicTP_GetSize
#define QTP_USE         TLSX_QuicTP_Use
#define QTP_WRITE       TLSX_QuicTP_Write
#define QTP_PARSE       TLSX_QuicTP_Parse

#endif /* WOLFSSL_QUIC */

#if defined(WOLFSSL_DTLS_CID)
#define CID_GET_SIZE  TLSX_ConnectionID_GetSize
#define CID_WRITE  TLSX_ConnectionID_Write
#define CID_PARSE  TLSX_ConnectionID_Parse
#define CID_FREE  TLSX_ConnectionID_Free
#else
#define CID_GET_SIZE(a) 0
#define CID_WRITE(a, b) 0
#define CID_PARSE(a, b, c, d) 0
#define CID_FREE(a, b) 0
#endif /* defined(WOLFSSL_DTLS_CID) */

/******************************************************************************/
/* TLS Extensions Framework                                                   */
/******************************************************************************/

/** Finds an extension in the provided list. */
TLSX* TLSX_Find(TLSX* list, TLSX_Type type)
{
    TLSX* extension = list;

    while (extension && extension->type != type)
        extension = extension->next;

    return extension;
}

/** Remove an extension. */
void TLSX_Remove(TLSX** list, TLSX_Type type, void* heap)
{
    TLSX* extension;
    TLSX** next;

    if (list == NULL)
        return;

    extension = *list;
    next = list;

    while (extension && extension->type != type) {
        next = &extension->next;
        extension = extension->next;
    }

    if (extension) {
        *next = extension->next;
        extension->next = NULL;
        TLSX_FreeAll(extension, heap);
    }
}

#if defined(WOLFSSL_TLS13) && defined(HAVE_ECH)
#define GREASE_ECH_SIZE 160
#define MAX_PUBLIC_NAME_SZ 256
#define TLS_INFO_CONST_STRING "tls ech"
#define TLS_INFO_CONST_STRING_SZ 7

/* return status after setting up ech to write a grease ech */
static int TLSX_GreaseECH_Use(TLSX** extensions, void* heap, WC_RNG* rng)
{
    int ret = 0;
    WOLFSSL_ECH* ech;

    if (extensions == NULL)
        return BAD_FUNC_ARG;

    ech = (WOLFSSL_ECH*)XMALLOC(sizeof(WOLFSSL_ECH), heap,
        DYNAMIC_TYPE_TMP_BUFFER);

    if (ech == NULL)
        return MEMORY_E;

    ForceZero(ech, sizeof(WOLFSSL_ECH));

    ech->state = ECH_WRITE_GREASE;

    /* 0 for outer */
    ech->type = ECH_TYPE_OUTER;
    /* kemId */
    ech->kemId = DHKEM_X25519_HKDF_SHA256;
    /* cipherSuite kdf */
    ech->cipherSuite.kdfId = HKDF_SHA256;
    /* cipherSuite aead */
    ech->cipherSuite.aeadId = HPKE_AES_128_GCM;

    /* random configId */
    ret = wc_RNG_GenerateByte(rng, &(ech->configId));

    /* curve25519 encLen */
    ech->encLen = DHKEM_X25519_ENC_LEN;

    if (ret == 0)
        ret = TLSX_Push(extensions, TLSX_ECH, ech, heap);

    if (ret != 0) {
        XFREE(ech, heap, DYNAMIC_TYPE_TMP_BUFFER);
    }

    return ret;
}

/* return status after setting up ech to write real ech */
static int TLSX_ECH_Use(WOLFSSL_EchConfig* echConfig, TLSX** extensions,
    void* heap, WC_RNG* rng)
{
    int ret = 0;
    int suiteIndex;
    WOLFSSL_ECH* ech;

    if (extensions == NULL)
        return BAD_FUNC_ARG;

    /* find a supported cipher suite */
    suiteIndex = EchConfigGetSupportedCipherSuite(echConfig);

    if (suiteIndex < 0)
        return suiteIndex;

    ech = (WOLFSSL_ECH*)XMALLOC(sizeof(WOLFSSL_ECH), heap,
        DYNAMIC_TYPE_TMP_BUFFER);

    if (ech == NULL)
        return MEMORY_E;

    ForceZero(ech, sizeof(WOLFSSL_ECH));

    ech->state = ECH_WRITE_REAL;

    ech->echConfig = echConfig;

    /* 0 for outer */
    ech->type = ECH_TYPE_OUTER;
    /* kemId */
    ech->kemId = echConfig->kemId;

    /* cipherSuite kdf */
    ech->cipherSuite.kdfId = echConfig->cipherSuites[suiteIndex].kdfId;
    /* cipherSuite aead */
    ech->cipherSuite.aeadId = echConfig->cipherSuites[suiteIndex].aeadId;
    /* configId */
    ech->configId = echConfig->configId;

    /* encLen */
    switch (echConfig->kemId)
    {
        case DHKEM_P256_HKDF_SHA256:
            ech->encLen = DHKEM_P256_ENC_LEN;
            break;
        case DHKEM_P384_HKDF_SHA384:
            ech->encLen = DHKEM_P384_ENC_LEN;
            break;
        case DHKEM_P521_HKDF_SHA512:
            ech->encLen = DHKEM_P521_ENC_LEN;
            break;
        case DHKEM_X25519_HKDF_SHA256:
            ech->encLen = DHKEM_X25519_ENC_LEN;
            break;
        case DHKEM_X448_HKDF_SHA512:
            ech->encLen = DHKEM_X448_ENC_LEN;
            break;
    }

    /* setup hpke */
    ech->hpke = (Hpke*)XMALLOC(sizeof(Hpke), heap, DYNAMIC_TYPE_TMP_BUFFER);

    if (ech->hpke == NULL) {
        XFREE(ech, heap, DYNAMIC_TYPE_TMP_BUFFER);
        return MEMORY_E;
    }

    ret = wc_HpkeInit(ech->hpke, ech->kemId, ech->cipherSuite.kdfId,
        ech->cipherSuite.aeadId, heap);

    /* setup the ephemeralKey */
    if (ret == 0)
        ret = wc_HpkeGenerateKeyPair(ech->hpke, &ech->ephemeralKey, rng);

    if (ret == 0)
        ret = TLSX_Push(extensions, TLSX_ECH, ech, heap);

    if (ret != 0) {
        XFREE(ech->hpke, heap, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(ech, heap, DYNAMIC_TYPE_TMP_BUFFER);
    }

    return ret;
}

/* return status after setting up ech to read and decrypt */
static int TLSX_ServerECH_Use(TLSX** extensions, void* heap,
    WOLFSSL_EchConfig* configs)
{
    int ret;
    WOLFSSL_ECH* ech;
    TLSX* echX;

    if (extensions == NULL)
        return BAD_FUNC_ARG;

    /* if we already have ech don't override it */
    echX = TLSX_Find(*extensions, TLSX_ECH);
    if (echX != NULL)
        return 0;

    ech = (WOLFSSL_ECH*)XMALLOC(sizeof(WOLFSSL_ECH), heap,
        DYNAMIC_TYPE_TMP_BUFFER);

    if (ech == NULL)
        return MEMORY_E;

    ForceZero(ech, sizeof(WOLFSSL_ECH));

    ech->state = ECH_WRITE_NONE;

    /* 0 for outer */
    ech->type = ECH_TYPE_OUTER;

    ech->echConfig = configs;

    /* setup the rest of the settings when we receive ech from the client */
    ret = TLSX_Push(extensions, TLSX_ECH, ech, heap);

    if (ret != 0)
        XFREE(ech, heap, DYNAMIC_TYPE_TMP_BUFFER);

    return ret;
}

/* return length after writing the ech */
static int TLSX_ECH_Write(WOLFSSL_ECH* ech, byte* writeBuf, word16* offset)
{
    int ret = 0;
    int rngRet = -1;
    word32 configsLen = 0;
    void* ephemeralKey = NULL;
    byte* writeBuf_p = writeBuf;
#ifdef WOLFSSL_SMALL_STACK
    Hpke* hpke = NULL;
    WC_RNG* rng = NULL;
#else
    Hpke hpke[1];
    WC_RNG rng[1];
#endif

    WOLFSSL_MSG("TLSX_ECH_Write");

    if (ech->state == ECH_WRITE_NONE || ech->state == ECH_PARSED_INTERNAL)
        return 0;

    if (ech->state == ECH_WRITE_RETRY_CONFIGS) {
        /* get size then write */
        ret = GetEchConfigsEx(ech->echConfig, NULL, &configsLen);

        if (ret != LENGTH_ONLY_E)
            return ret;

        ret = GetEchConfigsEx(ech->echConfig, writeBuf, &configsLen);

        if (ret != WOLFSSL_SUCCESS)
            return ret;

        *offset += configsLen;

        return 0;
    }

#ifdef WOLFSSL_SMALL_STACK
    hpke = (Hpke*)XMALLOC(sizeof(Hpke), NULL, DYNAMIC_TYPE_TMP_BUFFER);

    if (hpke == NULL)
        return MEMORY_E;

    rng = (WC_RNG*)XMALLOC(sizeof(WC_RNG), NULL, DYNAMIC_TYPE_RNG);

    if (rng == NULL) {
        XFREE(hpke, NULL, DYNAMIC_TYPE_RNG);
        return MEMORY_E;
    }
#endif

    /* type */
    *writeBuf_p = ech->type;
    writeBuf_p += sizeof(ech->type);

    /* outer has body, inner does not */
    if (ech->type == ECH_TYPE_OUTER) {
        /* kdfId */
        c16toa(ech->cipherSuite.kdfId, writeBuf_p);
        writeBuf_p += sizeof(ech->cipherSuite.kdfId);

        /* aeadId */
        c16toa(ech->cipherSuite.aeadId, writeBuf_p);
        writeBuf_p += sizeof(ech->cipherSuite.aeadId);

        /* configId */
        *writeBuf_p = ech->configId;
        writeBuf_p += sizeof(ech->configId);

        /* encLen */
        c16toa(ech->encLen, writeBuf_p);
        writeBuf_p += 2;

        if (ech->state == ECH_WRITE_GREASE) {
            /* hpke init */
            ret = wc_HpkeInit(hpke, ech->kemId, ech->cipherSuite.kdfId,
                ech->cipherSuite.aeadId, NULL);

            if (ret == 0)
                rngRet = ret = wc_InitRng(rng);

            /* create the ephemeralKey */
            if (ret == 0)
                ret = wc_HpkeGenerateKeyPair(hpke, &ephemeralKey, rng);

            /* enc */
            if (ret == 0) {
                ret = wc_HpkeSerializePublicKey(hpke, ephemeralKey, writeBuf_p,
                    &ech->encLen);
                writeBuf_p += ech->encLen;
            }

            if (ret == 0) {
                /* innerClientHelloLen */
                c16toa(GREASE_ECH_SIZE + ((writeBuf_p + 2 - writeBuf) % 32),
                    writeBuf_p);
                writeBuf_p += 2;

                /* innerClientHello */
                ret = wc_RNG_GenerateBlock(rng, writeBuf_p, GREASE_ECH_SIZE +
                    ((writeBuf_p - writeBuf) % 32));
                writeBuf_p += GREASE_ECH_SIZE + ((writeBuf_p - writeBuf) % 32);
            }

            if (rngRet == 0)
                wc_FreeRng(rng);

            if (ephemeralKey != NULL)
                wc_HpkeFreeKey(hpke, hpke->kem, ephemeralKey, hpke->heap);
        }
        else {
            /* write enc to writeBuf_p */
            ret = wc_HpkeSerializePublicKey(ech->hpke, ech->ephemeralKey,
                writeBuf_p, &ech->encLen);
            writeBuf_p += ech->encLen;

            /* innerClientHelloLen */
            c16toa(ech->innerClientHelloLen, writeBuf_p);
            writeBuf_p += 2;

            /* set payload offset for when we finalize */
            ech->outerClientPayload = writeBuf_p;

            /* write zeros for payload */
            XMEMSET(writeBuf_p, 0, ech->innerClientHelloLen);
            writeBuf_p += ech->innerClientHelloLen;
        }
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(hpke, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    XFREE(rng, NULL, DYNAMIC_TYPE_RNG);
#endif

    if (ret == 0)
        *offset += (writeBuf_p - writeBuf);

    return ret;
}

/* return the size needed for the ech extension */
static int TLSX_ECH_GetSize(WOLFSSL_ECH* ech)
{
    int ret;
    word32 size;

    if (ech->state == ECH_WRITE_GREASE) {
        size = sizeof(ech->type) + sizeof(ech->cipherSuite) +
            sizeof(ech->configId) + sizeof(word16) + ech->encLen +
            sizeof(word16);

        size += GREASE_ECH_SIZE + (size % 32);
    }
    else if (ech->state == ECH_WRITE_NONE ||
        ech->state == ECH_PARSED_INTERNAL) {
        size = 0;
    }
    else if (ech->state == ECH_WRITE_RETRY_CONFIGS) {
        /* get the size of the raw configs */
        ret = GetEchConfigsEx(ech->echConfig, NULL, &size);

        if (ret != LENGTH_ONLY_E)
            return ret;
    }
    else if (ech->type == ECH_TYPE_INNER)
    {
        size = sizeof(ech->type);
    }
    else
    {
        size = sizeof(ech->type) + sizeof(ech->cipherSuite) +
            sizeof(ech->configId) + sizeof(word16) + ech->encLen +
            sizeof(word16) + ech->innerClientHelloLen;
    }

    return (int)size;
}

/* return status after attempting to open the hpke encrypted ech extension, if
 * successful the inner client hello will be stored in
 * ech->innerClientHelloLen */
static int TLSX_ExtractEch(WOLFSSL_ECH* ech, WOLFSSL_EchConfig* echConfig,
    byte* aad, word32 aadLen, void* heap)
{
    int ret = 0;
    int expectedEncLen;
    int i;
    word32 rawConfigLen = 0;
    byte* info = NULL;
    word32 infoLen = 0;

    if (ech == NULL || echConfig == NULL || aad == NULL)
        return BAD_FUNC_ARG;

    /* verify the kem and key len */
    switch (echConfig->kemId)
    {
        case DHKEM_P256_HKDF_SHA256:
            expectedEncLen = DHKEM_P256_ENC_LEN;
            break;
        case DHKEM_P384_HKDF_SHA384:
            expectedEncLen = DHKEM_P384_ENC_LEN;
            break;
        case DHKEM_P521_HKDF_SHA512:
            expectedEncLen = DHKEM_P521_ENC_LEN;
            break;
        case DHKEM_X25519_HKDF_SHA256:
            expectedEncLen = DHKEM_X25519_ENC_LEN;
            break;
        case DHKEM_X448_HKDF_SHA512:
            expectedEncLen = DHKEM_X448_ENC_LEN;
            break;
        default:
            expectedEncLen = 0;
            break;
    }

    if (expectedEncLen != ech->encLen)
        return BAD_FUNC_ARG;

    /* verify the cipher suite */
    for (i = 0; i < echConfig->numCipherSuites; i++) {
        if (echConfig->cipherSuites[i].kdfId == ech->cipherSuite.kdfId &&
            echConfig->cipherSuites[i].aeadId == ech->cipherSuite.aeadId) {
            break;
        }
    }

    if (i >= echConfig->numCipherSuites) {
        return BAD_FUNC_ARG;
    }

    ech->hpke = (Hpke*)XMALLOC(sizeof(Hpke), heap, DYNAMIC_TYPE_TMP_BUFFER);

    if (ech->hpke == NULL)
        return MEMORY_E;

    ret = wc_HpkeInit(ech->hpke, echConfig->kemId, ech->cipherSuite.kdfId,
        ech->cipherSuite.aeadId, heap);

    /* get the rawConfigLen */
    if (ret == 0)
        ret = GetEchConfig(echConfig, NULL, &rawConfigLen);

    if (ret == LENGTH_ONLY_E)
        ret = 0;

    /* create info */
    if (ret == 0) {
        infoLen = TLS_INFO_CONST_STRING_SZ + 1 + rawConfigLen;
        info = (byte*)XMALLOC(infoLen, heap, DYNAMIC_TYPE_TMP_BUFFER);

        if (info == NULL)
            ret = MEMORY_E;
        else {
            XMEMCPY(info, (byte*)TLS_INFO_CONST_STRING,
                TLS_INFO_CONST_STRING_SZ + 1);
            ret = GetEchConfig(echConfig, info +
                TLS_INFO_CONST_STRING_SZ + 1, &rawConfigLen);
        }
    }

    /* decrypt the ech payload */
    if (ret == 0)
        ret = wc_HpkeOpenBase(ech->hpke, echConfig->receiverPrivkey, ech->enc,
            ech->encLen, info, infoLen, aad, aadLen, ech->outerClientPayload,
            ech->innerClientHelloLen,
            ech->innerClientHello + HANDSHAKE_HEADER_SZ);

    if (ret != 0) {
        XFREE(ech->hpke, heap, DYNAMIC_TYPE_TMP_BUFFER);
        ech->hpke = NULL;
    }

    if (info != NULL)
        XFREE(info, heap, DYNAMIC_TYPE_TMP_BUFFER);

    return ret;
}

/* parse the ech extension, if internal update ech->state and return, if
 * external attempt to extract the inner client_hello, return the status */
static int TLSX_ECH_Parse(WOLFSSL* ssl, const byte* readBuf, word16 size,
    byte msgType)
{
    int ret = 0;
    int i;
    TLSX* echX;
    WOLFSSL_ECH* ech;
    WOLFSSL_EchConfig* echConfig;
    byte* aadCopy;
    byte* readBuf_p = (byte*)readBuf;

    WOLFSSL_MSG("TLSX_ECH_Parse");

    if (size == 0)
        return BAD_FUNC_ARG;

    if (msgType == encrypted_extensions) {
        ret = wolfSSL_SetEchConfigs(ssl, readBuf, size);

        if (ret == WOLFSSL_SUCCESS)
            ret = 0;
    }
    else if (msgType == client_hello && ssl->ctx->echConfigs != NULL) {
        echX = TLSX_Find(ssl->extensions, TLSX_ECH);

        if (echX == NULL)
            return BAD_FUNC_ARG;

        ech = (WOLFSSL_ECH*)echX->data;

        /* read the ech parameters before the payload */
        ech->type = *readBuf_p;
        readBuf_p++;

        if (ech->type == ECH_TYPE_INNER) {
            ech->state = ECH_PARSED_INTERNAL;
            return 0;
        }

        /* technically the payload would only be 1 byte at this length */
        if (size < 11 + ech->encLen)
            return BAD_FUNC_ARG;

        ato16(readBuf_p, &ech->cipherSuite.kdfId);
        readBuf_p += 2;

        ato16(readBuf_p, &ech->cipherSuite.aeadId);
        readBuf_p += 2;

        ech->configId = *readBuf_p;
        readBuf_p++;

        ato16(readBuf_p, &ech->encLen);
        readBuf_p += 2;

        if (ech->encLen > HPKE_Npk_MAX)
            return BAD_FUNC_ARG;

        XMEMCPY(ech->enc, readBuf_p, ech->encLen);
        readBuf_p += ech->encLen;

        ato16(readBuf_p, &ech->innerClientHelloLen);
        ech->innerClientHelloLen -= AES_BLOCK_SIZE;
        readBuf_p += 2;

        ech->outerClientPayload = readBuf_p;

        /* make a copy of the aad */
        aadCopy = (byte*)XMALLOC(ech->aadLen, ssl->heap,
            DYNAMIC_TYPE_TMP_BUFFER);

        if (aadCopy == NULL)
            return MEMORY_E;

        XMEMCPY(aadCopy, ech->aad, ech->aadLen);

        /* set the ech payload of the copy to zeros */
        XMEMSET(aadCopy + (readBuf_p - ech->aad), 0,
            ech->innerClientHelloLen + AES_BLOCK_SIZE);

        /* allocate the inner payload buffer */
        ech->innerClientHello =
            (byte*)XMALLOC(ech->innerClientHelloLen + HANDSHAKE_HEADER_SZ,
            ssl->heap, DYNAMIC_TYPE_TMP_BUFFER);

        if (ech->innerClientHello == NULL) {
            XFREE(aadCopy, ssl->heap, DYNAMIC_TYPE_TMP_BUFFER);
            return MEMORY_E;
        }

        /* first check if the config id matches */
        echConfig = ssl->ctx->echConfigs;

        while (echConfig != NULL) {
            /* decrypt with this config */
            if (echConfig->configId == ech->configId) {
                ret = TLSX_ExtractEch(ech, echConfig, aadCopy, ech->aadLen,
                    ssl->heap);
                break;
            }

            echConfig = echConfig->next;
        }

        /* try to decrypt with all configs */
        if (echConfig == NULL || ret != 0) {
            echConfig = ssl->ctx->echConfigs;

            while (echConfig != NULL) {
                ret = TLSX_ExtractEch(ech, echConfig, aadCopy, ech->aadLen,
                    ssl->heap);

                if (ret== 0)
                    break;

                echConfig = echConfig->next;
            }
        }

        /* if we failed to extract */
        if (ret != 0) {
            XFREE(ech->innerClientHello, ssl->heap, DYNAMIC_TYPE_TMP_BUFFER);
            ech->innerClientHello = NULL;
            ech->state = ECH_WRITE_RETRY_CONFIGS;
        }
        else {
            i = 0;

            /* decrement until before the padding */
            while (ech->innerClientHello[ech->innerClientHelloLen +
                HANDSHAKE_HEADER_SZ - i - 1] != ECH_TYPE_INNER) {
                i++;
            }

            /* subtract the length of the padding from the length */
            ech->innerClientHelloLen -= i;
        }

        XFREE(aadCopy, ssl->heap, DYNAMIC_TYPE_TMP_BUFFER);

        return 0;
    }

    return ret;
}

/* free the ech struct and the dynamic buffer it uses */
static void TLSX_ECH_Free(WOLFSSL_ECH* ech, void* heap)
{
    if (ech->innerClientHello != NULL)
        XFREE(ech->innerClientHello, heap, DYNAMIC_TYPE_TMP_BUFFER);
    if (ech->ephemeralKey != NULL)
        wc_HpkeFreeKey(ech->hpke, ech->hpke->kem, ech->ephemeralKey,
            ech->hpke->heap);
    if (ech->hpke != NULL)
        XFREE(ech->hpke, heap, DYNAMIC_TYPE_TMP_BUFFER);

    XFREE(ech, heap, DYNAMIC_TYPE_TMP_BUFFER);
    (void)heap;
}

/* encrypt the client hello and store it in ech->outerClientPayload, return
 * status */
int TLSX_FinalizeEch(WOLFSSL_ECH* ech, byte* aad, word32 aadLen)
{
    int ret;
    void* receiverPubkey = NULL;
    byte* info;
    int infoLen;
    byte* aadCopy;

    /* import the server public key */
    ret = wc_HpkeDeserializePublicKey(ech->hpke, &receiverPubkey,
        ech->echConfig->receiverPubkey, ech->encLen);

    if (ret == 0) {
        /* create info */
        infoLen = TLS_INFO_CONST_STRING_SZ + 1 + ech->echConfig->rawLen;
        info = (byte*)XMALLOC(infoLen, ech->hpke->heap,
            DYNAMIC_TYPE_TMP_BUFFER);
        if (info == NULL)
            ret = MEMORY_E;

        if (ret == 0) {
            /* puts the null byte in for me */
            XMEMCPY(info, (byte*)TLS_INFO_CONST_STRING, TLS_INFO_CONST_STRING_SZ
                + 1);
            XMEMCPY(info + TLS_INFO_CONST_STRING_SZ + 1, ech->echConfig->raw,
                ech->echConfig->rawLen);

            /* make a copy of the aad since we overwrite it */
            aadCopy = (byte*)XMALLOC(aadLen, ech->hpke->heap,
                DYNAMIC_TYPE_TMP_BUFFER);
            if (aadCopy == NULL) {
                XFREE(info, ech->hpke->heap, DYNAMIC_TYPE_TMP_BUFFER);
                ret = MEMORY_E;
            }
        }

        if (ret == 0) {
            XMEMCPY(aadCopy, aad, aadLen);

            /* seal the payload */
            ret = wc_HpkeSealBase(ech->hpke, ech->ephemeralKey, receiverPubkey,
                info, infoLen, aadCopy, aadLen, ech->innerClientHello,
                ech->innerClientHelloLen - ech->hpke->Nt,
                ech->outerClientPayload);

            XFREE(info, ech->hpke->heap, DYNAMIC_TYPE_TMP_BUFFER);
            XFREE(aadCopy, ech->hpke->heap, DYNAMIC_TYPE_TMP_BUFFER);
        }
    }

    if (receiverPubkey != NULL)
        wc_HpkeFreeKey(ech->hpke, ech->hpke->kem, receiverPubkey,
            ech->hpke->heap);

    return ret;
}

#define GREASE_ECH_USE TLSX_GreaseECH_Use
#define ECH_USE TLSX_ECH_Use
#define SERVER_ECH_USE TLSX_ServerECH_Use
#define ECH_WRITE TLSX_ECH_Write
#define ECH_GET_SIZE TLSX_ECH_GetSize
#define ECH_PARSE TLSX_ECH_Parse
#define ECH_FREE TLSX_ECH_Free

#endif

/** Releases all extensions in the provided list. */
void TLSX_FreeAll(TLSX* list, void* heap)
{
    TLSX* extension;

    while ((extension = list)) {
        list = extension->next;

        switch (extension->type) {

#ifdef HAVE_SNI
            case TLSX_SERVER_NAME:
                SNI_FREE_ALL((SNI*)extension->data, heap);
                break;
#endif

            case TLSX_TRUSTED_CA_KEYS:
                TCA_FREE_ALL((TCA*)extension->data, heap);
                break;

            case TLSX_MAX_FRAGMENT_LENGTH:
                MFL_FREE_ALL(extension->data, heap);
                break;

            case TLSX_EXTENDED_MASTER_SECRET:
            case TLSX_TRUNCATED_HMAC:
                /* Nothing to do. */
                break;

            case TLSX_SUPPORTED_GROUPS:
                EC_FREE_ALL((SupportedCurve*)extension->data, heap);
                break;

            case TLSX_EC_POINT_FORMATS:
                PF_FREE_ALL((PointFormat*)extension->data, heap);
                break;

            case TLSX_STATUS_REQUEST:
                CSR_FREE_ALL((CertificateStatusRequest*)extension->data, heap);
                break;

            case TLSX_STATUS_REQUEST_V2:
                CSR2_FREE_ALL((CertificateStatusRequestItemV2*)extension->data,
                        heap);
                break;

            case TLSX_RENEGOTIATION_INFO:
                SCR_FREE_ALL(extension->data, heap);
                break;

            case TLSX_SESSION_TICKET:
                WOLF_STK_FREE(extension->data, heap);
                break;

            case TLSX_APPLICATION_LAYER_PROTOCOL:
                ALPN_FREE_ALL((ALPN*)extension->data, heap);
                break;
#if !defined(NO_CERTS) && !defined(WOLFSSL_NO_SIGALG)
            case TLSX_SIGNATURE_ALGORITHMS:
                SA_FREE_ALL((SignatureAlgorithms*)extension->data, heap);
                break;
#endif
#if defined(HAVE_ENCRYPT_THEN_MAC) && !defined(WOLFSSL_AEAD_ONLY)
            case TLSX_ENCRYPT_THEN_MAC:
                break;
#endif
#ifdef WOLFSSL_TLS13
            case TLSX_SUPPORTED_VERSIONS:
                break;

    #ifdef WOLFSSL_SEND_HRR_COOKIE
            case TLSX_COOKIE:
                CKE_FREE_ALL((Cookie*)extension->data, heap);
                break;
    #endif

    #if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
            case TLSX_PRE_SHARED_KEY:
                PSK_FREE_ALL((PreSharedKey*)extension->data, heap);
                break;

            case TLSX_PSK_KEY_EXCHANGE_MODES:
                break;
    #endif

    #ifdef WOLFSSL_EARLY_DATA
            case TLSX_EARLY_DATA:
                break;
    #endif

    #ifdef WOLFSSL_POST_HANDSHAKE_AUTH
            case TLSX_POST_HANDSHAKE_AUTH:
                break;
    #endif

    #if !defined(NO_CERTS) && !defined(WOLFSSL_NO_SIGALG)
            case TLSX_SIGNATURE_ALGORITHMS_CERT:
                break;
    #endif

            case TLSX_KEY_SHARE:
                KS_FREE_ALL((KeyShareEntry*)extension->data, heap);
                break;
    #if !defined(NO_CERTS) && !defined(WOLFSSL_NO_CA_NAMES)
            case TLSX_CERTIFICATE_AUTHORITIES:
                break;
    #endif
#endif
#ifdef WOLFSSL_SRTP
            case TLSX_USE_SRTP:
                SRTP_FREE((TlsxSrtp*)extension->data, heap);
                break;
#endif

    #ifdef WOLFSSL_QUIC
            case TLSX_KEY_QUIC_TP_PARAMS:
                FALL_THROUGH;
            case TLSX_KEY_QUIC_TP_PARAMS_DRAFT:
                QTP_FREE((QuicTransportParam*)extension->data, heap);
                break;
    #endif

#ifdef WOLFSSL_DTLS_CID
        case TLSX_CONNECTION_ID:
            CID_FREE((byte*)extension->data, heap);
            break;
#endif /* WOLFSSL_DTLS_CID */
#if defined(WOLFSSL_TLS13) && defined(HAVE_ECH)
        case TLSX_ECH:
            ECH_FREE((WOLFSSL_ECH*)extension->data, heap);
            break;
#endif
        default:
            break;
        }

        XFREE(extension, heap, DYNAMIC_TYPE_TLSX);
    }

    (void)heap;
}

/** Checks if the tls extensions are supported based on the protocol version. */
int TLSX_SupportExtensions(WOLFSSL* ssl) {
    return ssl && (IsTLS(ssl) || ssl->version.major == DTLS_MAJOR);
}

/** Tells the buffered size of the extensions in a list. */
static int TLSX_GetSize(TLSX* list, byte* semaphore, byte msgType,
                        word16* pLength)
{
    int    ret = 0;
    TLSX*  extension;
    word16 length = 0;
    byte   isRequest = (msgType == client_hello ||
                        msgType == certificate_request);

    while ((extension = list)) {
        list = extension->next;

        /* only extensions marked as response are sent back to the client. */
        if (!isRequest && !extension->resp)
            continue; /* skip! */

        /* ssl level extensions are expected to override ctx level ones. */
        if (!IS_OFF(semaphore, TLSX_ToSemaphore(extension->type)))
            continue; /* skip! */

        /* extension type + extension data length. */
        length += HELLO_EXT_TYPE_SZ + OPAQUE16_LEN;

        switch (extension->type) {

#ifdef HAVE_SNI
            case TLSX_SERVER_NAME:
                /* SNI only sends the name on the request. */
                if (isRequest)
                    length += SNI_GET_SIZE((SNI*)extension->data);
                break;
#endif

            case TLSX_TRUSTED_CA_KEYS:
                /* TCA only sends the list on the request. */
                if (isRequest)
                    length += TCA_GET_SIZE((TCA*)extension->data);
                break;

            case TLSX_MAX_FRAGMENT_LENGTH:
                length += MFL_GET_SIZE(extension->data);
                break;

            case TLSX_EXTENDED_MASTER_SECRET:
            case TLSX_TRUNCATED_HMAC:
                /* always empty. */
                break;

            case TLSX_SUPPORTED_GROUPS:
                length += EC_GET_SIZE((SupportedCurve*)extension->data);
                break;

            case TLSX_EC_POINT_FORMATS:
                length += PF_GET_SIZE((PointFormat*)extension->data);
                break;

            case TLSX_STATUS_REQUEST:
                length += CSR_GET_SIZE(
                         (CertificateStatusRequest*)extension->data, isRequest);
                break;

            case TLSX_STATUS_REQUEST_V2:
                length += CSR2_GET_SIZE(
                        (CertificateStatusRequestItemV2*)extension->data,
                        isRequest);
                break;

            case TLSX_RENEGOTIATION_INFO:
                length += SCR_GET_SIZE((SecureRenegotiation*)extension->data,
                        isRequest);
                break;

            case TLSX_SESSION_TICKET:
                length += WOLF_STK_GET_SIZE((SessionTicket*)extension->data,
                        isRequest);
                break;

            case TLSX_APPLICATION_LAYER_PROTOCOL:
                length += ALPN_GET_SIZE((ALPN*)extension->data);
                break;
#if !defined(NO_CERTS) && !defined(WOLFSSL_NO_SIGALG)
            case TLSX_SIGNATURE_ALGORITHMS:
                length += SA_GET_SIZE(extension->data);
                break;
#endif
#if defined(HAVE_ENCRYPT_THEN_MAC) && !defined(WOLFSSL_AEAD_ONLY)
            case TLSX_ENCRYPT_THEN_MAC:
                ret = ETM_GET_SIZE(msgType, &length);
                break;
#endif /* HAVE_ENCRYPT_THEN_MAC */
#ifdef WOLFSSL_TLS13
            case TLSX_SUPPORTED_VERSIONS:
                ret = SV_GET_SIZE(extension->data, msgType, &length);
                break;

    #ifdef WOLFSSL_SEND_HRR_COOKIE
            case TLSX_COOKIE:
                ret = CKE_GET_SIZE((Cookie*)extension->data, msgType, &length);
                break;
    #endif

    #if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
            case TLSX_PRE_SHARED_KEY:
                ret = PSK_GET_SIZE((PreSharedKey*)extension->data, msgType,
                                                                       &length);
                break;

            case TLSX_PSK_KEY_EXCHANGE_MODES:
                ret = PKM_GET_SIZE((byte)extension->val, msgType, &length);
                break;
    #endif

    #ifdef WOLFSSL_EARLY_DATA
            case TLSX_EARLY_DATA:
                ret = EDI_GET_SIZE(msgType, &length);
                break;
    #endif

    #ifdef WOLFSSL_POST_HANDSHAKE_AUTH
            case TLSX_POST_HANDSHAKE_AUTH:
                ret = PHA_GET_SIZE(msgType, &length);
                break;
    #endif

    #if !defined(NO_CERTS) && !defined(WOLFSSL_NO_SIGALG)
            case TLSX_SIGNATURE_ALGORITHMS_CERT:
                length += SAC_GET_SIZE(extension->data);
                break;
    #endif

            case TLSX_KEY_SHARE:
                length += KS_GET_SIZE((KeyShareEntry*)extension->data, msgType);
                break;
    #if !defined(NO_CERTS) && !defined(WOLFSSL_NO_CA_NAMES)
            case TLSX_CERTIFICATE_AUTHORITIES:
                length += CAN_GET_SIZE(extension->data);
                break;
    #endif
#endif
#ifdef WOLFSSL_SRTP
            case TLSX_USE_SRTP:
                length += SRTP_GET_SIZE((TlsxSrtp*)extension->data);
                break;
#endif

#ifdef WOLFSSL_QUIC
            case TLSX_KEY_QUIC_TP_PARAMS:
                FALL_THROUGH; /* followed by */
            case TLSX_KEY_QUIC_TP_PARAMS_DRAFT:
                length += QTP_GET_SIZE(extension);
                break;
#endif
#ifdef WOLFSSL_DTLS_CID
            case TLSX_CONNECTION_ID:
                length += CID_GET_SIZE((byte*)extension->data);
                break;
#endif /* WOLFSSL_DTLS_CID */
#if defined(WOLFSSL_TLS13) && defined(HAVE_ECH)
            case TLSX_ECH:
                length += ECH_GET_SIZE((WOLFSSL_ECH*)extension->data);
                break;
#endif
            default:
                break;
        }

        /* marks the extension as processed so ctx level */
        /* extensions don't overlap with ssl level ones. */
        TURN_ON(semaphore, TLSX_ToSemaphore(extension->type));
    }

    *pLength += length;

    return ret;
}

/** Writes the extensions of a list in a buffer. */
static int TLSX_Write(TLSX* list, byte* output, byte* semaphore,
                         byte msgType, word16* pOffset)
{
    int    ret = 0;
    TLSX*  extension;
    word16 offset = 0;
    word16 length_offset = 0;
    byte   isRequest = (msgType == client_hello ||
                        msgType == certificate_request);

    while ((extension = list)) {
        list = extension->next;

        /* only extensions marked as response are written in a response. */
        if (!isRequest && !extension->resp)
            continue; /* skip! */

        /* ssl level extensions are expected to override ctx level ones. */
        if (!IS_OFF(semaphore, TLSX_ToSemaphore(extension->type)))
            continue; /* skip! */

        /* writes extension type. */
        c16toa(extension->type, output + offset);
        offset += HELLO_EXT_TYPE_SZ + OPAQUE16_LEN;
        length_offset = offset;

        /* extension data should be written internally. */
        switch (extension->type) {
#ifdef HAVE_SNI
            case TLSX_SERVER_NAME:
                if (isRequest) {
                    WOLFSSL_MSG("SNI extension to write");
                    offset += SNI_WRITE((SNI*)extension->data, output + offset);
                }
                break;
#endif

            case TLSX_TRUSTED_CA_KEYS:
                WOLFSSL_MSG("Trusted CA Indication extension to write");
                if (isRequest) {
                    offset += TCA_WRITE((TCA*)extension->data, output + offset);
                }
                break;

            case TLSX_MAX_FRAGMENT_LENGTH:
                WOLFSSL_MSG("Max Fragment Length extension to write");
                offset += MFL_WRITE((byte*)extension->data, output + offset);
                break;

            case TLSX_EXTENDED_MASTER_SECRET:
                WOLFSSL_MSG("Extended Master Secret");
                /* always empty. */
                break;

            case TLSX_TRUNCATED_HMAC:
                WOLFSSL_MSG("Truncated HMAC extension to write");
                /* always empty. */
                break;

            case TLSX_SUPPORTED_GROUPS:
                WOLFSSL_MSG("Supported Groups extension to write");
                offset += EC_WRITE((SupportedCurve*)extension->data,
                                    output + offset);
                break;

            case TLSX_EC_POINT_FORMATS:
                WOLFSSL_MSG("Point Formats extension to write");
                offset += PF_WRITE((PointFormat*)extension->data,
                                    output + offset);
                break;

            case TLSX_STATUS_REQUEST:
                WOLFSSL_MSG("Certificate Status Request extension to write");
                offset += CSR_WRITE((CertificateStatusRequest*)extension->data,
                        output + offset, isRequest);
                break;

            case TLSX_STATUS_REQUEST_V2:
                WOLFSSL_MSG("Certificate Status Request v2 extension to write");
                offset += CSR2_WRITE(
                        (CertificateStatusRequestItemV2*)extension->data,
                        output + offset, isRequest);
                break;

            case TLSX_RENEGOTIATION_INFO:
                WOLFSSL_MSG("Secure Renegotiation extension to write");
                offset += SCR_WRITE((SecureRenegotiation*)extension->data,
                        output + offset, isRequest);
                break;

            case TLSX_SESSION_TICKET:
                WOLFSSL_MSG("Session Ticket extension to write");
                offset += WOLF_STK_WRITE((SessionTicket*)extension->data,
                        output + offset, isRequest);
                break;

            case TLSX_APPLICATION_LAYER_PROTOCOL:
                WOLFSSL_MSG("ALPN extension to write");
                offset += ALPN_WRITE((ALPN*)extension->data, output + offset);
                break;
#if !defined(NO_CERTS) && !defined(WOLFSSL_NO_SIGALG)
            case TLSX_SIGNATURE_ALGORITHMS:
                WOLFSSL_MSG("Signature Algorithms extension to write");
                offset += SA_WRITE(extension->data, output + offset);
                break;
#endif
#if defined(HAVE_ENCRYPT_THEN_MAC) && !defined(WOLFSSL_AEAD_ONLY)
            case TLSX_ENCRYPT_THEN_MAC:
                WOLFSSL_MSG("Encrypt-Then-Mac extension to write");
                ret = ETM_WRITE(extension->data, output, msgType, &offset);
                break;
#endif /* HAVE_ENCRYPT_THEN_MAC */
#ifdef WOLFSSL_TLS13
            case TLSX_SUPPORTED_VERSIONS:
                WOLFSSL_MSG("Supported Versions extension to write");
                ret = SV_WRITE(extension->data, output + offset, msgType, &offset);
                break;

    #ifdef WOLFSSL_SEND_HRR_COOKIE
            case TLSX_COOKIE:
                WOLFSSL_MSG("Cookie extension to write");
                ret = CKE_WRITE((Cookie*)extension->data, output + offset,
                                msgType, &offset);
                break;
    #endif

    #if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
            case TLSX_PRE_SHARED_KEY:
                WOLFSSL_MSG("Pre-Shared Key extension to write");
                ret = PSK_WRITE((PreSharedKey*)extension->data, output + offset,
                                                              msgType, &offset);
                break;

            case TLSX_PSK_KEY_EXCHANGE_MODES:
                WOLFSSL_MSG("PSK Key Exchange Modes extension to write");
                ret = PKM_WRITE((byte)extension->val, output + offset, msgType,
                                                                       &offset);
                break;
    #endif

    #ifdef WOLFSSL_EARLY_DATA
            case TLSX_EARLY_DATA:
                WOLFSSL_MSG("Early Data extension to write");
                ret = EDI_WRITE(extension->val, output + offset, msgType,
                                                                       &offset);
                break;
    #endif

    #ifdef WOLFSSL_POST_HANDSHAKE_AUTH
            case TLSX_POST_HANDSHAKE_AUTH:
                WOLFSSL_MSG("Post-Handshake Authentication extension to write");
                ret = PHA_WRITE(output + offset, msgType, &offset);
                break;
    #endif

    #if !defined(NO_CERTS) && !defined(WOLFSSL_NO_SIGALG)
            case TLSX_SIGNATURE_ALGORITHMS_CERT:
                WOLFSSL_MSG("Signature Algorithms extension to write");
                offset += SAC_WRITE(extension->data, output + offset);
                break;
    #endif

            case TLSX_KEY_SHARE:
                WOLFSSL_MSG("Key Share extension to write");
                offset += KS_WRITE((KeyShareEntry*)extension->data,
                                                      output + offset, msgType);
                break;
    #if !defined(NO_CERTS) && !defined(WOLFSSL_NO_CA_NAMES)
            case TLSX_CERTIFICATE_AUTHORITIES:
                WOLFSSL_MSG("Certificate Authorities extension to write");
                offset += CAN_WRITE(extension->data, output + offset);
                break;
    #endif
#endif
#ifdef WOLFSSL_SRTP
            case TLSX_USE_SRTP:
                offset += SRTP_WRITE((TlsxSrtp*)extension->data, output+offset);
                break;
#endif
#ifdef WOLFSSL_QUIC
            case TLSX_KEY_QUIC_TP_PARAMS:
                FALL_THROUGH;
            case TLSX_KEY_QUIC_TP_PARAMS_DRAFT:
                WOLFSSL_MSG("QUIC transport parameter to write");
                offset += QTP_WRITE((QuicTransportParam*)extension->data,
                                    output + offset);
                break;
#endif
#ifdef WOLFSSL_DTLS_CID
            case TLSX_CONNECTION_ID:
                offset += CID_WRITE((byte*)extension->data, output+offset);
                break;

#endif /* WOLFSSL_DTLS_CID */
#if defined(WOLFSSL_TLS13) && defined(HAVE_ECH)
            case TLSX_ECH:
                ret = ECH_WRITE((WOLFSSL_ECH*)extension->data,
                    output + offset, &offset);
                break;
#endif
            default:
                break;
        }

        /* writes extension data length. */
        c16toa(offset - length_offset, output + length_offset - OPAQUE16_LEN);

        /* marks the extension as processed so ctx level */
        /* extensions don't overlap with ssl level ones. */
        TURN_ON(semaphore, TLSX_ToSemaphore(extension->type));

        /* if we encountered an error propagate it */
        if (ret != 0)
            break;
    }

    *pOffset += offset;

    return ret;
}

#ifdef HAVE_SUPPORTED_CURVES

/* Populates the default supported groups / curves */
static int TLSX_PopulateSupportedGroups(WOLFSSL* ssl, TLSX** extensions)
{
    int ret = WOLFSSL_SUCCESS;
#ifdef WOLFSSL_TLS13
#if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
    if (ssl->options.resuming && ssl->session->namedGroup != 0) {
        return TLSX_UseSupportedCurve(extensions, ssl->session->namedGroup,
                                                                     ssl->heap);
    }
#endif

    if (ssl->numGroups != 0) {
        int i;
        for (i = 0; i < ssl->numGroups; i++) {
            ret = TLSX_UseSupportedCurve(extensions, ssl->group[i], ssl->heap);
            if (ret != WOLFSSL_SUCCESS)
                return ret;
        }
        return WOLFSSL_SUCCESS;
    }
#endif /* WOLFSSL_TLS13 */

#if defined(HAVE_ECC)
        /* list in order by strength, since not all servers choose by strength */
        #if (defined(HAVE_ECC521) || defined(HAVE_ALL_CURVES)) && ECC_MIN_KEY_SZ <= 521
            #ifndef NO_ECC_SECP
                ret = TLSX_UseSupportedCurve(extensions,
                                              WOLFSSL_ECC_SECP521R1, ssl->heap);
                if (ret != WOLFSSL_SUCCESS) return ret;
            #endif
        #endif
        #if (defined(HAVE_ECC512) || defined(HAVE_ALL_CURVES)) && ECC_MIN_KEY_SZ <= 512
            #ifdef HAVE_ECC_BRAINPOOL
                ret = TLSX_UseSupportedCurve(extensions,
                                        WOLFSSL_ECC_BRAINPOOLP512R1, ssl->heap);
                if (ret != WOLFSSL_SUCCESS) return ret;
            #endif
        #endif
        #if (defined(HAVE_ECC384) || defined(HAVE_ALL_CURVES)) && ECC_MIN_KEY_SZ <= 384
            #ifndef NO_ECC_SECP
                ret = TLSX_UseSupportedCurve(extensions,
                                              WOLFSSL_ECC_SECP384R1, ssl->heap);
                if (ret != WOLFSSL_SUCCESS) return ret;
            #endif
            #ifdef HAVE_ECC_BRAINPOOL
                ret = TLSX_UseSupportedCurve(extensions,
                                        WOLFSSL_ECC_BRAINPOOLP384R1, ssl->heap);
                if (ret != WOLFSSL_SUCCESS) return ret;
            #endif
        #endif
#endif /* HAVE_ECC */

        #ifndef HAVE_FIPS
            #if defined(HAVE_CURVE448) && ECC_MIN_KEY_SZ <= 448
                ret = TLSX_UseSupportedCurve(extensions,
                                                   WOLFSSL_ECC_X448, ssl->heap);
                if (ret != WOLFSSL_SUCCESS) return ret;
            #endif
        #endif /* HAVE_FIPS */

#if defined(HAVE_ECC) && defined(HAVE_SUPPORTED_CURVES)
        #if (!defined(NO_ECC256)  || defined(HAVE_ALL_CURVES)) && ECC_MIN_KEY_SZ <= 256
            #ifndef NO_ECC_SECP
                ret = TLSX_UseSupportedCurve(extensions,
                                              WOLFSSL_ECC_SECP256R1, ssl->heap);
                if (ret != WOLFSSL_SUCCESS) return ret;
            #endif
            #ifdef HAVE_ECC_KOBLITZ
                ret = TLSX_UseSupportedCurve(extensions,
                                              WOLFSSL_ECC_SECP256K1, ssl->heap);
                if (ret != WOLFSSL_SUCCESS) return ret;
            #endif
            #ifdef HAVE_ECC_BRAINPOOL
                ret = TLSX_UseSupportedCurve(extensions,
                                        WOLFSSL_ECC_BRAINPOOLP256R1, ssl->heap);
                if (ret != WOLFSSL_SUCCESS) return ret;
            #endif
            #ifdef WOLFSSL_SM2
                ret = TLSX_UseSupportedCurve(extensions,
                                              WOLFSSL_ECC_SM2P256V1, ssl->heap);
                if (ret != WOLFSSL_SUCCESS) return ret;
            #endif
        #endif
#endif /* HAVE_ECC */

        #ifndef HAVE_FIPS
            #if defined(HAVE_CURVE25519) && ECC_MIN_KEY_SZ <= 256
                ret = TLSX_UseSupportedCurve(extensions,
                                                 WOLFSSL_ECC_X25519, ssl->heap);
                if (ret != WOLFSSL_SUCCESS) return ret;
            #endif
        #endif /* HAVE_FIPS */

#if defined(HAVE_ECC) && defined(HAVE_SUPPORTED_CURVES)
        #if (defined(HAVE_ECC224) || defined(HAVE_ALL_CURVES)) && ECC_MIN_KEY_SZ <= 224
            #ifndef NO_ECC_SECP
                ret = TLSX_UseSupportedCurve(extensions,
                                              WOLFSSL_ECC_SECP224R1, ssl->heap);
                if (ret != WOLFSSL_SUCCESS) return ret;
            #endif
            #ifdef HAVE_ECC_KOBLITZ
                ret = TLSX_UseSupportedCurve(extensions,
                                              WOLFSSL_ECC_SECP224K1, ssl->heap);
                if (ret != WOLFSSL_SUCCESS) return ret;
            #endif
        #endif

    #ifndef HAVE_FIPS
        #if (defined(HAVE_ECC192) || defined(HAVE_ALL_CURVES)) && ECC_MIN_KEY_SZ <= 192
            #ifndef NO_ECC_SECP
                ret = TLSX_UseSupportedCurve(extensions,
                                              WOLFSSL_ECC_SECP192R1, ssl->heap);
                if (ret != WOLFSSL_SUCCESS) return ret;
            #endif
            #ifdef HAVE_ECC_KOBLITZ
                ret = TLSX_UseSupportedCurve(extensions,
                                              WOLFSSL_ECC_SECP192K1, ssl->heap);
                if (ret != WOLFSSL_SUCCESS) return ret;
            #endif
        #endif
        #if (defined(HAVE_ECC160) || defined(HAVE_ALL_CURVES)) && ECC_MIN_KEY_SZ <= 160
            #ifndef NO_ECC_SECP
                ret = TLSX_UseSupportedCurve(extensions,
                                              WOLFSSL_ECC_SECP160R1, ssl->heap);
                if (ret != WOLFSSL_SUCCESS) return ret;
            #endif
            #ifdef HAVE_ECC_SECPR2
                ret = TLSX_UseSupportedCurve(extensions,
                                              WOLFSSL_ECC_SECP160R2, ssl->heap);
                if (ret != WOLFSSL_SUCCESS) return ret;
            #endif
            #ifdef HAVE_ECC_KOBLITZ
                ret = TLSX_UseSupportedCurve(extensions,
                                              WOLFSSL_ECC_SECP160K1, ssl->heap);
                if (ret != WOLFSSL_SUCCESS) return ret;
            #endif
        #endif
    #endif /* HAVE_FIPS */
#endif /* HAVE_ECC */

#ifndef NO_DH
            /* Add FFDHE supported groups. */
        #ifdef HAVE_FFDHE_8192
            if (8192/8 >= ssl->options.minDhKeySz &&
                                            8192/8 <= ssl->options.maxDhKeySz) {
                ret = TLSX_UseSupportedCurve(extensions,
                                             WOLFSSL_FFDHE_8192, ssl->heap);
                if (ret != WOLFSSL_SUCCESS)
                    return ret;
            }
        #endif
        #ifdef HAVE_FFDHE_6144
            if (6144/8 >= ssl->options.minDhKeySz &&
                                            6144/8 <= ssl->options.maxDhKeySz) {
                ret = TLSX_UseSupportedCurve(extensions,
                                             WOLFSSL_FFDHE_6144, ssl->heap);
                if (ret != WOLFSSL_SUCCESS)
                    return ret;
            }
        #endif
        #ifdef HAVE_FFDHE_4096
            if (4096/8 >= ssl->options.minDhKeySz &&
                                            4096/8 <= ssl->options.maxDhKeySz) {
                ret = TLSX_UseSupportedCurve(extensions,
                                             WOLFSSL_FFDHE_4096, ssl->heap);
                if (ret != WOLFSSL_SUCCESS)
                    return ret;
            }
        #endif
        #ifdef HAVE_FFDHE_3072
            if (3072/8 >= ssl->options.minDhKeySz &&
                                            3072/8 <= ssl->options.maxDhKeySz) {
                ret = TLSX_UseSupportedCurve(extensions,
                                             WOLFSSL_FFDHE_3072, ssl->heap);
                if (ret != WOLFSSL_SUCCESS)
                    return ret;
            }
        #endif
        #ifdef HAVE_FFDHE_2048
            if (2048/8 >= ssl->options.minDhKeySz &&
                                            2048/8 <= ssl->options.maxDhKeySz) {
                ret = TLSX_UseSupportedCurve(extensions,
                                             WOLFSSL_FFDHE_2048, ssl->heap);
                if (ret != WOLFSSL_SUCCESS)
                    return ret;
            }
        #endif
#endif

#ifdef HAVE_PQC
#ifdef WOLFSSL_WC_KYBER
#ifdef WOLFSSL_KYBER512
    if (ret == WOLFSSL_SUCCESS)
        ret = TLSX_UseSupportedCurve(extensions, WOLFSSL_KYBER_LEVEL1,
                                     ssl->heap);
#endif
#ifdef WOLFSSL_KYBER768
    if (ret == WOLFSSL_SUCCESS)
        ret = TLSX_UseSupportedCurve(extensions, WOLFSSL_KYBER_LEVEL3,
                                     ssl->heap);
#endif
#ifdef WOLFSSL_KYBER768
    if (ret == WOLFSSL_SUCCESS)
        ret = TLSX_UseSupportedCurve(extensions, WOLFSSL_KYBER_LEVEL5,
                                     ssl->heap);
#endif
#elif defined(HAVE_LIBOQS)
    ret = TLSX_UseSupportedCurve(extensions, WOLFSSL_KYBER_LEVEL1, ssl->heap);
    if (ret == WOLFSSL_SUCCESS)
        ret = TLSX_UseSupportedCurve(extensions, WOLFSSL_KYBER_LEVEL3,
                                     ssl->heap);
    if (ret == WOLFSSL_SUCCESS)
        ret = TLSX_UseSupportedCurve(extensions, WOLFSSL_KYBER_LEVEL5,
                                     ssl->heap);
    if (ret == WOLFSSL_SUCCESS)
        ret = TLSX_UseSupportedCurve(extensions, WOLFSSL_P256_KYBER_LEVEL1,
                                     ssl->heap);
    if (ret == WOLFSSL_SUCCESS)
        ret = TLSX_UseSupportedCurve(extensions, WOLFSSL_P384_KYBER_LEVEL3,
                                     ssl->heap);
    if (ret == WOLFSSL_SUCCESS)
        ret = TLSX_UseSupportedCurve(extensions, WOLFSSL_P521_KYBER_LEVEL5,
                                     ssl->heap);
#elif defined(HAVE_PQM4)
    ret = TLSX_UseSupportedCurve(extensions, WOLFSSL_KYBER_LEVEL1, ssl->heap);
#endif /* HAVE_LIBOQS */
#endif /* HAVE_PQC */

    (void)ssl;
    (void)extensions;

    return ret;
}

#endif /* HAVE_SUPPORTED_CURVES */

int TLSX_PopulateExtensions(WOLFSSL* ssl, byte isServer)
{
    int ret = 0;
    byte* public_key      = NULL;
    word16 public_key_len = 0;
#if defined(WOLFSSL_TLS13) && (defined(HAVE_SESSION_TICKET) || !defined(NO_PSK))
    int usingPSK = 0;
#endif
#if defined(HAVE_SUPPORTED_CURVES) && defined(WOLFSSL_TLS13)
    TLSX* extension = NULL;
    word16 namedGroup = WOLFSSL_NAMED_GROUP_INVALID;
#endif

    /* server will add extension depending on what is parsed from client */
    if (!isServer) {
#if defined(HAVE_ENCRYPT_THEN_MAC) && !defined(WOLFSSL_AEAD_ONLY)
        if (!ssl->options.disallowEncThenMac) {
            ret = TLSX_EncryptThenMac_Use(ssl);
            if (ret != 0)
                return ret;
        }
#endif

#if (defined(HAVE_ECC) || defined(HAVE_CURVE25519) || \
                       defined(HAVE_CURVE448)) && defined(HAVE_SUPPORTED_CURVES)
        if (!ssl->options.userCurves && !ssl->ctx->userCurves) {
            if (TLSX_Find(ssl->ctx->extensions,
                                               TLSX_SUPPORTED_GROUPS) == NULL) {
                ret = TLSX_PopulateSupportedGroups(ssl, &ssl->extensions);
                if (ret != WOLFSSL_SUCCESS)
                    return ret;
            }
        }
        if ((!IsAtLeastTLSv1_3(ssl->version) || ssl->options.downgrade) &&
               TLSX_Find(ssl->ctx->extensions, TLSX_EC_POINT_FORMATS) == NULL &&
               TLSX_Find(ssl->extensions, TLSX_EC_POINT_FORMATS) == NULL) {
             ret = TLSX_UsePointFormat(&ssl->extensions,
                                         WOLFSSL_EC_PF_UNCOMPRESSED, ssl->heap);
             if (ret != WOLFSSL_SUCCESS)
                 return ret;
        }
#endif /* (HAVE_ECC || CURVE25519 || CURVE448) && HAVE_SUPPORTED_CURVES */

#ifdef WOLFSSL_SRTP
        if (ssl->options.dtls && ssl->dtlsSrtpProfiles != 0) {
            WOLFSSL_MSG("Adding DTLS SRTP extension");
            if ((ret = TLSX_UseSRTP(&ssl->extensions, ssl->dtlsSrtpProfiles,
                                                                ssl->heap)) != 0) {
                return ret;
            }
        }
#endif
    } /* is not server */

#if !defined(NO_CERTS) && !defined(WOLFSSL_NO_SIGALG)
    WOLFSSL_MSG("Adding signature algorithms extension");
    if ((ret = TLSX_SetSignatureAlgorithms(&ssl->extensions, ssl, ssl->heap))
                                                                         != 0) {
            return ret;
    }
#else
    ret = 0;
#endif
#ifdef WOLFSSL_TLS13
    #if !defined(NO_CERTS) && !defined(WOLFSSL_NO_CA_NAMES)
        if (isServer && IsAtLeastTLSv1_3(ssl->version)) {
            if (SSL_CA_NAMES(ssl) != NULL) {
                WOLFSSL_MSG("Adding certificate authorities extension");
                if ((ret = TLSX_Push(&ssl->extensions,
                        TLSX_CERTIFICATE_AUTHORITIES, ssl, ssl->heap)) != 0) {
                        return ret;
                }
            }
        }
    #endif
        if (!isServer && IsAtLeastTLSv1_3(ssl->version)) {
            /* Add mandatory TLS v1.3 extension: supported version */
            WOLFSSL_MSG("Adding supported versions extension");
            if ((ret = TLSX_SetSupportedVersions(&ssl->extensions, ssl,
                                                             ssl->heap)) != 0) {
                return ret;
            }

    #if !defined(HAVE_ECC) && !defined(HAVE_CURVE25519) && \
                       !defined(HAVE_CURVE448) && defined(HAVE_SUPPORTED_CURVES)
        if (TLSX_Find(ssl->ctx->extensions, TLSX_SUPPORTED_GROUPS) == NULL) {
            /* Put in DH groups for TLS 1.3 only. */
            ret = TLSX_PopulateSupportedGroups(ssl, &ssl->extensions);
            if (ret != WOLFSSL_SUCCESS)
                return ret;
        /* ret value will be overwritten in !NO_PSK case */
        #ifdef NO_PSK
            ret = 0;
        #endif
        }
    #endif /* !(HAVE_ECC || CURVE25519 || CURVE448) && HAVE_SUPPORTED_CURVES */

        #if !defined(NO_CERTS) && !defined(WOLFSSL_NO_SIGALG)
            if (ssl->certHashSigAlgoSz > 0) {
                WOLFSSL_MSG("Adding signature algorithms cert extension");
                if ((ret = TLSX_SetSignatureAlgorithmsCert(&ssl->extensions,
                                                        ssl, ssl->heap)) != 0) {
                    return ret;
                }
            }
        #endif

        #if defined(HAVE_SUPPORTED_CURVES)
            extension = TLSX_Find(ssl->extensions, TLSX_KEY_SHARE);
            if (extension == NULL) {
            #if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
                if (ssl->options.resuming && ssl->session->namedGroup != 0)
                    namedGroup = ssl->session->namedGroup;
                else
            #endif
                if (ssl->numGroups > 0) {
                    int set = 0;
                    int i, j;

                    /* try to find the highest element in ssl->group[]
                     * that is contained in preferredGroup[].
                     */
                    namedGroup = preferredGroup[0];
                    for (i = 0; i < ssl->numGroups && !set; i++) {
                        for (j = 0; preferredGroup[j] != WOLFSSL_NAMED_GROUP_INVALID; j++) {
                            if (preferredGroup[j] == ssl->group[i]
#ifdef HAVE_LIBOQS
                                && TLSX_KeyShare_IsSupported(preferredGroup[j])
#endif
                                                                ) {
                                namedGroup = ssl->group[i];
                                set = 1;
                                break;
                            }
                        }
                    }
                    if (!set)
                        namedGroup = WOLFSSL_NAMED_GROUP_INVALID;
                }
                else {
                    /* Choose the most preferred group. */
                    namedGroup = preferredGroup[0];
#ifdef HAVE_LIBOQS
                    if (!TLSX_KeyShare_IsSupported(namedGroup)) {
                        int i = 1;
                        for (;preferredGroup[i] != WOLFSSL_NAMED_GROUP_INVALID;
                              i++) {
                            if (TLSX_KeyShare_IsSupported(preferredGroup[i]))
                                break;
                        }
                        namedGroup = preferredGroup[i];
                    }
#endif
                }
            }
            else {
                KeyShareEntry* kse = (KeyShareEntry*)extension->data;
                if (kse)
                    namedGroup = kse->group;
            }
            if (namedGroup != WOLFSSL_NAMED_GROUP_INVALID) {
#ifdef HAVE_PQC
                /* For KEMs, the key share has already been generated, but not
                 * if we are resuming. */
                if (!WOLFSSL_NAMED_GROUP_IS_PQC(namedGroup)
#ifdef HAVE_SESSION_TICKET
                    || ssl->options.resuming
#endif /* HAVE_SESSION_TICKET */
                   )
#endif /* HAVE_PQC */
                {
                    ret = TLSX_KeyShare_Use(ssl, namedGroup, 0, NULL, NULL,
                            &ssl->extensions);
                }
                if (ret != 0)
                    return ret;
            }
        #endif /* HAVE_SUPPORTED_CURVES */

        #if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
            TLSX_Remove(&ssl->extensions, TLSX_PRE_SHARED_KEY, ssl->heap);
        #endif
        #if defined(HAVE_SESSION_TICKET)
            if (ssl->options.resuming && ssl->session->ticketLen > 0) {
                WOLFSSL_SESSION* sess = ssl->session;
            #ifdef WOLFSSL_32BIT_MILLI_TIME
                word32 now, milli;
            #else
                word64 now, milli;
            #endif

                if (sess->ticketLen > MAX_PSK_ID_LEN) {
                    WOLFSSL_MSG("Session ticket length for PSK ext is too large");
                    return BUFFER_ERROR;
                }

                /* Determine the MAC algorithm for the cipher suite used. */
                ssl->options.cipherSuite0 = sess->cipherSuite0;
                ssl->options.cipherSuite  = sess->cipherSuite;
                ret = SetCipherSpecs(ssl);
                if (ret != 0)
                    return ret;
                now = TimeNowInMilliseconds();
                if (now == 0)
                    return GETTIME_ERROR;
            #ifdef WOLFSSL_32BIT_MILLI_TIME
                if (now < sess->ticketSeen)
                    milli = (0xFFFFFFFFU - sess->ticketSeen) + 1 + now;
                else
                    milli = now - sess->ticketSeen;
                milli += sess->ticketAdd;

                /* Pre-shared key is mandatory extension for resumption. */
                ret = TLSX_PreSharedKey_Use(&ssl->extensions, sess->ticket,
                    sess->ticketLen, milli, ssl->specs.mac_algorithm,
                    ssl->options.cipherSuite0, ssl->options.cipherSuite, 1,
                    NULL, ssl->heap);
            #else
                milli = now - sess->ticketSeen + sess->ticketAdd;

                /* Pre-shared key is mandatory extension for resumption. */
                ret = TLSX_PreSharedKey_Use(&ssl->extensions, sess->ticket,
                    sess->ticketLen, (word32)milli, ssl->specs.mac_algorithm,
                    ssl->options.cipherSuite0, ssl->options.cipherSuite, 1,
                    NULL, ssl->heap);
            #endif
                if (ret != 0)
                    return ret;

                usingPSK = 1;
            }
        #endif
    #ifndef NO_PSK
        #ifndef WOLFSSL_PSK_ONE_ID
            if (ssl->options.client_psk_cs_cb != NULL) {
                int i;
                const Suites* suites = WOLFSSL_SUITES(ssl);
                for (i = 0; i < suites->suiteSz; i += 2) {
                    byte cipherSuite0 = suites->suites[i + 0];
                    byte cipherSuite = suites->suites[i + 1];
                    unsigned int keySz;
                #ifdef WOLFSSL_PSK_MULTI_ID_PER_CS
                    int cnt = 0;
                #endif

                #ifdef HAVE_NULL_CIPHER
                    if (cipherSuite0 == ECC_BYTE ||
                        cipherSuite0 == ECDHE_PSK_BYTE) {
                        if (cipherSuite != TLS_SHA256_SHA256 &&
                                             cipherSuite != TLS_SHA384_SHA384) {
                            continue;
                        }
                    }
                    else
                #endif
                #if (defined(WOLFSSL_SM4_GCM) || defined(WOLFSSL_SM4_CCM)) && \
                    defined(WOLFSSL_SM3)
                    if (cipherSuite0 == CIPHER_BYTE) {
                        if ((cipherSuite != TLS_SM4_GCM_SM3) &&
                            (cipherSuite != TLS_SM4_CCM_SM3)) {
                            continue;
                        }
                    }
                    else
                #endif
                    if (cipherSuite0 != TLS13_BYTE)
                        continue;

                #ifdef WOLFSSL_PSK_MULTI_ID_PER_CS
                    do {
                        ssl->arrays->client_identity[0] = cnt;
                #endif

                        ssl->arrays->client_identity[MAX_PSK_ID_LEN] = '\0';
                        keySz = ssl->options.client_psk_cs_cb(
                            ssl, ssl->arrays->server_hint,
                            ssl->arrays->client_identity, MAX_PSK_ID_LEN,
                            ssl->arrays->psk_key, MAX_PSK_KEY_LEN,
                            GetCipherNameInternal(cipherSuite0, cipherSuite));
                        if (keySz > 0) {
                            ssl->arrays->psk_keySz = keySz;
                            ret = TLSX_PreSharedKey_Use(&ssl->extensions,
                                (byte*)ssl->arrays->client_identity,
                                (word16)XSTRLEN(ssl->arrays->client_identity),
                                0, SuiteMac(WOLFSSL_SUITES(ssl)->suites + i),
                                cipherSuite0, cipherSuite, 0, NULL, ssl->heap);
                            if (ret != 0)
                                return ret;
                #ifdef WOLFSSL_PSK_MULTI_ID_PER_CS
                            cnt++;
                #endif
                        }
                #ifdef WOLFSSL_PSK_MULTI_ID_PER_CS
                    }
                    while (keySz > 0);
                #endif
                }

                usingPSK = 1;
            }
            else
        #endif
            if (ssl->options.client_psk_cb != NULL ||
                                     ssl->options.client_psk_tls13_cb != NULL) {
                /* Default cipher suite. */
                byte cipherSuite0 = TLS13_BYTE;
                byte cipherSuite = WOLFSSL_DEF_PSK_CIPHER;
                int cipherSuiteFlags = WOLFSSL_CIPHER_SUITE_FLAG_NONE;
                const char* cipherName = NULL;

                if (ssl->options.client_psk_tls13_cb != NULL) {
                    ssl->arrays->psk_keySz = ssl->options.client_psk_tls13_cb(
                        ssl, ssl->arrays->server_hint,
                        ssl->arrays->client_identity, MAX_PSK_ID_LEN,
                        ssl->arrays->psk_key, MAX_PSK_KEY_LEN, &cipherName);
                    if (GetCipherSuiteFromName(cipherName, &cipherSuite0,
                                        &cipherSuite, &cipherSuiteFlags) != 0) {
                        return PSK_KEY_ERROR;
                    }
                }
                else {
                    ssl->arrays->psk_keySz = ssl->options.client_psk_cb(ssl,
                        ssl->arrays->server_hint, ssl->arrays->client_identity,
                        MAX_PSK_ID_LEN, ssl->arrays->psk_key, MAX_PSK_KEY_LEN);
                }
        #if defined(OPENSSL_EXTRA)
                /* OpenSSL treats 0 as a PSK key length of 0
                 * and meaning no PSK available.
                 */
                if (ssl->arrays->psk_keySz > MAX_PSK_KEY_LEN) {
                    return PSK_KEY_ERROR;
                }
                if (ssl->arrays->psk_keySz > 0) {
        #else
                if (ssl->arrays->psk_keySz == 0 ||
                                     ssl->arrays->psk_keySz > MAX_PSK_KEY_LEN) {
                    return PSK_KEY_ERROR;
                }
        #endif
                ssl->arrays->client_identity[MAX_PSK_ID_LEN] = '\0';

                ssl->options.cipherSuite0 = cipherSuite0;
                ssl->options.cipherSuite  = cipherSuite;
                (void)cipherSuiteFlags;
                ret = SetCipherSpecs(ssl);
                if (ret != 0)
                    return ret;

                ret = TLSX_PreSharedKey_Use(&ssl->extensions,
                                  (byte*)ssl->arrays->client_identity,
                                  (word16)XSTRLEN(ssl->arrays->client_identity),
                                  0, ssl->specs.mac_algorithm,
                                  cipherSuite0, cipherSuite, 0,
                                  NULL, ssl->heap);
                if (ret != 0)
                    return ret;

                usingPSK = 1;
        #if defined(OPENSSL_EXTRA)
                }
        #endif
            }
    #endif /* !NO_PSK */
        #if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)

            /* Some servers do not generate session tickets unless
             * the extension is seen in a non-resume client hello.
             * We used to send it only if we were otherwise using PSK.
             * Now always send it. Define NO_TLSX_PSKKEM_PLAIN_ANNOUNCE
             * to revert to the old behaviour. */
            #ifdef NO_TLSX_PSKKEM_PLAIN_ANNOUNCE
            if (usingPSK)
            #endif
            {
                byte modes = 0;

                (void)usingPSK;
                /* Pre-shared key modes: mandatory extension for resumption. */
            #ifdef HAVE_SUPPORTED_CURVES
                if (!ssl->options.onlyPskDheKe)
            #endif
                {
                    modes = 1 << PSK_KE;
                }
            #if !defined(NO_DH) || defined(HAVE_ECC) || \
                              defined(HAVE_CURVE25519) || defined(HAVE_CURVE448)
                if (!ssl->options.noPskDheKe) {
                    modes |= 1 << PSK_DHE_KE;
                }
            #endif
                ret = TLSX_PskKeyModes_Use(ssl, modes);
                if (ret != 0)
                    return ret;
            }
        #endif
        #if defined(WOLFSSL_POST_HANDSHAKE_AUTH)
            if (!isServer && ssl->options.postHandshakeAuth) {
                ret = TLSX_PostHandAuth_Use(ssl);
                if (ret != 0)
                    return ret;
            }
        #endif
#if defined(HAVE_ECH)
            /* GREASE ECH */
            if (ssl->echConfigs == NULL) {
                ret = GREASE_ECH_USE(&(ssl->extensions), ssl->heap, ssl->rng);
            }
            else if (ssl->echConfigs != NULL) {
                ret = ECH_USE(ssl->echConfigs, &(ssl->extensions), ssl->heap,
                    ssl->rng);
            }
#endif
        }
#if defined(HAVE_ECH)
        else if (IsAtLeastTLSv1_3(ssl->version)) {
            if (ssl->ctx->echConfigs != NULL) {
                ret = SERVER_ECH_USE(&(ssl->extensions), ssl->heap,
                    ssl->ctx->echConfigs);

                if (ret == 0)
                    TLSX_SetResponse(ssl, TLSX_ECH);
            }
        }
#endif

#endif

    (void)isServer;
    (void)public_key;
    (void)public_key_len;
    (void)ssl;

    return ret;
}


#if defined(WOLFSSL_TLS13) || !defined(NO_WOLFSSL_CLIENT)

#if defined(WOLFSSL_TLS13) && defined(HAVE_ECH)
/* because the size of ech depends on the size of other extensions we need to
 * get the size with ech special and process ech last, return status */
static int TLSX_GetSizeWithEch(WOLFSSL* ssl, byte* semaphore, byte msgType,
    word16* pLength)
{
    int ret = 0;
    TLSX* echX = NULL;
    TLSX* serverNameX = NULL;
    TLSX** extensions = NULL;
#ifdef WOLFSSL_SMALL_STACK
    char* tmpServerName = NULL;
#else
    char tmpServerName[MAX_PUBLIC_NAME_SZ];
#endif

    /* calculate the rest of the extensions length with inner ech */
    if (ssl->extensions)
        echX = TLSX_Find(ssl->extensions, TLSX_ECH);

    if (echX == NULL && ssl->ctx && ssl->ctx->extensions)
        echX = TLSX_Find(ssl->ctx->extensions, TLSX_ECH);

    /* if type is outer change sni to public name */
    if (echX != NULL && ((WOLFSSL_ECH*)echX->data)->type == ECH_TYPE_OUTER) {
        if (ssl->extensions) {
            serverNameX = TLSX_Find(ssl->extensions, TLSX_SERVER_NAME);

            if (serverNameX != NULL)
                extensions = &ssl->extensions;
        }

        if (serverNameX == NULL && ssl->ctx && ssl->ctx->extensions) {
            serverNameX = TLSX_Find(ssl->ctx->extensions, TLSX_SERVER_NAME);
            extensions = &ssl->ctx->extensions;
        }

        /* store the inner server name */
        if (serverNameX != NULL) {
            char* hostName = ((SNI*)serverNameX->data)->data.host_name;
            word32 hostNameSz = (word32)XSTRLEN(hostName) + 1;

        #ifdef WOLFSSL_SMALL_STACK
            tmpServerName = (char*)XMALLOC(hostNameSz, ssl->heap,
                DYNAMIC_TYPE_TMP_BUFFER);
            if (tmpServerName == NULL)
                return MEMORY_E;
        #else
            /* truncate if too long */
            if (hostNameSz > MAX_PUBLIC_NAME_SZ)
                hostNameSz = MAX_PUBLIC_NAME_SZ;
        #endif

            XMEMCPY(tmpServerName, hostName, hostNameSz);
        }

        /* remove the inner server name */
        TLSX_Remove(extensions, TLSX_SERVER_NAME, ssl->heap);

        ret = TLSX_UseSNI(extensions, WOLFSSL_SNI_HOST_NAME,
            ((WOLFSSL_ECH*)echX->data)->echConfig->publicName,
            XSTRLEN(((WOLFSSL_ECH*)echX->data)->echConfig->publicName),
            ssl->heap);

        /* set the public name as the server name */
        if (ret == WOLFSSL_SUCCESS)
            ret = 0;
    }

    if (ret == 0 && ssl->extensions)
        ret = TLSX_GetSize(ssl->extensions, semaphore, msgType, pLength);

    if (ret == 0 && ssl->ctx && ssl->ctx->extensions)
        ret = TLSX_GetSize(ssl->ctx->extensions, semaphore, msgType, pLength);

    if (serverNameX != NULL) {
        /* remove the public name SNI */
        TLSX_Remove(extensions, TLSX_SERVER_NAME, ssl->heap);

        ret = TLSX_UseSNI(extensions, WOLFSSL_SNI_HOST_NAME,
            tmpServerName, XSTRLEN(tmpServerName), ssl->heap);

        /* restore the inner server name */
        if (ret == WOLFSSL_SUCCESS)
            ret = 0;
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(tmpServerName, ssl->heap, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return ret;
}
#endif

/** Tells the buffered size of extensions to be sent into the client hello. */
int TLSX_GetRequestSize(WOLFSSL* ssl, byte msgType, word16* pLength)
{
    int ret = 0;
    word16 length = 0;
    byte semaphore[SEMAPHORE_SIZE] = {0};

    if (!TLSX_SupportExtensions(ssl))
        return 0;
    if (msgType == client_hello) {
        EC_VALIDATE_REQUEST(ssl, semaphore);
        PF_VALIDATE_REQUEST(ssl, semaphore);
        WOLF_STK_VALIDATE_REQUEST(ssl);
#if !defined(NO_CERTS) && !defined(WOLFSSL_NO_SIGALG)
        if (WOLFSSL_SUITES(ssl)->hashSigAlgoSz == 0)
            TURN_ON(semaphore, TLSX_ToSemaphore(TLSX_SIGNATURE_ALGORITHMS));
#endif
#if defined(WOLFSSL_TLS13)
        if (!IsAtLeastTLSv1_2(ssl)) {
            TURN_ON(semaphore, TLSX_ToSemaphore(TLSX_SUPPORTED_VERSIONS));
        }
    #if !defined(WOLFSSL_NO_TLS12) || !defined(NO_OLD_TLS)
        if (!IsAtLeastTLSv1_3(ssl->version)) {
            TURN_ON(semaphore, TLSX_ToSemaphore(TLSX_KEY_SHARE));
        #if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
            TURN_ON(semaphore, TLSX_ToSemaphore(TLSX_PRE_SHARED_KEY));
            TURN_ON(semaphore, TLSX_ToSemaphore(TLSX_PSK_KEY_EXCHANGE_MODES));
        #endif
        #ifdef WOLFSSL_EARLY_DATA
            TURN_ON(semaphore, TLSX_ToSemaphore(TLSX_EARLY_DATA));
        #endif
        #ifdef WOLFSSL_SEND_HRR_COOKIE
            TURN_ON(semaphore, TLSX_ToSemaphore(TLSX_COOKIE));
        #endif
        #ifdef WOLFSSL_POST_HANDSHAKE_AUTH
            TURN_ON(semaphore, TLSX_ToSemaphore(TLSX_POST_HANDSHAKE_AUTH));
        #endif
        #if !defined(NO_CERTS) && !defined(WOLFSSL_NO_CA_NAMES)
            TURN_ON(semaphore,
                    TLSX_ToSemaphore(TLSX_CERTIFICATE_AUTHORITIES));
        #endif
        }
    #endif
#endif /* WOLFSSL_TLS13 */
    #if defined(HAVE_CERTIFICATE_STATUS_REQUEST) \
     || defined(HAVE_CERTIFICATE_STATUS_REQUEST_V2)
        if (!SSL_CM(ssl)->ocspStaplingEnabled) {
            /* mark already sent, so it won't send it */
            TURN_ON(semaphore, TLSX_ToSemaphore(TLSX_STATUS_REQUEST));
            TURN_ON(semaphore, TLSX_ToSemaphore(TLSX_STATUS_REQUEST_V2));
        }
    #endif
    }

#ifdef WOLFSSL_TLS13
    #ifndef NO_CERTS
    else if (msgType == certificate_request) {
        /* Don't send out any extension except those that are turned off. */
        XMEMSET(semaphore, 0xff, SEMAPHORE_SIZE);
#if !defined(NO_CERTS) && !defined(WOLFSSL_NO_SIGALG)
        TURN_OFF(semaphore, TLSX_ToSemaphore(TLSX_SIGNATURE_ALGORITHMS));
#endif
#if !defined(NO_CERTS) && !defined(WOLFSSL_NO_CA_NAMES)
        if (SSL_CA_NAMES(ssl) != NULL)
            TURN_OFF(semaphore, TLSX_ToSemaphore(TLSX_CERTIFICATE_AUTHORITIES));
#endif
        /* TODO: TLSX_SIGNED_CERTIFICATE_TIMESTAMP, OID_FILTERS
         *       TLSX_STATUS_REQUEST
         */
    }
    #endif
#if defined(HAVE_ECH)
    if (ssl->options.useEch == 1 && msgType == client_hello) {
        ret = TLSX_GetSizeWithEch(ssl, semaphore, msgType, &length);
        if (ret != 0)
            return ret;
    }
    else
#endif /* HAVE_ECH */
#endif /* WOLFSSL_TLS13 */
    {
        if (ssl->extensions) {
            ret = TLSX_GetSize(ssl->extensions, semaphore, msgType, &length);
            if (ret != 0)
                return ret;
        }
        if (ssl->ctx && ssl->ctx->extensions) {
            ret = TLSX_GetSize(ssl->ctx->extensions, semaphore, msgType,
                &length);
            if (ret != 0)
                return ret;
        }
    }

#ifdef HAVE_EXTENDED_MASTER
    if (msgType == client_hello && ssl->options.haveEMS &&
                  (!IsAtLeastTLSv1_3(ssl->version) || ssl->options.downgrade)) {
        length += HELLO_EXT_SZ;
    }
#endif

    if (length)
        length += OPAQUE16_LEN; /* for total length storage. */

    *pLength += length;

    return ret;
}

#if defined(WOLFSSL_TLS13) && defined(HAVE_ECH)
/* return status after writing the extensions with ech written last */
static int TLSX_WriteWithEch(WOLFSSL* ssl, byte* output, byte* semaphore,
    byte msgType, word16* pOffset)
{
    int ret = 0;
    TLSX* echX = NULL;
    TLSX* serverNameX = NULL;
    TLSX** extensions = NULL;
#ifdef WOLFSSL_SMALL_STACK
    char* tmpServerName = NULL;
#else
    char tmpServerName[MAX_PUBLIC_NAME_SZ];
#endif

    /* get the echX from either extensions or ctx */
    if (ssl->extensions)
        echX = TLSX_Find(ssl->extensions, TLSX_ECH);

    if (echX == NULL && ssl->ctx && ssl->ctx->extensions) {
        /* if not NULL the semaphore will stop it from being counted */
        if (echX == NULL)
            echX = TLSX_Find(ssl->ctx->extensions, TLSX_ECH);
    }

    /* if type is outer change sni to public name */
    if (echX != NULL && ((WOLFSSL_ECH*)echX->data)->type == ECH_TYPE_OUTER) {
        if (ssl->extensions) {
            serverNameX = TLSX_Find(ssl->extensions, TLSX_SERVER_NAME);

            if (serverNameX != NULL)
                extensions = &ssl->extensions;
        }

        if (serverNameX == NULL && ssl->ctx && ssl->ctx->extensions) {
            serverNameX = TLSX_Find(ssl->ctx->extensions, TLSX_SERVER_NAME);
            extensions = &ssl->ctx->extensions;
        }

        /* store the inner server name */
        if (serverNameX != NULL) {
            char* hostName = ((SNI*)serverNameX->data)->data.host_name;
            word32 hostNameSz = (word32)XSTRLEN(hostName) + 1;

        #ifdef WOLFSSL_SMALL_STACK
            tmpServerName = (char*)XMALLOC(hostNameSz, ssl->heap,
                DYNAMIC_TYPE_TMP_BUFFER);
            if (tmpServerName == NULL)
                return MEMORY_E;
        #else
            /* truncate if too long */
            if (hostNameSz > MAX_PUBLIC_NAME_SZ)
                hostNameSz = MAX_PUBLIC_NAME_SZ;
        #endif

            XMEMCPY(tmpServerName, hostName, hostNameSz);
        }

        /* remove the inner server name */
        TLSX_Remove(extensions, TLSX_SERVER_NAME, ssl->heap);

        ret = TLSX_UseSNI(extensions, WOLFSSL_SNI_HOST_NAME,
            ((WOLFSSL_ECH*)echX->data)->echConfig->publicName,
            XSTRLEN(((WOLFSSL_ECH*)echX->data)->echConfig->publicName),
            ssl->heap);

        /* set the public name as the server name */
        if (ret == WOLFSSL_SUCCESS)
            ret = 0;
    }

    if (echX != NULL) {
        /* turn ech on so it doesn't write, then write it last */
        TURN_ON(semaphore, TLSX_ToSemaphore(echX->type));
    }

    if (ret == 0 && ssl->extensions) {
        ret = TLSX_Write(ssl->extensions, output + *pOffset, semaphore,
            msgType, pOffset);
    }

    if (ret == 0 && ssl->ctx && ssl->ctx->extensions) {
        ret = TLSX_Write(ssl->ctx->extensions, output + *pOffset, semaphore,
            msgType, pOffset);
    }

    if (echX != NULL) {
        /* turn off and write it last */
        TURN_OFF(semaphore, TLSX_ToSemaphore(echX->type));
    }

    if (ret == 0 && ssl->extensions) {
        ret = TLSX_Write(ssl->extensions, output + *pOffset, semaphore,
            msgType, pOffset);
    }

    if (ret == 0 && ssl->ctx && ssl->ctx->extensions) {
        ret = TLSX_Write(ssl->ctx->extensions, output + *pOffset, semaphore,
            msgType, pOffset);
    }

    if (serverNameX != NULL) {
        /* remove the public name SNI */
        TLSX_Remove(extensions, TLSX_SERVER_NAME, ssl->heap);

        ret = TLSX_UseSNI(extensions, WOLFSSL_SNI_HOST_NAME, tmpServerName,
            XSTRLEN(tmpServerName), ssl->heap);

        /* restore the inner server name */
        if (ret == WOLFSSL_SUCCESS)
            ret = 0;
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(tmpServerName, ssl->heap, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return ret;
}
#endif

/** Writes the extensions to be sent into the client hello. */
int TLSX_WriteRequest(WOLFSSL* ssl, byte* output, byte msgType, word16* pOffset)
{
    int ret = 0;
    word16 offset = 0;
    byte semaphore[SEMAPHORE_SIZE] = {0};

    if (!TLSX_SupportExtensions(ssl) || output == NULL)
        return 0;

    offset += OPAQUE16_LEN; /* extensions length */

    if (msgType == client_hello) {
        EC_VALIDATE_REQUEST(ssl, semaphore);
        PF_VALIDATE_REQUEST(ssl, semaphore);
        WOLF_STK_VALIDATE_REQUEST(ssl);
#if !defined(NO_CERTS) && !defined(WOLFSSL_NO_SIGALG)
        if (WOLFSSL_SUITES(ssl)->hashSigAlgoSz == 0)
            TURN_ON(semaphore, TLSX_ToSemaphore(TLSX_SIGNATURE_ALGORITHMS));
#endif
#ifdef WOLFSSL_TLS13
        if (!IsAtLeastTLSv1_2(ssl)) {
            TURN_ON(semaphore, TLSX_ToSemaphore(TLSX_SUPPORTED_VERSIONS));
        }
    #if !defined(WOLFSSL_NO_TLS12) || !defined(NO_OLD_TLS)
        if (!IsAtLeastTLSv1_3(ssl->version)) {
            TURN_ON(semaphore, TLSX_ToSemaphore(TLSX_KEY_SHARE));
        #if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
            TURN_ON(semaphore, TLSX_ToSemaphore(TLSX_PSK_KEY_EXCHANGE_MODES));
        #endif
        #ifdef WOLFSSL_EARLY_DATA
            TURN_ON(semaphore, TLSX_ToSemaphore(TLSX_EARLY_DATA));
        #endif
        #ifdef WOLFSSL_SEND_HRR_COOKIE
            TURN_ON(semaphore, TLSX_ToSemaphore(TLSX_COOKIE));
        #endif
        #ifdef WOLFSSL_POST_HANDSHAKE_AUTH
            TURN_ON(semaphore, TLSX_ToSemaphore(TLSX_POST_HANDSHAKE_AUTH));
        #endif
        #if !defined(NO_CERTS) && !defined(WOLFSSL_NO_CA_NAMES)
            TURN_ON(semaphore,
                    TLSX_ToSemaphore(TLSX_CERTIFICATE_AUTHORITIES));
        #endif
        }
    #endif
    #if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
        /* Must write Pre-shared Key extension at the end in TLS v1.3.
         * Must not write out Pre-shared Key extension in earlier versions of
         * protocol.
         */
        TURN_ON(semaphore, TLSX_ToSemaphore(TLSX_PRE_SHARED_KEY));
    #endif
#endif /* WOLFSSL_TLS13 */
    #if defined(HAVE_CERTIFICATE_STATUS_REQUEST) \
     || defined(HAVE_CERTIFICATE_STATUS_REQUEST_V2)
         /* mark already sent, so it won't send it */
        if (!SSL_CM(ssl)->ocspStaplingEnabled) {
            TURN_ON(semaphore, TLSX_ToSemaphore(TLSX_STATUS_REQUEST));
            TURN_ON(semaphore, TLSX_ToSemaphore(TLSX_STATUS_REQUEST_V2));
        }
    #endif
    }
#ifdef WOLFSSL_TLS13
    #ifndef NO_CERTS
    else if (msgType == certificate_request) {
        /* Don't send out any extension except those that are turned off. */
        XMEMSET(semaphore, 0xff, SEMAPHORE_SIZE);
#if !defined(NO_CERTS) && !defined(WOLFSSL_NO_SIGALG)
        TURN_OFF(semaphore, TLSX_ToSemaphore(TLSX_SIGNATURE_ALGORITHMS));
#endif
#if !defined(NO_CERTS) && !defined(WOLFSSL_NO_CA_NAMES)
        if (SSL_CA_NAMES(ssl) != NULL) {
            TURN_OFF(semaphore,
                    TLSX_ToSemaphore(TLSX_CERTIFICATE_AUTHORITIES));
        }
#endif
        /* TODO: TLSX_SIGNED_CERTIFICATE_TIMESTAMP, TLSX_OID_FILTERS
         *       TLSX_STATUS_REQUEST
         */
    }
    #endif
#endif
#if defined(WOLFSSL_TLS13) && defined(HAVE_ECH)
    if (ssl->options.useEch == 1 && msgType == client_hello) {
        ret = TLSX_WriteWithEch(ssl, output, semaphore,
                         msgType, &offset);
        if (ret != 0)
            return ret;
    }
    else
#endif
    {
        if (ssl->extensions) {
            ret = TLSX_Write(ssl->extensions, output + offset, semaphore,
                             msgType, &offset);
            if (ret != 0)
                return ret;
        }
        if (ssl->ctx && ssl->ctx->extensions) {
            ret = TLSX_Write(ssl->ctx->extensions, output + offset, semaphore,
                             msgType, &offset);
            if (ret != 0)
                return ret;
        }
    }

#ifdef HAVE_EXTENDED_MASTER
    if (msgType == client_hello && ssl->options.haveEMS &&
                  (!IsAtLeastTLSv1_3(ssl->version) || ssl->options.downgrade)) {
        WOLFSSL_MSG("EMS extension to write");
        c16toa(HELLO_EXT_EXTMS, output + offset);
        offset += HELLO_EXT_TYPE_SZ;
        c16toa(0, output + offset);
        offset += HELLO_EXT_SZ_SZ;
    }
#endif

#ifdef WOLFSSL_TLS13
    #if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
    if (msgType == client_hello && IsAtLeastTLSv1_3(ssl->version)) {
        /* Write out what we can of Pre-shared key extension.  */
        TURN_OFF(semaphore, TLSX_ToSemaphore(TLSX_PRE_SHARED_KEY));
        ret = TLSX_Write(ssl->extensions, output + offset, semaphore,
                         client_hello, &offset);
        if (ret != 0)
            return ret;
    }
    #endif
#endif

    if (offset > OPAQUE16_LEN || msgType != client_hello)
        c16toa(offset - OPAQUE16_LEN, output); /* extensions length */

     *pOffset += offset;

    return ret;
}

#endif /* WOLFSSL_TLS13 || !NO_WOLFSSL_CLIENT */

#if defined(WOLFSSL_TLS13) || !defined(NO_WOLFSSL_SERVER)

/** Tells the buffered size of extensions to be sent into the server hello. */
int TLSX_GetResponseSize(WOLFSSL* ssl, byte msgType, word16* pLength)
{
    int ret = 0;
    word16 length = 0;
    byte semaphore[SEMAPHORE_SIZE] = {0};

    switch (msgType) {
#ifndef NO_WOLFSSL_SERVER
        case server_hello:
            PF_VALIDATE_RESPONSE(ssl, semaphore);
        #ifdef WOLFSSL_TLS13
                if (IsAtLeastTLSv1_3(ssl->version)) {
                    XMEMSET(semaphore, 0xff, SEMAPHORE_SIZE);
                    TURN_OFF(semaphore,
                                     TLSX_ToSemaphore(TLSX_SUPPORTED_VERSIONS));
                #if defined(HAVE_SUPPORTED_CURVES)
                #if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
                    if (!ssl->options.noPskDheKe)
                #endif
                    {
                        /* Expect KeyShare extension in ServerHello. */
                        TURN_OFF(semaphore, TLSX_ToSemaphore(TLSX_KEY_SHARE));
                    }
                #endif
                #if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
                    TURN_OFF(semaphore, TLSX_ToSemaphore(TLSX_PRE_SHARED_KEY));
                #endif
                #ifdef WOLFSSL_DTLS_CID
                    TURN_OFF(semaphore, TLSX_ToSemaphore(TLSX_CONNECTION_ID));
                #endif
                }
            #if !defined(WOLFSSL_NO_TLS12) || !defined(NO_OLD_TLS)
                else {
                #ifdef HAVE_SUPPORTED_CURVES
                    TURN_ON(semaphore, TLSX_ToSemaphore(TLSX_KEY_SHARE));
                #endif
                #if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
                    TURN_ON(semaphore, TLSX_ToSemaphore(TLSX_PRE_SHARED_KEY));
                #endif
                }
            #endif
        #endif /* WOLFSSL_TLS13 */
            break;

    #ifdef WOLFSSL_TLS13
        case hello_retry_request:
            XMEMSET(semaphore, 0xff, SEMAPHORE_SIZE);
            TURN_OFF(semaphore, TLSX_ToSemaphore(TLSX_SUPPORTED_VERSIONS));
        #ifdef HAVE_SUPPORTED_CURVES
        #if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
            if (!ssl->options.noPskDheKe)
        #endif
            {
                /* Expect KeyShare extension in HelloRetryRequest. */
                TURN_OFF(semaphore, TLSX_ToSemaphore(TLSX_KEY_SHARE));
            }
        #endif
        #ifdef WOLFSSL_SEND_HRR_COOKIE
            TURN_OFF(semaphore, TLSX_ToSemaphore(TLSX_COOKIE));
        #endif
            break;
    #endif

    #ifdef WOLFSSL_TLS13
        case encrypted_extensions:
            /* Send out all extension except those that are turned on. */
        #ifdef HAVE_ECC
            TURN_ON(semaphore, TLSX_ToSemaphore(TLSX_EC_POINT_FORMATS));
        #endif
            TURN_ON(semaphore, TLSX_ToSemaphore(TLSX_SUPPORTED_VERSIONS));
        #ifdef HAVE_SESSION_TICKET
            TURN_ON(semaphore, TLSX_ToSemaphore(TLSX_SESSION_TICKET));
        #endif
        #ifdef HAVE_SUPPORTED_CURVES
            TURN_ON(semaphore, TLSX_ToSemaphore(TLSX_KEY_SHARE));
        #endif
        #if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
            TURN_ON(semaphore, TLSX_ToSemaphore(TLSX_PRE_SHARED_KEY));
        #endif
        #ifdef HAVE_CERTIFICATE_STATUS_REQUEST
            TURN_ON(semaphore, TLSX_ToSemaphore(TLSX_STATUS_REQUEST));
        #endif
        #ifdef HAVE_CERTIFICATE_STATUS_REQUEST_V2
            TURN_ON(semaphore, TLSX_ToSemaphore(TLSX_STATUS_REQUEST_V2));
        #endif
        #if defined(HAVE_SERVER_RENEGOTIATION_INFO)
            TURN_ON(semaphore, TLSX_ToSemaphore(TLSX_RENEGOTIATION_INFO));
        #endif
        #ifdef WOLFSSL_DTLS_CID
            TURN_ON(semaphore, TLSX_ToSemaphore(TLSX_CONNECTION_ID));
        #endif /* WOLFSSL_DTLS_CID */
            break;

        #ifdef WOLFSSL_EARLY_DATA
        case session_ticket:
            if (ssl->options.tls1_3) {
                XMEMSET(semaphore, 0xff, SEMAPHORE_SIZE);
                TURN_OFF(semaphore, TLSX_ToSemaphore(TLSX_EARLY_DATA));
            }
            break;
        #endif
    #endif
#endif

#ifdef WOLFSSL_TLS13
    #ifndef NO_CERTS
        case certificate:
            /* Don't send out any extension except those that are turned off. */
            XMEMSET(semaphore, 0xff, SEMAPHORE_SIZE);
            TURN_OFF(semaphore, TLSX_ToSemaphore(TLSX_STATUS_REQUEST));
            /* TODO: TLSX_SIGNED_CERTIFICATE_TIMESTAMP,
             *       TLSX_SERVER_CERTIFICATE_TYPE
             */
            break;
    #endif
#endif
    }

#ifdef HAVE_EXTENDED_MASTER
    if (ssl->options.haveEMS && msgType == server_hello &&
                                              !IsAtLeastTLSv1_3(ssl->version)) {
        length += HELLO_EXT_SZ;
    }
#endif

    if (TLSX_SupportExtensions(ssl)) {
        ret = TLSX_GetSize(ssl->extensions, semaphore, msgType, &length);
        if (ret != 0)
            return ret;
    }

    /* All the response data is set at the ssl object only, so no ctx here. */

    if (length || msgType != server_hello)
        length += OPAQUE16_LEN; /* for total length storage. */

    *pLength += length;

    return ret;
}

/** Writes the server hello extensions into a buffer. */
int TLSX_WriteResponse(WOLFSSL *ssl, byte* output, byte msgType, word16* pOffset)
{
    int ret = 0;
    word16 offset = 0;

    if (TLSX_SupportExtensions(ssl) && output) {
        byte semaphore[SEMAPHORE_SIZE] = {0};

        switch (msgType) {
#ifndef NO_WOLFSSL_SERVER
            case server_hello:
                PF_VALIDATE_RESPONSE(ssl, semaphore);
    #ifdef WOLFSSL_TLS13
                if (IsAtLeastTLSv1_3(ssl->version)) {
                    XMEMSET(semaphore, 0xff, SEMAPHORE_SIZE);
                    TURN_OFF(semaphore,
                                     TLSX_ToSemaphore(TLSX_SUPPORTED_VERSIONS));
            #ifdef HAVE_SUPPORTED_CURVES
                #if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
                    if (!ssl->options.noPskDheKe)
                #endif
                    {
                        /* Write out KeyShare in ServerHello. */
                        TURN_OFF(semaphore, TLSX_ToSemaphore(TLSX_KEY_SHARE));
                    }
            #endif
            #if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
                    TURN_OFF(semaphore, TLSX_ToSemaphore(TLSX_PRE_SHARED_KEY));
            #endif
            #ifdef WOLFSSL_DTLS_CID
                    TURN_OFF(semaphore, TLSX_ToSemaphore(TLSX_CONNECTION_ID));
            #endif /* WOLFSSL_DTLS_CID */
                }
        #if !defined(WOLFSSL_NO_TLS12) || !defined(NO_OLD_TLS)
                else {
            #ifdef HAVE_SUPPORTED_CURVES
                    TURN_ON(semaphore, TLSX_ToSemaphore(TLSX_KEY_SHARE));
            #endif
            #if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
                    TURN_ON(semaphore, TLSX_ToSemaphore(TLSX_PRE_SHARED_KEY));
            #endif
                }
        #endif
    #endif
                break;

    #ifdef WOLFSSL_TLS13
            case hello_retry_request:
                XMEMSET(semaphore, 0xff, SEMAPHORE_SIZE);
                TURN_OFF(semaphore, TLSX_ToSemaphore(TLSX_SUPPORTED_VERSIONS));
        #ifdef HAVE_SUPPORTED_CURVES
            #if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
                if (!ssl->options.noPskDheKe)
            #endif
                {
                    /* Write out KeyShare in HelloRetryRequest. */
                    TURN_OFF(semaphore, TLSX_ToSemaphore(TLSX_KEY_SHARE));
                }
        #endif
                /* Cookie is written below as last extension. */
                break;
    #endif

    #ifdef WOLFSSL_TLS13
            case encrypted_extensions:
                /* Send out all extension except those that are turned on. */
        #ifdef HAVE_ECC
                TURN_ON(semaphore, TLSX_ToSemaphore(TLSX_EC_POINT_FORMATS));
        #endif
                TURN_ON(semaphore, TLSX_ToSemaphore(TLSX_SUPPORTED_VERSIONS));
        #ifdef HAVE_SESSION_TICKET
                TURN_ON(semaphore, TLSX_ToSemaphore(TLSX_SESSION_TICKET));
        #endif
        #ifdef HAVE_SUPPORTED_CURVES
                TURN_ON(semaphore, TLSX_ToSemaphore(TLSX_KEY_SHARE));
        #endif
        #if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
                TURN_ON(semaphore, TLSX_ToSemaphore(TLSX_PRE_SHARED_KEY));
        #endif
        #ifdef HAVE_CERTIFICATE_STATUS_REQUEST
                TURN_ON(semaphore, TLSX_ToSemaphore(TLSX_STATUS_REQUEST));
        #endif
        #ifdef HAVE_CERTIFICATE_STATUS_REQUEST_V2
            TURN_ON(semaphore, TLSX_ToSemaphore(TLSX_STATUS_REQUEST_V2));
        #endif
        #if defined(HAVE_SERVER_RENEGOTIATION_INFO)
            TURN_ON(semaphore, TLSX_ToSemaphore(TLSX_RENEGOTIATION_INFO));
        #endif
        #ifdef WOLFSSL_DTLS_CID
            TURN_ON(semaphore, TLSX_ToSemaphore(TLSX_CONNECTION_ID));
        #endif /* WOLFSSL_DTLS_CID */
                break;

        #ifdef WOLFSSL_EARLY_DATA
            case session_ticket:
                if (ssl->options.tls1_3) {
                    XMEMSET(semaphore, 0xff, SEMAPHORE_SIZE);
                    TURN_OFF(semaphore, TLSX_ToSemaphore(TLSX_EARLY_DATA));
                }
                break;
        #endif
    #endif
#endif

    #ifdef WOLFSSL_TLS13
        #ifndef NO_CERTS
            case certificate:
                /* Don't send out any extension except those that are turned
                 * off. */
                XMEMSET(semaphore, 0xff, SEMAPHORE_SIZE);
                TURN_OFF(semaphore, TLSX_ToSemaphore(TLSX_STATUS_REQUEST));
                /* TODO: TLSX_SIGNED_CERTIFICATE_TIMESTAMP,
                 *       TLSX_SERVER_CERTIFICATE_TYPE
                 */
                break;
        #endif
    #endif

            default:
                break;
        }

        offset += OPAQUE16_LEN; /* extensions length */

        ret = TLSX_Write(ssl->extensions, output + offset, semaphore,
                         msgType, &offset);
        if (ret != 0)
            return ret;

#if defined(WOLFSSL_TLS13) && defined(WOLFSSL_SEND_HRR_COOKIE)
        if (msgType == hello_retry_request) {
            XMEMSET(semaphore, 0xff, SEMAPHORE_SIZE);
            TURN_OFF(semaphore, TLSX_ToSemaphore(TLSX_COOKIE));
            ret = TLSX_Write(ssl->extensions, output + offset, semaphore,
                             msgType, &offset);
            if (ret != 0)
                return ret;
        }
#endif

#ifdef HAVE_EXTENDED_MASTER
        if (ssl->options.haveEMS && msgType == server_hello &&
                                              !IsAtLeastTLSv1_3(ssl->version)) {
            WOLFSSL_MSG("EMS extension to write");
            c16toa(HELLO_EXT_EXTMS, output + offset);
            offset += HELLO_EXT_TYPE_SZ;
            c16toa(0, output + offset);
            offset += HELLO_EXT_SZ_SZ;
        }
#endif

        if (offset > OPAQUE16_LEN || msgType != server_hello)
            c16toa(offset - OPAQUE16_LEN, output); /* extensions length */
    }

    if (pOffset)
        *pOffset += offset;

    return ret;
}

#endif /* WOLFSSL_TLS13 || !NO_WOLFSSL_SERVER */

#ifdef WOLFSSL_TLS13
int TLSX_ParseVersion(WOLFSSL* ssl, const byte* input, word16 length,
                      byte msgType, int* found)
{
    int ret = 0;
    int offset = 0;

    *found = 0;
    while (offset < (int)length) {
        word16 type;
        word16 size;

        if (offset + (2 * OPAQUE16_LEN) > length) {
            ret = BUFFER_ERROR;
            break;
        }

        ato16(input + offset, &type);
        offset += HELLO_EXT_TYPE_SZ;

        ato16(input + offset, &size);
        offset += OPAQUE16_LEN;

        if (offset + size > length) {
            ret = BUFFER_ERROR;
            break;
        }

        if (type == TLSX_SUPPORTED_VERSIONS) {
            *found = 1;

            WOLFSSL_MSG("Supported Versions extension received");

            ret = SV_PARSE(ssl, input + offset, size, msgType, &ssl->version,
                           &ssl->options, &ssl->extensions);
            break;
        }

        offset += size;
    }

    return ret;
}
#endif

/** Parses a buffer of TLS extensions. */
int TLSX_Parse(WOLFSSL* ssl, const byte* input, word16 length, byte msgType,
                                                                 Suites *suites)
{
    int ret = 0;
    word16 offset = 0;
    byte isRequest = (msgType == client_hello ||
                      msgType == certificate_request);

#ifdef HAVE_EXTENDED_MASTER
    byte pendingEMS = 0;
#endif
#if defined(WOLFSSL_TLS13) && (defined(HAVE_SESSION_TICKET) || !defined(NO_PSK))
    int pskDone = 0;
#endif
    byte seenType[SEMAPHORE_SIZE];  /* Seen known extensions. */

    if (!ssl || !input || (isRequest && !suites))
        return BAD_FUNC_ARG;

    /* No known extensions seen yet. */
    XMEMSET(seenType, 0, sizeof(seenType));

    while (ret == 0 && offset < length) {
        word16 type;
        word16 size;

#if defined(WOLFSSL_TLS13) && (defined(HAVE_SESSION_TICKET) || !defined(NO_PSK))
        if (msgType == client_hello && pskDone) {
            WOLFSSL_ERROR_VERBOSE(PSK_KEY_ERROR);
            return PSK_KEY_ERROR;
        }
#endif

        if (length - offset < HELLO_EXT_TYPE_SZ + OPAQUE16_LEN)
            return BUFFER_ERROR;

        ato16(input + offset, &type);
        offset += HELLO_EXT_TYPE_SZ;

        ato16(input + offset, &size);
        offset += OPAQUE16_LEN;

        /* Check we have a bit for extension type. */
        if ((type <= 62) || (type == TLSX_RENEGOTIATION_INFO)
        #ifdef WOLFSSL_QUIC
            || (type == TLSX_KEY_QUIC_TP_PARAMS_DRAFT)
        #endif
            )
        {
            /* Detect duplicate recognized extensions. */
            if (IS_OFF(seenType, TLSX_ToSemaphore(type))) {
                TURN_ON(seenType, TLSX_ToSemaphore(type));
            }
            else {
                return DUPLICATE_TLS_EXT_E;
            }
        }

        if (length - offset < size)
            return BUFFER_ERROR;

        switch (type) {
#ifdef HAVE_SNI
            case TLSX_SERVER_NAME:
                WOLFSSL_MSG("SNI extension received");
            #ifdef WOLFSSL_DEBUG_TLS
                WOLFSSL_BUFFER(input + offset, size);
            #endif

#ifdef WOLFSSL_TLS13
                if (IsAtLeastTLSv1_3(ssl->version)) {
                    if (msgType != client_hello &&
                        msgType != encrypted_extensions)
                        return EXT_NOT_ALLOWED;
                }
                else
#endif
                {
                    if (msgType != client_hello &&
                        msgType != server_hello)
                        return EXT_NOT_ALLOWED;
                }
                ret = SNI_PARSE(ssl, input + offset, size, isRequest);
                break;
#endif

            case TLSX_TRUSTED_CA_KEYS:
                WOLFSSL_MSG("Trusted CA extension received");
            #ifdef WOLFSSL_DEBUG_TLS
                WOLFSSL_BUFFER(input + offset, size);
            #endif

#ifdef WOLFSSL_TLS13
                /* RFC 8446 4.2.4 states trusted_ca_keys is not used
                   in TLS 1.3. */
                if (IsAtLeastTLSv1_3(ssl->version)) {
                    return EXT_NOT_ALLOWED;
                }
                else
#endif
                {
                    if (msgType != client_hello &&
                        msgType != server_hello)
                        return EXT_NOT_ALLOWED;
                }
                ret = TCA_PARSE(ssl, input + offset, size, isRequest);
                break;

            case TLSX_MAX_FRAGMENT_LENGTH:
                WOLFSSL_MSG("Max Fragment Length extension received");
            #ifdef WOLFSSL_DEBUG_TLS
                WOLFSSL_BUFFER(input + offset, size);
            #endif

#ifdef WOLFSSL_TLS13
                if (IsAtLeastTLSv1_3(ssl->version)) {
                    if (msgType != client_hello &&
                        msgType != encrypted_extensions) {
                        WOLFSSL_ERROR_VERBOSE(EXT_NOT_ALLOWED);
                        return EXT_NOT_ALLOWED;
                    }
                }
                else
#endif
                {
                    if (msgType != client_hello &&
                        msgType != server_hello) {
                        WOLFSSL_ERROR_VERBOSE(EXT_NOT_ALLOWED);
                        return EXT_NOT_ALLOWED;
                    }
                }
                ret = MFL_PARSE(ssl, input + offset, size, isRequest);
                break;

            case TLSX_TRUNCATED_HMAC:
                WOLFSSL_MSG("Truncated HMAC extension received");
            #ifdef WOLFSSL_DEBUG_TLS
                WOLFSSL_BUFFER(input + offset, size);
            #endif

#ifdef WOLFSSL_TLS13
                if (IsAtLeastTLSv1_3(ssl->version))
                    break;
#endif
                if (msgType != client_hello)
                    return EXT_NOT_ALLOWED;
                ret = THM_PARSE(ssl, input + offset, size, isRequest);
                break;

            case TLSX_SUPPORTED_GROUPS:
                WOLFSSL_MSG("Supported Groups extension received");
            #ifdef WOLFSSL_DEBUG_TLS
                WOLFSSL_BUFFER(input + offset, size);
            #endif

#ifdef WOLFSSL_TLS13
                if (IsAtLeastTLSv1_3(ssl->version)) {
                    if (msgType != client_hello &&
                        msgType != encrypted_extensions) {
                        WOLFSSL_ERROR_VERBOSE(EXT_NOT_ALLOWED);
                        return EXT_NOT_ALLOWED;
                    }
                }
                else
#endif
                {
                    if (msgType != client_hello) {
                        WOLFSSL_ERROR_VERBOSE(EXT_NOT_ALLOWED);
                        return EXT_NOT_ALLOWED;
                    }
                }
                ret = EC_PARSE(ssl, input + offset, size, isRequest,
                        &ssl->extensions);
                break;

            case TLSX_EC_POINT_FORMATS:
                WOLFSSL_MSG("Point Formats extension received");
            #ifdef WOLFSSL_DEBUG_TLS
                WOLFSSL_BUFFER(input + offset, size);
            #endif

#ifdef WOLFSSL_TLS13
                if (IsAtLeastTLSv1_3(ssl->version))
                    break;
#endif
                if (msgType != client_hello &&
                    msgType != server_hello) {
                    WOLFSSL_ERROR_VERBOSE(EXT_NOT_ALLOWED);
                    return EXT_NOT_ALLOWED;
                }

                ret = PF_PARSE(ssl, input + offset, size, isRequest);
                break;

            case TLSX_STATUS_REQUEST:
                WOLFSSL_MSG("Certificate Status Request extension received");
            #ifdef WOLFSSL_DEBUG_TLS
                WOLFSSL_BUFFER(input + offset, size);
            #endif

#ifdef WOLFSSL_TLS13
                if (IsAtLeastTLSv1_3(ssl->version)) {
                    if (msgType != client_hello &&
                        msgType != certificate_request &&
                        msgType != certificate)
                        return EXT_NOT_ALLOWED;
                }
                else
 #endif
                {
                    if (msgType != client_hello &&
                        msgType != server_hello)
                        return EXT_NOT_ALLOWED;
                }
                ret = CSR_PARSE(ssl, input + offset, size, isRequest);
                break;

            case TLSX_STATUS_REQUEST_V2:
                WOLFSSL_MSG("Certificate Status Request v2 extension received");
            #ifdef WOLFSSL_DEBUG_TLS
                WOLFSSL_BUFFER(input + offset, size);
            #endif

#if defined(WOLFSSL_TLS13) && defined(HAVE_CERTIFICATE_STATUS_REQUEST_V2)
                if (IsAtLeastTLSv1_3(ssl->version)) {
                    if (msgType != client_hello &&
                        msgType != certificate_request &&
                        msgType != certificate)
                        return EXT_NOT_ALLOWED;
                }
                else
#endif
                {
                    if (msgType != client_hello &&
                        msgType != server_hello)
                        return EXT_NOT_ALLOWED;
                }
                ret = CSR2_PARSE(ssl, input + offset, size, isRequest);
                break;

#ifdef HAVE_EXTENDED_MASTER
            case HELLO_EXT_EXTMS:
                WOLFSSL_MSG("Extended Master Secret extension received");
            #ifdef WOLFSSL_DEBUG_TLS
                WOLFSSL_BUFFER(input + offset, size);
            #endif

#if defined(WOLFSSL_TLS13)
                if (IsAtLeastTLSv1_3(ssl->version))
                    break;
#endif
                if (msgType != client_hello &&
                    msgType != server_hello)
                    return EXT_NOT_ALLOWED;
                if (size != 0)
                    return BUFFER_ERROR;

#ifndef NO_WOLFSSL_SERVER
                if (isRequest)
                    ssl->options.haveEMS = 1;
#endif
                pendingEMS = 1;
                break;
#endif

            case TLSX_RENEGOTIATION_INFO:
                WOLFSSL_MSG("Secure Renegotiation extension received");
            #ifdef WOLFSSL_DEBUG_TLS
                WOLFSSL_BUFFER(input + offset, size);
            #endif

#ifdef WOLFSSL_TLS13
                if (IsAtLeastTLSv1_3(ssl->version))
                    break;
#endif
                if (msgType != client_hello &&
                    msgType != server_hello)
                    return EXT_NOT_ALLOWED;
                ret = SCR_PARSE(ssl, input + offset, size, isRequest);
                break;

            case TLSX_SESSION_TICKET:
                WOLFSSL_MSG("Session Ticket extension received");
            #ifdef WOLFSSL_DEBUG_TLS
                WOLFSSL_BUFFER(input + offset, size);
            #endif

#if defined(WOLFSSL_TLS13) && defined(HAVE_SESSION_TICKET)
                if (IsAtLeastTLSv1_3(ssl->version)) {
                    if (msgType != client_hello)
                        return EXT_NOT_ALLOWED;
                }
                else
#endif
                {
                    if (msgType != client_hello &&
                        msgType != server_hello)
                        return EXT_NOT_ALLOWED;
                }
                ret = WOLF_STK_PARSE(ssl, input + offset, size, isRequest);
                break;

            case TLSX_APPLICATION_LAYER_PROTOCOL:
                WOLFSSL_MSG("ALPN extension received");

            #ifdef WOLFSSL_DEBUG_TLS
                WOLFSSL_BUFFER(input + offset, size);
            #endif

#if defined(WOLFSSL_TLS13) && defined(HAVE_ALPN)
                if (IsAtLeastTLSv1_3(ssl->version)) {
                    if (msgType != client_hello &&
                        msgType != encrypted_extensions)
                        return EXT_NOT_ALLOWED;
                }
                else
#endif
                {
                    if (msgType != client_hello &&
                        msgType != server_hello)
                        return EXT_NOT_ALLOWED;
                }
                ret = ALPN_PARSE(ssl, input + offset, size, isRequest);
                break;
#if !defined(NO_CERTS) && !defined(WOLFSSL_NO_SIGALG)
            case TLSX_SIGNATURE_ALGORITHMS:
                WOLFSSL_MSG("Signature Algorithms extension received");
            #ifdef WOLFSSL_DEBUG_TLS
                WOLFSSL_BUFFER(input + offset, size);
            #endif

                if (!IsAtLeastTLSv1_2(ssl))
                    break;
            #ifdef WOLFSSL_TLS13
                if (IsAtLeastTLSv1_3(ssl->version)) {
                    if (msgType != client_hello &&
                        msgType != certificate_request)
                        return EXT_NOT_ALLOWED;
                }
                else
            #endif
                {
                    if (msgType != client_hello)
                        return EXT_NOT_ALLOWED;
                }
                ret = SA_PARSE(ssl, input + offset, size, isRequest, suites);
                break;
#endif

#if defined(HAVE_ENCRYPT_THEN_MAC) && !defined(WOLFSSL_AEAD_ONLY)
            case TLSX_ENCRYPT_THEN_MAC:
                WOLFSSL_MSG("Encrypt-Then-Mac extension received");

                /* Ignore for TLS 1.3+ */
                if (IsAtLeastTLSv1_3(ssl->version))
                    break;
                if (msgType != client_hello &&
                    msgType != server_hello)
                    return EXT_NOT_ALLOWED;

                ret = ETM_PARSE(ssl, input + offset, size, msgType);
                break;
#endif /* HAVE_ENCRYPT_THEN_MAC */

#ifdef WOLFSSL_TLS13
            case TLSX_SUPPORTED_VERSIONS:
                WOLFSSL_MSG("Skipping Supported Versions - already processed");
            #ifdef WOLFSSL_DEBUG_TLS
                WOLFSSL_BUFFER(input + offset, size);
            #endif
                if (msgType != client_hello &&
                    msgType != server_hello &&
                    msgType != hello_retry_request)
                    return EXT_NOT_ALLOWED;

                break;

    #ifdef WOLFSSL_SEND_HRR_COOKIE
            case TLSX_COOKIE:
                WOLFSSL_MSG("Cookie extension received");
            #ifdef WOLFSSL_DEBUG_TLS
                WOLFSSL_BUFFER(input + offset, size);
            #endif

                if (!IsAtLeastTLSv1_3(ssl->version))
                    break;

                if (msgType != client_hello &&
                    msgType != hello_retry_request) {
                    return EXT_NOT_ALLOWED;
                }

                ret = CKE_PARSE(ssl, input + offset, size, msgType);
                break;
    #endif

    #if defined(HAVE_SESSION_TICKET) || !defined(NO_PSK)
            case TLSX_PRE_SHARED_KEY:
                WOLFSSL_MSG("Pre-Shared Key extension received");
            #ifdef WOLFSSL_DEBUG_TLS
                WOLFSSL_BUFFER(input + offset, size);
            #endif

                if (!IsAtLeastTLSv1_3(ssl->version))
                    break;

                if (msgType != client_hello &&
                    msgType != server_hello) {
                    WOLFSSL_ERROR_VERBOSE(EXT_NOT_ALLOWED);
                    return EXT_NOT_ALLOWED;
                }

                ret = PSK_PARSE(ssl, input + offset, size, msgType);
                pskDone = 1;
                break;

            case TLSX_PSK_KEY_EXCHANGE_MODES:
                WOLFSSL_MSG("PSK Key Exchange Modes extension received");
            #ifdef WOLFSSL_DEBUG_TLS
                WOLFSSL_BUFFER(input + offset, size);
            #endif

                if (!IsAtLeastTLSv1_3(ssl->version))
                    break;

                if (msgType != client_hello) {
                    WOLFSSL_ERROR_VERBOSE(EXT_NOT_ALLOWED);
                    return EXT_NOT_ALLOWED;
                }

                ret = PKM_PARSE(ssl, input + offset, size, msgType);
                break;
    #endif

    #ifdef WOLFSSL_EARLY_DATA
            case TLSX_EARLY_DATA:
                WOLFSSL_MSG("Early Data extension received");
            #ifdef WOLFSSL_DEBUG_TLS
                WOLFSSL_BUFFER(input + offset, size);
            #endif

                if (!IsAtLeastTLSv1_3(ssl->version))
                    break;

                if (msgType != client_hello && msgType != session_ticket &&
                    msgType != encrypted_extensions) {
                    WOLFSSL_ERROR_VERBOSE(EXT_NOT_ALLOWED);
                    return EXT_NOT_ALLOWED;
                }
                ret = EDI_PARSE(ssl, input + offset, size, msgType);
                break;
    #endif

    #ifdef WOLFSSL_POST_HANDSHAKE_AUTH
            case TLSX_POST_HANDSHAKE_AUTH:
                WOLFSSL_MSG("Post Handshake Authentication extension received");
            #ifdef WOLFSSL_DEBUG_TLS
                WOLFSSL_BUFFER(input + offset, size);
            #endif

                if (!IsAtLeastTLSv1_3(ssl->version))
                    break;

                if (msgType != client_hello) {
                    WOLFSSL_ERROR_VERBOSE(EXT_NOT_ALLOWED);
                    return EXT_NOT_ALLOWED;
                }

                ret = PHA_PARSE(ssl, input + offset, size, msgType);
                break;
    #endif

    #if !defined(NO_CERTS) && !defined(WOLFSSL_NO_SIGALG)
            case TLSX_SIGNATURE_ALGORITHMS_CERT:
                WOLFSSL_MSG("Signature Algorithms extension received");
            #ifdef WOLFSSL_DEBUG_TLS
                WOLFSSL_BUFFER(input + offset, size);
            #endif

                if (!IsAtLeastTLSv1_3(ssl->version))
                    break;

                if (msgType != client_hello &&
                        msgType != certificate_request) {
                    WOLFSSL_ERROR_VERBOSE(EXT_NOT_ALLOWED);
                    return EXT_NOT_ALLOWED;
                }

                ret = SAC_PARSE(ssl, input + offset, size, isRequest);
                break;
    #endif

    #if !defined(NO_CERTS) && !defined(WOLFSSL_NO_CA_NAMES)
            case TLSX_CERTIFICATE_AUTHORITIES:
                WOLFSSL_MSG("Certificate Authorities extension received");
            #ifdef WOLFSSL_DEBUG_TLS
                WOLFSSL_BUFFER(input + offset, size);
            #endif

                if (!IsAtLeastTLSv1_3(ssl->version))
                    break;

                if (msgType != client_hello &&
                        msgType != certificate_request) {
                    WOLFSSL_ERROR_VERBOSE(EXT_NOT_ALLOWED);
                    return EXT_NOT_ALLOWED;
                }

                ret = CAN_PARSE(ssl, input + offset, size, isRequest);
                break;
    #endif

            case TLSX_KEY_SHARE:
                WOLFSSL_MSG("Key Share extension received");
            #ifdef WOLFSSL_DEBUG_TLS
                WOLFSSL_BUFFER(input + offset, size);
            #endif

    #ifdef HAVE_SUPPORTED_CURVES
                if (!IsAtLeastTLSv1_3(ssl->version))
                    break;

                if (msgType != client_hello && msgType != server_hello &&
                        msgType != hello_retry_request) {
                    WOLFSSL_ERROR_VERBOSE(EXT_NOT_ALLOWED);
                    return EXT_NOT_ALLOWED;
                }
    #endif

                ret = KS_PARSE(ssl, input + offset, size, msgType);
                break;
#endif
#ifdef WOLFSSL_SRTP
            case TLSX_USE_SRTP:
                WOLFSSL_MSG("Use SRTP extension received");
                ret = SRTP_PARSE(ssl, input + offset, size, isRequest);
                break;
#endif
#ifdef WOLFSSL_QUIC
            case TLSX_KEY_QUIC_TP_PARAMS:
                FALL_THROUGH;
            case TLSX_KEY_QUIC_TP_PARAMS_DRAFT:
                WOLFSSL_MSG("QUIC transport parameter received");
            #ifdef WOLFSSL_DEBUG_TLS
                WOLFSSL_BUFFER(input + offset, size);
            #endif

                if (IsAtLeastTLSv1_3(ssl->version) &&
                        msgType != client_hello &&
                        msgType != server_hello &&
                        msgType != encrypted_extensions) {
                    return EXT_NOT_ALLOWED;
                }
                else if (!IsAtLeastTLSv1_3(ssl->version) &&
                         msgType == encrypted_extensions) {
                    return EXT_NOT_ALLOWED;
                }
                else if (WOLFSSL_IS_QUIC(ssl)) {
                    ret = QTP_PARSE(ssl, input + offset, size, type, msgType);
                }
                else {
                    WOLFSSL_MSG("QUIC transport param TLS extension type, but no QUIC");
                    return EXT_NOT_ALLOWED; /* be safe, this should not happen */
                }
                break;
#endif /* WOLFSSL_QUIC */
#if defined(WOLFSSL_DTLS_CID)
            case TLSX_CONNECTION_ID:
                /* connection ID not supported in DTLSv1.2 */
                if (!IsAtLeastTLSv1_3(ssl->version))
                    break;

                if (msgType != client_hello && msgType != server_hello)
                    return EXT_NOT_ALLOWED;

                WOLFSSL_MSG("ConnectionID extension received");
                ret = CID_PARSE(ssl, input + offset, size, isRequest);
                break;

#endif /* defined(WOLFSSL_DTLS_CID) */
#if defined(WOLFSSL_TLS13) && defined(HAVE_ECH)
            case TLSX_ECH:
                ret = ECH_PARSE(ssl, input + offset, size, msgType);
                break;
#endif
            default:
                WOLFSSL_MSG("Unknown TLS extension type");
        }

        /* offset should be updated here! */
        offset += size;
    }

#ifdef HAVE_EXTENDED_MASTER
    if (IsAtLeastTLSv1_3(ssl->version) && msgType == hello_retry_request) {
        /* Don't change EMS status until server_hello received.
         * Second ClientHello must have same extensions.
         */
    }
    else if (!isRequest && ssl->options.haveEMS && !pendingEMS)
        ssl->options.haveEMS = 0;
#endif

    if (ret == 0)
        ret = SNI_VERIFY_PARSE(ssl, isRequest);
    if (ret == 0)
        ret = TCA_VERIFY_PARSE(ssl, isRequest);

    return ret;
}

/* undefining semaphore macros */
#undef IS_OFF
#undef TURN_ON
#undef SEMAPHORE_SIZE

#endif /* HAVE_TLS_EXTENSIONS */

#ifndef NO_WOLFSSL_CLIENT

    WOLFSSL_METHOD* wolfTLS_client_method(void)
    {
        return wolfTLS_client_method_ex(NULL);
    }
    WOLFSSL_METHOD* wolfTLS_client_method_ex(void* heap)
    {
        WOLFSSL_METHOD* method =
                              (WOLFSSL_METHOD*) XMALLOC(sizeof(WOLFSSL_METHOD),
                                                     heap, DYNAMIC_TYPE_METHOD);
        (void)heap;
        WOLFSSL_ENTER("TLS_client_method_ex");
        if (method) {
        #if defined(WOLFSSL_TLS13)
            InitSSL_Method(method, MakeTLSv1_3());
        #elif !defined(WOLFSSL_NO_TLS12)
            InitSSL_Method(method, MakeTLSv1_2());
        #elif !defined(NO_OLD_TLS)
            InitSSL_Method(method, MakeTLSv1_1());
        #elif defined(WOLFSSL_ALLOW_TLSV10)
            InitSSL_Method(method, MakeTLSv1());
        #else
            #error No TLS version enabled!
        #endif

            method->downgrade = 1;
            method->side      = WOLFSSL_CLIENT_END;
        }
        return method;
    }

#ifndef NO_OLD_TLS
    #ifdef WOLFSSL_ALLOW_TLSV10
    WOLFSSL_METHOD* wolfTLSv1_client_method(void)
    {
        return wolfTLSv1_client_method_ex(NULL);
    }
    WOLFSSL_METHOD* wolfTLSv1_client_method_ex(void* heap)
    {
        WOLFSSL_METHOD* method =
                             (WOLFSSL_METHOD*) XMALLOC(sizeof(WOLFSSL_METHOD),
                                                     heap, DYNAMIC_TYPE_METHOD);
        (void)heap;
        WOLFSSL_ENTER("TLSv1_client_method_ex");
        if (method)
            InitSSL_Method(method, MakeTLSv1());
        return method;
    }
    #endif /* WOLFSSL_ALLOW_TLSV10 */

    WOLFSSL_METHOD* wolfTLSv1_1_client_method(void)
    {
        return wolfTLSv1_1_client_method_ex(NULL);
    }
    WOLFSSL_METHOD* wolfTLSv1_1_client_method_ex(void* heap)
    {
        WOLFSSL_METHOD* method =
                              (WOLFSSL_METHOD*) XMALLOC(sizeof(WOLFSSL_METHOD),
                                                     heap, DYNAMIC_TYPE_METHOD);
        (void)heap;
        WOLFSSL_ENTER("TLSv1_1_client_method_ex");
        if (method)
            InitSSL_Method(method, MakeTLSv1_1());
        return method;
    }
#endif /* !NO_OLD_TLS */

#ifndef WOLFSSL_NO_TLS12
    WOLFSSL_ABI
    WOLFSSL_METHOD* wolfTLSv1_2_client_method(void)
    {
        return wolfTLSv1_2_client_method_ex(NULL);
    }
    WOLFSSL_METHOD* wolfTLSv1_2_client_method_ex(void* heap)
    {
        WOLFSSL_METHOD* method =
                              (WOLFSSL_METHOD*) XMALLOC(sizeof(WOLFSSL_METHOD),
                                                     heap, DYNAMIC_TYPE_METHOD);
        (void)heap;
        WOLFSSL_ENTER("TLSv1_2_client_method_ex");
        if (method)
            InitSSL_Method(method, MakeTLSv1_2());
        return method;
    }
#endif /* WOLFSSL_NO_TLS12 */

#ifdef WOLFSSL_TLS13
    /* The TLS v1.3 client method data.
     *
     * returns the method data for a TLS v1.3 client.
     */
    WOLFSSL_ABI
    WOLFSSL_METHOD* wolfTLSv1_3_client_method(void)
    {
        return wolfTLSv1_3_client_method_ex(NULL);
    }

    /* The TLS v1.3 client method data.
     *
     * heap  The heap used for allocation.
     * returns the method data for a TLS v1.3 client.
     */
    WOLFSSL_METHOD* wolfTLSv1_3_client_method_ex(void* heap)
    {
        WOLFSSL_METHOD* method = (WOLFSSL_METHOD*)
                                 XMALLOC(sizeof(WOLFSSL_METHOD), heap,
                                         DYNAMIC_TYPE_METHOD);
        (void)heap;
        WOLFSSL_ENTER("TLSv1_3_client_method_ex");
        if (method)
            InitSSL_Method(method, MakeTLSv1_3());
        return method;
    }
#endif /* WOLFSSL_TLS13 */

#ifdef WOLFSSL_DTLS

    WOLFSSL_METHOD* wolfDTLS_client_method(void)
    {
        return wolfDTLS_client_method_ex(NULL);
    }
    WOLFSSL_METHOD* wolfDTLS_client_method_ex(void* heap)
    {
        WOLFSSL_METHOD* method =
                              (WOLFSSL_METHOD*) XMALLOC(sizeof(WOLFSSL_METHOD),
                                                     heap, DYNAMIC_TYPE_METHOD);
        (void)heap;
        WOLFSSL_ENTER("DTLS_client_method_ex");
        if (method) {
        #if defined(WOLFSSL_DTLS13)
            InitSSL_Method(method, MakeDTLSv1_3());
        #elif !defined(WOLFSSL_NO_TLS12)
            InitSSL_Method(method, MakeDTLSv1_2());
        #elif !defined(NO_OLD_TLS)
            InitSSL_Method(method, MakeDTLSv1());
        #else
            #error No DTLS version enabled!
        #endif

            method->downgrade = 1;
            method->side      = WOLFSSL_CLIENT_END;
        }
        return method;
    }

    #ifndef NO_OLD_TLS
    WOLFSSL_METHOD* wolfDTLSv1_client_method(void)
    {
        return wolfDTLSv1_client_method_ex(NULL);
    }
    WOLFSSL_METHOD* wolfDTLSv1_client_method_ex(void* heap)
    {
        WOLFSSL_METHOD* method =
                          (WOLFSSL_METHOD*) XMALLOC(sizeof(WOLFSSL_METHOD),
                                                 heap, DYNAMIC_TYPE_METHOD);
        (void)heap;
        WOLFSSL_ENTER("DTLSv1_client_method_ex");
        if (method)
            InitSSL_Method(method, MakeDTLSv1());
        return method;
    }
    #endif  /* NO_OLD_TLS */

    #ifndef WOLFSSL_NO_TLS12
    WOLFSSL_METHOD* wolfDTLSv1_2_client_method(void)
    {
        return wolfDTLSv1_2_client_method_ex(NULL);
    }
    WOLFSSL_METHOD* wolfDTLSv1_2_client_method_ex(void* heap)
    {
        WOLFSSL_METHOD* method =
                          (WOLFSSL_METHOD*) XMALLOC(sizeof(WOLFSSL_METHOD),
                                                 heap, DYNAMIC_TYPE_METHOD);
        (void)heap;
        WOLFSSL_ENTER("DTLSv1_2_client_method_ex");
        if (method)
            InitSSL_Method(method, MakeDTLSv1_2());
        (void)heap;
        return method;
    }
    #endif /* !WOLFSSL_NO_TLS12 */
#endif /* WOLFSSL_DTLS */

#endif /* NO_WOLFSSL_CLIENT */


/* EITHER SIDE METHODS */
#if defined(OPENSSL_EXTRA) || defined(WOLFSSL_EITHER_SIDE)
    #ifndef NO_OLD_TLS
    #ifdef WOLFSSL_ALLOW_TLSV10
    /* Gets a WOLFSSL_METHOD type that is not set as client or server
     *
     * Returns a pointer to a WOLFSSL_METHOD struct
     */
    WOLFSSL_METHOD* wolfTLSv1_method(void)
    {
        return wolfTLSv1_method_ex(NULL);
    }
    WOLFSSL_METHOD* wolfTLSv1_method_ex(void* heap)
    {
        WOLFSSL_METHOD* m;
        WOLFSSL_ENTER("TLSv1_method");
    #ifndef NO_WOLFSSL_CLIENT
        m = wolfTLSv1_client_method_ex(heap);
    #else
        m = wolfTLSv1_server_method_ex(heap);
    #endif
        if (m != NULL) {
            m->side = WOLFSSL_NEITHER_END;
        }

        return m;
    }
    #endif /* WOLFSSL_ALLOW_TLSV10 */

    /* Gets a WOLFSSL_METHOD type that is not set as client or server
     *
     * Returns a pointer to a WOLFSSL_METHOD struct
     */
    WOLFSSL_METHOD* wolfTLSv1_1_method(void)
    {
        return wolfTLSv1_1_method_ex(NULL);
    }
    WOLFSSL_METHOD* wolfTLSv1_1_method_ex(void* heap)
    {
        WOLFSSL_METHOD* m;
        WOLFSSL_ENTER("TLSv1_1_method");
    #ifndef NO_WOLFSSL_CLIENT
        m = wolfTLSv1_1_client_method_ex(heap);
    #else
        m = wolfTLSv1_1_server_method_ex(heap);
    #endif
        if (m != NULL) {
            m->side = WOLFSSL_NEITHER_END;
        }
        return m;
    }
    #endif /* !NO_OLD_TLS */

    #ifndef WOLFSSL_NO_TLS12
    /* Gets a WOLFSSL_METHOD type that is not set as client or server
     *
     * Returns a pointer to a WOLFSSL_METHOD struct
     */
    WOLFSSL_METHOD* wolfTLSv1_2_method(void)
    {
        return wolfTLSv1_2_method_ex(NULL);
    }
    WOLFSSL_METHOD* wolfTLSv1_2_method_ex(void* heap)
    {
        WOLFSSL_METHOD* m;
        WOLFSSL_ENTER("TLSv1_2_method");
    #ifndef NO_WOLFSSL_CLIENT
        m = wolfTLSv1_2_client_method_ex(heap);
    #else
        m = wolfTLSv1_2_server_method_ex(heap);
    #endif
        if (m != NULL) {
            m->side = WOLFSSL_NEITHER_END;
        }
        return m;
    }
    #endif /* !WOLFSSL_NO_TLS12 */

    #ifdef WOLFSSL_TLS13
    /* Gets a WOLFSSL_METHOD type that is not set as client or server
     *
     * Returns a pointer to a WOLFSSL_METHOD struct
     */
    WOLFSSL_METHOD* wolfTLSv1_3_method(void)
    {
        return wolfTLSv1_3_method_ex(NULL);
    }
    WOLFSSL_METHOD* wolfTLSv1_3_method_ex(void* heap)
    {
        WOLFSSL_METHOD* m;
        WOLFSSL_ENTER("TLSv1_3_method");
    #ifndef NO_WOLFSSL_CLIENT
        m = wolfTLSv1_3_client_method_ex(heap);
    #else
        m = wolfTLSv1_3_server_method_ex(heap);
    #endif
        if (m != NULL) {
            m->side = WOLFSSL_NEITHER_END;
        }
        return m;
    }
    #endif /* WOLFSSL_TLS13 */

#ifdef WOLFSSL_DTLS
    WOLFSSL_METHOD* wolfDTLS_method(void)
    {
        return wolfDTLS_method_ex(NULL);
    }
    WOLFSSL_METHOD* wolfDTLS_method_ex(void* heap)
    {
        WOLFSSL_METHOD* m;
        WOLFSSL_ENTER("DTLS_method_ex");
    #ifndef NO_WOLFSSL_CLIENT
        m = wolfDTLS_client_method_ex(heap);
    #else
        m = wolfDTLS_server_method_ex(heap);
    #endif
        if (m != NULL) {
            m->side = WOLFSSL_NEITHER_END;
        }
        return m;
    }

    #ifndef NO_OLD_TLS
    WOLFSSL_METHOD* wolfDTLSv1_method(void)
    {
        return wolfDTLSv1_method_ex(NULL);
    }
    WOLFSSL_METHOD* wolfDTLSv1_method_ex(void* heap)
    {
        WOLFSSL_METHOD* m;
        WOLFSSL_ENTER("DTLSv1_method_ex");
    #ifndef NO_WOLFSSL_CLIENT
        m = wolfDTLSv1_client_method_ex(heap);
    #else
        m = wolfDTLSv1_server_method_ex(heap);
    #endif
        if (m != NULL) {
            m->side = WOLFSSL_NEITHER_END;
        }
        return m;
    }
    #endif /* !NO_OLD_TLS */
    #ifndef WOLFSSL_NO_TLS12
    WOLFSSL_METHOD* wolfDTLSv1_2_method(void)
    {
        return wolfDTLSv1_2_method_ex(NULL);
    }
    WOLFSSL_METHOD* wolfDTLSv1_2_method_ex(void* heap)
    {
        WOLFSSL_METHOD* m;
        WOLFSSL_ENTER("DTLSv1_2_method");
    #ifndef NO_WOLFSSL_CLIENT
        m = wolfDTLSv1_2_client_method_ex(heap);
    #else
        m = wolfDTLSv1_2_server_method_ex(heap);
    #endif
        if (m != NULL) {
            m->side = WOLFSSL_NEITHER_END;
        }
        return m;
    }
    #endif /* !WOLFSSL_NO_TLS12 */
#endif /* WOLFSSL_DTLS */
#endif /* OPENSSL_EXTRA || WOLFSSL_EITHER_SIDE */


#ifndef NO_WOLFSSL_SERVER

    WOLFSSL_METHOD* wolfTLS_server_method(void)
    {
        return wolfTLS_server_method_ex(NULL);
    }

    WOLFSSL_METHOD* wolfTLS_server_method_ex(void* heap)
    {
        WOLFSSL_METHOD* method =
                              (WOLFSSL_METHOD*) XMALLOC(sizeof(WOLFSSL_METHOD),
                                                     heap, DYNAMIC_TYPE_METHOD);
        (void)heap;
        WOLFSSL_ENTER("TLS_server_method_ex");
        if (method) {
        #if defined(WOLFSSL_TLS13)
            InitSSL_Method(method, MakeTLSv1_3());
        #elif !defined(WOLFSSL_NO_TLS12)
            InitSSL_Method(method, MakeTLSv1_2());
        #elif !defined(NO_OLD_TLS)
            InitSSL_Method(method, MakeTLSv1_1());
        #elif defined(WOLFSSL_ALLOW_TLSV10)
            InitSSL_Method(method, MakeTLSv1());
        #else
            #error No TLS version enabled!
        #endif

            method->downgrade = 1;
            method->side      = WOLFSSL_SERVER_END;
        }
        return method;
    }

#ifndef NO_OLD_TLS
    #ifdef WOLFSSL_ALLOW_TLSV10
    WOLFSSL_METHOD* wolfTLSv1_server_method(void)
    {
        return wolfTLSv1_server_method_ex(NULL);
    }
    WOLFSSL_METHOD* wolfTLSv1_server_method_ex(void* heap)
    {
        WOLFSSL_METHOD* method =
                              (WOLFSSL_METHOD*) XMALLOC(sizeof(WOLFSSL_METHOD),
                                                     heap, DYNAMIC_TYPE_METHOD);
        (void)heap;
        WOLFSSL_ENTER("TLSv1_server_method_ex");
        if (method) {
            InitSSL_Method(method, MakeTLSv1());
            method->side = WOLFSSL_SERVER_END;
        }
        return method;
    }
    #endif /* WOLFSSL_ALLOW_TLSV10 */

    WOLFSSL_METHOD* wolfTLSv1_1_server_method(void)
    {
        return wolfTLSv1_1_server_method_ex(NULL);
    }
    WOLFSSL_METHOD* wolfTLSv1_1_server_method_ex(void* heap)
    {
        WOLFSSL_METHOD* method =
                              (WOLFSSL_METHOD*) XMALLOC(sizeof(WOLFSSL_METHOD),
                                                     heap, DYNAMIC_TYPE_METHOD);
        (void)heap;
        WOLFSSL_ENTER("TLSv1_1_server_method_ex");
        if (method) {
            InitSSL_Method(method, MakeTLSv1_1());
            method->side = WOLFSSL_SERVER_END;
        }
        return method;
    }
#endif /* !NO_OLD_TLS */


#ifndef WOLFSSL_NO_TLS12
    WOLFSSL_ABI
    WOLFSSL_METHOD* wolfTLSv1_2_server_method(void)
    {
        return wolfTLSv1_2_server_method_ex(NULL);
    }
    WOLFSSL_METHOD* wolfTLSv1_2_server_method_ex(void* heap)
    {
        WOLFSSL_METHOD* method =
                              (WOLFSSL_METHOD*) XMALLOC(sizeof(WOLFSSL_METHOD),
                                                     heap, DYNAMIC_TYPE_METHOD);
        (void)heap;
        WOLFSSL_ENTER("TLSv1_2_server_method_ex");
        if (method) {
            InitSSL_Method(method, MakeTLSv1_2());
            method->side = WOLFSSL_SERVER_END;
        }
        return method;
    }
#endif /* !WOLFSSL_NO_TLS12 */

#ifdef WOLFSSL_TLS13
    /* The TLS v1.3 server method data.
     *
     * returns the method data for a TLS v1.3 server.
     */
    WOLFSSL_ABI
    WOLFSSL_METHOD* wolfTLSv1_3_server_method(void)
    {
        return wolfTLSv1_3_server_method_ex(NULL);
    }

    /* The TLS v1.3 server method data.
     *
     * heap  The heap used for allocation.
     * returns the method data for a TLS v1.3 server.
     */
    WOLFSSL_METHOD* wolfTLSv1_3_server_method_ex(void* heap)
    {
        WOLFSSL_METHOD* method =
                              (WOLFSSL_METHOD*) XMALLOC(sizeof(WOLFSSL_METHOD),
                                                     heap, DYNAMIC_TYPE_METHOD);
        (void)heap;
        WOLFSSL_ENTER("TLSv1_3_server_method_ex");
        if (method) {
            InitSSL_Method(method, MakeTLSv1_3());
            method->side = WOLFSSL_SERVER_END;
        }
        return method;
    }
#endif /* WOLFSSL_TLS13 */

#ifdef WOLFSSL_DTLS
    WOLFSSL_METHOD* wolfDTLS_server_method(void)
    {
        return wolfDTLS_server_method_ex(NULL);
    }
    WOLFSSL_METHOD* wolfDTLS_server_method_ex(void* heap)
    {
        WOLFSSL_METHOD* method =
                              (WOLFSSL_METHOD*) XMALLOC(sizeof(WOLFSSL_METHOD),
                                                     heap, DYNAMIC_TYPE_METHOD);
        (void)heap;
        WOLFSSL_ENTER("DTLS_server_method_ex");
        if (method) {
        #if defined(WOLFSSL_DTLS13)
            InitSSL_Method(method, MakeDTLSv1_3());
        #elif !defined(WOLFSSL_NO_TLS12)
            InitSSL_Method(method, MakeDTLSv1_2());
        #elif !defined(NO_OLD_TLS)
            InitSSL_Method(method, MakeDTLSv1());
        #else
            #error No DTLS version enabled!
        #endif

            method->downgrade = 1;
            method->side      = WOLFSSL_SERVER_END;
        }
        return method;
    }

    #ifndef NO_OLD_TLS
    WOLFSSL_METHOD* wolfDTLSv1_server_method(void)
    {
        return wolfDTLSv1_server_method_ex(NULL);
    }
    WOLFSSL_METHOD* wolfDTLSv1_server_method_ex(void* heap)
    {
        WOLFSSL_METHOD* method =
                          (WOLFSSL_METHOD*) XMALLOC(sizeof(WOLFSSL_METHOD),
                                                 heap, DYNAMIC_TYPE_METHOD);
        (void)heap;
        WOLFSSL_ENTER("DTLSv1_server_method_ex");
        if (method) {
            InitSSL_Method(method, MakeDTLSv1());
            method->side = WOLFSSL_SERVER_END;
        }
        return method;
    }
    #endif /* !NO_OLD_TLS */

    #ifndef WOLFSSL_NO_TLS12
    WOLFSSL_METHOD* wolfDTLSv1_2_server_method(void)
    {
        return wolfDTLSv1_2_server_method_ex(NULL);
    }
    WOLFSSL_METHOD* wolfDTLSv1_2_server_method_ex(void* heap)
    {
        WOLFSSL_METHOD* method =
                          (WOLFSSL_METHOD*) XMALLOC(sizeof(WOLFSSL_METHOD),
                                                 heap, DYNAMIC_TYPE_METHOD);
        WOLFSSL_ENTER("DTLSv1_2_server_method_ex");
        (void)heap;
        if (method) {
            InitSSL_Method(method, MakeDTLSv1_2());
            method->side = WOLFSSL_SERVER_END;
        }
        (void)heap;
        return method;
    }
    #endif /* !WOLFSSL_NO_TLS12 */
#endif /* WOLFSSL_DTLS */

#endif /* NO_WOLFSSL_SERVER */

#endif /* NO_TLS */
#endif /* WOLFCRYPT_ONLY */
