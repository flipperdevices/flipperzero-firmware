/* pwdbased.c
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

#ifndef NO_PWDBASED

#include <wolfssl/wolfcrypt/pwdbased.h>
#include <wolfssl/wolfcrypt/hmac.h>
#include <wolfssl/wolfcrypt/hash.h>
#include <wolfssl/wolfcrypt/wolfmath.h>
#include <wolfssl/wolfcrypt/error-crypt.h>

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif


#ifdef HAVE_PBKDF1

/* PKCS#5 v1.5 with non standard extension to optionally derive the extra data (IV) */
int wc_PBKDF1_ex(byte* key, int keyLen, byte* iv, int ivLen,
    const byte* passwd, int passwdLen, const byte* salt, int saltLen,
    int iterations, int hashType, void* heap)
{
    int  err;
    int  keyLeft, ivLeft, i;
    int  store;
    int  keyOutput = 0;
    int  digestLen;
    byte digest[WC_MAX_DIGEST_SIZE];
#ifdef WOLFSSL_SMALL_STACK
    wc_HashAlg* hash = NULL;
#else
    wc_HashAlg  hash[1];
#endif
    enum wc_HashType hashT;

    (void)heap;

    if (key == NULL || keyLen < 0 || passwdLen < 0 || saltLen < 0 || ivLen < 0){
        return BAD_FUNC_ARG;
    }

    if (iterations <= 0)
        iterations = 1;

    hashT = wc_HashTypeConvert(hashType);
    err = wc_HashGetDigestSize(hashT);
    if (err < 0)
        return err;
    digestLen = err;

    /* initialize hash */
#ifdef WOLFSSL_SMALL_STACK
    hash = (wc_HashAlg*)XMALLOC(sizeof(wc_HashAlg), heap,
                                DYNAMIC_TYPE_HASHCTX);
    if (hash == NULL)
        return MEMORY_E;
#endif

    err = wc_HashInit_ex(hash, hashT, heap, INVALID_DEVID);
    if (err != 0) {
    #ifdef WOLFSSL_SMALL_STACK
        XFREE(hash, heap, DYNAMIC_TYPE_HASHCTX);
    #endif
        return err;
    }

    keyLeft = keyLen;
    ivLeft  = ivLen;
    while (keyOutput < (keyLen + ivLen)) {
        int digestLeft = digestLen;
        /* D_(i - 1) */
        if (keyOutput) { /* first time D_0 is empty */
            err = wc_HashUpdate(hash, hashT, digest, (word32)digestLen);
            if (err != 0) break;
        }

        /* data */
        err = wc_HashUpdate(hash, hashT, passwd, (word32)passwdLen);
        if (err != 0) break;

        /* salt */
        if (salt) {
            err = wc_HashUpdate(hash, hashT, salt, (word32)saltLen);
            if (err != 0) break;
        }

        err = wc_HashFinal(hash, hashT, digest);
        if (err != 0) break;

        /* count */
        for (i = 1; i < iterations; i++) {
            err = wc_HashUpdate(hash, hashT, digest, (word32)digestLen);
            if (err != 0) break;

            err = wc_HashFinal(hash, hashT, digest);
            if (err != 0) break;
        }

        if (err != 0) break;

        if (keyLeft) {
            store = (int)min((word32)keyLeft, (word32)digestLen);
            XMEMCPY(&key[keyLen - keyLeft], digest, (size_t)store);

            keyOutput  += store;
            keyLeft    -= store;
            digestLeft -= store;
        }

        if (ivLeft && digestLeft) {
            store = (int)min((word32)ivLeft, (word32)digestLeft);
            if (iv != NULL)
                XMEMCPY(&iv[ivLen - ivLeft],
                        &digest[digestLen - digestLeft], (size_t)store);
            keyOutput += store;
            ivLeft    -= store;
        }
    }

    wc_HashFree(hash, hashT);

#ifdef WOLFSSL_SMALL_STACK
    XFREE(hash, heap, DYNAMIC_TYPE_HASHCTX);
#endif

    if (err != 0)
        return err;

    if (keyOutput != (keyLen + ivLen))
        return BUFFER_E;

    return err;
}

