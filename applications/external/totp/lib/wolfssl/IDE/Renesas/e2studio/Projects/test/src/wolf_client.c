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

#include <stdio.h>
#include <string.h>
#include "r_t4_itcpip.h"

#include "wolfssl/wolfcrypt/settings.h"
#include "wolfssl/ssl.h"
#include "wolfssl/certs_test.h"
#include "key_data.h"

#define SIMPLE_TLSSEVER_IP       "192.168.1.45"
#define SIMPLE_TLSSERVER_PORT    "11111"

ER    t4_tcp_callback(ID cepid, FN fncd , VP p_parblk);
uint32_t g_encrypted_root_public_key[140];

static WOLFSSL_CTX *client_ctx;

static int my_IORecv(WOLFSSL* ssl, char* buff, int sz, void* ctx)
{
    int ret;
    ID  cepid;

    if(ctx != NULL)
        cepid = *(ID *)ctx;
    else
        return WOLFSSL_CBIO_ERR_GENERAL;

    ret = tcp_rcv_dat(cepid, buff, sz, TMO_FEVR);
    if(ret > 0)
        return ret;
    else
        return WOLFSSL_CBIO_ERR_GENERAL;
}

static int my_IOSend(WOLFSSL* ssl, char* buff, int sz, void* ctx)
{
    int ret;
    ID  cepid;

    if(ctx != NULL)
        cepid = *(ID *)ctx;
    else
        return WOLFSSL_CBIO_ERR_GENERAL;

    ret = tcp_snd_dat(cepid, buff, sz, TMO_FEVR);
    if(ret == sz)
        return ret;
    else
        return WOLFSSL_CBIO_ERR_GENERAL;
}

static int getIPaddr(char *arg)
{
    int a1, a2, a3, a4;
    if(sscanf(arg, "%d.%d.%d.%d", &a1, &a2, &a3, &a4) == 4)
         return (a1 << 24) | (a2 << 16) | (a3 << 8) | a4;
    else return 0;
}

static int getPort(char *arg)
{
    int port;
    if(sscanf(arg, "%d", &port) == 1)
         return port;
    else return 0;
}

void wolfSSL_TLS_client_init(const char* cipherlist)
{

    #ifndef NO_FILESYSTEM
        #ifdef USE_ECC_CERT
        char *cert       = "./certs/ca-ecc-cert.pem";
        #else
        char *cert       = "./certs/ca-cert.pem";
        #endif
    #else
        #ifdef USE_ECC_CERT
        const unsigned char *cert       = ca_ecc_der_256;
        #define  SIZEOF_CERT sizeof_ca_ecc_der_256
        #else
        const unsigned char *cert       = ca_cert_der_2048;
        #define  SIZEOF_CERT sizeof_ca_cert_der_2048
        #endif
    #endif

    wolfSSL_Init();
    #ifdef DEBUG_WOLFSSL
        wolfSSL_Debugging_ON();
    #endif

    /* Create and initialize WOLFSSL_CTX */
    if ((client_ctx = wolfSSL_CTX_new(wolfTLSv1_2_client_method_ex((void *)NULL))) == NULL) {
        printf("ERROR: failed to create WOLFSSL_CTX\n");
        return;
    }

    #if !defined(NO_FILESYSTEM)
    if (wolfSSL_CTX_load_verify_locations(client_ctx, cert, 0) != SSL_SUCCESS) {
        printf("ERROR: can't load \"%s\"\n", cert);
        return NULL;
    }
    #else
    if (wolfSSL_CTX_load_verify_buffer(client_ctx, cert, SIZEOF_CERT, SSL_FILETYPE_ASN1) != SSL_SUCCESS){
           printf("ERROR: can't load certificate data\n");
       return;
    }
    #endif

    /* Register callbacks */
    wolfSSL_SetIORecv(client_ctx, my_IORecv);
    wolfSSL_SetIOSend(client_ctx, my_IOSend);

    /* use specific cipher */
    if (cipherlist != NULL && wolfSSL_CTX_set_cipher_list(client_ctx, cipherlist) != WOLFSSL_SUCCESS) {
        wolfSSL_CTX_free(client_ctx); client_ctx = NULL;
        printf("client can't set cipher list 1");
    }
}

void wolfSSL_TLS_client( )
{
    ID cepid = 1;
    ER ercd;
    int ret;
    WOLFSSL_CTX *ctx = (WOLFSSL_CTX *)client_ctx;
    WOLFSSL *ssl;
    #define BUFF_SIZE 256
    static const char sendBuff[]= "Hello Server\n" ;
    char    rcvBuff[BUFF_SIZE] = {0};
    static T_IPV4EP my_addr = { 0, 0 };
    T_IPV4EP dst_addr;

    if((dst_addr.ipaddr = getIPaddr(SIMPLE_TLSSEVER_IP)) == 0){
        printf("ERROR: IP address\n");
        return;
    }
    if((dst_addr.portno = getPort(SIMPLE_TLSSERVER_PORT)) == 0){
        printf("ERROR: IP address\n");
        return;
    }

    if((ercd = tcp_con_cep(cepid, &my_addr, &dst_addr, TMO_FEVR)) != E_OK) {
        printf("ERROR TCP Connect: %d\n", ercd);
        return;
    }

    if((ssl = wolfSSL_new(ctx)) == NULL) {
        printf("ERROR wolfSSL_new: %d\n", wolfSSL_get_error(ssl, 0));
        return;
    }

    /* set callback context */
    wolfSSL_SetIOReadCtx(ssl, (void *)&cepid);
    wolfSSL_SetIOWriteCtx(ssl, (void *)&cepid);

    if(wolfSSL_connect(ssl) != SSL_SUCCESS) {
        printf("ERROR SSL connect: %d\n",  wolfSSL_get_error(ssl, 0));
        return;
    }

    if (wolfSSL_write(ssl, sendBuff, strlen(sendBuff)) != strlen(sendBuff)) {
        printf("ERROR SSL write: %d\n", wolfSSL_get_error(ssl, 0));
        return;
    }

    if ((ret=wolfSSL_read(ssl, rcvBuff, BUFF_SIZE)) < 0) {
        printf("ERROR SSL read: %d\n", wolfSSL_get_error(ssl, 0));
        return;
    }

    rcvBuff[ret] = '\0' ;
    printf("Received: %s\n", rcvBuff);

    /* frees all data before client termination */
    wolfSSL_free(ssl);
    wolfSSL_CTX_free(ctx);
    wolfSSL_Cleanup();

    tcp_sht_cep(cepid);
    tcp_cls_cep(cepid, TMO_FEVR);

    return;
}
