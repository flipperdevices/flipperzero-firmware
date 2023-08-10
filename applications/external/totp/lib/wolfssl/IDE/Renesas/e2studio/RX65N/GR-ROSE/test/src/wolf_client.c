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
#include "wolfssl_demo.h"
#if defined(WOLFSSL_RENESAS_TSIP_TLS)
    #include <wolfssl/wolfcrypt/port/Renesas/renesas-tsip-crypt.h>
#endif

#define SIMPLE_TLSSEVER_IP       "192.168.1.12"
#define SIMPLE_TLSSERVER_PORT    "11111"

ER    t4_tcp_callback(ID cepid, FN fncd , VP p_parblk);

static WOLFSSL_CTX *client_ctx;

#if defined(WOLFSSL_RENESAS_TSIP_TLS)
uint32_t g_encrypted_root_public_key[140];
static   TsipUserCtx userContext;
#endif

#if defined(TLS_CLIENT)
#if defined(WOLFSSL_RENESAS_TSIP_TLS) && defined(WOLFSSL_STATIC_MEMORY)

    extern WOLFSSL_HEAP_HINT*  heapHint;

    #define BUFFSIZE_IO  (16 * 1024)
    unsigned char heapBufIO[BUFFSIZE_IO];

#endif /* WOLFSSL_RENESAS_TSIP_TLS && WOLFSSL_STATIC_MEMORY */
#endif /* TLS_CLIENT */

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
        #if defined(USE_ECC_CERT) && defined(USE_CERT_BUFFERS_256) 
            const unsigned char *cert       = ca_ecc_cert_der_256;
            #define  SIZEOF_CERT sizeof_ca_ecc_cert_der_256
        #else
            const unsigned char *cert       = ca_cert_der_2048;
            #define  SIZEOF_CERT sizeof_ca_cert_der_2048
        #endif
    #endif

    wolfSSL_Init();
    #ifdef DEBUG_WOLFSSL
        wolfSSL_Debugging_ON();
    #endif

    /*---------------------------------------------*/
    /*          Allocate WOLFSSL_CTX               */
    /*---------------------------------------------*/

#if defined(WOLFSSL_STATIC_MEMORY)

    if ((client_ctx = wolfSSL_CTX_new_ex(wolfSSLv23_client_method_ex(heapHint),
                                                      heapHint)) == NULL) {
        printf("ERROR: failed to create WOLFSSL_CTX\n");
        return;                                                
    }

    if ((wolfSSL_CTX_load_static_memory(&client_ctx, NULL, heapBufIO,
                sizeof(heapBufIO), WOLFMEM_IO_POOL, 10)) != WOLFSSL_SUCCESS) {
        printf("ERROR: failed to set static memory for IO\n");
        return;
    }

#else

    /* Create and initialize WOLFSSL_CTX */
    if ((client_ctx = 
        wolfSSL_CTX_new(wolfSSLv23_client_method_ex((void *)NULL))) == NULL) {
        printf("ERROR: failed to create WOLFSSL_CTX\n");
        return;
    }
#endif /* WOLFSSL_STATIC_MEMORY */

    /*---------------------------------------------*/
    /*          Set up TSIP callbacks              */
    /*---------------------------------------------*/

    #ifdef WOLFSSL_RENESAS_TSIP_TLS
    tsip_set_callbacks(client_ctx);
    #endif

    /*---------------------------------------------*/
    /*          Root CA certificate                */
    /*---------------------------------------------*/

    if (wolfSSL_CTX_load_verify_buffer(client_ctx, cert, SIZEOF_CERT, 
                                            SSL_FILETYPE_ASN1) != SSL_SUCCESS){
           printf("ERROR: can't load certificate data\n");
       return;
    }

    /*---------------------------------------------*/
    /*          Set up IO callbacks                */
    /*---------------------------------------------*/

    wolfSSL_SetIORecv(client_ctx, my_IORecv);
    wolfSSL_SetIOSend(client_ctx, my_IOSend);

    /*---------------------------------------------*/
    /*          Set up cipher suites               */
    /*---------------------------------------------*/

    /* use specific cipher */
    if (cipherlist != NULL && 
        wolfSSL_CTX_set_cipher_list(client_ctx, cipherlist) != WOLFSSL_SUCCESS) {
        wolfSSL_CTX_free(client_ctx); client_ctx = NULL;
        printf("client can't set cipher list");
        return;
    }
    
