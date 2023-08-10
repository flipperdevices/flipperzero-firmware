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

#include <wolfssl/ssl.h>
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"

#define FREQ 10000 /* Hz */

#define FLASH_HP_DF_BLOCK_0   0x08000000U /*   64 B:  0x40100000 - 0x4010003F */
#define FLASH_HP_DF_BLOCK_1   0x08000040U /*   64 B:  0x40100040 - 0x4010007F */
#define FLASH_HP_DF_BLOCK_2   0x08000080U /*   64 B:  0x40100080 - 0x401000BF */
#define FLASH_HP_DF_BLOCK_3   0x080000C0U /*   64 B:  0x401000C0 - 0x401000FF */
#define DIRECT_KEY_ADDRESS_256      FLASH_HP_DF_BLOCK_1
#define DIRECT_KEY_ADDRESS_128      FLASH_HP_DF_BLOCK_2

/* Client connects to the server with these details. */
#define SERVER_IP    "192.168.11.49"
#define DEFAULT_PORT 11111

/* Enable wolfcrypt test */
/* can be enabled with benchmark test */
/*#define CRYPT_TEST*/

/* Enable benchmark               */
/* can be enabled with cyrpt test */
/*#define BENCHMARK*/

/* Enable TLS client     */
/* cannot enable with CRYPT_TEST or BENCHMARK */
#define TLS_CLIENT

/* use multi-thread example */
/*#define TLS_MULTITHREAD_TEST*/
#if defined(TLS_MULTITHREAD_TEST)
 #define THREAD_STACK_SIZE (5 * 1024)
#endif

/* Use RSA certificates */
#define USE_CERT_BUFFERS_2048
/* Use ECC certificates */
/*#define USE_CERT_BUFFERS_256*/

#if defined(USE_CERT_BUFFERS_2048) && defined(USE_CERT_BUFFERS_256)
    #error please set either macro USE_CERT_BUFFERS_2048 or USE_CERT_BUFFERS_256
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

void sce_test();
void TCPInit();
void wolfSSL_TLS_client_init();
void wolfSSL_TLS_client_do(void *pvParam);
void wolfSSL_TLS_cleanup();
extern WOLFSSL_CTX *client_ctx;

#ifdef TLS_MULTITHREAD_TEST
extern xSemaphoreHandle exit_semaph;
#endif

static void util_Cleanup(xSocket_t xSock, WOLFSSL_CTX *ctx, WOLFSSL *ssl) {
    printf("Cleaning up socket and wolfSSL objects.\n");
    if (xSock != NULL)
        FreeRTOS_closesocket(xSock);
    if (ssl != NULL)
        wolfSSL_free(ssl);
    if (ctx != NULL)
        wolfSSL_CTX_free(ctx);
    wolfSSL_Cleanup();
}

static inline void util_inf_loop(xSocket_t xClientSocket, WOLFSSL_CTX *ctx,
        WOLFSSL *ssl) {
    util_Cleanup(xClientSocket, ctx, ssl);
    printf("Reached infinite loop.\n");
    while (1)
        ;
}
#endif /* WOLFSSL_DEMO_H_ */
