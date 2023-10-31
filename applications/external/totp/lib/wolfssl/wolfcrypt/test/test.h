/* wolfcrypt/test/test.h
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


#ifndef WOLFCRYPT_TEST_H
#define WOLFCRYPT_TEST_H

#include <wolfssl/wolfcrypt/types.h>

#ifdef __cplusplus
    extern "C" {
#endif

#ifdef WC_TEST_RET_CUSTOM_TYPE
    typedef WC_TEST_RET_CUSTOM_TYPE wc_test_ret_t;
#else
    typedef sword32 wc_test_ret_t;
#endif

#include <wolfssl/wolfcrypt/settings.h>

#ifdef HAVE_STACK_SIZE
THREAD_RETURN WOLFSSL_THREAD wolfcrypt_test(void* args);
#else
wc_test_ret_t wolfcrypt_test(void* args);
#endif

#ifndef NO_MAIN_DRIVER
wc_test_ret_t wolfcrypt_test_main(int argc, char** argv);
#endif

#if defined(WOLFSSL_ESPIDF) || defined(_WIN32_WCE)
int wolf_test_task(void);
#endif

#ifndef WC_TEST_RET_HAVE_CUSTOM_MACROS

#define WC_TEST_RET_TAG_NC     0L
#define WC_TEST_RET_TAG_EC     1L
#define WC_TEST_RET_TAG_ERRNO  2L
#define WC_TEST_RET_TAG_I      3L

#define WC_TEST_RET_ENC(line, i, tag)                           \
        ((wc_test_ret_t)(-((wc_test_ret_t)(line) + ((wc_test_ret_t)((word32)(i) & 0x7ffL) * 100000L) + ((wc_test_ret_t)(tag) << 29L))))

#ifndef WC_TEST_RET_LN
#define WC_TEST_RET_LN __LINE__
#endif

/* encode no code */
#define WC_TEST_RET_ENC_NC WC_TEST_RET_ENC(WC_TEST_RET_LN, 0, WC_TEST_RET_TAG_NC)

/* encode positive integer */
#define WC_TEST_RET_ENC_I(i) WC_TEST_RET_ENC(WC_TEST_RET_LN, i, WC_TEST_RET_TAG_I)

/* encode error code (negative integer) */
#define WC_TEST_RET_ENC_EC(ec) WC_TEST_RET_ENC(WC_TEST_RET_LN, -(ec), WC_TEST_RET_TAG_EC)

/* encode system/libc error code */
#if defined(HAVE_ERRNO_H) && !defined(NO_FILESYSTEM) && \
    !defined(NO_STDIO_FILESYSTEM) && !defined(WOLFSSL_USER_IO)
#include <errno.h>
#define WC_TEST_RET_ENC_ERRNO WC_TEST_RET_ENC(WC_TEST_RET_LN, errno, WC_TEST_RET_TAG_ERRNO)
#else
#define WC_TEST_RET_ENC_ERRNO WC_TEST_RET_ENC_NC
#endif

#define WC_TEST_RET_DEC_TAG(x) ((-(x)) >> 29L)

/* decode line number */
#define WC_TEST_RET_DEC_LN(x) ((int)(((-(x)) & ~(3L << 29L)) % 100000L))

/* decode integer or errno */
#define WC_TEST_RET_DEC_I(x) ((int)((((-(x)) & ~(3L << 29L)) / 100000L)))

/* decode error code */
#define WC_TEST_RET_DEC_EC(x) ((int)(-WC_TEST_RET_DEC_I(x)))

#endif /* !WC_TEST_RET_HAVE_CUSTOM_MACROS */

#ifdef __cplusplus
    }  /* extern "C" */
#endif


#endif /* WOLFCRYPT_TEST_H */
