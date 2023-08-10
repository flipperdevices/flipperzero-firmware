/* client.c
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

#include "client.h"
/* the usual suspects */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* socket includes */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

/* utility functions shared between client and server */
#include <shared/util.h>

/* wolfSSL */
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/ssl.h>
#include <wolfssl/certs_test.h>

/* Azure Sphere */
#include <applibs/networking.h>

int main(int argc, char** argv)
{
    bool isNetworkingReady = false;
    SOCKET_T           sockfd = 0;
    char               buff[256];
    size_t             len;
    int ret;

    /* declare wolfSSL objects */
    WOLFSSL_CTX* ctx = NULL;
    WOLFSSL* ssl = NULL;

	util_PrintIfAddr();

    /* Check if the Azure Sphere Dev Board has network connectivity. */
    if ((Networking_IsNetworkingReady(&isNetworkingReady) < 0) || !isNetworkingReady) {
        fprintf(stderr, "ERROR: network is not up.\n");
        return -1;
    }

    ret = wolfIO_TcpConnect(&sockfd, SERVER_IP, DEFAULT_PORT, 0);
    if ((ret != 0) || ((int)sockfd < 0)) {
        fprintf(stderr, "ERROR: failed to create socket.");
        return -1;
    }

    /* Initialize wolfSSL */
    wolfSSL_Init();

    /* Create and initialize WOLFSSL_CTX */
    ctx = wolfSSL_CTX_new(wolfTLSv1_2_client_method());
    if (ctx == NULL) {
        fprintf(stderr, "ERROR: failed to create WOLFSSL_CTX\n");
        util_Cleanup(sockfd,ctx,ssl);
        return -1;
    }

    /* Load client certificates into WOLFSSL_CTX */
    ret = wolfSSL_CTX_load_verify_buffer(ctx, CERT, SIZEOF_CERT, WOLFSSL_FILETYPE_ASN1);
    if (ret != SSL_SUCCESS) {
        fprintf(stderr, "ERROR: failed to load client certificate, "
                        "please check the buffer.\n");
        util_Cleanup(sockfd,ctx,ssl);
        return -1;
    }

    /* Create a WOLFSSL object */
    if ((ssl = wolfSSL_new(ctx)) == NULL) {
        fprintf(stderr, "ERROR: failed to create WOLFSSL object\n");
        util_Cleanup(sockfd,ctx,ssl);
        return -1;
    }

    /* Attach wolfSSL to the socket */
    wolfSSL_set_fd(ssl, sockfd);

    /* Connect to wolfSSL on the server side */
    if (wolfSSL_connect(ssl) != SSL_SUCCESS) {
        fprintf(stderr, "ERROR: failed to connect to wolfSSL\n");
        util_Cleanup(sockfd,ctx,ssl);
        return -1;
    }

    /* Get length of message for server. */
    printf("\nMessage for server: %s\n",msg);
    len = strnlen(msg, sizeof(msg));

    /* Send the message to the server */
    if (wolfSSL_write(ssl, msg, (int)len) != len) {
        fprintf(stderr, "ERROR: failed to write\n");
        util_Cleanup(sockfd,ctx,ssl);
        return -1;
    }

    /* Read the server data into our buff array */
    memset(buff, 0, sizeof(buff));
    if (wolfSSL_read(ssl, buff, sizeof(buff) - 1) == -1) {
        fprintf(stderr, "ERROR: failed to read\n");
        util_Cleanup(sockfd,ctx,ssl);
        return -1;
    }

    /* Print to stdout any data the server sends */
    printf("Server Reply: %s\n", buff);

    /* Cleanup and return */
    util_Cleanup(sockfd,ctx,ssl);
    return 0; /* Return reporting a success */
}
