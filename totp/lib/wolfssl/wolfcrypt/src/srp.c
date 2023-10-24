/* srp.c
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

#ifdef WOLFCRYPT_HAVE_SRP

#include <wolfssl/wolfcrypt/srp.h>
#include <wolfssl/wolfcrypt/random.h>
#include <wolfssl/wolfcrypt/error-crypt.h>

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

/** Computes the session key using the Mask Generation Function 1. */
static int wc_SrpSetKey(Srp* srp, byte* secret, word32 size);

static int SrpHashInit(SrpHash* hash, SrpType type, void* heap)
{
    hash->type = type;

    switch (type) {
        case SRP_TYPE_SHA:
            #ifndef NO_SHA
                return wc_InitSha_ex(&hash->data.sha, heap, INVALID_DEVID);
            #else
                return BAD_FUNC_ARG;
            #endif

        case SRP_TYPE_SHA256:
            #ifndef NO_SHA256
                return wc_InitSha256_ex(&hash->data.sha256, heap, INVALID_DEVID);
            #else
                return BAD_FUNC_ARG;
            #endif

        case SRP_TYPE_SHA384:
            #ifdef WOLFSSL_SHA384
                return wc_InitSha384_ex(&hash->data.sha384, heap, INVALID_DEVID);
            #else
                return BAD_FUNC_ARG;
            #endif

        case SRP_TYPE_SHA512:
            #ifdef WOLFSSL_SHA512
                return wc_InitSha512_ex(&hash->data.sha512, heap, INVALID_DEVID);
            #else
                return BAD_FUNC_ARG;
            #endif

        default:
            return BAD_FUNC_ARG;
    }
}

static int SrpHashUpdate(SrpHash* hash, const byte* data, word32 size)
{
    switch (hash->type) {
        case SRP_TYPE_SHA:
            #ifndef NO_SHA
                return wc_ShaUpdate(&hash->data.sha, data, size);
            #else
                return BAD_FUNC_ARG;
            #endif

        case SRP_TYPE_SHA256:
            #ifndef NO_SHA256
                return wc_Sha256Update(&hash->data.sha256, data, size);
            #else
                return BAD_FUNC_ARG;
            #endif

        case SRP_TYPE_SHA384:
            #ifdef WOLFSSL_SHA384
                return wc_Sha384Update(&hash->data.sha384, data, size);
            #else
                return BAD_FUNC_ARG;
            #endif

        case SRP_TYPE_SHA512:
            #ifdef WOLFSSL_SHA512
                return wc_Sha512Update(&hash->data.sha512, data, size);
            #else
                return BAD_FUNC_ARG;
            #endif

        default:
            return BAD_FUNC_ARG;
    }
}

static int SrpHashFinal(SrpHash* hash, byte* digest)
{
    switch (hash->type) {
        case SRP_TYPE_SHA:
            #ifndef NO_SHA
                return wc_ShaFinal(&hash->data.sha, digest);
            #else
                return BAD_FUNC_ARG;
            #endif

        case SRP_TYPE_SHA256:
            #ifndef NO_SHA256
                return wc_Sha256Final(&hash->data.sha256, digest);
            #else
                return BAD_FUNC_ARG;
            #endif

        case SRP_TYPE_SHA384:
            #ifdef WOLFSSL_SHA384
                return wc_Sha384Final(&hash->data.sha384, digest);
            #else
                return BAD_FUNC_ARG;
            #endif

        case SRP_TYPE_SHA512:
            #ifdef WOLFSSL_SHA512
                return wc_Sha512Final(&hash->data.sha512, digest);
            #else
                return BAD_FUNC_ARG;
            #endif

        default:
            return BAD_FUNC_ARG;
    }
}

static word32 SrpHashSize(SrpType type)
{
    switch (type) {
        case SRP_TYPE_SHA:
            #ifndef NO_SHA
                return WC_SHA_DIGEST_SIZE;
            #else
                return 0;
            #endif

        case SRP_TYPE_SHA256:
            #ifndef NO_SHA256
                return WC_SHA256_DIGEST_SIZE;
            #else
                return 0;
            #endif

        case SRP_TYPE_SHA384:
            #ifdef WOLFSSL_SHA384
                return WC_SHA384_DIGEST_SIZE;
            #else
                return 0;
            #endif

        case SRP_TYPE_SHA512:
            #ifdef WOLFSSL_SHA512
                return WC_SHA512_DIGEST_SIZE;
            #else
                return 0;
            #endif

        default:
            return 0;
    }
}

