/* sctp-client-dtls.c
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


/* wolfssl */
#ifndef WOLFSSL_USER_SETTINGS
    #include <wolfssl/options.h>
#endif
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/ssl.h>

#if defined(WOLFSSL_SCTP) && defined(WOLFSSL_DTLS)
/* sctp */
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

/* std */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define cacert "./certs/ca-cert.pem"

static int err_sys(const char* msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
#endif /* WOLFSSL_SCTP && WOLFSSL_DTLS */

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
#if defined(WOLFSSL_SCTP) && defined(WOLFSSL_DTLS)
    int sd = socket(PF_INET, SOCK_STREAM, IPPROTO_SCTP);

    if (sd < 0)
        err_sys("sctp socket error");

    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = inet_addr("127.0.0.1");
    sa.sin_port = htons(12345);

    int ret = connect(sd, (struct sockaddr*)&sa, sizeof(sa));
    if (ret < 0)
        err_sys("sctp connect error");

    const char* response = "hello there";
    char buffer[80];

    WOLFSSL_CTX* ctx = wolfSSL_CTX_new(wolfDTLSv1_2_client_method());
    if (ctx == NULL)
        err_sys("ctx new dtls client failed");

    ret = wolfSSL_CTX_dtls_set_sctp(ctx);
    if (ret != WOLFSSL_SUCCESS)
        err_sys("set sctp mode failed");

    ret = wolfSSL_CTX_load_verify_locations(ctx, cacert, NULL);
    if (ret != WOLFSSL_SUCCESS)
        err_sys("ca cert error");

    WOLFSSL* ssl = wolfSSL_new(ctx);
    if (ssl == NULL)
        err_sys("ssl new dtls client failed");

    wolfSSL_set_fd(ssl, sd);

    ret = wolfSSL_connect(ssl);
    if (ret != WOLFSSL_SUCCESS)
        err_sys("ssl connect failed");

    printf("TLS version is %s\n", wolfSSL_get_version(ssl));
    printf("Cipher Suite is %s\n",
           wolfSSL_CIPHER_get_name(wolfSSL_get_current_cipher(ssl)));

    wolfSSL_write(ssl, response, (int)strlen(response));
    int got = wolfSSL_read(ssl, buffer, sizeof(buffer));
    if (got > 0) {
        buffer[got] = 0;
        printf("server said: %s\n", buffer);
    }

    unsigned char bigBuf[4096];
    unsigned int i;

    for (i = 0; i < (int)sizeof(bigBuf); i++)
        bigBuf[i] = (unsigned char)(i & 0xFF);
    wolfSSL_write(ssl, bigBuf, sizeof(bigBuf));
    memset(bigBuf, 0, sizeof(bigBuf));

    wolfSSL_read(ssl, bigBuf, sizeof(bigBuf));
    for (i = 0; i < sizeof(bigBuf); i++) {
        if (bigBuf[i] != (unsigned char)(i & 0xFF)) {
            fprintf(stderr, "big message check fail\n");
            break;
        }
    }

    wolfSSL_shutdown(ssl);
    wolfSSL_free(ssl);
    wolfSSL_CTX_free(ctx);

    close(sd);
#endif /* WOLFSSL_SCTP && WOLFSSL_DTLS */

    return 0;
}