/* PKCS#5 v1.5 */
int wc_PBKDF1(byte* output, const byte* passwd, int pLen, const byte* salt,
           int sLen, int iterations, int kLen, int hashType)
{
    return wc_PBKDF1_ex(output, kLen, NULL, 0,
        passwd, pLen, salt, sLen, iterations, hashType, NULL);
}

#endif /* HAVE_PKCS5 */

#if defined(HAVE_PBKDF2) && !defined(NO_HMAC)

int wc_PBKDF2_ex(byte* output, const byte* passwd, int pLen, const byte* salt,
           int sLen, int iterations, int kLen, int hashType, void* heap, int devId)
{
    int    hLen;
    int    ret;
#ifdef WOLFSSL_SMALL_STACK
    byte*  buffer;
    Hmac*  hmac;
#else
    byte   buffer[WC_MAX_DIGEST_SIZE];
    Hmac   hmac[1];
#endif
    enum wc_HashType hashT;

    if (output == NULL || pLen < 0 || sLen < 0 || kLen < 0) {
        return BAD_FUNC_ARG;
    }

    if (iterations <= 0)
        iterations = 1;

    hashT = wc_HashTypeConvert(hashType);
    hLen = wc_HashGetDigestSize(hashT);
    if (hLen < 0)
        return BAD_FUNC_ARG;

#ifdef WOLFSSL_SMALL_STACK
    buffer = (byte*)XMALLOC(WC_MAX_DIGEST_SIZE, heap, DYNAMIC_TYPE_TMP_BUFFER);
    if (buffer == NULL)
        return MEMORY_E;
    hmac = (Hmac*)XMALLOC(sizeof(Hmac), heap, DYNAMIC_TYPE_HMAC);
    if (hmac == NULL) {
        XFREE(buffer, heap, DYNAMIC_TYPE_TMP_BUFFER);
        return MEMORY_E;
    }
#endif

    ret = wc_HmacInit(hmac, heap, devId);
    if (ret == 0) {
        word32 i = 1;
        /* use int hashType here, since HMAC FIPS uses the old unique value */
        ret = wc_HmacSetKey(hmac, hashType, passwd, (word32)pLen);

        while (ret == 0 && kLen) {
            int currentLen;
            int j;

            ret = wc_HmacUpdate(hmac, salt, (word32)sLen);
            if (ret != 0)
                break;

            /* encode i */
            for (j = 0; j < 4; j++) {
                byte b = (byte)(i >> ((3-j) * 8));

                ret = wc_HmacUpdate(hmac, &b, 1);
                if (ret != 0)
                    break;
            }

            /* check ret from inside for loop */
            if (ret != 0)
                break;

            ret = wc_HmacFinal(hmac, buffer);
            if (ret != 0)
                break;

            currentLen = (int)min((word32)kLen, (word32)hLen);
            XMEMCPY(output, buffer, (size_t)currentLen);

            for (j = 1; j < iterations; j++) {
                ret = wc_HmacUpdate(hmac, buffer, (word32)hLen);
                if (ret != 0)
                    break;
                ret = wc_HmacFinal(hmac, buffer);
                if (ret != 0)
                    break;
                xorbuf(output, buffer, (word32)currentLen);
            }

            /* check ret from inside for loop */
            if (ret != 0)
                break;

            output += currentLen;
            kLen   -= currentLen;
            i++;
        }
        wc_HmacFree(hmac);
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(buffer, heap, DYNAMIC_TYPE_TMP_BUFFER);
    XFREE(hmac, heap, DYNAMIC_TYPE_HMAC);
#endif

    return ret;
}

int wc_PBKDF2(byte* output, const byte* passwd, int pLen, const byte* salt,
           int sLen, int iterations, int kLen, int hashType)
{
    return wc_PBKDF2_ex(output, passwd, pLen, salt, sLen, iterations, kLen,
        hashType, NULL, INVALID_DEVID);
}

#endif /* HAVE_PBKDF2 && !NO_HMAC */

#ifdef HAVE_PKCS12

/* helper for PKCS12_PBKDF(), does hash operation */
static int DoPKCS12Hash(int hashType, byte* buffer, word32 totalLen,
                 byte* Ai, word32 u, int iterations)
{
    int i;
    int ret = 0;
#ifdef WOLFSSL_SMALL_STACK
    wc_HashAlg* hash = NULL;
#else
    wc_HashAlg  hash[1];
#endif
    enum wc_HashType hashT;

    if (buffer == NULL || Ai == NULL) {
        return BAD_FUNC_ARG;
    }

    hashT = wc_HashTypeConvert(hashType);

    /* initialize hash */
#ifdef WOLFSSL_SMALL_STACK
    hash = (wc_HashAlg*)XMALLOC(sizeof(wc_HashAlg), NULL,
                                DYNAMIC_TYPE_HASHCTX);
    if (hash == NULL)
        return MEMORY_E;
#endif

    ret = wc_HashInit(hash, hashT);
    if (ret != 0) {
    #ifdef WOLFSSL_SMALL_STACK
        XFREE(hash, NULL, DYNAMIC_TYPE_HASHCTX);
    #endif
        return ret;
    }

    ret = wc_HashUpdate(hash, hashT, buffer, totalLen);

    if (ret == 0)
        ret = wc_HashFinal(hash, hashT, Ai);

    for (i = 1; i < iterations; i++) {
        if (ret == 0)
            ret = wc_HashUpdate(hash, hashT, Ai, u);
        if (ret == 0)
            ret = wc_HashFinal(hash, hashT, Ai);
    }

    wc_HashFree(hash, hashT);

#ifdef WOLFSSL_SMALL_STACK
    XFREE(hash, NULL, DYNAMIC_TYPE_HASHCTX);
#endif

    return ret;
}


int wc_PKCS12_PBKDF(byte* output, const byte* passwd, int passLen,
    const byte* salt, int saltLen, int iterations, int kLen, int hashType,
    int id)
{
    return wc_PKCS12_PBKDF_ex(output, passwd, passLen, salt, saltLen,
                              iterations, kLen, hashType, id, NULL);
}


/* extended API that allows a heap hint to be used */
int wc_PKCS12_PBKDF_ex(byte* output, const byte* passwd, int passLen,
                       const byte* salt, int saltLen, int iterations, int kLen,
                       int hashType, int id, void* heap)
{
    /* all in bytes instead of bits */
    word32 u, v, dLen, pLen, iLen, sLen, totalLen;
    int    dynamic = 0;
    int    ret = 0;
    word32 i;
    byte   *D, *S, *P, *I;
#ifdef WOLFSSL_SMALL_STACK
    byte   staticBuffer[1]; /* force dynamic usage */
#else
    byte   staticBuffer[1024];
#endif
    byte*  buffer = staticBuffer;

#ifdef WOLFSSL_SMALL_STACK
    byte*  Ai = NULL;
    byte*  B = NULL;
    mp_int *B1 = NULL;
    mp_int *i1 = NULL;
    mp_int *res = NULL;
#else
    byte   Ai[WC_MAX_DIGEST_SIZE];
    byte   B[WC_MAX_BLOCK_SIZE];
    mp_int B1[1];
    mp_int i1[1];
    mp_int res[1];
#endif
    enum wc_HashType hashT;

    (void)heap;

    if (output == NULL || passLen <= 0 || saltLen <= 0 || kLen < 0) {
        return BAD_FUNC_ARG;
    }

    if (iterations <= 0)
        iterations = 1;

    hashT = wc_HashTypeConvert(hashType);
    ret = wc_HashGetDigestSize(hashT);
    if (ret < 0)
        return ret;
    if (ret == 0)
        return BAD_STATE_E;
    u = (word32)ret;

    ret = wc_HashGetBlockSize(hashT);
    if (ret < 0)
        return ret;
    if (ret == 0)
        return BAD_STATE_E;
    v = (word32)ret;

#ifdef WOLFSSL_SMALL_STACK
    Ai = (byte*)XMALLOC(WC_MAX_DIGEST_SIZE, heap, DYNAMIC_TYPE_TMP_BUFFER);
    if (Ai == NULL)
        return MEMORY_E;

    B = (byte*)XMALLOC(WC_MAX_BLOCK_SIZE, heap, DYNAMIC_TYPE_TMP_BUFFER);
    if (B == NULL) {
        XFREE(Ai, heap, DYNAMIC_TYPE_TMP_BUFFER);
        return MEMORY_E;
    }
#endif

    XMEMSET(Ai, 0, WC_MAX_DIGEST_SIZE);
    XMEMSET(B,  0, WC_MAX_BLOCK_SIZE);

    dLen = v;
    sLen = v * (((word32)saltLen + v - 1) / v);

    /* with passLen checked at the top of the function for >= 0 then passLen
     * must be 1 or greater here and is always 'true' */
    pLen = v * (((word32)passLen + v - 1) / v);
    iLen = sLen + pLen;

    totalLen = dLen + sLen + pLen;

    if (totalLen > sizeof(staticBuffer)) {
        buffer = (byte*)XMALLOC(totalLen, heap, DYNAMIC_TYPE_KEY);
        if (buffer == NULL) {
#ifdef WOLFSSL_SMALL_STACK
            XFREE(Ai, heap, DYNAMIC_TYPE_TMP_BUFFER);
            XFREE(B,  heap, DYNAMIC_TYPE_TMP_BUFFER);
#endif
            return MEMORY_E;
        }
        dynamic = 1;
    }

    D = buffer;
    S = D + dLen;
    P = S + sLen;
    I = S;

    XMEMSET(D, id, dLen);

    for (i = 0; i < sLen; i++)
        S[i] = salt[i % (word32)saltLen];
    for (i = 0; i < pLen; i++)
        P[i] = passwd[i % (word32)passLen];

#ifdef WOLFSSL_SMALL_STACK
    if (((B1 = (mp_int *)XMALLOC(sizeof(*B1), heap, DYNAMIC_TYPE_TMP_BUFFER))
         == NULL) ||
        ((i1 = (mp_int *)XMALLOC(sizeof(*i1), heap, DYNAMIC_TYPE_TMP_BUFFER))
         == NULL) ||
        ((res = (mp_int *)XMALLOC(sizeof(*res), heap, DYNAMIC_TYPE_TMP_BUFFER))
         == NULL)) {
        ret = MEMORY_E;
        goto out;
    }
#endif

    while (kLen > 0) {
        word32 currentLen;

        ret = DoPKCS12Hash(hashType, buffer, totalLen, Ai, u, iterations);
        if (ret < 0)
            break;

        for (i = 0; i < v; i++)
            B[i] = Ai[(word32)i % u];

        if (mp_init(B1) != MP_OKAY)
            ret = MP_INIT_E;
        else if (mp_read_unsigned_bin(B1, B, v) != MP_OKAY)
            ret = MP_READ_E;
        else if (mp_add_d(B1, (mp_digit)1, B1) != MP_OKAY)
            ret = MP_ADD_E;

        if (ret != 0) {
            mp_clear(B1);
            break;
        }

        for (i = 0; i < iLen; i += v) {
            int    outSz;

            if (mp_init_multi(i1, res, NULL, NULL, NULL, NULL) != MP_OKAY) {
                ret = MP_INIT_E;
                break;
            }
            if (mp_read_unsigned_bin(i1, I + i, v) != MP_OKAY)
                ret = MP_READ_E;
            else if (mp_add(i1, B1, res) != MP_OKAY)
                ret = MP_ADD_E;
            else if ( (outSz = mp_unsigned_bin_size(res)) < 0)
                ret = MP_TO_E;
            else {
                if (outSz > (int)v) {
                    /* take off MSB */
                    byte  tmp[WC_MAX_BLOCK_SIZE + 1];
                    ret = mp_to_unsigned_bin(res, tmp);
                    XMEMCPY(I + i, tmp + 1, v);
                }
                else if (outSz < (int)v) {
                    XMEMSET(I + i, 0, v - (word32)outSz);
                    ret = mp_to_unsigned_bin(res, I + i + v - (word32)outSz);
                }
                else
                    ret = mp_to_unsigned_bin(res, I + i);
            }

            mp_clear(i1);
            mp_clear(res);
            if (ret < 0) break;
        }

        if (ret < 0) {
            mp_clear(B1);
            break;
        }

        currentLen = min((word32)kLen, u);
        XMEMCPY(output, Ai, currentLen);
        output += currentLen;
        kLen   -= (int)currentLen;
        mp_clear(B1);
    }

#ifdef WOLFSSL_SMALL_STACK
  out:

    if (Ai != NULL)
        XFREE(Ai, heap, DYNAMIC_TYPE_TMP_BUFFER);
    if (B != NULL)
        XFREE(B,  heap, DYNAMIC_TYPE_TMP_BUFFER);
    if (B1 != NULL)
        XFREE(B1, heap, DYNAMIC_TYPE_TMP_BUFFER);
    if (i1 != NULL)
        XFREE(i1, heap, DYNAMIC_TYPE_TMP_BUFFER);
    if (res != NULL)
        XFREE(res, heap, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    if (dynamic)
        XFREE(buffer, heap, DYNAMIC_TYPE_KEY);

    return ret;
}

#endif /* HAVE_PKCS12 */

#ifdef HAVE_SCRYPT
/* Rotate the 32-bit value a by b bits to the left.
 *
 * a  32-bit value.
 * b  Number of bits to rotate.
 * returns rotated value.
 */
#define R(a, b) rotlFixed(a, b)

/* (2^32 - 1) */
#define SCRYPT_WORD32_MAX 4294967295U

/* One round of Salsa20/8.
 * Code taken from RFC 7914: scrypt PBKDF.
 *
 * out  Output buffer.
 * in   Input data to hash.
 */
static void scryptSalsa(word32* out, word32* in)
{
    int    i;
    word32 x[16];

#ifdef LITTLE_ENDIAN_ORDER
    XMEMCPY(x, in, sizeof(x));
#else
    for (i = 0; i < 16; i++)
        x[i] = ByteReverseWord32(in[i]);
#endif
    for (i = 8; i > 0; i -= 2) {
        x[ 4] ^= R(x[ 0] + x[12],  7);  x[ 8] ^= R(x[ 4] + x[ 0],  9);
        x[12] ^= R(x[ 8] + x[ 4], 13);  x[ 0] ^= R(x[12] + x[ 8], 18);
        x[ 9] ^= R(x[ 5] + x[ 1],  7);  x[13] ^= R(x[ 9] + x[ 5],  9);
        x[ 1] ^= R(x[13] + x[ 9], 13);  x[ 5] ^= R(x[ 1] + x[13], 18);
        x[14] ^= R(x[10] + x[ 6],  7);  x[ 2] ^= R(x[14] + x[10],  9);
        x[ 6] ^= R(x[ 2] + x[14], 13);  x[10] ^= R(x[ 6] + x[ 2], 18);
        x[ 3] ^= R(x[15] + x[11],  7);  x[ 7] ^= R(x[ 3] + x[15],  9);
        x[11] ^= R(x[ 7] + x[ 3], 13);  x[15] ^= R(x[11] + x[ 7], 18);
        x[ 1] ^= R(x[ 0] + x[ 3],  7);  x[ 2] ^= R(x[ 1] + x[ 0],  9);
        x[ 3] ^= R(x[ 2] + x[ 1], 13);  x[ 0] ^= R(x[ 3] + x[ 2], 18);
        x[ 6] ^= R(x[ 5] + x[ 4],  7);  x[ 7] ^= R(x[ 6] + x[ 5],  9);
        x[ 4] ^= R(x[ 7] + x[ 6], 13);  x[ 5] ^= R(x[ 4] + x[ 7], 18);
        x[11] ^= R(x[10] + x[ 9],  7);  x[ 8] ^= R(x[11] + x[10],  9);
        x[ 9] ^= R(x[ 8] + x[11], 13);  x[10] ^= R(x[ 9] + x[ 8], 18);
        x[12] ^= R(x[15] + x[14],  7);  x[13] ^= R(x[12] + x[15],  9);
        x[14] ^= R(x[13] + x[12], 13);  x[15] ^= R(x[14] + x[13], 18);
    }
#ifdef LITTLE_ENDIAN_ORDER
    for (i = 0; i < 16; ++i)
        out[i] = in[i] + x[i];
#else
    for (i = 0; i < 16; i++)
        out[i] = ByteReverseWord32(ByteReverseWord32(in[i]) + x[i]);
#endif
}

/* Mix a block using Salsa20/8.
 * Based on RFC 7914: scrypt PBKDF.
 *
 * b  Blocks to mix.
 * y  Temporary storage.
 * r  Size of the block.
 */
static void scryptBlockMix(byte* b, byte* y, int r)
{
#ifdef WORD64_AVAILABLE
    word64  x[8];
    word64* b64 = (word64*)b;
    word64* y64 = (word64*)y;
#else
    word32  x[16];
    word32* b32 = (word32*)b;
    word32* y32 = (word32*)y;
#endif
    int  i;
    int  j;

    /* Step 1. */
    XMEMCPY(x, b + (2 * r - 1) * 64, sizeof(x));
    /* Step 2. */
    for (i = 0; i < 2 * r; i++)
    {
#ifdef WORD64_AVAILABLE
        for (j = 0; j < 8; j++)
            x[j] ^= b64[i * 8 + j];

#else
        for (j = 0; j < 16; j++)
            x[j] ^= b32[i * 16 + j];
#endif
        scryptSalsa((word32*)x, (word32*)x);
        XMEMCPY(y + i * 64, x, sizeof(x));
    }
    /* Step 3. */
    for (i = 0; i < r; i++) {
#ifdef WORD64_AVAILABLE
        for (j = 0; j < 8; j++) {
            b64[i * 8 + j] = y64[2 * i * 8 + j];
            b64[(r + i) * 8 + j] = y64[(2 * i + 1) * 8 + j];
        }
#else
        for (j = 0; j < 16; j++) {
            b32[i * 16 + j] = y32[2 * i * 16 + j];
            b32[(r + i) * 16 + j] = y32[(2 * i + 1) * 16 + j];
        }
#endif
    }
}

/* Random oracles mix.
 * Based on RFC 7914: scrypt PBKDF.
 *
 * x  Data to mix.
 * v  Temporary buffer.
 * y  Temporary buffer for the block mix.
 * r  Block size parameter.
 * n  CPU/Memory cost parameter.
 */
static void scryptROMix(byte* x, byte* v, byte* y, int r, word32 n)
{
    word32 i;
    word32 j;
    word32 k;
    word32 bSz = (word32)(128 * r);
#ifdef WORD64_AVAILABLE
    word64* x64 = (word64*)x;
    word64* v64 = (word64*)v;
#else
    word32* x32 = (word32*)x;
    word32* v32 = (word32*)v;
#endif

    /* Step 1. X = B (B not needed therefore not implemented) */
    /* Step 2. */
    for (i = 0; i < n; i++)
    {
        XMEMCPY(v + i * bSz, x, bSz);
        scryptBlockMix(x, y, r);
    }

    /* Step 3. */
    for (i = 0; i < n; i++)
    {
#ifdef LITTLE_ENDIAN_ORDER
#ifdef WORD64_AVAILABLE
        j = (word32)(*(word64*)(x + (2*r - 1) * 64) & (n-1));
#else
        j = *(word32*)(x + (2*r - 1) * 64) & (n-1);
#endif
#else
        byte* t = x + (2*r - 1) * 64;
        j = (t[0] | (t[1] << 8) | (t[2] << 16) | ((word32)t[3] << 24)) & (n-1);
#endif
#ifdef WORD64_AVAILABLE
        for (k = 0; k < bSz / 8; k++)
            x64[k] ^= v64[j * bSz / 8 + k];
#else
        for (k = 0; k < bSz / 4; k++)
            x32[k] ^= v32[j * bSz / 4 + k];
#endif
        scryptBlockMix(x, y, r);
    }
    /* Step 4. B' = X (B = X = B' so not needed, therefore not implemented) */
}

/* Generates an key derived from a password and salt using a memory hard
 * algorithm.
 * Implements RFC 7914: scrypt PBKDF.
 *
 * output     The derived key.
 * passwd     The password to derive key from.
 * passLen    The length of the password.
 * salt       The key specific data.
 * saltLen    The length of the salt data.
 * cost       The CPU/memory cost parameter. Range: 1..(128*r/8-1)
 *            (Iterations = 2^cost)
 * blockSize  The number of 128 byte octets in a working block.
 * parallel   The number of parallel mix operations to perform.
 *            (Note: this implementation does not use threads.)
 * dkLen      The length of the derived key in bytes.
 * returns BAD_FUNC_ARG when: blockSize is too large for cost.
 */
int wc_scrypt(byte* output, const byte* passwd, int passLen,
              const byte* salt, int saltLen, int cost, int blockSize,
              int parallel, int dkLen)
{
    int    ret = 0;
    int    i;
    byte*  v = NULL;
    byte*  y = NULL;
    byte*  blocks = NULL;
    word32 blocksSz;
    word32 bSz;

    if (blockSize > 8)
        return BAD_FUNC_ARG;

    if (cost < 1 || cost >= 128 * blockSize / 8 || parallel < 1 || dkLen < 1)
        return BAD_FUNC_ARG;

    /* The following comparison used to be:
     *    ((word32)parallel > (SCRYPT_MAX / (128 * blockSize)))
     * where SCRYPT_MAX is (2^32 - 1) * 32. For some compilers, the RHS of
     * the comparison is greater than parallel's type. It wouldn't promote
     * both sides to word64. What follows is just arithmetic simplification.
     */
    if (parallel > (int)((SCRYPT_WORD32_MAX / 4) / (word32)blockSize))
        return BAD_FUNC_ARG;

    bSz = 128 * (word32)blockSize;
    if (parallel > (int)(SCRYPT_WORD32_MAX / bSz))
        return BAD_FUNC_ARG;
    blocksSz = bSz * (word32)parallel;
    blocks = (byte*)XMALLOC((size_t)blocksSz, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (blocks == NULL) {
        ret = MEMORY_E;
        goto end;
    }
    /* Temporary for scryptROMix. */
    v = (byte*)XMALLOC((size_t)((1 << cost) * bSz), NULL,
                       DYNAMIC_TYPE_TMP_BUFFER);
    if (v == NULL) {
        ret = MEMORY_E;
        goto end;
    }
    /* Temporary for scryptBlockMix. */
    y = (byte*)XMALLOC((size_t)(blockSize * 128), NULL,
                       DYNAMIC_TYPE_TMP_BUFFER);
    if (y == NULL) {
        ret = MEMORY_E;
        goto end;
    }

    /* Step 1. */
    ret = wc_PBKDF2(blocks, passwd, passLen, salt, saltLen, 1, (int)blocksSz,
                    WC_SHA256);
    if (ret != 0)
        goto end;

    /* Step 2. */
    for (i = 0; i < parallel; i++)
        scryptROMix(blocks + i * (int)bSz, v, y, (int)blockSize, 1 << cost);

    /* Step 3. */
    ret = wc_PBKDF2(output, passwd, passLen, blocks, (int)blocksSz, 1, dkLen,
                    WC_SHA256);
end:
    if (blocks != NULL)
        XFREE(blocks, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (v != NULL)
        XFREE(v, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (y != NULL)
        XFREE(y, NULL, DYNAMIC_TYPE_TMP_BUFFER);

    return ret;
}

/* Generates an key derived from a password and salt using a memory hard
 * algorithm.
 * Implements RFC 7914: scrypt PBKDF.
 *
 * output      Derived key.
 * passwd      Password to derive key from.
 * passLen     Length of the password.
 * salt        Key specific data.
 * saltLen     Length of the salt data.
 * iterations  Number of iterations to perform. Range: 1 << (1..(128*r/8-1))
 * blockSize   Number of 128 byte octets in a working block.
 * parallel    Number of parallel mix operations to perform.
 *             (Note: this implementation does not use threads.)
 * dkLen       Length of the derived key in bytes.
 * returns BAD_FUNC_ARG when: iterations is not a power of 2 or blockSize is too
 *                            large for iterations.
 */
int wc_scrypt_ex(byte* output, const byte* passwd, int passLen,
                 const byte* salt, int saltLen, word32 iterations,
                 int blockSize, int parallel, int dkLen)
{
    int cost;

    /* Iterations must be a power of 2. */
    if ((iterations & (iterations - 1)) != 0)
        return BAD_FUNC_ARG;

    for (cost = -1; iterations != 0; cost++) {
        iterations >>= 1;
    }

    return wc_scrypt(output, passwd, passLen, salt, saltLen, cost, blockSize,
                     parallel, dkLen);
}
#endif /* HAVE_SCRYPT */

#endif /* NO_PWDBASED */
