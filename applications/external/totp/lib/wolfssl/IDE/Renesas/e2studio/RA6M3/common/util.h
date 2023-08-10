/* util.h
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

#ifndef _UTIL_H_
#define _UTIL_H_
#include "common/user_settings.h"
#include <wolfssl/ssl.h>
#include "FreeRTOS_IP.h"
#include <stdio.h>

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

#endif /* _UTIL_H */
