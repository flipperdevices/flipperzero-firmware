/* server.c
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

#include "server.h"
/* the usual suspects */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* socket includes */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

/* <shared/util.h> includes */
#include <shared/util.h>

/* wolfSSL */
#include <wolfssl/ssl.h>
#include <wolfssl/certs_test.h>

/* Azure Sphere */
#include <applibs/log.h>
#include <applibs/networking.h>

#define BIND_PORT		11111
#define CERT_BUF        server_cert_der_2048
#define SIZEOF_CERT_BUF sizeof_server_cert_der_2048
#define KEY_BUF         server_key_der_2048
#define SIZEOF_KEY_BUF  sizeof_server_key_der_2048

int main(void)
{
    bool isNetworkingReady = false;
    int                sockfd;
    int                connd;
    struct sockaddr_in servAddr;
    struct sockaddr_in clientAddr;
    socklen_t          size = sizeof(clientAddr);
    char               buff[256];
    size_t             len;
    int                shutdown = 0;
    int                ret;
    const char*        reply = "I hear ya fa shizzle!\n";

    /* declare wolfSSL objects */
    WOLFSSL_CTX* ctx = NULL;
    WOLFSSL*     ssl = NULL;

    util_PrintIfAddr();

    /* Check if the Azure Sphere Dev Board has network connectivity. */
    if ((Networking_IsNetworkingReady(&isNetworkingReady) < 0) || !isNetworkingReady) {
        fprintf(stderr,"Error: Network is not up.\n");
        return -1;
    }

    /* Initialize wolfSSL */
    wolfSSL_Init();

    /* Create a socket that uses an internet IPv4 address,
     * Sets the socket to be stream based (TCP),
     * 0 means choose the default protocol. */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "ERROR: failed to create the socket\n");
        util_Cleanup(sockfd, ctx, ssl);
        return -1;
    }

    /* Create and initialize WOLFSSL_CTX */
    if ((ctx = wolfSSL_CTX_new(wolfTLSv1_2_server_method())) == NULL) {
        fprintf(stderr, "ERROR: failed to create WOLFSSL_CTX\n");
        util_Cleanup(sockfd, ctx, ssl);
        return -1;
    }

    /* Load server certificates into WOLFSSL_CTX */
    if (wolfSSL_CTX_use_certificate_buffer(ctx, CERT_BUF, SIZEOF_CERT_BUF, SSL_FILETYPE_ASN1)
        != SSL_SUCCESS) {
        fprintf(stderr, "ERROR: failed to load %s, please check the file.\n",
                CERT_BUF);
        util_Cleanup(sockfd, ctx, ssl);
        return -1;
    }

    /* Load server key into WOLFSSL_CTX */
    if (wolfSSL_CTX_use_PrivateKey_buffer(ctx, KEY_BUF, SIZEOF_KEY_BUF, SSL_FILETYPE_ASN1)
        != SSL_SUCCESS) {
        fprintf(stderr, "ERROR: failed to load %s, please check the file.\n",
                KEY_BUF);
        util_Cleanup(sockfd, ctx, ssl);
        return -1;
    }

    /* Initialize the server address struct with zeros */
    memset(&servAddr, 0, sizeof(servAddr));

    /* Fill in the server address */
    servAddr.sin_family      = AF_INET;             /* using IPv4      */
    servAddr.sin_port        = htons(BIND_PORT);    /* on BIND_PORT    */
    servAddr.sin_addr.s_addr = INADDR_ANY;          /* from anywhere   */

    /* Bind the server socket to our port */
    if (bind(sockfd, (struct sockaddr*)&servAddr, sizeof(servAddr)) == -1) {
        fprintf(stderr, "ERROR: failed to bind\n");
        util_Cleanup(sockfd, ctx, ssl);
        return -1;
    }

    /* Listen for a new connection, allow 5 pending connections */
    if (listen(sockfd, 5) == -1) {
        fprintf(stderr, "ERROR: failed to listen\n");
        util_Cleanup(sockfd, ctx, ssl);
        return -1;
    }

    /* Continue to accept clients until shutdown is issued */
    while (!shutdown) {
        printf("Waiting for a connection...\n");

        /* Accept client connections */
        if ((connd = accept(sockfd, (struct sockaddr*)&clientAddr, &size))
            == -1) {
            fprintf(stderr, "ERROR: failed to accept the connection\n\n");
            util_Cleanup(sockfd, ctx, ssl);
            return -1;
        }

        /* Create a WOLFSSL object */
        if ((ssl = wolfSSL_new(ctx)) == NULL) {
            fprintf(stderr, "ERROR: failed to create WOLFSSL object\n");
            util_Cleanup(sockfd, ctx, ssl);
            return -1;
        }

        /* Attach wolfSSL to the socket */
        wolfSSL_set_fd(ssl, connd);

        /* Establish TLS connection */
        ret = wolfSSL_accept(ssl);
        if (ret != SSL_SUCCESS) {
            fprintf(stderr, "wolfSSL_accept error = %d\n",
                wolfSSL_get_error(ssl, ret));
            util_Cleanup(sockfd, ctx, ssl);
            return -1;
        }

        printf("Client connected successfully\n");

        /* Read the client data into our buff array */
        memset(buff, 0, sizeof(buff));
        if (wolfSSL_read(ssl, buff, sizeof(buff)-1) == -1) {
            fprintf(stderr, "ERROR: failed to read\n");
            util_Cleanup(sockfd, ctx, ssl);
            return -1;
        }

        /* Print to stdout any data the client sends */
        printf("Client: %s\n", buff);

        /* Check for server shutdown command */
        if (strncmp(buff, "shutdown", 8) == 0) {
            printf("Shutdown command issued!\n");
            shutdown = 1;
        }

        /* Write our reply into buff */
        memset(buff, 0, sizeof(buff));
        memcpy(buff, reply, strlen(reply));
        len = strnlen(buff, sizeof(buff));

        /* Reply back to the client */
        if (wolfSSL_write(ssl, buff, (int)len) != len) {
            fprintf(stderr, "ERROR: failed to write\n");
            util_Cleanup(sockfd, ctx, ssl);
            return -1;
        }

        /* Cleanup after this connection */
        wolfSSL_free(ssl);      /* Free the wolfSSL object              */
        close(connd);           /* Close the connection to the client   */
    }

    printf("Shutdown complete\n");

    /* Cleanup and return */
    util_Cleanup(sockfd, ctx, ssl);
    return 0;               /* Return reporting a success               */
}
