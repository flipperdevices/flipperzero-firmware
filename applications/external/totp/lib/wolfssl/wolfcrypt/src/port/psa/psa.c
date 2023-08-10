/* psa.c
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

#if defined(WOLFSSL_HAVE_PSA)

#include <psa/crypto.h>

#include <wolfssl/wolfcrypt/port/psa/psa.h>

#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/types.h>

#if defined(WOLFSSL_PSA_GLOBAL_LOCK)
static wolfSSL_Mutex psa_global_mutex;

void PSA_LOCK()
{
    /* ideally we should propagate the return error here. Leaving out for code
       simplicity for now. */
    wc_LockMutex(&psa_global_mutex);
}

void PSA_UNLOCK()
{
    wc_UnLockMutex(&psa_global_mutex);
}

#endif


int wc_psa_init()
{
    psa_status_t s;

#if defined(WOLFSSL_PSA_GLOBAL_LOCK)
    wc_InitMutex(&psa_global_mutex);
#endif

    PSA_LOCK();
    s = psa_crypto_init();
    PSA_UNLOCK();
    if (s != PSA_SUCCESS)
        return WC_HW_E;

    return 0;
}

#if !defined(WOLFSSL_PSA_NO_RNG)
/**
 * wc_psa_get_random() - generate @size random bytes in @out
 * @out: output buffer
 * @size: number of random bytes to generate
 *
 * return: 0 on success
 */
int wc_psa_get_random(unsigned char *out, word32 sz)
{
    psa_status_t s;

    PSA_LOCK();
    s = psa_generate_random((uint8_t*)out, sz);
    PSA_UNLOCK();
    if (s != PSA_SUCCESS)
        return WC_HW_E;

    return 0;
}
#endif

#endif /* WOLFSSL_HAVE_PSA */
