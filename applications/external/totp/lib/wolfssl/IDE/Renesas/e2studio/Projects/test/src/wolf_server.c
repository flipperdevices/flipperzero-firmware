/* wolf_server.c
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
#include "wolfssl_demo.h"

static WOLFSSL_CTX *server_ctx;
static byte doCliCertCheck;

static int my_IORecv(WOLFSSL* ssl, char* buff, int sz, void* ctx)
{
    int ret;
    ID  cepid;

    if(ctx != NULL)
        cepid = *(ID *)ctx;
    else
        return WOLFSSL_CBIO_ERR_GENERAL;

    ret = tcp_rcv_dat(cepid, buff, sz, TMO_FEVR);
    if(ret == sz)
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


void wolfSSL_TLS_server_init(byte doClientCheck)
{

    int ret;


    #ifndef NO_FILESYSTEM
        #ifdef USE_ECC_CERT
        char *cert       = "./certs/server-ecc-cert.pem";
        char *key        = "./certs/server-ecc-key.pem";
        #else
        char *cert       = "./certs/server-cert.pem";
        char *key        = "./certs/server-key.pem";
        #endif
        char *clientCert = "./certs/client-cert.pem";
    #else
        #ifdef USE_ECC_CERT
        char *cert       = serv_ecc_der_256;
        int  sizeof_cert = sizeof_serv_ecc_der_256;
        char *cert       = serv_ecc_key_der_256;
        int  sizeof_key  = sizeof_serv_ecc_key_der_256;
        #else
        const unsigned char *cert       = server_cert_der_2048;
        #define sizeof_cert sizeof_server_cert_der_2048
        const unsigned char *key        = server_key_der_2048;
        #define  sizeof_key sizeof_server_key_der_2048
        const unsigned char *clientCert = client_cert_der_2048;
        #define  sizeof_clicert sizeof_client_cert_der_2048
        #endif
    #endif


    wolfSSL_Init();
    #ifdef DEBUG_WOLFSSL
        wolfSSL_Debugging_ON();
    #endif

    /* Create and initialize WOLFSSL_CTX */
    if ((server_ctx = wolfSSL_CTX_new(wolfSSLv23_server_method_ex((void *)NULL)))
                                                                    == NULL) {
        printf("ERROR: failed to create WOLFSSL_CTX\n");
        return;
    }

    #if !defined(NO_FILESYSTEM)
        ret = wolfSSL_CTX_use_certificate_file(server_ctx, cert, 0);
    #else
        ret = wolfSSL_CTX_use_certificate_buffer(server_ctx, cert,
                                            sizeof_cert, SSL_FILETYPE_ASN1);
    #endif
        if (ret != SSL_SUCCESS) {
            printf("Error %d loading server-cert!\n", ret);
            return;
        }

        /* Load server key into WOLFSSL_CTX */
    #if !defined(NO_FILESYSTEM)
        ret = wolfSSL_CTX_use_PrivateKey_file(server_ctx, key, 0);
    #else
        ret = wolfSSL_CTX_use_PrivateKey_buffer(server_ctx, key, sizeof_key, 
                                                        SSL_FILETYPE_ASN1);
    #endif
        if (ret != SSL_SUCCESS) {
            printf("Error %d loading server-key!\n", ret);
            return;
        }
#if defined(WOLFSSL_RENESAS_TSIP)
        doCliCertCheck = 1;
#endif
        if (doCliCertCheck) {
            wolfSSL_CTX_set_verify(server_ctx, WOLFSSL_VERIFY_PEER |
                                WOLFSSL_VERIFY_FAIL_IF_NO_PEER_CERT, 0);
#if !defined(NO_FILESYSTEM)
            if (wolfSSL_CTX_load_verify_locations(server_ctx, clientCert, 0) 
                                                                != WOLFSSL_SUCCESS)
#else
            if (wolfSSL_CTX_load_verify_buffer(server_ctx, clientCert, 
                                               sizeof_clicert,
                                               SSL_FILETYPE_ASN1) != SSL_SUCCESS)
#endif
                printf("can't load ca file, Please run from wolfSSL home dir\n");
        }
   
   /* Register callbacks */
   wolfSSL_SetIORecv(server_ctx, my_IORecv);
   wolfSSL_SetIOSend(server_ctx, my_IOSend);
   
}

void wolfSSL_TLS_server( )
{
    ID cepid = 1;
    ID repid = 1;
    ER ercd;
    WOLFSSL_CTX *ctx = (WOLFSSL_CTX *)server_ctx;

    WOLFSSL *ssl;
    int len;
    #define BUFF_SIZE 256
    char buff[BUFF_SIZE];
    T_IPV4EP dst_addr = {0, 0};

    if((ercd = tcp_acp_cep(cepid, repid, &dst_addr, TMO_FEVR)) != E_OK) {
        printf("ERROR TCP Accept: %d\n", ercd);
        return;
    }

    if((ssl = wolfSSL_new(ctx)) == NULL) {
        printf("ERROR: failed wolfSSL_new\n");
        return;
    }

    wolfSSL_SetIOReadCtx(ssl, (void *)&cepid);
    wolfSSL_SetIOWriteCtx(ssl, (void *)&cepid);

    if (wolfSSL_accept(ssl) < 0) {
        printf("ERROR: SSL Accept(%d)\n", wolfSSL_get_error(ssl, 0));
        return;
    }

    if ((len = wolfSSL_read(ssl, buff, sizeof(buff) - 1)) < 0) {
        printf("ERROR: SSL Read(%d)\n", wolfSSL_get_error(ssl, 0));
        return;
    }

    buff[len] = '\0';
    printf("Received: %s\n", buff);

    if (wolfSSL_write(ssl, buff, len) != len) {
        printf("ERROR: SSL Write(%d)\n", wolfSSL_get_error(ssl, 0));
        return;
    }

    wolfSSL_free(ssl);
    tcp_sht_cep(cepid);
}
