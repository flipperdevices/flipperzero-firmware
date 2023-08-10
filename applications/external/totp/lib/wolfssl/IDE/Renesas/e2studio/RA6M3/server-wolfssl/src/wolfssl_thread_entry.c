/* wolfssl_thread_entry.c
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
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/ssl.h>
#include <wolfssl/wolfio.h>
#include "wolfssl_thread_entry.h"

/* FreeRTOS+TCP */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"

/* Project Tools */
#include "common/util.h"
#include <stdio.h>
#include "hal_data.h"

/* the function is called just before main() to set up pins */
/* this needs to be called to setup IO Port */
void R_BSP_WarmStart (bsp_warm_start_event_t event)
{

    if (BSP_WARM_START_POST_C == event) {
        /* C runtime environment and system clocks are setup. */
        /* Configure pins. */
        R_IOPORT_Open(&g_ioport_ctrl, g_ioport.p_cfg);
    }
}


void wolfssl_thread_entry(void *pvParameters) {
    FSP_PARAMETER_NOT_USED(pvParameters);

    /* FreeRTOS+TCP parameters and objects */
    BaseType_t fr_status;
    struct freertos_sockaddr xClient, xBindAddress;
    Socket_t xListeningSocket, xConnectedSocket;
    socklen_t xSize = sizeof(xClient);
    const BaseType_t xBacklog = 1; /* Max number of connections */
    static const TickType_t xReceiveTimeOut = portMAX_DELAY;

    /* Return Code */
    int ret = WOLFSSL_FAILURE;

    /* Send/Receive Message */
    const char *reply = "I hear ya fa shizzle!\n";
    char buff[256];

    /* wolfSSL objects */
    WOLFSSL_CTX *ctx = NULL;
    WOLFSSL *ssl = NULL;

    /* Output to Renesas Debug Virtual Console */
    initialise_monitor_handles();
#ifdef DEBUG_WOLFSSL
    wolfSSL_Debugging_ON();
#endif

    fr_status = FreeRTOS_IPInit(ucIPAddress,ucNetMask, ucGatewayAddress,
                                ucDNSServerAddress, g_ether0_mac_address);

    if (pdPASS != fr_status) {
        printf("Error [%ld]: FreeRTOS_IPInit.\n",fr_status);
        while (1);
    }
    /* Attempt to open the socket. */
    xListeningSocket = FreeRTOS_socket(FREERTOS_AF_INET,
    FREERTOS_SOCK_STREAM,
    FREERTOS_IPPROTO_TCP);
    configASSERT(xListeningSocket != FREERTOS_INVALID_SOCKET);

    /* Set a time out so accept() will just wait for a connection. */
    FreeRTOS_setsockopt(xListeningSocket, 0,
    FREERTOS_SO_RCVTIMEO, &xReceiveTimeOut, sizeof(xReceiveTimeOut));

    xBindAddress.sin_port = (uint16_t) DEFAULT_PORT;
    xBindAddress.sin_port = FreeRTOS_htons(xBindAddress.sin_port);

    configASSERT(xListeningSocket != FREERTOS_INVALID_SOCKET);

    ret = FreeRTOS_bind(xListeningSocket, &xBindAddress, sizeof(xBindAddress));
    if (ret == FR_SOCKET_SUCCESS)
        ret = FreeRTOS_listen(xListeningSocket, xBacklog);

    if (ret != FR_SOCKET_SUCCESS) {
        printf("Error [%d]: FreeRTOS_bind.\n",ret);
        while (1);
    }

    while (1) {
        ret = WOLFSSL_FAILURE;
        xConnectedSocket = FreeRTOS_accept(xListeningSocket, &xClient, &xSize);
        configASSERT(xConnectedSocket != FREERTOS_INVALID_SOCKET);

        /* Create WOLFSSL_CTX object */
        ctx = wolfSSL_CTX_new(wolfSSLv23_server_method_ex((void *)NULL));

        /* Load server certificates into WOLFSSL_CTX */
        if (ctx == NULL) {
            printf("Error: wolfSSL_CTX_new.\n");
            break;
        }
        ret = wolfSSL_CTX_use_certificate_buffer(ctx, CERT_BUF, SIZEOF_CERT_BUF,
                                                 SSL_FILETYPE_ASN1);

        /* Load server key into WOLFSSL_CTX */
        if (ret != WOLFSSL_SUCCESS) {
            printf("Error [%d]: wolfSSL_CTX_use_certificate_buffer.\n",ret);
            break;
        }
        ret = wolfSSL_CTX_use_PrivateKey_buffer(ctx, KEY_BUF, SIZEOF_KEY_BUF,
                                                SSL_FILETYPE_ASN1);

        /* Create a WOLFSSL object */
        if (ret != WOLFSSL_SUCCESS) {
            printf("Error [%d]: wolfSSL_CTX_use_PrivateKey_buffer.\n",ret);
            break;
        }
        ssl = wolfSSL_new(ctx);

        /* Attach wolfSSL to the socket */
        if (ssl == NULL) {
            printf("Error: wolfSSL_new.\n");
            break;
        }
        ret = wolfSSL_set_fd(ssl, (int) xConnectedSocket);

        /* Establish TLS connection */
        if (ret != WOLFSSL_SUCCESS) {
            printf("Error [%d]: wolfSSL_set_fd.\n",ret);
            break;
        }
        ret = wolfSSL_accept(ssl);

        /* Read the client data into our buff array */
        if (ret != WOLFSSL_SUCCESS) {
            printf("Error [%d]: wolfSSL_set_fd.\n",ret);
            break;
        }
        memset(buff, 0, sizeof(buff));
        ret = wolfSSL_read(ssl, buff, sizeof(buff) - 1);
        if (ret < 0)
            break;

        /* Check for server shutdown command */
        if (strncmp(buff, "shutdown", 8) == 0)
            break;

        /* Write our reply into buff */
        memset(buff, 0, sizeof(buff));
        memcpy(buff, reply, strlen(reply));

        /* Reply back to the client */
        ret = wolfSSL_write(ssl, buff, (int) strlen(buff));
        if (ret < 0)
            break;

        /* Cleanup after this connection */
        util_Cleanup(xConnectedSocket, ctx, ssl);
    }

    /* Shutdown TLS Server */
    ret = wolfSSL_shutdown(ssl);
    if (ret == WOLFSSL_SHUTDOWN_NOT_DONE) {
        wolfSSL_shutdown(ssl);
    }

    /* Cleanup connection */
    util_inf_loop(xConnectedSocket, ctx, ssl);
}
