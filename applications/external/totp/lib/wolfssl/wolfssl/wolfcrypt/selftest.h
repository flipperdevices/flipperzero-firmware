/* selftest.h
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



#ifndef WOLFCRYPT_SELF_TEST_H
#define WOLFCRYPT_SELF_TEST_H

#include <wolfssl/wolfcrypt/types.h>


#ifdef __cplusplus
    extern "C" {
#endif

#ifdef HAVE_SELFTEST
    /* Get wolfCrypt CAVP version */
    WOLFSSL_API const char* wolfCrypt_GetVersion_CAVP_selftest(void);

    /* wolfCrypt self test, runs CAVP KATs */
    WOLFSSL_API int wolfCrypt_SelfTest(void);
#endif

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* WOLFCRYPT_SELF_TEST_H */


