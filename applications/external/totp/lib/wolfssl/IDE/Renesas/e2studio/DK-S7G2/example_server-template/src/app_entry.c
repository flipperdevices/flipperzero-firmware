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
#include "nx_api.h"
#include "stdio.h"
extern void initialise_monitor_handles(void);

#include <wolfssl/ssl.h>
#define USE_CERT_BUFFERS_2048
#define USE_CERT_BUFFERS_256
#include <wolfssl/certs_test.h>
#define TLS_PORT 11111

static void server()
{
    UINT TEST_PORT = TLS_PORT;
    NX_TCP_SOCKET sockfd;
    int ret;
    unsigned char* cert;
    int certSz;

    unsigned char* key;
    int keySz;

    WOLFSSL_CTX* ctx;
    WOLFSSL* ssl;

    ctx = wolfSSL_CTX_new(wolfSSLv23_server_method());
    if (ctx == NULL) {
        printf("Unable to create ctx\n");
        return;
    }

#ifndef NO_RSA
    cert = server_cert_der_2048;
    certSz = sizeof_server_cert_der_2048;
    key = server_key_der_2048;
    keySz = sizeof_server_key_der_2048;
#endif

#if 0
    /* Use ECDSA */
    cert = serv_ecc_der_256;
    certSz = sizeof_serv_ecc_der_256;
    key = ecc_key_der_256;
    keySz = sizeof_ecc_key_der_256;
#endif

    ret = wolfSSL_CTX_use_certificate_buffer(ctx, cert,
                                           certSz, SSL_FILETYPE_ASN1);
    if (ret != WOLFSSL_SUCCESS) {
        printf("Unable to load certificate ret = %d\n", ret);
        wolfSSL_CTX_free(ctx);
        return;
    }

    ret = wolfSSL_CTX_use_PrivateKey_buffer(ctx, key,
                                            keySz, SSL_FILETYPE_ASN1);
    if (ret != WOLFSSL_SUCCESS) {
        printf("Unable to load key ret = %d\n", ret);
        wolfSSL_CTX_free(ctx);
        return;
    }

#ifdef WOLFSSL_TLS13
        {
            int groups[3];
            int idx = 0;

    #ifdef HAVE_CURVE25519
            groups[idx++] = WOLFSSL_ECC_X25519;
    #endif
    #ifdef HAVE_ECC
            groups[idx++] = WOLFSSL_ECC_SECP256R1;
    #endif
            groups[idx++] = WOLFSSL_FFDHE_2048;

            ret = wolfSSL_CTX_set_groups(ctx, groups, idx);
            if (ret != WOLFSSL_SUCCESS) {
                printf("Unable to set groups\n");
            }
        }
#endif
    printf("Waiting for connections on port %d\n", TEST_PORT);

    ret = (int)nx_tcp_socket_create(&g_ip0, &sockfd, "TLS_SERVER", NX_IP_NORMAL,
            NX_FRAGMENT_OKAY, NX_IP_TIME_TO_LIVE, 1500, NX_NULL, NX_NULL);
    if (ret != NX_SUCCESS) {
        printf("failed to create socket err = 0x%X\n", ret);
    }

    ret = (int)nx_tcp_server_socket_listen(&g_ip0, TEST_PORT, &sockfd,
                                           NX_MAX_LISTEN_REQUESTS, NULL);
    if (ret != NX_SUCCESS) {
        printf("failed to listen\n");
    }

    while (1) {
        ret = (int)nx_tcp_server_socket_accept(&sockfd, NX_WAIT_FOREVER);
        if (ret != NX_SUCCESS) {
            printf("failed to accept with error 0x%X\n", ret);
            break;
        }

        ssl = wolfSSL_new(ctx);
        if (ssl == NULL) {
            printf("Error creating ssl\n");
            break;
        }

        wolfSSL_SetIO_NetX(ssl, &sockfd, NX_WAIT_FOREVER);

        ret = wolfSSL_accept(ssl);
        if (ret != WOLFSSL_SUCCESS) {
            printf("Error %d with wolfssl accept\n", wolfSSL_get_error(ssl, ret));
            wolfSSL_free(ssl);
            break;
        }
        wolfSSL_free(ssl);

        nx_tcp_socket_disconnect(&sockfd, NX_WAIT_FOREVER);

        ret = (int)nx_tcp_server_socket_unaccept(&sockfd);
        if (ret != NX_SUCCESS) {
            printf("failed to unaccept with error 0x%X\n", ret);
            break;
        }

        ret = (int)nx_tcp_server_socket_relisten(&g_ip0, TEST_PORT, &sockfd);
        if (ret != NX_SUCCESS && ret != NX_CONNECTION_PENDING) {
            printf("failed to relisten 0x%X\n", ret);
            break;
        }
    }
    ret = (int)nx_tcp_server_socket_unlisten(&g_ip0, TEST_PORT);
    if (ret != NX_SUCCESS) {
        printf("failed to unlisten\n");
        return;
    }

    nx_tcp_socket_delete(&sockfd);

    wolfSSL_CTX_free(ctx);
}

/* app entry function */
void app_entry(void)
{
    initialise_monitor_handles();
    wolfSSL_Init();

#ifdef DEBUG_WOLFSSL
    wolfSSL_Debugging_ON();
#endif
    server();
    wolfSSL_Cleanup();
    printf("Server closed down\n");
    while (1)
    {
        tx_thread_sleep (1);
    }
}
