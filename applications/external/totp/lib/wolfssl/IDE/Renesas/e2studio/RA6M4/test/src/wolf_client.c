/* wolf_client.c
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

#include "wolfssl_demo.h"

#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include "wolfssl/wolfcrypt/settings.h"
#include "wolfssl/ssl.h"
#include "wolfssl/certs_test.h"

uint32_t g_encrypted_root_public_key[140];
WOLFSSL_CTX *client_ctx = NULL;

extern uint8_t g_ether0_mac_address[6];
static const byte ucIPAddress[4]          = { 192, 168, 11, 241 };
static const byte ucNetMask[4]            = { 255, 255, 255, 0 };
static const byte ucGatewayAddress[4]     = { 192, 168, 11, 1 };
static const byte ucDNSServerAddress[4]   = { 192, 168, 11, 1 };

#define FR_SOCKET_SUCCESS 0

#ifdef TLS_MULTITHREAD_TEST
     xSemaphoreHandle exit_semaph;
     extern User_SCEPKCbInfo guser_PKCbInfo_taskA;
     extern User_SCEPKCbInfo guser_PKCbInfo_taskB;
#else
     extern User_SCEPKCbInfo guser_PKCbInfo;
#endif

int SEGGER_RTT_vprintf(unsigned BufferIndex, const char * sFormat, va_list * pParamList);

static int msg(const char* pname, int l,
                         const char * sFormat, ...)
{
    int r = 0;
    va_list ParamList;

    va_start(ParamList, sFormat);

    printf("[%s][%02d] ", pname, l);
    r = SEGGER_RTT_vprintf(0, sFormat, &ParamList);

    va_end(ParamList);

    return r;
}

void TCPInit( )
{
   BaseType_t fr_status;
  
   /* FreeRTOS+TCP Ethernet and IP Setup */
   fr_status = FreeRTOS_IPInit(ucIPAddress,
                               ucNetMask,
                               ucGatewayAddress,
                               ucDNSServerAddress,
                               g_ether0_mac_address);
   if (pdPASS != fr_status) {
       printf("Error [%ld]: FreeRTOS_IPInit.\n",fr_status);
   }
}

void wolfSSL_TLS_client_init()
{
    #ifndef NO_FILESYSTEM
        #ifdef USE_ECC_CERT
        char *cert       = "./certs/ca-ecc-cert.pem";
        #else
        char *cert       = "./certs/ca-cert.pem";
        #endif
    #else
        #ifdef USE_CERT_BUFFERS_256
        const unsigned char *cert       = ca_ecc_cert_der_256;
        #define  SIZEOF_CERT sizeof_ca_ecc_cert_der_256
        #else
        const unsigned char *cert       = ca_cert_der_2048;
        #define  SIZEOF_CERT sizeof_ca_cert_der_2048
        #endif
    #endif

    wolfSSL_Init();

    /* Create and initialize WOLFSSL_CTX */
    if ((client_ctx = wolfSSL_CTX_new(
                        wolfSSLv23_client_method_ex((void *)NULL))) == NULL) {
        printf("ERROR: failed to create WOLFSSL_CTX\n");
        return;
    }
    #if defined(WOLFSSL_RENESAS_SCEPROTECT)
          /* set callback functions for ECC */
          wc_sce_set_callbacks(client_ctx);
    #endif
    
    #if !defined(NO_FILESYSTEM)
    if (wolfSSL_CTX_load_verify_locations(client_ctx, cert, 0) != SSL_SUCCESS) {
        printf("ERROR: can't load \"%s\"\n", cert);
        return NULL;
    }
    #else
    if (wolfSSL_CTX_load_verify_buffer(client_ctx, cert, SIZEOF_CERT, 
                                            SSL_FILETYPE_ASN1) != SSL_SUCCESS){
           printf("ERROR: can't load certificate data\n");
       return;
    }
    #endif
}

