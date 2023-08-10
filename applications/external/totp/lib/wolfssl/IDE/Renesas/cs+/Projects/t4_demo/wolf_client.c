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
#include "wolf_demo.h"

ER    t4_tcp_callback(ID cepid, FN fncd , VP p_parblk);

static int my_IORecv(WOLFSSL* ssl, char* buff, int sz, void* ctx)
{
    int ret;
    ID  cepid;

    if(ctx != NULL)cepid = *(ID *)ctx;
    else return WOLFSSL_CBIO_ERR_GENERAL;

    ret = tcp_rcv_dat(cepid, buff, sz, TMO_FEVR);
    if(ret > 0)return ret;
    else         return WOLFSSL_CBIO_ERR_GENERAL;
}

static int my_IOSend(WOLFSSL* ssl, char* buff, int sz, void* ctx)
{
    int ret;
    ID  cepid;

    if(ctx != NULL)cepid = *(ID *)ctx;
    else return WOLFSSL_CBIO_ERR_GENERAL;

    ret = tcp_snd_dat(cepid, buff, sz, TMO_FEVR);
    if(ret == sz)return ret;
    else         return WOLFSSL_CBIO_ERR_GENERAL;
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

WOLFSSL_CTX *wolfSSL_TLS_client_init()
{

    WOLFSSL_CTX* ctx;
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
    if ((ctx = wolfSSL_CTX_new(wolfTLSv1_2_client_method_ex((void *)NULL))) == NULL) {
        printf("ERROR: failed to create WOLFSSL_CTX\n");
        return NULL;
    }

    #if !defined(NO_FILESYSTEM)
    if (wolfSSL_CTX_load_verify_locations(ctx, cert, 0) != SSL_SUCCESS) {
        printf("ERROR: can't load \"%s\"\n", cert);
        return NULL;
    }
    #else
    if (wolfSSL_CTX_load_verify_buffer(ctx, cert, SIZEOF_CERT, SSL_FILETYPE_ASN1) != SSL_SUCCESS){
           printf("ERROR: can't load certificate data\n");
       return NULL;
    }
    #endif


    /* Register callbacks */
    wolfSSL_SetIORecv(ctx, my_IORecv);
    wolfSSL_SetIOSend(ctx, my_IOSend);
    return (void *)ctx;

}


void wolfSSL_TLS_client(void *v_ctx, func_args *args)
{
    ID cepid = 1;
    ER ercd;
    int ret;
    WOLFSSL_CTX *ctx = (WOLFSSL_CTX *)v_ctx;
    WOLFSSL *ssl;
    #define BUFF_SIZE 256
    static const char sendBuff[]= "Hello Server\n" ;
    char    rcvBuff[BUFF_SIZE] = {0};
    static T_IPV4EP my_addr = { 0, 0 };
    T_IPV4EP dst_addr;

    if(args->argc >= 2){
	    if((dst_addr.ipaddr = getIPaddr(args->argv[1])) == 0){
		printf("ERROR: IP address\n");
	        return;
	    }
	    if((dst_addr.portno = getPort(args->argv[2])) == 0){
		printf("ERROR: IP address\n");
	        return;
	    }
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
