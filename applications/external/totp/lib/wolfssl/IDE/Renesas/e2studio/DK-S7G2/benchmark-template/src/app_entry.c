/* app_entry.c
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



#include "app.h"
#include "stdio.h"
extern void initialise_monitor_handles(void);

#include <wolfcrypt/benchmark/benchmark.h>

#ifndef WOLFCRYPT_ONLY

#include <wolfssl/ssl.h>

#define USE_CERT_BUFFERS_2048
#define USE_CERT_BUFFERS_256
#include <wolfssl/certs_test.h>

#include "nx_api.h"
#define CONNECTION_TIMES 100
#define SERVER_IP IP_ADDRESS(10,22,73,128)
#define TLS_PORT 11111
#define TCP_PORT 11112

static double milliseconds = 0;
void  timer_callback(timer_callback_args_t * args)
{
    milliseconds++;
    (void)args;
}

/* version is the type of TLS version to use. For example TLS1.2 = version 2
 * and TLS1.3 = version 3
 *
 * suites is a null terminated string containing the cipher suites to us, or
 * can be NULL for default*/
static void benchmark_TLS(int version, char* suites, int group)
{
    UINT TEST_PORT = TLS_PORT;
    ULONG TEST_IP  = SERVER_IP;
    int i;
    NX_TCP_SOCKET sockfd;
    int ret;
#ifdef WOLFSSL_TLS13
    int groups[1];
#endif
    double start;
    WOLFSSL_METHOD* method = NULL;

    WOLFSSL_CTX* ctx;
    WOLFSSL* ssl;

    switch (version) {
        case 2: method = wolfTLSv1_2_client_method(); break;
#ifdef WOLFSSL_TLS13
        case 3: method = wolfTLSv1_3_client_method(); break;
#endif
        default:
            printf("Unknown TLS version (Check if built with it supported)\n");
            return;
    }

    ctx = wolfSSL_CTX_new(method);
    if (ctx == NULL) {
        printf("unable to create ctx\n");
        return;
    }

#ifndef NO_RSA
    /* add default RSA CA */
    ret = wolfSSL_CTX_load_verify_buffer(ctx, ca_cert_der_2048,
            sizeof_ca_cert_der_2048, SSL_FILETYPE_ASN1);
    if (ret != WOLFSSL_SUCCESS) {
        printf("error %d loading CA\n", ret);
        return;
    }
#endif

#ifdef HAVE_ECC
    /* add default ECDSA CA */
    ret = wolfSSL_CTX_load_verify_buffer(ctx, ca_ecc_cert_der_256,
            sizeof_ca_ecc_cert_der_256, SSL_FILETYPE_ASN1);
    if (ret != WOLFSSL_SUCCESS) {
        printf("error %d loading CA\n", ret);
        return;
    }

#endif

    if (suites != NULL) {
        ret = wolfSSL_CTX_set_cipher_list(ctx, suites);
        if (ret != WOLFSSL_SUCCESS) {
            printf("error %d setting cipher suites %s\n", ret, suites);
            return;
        }
    }

#ifdef WOLFSSL_TLS13
    if (version == 3) {
        groups[0] = group;
        ret = wolfSSL_CTX_set_groups(ctx, groups, 1);
        if (ret != WOLFSSL_SUCCESS) {
            printf("error setting group\n");
            return;
        }
    }
#endif

    printf("Trying to connect to 0x%lX on port %d\n", TEST_IP, TEST_PORT);

    milliseconds = 0;
    g_timer0.p_api->open(g_timer0.p_ctrl, g_timer0.p_cfg);
    g_timer0.p_api->start(g_timer0.p_ctrl);

    start = (double)tx_time_get(); // TX_TIMER_TICKS_PER_SECOND = 100
    ret = (int)nx_tcp_socket_create(&g_ip0, &sockfd, "TLS_CLIENT", NX_IP_NORMAL,
            NX_FRAGMENT_OKAY, NX_IP_TIME_TO_LIVE, 1500, NX_NULL, NX_NULL);
    if (ret != NX_SUCCESS) {
        printf("failed to create socket err = 0x%X\n", ret);
        return;
    }
    for (i = 0; i < CONNECTION_TIMES; i++) {

        ret = (int)nx_tcp_client_socket_bind(&sockfd, NX_ANY_PORT,
                NX_WAIT_FOREVER);
        if (ret != NX_SUCCESS) {
            printf("failed to bind socket\n");
            return;
        }

        ret = (int)nx_tcp_client_socket_connect(&sockfd, TEST_IP, TEST_PORT,
                NX_WAIT_FOREVER);
        if (ret != NX_SUCCESS) {
            printf("failed to connect with error 0x%X\n", ret);
            return;
        }

        ssl = wolfSSL_new(ctx);
        if (ssl == NULL) {
            printf("Error creating ssl\n");
            return;
        }

#ifdef WOLFSSL_TLS13
        if (version == 3) {
            ret = wolfSSL_UseKeyShare(ssl, group);
            if (ret != WOLFSSL_SUCCESS) {
                printf("Error %d with set key share\n", ret);
                return;
            }
        }
#endif

        wolfSSL_SetIO_NetX(ssl, &sockfd, NX_WAIT_FOREVER);

        ret = wolfSSL_connect(ssl);
        if (ret != WOLFSSL_SUCCESS) {
            printf("Error %d with wolfssl connect\n", ret);
            return;
        }
        wolfSSL_free(ssl);

        nx_tcp_socket_disconnect(&sockfd, NX_WAIT_FOREVER);

        ret = nx_tcp_client_socket_unbind(&sockfd);
        if (ret != NX_SUCCESS) {
            printf("failed to unbind with error 0x%X\n", ret);
            return;
        }

    }
    nx_tcp_socket_delete(&sockfd);

    g_timer0.p_api->stop(g_timer0.p_ctrl);
    start = (double)tx_time_get() - start;
    g_timer0.p_api->close(g_timer0.p_ctrl);

    printf("%d TLS connections took %f seconds and %f tx_time ticks\n",
            CONNECTION_TIMES, (milliseconds / 10), start);
    wolfSSL_CTX_free(ctx);
}