static void SrpHashFree(SrpHash* hash)
{
    switch (hash->type) {
        case SRP_TYPE_SHA:
        #ifndef NO_SHA
            wc_ShaFree(&hash->data.sha);
        #endif
            break;
        case SRP_TYPE_SHA256:
        #ifndef NO_SHA256
            wc_Sha256Free(&hash->data.sha256);
        #endif
            break;
        case SRP_TYPE_SHA384:
        #ifdef WOLFSSL_SHA384
            wc_Sha384Free(&hash->data.sha384);
        #endif
            break;
        case SRP_TYPE_SHA512:
        #ifdef WOLFSSL_SHA512
            wc_Sha512Free(&hash->data.sha512);
        #endif
            break;
        default:
            break;
    }
}


int wc_SrpInit_ex(Srp* srp, SrpType type, SrpSide side, void* heap, int devId)
{
    int r;

    /* validating params */

    if (!srp)
        return BAD_FUNC_ARG;

    if (side != SRP_CLIENT_SIDE && side != SRP_SERVER_SIDE)
        return BAD_FUNC_ARG;

    switch (type) {
        case SRP_TYPE_SHA:
            #ifdef NO_SHA
                return NOT_COMPILED_IN;
            #else
                break; /* OK */
            #endif

        case SRP_TYPE_SHA256:
            #ifdef NO_SHA256
                return NOT_COMPILED_IN;
            #else
                break; /* OK */
            #endif

        case SRP_TYPE_SHA384:
            #ifndef WOLFSSL_SHA384
                return NOT_COMPILED_IN;
            #else
                break; /* OK */
            #endif

        case SRP_TYPE_SHA512:
            #ifndef WOLFSSL_SHA512
                return NOT_COMPILED_IN;
            #else
                break; /* OK */
            #endif

        default:
            return BAD_FUNC_ARG;
    }

    /* initializing variables */
    XMEMSET(srp, 0, sizeof(Srp));

    if ((r = SrpHashInit(&srp->client_proof, type, srp->heap)) != 0)
        return r;

    if ((r = SrpHashInit(&srp->server_proof, type, srp->heap)) != 0) {
        SrpHashFree(&srp->client_proof);
        return r;
    }
    if ((r = mp_init_multi(&srp->N,    &srp->g, &srp->auth,
                           &srp->priv, 0, 0)) != 0) {
        SrpHashFree(&srp->client_proof);
        SrpHashFree(&srp->server_proof);
        return r;
    }

    srp->side = side;    srp->type   = type;
    srp->salt = NULL;    srp->saltSz = 0;
    srp->user = NULL;    srp->userSz = 0;
    srp->key  = NULL;    srp->keySz  = 0;

    srp->keyGenFunc_cb = wc_SrpSetKey;

    /* default heap hint to NULL or test value */
#ifdef WOLFSSL_HEAP_TEST
    srp->heap = (void*)WOLFSSL_HEAP_TEST;
#else
    srp->heap = heap;
#endif /* WOLFSSL_HEAP_TEST */

    (void)devId; /* future */

    return 0;
}

int wc_SrpInit(Srp* srp, SrpType type, SrpSide side)
{
    return wc_SrpInit_ex(srp, type, side, NULL, INVALID_DEVID);
}

void wc_SrpTerm(Srp* srp)
{
    if (srp) {
        mp_clear(&srp->N);    mp_clear(&srp->g);
        mp_clear(&srp->auth); mp_clear(&srp->priv);
        if (srp->salt) {
            ForceZero(srp->salt, srp->saltSz);
            XFREE(srp->salt, srp->heap, DYNAMIC_TYPE_SRP);
        }
        if (srp->user) {
            ForceZero(srp->user, srp->userSz);
            XFREE(srp->user, srp->heap, DYNAMIC_TYPE_SRP);
        }
        if (srp->key) {
            ForceZero(srp->key, srp->keySz);
            XFREE(srp->key, srp->heap, DYNAMIC_TYPE_SRP);
        }

        SrpHashFree(&srp->client_proof);
        SrpHashFree(&srp->server_proof);
        ForceZero(srp, sizeof(Srp));
    }
}

