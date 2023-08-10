/* memory-tls.c
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

/* IoT-safe client side demo
    - server uses software crypto and buffers
    - client uses IoT-Safe

    Client and server communicates in a cooperative
    scheduling mechanism within the same thread.
    Two buffers in memory are used for client<=>server communication.
*/

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/ssl.h>
#include <wolfssl/error-ssl.h>
#include <wolfssl/wolfcrypt/port/iotsafe/iotsafe.h>

#include <stdio.h>
#include <string.h>
#include "ca-cert.c"


/* The following define
 * activates mutual authentication */
#define CLIENT_AUTH

#define CLIENT_IOTSAFE
#define CA_ECC


static int client_state = 0;
static int server_state = 0;

static uint8_t cert_buffer[2048];
static uint32_t cert_buffer_size;

static WOLFSSL_CTX* srv_ctx = NULL;
static WOLFSSL* srv_ssl = NULL;
static WOLFSSL_CTX *cli_ctx = NULL;
static WOLFSSL *cli_ssl = NULL;


/* client messages to server in memory */
#define TLS_BUFFERS_SZ (1024 * 8)
static unsigned char to_server[TLS_BUFFERS_SZ];
static int server_bytes;
static int server_write_idx;
static int server_read_idx;

/* server messages to client in memory */
static unsigned char to_client[TLS_BUFFERS_SZ];
static int client_bytes;
static int client_write_idx;
static int client_read_idx;


/* server send callback */
int ServerSend(WOLFSSL* ssl, char* buf, int sz, void* ctx)
{
    if (client_write_idx + sz > TLS_BUFFERS_SZ) {
        return WOLFSSL_CBIO_ERR_WANT_WRITE;
    }
    printf("=== Srv-Cli: %d\n", sz);
    XMEMCPY(&to_client[client_write_idx], buf, sz);
    client_write_idx += sz;
    client_bytes += sz;
    return sz;
}


/* server recv callback */
int ServerRecv(WOLFSSL* ssl, char* buf, int sz, void* ctx)
{
    if (server_bytes - server_read_idx < sz) {
        return WOLFSSL_CBIO_ERR_WANT_READ;
    }
    XMEMCPY(buf, &to_server[server_read_idx], sz);
    server_read_idx += sz;

    if (server_read_idx == server_write_idx) {
        server_read_idx = server_write_idx = 0;
        server_bytes = 0;
    }
    printf("=== Srv RX: %d\n", sz);
    return sz;
}


/* client send callback */
int ClientSend(WOLFSSL* ssl, char* buf, int sz, void* ctx)
{
    if (server_write_idx + sz > TLS_BUFFERS_SZ)
        return WOLFSSL_CBIO_ERR_WANT_WRITE;

    printf("=== Cli->Srv: %d\n", sz);
    XMEMCPY(&to_server[server_write_idx], buf, sz);
    server_write_idx += sz;
    server_bytes += sz;

    return sz;
}


/* client recv callback */
int ClientRecv(WOLFSSL* ssl, char* buf, int sz, void* ctx)
{
    if (client_bytes - client_read_idx < sz) {
        return WOLFSSL_CBIO_ERR_WANT_READ;
    }

    XMEMCPY(buf, &to_client[client_read_idx], sz);
    client_read_idx += sz;

    if (client_read_idx == client_write_idx) {
        client_read_idx = client_write_idx = 0;
        client_bytes = 0;
    }
    printf("=== Cli RX: %d\n", sz);
    return sz;
}