void wolfSSL_TLS_client_do(void *pvParam)
{

    int ret;
    int i = 0;
#if defined(TLS_MULTITHREAD_TEST)
    BaseType_t xStatus;
#endif
    TestInfo* p = (TestInfo*)pvParam;
    /* FreeRTOS+TCP Objects */
    socklen_t xSize = sizeof(struct freertos_sockaddr);
    xSocket_t xClientSocket = NULL;
    struct freertos_sockaddr xRemoteAddress;
    
    WOLFSSL_CTX *ctx = (WOLFSSL_CTX *)p->ctx;
    WOLFSSL *ssl = NULL;
    const char* pcName = p->name;

    #define BUFF_SIZE 256
    static const char sendBuff[]= "Hello Server\n" ;
    
    char    rcvBuff[BUFF_SIZE] = {0};
    
    i = p->id;
    /* Client Socket Setup */
    xRemoteAddress.sin_port = FreeRTOS_htons(p->port);
    xRemoteAddress.sin_addr = FreeRTOS_inet_addr(SERVER_IP);

     /* Create a FreeRTOS TCP Socket and connect */
     xClientSocket = FreeRTOS_socket(FREERTOS_AF_INET,
                                             FREERTOS_SOCK_STREAM,
                                             FREERTOS_IPPROTO_TCP);

     configASSERT(xClientSocket != FREERTOS_INVALID_SOCKET);

     FreeRTOS_bind(xClientSocket, NULL, sizeof(xSize));

     /* Client Socket Connect */
     ret = FreeRTOS_connect(xClientSocket,
                                 &xRemoteAddress,
                                 sizeof(xRemoteAddress));

     if (ret != FR_SOCKET_SUCCESS) {
         msg(pcName, i, " Error [%d]: FreeRTOS_connect.\n", ret);
         goto out;
     }

     #if defined(TLS_MULTITHREAD_TEST)
     msg(pcName, i, " Ready to connect.\n");
     xStatus = xSemaphoreTake(p->xBinarySemaphore, portMAX_DELAY);
     if (xStatus != pdTRUE) {
        msg(pcName, i, " Error : Failed to xSemaphoreTake\n");
        goto out;
     }
     #endif

     msg(pcName, i, " Start to connect to the server.\n");

     if((ssl = wolfSSL_new(ctx)) == NULL) {
          msg(pcName, i, " ERROR wolfSSL_new: %d\n", wolfSSL_get_error(ssl, 0));
          goto out;
     }
     #if defined(WOLFSSL_RENESAS_SCEPROTECT)

       /* Set callback CTX */
        #if !defined(TLS_MULTITHREAD_TEST)
        
        memset(&guser_PKCbInfo, 0, sizeof(User_SCEPKCbInfo));
        guser_PKCbInfo.devId = 0;
        wc_sce_set_callback_ctx(ssl, (void*)&guser_PKCbInfo);
        
        #else
        if (p->port - DEFAULT_PORT == 0) {
           memset(&guser_PKCbInfo_taskA, 0, sizeof(User_SCEPKCbInfo));
           wc_sce_set_callback_ctx(ssl, (void*)&guser_PKCbInfo_taskA);
        }
        else {
           memset(&guser_PKCbInfo_taskB, 0, sizeof(User_SCEPKCbInfo));
           wc_sce_set_callback_ctx(ssl, (void*)&guser_PKCbInfo_taskB);
        }
        #endif
        
     #endif

     /* Attach wolfSSL to the socket */
     ret = wolfSSL_set_fd(ssl, (int) xClientSocket);
     if (ret != WOLFSSL_SUCCESS) {
         msg(pcName, i, " Error [%d]: wolfSSL_set_fd.\n",ret);
     }

     msg(pcName, i, "  Cipher : %s\n", 
                                    (p->cipher == NULL) ? "NULL" : p->cipher);
     /* use specific cipher */
     if (p->cipher != NULL && wolfSSL_set_cipher_list(ssl, p->cipher) 
                                                           != WOLFSSL_SUCCESS) {
          msg(pcName, i, " client can't set cipher list 1");
          goto out;
     }

     #ifdef DEBUG_WOLFSSL
     wolfSSL_Debugging_ON();
     #endif

     if(wolfSSL_connect(ssl) != SSL_SUCCESS) {
        msg(pcName, i, " ERROR SSL connect: %d\n",  wolfSSL_get_error(ssl, 0));
        goto out;
     }

     #ifdef DEBUG_WOLFSSL
     wolfSSL_Debugging_OFF();
     #endif

     if (wolfSSL_write(ssl, sendBuff, (int)strlen(sendBuff)) 
                                                    != (int)strlen(sendBuff)) {
        msg(pcName, i, " ERROR SSL write: %d\n", wolfSSL_get_error(ssl, 0));
        goto out;
     }

     if ((ret=wolfSSL_read(ssl, rcvBuff, BUFF_SIZE)) < 0) {
         msg(pcName, i, " ERROR SSL read: %d\n", wolfSSL_get_error(ssl, 0));
         goto out;
     }

     rcvBuff[ret] = '\0' ;
     msg(pcName, i, " Received: %s\n\n", rcvBuff);

 out:
    if (ssl) {
        wolfSSL_shutdown(ssl);
        wolfSSL_free(ssl);
        ssl = NULL;
        /* need to reset callback */
        wc_sce_set_callbacks(client_ctx);
    }
    /* clean up socket */
    if (xClientSocket) {
        FreeRTOS_shutdown(xClientSocket, FREERTOS_SHUT_RDWR);
        FreeRTOS_closesocket(xClientSocket);
        xClientSocket = NULL;
    }

#ifdef TLS_MULTITHREAD_TEST
    xSemaphoreGive(exit_semaph);
    vTaskDelete(NULL);
#endif

}

void wolfSSL_TLS_cleanup()
{
     if (client_ctx) {
          wolfSSL_CTX_free(client_ctx);
     }

     wolfSSL_Cleanup();

}