int wc_SrpSetUsername(Srp* srp, const byte* username, word32 size)
{
    if (!srp || !username)
        return BAD_FUNC_ARG;

    /* +1 for NULL char */
    srp->user = (byte*)XMALLOC(size + 1, srp->heap, DYNAMIC_TYPE_SRP);
    if (srp->user == NULL)
        return MEMORY_E;

    srp->userSz = size;
    XMEMCPY(srp->user, username, srp->userSz);
    srp->user[size] = '\0';

    return 0;
}

int wc_SrpSetParams(Srp* srp, const byte* N,    word32 nSz,
                              const byte* g,    word32 gSz,
                              const byte* salt, word32 saltSz)
{
    SrpHash hash;
    byte digest1[SRP_MAX_DIGEST_SIZE];
    byte digest2[SRP_MAX_DIGEST_SIZE];
    byte pad = 0;
    int r;
    word32 i, j = 0;

    if (!srp || !N || !g || !salt || nSz < gSz)
        return BAD_FUNC_ARG;

    if (!srp->user)
        return SRP_CALL_ORDER_E;

    /* Set N */
    if (mp_read_unsigned_bin(&srp->N, N, nSz) != MP_OKAY)
        return MP_READ_E;

    if (mp_count_bits(&srp->N) < SRP_MODULUS_MIN_BITS)
        return BAD_FUNC_ARG;

    /* Set g */
    if (mp_read_unsigned_bin(&srp->g, g, gSz) != MP_OKAY)
        return MP_READ_E;

    if (mp_cmp(&srp->N, &srp->g) != MP_GT)
        return BAD_FUNC_ARG;

    /* Set salt */
    if (srp->salt) {
        ForceZero(srp->salt, srp->saltSz);
        XFREE(srp->salt, srp->heap, DYNAMIC_TYPE_SRP);
    }

    srp->salt = (byte*)XMALLOC(saltSz, srp->heap, DYNAMIC_TYPE_SRP);
    if (srp->salt == NULL)
        return MEMORY_E;

    XMEMCPY(srp->salt, salt, saltSz);
    srp->saltSz = saltSz;

    /* Set k = H(N, g) */
            r = SrpHashInit(&hash, srp->type, srp->heap);
    if (!r) r = SrpHashUpdate(&hash, (byte*) N, nSz);
    for (i = 0; (word32)i < nSz - gSz; i++) {
        if (!r) r = SrpHashUpdate(&hash, &pad, 1);
    }
    if (!r) r = SrpHashUpdate(&hash, (byte*) g, gSz);
    if (!r) r = SrpHashFinal(&hash, srp->k);
    SrpHashFree(&hash);

    /* update client proof */

    /* digest1 = H(N) */
    if (!r) r = SrpHashInit(&hash, srp->type, srp->heap);
    if (!r) r = SrpHashUpdate(&hash, (byte*) N, nSz);
    if (!r) r = SrpHashFinal(&hash, digest1);
    SrpHashFree(&hash);

    /* digest2 = H(g) */
    if (!r) r = SrpHashInit(&hash, srp->type, srp->heap);
    if (!r) r = SrpHashUpdate(&hash, (byte*) g, gSz);
    if (!r) r = SrpHashFinal(&hash, digest2);
    SrpHashFree(&hash);

    /* digest1 = H(N) ^ H(g) */
    if (r == 0) {
        for (i = 0, j = SrpHashSize(srp->type); i < j; i++)
            digest1[i] ^= digest2[i];
    }

    /* digest2 = H(user) */
    if (!r) r = SrpHashInit(&hash, srp->type, srp->heap);
    if (!r) r = SrpHashUpdate(&hash, srp->user, srp->userSz);
    if (!r) r = SrpHashFinal(&hash, digest2);
    SrpHashFree(&hash);

    /* client proof = H( H(N) ^ H(g) | H(user) | salt) */
    if (!r) r = SrpHashUpdate(&srp->client_proof, digest1, j);
    if (!r) r = SrpHashUpdate(&srp->client_proof, digest2, j);
    if (!r) r = SrpHashUpdate(&srp->client_proof, salt, saltSz);

    return r;
}