/* wolfSSL Client loop */
static int client_loop(void)
{
    /* set up client */
    int ret;
    const char* helloStr = "hello iot-safe wolfSSL";
    #if (IOTSAFE_ID_SIZE == 1)
    byte cert_file_id, privkey_id, keypair_id, peer_pubkey_id, peer_cert_id, serv_cert_id;
    byte ca_cert_id;
    #else
    word16 cert_file_id, privkey_id, keypair_id, peer_pubkey_id, peer_cert_id, serv_cert_id;
    word16 ca_cert_id;
    #endif
    cert_file_id = CRT_CLIENT_FILE_ID;
    privkey_id = PRIVKEY_ID;
    keypair_id = ECDH_KEYPAIR_ID;
    peer_pubkey_id = PEER_PUBKEY_ID;
    peer_cert_id = PEER_CERT_ID;
    ca_cert_id = CRT_SERVER_FILE_ID;
    serv_cert_id = CRT_SERVER_FILE_ID;


    printf("=== CLIENT step %d ===\n", client_state);
    if (client_state == 0) {
        printf("Client: Creating new CTX\n");
    #ifdef WOLFSSL_TLS13
        cli_ctx = wolfSSL_CTX_new(wolfTLSv1_3_client_method());
    #else
        cli_ctx = wolfSSL_CTX_new(wolfTLSv1_2_client_method());
    #endif
        if (cli_ctx == NULL) {
            printf("Bad client ctx new");
            return 0;
        }
        printf("Client: Enabling IoT Safe in CTX\n");
        wolfSSL_CTX_iotsafe_enable(cli_ctx);

        printf("Loading CA\n");
#ifdef SOFT_SERVER_CA
        ret = wolfSSL_CTX_load_verify_buffer(cli_ctx, ca_ecc_cert_der_256,
                sizeof_ca_ecc_cert_der_256, WOLFSSL_FILETYPE_ASN1);
        if (ret != WOLFSSL_SUCCESS) {
            printf("Bad CA\n");
            return -1;
        }
#else
        cert_buffer_size = wolfIoTSafe_GetCert_ex(&ca_cert_id, IOTSAFE_ID_SIZE,
                cert_buffer,
                sizeof(cert_buffer));
        if (cert_buffer_size < 1) {
            printf("Bad server cert\n");
            return -1;
        }
        printf("Loaded Server CA from IoT-Safe, size = %lu\n",
                cert_buffer_size);
        ret = wolfSSL_CTX_load_verify_buffer(cli_ctx, cert_buffer,
                cert_buffer_size, WOLFSSL_FILETYPE_ASN1);
#endif


        printf("Loading Server Certificate\n");
#ifdef SOFT_SERVER_CERT
        /*
        ret = wolfSSL_CTX_load_verify_buffer(cli_ctx, serv_ecc_der_256,
                sizeof_serv_ecc_der_256, WOLFSSL_FILETYPE_ASN1);
        if (ret != WOLFSSL_SUCCESS) {
            printf("Bad Server certificate!\n");
            return -1;
        }
        */
#else
        cert_buffer_size = wolfIoTSafe_GetCert_ex(&serv_cert_id,IOTSAFE_ID_SIZE,
                cert_buffer,
            sizeof(cert_buffer));
        if (cert_buffer_size < 1) {
            printf("Bad server certificate!\n");
            return -1;
        }
        printf("Loaded Server certificate from IoT-Safe, size = %lu\n",
                cert_buffer_size);
        if (wolfSSL_CTX_load_verify_buffer(cli_ctx, cert_buffer, cert_buffer_size,
                    WOLFSSL_FILETYPE_ASN1) != WOLFSSL_SUCCESS) {
            printf("Cannot load server cert\n");
            return -1;
        }
#endif
        printf("Server certificate successfully imported.\n");
        wolfSSL_CTX_set_verify(cli_ctx, WOLFSSL_VERIFY_PEER, NULL);

#ifdef CLIENT_AUTH
        cert_buffer_size = wolfIoTSafe_GetCert_ex(&cert_file_id, IOTSAFE_ID_SIZE,
                cert_buffer,
            sizeof(cert_buffer));
        if (cert_buffer_size < 1) {
            printf("Bad client cert\n");
            return -1;
        }
        printf("Loaded Client certificate from IoT-Safe, size = %lu\n",
            cert_buffer_size);
        if (wolfSSL_CTX_use_certificate_buffer(cli_ctx, cert_buffer,
                  cert_buffer_size, WOLFSSL_FILETYPE_ASN1) != WOLFSSL_SUCCESS) {
            printf("Cannot load client cert\n");
            return -1;
        }
        printf("Client certificate successfully imported.\n");
#endif

        /* Setting IO Send/Receive functions to local memory-based message
         * passing (ClientSend, ClientRecv) */
        wolfSSL_CTX_SetIOSend(cli_ctx, ClientSend);
        wolfSSL_CTX_SetIORecv(cli_ctx, ClientRecv);

        printf("Creating new SSL object\n");
        cli_ssl = wolfSSL_new(cli_ctx);
        if (cli_ssl == NULL)  {
            printf("bad client new");
            return 0;
        }

        printf("Setting TLS options: turn on IoT-safe for this socket\n");

        wolfSSL_iotsafe_on_ex(cli_ssl, &privkey_id, &keypair_id,
            &peer_pubkey_id, &peer_cert_id, IOTSAFE_ID_SIZE);

    #ifdef WOLFSSL_TLS13
        printf("Setting TLSv1.3 for SECP256R1 key share\n");
        wolfSSL_UseKeyShare(cli_ssl, WOLFSSL_ECC_SECP256R1);
    #endif

        client_state++;
    }

    if (client_state == 1) {
        printf("Connecting to server...\n");
        ret = wolfSSL_connect(cli_ssl);
        if (ret != WOLFSSL_SUCCESS) {
            if (wolfSSL_want_read(cli_ssl) || wolfSSL_want_write(cli_ssl)) {
                return 0;
            }
            printf("Error in client tls connect: %d\n",
                wolfSSL_get_error(cli_ssl, ret));
            client_state = 0;
            return -1;
        }
        printf("Client connected!\n");
        client_state++;
    }

    if (client_state == 2) {
        printf("Sending message: %s\n", helloStr);
        ret = wolfSSL_write(cli_ssl, helloStr, XSTRLEN(helloStr));
        if (ret >= 0) {
            printf("wolfSSL client test success!\n");

            wolfSSL_free(cli_ssl); cli_ssl = NULL;
            wolfSSL_CTX_free(cli_ctx); cli_ctx = NULL;
            client_state = 0;
        }
        else if (wolfSSL_get_error(cli_ssl, ret) != WOLFSSL_ERROR_WANT_WRITE) {
            printf("Error in client tls write");
            client_state = 0;
            return -1;
        }
    }

    return ret;
}

