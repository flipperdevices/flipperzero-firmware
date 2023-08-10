/* tls_client.c
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
#include <wolfssl/wolfcrypt/error-crypt.h>

#if !defined(WOLFCRYPT_ONLY) && !defined(NO_WOLFSSL_CLIENT)

#include <wolfssl/ssl.h>
#include <wolfssl/wolfcrypt/logging.h>
#include <stdio.h>

#define MAXSZ              1024

/*------------------------------------------------------------------------*/
/* TLS CLIENT */
/*------------------------------------------------------------------------*/
static int CbIORecv(WOLFSSL *ssl, char *buf, int sz, void *ctx)
{
    int ret = WOLFSSL_CBIO_ERR_GENERAL;

    (void)ssl;
    (void)ctx;

    /* TODO: Exchange data over your own transport */
    #warning TODO: Implement your own recv data transport
#if 0
    ret = usart_read_buffer_wait(&cdc_uart_module, buf, sz);
    if (ret == STATUS_ERR_TIMEOUT)
        return WOLFSSL_CBIO_ERR_WANT_READ;

    return (ret == STATUS_OK) ? sz : WOLFSSL_CBIO_ERR_GENERAL;
#else
    return ret;
#endif
}

static int CbIOSend(WOLFSSL *ssl, char *buf, int sz, void *ctx)
{
    int ret = WOLFSSL_CBIO_ERR_GENERAL;

    (void)ssl;
    (void)ctx;

    /* TODO: Exchange data over your own transport */
    #warning TODO: Implement your own send data transport
#if 0
    ret = usart_write_buffer_wait(&cdc_uart_module, buf, sz);
    if (ret == STATUS_ERR_TIMEOUT)
        return WOLFSSL_CBIO_ERR_WANT_WRITE;

    return (ret == STATUS_OK) ? sz : WOLFSSL_CBIO_ERR_GENERAL;
#else
    return ret;
#endif
}

static int tls_client(void)
{
    char msg[] = "Hello WolfSSL!\r\n";
    char reply[MAXSZ];
    int ret, msgSz, error;
    WOLFSSL* ssl = NULL;
    WOLFSSL_CTX* ctx = NULL;

    if ((ctx = wolfSSL_CTX_new(wolfTLSv1_2_client_method())) == NULL) {
        printf("CTXnew failed.\n");
        goto fail;
    }

    /*------------------------------------------------------------------------*/
    /* ECDHE-ECDSA */
    /*------------------------------------------------------------------------*/
    /*--------------------*/
    /* for peer auth use: */
    /*--------------------*/
    //    wolfSSL_CTX_load_verify_buffer(ctx, rsa_key_der_1024,
    //                                    sizeof_rsa_key_der_1024, SSL_FILETYPE_ASN1);
    //    wolfSSL_CTX_load_verify_buffer(ctx, server_cert_der_1024,
    //                                    sizeof_server_cert_der_1024, SSL_FILETYPE_ASN1);
    /*---------------------*/
    /* for no peer auth:   */
    /*---------------------*/
    wolfSSL_CTX_set_verify(ctx, WOLFSSL_VERIFY_NONE, NULL);
    /*---------------------*/
    /* end peer auth option*/
    /*---------------------*/
    if ((ret = wolfSSL_CTX_set_cipher_list(ctx, "ECDHE-ECDSA-AES128-SHA256")) != WOLFSSL_SUCCESS) {
        wolfSSL_CTX_free(ctx);
        printf("CTXset_cipher_list failed, error: %d\n", ret);
        goto fail;
    }
    /*------------------------------------------------------------------------*/
    /* END CIPHER SUITE OPTIONS */
    /*------------------------------------------------------------------------*/
    wolfSSL_CTX_SetIORecv(ctx, CbIORecv);
    wolfSSL_CTX_SetIOSend(ctx, CbIOSend);

    if ((ssl = wolfSSL_new(ctx)) == NULL) {
        error = wolfSSL_get_error(ssl, 0);
        printf("wolfSSL_new failed %d\n", error);
        wolfSSL_CTX_free(ctx);
        return -1;
    }

    /* non blocking accept and connect */
    ret = WOLFSSL_FAILURE;

    while (ret != WOLFSSL_SUCCESS) {
        /* client connect */
        ret = wolfSSL_connect(ssl);
        error = wolfSSL_get_error(ssl, 0);
        if (ret != WOLFSSL_SUCCESS) {
            if (error != WOLFSSL_ERROR_WANT_READ && error != WOLFSSL_ERROR_WANT_WRITE) {
                /* Fail */
                printf("wolfSSL connect failed with return code %d\n", error);
                goto fail;
            }
        }
        /* Success */
    }

    /* read and write */
    while (1) {
        /* client send/read */
        msgSz = sizeof(msg);
        ret   = wolfSSL_write(ssl, msg, msgSz);
        error = wolfSSL_get_error(ssl, 0);
        if (ret != msgSz) {
            if (error != WOLFSSL_ERROR_WANT_READ && error != WOLFSSL_ERROR_WANT_WRITE) {
                /* Write failed */
                goto fail;
            }
        }
        /* Write succeeded */
        break;
    }

    while (1) {
        ret = wolfSSL_read(ssl, reply, sizeof(reply) - 1);
        error = wolfSSL_get_error(ssl, 0);
        if (ret < 0) {
            if (error != WOLFSSL_ERROR_WANT_READ && error != WOLFSSL_ERROR_WANT_WRITE) {
                /* Can put print here, the server enters a loop waiting to read
                 * a confirmation message at this point */
                // printf("client read failed\n");
                goto fail;
            }
            continue;
        }
        else {
            /* Can put print here, the server enters a loop waiting to read
             * a confirmation message at this point */
            reply[ret] = '\0';
            // printf("Client Received Reply: %s\n", reply);
            break;
        }

    }

    return 0;

fail:
    wolfSSL_shutdown(ssl);
    wolfSSL_free(ssl);
    wolfSSL_CTX_free(ctx);

    return -1;
}
#endif /* !WOLFCRYPT_ONLY && !NO_WOLFSSL_CLIENT */


int main(void)
{
    int ret;

#if !defined(WOLFCRYPT_ONLY) && !defined(NO_WOLFSSL_CLIENT)
    wolfSSL_Init();

    ret = tls_client();

    wolfSSL_Cleanup();
#else
    ret = NOT_COMPILED_IN;
#endif

	return ret;
}