int wc_SrpSetPassword(Srp* srp, const byte* password, word32 size)
{
    SrpHash hash;
    byte digest[SRP_MAX_DIGEST_SIZE];
    word32 digestSz;
    int r;

    if (!srp || !password || srp->side != SRP_CLIENT_SIDE)
        return BAD_FUNC_ARG;

    if (!srp->salt)
        return SRP_CALL_ORDER_E;

    digestSz = SrpHashSize(srp->type);

    /* digest = H(username | ':' | password) */
            r = SrpHashInit(&hash, srp->type, srp->heap);
    if (!r) r = SrpHashUpdate(&hash, srp->user, srp->userSz);
    if (!r) r = SrpHashUpdate(&hash, (const byte*) ":", 1);
    if (!r) r = SrpHashUpdate(&hash, password, size);
    if (!r) r = SrpHashFinal(&hash, digest);
    SrpHashFree(&hash);

    /* digest = H(salt | H(username | ':' | password)) */
    if (!r) r = SrpHashInit(&hash, srp->type, srp->heap);
    if (!r) r = SrpHashUpdate(&hash, srp->salt, srp->saltSz);
    if (!r) r = SrpHashUpdate(&hash, digest, digestSz);
    if (!r) r = SrpHashFinal(&hash, digest);
    SrpHashFree(&hash);

    /* Set x (private key) */
    if (!r) r = mp_read_unsigned_bin(&srp->auth, digest, digestSz);

    ForceZero(digest, SRP_MAX_DIGEST_SIZE);

    return r;
}