/* wolfSSL Server Loop */
static int server_loop(void)
{
    int ret;
    unsigned char buf[80];

    printf("=== SERVER step %d ===\n", server_state);

    if (server_state == 0) {
    #ifdef WOLFSSL_TLS13
        srv_ctx = wolfSSL_CTX_new(wolfTLSv1_3_server_method());
    #else
        srv_ctx = wolfSSL_CTX_new(wolfTLSv1_2_server_method());
    #endif
        if (srv_ctx == NULL) {
            printf("bad server ctx new");
            return -1;
        }
#ifdef CLIENT_AUTH
        ret = wolfSSL_CTX_load_verify_buffer(srv_ctx, ca_ecc_cert_der_256,
            sizeof_ca_ecc_cert_der_256, WOLFSSL_FILETYPE_ASN1);
        if (ret != WOLFSSL_SUCCESS) {
            printf("Bad CA load: %d\n", ret);
        }
        ret = wolfSSL_CTX_load_verify_buffer(srv_ctx, cliecc_cert_der_256,
            sizeof_cliecc_cert_der_256, WOLFSSL_FILETYPE_ASN1);
        if (ret != WOLFSSL_SUCCESS) {
            printf("Bad Client cert load: %d\n", ret);
        }
        wolfSSL_CTX_set_verify(srv_ctx, WOLFSSL_VERIFY_PEER, NULL);
#endif

        if (wolfSSL_CTX_use_PrivateKey_buffer(srv_ctx, ecc_key_der_256,
            sizeof_ecc_key_der_256, WOLFSSL_FILETYPE_ASN1) != WOLFSSL_SUCCESS) {
            printf("Cannot load server private key\n");
        }
        if (wolfSSL_CTX_use_certificate_buffer(srv_ctx, serv_ecc_der_256,
           sizeof_serv_ecc_der_256, WOLFSSL_FILETYPE_ASN1) != WOLFSSL_SUCCESS) {
            printf("Cannot load server cert\n");
        }
        wolfSSL_CTX_SetIOSend(srv_ctx, ServerSend);
        wolfSSL_CTX_SetIORecv(srv_ctx, ServerRecv);

        srv_ssl = wolfSSL_new(srv_ctx);
        if (srv_ssl == NULL) {
            printf("bad server new");
            return -1;
        }

    #ifdef WOLFSSL_TLS13
        printf("Setting TLSv1.3 for SECP256R1 key share\n");
        wolfSSL_UseKeyShare(srv_ssl, WOLFSSL_ECC_SECP256R1);
    #endif
        server_state++;
    }

    if (server_state == 1) {
        /* accept tls connection without tcp sockets */
        ret = wolfSSL_accept(srv_ssl);
        if (ret != WOLFSSL_SUCCESS) {
            if (wolfSSL_want_read(srv_ssl) || wolfSSL_want_write(srv_ssl)) {
                return 0;
            }
            printf("Error in server tls accept: %d\n",
                wolfSSL_get_error(srv_ssl, ret));
            server_state = 0;
            return -1;
        }
        printf("wolfSSL accept success!\n");
        server_state++;
    }

    if (server_state == 2) {
        ret = wolfSSL_read(srv_ssl, buf, sizeof(buf)-1);
        if (wolfSSL_get_error(srv_ssl, ret) == WOLFSSL_ERROR_WANT_READ) {
            return 0;
        }
        if (ret < 0) {
            printf("SERVER READ ERROR: %d\n", wolfSSL_get_error(srv_ssl, ret));
            return -1;
        }
        if (ret > 0) {
            printf("++++++ Server received msg from client: '%s'\n", buf);
            printf("IoT-Safe TEST SUCCESSFUL\n");

            wolfSSL_free(srv_ssl); srv_ssl = NULL;
            wolfSSL_CTX_free(srv_ctx); srv_ctx = NULL;

            server_state = 0;
            return -1; /* exit test loop, so it doesn't keep running forever */
        }
    }

    return 0;
}


int memory_tls_test(void)
{
    int ret_s, ret_c;

    printf("Starting memory-tls test...\n");
    do {
        ret_s = server_loop();
        if (ret_s >= 0)
            ret_c = client_loop();
    } while ((ret_s >= 0) && (ret_c >= 0));

    /* clean up */
    wolfSSL_free(cli_ssl);
    wolfSSL_CTX_free(cli_ctx);
    wolfSSL_free(srv_ssl);
    wolfSSL_CTX_free(srv_ctx);


    return 0;
}
