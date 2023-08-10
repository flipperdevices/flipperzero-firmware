/* wolfssl_demo.h
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

#ifndef WOLFSSL_DEMO_H_
#define WOLFSSL_DEMO_H_


/**************************************************/
/******* DO NOT CHANGE the following order ********/
/**************************************************/

/* Include logging header files and define logging macros in the following order:
 * 1. Include the header file "logging_levels.h".
 * 2. Define the LIBRARY_LOG_NAME and LIBRARY_LOG_LEVEL macros depending on
 * the logging configuration for DEMO.
 * 3. Include the header file "logging_stack.h", if logging is enabled for DEMO.
 */

#include "logging_levels.h"

/* Logging configuration for the Demo. */
#ifndef LIBRARY_LOG_NAME
    #define LIBRARY_LOG_NAME    "WOLFSSL_Demo"
#endif

#ifndef LIBRARY_LOG_LEVEL
    #define LIBRARY_LOG_LEVEL    LOG_INFO
#endif
#include "logging_stack.h"
#include "wolfssl/ssl.h"

/* Enable wolfcrypt test demo */
/*#define CRYPT_TEST */

/* Enable benchmark demo      */
/*#define BENCHMARK*/

/* Enable TLS client demo     */
/* cannot enable with other definition */
#define TLS_CLIENT

/* use multi-thread example */
/* #define TLS_MULTITHREAD_TEST */
#if defined(TLS_MULTITHREAD_TEST) && defined(WOLFSSL_TLS13)
    #error "MULTITHREAD_TEST is only available when not set WOLFSSL_TLS13 \
           because it is not verified yet."
#endif

#if defined(TLS_MULTITHREAD_TEST)
 #define THREAD_STACK_SIZE (5 * 1024)
#endif

typedef struct tagTestInfo
{
     int  id;
     int  port;
     char name[32];
     const char* cipher;
     WOLFSSL_CTX* ctx;
     wolfSSL_Logging_cb log_f;
#if defined(TLS_MULTITHREAD_TEST)
     SemaphoreHandle_t xBinarySemaphore;
#endif
} TestInfo;


#endif /* WOLFSSL_DEMO_H_ */
