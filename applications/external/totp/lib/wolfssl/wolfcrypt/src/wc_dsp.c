/* wc_dsp.c
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
#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

#if defined(WOLFSSL_DSP)
#include "remote.h"
#include "rpcmem.h"
static wolfSSL_DSP_Handle_cb handle_function = NULL;
static remote_handle64 defaultHandle;
static wolfSSL_Mutex handle_mutex; /* mutex for access to single default handle */

#define WOLFSSL_HANDLE_DONE 1
#define WOLFSSL_HANDLE_GET 0

/* callback function for setting the default handle in single threaded
 * use cases */
static int default_handle_cb(remote_handle64 *handle, int finished, void *ctx)
{
    (void)ctx;
    if (finished == WOLFSSL_HANDLE_DONE) {
        if (wc_UnLockMutex(&handle_mutex) != 0) {
            WOLFSSL_MSG("Unlock handle mutex failed");
            return -1;
        }
    }
    else {
        if (wc_LockMutex(&handle_mutex) != 0) {
            WOLFSSL_MSG("Lock handle mutex failed");
            return -1;
        }
        *handle = defaultHandle;
    }
    return 0;
}


/* Set global callback for getting handle to use
 * return 0 on success */
int wolfSSL_SetHandleCb(wolfSSL_DSP_Handle_cb in)
{
    handle_function = in;
    return 0;
}


/* returns 1 if global handle callback is set and 0 if not */
int wolfSSL_GetHandleCbSet()
{
    return (handle_function != NULL)? 1: 0;
}


/* Local function for setting up default handle
 * returns 0 on success */
int wolfSSL_InitHandle()
{
    char *sp_URI_value;
    int ret;

    sp_URI_value = wolfSSL_URI "&_dom=adsp";
    ret = wolfSSL_open(sp_URI_value, &defaultHandle);
    if (ret != 0) {
        WOLFSSL_MSG("Unable to open aDSP?");
        return -1;
    }
    wolfSSL_SetHandleCb(default_handle_cb);
    ret = wc_InitMutex(&handle_mutex);
    if (ret != 0) {
        WOLFSSL_MSG("Unable to init handle mutex");
        return -1;
    }
    return 0;
}


/* internal function that closes default handle and frees mutex */
void wolfSSL_CleanupHandle()
{
    wolfSSL_close(defaultHandle);
    wc_FreeMutex(&handle_mutex);
}
#if defined(WOLFSSL_HAVE_SP_ECC)

/* ecc conversion from sp_c32.c */
#include <wolfssl/wolfcrypt/sp.h>


#ifndef WOLFSSL_SP_NO_256

#ifdef HAVE_ECC_VERIFY
/* Read big endian unsigned byte array into r.
 *
 * r  A single precision integer.
 * size  Maximum number of bytes to convert
 * a  Byte array.
 * n  Number of bytes in array to read.
 */
static void int_256_from_bin(int32* r, int size, const byte* a, int n)
{
    int i, j = 0;
    word32 s = 0;

    r[0] = 0;
    for (i = n-1; i >= 0; i--) {
        r[j] |= (((int32)a[i]) << s);
        if (s >= 18U) {
            r[j] &= 0x3ffffff;
            s = 26U - s;
            if (j + 1 >= size) {
                break;
            }
            r[++j] = (int32)a[i] >> s;
            s = 8U - s;
        }
        else {
            s += 8U;
        }
    }

    for (j++; j < size; j++) {
        r[j] = 0;
    }
}

/* Convert an mp_int to an array of sp_digit.
 *
 * r  A single precision integer.
 * size  Maximum number of bytes to convert
 * a  A multi-precision integer.
 */
static void int_256_from_mp(int32* r, int size, const mp_int* a)
{
#if DIGIT_BIT == 26
    int j;

    XMEMCPY(r, a->dp, sizeof(int32) * a->used);

    for (j = a->used; j < size; j++) {
        r[j] = 0;
    }
#elif DIGIT_BIT > 26
    int i, j = 0;
    word32 s = 0;

    r[0] = 0;
    for (i = 0; i < a->used && j < size; i++) {
        r[j] |= ((int32)a->dp[i] << s);
        r[j] &= 0x3ffffff;
        s = 26U - s;
        if (j + 1 >= size) {
            break;
        }
        /* lint allow cast of mismatch word32 and mp_digit */
        r[++j] = (int32)(a->dp[i] >> s); /*lint !e9033*/
        while ((s + 26U) <= (word32)DIGIT_BIT) {
            s += 26U;
            r[j] &= 0x3ffffff;
            if (j + 1 >= size) {
                break;
            }
            if (s < (word32)DIGIT_BIT) {
                /* lint allow cast of mismatch word32 and mp_digit */
                r[++j] = (int32)(a->dp[i] >> s); /*lint !e9033*/
            }
            else {
                r[++j] = 0L;
            }
        }
        s = (word32)DIGIT_BIT - s;
    }

    for (j++; j < size; j++) {
        r[j] = 0;
    }
#else
    int i, j = 0, s = 0;

    r[0] = 0;
    for (i = 0; i < a->used && j < size; i++) {
        r[j] |= ((int32)a->dp[i]) << s;
        if (s + DIGIT_BIT >= 26) {
            r[j] &= 0x3ffffff;
            if (j + 1 >= size) {
                break;
            }
            s = 26 - s;
            if (s == DIGIT_BIT) {
                r[++j] = 0;
                s = 0;
            }
            else {
                r[++j] = a->dp[i] >> s;
                s = DIGIT_BIT - s;
            }
        }
        else {
            s += DIGIT_BIT;
        }
    }

    for (j++; j < size; j++) {
        r[j] = 0;
    }
#endif
}

