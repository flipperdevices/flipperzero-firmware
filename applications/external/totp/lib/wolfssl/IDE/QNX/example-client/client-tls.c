/* client-tls.c
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* socket includes */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

/* wolfSSL */
#include <wolfssl/options.h>
#include <wolfssl/ssl.h>

/* malloc out buffer and fill it with converted DER to PEM
 * returns pem size on success
 */
static int convertDerToPem(int type, char* file, unsigned char **out)
{
    int derSz, pemSz;
    unsigned char der[4096];
    unsigned char *pem;
    FILE* f;

    f = fopen(file, "rb");
    if (f == NULL) {
        fprintf(stderr, "unable to open cert file %s\n", file);
        return -1;
    }
    derSz = fread(der, 1, sizeof(der), f);
    fclose(f);

    pemSz = wc_DerToPemEx(der, derSz, NULL, 0, NULL, type);
    if (pemSz <= 0) {
        fprintf(stderr, "issue getting pem size needed\n");
        return -1;
    }

    pem = (unsigned char*)malloc(pemSz);
    if (pem == NULL) {
        fprintf(stderr, "issue malloc'ing pem size needed\n");
        return -1;
    }

    pemSz = wc_DerToPemEx(der, derSz, pem, pemSz, NULL, type);
    if (pemSz <= 0) {
        fprintf(stderr, "issue %d converting der to pem\n", pemSz);
        free(pem);
        return -1;
    }
    *out = pem;
    return pemSz;
}


