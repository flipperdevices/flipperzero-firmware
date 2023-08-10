/* simple_client_example.c
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

#include "simple_client_example.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

#ifndef WOLFSSL_USER_SETTINGS
#include <wolfssl/options.h>
#endif
#include <wolfssl/ssl.h>

#define SERVER_HOST "www.wolfssl.com"
#define SERVER_PORT "443"

int simple_client_example(void)
{
    WOLFSSL_CTX* ctx;
    WOLFSSL* ssl;
    int sockfd, ret;
    
    /* Resolve the server address */
    struct addrinfo hints, *server_addr;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    ret = getaddrinfo(SERVER_HOST, SERVER_PORT, &hints, &server_addr);
    if (ret != 0) {
        printf("Failed to resolve server address: %s\n", gai_strerror(ret));
        return 1;
    }

    /* Create a TCP socket */
    sockfd = socket(server_addr->ai_family,
                    server_addr->ai_socktype,
                    server_addr->ai_protocol);
    if (sockfd == -1) {
        perror("Failed to create socket");
        freeaddrinfo(server_addr);
        return 1;
    }

    /* Connect to the server */
    ret = connect(sockfd, server_addr->ai_addr, server_addr->ai_addrlen);
    if (ret == -1) {
        perror("Failed to connect to server");
        freeaddrinfo(server_addr);
        close(sockfd);
        return 1;
    }

    freeaddrinfo(server_addr);

    /* Initialize wolfSSL library */
    wolfSSL_Init();

    /* Create a new SSL context */
    ctx = wolfSSL_CTX_new(wolfTLSv1_3_client_method());
    if (ctx == NULL) {
        printf("Unable to create SSL context.\n");
        close(sockfd);
        return 1;
    }
    
    /* Load CA certificate into WOLFSSL_CTX
     * NOTE: CERT_PATH macro is set relative to Xcode $(PROJECT_DIR) environment
     * variable in the preprocessor macros section of the project build settings
     * to avoid hardcoding a path in the source code. The CA cert is located at
     * wolfssl/certs/wolfssl-website-ca.pem. */
    ret = wolfSSL_CTX_load_verify_locations(ctx, CERT_PATH, NULL);
    if (ret != WOLFSSL_SUCCESS) {
        printf("ERROR: failed to load %s, please check the file.\n", CERT_PATH);
        wolfSSL_CTX_free(ctx);
        close(sockfd);
        return 1;
    }

    /* Create a new SSL object */
    ssl = wolfSSL_new(ctx);
    if (ssl == NULL) {
        printf("Unable to create SSL object.\n");
        wolfSSL_CTX_free(ctx);
        close(sockfd);
        return 1;
    }

    /* Attach the SSL object to the socket file descriptor */
    wolfSSL_set_fd(ssl, sockfd);

    /* Perform the SSL handshake */
    ret = wolfSSL_connect(ssl);
    if (ret != SSL_SUCCESS) {
        printf("SSL connection failed: %d\n", wolfSSL_get_error(ssl, ret));
        wolfSSL_free(ssl);
        wolfSSL_CTX_free(ctx);
        close(sockfd);
        return 1;
    }

    /* Send an HTTP request */
    const char* request = "GET / HTTP/1.1\r\nHost: www.wolfssl.com\r\n\r\n";
    ret = wolfSSL_write(ssl, request, (int)strlen(request));
    if (ret < 0) {
        printf("Failed to send HTTP request.\n");
    }

    /* Receive and print the server's response */
    char buffer[1024];
    ret = wolfSSL_read(ssl, buffer, sizeof(buffer) - 1);
    if (ret > 0) {
        buffer[ret] = '\0';
        printf("Received %d bytes:\n%s\n", ret, buffer);
    } else {
        printf("Failed to receive server response.\n");
    }

    /* Clean up and close the connection */
    wolfSSL_shutdown(ssl);
    wolfSSL_free(ssl);
    wolfSSL_CTX_free(ctx);
    wolfSSL_Cleanup();
    close(sockfd);

    return 0;
}

