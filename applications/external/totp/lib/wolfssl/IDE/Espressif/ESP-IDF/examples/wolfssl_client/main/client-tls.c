/* client-tls-callback.c
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
/* the usual suspects */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

/* ESP specific */
#include "wifi_connect.h"

/* socket includes */
#include "lwip/netdb.h"
#include "lwip/sockets.h"

/* wolfSSL */
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/ssl.h>
#include <wolfssl/certs_test.h>

#ifdef WOLFSSL_TRACK_MEMORY
    #include <wolfssl/wolfcrypt/mem_track.h>
#endif

static const char* const TAG = "tls_client";

#if defined(DEBUG_WOLFSSL)

static void ShowCiphers(void)
{
    char ciphers[4096];

    int ret = wolfSSL_get_ciphers(ciphers, (int)sizeof(ciphers));

    if (ret == WOLFSSL_SUCCESS)
        printf("%s\n", ciphers);
}

#endif

#if defined(WOLFSSL_ESPWROOM32SE) && defined(HAVE_PK_CALLBACKS) \
                                  && defined(WOLFSSL_ATECC508A)

#include "wolfssl/wolfcrypt/port/atmel/atmel.h"

/* when you want to use custom slot allocation */
/* enable the definition CUSTOM_SLOT_ALLOCATION.*/

#if defined(CUSTOM_SLOT_ALLOCATION)

static byte mSlotList[ATECC_MAX_SLOT];

int atmel_set_slot_allocator(atmel_slot_alloc_cb alloc,
                                                atmel_slot_dealloc_cb dealloc);
/* initialize slot array */
void my_atmel_slotInit()
{
    int i;

    for(i=0;i<ATECC_MAX_SLOT; i++) {
        mSlotList[i] = ATECC_INVALID_SLOT;
    }
}
/* allocate slot depending on slotType */
int my_atmel_alloc(int slotType)
{
    int i, slot = -1;

    switch(slotType){
        case ATMEL_SLOT_ENCKEY:
            slot = 2;
            break;
        case ATMEL_SLOT_DEVICE:
            slot = 0;
            break;
        case ATMEL_SLOT_ECDHE:
            slot = 0;
            break;
        case ATMEL_SLOT_ECDHE_ENC:
            slot = 4;
            break;
        case ATMEL_SLOT_ANY:
            for(i=0;i<ATECC_MAX_SLOT;i++){
                if(mSlotList[i] == ATECC_INVALID_SLOT){
                    slot = i;
                    break;
                }
            }
    }

    return slot;
}
/* free slot array       */
void my_atmel_free(int slotId)
{
    if(slotId >= 0 && slotId < ATECC_MAX_SLOT){
        mSlotList[slotId] = ATECC_INVALID_SLOT;
    }
}
#endif /* CUSTOM_SLOT_ALLOCATION                                       */
#endif /* WOLFSSL_ESPWROOM32SE && HAVE_PK_CALLBACK && WOLFSSL_ATECC508A */