int main(int argc, char** argv)
{
    int                sockfd;
    struct sockaddr_in servAddr;
    char               buff[256];
    size_t             len;
    int                ret;
    int                port;

    int pemSz;
    unsigned char *pem;
    FILE* f;

    /* declare wolfSSL objects */
    WOLFSSL_CTX* ctx;
    WOLFSSL*     ssl;

    /* Check for proper calling convention */
    if (argc != 6) {
        printf("usage: %s <port> <IPv4 address> <CA PEM> <cert DER> <key DER>\n",
                argv[0]);
        return 0;
    }
    port = atoi(argv[1]);

    /* Create a socket that uses an internet IPv4 address,
     * Sets the socket to be stream based (TCP),
     * 0 means choose the default protocol. */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "ERROR: failed to create the socket\n");
        ret = -1;
        goto end;
    }

    /* Initialize the server address struct with zeros */
    memset(&servAddr, 0, sizeof(servAddr));

    /* Fill in the server address */
    servAddr.sin_family = AF_INET;             /* using IPv4      */
    servAddr.sin_port   = htons(port); /* on DEFAULT_PORT */

    /* Get the server IPv4 address from the command line call */
    if (inet_pton(AF_INET, argv[2], &servAddr.sin_addr) != 1) {
        fprintf(stderr, "ERROR: invalid address\n");
        ret = -1;
        goto end;
    }

    /* Connect to the server */
    if ((ret = connect(sockfd, (struct sockaddr*) &servAddr, sizeof(servAddr)))
         == -1) {
        fprintf(stderr, "ERROR: failed to connect\n");
        goto end;
    }

    /*---------------------------------*/
    /* Start of security */
    /*---------------------------------*/
    /* Initialize wolfSSL */
    if ((ret = wolfSSL_Init()) != WOLFSSL_SUCCESS) {
        fprintf(stderr, "ERROR: Failed to initialize the library\n");
        goto socket_cleanup;
    }

    /* Create and initialize WOLFSSL_CTX */
    if ((ctx = wolfSSL_CTX_new(wolfSSLv23_client_method())) == NULL) {
        fprintf(stderr, "ERROR: failed to create WOLFSSL_CTX\n");
        ret = -1;
        goto socket_cleanup;
    }

    /* load cert and convert DER to PEM using dynamic length */
    pemSz = convertDerToPem(CERT_TYPE, argv[4], &pem);
    if (pemSz <= 0) {
        fprintf(stderr, "ERROR: converting DER cert to PEM\n");
        ret = -1;
        goto socket_cleanup;
    }

    if (wolfSSL_CTX_use_certificate_buffer(ctx, pem, pemSz,
                WOLFSSL_FILETYPE_PEM) != WOLFSSL_SUCCESS) {
        fprintf(stderr, "issue loading in pem cert\n");
        ret = -1;
        free(pem);
        goto socket_cleanup;
    }
    free(pem);

    /* load key and convert DER to PEM using dynamic length */
    pemSz = convertDerToPem(PRIVATEKEY_TYPE, argv[5], &pem);
    if (pemSz <= 0) {
        fprintf(stderr, "ERROR: converting DER key to PEM\n");
        ret = -1;
        goto socket_cleanup;
    }

    if (wolfSSL_CTX_use_PrivateKey_buffer(ctx, pem, pemSz,
                WOLFSSL_FILETYPE_PEM) != WOLFSSL_SUCCESS) {
        fprintf(stderr, "issue loading in pem key\n");
        ret = -1;
        free(pem);
        goto socket_cleanup;
    }
    free(pem);

    /* Load client certificates into WOLFSSL_CTX */
    f = fopen(argv[3], "rb");
    if (f == NULL) {
        fprintf(stderr, "unable to open %s\n", argv[3]);
        ret = -1;
        goto socket_cleanup;
    }
    fseek(f, 0, SEEK_END);
    pemSz = ftell(f);
    rewind(f);
    pem = malloc(pemSz);
    if (pem == NULL) {
        fclose(f);
        ret = -1;
        goto socket_cleanup;
    }
    pemSz = fread(pem, 1, pemSz, f);
    fclose(f);

    ret = wolfSSL_CTX_load_verify_buffer(ctx, pem, pemSz, WOLFSSL_FILETYPE_PEM);
    if (ret != SSL_SUCCESS) {
        fprintf(stderr, "ERROR %d: failed to load %s, please check the file.\n",
                ret, argv[3]);
        free(pem);
        goto ctx_cleanup;
    }
    free(pem);

    /* Create a WOLFSSL object */
    if ((ssl = wolfSSL_new(ctx)) == NULL) {
        fprintf(stderr, "ERROR: failed to create WOLFSSL object\n");
        ret = -1;
        goto ctx_cleanup;
    }

    /* Attach wolfSSL to the socket */
    if ((ret = wolfSSL_set_fd(ssl, sockfd)) != WOLFSSL_SUCCESS) {
        fprintf(stderr, "ERROR: Failed to set the file descriptor\n");
        goto cleanup;
    }

    /* Connect to wolfSSL on the server side */
    if ((ret = wolfSSL_connect(ssl)) != SSL_SUCCESS) {
        fprintf(stderr, "ERROR: failed to connect to wolfSSL\n");
        goto cleanup;
    }

    /* Get a message for the server from stdin */
    printf("Message for server: ");
    memset(buff, 0, sizeof(buff));
    if (fgets(buff, sizeof(buff), stdin) == NULL) {
        fprintf(stderr, "ERROR: failed to get message for server\n");
        ret = -1;
        goto cleanup;
    }
    len = strnlen(buff, sizeof(buff));

    /* Send the message to the server */
    if ((ret = wolfSSL_write(ssl, buff, len)) != len) {
        fprintf(stderr, "ERROR: failed to write entire message\n");
        fprintf(stderr, "%d bytes of %d bytes were sent", ret, (int) len);
        goto cleanup;
    }

    /* Read the server data into our buff array */
    memset(buff, 0, sizeof(buff));
    if ((ret = wolfSSL_read(ssl, buff, sizeof(buff)-1)) == -1) {
        fprintf(stderr, "ERROR: failed to read\n");
        goto cleanup;
    }

    /* Print to stdout any data the server sends */
    printf("Server: %s\n", buff);

    /* Cleanup and return */
cleanup:
    wolfSSL_free(ssl);      /* Free the wolfSSL object                  */
ctx_cleanup:
    wolfSSL_CTX_free(ctx);  /* Free the wolfSSL context object          */
    wolfSSL_Cleanup();      /* Cleanup the wolfSSL environment          */
socket_cleanup:
    close(sockfd);          /* Close the connection to the server       */
end:
    return ret;               /* Return reporting a success               */
}