static void benchmark_TCP()
{
    UINT TEST_PORT = TCP_PORT;
    ULONG TEST_IP  = SERVER_IP;
    int i;
    NX_TCP_SOCKET sockfd;
    int ret;
    double start;


    {
        NX_PACKET* response;
        printf("Pinging server to see if up .. ");
        fflush(stdout);
        ret = (int)nx_icmp_ping(&g_ip0, TEST_IP, "Hello", strlen("Hello"),
                &response, 2000);
        if (ret != NX_SUCCESS) {
            printf("Unable to ping server, error = 0x%X\n", ret);
            return;
        }
        printf("got response from server\n");
        nx_packet_release(response);
    }

    printf("Benchmarking client TCP connection\n");
    printf("Trying to connect to 0x%lX on port %d\n", TEST_IP, TEST_PORT);
    start = (double)tx_time_get() / TX_TIMER_TICKS_PER_SECOND;
    ret = (int)nx_tcp_socket_create(&g_ip0, &sockfd, "TCP_CLIENT", NX_IP_NORMAL,
            NX_FRAGMENT_OKAY, NX_IP_TIME_TO_LIVE, 256, NX_NULL, NX_NULL);
    if (ret != NX_SUCCESS) {
        printf("failed to create socket err = 0x%X\n", ret);
        return;
    }

    for (i = 0; i < CONNECTION_TIMES; i++) {
        ret = (int)nx_tcp_client_socket_bind(&sockfd, NX_ANY_PORT,
                NX_WAIT_FOREVER);
        if (ret != NX_SUCCESS) {
            printf("failed to bind socket\n");
            return;
        }

        ret = (int)nx_tcp_client_socket_connect(&sockfd, TEST_IP, TEST_PORT,
                NX_WAIT_FOREVER);
        if (ret != NX_SUCCESS) {
            printf("failed to connect with error 0x%X\n", ret);
            return;
        }

        nx_tcp_socket_disconnect(&sockfd, NX_WAIT_FOREVER);

        ret = (int)nx_tcp_client_socket_unbind(&sockfd);
        if (ret != NX_SUCCESS) {
            printf("failed to unbind with error 0x%X\n", ret);
            return;
        }
    }
    nx_tcp_socket_delete(&sockfd);

    start = ((double)tx_time_get() / TX_TIMER_TICKS_PER_SECOND) - start;
    printf("%d TCP connections took %f seconds\n", CONNECTION_TIMES, start);
}
#endif /* WOLFCRYPT_ONLY */

/* Benchmark entry function */
void app_entry(void)
{
    initialise_monitor_handles();

#ifdef DEBUG_WOLFSSL
    wolfSSL_Debugging_ON();
#endif

    wolfSSL_Init();

#ifndef WOLFCRYPT_ONLY
    benchmark_TCP();

    printf("\nBenchmarking client TLSv1.2 connection using ECDHE-RSA-AES128-GCM-SHA256\n");
    benchmark_TLS(2, "ECDHE-RSA-AES128-GCM-SHA256", 0);
    #ifdef WOLFSSL_TLS13
    #ifdef HAVE_CURVE25519
    printf("\nBenchmarking client TLSv1.3 WOLFSSL_ECC_X25519 connection using TLS13_AES128_GCM_SHA256\n");
    benchmark_TLS(3, "TLS13-AES128-GCM-SHA256", (int)WOLFSSL_ECC_X25519);
    #endif
    #ifdef HAVE_ECC
    printf("\nBenchmarking client TLSv1.3 WOLFSSL_ECC_SECP256R1 connection using TLS13_AES128_GCM_SHA256\n");
    benchmark_TLS(3, "TLS13-AES128-GCM-SHA256", (int)WOLFSSL_ECC_SECP256R1);
    #endif
    printf("\nBenchmarking client TLSv1.3 WOLFSSL_FFDHE_2048 connection using TLS13_AES128_GCM_SHA256\n");
    benchmark_TLS(3, "TLS13-AES128-GCM-SHA256", (int)WOLFSSL_FFDHE_2048);
    #endif
#endif

#if 1
    /* run wolfcrypt benchmarks */
    benchmark_test(NULL);
#endif

    wolfSSL_Cleanup();
    while (1)
    {
        tx_thread_sleep (100);
    }
}