#if defined(WOLFSSL_TLS13)
    if (wolfSSL_CTX_UseSupportedCurve(client_ctx, WOLFSSL_ECC_SECP256R1) 
                                                        != WOLFSSL_SUCCESS) {
        wolfSSL_CTX_free(client_ctx); client_ctx = NULL;
        printf("client can't set use supported curves\n");
        return;
    }
#endif
}

void wolfSSL_TLS_client( )
{
    ID  cepid = 1;
    ER  ercd;
    int ret;
    WOLFSSL_CTX *ctx = (WOLFSSL_CTX *)client_ctx;
    WOLFSSL *ssl = NULL;

    #define BUFF_SIZE 256
    static const char sendBuff[]= "Hello Server\n" ;
    
    char    rcvBuff[BUFF_SIZE] = {0};
    
    static T_IPV4EP my_addr = { 0, 0 };
    
    T_IPV4EP dst_addr;

    if((dst_addr.ipaddr = getIPaddr(SIMPLE_TLSSEVER_IP)) == 0){
        printf("ERROR: IP address\n");
        goto out;
    }
    if((dst_addr.portno = getPort(SIMPLE_TLSSERVER_PORT)) == 0){
        printf("ERROR: IP address\n");
        goto out;
    }

    if((ercd = tcp_con_cep(cepid, &my_addr, &dst_addr, TMO_FEVR)) != E_OK) {
        printf("ERROR TCP Connect: %d\n", ercd);
        goto out;
    }

    /*---------------------------------------------*/
    /*          Allocate WOLFSSL object            */
    /*---------------------------------------------*/

    if((ssl = wolfSSL_new(ctx)) == NULL) {
        printf("ERROR wolfSSL_new: %d\n", wolfSSL_get_error(ssl, 0));
        goto out;
    }

    /*---------------------------------------------*/
    /*          Set up callback context for TSIP   */
    /*---------------------------------------------*/

    #ifdef WOLFSSL_RENESAS_TSIP_TLS
    tsip_set_callback_ctx(ssl, &userContext);
    #endif

    /*---------------------------------------------*/
    /*          Client Certificate                 */
    /*---------------------------------------------*/
#ifdef USE_ECC_CERT

    /* ECDSA client certificate */   
    if (wolfSSL_use_certificate_buffer(ssl, cliecc_cert_der_256,
            sizeof_cliecc_cert_der_256, WOLFSSL_FILETYPE_ASN1) != SSL_SUCCESS) {
        printf("ERROR wolfSSL_use_certificate_buffer: %d\n",
                                                wolfSSL_get_error(ssl, 0));
        goto out;    
    }

#else

    /* RSA client certificate */
    if (wolfSSL_use_certificate_buffer(ssl, client_cert_der_2048,
        sizeof_client_cert_der_2048, WOLFSSL_FILETYPE_ASN1) != SSL_SUCCESS) {
        printf("ERROR wolfSSL_use_certificate_buffer: %d\n",
                                                wolfSSL_get_error(ssl, 0));
        goto out;    
    }

#endif /* USE_ECC_CERT */

    /*---------------------------------------------*/
    /*          Client Keys                        */
    /*---------------------------------------------*/

#ifdef USE_ECC_CERT

    #ifdef WOLFSSL_RENESAS_TSIP_TLS

    /* TSIP specific ECC private key */
    if (tsip_use_PrivateKey_buffer_TLS(ssl,
                (const char*)g_key_block_data.encrypted_user_ecc256_private_key,
                sizeof(g_key_block_data.encrypted_user_ecc256_private_key),
                TSIP_ECCP256) != 0) {
        printf("ERROR tsip_use_PrivateKey_buffer_TLS\n");
        goto out;
    }
    
    #else

    /* DER format ECC private key */
    if (wolfSSL_use_PrivateKey_buffer(ssl,
                                    ecc_clikey_der_256,
                                    sizeof_ecc_clikey_der_256, 
                                    WOLFSSL_FILETYPE_ASN1) != SSL_SUCCESS) {
        printf("ERROR wolfSSL_use_PrivateKey_buffer: %d\n",
                                                wolfSSL_get_error(ssl, 0));
        goto out; 
    }

    #endif

#else

    #if defined(WOLFSSL_RENESAS_TSIP_TLS)

    /* Note: TSIP asks client key pair for client authentication. */

    /* TSIP specific RSA private key */
    if (tsip_use_PrivateKey_buffer_TLS(ssl,
               (const char*)g_key_block_data.encrypted_user_rsa2048_private_key,
               sizeof(g_key_block_data.encrypted_user_rsa2048_private_key),
                                                        TSIP_RSA2048) != 0) {
        printf("ERROR tsip_use_PrivateKey_buffer_TLS\n");
        goto out;
    }

    /* TSIP specific RSA public key */
    if (tsip_use_PublicKey_buffer_TLS(ssl,
                (const char*)g_key_block_data.encrypted_user_rsa2048_public_key,
                sizeof(g_key_block_data.encrypted_user_rsa2048_public_key),
                                                        TSIP_RSA2048) != 0) {
        printf("ERROR tsip_use_PublicKey_buffer_TLS\n");
        goto out;
    }

    #else

    if (wolfSSL_use_PrivateKey_buffer(ssl, client_key_der_2048,
        sizeof_client_key_der_2048, WOLFSSL_FILETYPE_ASN1) != SSL_SUCCESS) {
        printf("ERROR wolfSSL_use_PrivateKey_buffer: %d\n",
                                                wolfSSL_get_error(ssl, 0));
        goto out; 
    }

    #endif /* WOLFSSL_RENESAS_TSIP_TLS */

#endif /* USE_ECC_CERT */


    /*---------------------------------------------*/
    /*          Set up IO callback context         */
    /*---------------------------------------------*/

    wolfSSL_SetIOReadCtx(ssl, (void *)&cepid);
    wolfSSL_SetIOWriteCtx(ssl, (void *)&cepid);

    /*---------------------------------------------*/
    /*          TLS handshake                      */
    /*---------------------------------------------*/

    if(wolfSSL_connect(ssl) != SSL_SUCCESS) {
        printf("ERROR SSL connect: %d\n",  wolfSSL_get_error(ssl, 0));
        goto out;
    }

    if (wolfSSL_write(ssl, sendBuff, strlen(sendBuff)) != strlen(sendBuff)) {
        printf("ERROR SSL write: %d\n", wolfSSL_get_error(ssl, 0));
        goto out;
    }

    if ((ret=wolfSSL_read(ssl, rcvBuff, BUFF_SIZE)) < 0) {
        printf("ERROR SSL read: %d\n", wolfSSL_get_error(ssl, 0));
        goto out;
    }

    rcvBuff[ret] = '\0' ;
    printf("Received: %s\n\n", rcvBuff);

 out:

    /* frees all data before client termination */
    if(ssl) {
        wolfSSL_shutdown(ssl);
        wolfSSL_free(ssl);
    }
    if(ctx) {
        wolfSSL_CTX_free(ctx);
    }

    wolfSSL_Cleanup();

    tcp_sht_cep(cepid);
    tcp_cls_cep(cepid, TMO_FEVR);

    return;
}