int wc_SrpGetVerifier(Srp* srp, byte* verifier, word32* size)
{
#ifdef WOLFSSL_SMALL_STACK
    mp_int *v = NULL;
#else
    mp_int v[1];
#endif
    int r;

    if (!srp || !verifier || !size || srp->side != SRP_CLIENT_SIDE)
        return BAD_FUNC_ARG;

    if (mp_iszero(&srp->auth) == MP_YES)
        return SRP_CALL_ORDER_E;

#ifdef WOLFSSL_SMALL_STACK
    if ((v = (mp_int *)XMALLOC(sizeof(*v), srp->heap, DYNAMIC_TYPE_TMP_BUFFER)) == NULL)
        return MEMORY_E;
#endif

    r = mp_init(v);
    if (r != MP_OKAY)
        r = MP_INIT_E;
    /* v = g ^ x % N */
    if (!r) r = mp_exptmod(&srp->g, &srp->auth, &srp->N, v);
    if (!r) r = *size < (word32)mp_unsigned_bin_size(v) ? BUFFER_E : MP_OKAY;
    if (!r) r = mp_to_unsigned_bin(v, verifier);
    if (!r) *size = (word32)mp_unsigned_bin_size(v);

    mp_clear(v);
#ifdef WOLFSSL_SMALL_STACK
    XFREE(v, srp->heap, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return r;
}

int wc_SrpSetVerifier(Srp* srp, const byte* verifier, word32 size)
{
    if (!srp || !verifier || srp->side != SRP_SERVER_SIDE)
        return BAD_FUNC_ARG;

    return mp_read_unsigned_bin(&srp->auth, verifier, size);
}

int wc_SrpSetPrivate(Srp* srp, const byte* priv, word32 size)
{
#ifdef WOLFSSL_SMALL_STACK
    mp_int *p = NULL;
#else
    mp_int p[1];
#endif
    int r;

    if (!srp || !priv || !size)
        return BAD_FUNC_ARG;

    if (mp_iszero(&srp->auth) == MP_YES)
        return SRP_CALL_ORDER_E;

#ifdef WOLFSSL_SMALL_STACK
    if ((p = (mp_int *)XMALLOC(sizeof(*p), srp->heap, DYNAMIC_TYPE_TMP_BUFFER)) == NULL)
        return MEMORY_E;
#endif

    r = mp_init(p);
    if (r != MP_OKAY)
        r = MP_INIT_E;
    if (!r) r = mp_read_unsigned_bin(p, priv, size);
    if (!r) r = mp_mod(p, &srp->N, &srp->priv);
    if (!r) r = mp_iszero(&srp->priv) == MP_YES ? SRP_BAD_KEY_E : 0;

    mp_clear(p);
#ifdef WOLFSSL_SMALL_STACK
    XFREE(p, srp->heap, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return r;
}

/** Generates random data using wolfcrypt RNG. */
static int wc_SrpGenPrivate(Srp* srp, byte* priv, word32 size)
{
    WC_RNG rng;
    int r = wc_InitRng_ex(&rng, srp->heap, INVALID_DEVID);

    if (!r) r = wc_RNG_GenerateBlock(&rng, priv, size);
    if (!r) r = wc_SrpSetPrivate(srp, priv, size);
    if (!r) wc_FreeRng(&rng);

    return r;
}

int wc_SrpGetPublic(Srp* srp, byte* pub, word32* size)
{
#ifdef WOLFSSL_SMALL_STACK
    mp_int *pubkey = NULL;
#else
    mp_int pubkey[1];
#endif
    word32 modulusSz;
    int r;

    if (!srp || !pub || !size)
        return BAD_FUNC_ARG;

    if (mp_iszero(&srp->auth) == MP_YES)
        return SRP_CALL_ORDER_E;

    modulusSz = (word32)mp_unsigned_bin_size(&srp->N);
    if (*size < modulusSz)
        return BUFFER_E;

#ifdef WOLFSSL_SMALL_STACK
    if ((pubkey = (mp_int *)XMALLOC(sizeof(*pubkey), srp->heap, DYNAMIC_TYPE_TMP_BUFFER)) == NULL)
        return MEMORY_E;
#endif
    r = mp_init(pubkey);
    if (r != MP_OKAY)
        r = MP_INIT_E;

    /* priv = random() */
    if (mp_iszero(&srp->priv) == MP_YES)
        if (! r) r = wc_SrpGenPrivate(srp, pub, SRP_PRIVATE_KEY_MIN_BITS / 8);

    /* client side: A = g ^ a % N */
    if (srp->side == SRP_CLIENT_SIDE) {
        if (!r) r = mp_exptmod(&srp->g, &srp->priv, &srp->N, pubkey);

    /* server side: B = (k * v + (g ^ b % N)) % N */
    } else {
        if (! r) {
#ifdef WOLFSSL_SMALL_STACK
            mp_int *i = NULL, *j = NULL;
#else
            mp_int i[1], j[1];
#endif
#ifdef WOLFSSL_SMALL_STACK
            if (((i = (mp_int *)XMALLOC(sizeof(*i), srp->heap, DYNAMIC_TYPE_TMP_BUFFER)) == NULL) ||
                ((j = (mp_int *)XMALLOC(sizeof(*j), srp->heap, DYNAMIC_TYPE_TMP_BUFFER)) == NULL))
                r = MEMORY_E;
            if (!r)
#endif
            {
                r = mp_init_multi(i, j, 0, 0, 0, 0);
            }
            if (!r) r = mp_read_unsigned_bin(i, srp->k,SrpHashSize(srp->type));
            if (!r) r = mp_iszero(i) == MP_YES ? SRP_BAD_KEY_E : 0;
            if (!r) r = mp_exptmod(&srp->g, &srp->priv, &srp->N, pubkey);
            if (!r) r = mp_mulmod(i, &srp->auth, &srp->N, j);
            if (!r) r = mp_add(j, pubkey, i);
            if (!r) r = mp_mod(i, &srp->N, pubkey);
#ifdef WOLFSSL_SMALL_STACK
            if (i != NULL) {
                mp_clear(i);
                XFREE(i, srp->heap, DYNAMIC_TYPE_TMP_BUFFER);
            }
            if (j != NULL) {
                mp_clear(j);
                XFREE(j, srp->heap, DYNAMIC_TYPE_TMP_BUFFER);
            }
#else
            mp_clear(i); mp_clear(j);
#endif
        }
    }

    /* extract public key to buffer */
    XMEMSET(pub, 0, modulusSz);
    if (!r) r = mp_to_unsigned_bin(pubkey, pub);
    if (!r) *size = (word32)mp_unsigned_bin_size(pubkey);

    mp_clear(pubkey);
#ifdef WOLFSSL_SMALL_STACK
    XFREE(pubkey, srp->heap, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return r;
}

static int wc_SrpSetKey(Srp* srp, byte* secret, word32 size)
{
    SrpHash hash;
    byte digest[SRP_MAX_DIGEST_SIZE];
    word32 i, j, digestSz = SrpHashSize(srp->type);
    byte counter[4];
    int r = BAD_FUNC_ARG;

    XMEMSET(digest, 0, SRP_MAX_DIGEST_SIZE);

    srp->key = (byte*)XMALLOC(2 * digestSz, srp->heap, DYNAMIC_TYPE_SRP);
    if (srp->key == NULL)
        return MEMORY_E;

    srp->keySz = 2 * digestSz;

    for (i = j = 0; j < srp->keySz; i++) {
        counter[0] = (byte)(i >> 24);
        counter[1] = (byte)(i >> 16);
        counter[2] = (byte)(i >>  8);
        counter[3] = (byte) i;

        r = SrpHashInit(&hash, srp->type, srp->heap);
        if (!r) r = SrpHashUpdate(&hash, secret, size);
        if (!r) r = SrpHashUpdate(&hash, counter, 4);

        if (!r) {
            if (j + digestSz > srp->keySz) {
                r = SrpHashFinal(&hash, digest);
                XMEMCPY(srp->key + j, digest, srp->keySz - j);
                j = srp->keySz;
            }
            else
            {
                r = SrpHashFinal(&hash, srp->key + j);
                j += digestSz;
            }
        }
        SrpHashFree(&hash);
        if (r)
            break;
    }

    ForceZero(digest, sizeof(digest));
    ForceZero(&hash, sizeof(SrpHash));

    return r;
}

int wc_SrpComputeKey(Srp* srp, byte* clientPubKey, word32 clientPubKeySz,
                               byte* serverPubKey, word32 serverPubKeySz)
{
#ifdef WOLFSSL_SMALL_STACK
    SrpHash *hash = NULL;
    byte *digest = NULL;
    mp_int *u = NULL;
    mp_int *s = NULL;
    mp_int *temp1 = NULL;
    mp_int *temp2 = NULL;
#else
    SrpHash hash[1];
    byte digest[SRP_MAX_DIGEST_SIZE];
    mp_int u[1], s[1], temp1[1], temp2[1];
#endif
    byte *secret = NULL;
    word32 i, secretSz, digestSz;
    byte pad = 0;
    int r;

    /* validating params */

    if (!srp || !clientPubKey || clientPubKeySz == 0
        || !serverPubKey || serverPubKeySz == 0) {
        return BAD_FUNC_ARG;
    }

#ifdef WOLFSSL_SMALL_STACK
    hash = (SrpHash *)XMALLOC(sizeof *hash, srp->heap, DYNAMIC_TYPE_SRP);
    digest = (byte *)XMALLOC(SRP_MAX_DIGEST_SIZE, srp->heap, DYNAMIC_TYPE_SRP);
    u = (mp_int *)XMALLOC(sizeof *u, srp->heap, DYNAMIC_TYPE_SRP);
    s = (mp_int *)XMALLOC(sizeof *s, srp->heap, DYNAMIC_TYPE_SRP);
    temp1 = (mp_int *)XMALLOC(sizeof *temp1, srp->heap, DYNAMIC_TYPE_SRP);
    temp2 = (mp_int *)XMALLOC(sizeof *temp2, srp->heap, DYNAMIC_TYPE_SRP);

    if ((hash == NULL) ||
        (digest == NULL) ||
        (u == NULL) ||
        (s == NULL) ||
        (temp1 == NULL) ||
        (temp2 == NULL)) {
        r = MEMORY_E;
        goto out;
    }
#endif

    if ((mp_init_multi(u, s, temp1, temp2, 0, 0)) != MP_OKAY) {
        r = MP_INIT_E;
        goto out;
    }

    if (mp_iszero(&srp->priv) == MP_YES) {
        r = SRP_CALL_ORDER_E;
        goto out;
    }

    /* initializing variables */

    if ((r = SrpHashInit(hash, srp->type, srp->heap)) != 0)
        goto out;

    digestSz = SrpHashSize(srp->type);
    secretSz = (word32)mp_unsigned_bin_size(&srp->N);

    if ((secretSz < clientPubKeySz) || (secretSz < serverPubKeySz)) {
        r = BAD_FUNC_ARG;
        goto out;
    }

    if ((secret = (byte*)XMALLOC(secretSz, srp->heap, DYNAMIC_TYPE_SRP)) == NULL) {
        r = MEMORY_E;
        goto out;
    }

    /* building u (random scrambling parameter) */

    /* H(A) */
    for (i = 0; i < secretSz - clientPubKeySz; i++) {
        if ((r = SrpHashUpdate(hash, &pad, 1)))
            goto out;
    }

    if ((r = SrpHashUpdate(hash, clientPubKey, clientPubKeySz)))
        goto out;

    /* H(A | B) */
    for (i = 0; i < secretSz - serverPubKeySz; i++) {
        if ((r = SrpHashUpdate(hash, &pad, 1)))
            goto out;
    }
    if ((r = SrpHashUpdate(hash, serverPubKey, serverPubKeySz)))
        goto out;

    /* set u */
    if ((r = SrpHashFinal(hash, digest)))
        goto out;
    if ((r = mp_read_unsigned_bin(u, digest, SrpHashSize(srp->type))))
        goto out;
    SrpHashFree(hash);

    /* building s (secret) */

    if (srp->side == SRP_CLIENT_SIDE) {

        /* temp1 = B - k * v; rejects k == 0, B == 0 and B >= N. */
        if ((r = mp_read_unsigned_bin(temp1, srp->k, digestSz)))
            goto out;
        if (mp_iszero(temp1) == MP_YES) {
            r = SRP_BAD_KEY_E;
            goto out;
        }
        if ((r = mp_exptmod(&srp->g, &srp->auth, &srp->N, temp2)))
            goto out;
        if ((r = mp_mulmod(temp1, temp2, &srp->N, s)))
            goto out;
        if ((r = mp_read_unsigned_bin(temp2, serverPubKey, serverPubKeySz)))
            goto out;
        if (mp_iszero(temp2) == MP_YES) {
            r = SRP_BAD_KEY_E;
            goto out;
        }
        if (mp_cmp(temp2, &srp->N) != MP_LT) {
            r = SRP_BAD_KEY_E;
            goto out;
        }
        if ((r = mp_submod(temp2, s, &srp->N, temp1)))
            goto out;

        /* temp2 = a + u * x */
        if ((r = mp_mulmod(u, &srp->auth, &srp->N, s)))
            goto out;
        if ((r = mp_add(&srp->priv, s, temp2)))
            goto out;

        /* secret = temp1 ^ temp2 % N */
        if ((r = mp_exptmod(temp1, temp2, &srp->N, s)))
            goto out;

    } else if (srp->side == SRP_SERVER_SIDE) {
        /* temp1 = v ^ u % N */
        if ((r = mp_exptmod(&srp->auth, u, &srp->N, temp1)))
            goto out;

        /* temp2 = A * temp1 % N; rejects A == 0, A >= N */
        if ((r = mp_read_unsigned_bin(s, clientPubKey, clientPubKeySz)))
            goto out;
        if (mp_iszero(s) == MP_YES) {
            r = SRP_BAD_KEY_E;
            goto out;
        }
        if (mp_cmp(s, &srp->N) != MP_LT) {
            r = SRP_BAD_KEY_E;
            goto out;
        }
        if ((r = mp_mulmod(s, temp1, &srp->N, temp2)))
            goto out;

        /* rejects A * v ^ u % N >= 1, A * v ^ u % N == -1 % N */
        if ((r = mp_read_unsigned_bin(temp1, (const byte*)"\001", 1)))
            goto out;
        if (mp_cmp(temp2, temp1) != MP_GT) {
            r = SRP_BAD_KEY_E;
            goto out;
        }
        if ((r = mp_sub(&srp->N, temp1, s)))
            goto out;
        if (mp_cmp(temp2, s) == MP_EQ) {
            r = SRP_BAD_KEY_E;
            goto out;
        }

        /* secret = temp2 * b % N */
        if ((r = mp_exptmod(temp2, &srp->priv, &srp->N, s)))
            goto out;
    }

    /* building session key from secret */

    if ((r = mp_to_unsigned_bin(s, secret)))
        goto out;
    if ((r = srp->keyGenFunc_cb(srp, secret, (word32)mp_unsigned_bin_size(s))))
        goto out;

    /* updating client proof = H( H(N) ^ H(g) | H(user) | salt | A | B | K) */

    if ((r = SrpHashUpdate(&srp->client_proof, clientPubKey, clientPubKeySz)))
        goto out;
    if ((r = SrpHashUpdate(&srp->client_proof, serverPubKey, serverPubKeySz)))
        goto out;
    if ((r = SrpHashUpdate(&srp->client_proof, srp->key,     srp->keySz)))
        goto out;

    /* updating server proof = H(A) */

    r = SrpHashUpdate(&srp->server_proof, clientPubKey, clientPubKeySz);

  out:

    if (secret) {
        ForceZero(secret, secretSz);
        XFREE(secret, srp->heap, DYNAMIC_TYPE_SRP);
    }

#ifdef WOLFSSL_SMALL_STACK
    if (hash)
        XFREE(hash, srp->heap, DYNAMIC_TYPE_SRP);
    if (digest)
        XFREE(digest, srp->heap, DYNAMIC_TYPE_SRP);
    if (u) {
        if (r != MP_INIT_E)
            mp_clear(u);
        XFREE(u, srp->heap, DYNAMIC_TYPE_SRP);
    }
    if (s) {
        if (r != MP_INIT_E)
            mp_clear(s);
        XFREE(s, srp->heap, DYNAMIC_TYPE_SRP);
    }
    if (temp1) {
        if (r != MP_INIT_E)
            mp_clear(temp1);
        XFREE(temp1, srp->heap, DYNAMIC_TYPE_SRP);
    }
    if (temp2) {
        if (r != MP_INIT_E)
            mp_clear(temp2);
        XFREE(temp2, srp->heap, DYNAMIC_TYPE_SRP);
    }
#else
    if (r != MP_INIT_E) {
        mp_clear(u);
        mp_clear(s);
        mp_clear(temp1);
        mp_clear(temp2);
    }
#endif

    return r;
}

int wc_SrpGetProof(Srp* srp, byte* proof, word32* size)
{
    int r;

    if (!srp || !proof || !size)
        return BAD_FUNC_ARG;

    if (*size < SrpHashSize(srp->type))
        return BUFFER_E;

    if ((r = SrpHashFinal(srp->side == SRP_CLIENT_SIDE
                          ? &srp->client_proof
                          : &srp->server_proof, proof)) != 0)
        return r;

    *size = SrpHashSize(srp->type);

    if (srp->side == SRP_CLIENT_SIDE) {
        /* server proof = H( A | client proof | K) */
        if (!r) r = SrpHashUpdate(&srp->server_proof, proof, *size);
        if (!r) r = SrpHashUpdate(&srp->server_proof, srp->key, srp->keySz);
    }

    return r;
}

int wc_SrpVerifyPeersProof(Srp* srp, byte* proof, word32 size)
{
    byte digest[SRP_MAX_DIGEST_SIZE];
    int r;

    if (!srp || !proof)
        return BAD_FUNC_ARG;

    if (size != SrpHashSize(srp->type))
        return BUFFER_E;

    r = SrpHashFinal(srp->side == SRP_CLIENT_SIDE ? &srp->server_proof
                                                  : &srp->client_proof, digest);

    if (srp->side == SRP_SERVER_SIDE) {
        /* server proof = H( A | client proof | K) */
        if (!r) r = SrpHashUpdate(&srp->server_proof, proof, size);
        if (!r) r = SrpHashUpdate(&srp->server_proof, srp->key, srp->keySz);
    }

    if (!r && XMEMCMP(proof, digest, size) != 0)
        r = SRP_VERIFY_E;

    return r;
}

#endif /* WOLFCRYPT_HAVE_SRP */