/* client task */
void tls_smp_client_task()
{
    int ret;
    int sockfd;
    int doPeerCheck;
    int sendGet;
    struct sockaddr_in servAddr;
    char buff[256];
    const char* ch = TLS_SMP_TARGET_HOST;
    size_t len;
    struct hostent *hp;
    struct ip4_addr *ip4_addr;
    const char sndMsg[] = "GET /index.html HTTP/1.0\r\n\r\n";

    /* declare wolfSSL objects */
    WOLFSSL_CTX *ctx;
    WOLFSSL *ssl;

    WOLFSSL_ENTER("tls_smp_client_task");

    doPeerCheck = 0;
    sendGet = 0;

#ifdef DEBUG_WOLFSSL
    WOLFSSL_MSG("Debug ON");
    wolfSSL_Debugging_ON();
    ShowCiphers();
#endif
    /* Initialize wolfSSL */
    wolfSSL_Init();

    /* Create a socket that uses an internet IPv4 address,
     * Sets the socket to be stream based (TCP),
     * 0 means choose the default protocol. */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        ESP_LOGE(TAG,"ERROR: failed to create the socket\n");
    }

    ESP_LOGI(TAG, "get target IP address");

    hp = gethostbyname(TLS_SMP_TARGET_HOST);
    if (!hp) {
         ESP_LOGE(TAG, "Failed to get host name.");
         ip4_addr = NULL;
    } else {

         ip4_addr = (struct ip4_addr *)hp->h_addr;
         ESP_LOGI(TAG, IPSTR, IP2STR(ip4_addr));
    }
    /* Create and initialize WOLFSSL_CTX */
    if ((ctx = wolfSSL_CTX_new(wolfSSLv23_client_method())) == NULL) {
        ESP_LOGE(TAG,"ERROR: failed to create WOLFSSL_CTX\n");
    }
    WOLFSSL_MSG("Loading...cert");
    /* Load client certificates into WOLFSSL_CTX */
    if ((ret = wolfSSL_CTX_load_verify_buffer(ctx, ca_cert_der_2048,
        sizeof_ca_cert_der_2048, WOLFSSL_FILETYPE_ASN1)) != SSL_SUCCESS) {
        ESP_LOGE(TAG,"ERROR: failed to load %d, please check the file.\n",ret);
    }
    /* not peer check */
    if( doPeerCheck == 0 ){
        wolfSSL_CTX_set_verify(ctx, WOLFSSL_VERIFY_NONE, 0);
    } else {
        WOLFSSL_MSG("Loading... our cert");
        /* load our certificate */
   	    if ((ret = wolfSSL_CTX_use_certificate_chain_buffer_format(ctx, client_cert_der_2048,
            sizeof_client_cert_der_2048, WOLFSSL_FILETYPE_ASN1)) != SSL_SUCCESS) {
            ESP_LOGE(TAG,"ERROR: failed to load chain %d, please check the file.\n",ret);
        }

        if ((ret = wolfSSL_CTX_use_PrivateKey_buffer(ctx, client_key_der_2048,
            sizeof_client_key_der_2048, WOLFSSL_FILETYPE_ASN1))  != SSL_SUCCESS) {
            wolfSSL_CTX_free(ctx); ctx = NULL;
            ESP_LOGE(TAG,"ERROR: failed to load key %d, please check the file.\n", ret);
        }

        wolfSSL_CTX_set_verify(ctx, WOLFSSL_VERIFY_PEER, 0);
    }

    /* Initialize the server address struct with zeros */
    memset(&servAddr, 0, sizeof(servAddr));

    /* Fill in the server address */
    servAddr.sin_family = AF_INET;           /* using IPv4      */
    servAddr.sin_port = htons(DEFAULT_PORT); /* on DEFAULT_PORT */

    if(*ch >= '1' && *ch <= '9') {
        /* Get the server IPv4 address from the command line call */
        WOLFSSL_MSG("inet_pton");
        if ((ret = inet_pton(AF_INET, TLS_SMP_TARGET_HOST,
                 &servAddr.sin_addr)) != 1) {
            ESP_LOGE(TAG,"ERROR: invalid address ret=%d\n", ret);
        }
    } else {
        servAddr.sin_addr.s_addr = ip4_addr->addr;
    }

    /* Connect to the server */
    sprintf(buff, "Connecting to server....%s(port:%d)", TLS_SMP_TARGET_HOST
                                                      , DEFAULT_PORT);
    WOLFSSL_MSG(buff);
    printf("%s\n",buff);
    if ((ret = connect(sockfd, (struct sockaddr *)&servAddr,
                                    sizeof(servAddr))) == -1){
        ESP_LOGE(TAG,"ERROR: failed to connect ret=%d\n", ret);
    }

    WOLFSSL_MSG("Create a WOLFSSL object");
    /* Create a WOLFSSL object */
    if ((ssl = wolfSSL_new(ctx)) == NULL) {
        ESP_LOGE(TAG,"ERROR: failed to create WOLFSSL object\n");
    }

    /* when using atecc608a on esp32-wroom-32se */
#if defined(WOLFSSL_ESPWROOM32SE) && defined(HAVE_PK_CALLBACKS) \
                                  && defined(WOLFSSL_ATECC508A)
    atcatls_set_callbacks(ctx);
    /* when using custom slot-allocation */
    #if defined(CUSTOM_SLOT_ALLOCATION)
    my_atmel_slotInit();
    atmel_set_slot_allocator(my_atmel_alloc, my_atmel_free);
    #endif
#endif

    /* Attach wolfSSL to the socket */
    wolfSSL_set_fd(ssl, sockfd);

    WOLFSSL_MSG("Connect to wolfSSL on the server side");
    /* Connect to wolfSSL on the server side */
    if (wolfSSL_connect(ssl) != SSL_SUCCESS) {
        ESP_LOGE(TAG,"ERROR: failed to connect to wolfSSL\n");
    }

    /* Get a message for the server from stdin */
    WOLFSSL_MSG("Message for server: ");
    memset(buff, 0, sizeof(buff));

    if(sendGet){
        printf("SSL connect ok, sending GET...\n");
        len = XSTRLEN(sndMsg);
        strncpy(buff, sndMsg, len);
        buff[len] = '\0';
    } else {
        sprintf(buff, "message from esp32 tls client\n");
        len = strnlen(buff, sizeof(buff));
    }
    /* Send the message to the server */
    if (wolfSSL_write(ssl, buff, len) != len) {
        ESP_LOGE(TAG,"ERROR: failed to write\n");
    }

    /* Read the server data into our buff array */
    memset(buff, 0, sizeof(buff));
    if (wolfSSL_read(ssl, buff, sizeof(buff) - 1) == -1) {
        ESP_LOGE(TAG,"ERROR: failed to read\n");
    }

    /* Print to stdout any data the server sends */
    printf("Server:");
    printf("%s", buff);
    /* Cleanup and return */
    wolfSSL_free(ssl);     /* Free the wolfSSL object                  */
    wolfSSL_CTX_free(ctx); /* Free the wolfSSL context object          */
    wolfSSL_Cleanup();     /* Cleanup the wolfSSL environment          */
    close(sockfd);         /* Close the connection to the server       */

    vTaskDelete(NULL);

    return;                /* Return reporting a success               */
}
