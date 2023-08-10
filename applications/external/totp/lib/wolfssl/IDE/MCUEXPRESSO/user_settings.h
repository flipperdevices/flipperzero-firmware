/* user_settings.h
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

#ifndef USER_SETTINGS_H_
#define USER_SETTINGS_H_

#define FREERTOS
#define NO_WOLFSSL_DIR
#define WOLFSSL_NO_SOCK
#define WOLFSSL_LWIP
#define WOLFSSL_USER_IO

#define DEBUG_WOLFSSL
#define WOLFSSL_LOG_PRINTF

/* macros for test suite and benchmark */
//#define NO_CRYPT_TEST
#ifndef NO_CRYPT_TEST
    #define NO_MAIN_DRIVER

    #include <stdio.h>
    #include <stdarg.h>
    static void myPrintf(const char* fmt, ...)
    {
	    int ret;
	    char line[150];
	    va_list ap;

	    va_start(ap, fmt);
	    ret = vsnprintf(line, sizeof(line), fmt, ap);
	    line[sizeof(line)-1] = '\0';

	    DbgConsole_Printf("%s", line);

	    /* add CR on newlines */
	    if (ret > 0 && line[ret-1] == '\n') {
	        DbgConsole_Printf("\r");
        }
    }
    #define XPRINTF myPrintf
    #define USE_CERT_BUFFERS_2048
    #define USE_CERT_BUFFERS_1024
    #define USE_CERT_BUFFERS_256
    #define NO_WRITE_TEMP_FILES
    #define BENCH_EMBEDDED

    /* set high for handling wolfcrypt tests */
    #define WOLFSSL_STATIC_MEMORY_TEST_SZ 350000
#endif

/* math implementation (fast math or sp math, choose one) */
#define USE_FAST_MATH
#ifdef USE_FAST_MATH
    /* big enough for even 4096 bit RSA key */
	#define FP_MAX_BITS 8192
	#define TFM_TIMING_RESISTANT
	#define ECC_TIMING_RESISTANT
    #define ALT_ECC_SIZE
#endif

//#define WOLFSSL_SP_MATH_ALL
#ifdef  WOLFSSL_SP_MATH_ALL
#endif

/* optimizations for common public key operations
 * adds a lot of code size in exchange for performance increase */
#if 1
#define WOLFSSL_HAVE_SP_RSA
#define WOLFSSL_HAVE_SP_DH
#define WOLFSSL_HAVE_SP_ECC
#endif

/* RT685 uses the same fsl_trng API, note that the application
 * needs to initialize and teardown the TRNG (TRNG_Init, TRNG_Deinit) */
#define FREESCALE_KSDK_2_0_TRNG
#define FREESCALE_RTC

/* use loaded stack as a pool of memory for allocations rather than
 * using heap */
#define WOLFSSL_STATIC_MEMORY
#define WOLFSSL_SP_NO_MALLOC /* use no malloc version of SP if compiled in */
#define WOLFSSL_NO_REALLOC

/* this removes all system malloc calls, will fail on some compatibility
 * layer API's and others that have no heap hint */
//#define WOLFSSL_NO_MALLOC

#define WC_RSA_BLINDING
#define HAVE_ECC
#define OPENSSL_EXTRA
#define OPENSSL_ALL
#define NO_WOLFSSL_SERVER
#define HAVE_TLS_EXTENSIONS
#define WC_RSA_PSS
#define WOLFSSL_KEY_GEN
#define HAVE_SMIME
#ifdef  HAVE_SMIME
    #define HAVE_PKCS7
    #define HAVE_X963_KDF
    #define HAVE_AES_KEYWRAP
    #define WOLFSSL_AES_DIRECT
#endif

#endif /* USER_SETTINGS_H_ */
