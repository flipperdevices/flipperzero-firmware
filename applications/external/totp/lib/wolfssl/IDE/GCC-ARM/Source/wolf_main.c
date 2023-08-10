/* wolf_main.c
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


#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/random.h> /* for CUSTOM_RAND_TYPE */

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>


/* TIME CODE */
/* TODO: Implement real RTC */
/* Optionally you can define NO_ASN_TIME to disable all cert time checks */
static int gTimeMs;
static int hw_get_time_sec(void)
{
    #warning Must implement your own time source if validating certificates

	return ++gTimeMs;
}

/* This is used by wolfCrypt asn.c for cert time checking */
unsigned long my_time(unsigned long* timer)
{
    (void)timer;
    return hw_get_time_sec();
}

#ifndef WOLFCRYPT_ONLY
/* This is used by TLS only */
unsigned int LowResTimer(void)
{
    return hw_get_time_sec();
}
#endif

#ifndef NO_CRYPT_BENCHMARK
/* This is used by wolfCrypt benchmark tool only */
double current_time(int reset)
{
    double timeNow;
    int timeMs = gTimeMs;
    (void)reset;
    timeNow = (timeMs / 1000); // sec
    timeNow += (double)(timeMs % 1000) / 1000; // ms
    return timeNow;
}
#endif

/* RNG CODE */
/* TODO: Implement real RNG */
static unsigned int gCounter;
unsigned int hw_rand(void)
{
    #warning Must implement your own random source

    return ++gCounter;
}

unsigned int my_rng_seed_gen(void)
{
    return hw_rand();
}

int my_rng_gen_block(unsigned char* output, unsigned int sz)
{
    uint32_t i = 0;

    while (i < sz)
    {
        /* If not aligned or there is odd/remainder */
        if( (i + sizeof(CUSTOM_RAND_TYPE)) > sz ||
            ((uint32_t)&output[i] % sizeof(CUSTOM_RAND_TYPE)) != 0
        ) {
            /* Single byte at a time */
            output[i++] = (unsigned char)my_rng_seed_gen();
        }
        else {
            /* Use native 8, 16, 32 or 64 copy instruction */
            *((CUSTOM_RAND_TYPE*)&output[i]) = my_rng_seed_gen();
            i += sizeof(CUSTOM_RAND_TYPE);
        }
    }

    return 0;
}


#ifdef XMALLOC_OVERRIDE
void *myMalloc(size_t n, void* heap, int type)
{
    (void)n;
    (void)heap;
    (void)type;

    #warning Must implement your own malloc

    return NULL;
}
void myFree(void *p, void* heap, int type)
{
    (void)p;
    (void)heap;
    (void)type;

    #warning Must implement your own free
}

/* Required for normal math (!USE_FAST_MATH) */
void *myRealloc(void *p, size_t n, void* heap, int type)
{
    (void)p;
    (void)n;
    (void)heap;
    (void)type;

    #warning Must implement your own realloc

    return NULL;
}
#endif /* XMALLOC_OVERRIDE */
