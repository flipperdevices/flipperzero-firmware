/* sctp-server.c
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

#ifndef WOLFSSL_USER_SETTINGS
    #include <wolfssl/options.h>
#endif
#include <wolfssl/wolfcrypt/settings.h>

#ifdef WOLFSSL_SCTP
/* sctp */
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

/* std */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#endif /* WOLFSSL_SCTP */

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
#ifdef WOLFSSL_SCTP
    int sd = socket(PF_INET, SOCK_STREAM, IPPROTO_SCTP);

    if (sd < 0)
        perror("sctp socket error");

    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl(INADDR_ANY);
    sa.sin_port = htons(12345);

    int ret = bind(sd, (struct sockaddr*)&sa, sizeof(sa));
    if (ret < 0)
        perror("sctp bind error");

    listen(sd, 3);

    int client_sd = accept(sd, NULL, NULL);
    if (client_sd < 0)
        perror("sctp accept error");

    const char* response = "hi there";
    char buffer[80];

    int got = (int)recv(client_sd, buffer, sizeof(buffer), 0);
    if (got > 0) {
        buffer[got] = 0;
        printf("client said: %s\n", buffer);
    }
    send(client_sd, response, strlen(response), 0);


    close(sd);
#endif /* WOLFSSL_SCTP */
    return 0;
}