/* Verify the signature values with the hash and public key.
 *   e = Truncate(hash, 256)
 *   u1 = e/s mod order
 *   u2 = r/s mod order
 *   r == (u1.G + u2.Q)->x mod order
 * Optimization: Leave point in projective form.
 *   (x, y, 1) == (x' / z'*z', y' / z'*z'*z', z' / z')
 *   (r + n*order).z'.z' mod prime == (u1.G + u2.Q)->x'
 * The hash is truncated to the first 256 bits.
 *
 * hash     Hash to sign.
 * hashLen  Length of the hash data.
 * rng      Random number generator.
 * priv     Private part of key - scalar.
 * rm       First part of result as an mp_int.
 * sm       Sirst part of result as an mp_int.
 * heap     Heap to use for allocation.
 * returns RNG failures, MEMORY_E when memory allocation fails and
 * MP_OKAY on success.
 */
int sp_dsp_ecc_verify_256(remote_handle64 handleIn, const byte* hash, word32 hashLen, mp_int* pX,
    mp_int* pY, mp_int* pZ, mp_int* r, mp_int* sm, int* res, void* heap)
{
    int ret;
    remote_handle64 handle = handleIn;

#if 0
    /* calling to alloc memory on the ION using these settings slowed the performance down slightly */
    int32 *x = (int32*)rpcmem_alloc(RPCMEM_HEAP_ID_SYSTEM, RPCMEM_DEFAULT_FLAGS, 10*sizeof(int));
    int32 *y = (int32*)rpcmem_alloc(RPCMEM_HEAP_ID_SYSTEM, RPCMEM_DEFAULT_FLAGS, 10*sizeof(int));
    int32 *z = (int32*)rpcmem_alloc(RPCMEM_HEAP_ID_SYSTEM, RPCMEM_DEFAULT_FLAGS, 10*sizeof(int));
    int32 *s = (int32*)rpcmem_alloc(RPCMEM_HEAP_ID_SYSTEM, RPCMEM_DEFAULT_FLAGS, 10*sizeof(int));
    int32 *u1 = (int32*)rpcmem_alloc(RPCMEM_HEAP_ID_SYSTEM, RPCMEM_DEFAULT_FLAGS, 10*sizeof(int));
    int32 *u2 = (int32*)rpcmem_alloc(RPCMEM_HEAP_ID_SYSTEM, RPCMEM_DEFAULT_FLAGS, 10*sizeof(int));
#endif
    int32 x[10] __attribute__((aligned(128)));
    int32 y[10] __attribute__((aligned(128)));
    int32 z[10] __attribute__((aligned(128)));
    int32 s[10] __attribute__((aligned(128)));
    int32 u1[10] __attribute__((aligned(128)));
    int32 u2[10] __attribute__((aligned(128)));

    if (hashLen > 32U) {
        hashLen = 32U;
    }

    int_256_from_bin(u1, 10, hash, (int)hashLen);
    int_256_from_mp(u2, 10, r);
    int_256_from_mp(s, 10, sm);
    int_256_from_mp(x, 10, pX);
    int_256_from_mp(y, 10, pY);
    int_256_from_mp(z, 10, pZ);

    if (handle_function != NULL) {
        handle_function(&handle, WOLFSSL_HANDLE_GET, NULL);
    }

    *res = 0;
    ret = wolfSSL_DSP_ECC_Verify_256(handle, u1, 10, u2, 10, s, 10, x, 10, y, 10, z, 10, res);

    if (handle_function != NULL) {
        handle_function(&handle, WOLFSSL_HANDLE_DONE, NULL);
    }
#if 0
    rpcmem_free(x);
    rpcmem_free(y);
    rpcmem_free(z);
    rpcmem_free(s);
    rpcmem_free(u1);
    rpcmem_free(u2);
#endif
    return ret;
}


/* Used to assign a handle to an ecc_key structure.
 * returns 0 on success */
int wc_ecc_set_handle(ecc_key* key, remote_handle64 handle)
{
    if (key == NULL) {
        return BAD_FUNC_ARG;
    }
    key->handle = handle;
    return 0;
}
#endif /* HAVE_ECC_VERIFY */
#endif /* !WOLFSSL_SP_NO_256 */
#endif /* WOLFSSL_HAVE_SP_ECC */
#endif /* WOLFSSL_DSP */
