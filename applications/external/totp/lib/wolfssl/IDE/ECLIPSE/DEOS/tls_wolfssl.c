/* tls_wolfssl.c
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
#include <wolfcrypt/test/test.h>
#include <wolfcrypt/benchmark/benchmark.h>
#include <wolfssl/wolfcrypt/logging.h> /* to use WOLFSSL_MSG */
#include <tls_wolfssl.h>


int setupTransport(clientConnectionHandleType* connectionHandle,
                   char* connectionId) {
    int ret, error;
    void * sendBuffer;
    size_t bufferSizeInBytes;

    if ((ret = socketTransportInitialize("mailbox-transport.config",
                                         "transportConfigurationId",
                                         waitIndefinitely,&error)) != transportSuccess)
        printf("Initialize 0x%x, error=%d\n", ret, error);

    else if ((ret = socketTransportClientInitialize(waitIndefinitely,
                                                    &error)) != transportSuccess)
        printf("ClientInitialize 0x%x, error=%d\n", ret, error);

    else if ((ret = socketTransportCreateConnection(connectionId,
                                                    waitIndefinitely,
                                                    COMPATIBILITY_ID_2,
                                                    connectionHandle,
                                                    &sendBuffer,
                                                    &bufferSizeInBytes,
                                                    &error)) != transportSuccess)
        printf("CreateConnection 0x%x, error=%d\n", ret, error);

    else if ((ret = socketTransportSetConnectionForThread(currentThreadHandle(),
                                                          *connectionHandle,
                                                          waitIndefinitely,
                                                          &error)) != transportSuccess)
        printf("SetConnectionForThread 0x%x, error=%d\n", ret, error);

    return ret;
}

#if !defined(NO_WOLFSSL_CLIENT )

/* 172.217.3.174 is the IP address of https://www.google.com */
#define TCP_SERVER_IP_ADDR "172.217.3.174"
#define TCP_SERVER_DOMAIN_NAME "www.google.com"
#define TCP_SERVER_PORT 443

#define TX_BUF_SIZE 64
#define RX_BUF_SIZE 1024

#define TX_MSG "GET /index.html HTTP/1.0\n\n"
#define TX_MSG_SIZE sizeof(TX_MSG)

static const unsigned char google_certs_ca[]="\n\
## Google Internet Authority G3 \n\
-----BEGIN CERTIFICATE-----\n\
MIIEXDCCA0SgAwIBAgINAeOpMBz8cgY4P5pTHTANBgkqhkiG9w0BAQsFADBMMSAw\n\
HgYDVQQLExdHbG9iYWxTaWduIFJvb3QgQ0EgLSBSMjETMBEGA1UEChMKR2xvYmFs\n\
U2lnbjETMBEGA1UEAxMKR2xvYmFsU2lnbjAeFw0xNzA2MTUwMDAwNDJaFw0yMTEy\n\
MTUwMDAwNDJaMFQxCzAJBgNVBAYTAlVTMR4wHAYDVQQKExVHb29nbGUgVHJ1c3Qg\n\
U2VydmljZXMxJTAjBgNVBAMTHEdvb2dsZSBJbnRlcm5ldCBBdXRob3JpdHkgRzMw\n\
ggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDKUkvqHv/OJGuo2nIYaNVW\n\
XQ5IWi01CXZaz6TIHLGp/lOJ+600/4hbn7vn6AAB3DVzdQOts7G5pH0rJnnOFUAK\n\
71G4nzKMfHCGUksW/mona+Y2emJQ2N+aicwJKetPKRSIgAuPOB6Aahh8Hb2XO3h9\n\
RUk2T0HNouB2VzxoMXlkyW7XUR5mw6JkLHnA52XDVoRTWkNty5oCINLvGmnRsJ1z\n\
ouAqYGVQMc/7sy+/EYhALrVJEA8KbtyX+r8snwU5C1hUrwaW6MWOARa8qBpNQcWT\n\
kaIeoYvy/sGIJEmjR0vFEwHdp1cSaWIr6/4g72n7OqXwfinu7ZYW97EfoOSQJeAz\n\
AgMBAAGjggEzMIIBLzAOBgNVHQ8BAf8EBAMCAYYwHQYDVR0lBBYwFAYIKwYBBQUH\n\
AwEGCCsGAQUFBwMCMBIGA1UdEwEB/wQIMAYBAf8CAQAwHQYDVR0OBBYEFHfCuFCa\n\
Z3Z2sS3ChtCDoH6mfrpLMB8GA1UdIwQYMBaAFJviB1dnHB7AagbeWbSaLd/cGYYu\n\
MDUGCCsGAQUFBwEBBCkwJzAlBggrBgEFBQcwAYYZaHR0cDovL29jc3AucGtpLmdv\n\
b2cvZ3NyMjAyBgNVHR8EKzApMCegJaAjhiFodHRwOi8vY3JsLnBraS5nb29nL2dz\n\
cjIvZ3NyMi5jcmwwPwYDVR0gBDgwNjA0BgZngQwBAgIwKjAoBggrBgEFBQcCARYc\n\
aHR0cHM6Ly9wa2kuZ29vZy9yZXBvc2l0b3J5LzANBgkqhkiG9w0BAQsFAAOCAQEA\n\
HLeJluRT7bvs26gyAZ8so81trUISd7O45skDUmAge1cnxhG1P2cNmSxbWsoiCt2e\n\
ux9LSD+PAj2LIYRFHW31/6xoic1k4tbWXkDCjir37xTTNqRAMPUyFRWSdvt+nlPq\n\
wnb8Oa2I/maSJukcxDjNSfpDh/Bd1lZNgdd/8cLdsE3+wypufJ9uXO1iQpnh9zbu\n\
FIwsIONGl1p3A8CgxkqI/UAih3JaGOqcpcdaCIzkBaR9uYQ1X4k2Vg5APRLouzVy\n\
7a8IVk6wuy6pm+T7HT4LY8ibS5FEZlfAFLSW8NwsVz9SBK2Vqn1N0PIMn5xA6NZV\n\
c7o835DLAFshEWfC7TIe3g==\n\
-----END CERTIFICATE-----\n\
## Google Trust Services- GlobalSign Root CA-R2\n\
-----BEGIN CERTIFICATE-----\n\
MIIDujCCAqKgAwIBAgILBAAAAAABD4Ym5g0wDQYJKoZIhvcNAQEFBQAwTDEgMB4G\n\
A1UECxMXR2xvYmFsU2lnbiBSb290IENBIC0gUjIxEzARBgNVBAoTCkdsb2JhbFNp\n\
Z24xEzARBgNVBAMTCkdsb2JhbFNpZ24wHhcNMDYxMjE1MDgwMDAwWhcNMjExMjE1\n\
MDgwMDAwWjBMMSAwHgYDVQQLExdHbG9iYWxTaWduIFJvb3QgQ0EgLSBSMjETMBEG\n\
A1UEChMKR2xvYmFsU2lnbjETMBEGA1UEAxMKR2xvYmFsU2lnbjCCASIwDQYJKoZI\n\
hvcNAQEBBQADggEPADCCAQoCggEBAKbPJA6+Lm8omUVCxKs+IVSbC9N/hHD6ErPL\n\
v4dfxn+G07IwXNb9rfF73OX4YJYJkhD10FPe+3t+c4isUoh7SqbKSaZeqKeMWhG8\n\
eoLrvozps6yWJQeXSpkqBy+0Hne/ig+1AnwblrjFuTosvNYSuetZfeLQBoZfXklq\n\
tTleiDTsvHgMCJiEbKjNS7SgfQx5TfC4LcshytVsW33hoCmEofnTlEnLJGKRILzd\n\
C9XZzPnqJworc5HGnRusyMvo4KD0L5CLTfuwNhv2GXqF4G3yYROIXJ/gkwpRl4pa\n\
zq+r1feqCapgvdzZX99yqWATXgAByUr6P6TqBwMhAo6CygPCm48CAwEAAaOBnDCB\n\
mTAOBgNVHQ8BAf8EBAMCAQYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUm+IH\n\
V2ccHsBqBt5ZtJot39wZhi4wNgYDVR0fBC8wLTAroCmgJ4YlaHR0cDovL2NybC5n\n\
bG9iYWxzaWduLm5ldC9yb290LXIyLmNybDAfBgNVHSMEGDAWgBSb4gdXZxwewGoG\n\
3lm0mi3f3BmGLjANBgkqhkiG9w0BAQUFAAOCAQEAmYFThxxol4aR7OBKuEQLq4Gs\n\
J0/WwbgcQ3izDJr86iw8bmEbTUsp9Z8FHSbBuOmDAGJFtqkIk7mpM0sYmsL4h4hO\n\
291xNBrBVNpGP+DTKqttVCL1OmLNIG+6KYnX3ZHu01yiPqFbQfXf5WRDLenVOavS\n\
ot+3i9DAgBkcRcAtjOj4LaR0VknFBbVPFd5uRHg5h6h+u/N5GJG79G+dwfCMNYxd\n\
AfvDbbnvRG15RjF+Cv6pgsH/76tuIMRQyV+dTZsXjAzlAcmgQWpzU/qlULRuJQ/7\n\
TBj0/VLZjmmx6BEP3ojY+x1J96relc8geMJgEtslQIxq/H5COEBkEveegeGTLg==\n\
-----END CERTIFICATE-----\n\
";

void wolfssl_client_test(uintData_t statusPtr) {
    int sock;
    char rx_buf[RX_BUF_SIZE];
    char tx_buf[TX_BUF_SIZE];
    int ret = 0, error = 0;

    sockaddr_in server_addr;
    clientConnectionHandleType TCPclientHandle;

    WOLFSSL* ssl;
    WOLFSSL_CTX* ctx;

    /* set up the mailbox transport */

    if (setupTransport(&TCPclientHandle, (char*)"connectionId1") != transportSuccess){
        printf("TCP transport set up failed \n");
        return;
      }

    printf("Creating a network socket...\n");

    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock == SOCKET_ERROR) {
        printf("ERROR: Failed to create socket, err = %d\n", errno);
        return;
    }

    printf("Clearing memory for server_addr struct\n");

    XMEMSET((char *) &server_addr, 0u, sizeof(server_addr));

    printf("Connecting to server IP address: %s, port: %d\n",
                    TCP_SERVER_IP_ADDR, TCP_SERVER_PORT);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(TCP_SERVER_IP_ADDR);
    server_addr.sin_port = htons(TCP_SERVER_PORT);

    printf("Calling connect on socket\n");
    if (connect(sock, (sockaddr *) &server_addr, sizeof(server_addr)) < 0 ) {
        printf("ERROR: connect, err = %d\n", errno);
        closesocket(sock);
        return;
    }

    #ifdef DEBUG_WOLFSSL
        wolfSSL_Debugging_ON();
    #endif

    /* wolfSSL INIT and CTX SETUP */

    wolfSSL_Init();

    /* chooses the highest possible TLS version */

    ctx = wolfSSL_CTX_new(wolfSSLv23_client_method());

    /* SET UP NETWORK SOCKET */
    if (ctx == 0) {
        printf("ERROR: wolfSSL_CTX_new failed\n");
        closesocket(sock);
        return;
    }

    WOLFSSL_MSG("wolfSSL_CTX_new done");

    wolfSSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);

    ret = wolfSSL_CTX_load_verify_buffer(ctx,
                                         google_certs_ca,
                                         sizeof(google_certs_ca),
                                         SSL_FILETYPE_PEM);

    if (ret != SSL_SUCCESS) {
        printf("ERROR: wolfSSL_CTX_load_verify_buffer() failed\n");
        closesocket(sock);
        wolfSSL_CTX_free(ctx);
        return;
    }
    ssl = wolfSSL_new(ctx);
    if (ssl == NULL) {
        printf("ERROR: wolfSSL_new() failed\n");
        closesocket(sock);
        wolfSSL_CTX_free(ctx);
        return;
    }

    WOLFSSL_MSG("wolfSSL_new done");
    ret = wolfSSL_set_fd(ssl, sock);
    if (ret != SSL_SUCCESS) {
        printf("ERROR: wolfSSL_set_fd() failed\n");
        closesocket(sock);
        wolfSSL_free(ssl);
        wolfSSL_CTX_free(ctx);
        return;
    }
    WOLFSSL_MSG("wolfSSL_set_fd done");
    do {
        error = 0; /* reset error */
        ret = wolfSSL_connect(ssl);
        if (ret != SSL_SUCCESS) {
            error = wolfSSL_get_error(ssl, 0);
            printf("ERROR: wolfSSL_connect() failed, err = %d\n", error);
            if (error != SSL_ERROR_WANT_READ) {
                closesocket(sock);
                wolfSSL_free(ssl);
                wolfSSL_CTX_free(ctx);
                return;
            }
            /* goToSleep() for 1 sec*/
        }
    } while ((ret != SSL_SUCCESS) && (error == SSL_ERROR_WANT_READ));

    printf("wolfSSL_connect() ok... sending GET\n");
    XSTRNCPY(tx_buf, TX_MSG, TX_MSG_SIZE);
    if (wolfSSL_write(ssl, tx_buf, TX_MSG_SIZE) != TX_MSG_SIZE) {
        error = wolfSSL_get_error(ssl, 0);
        printf("ERROR: wolfSSL_write() failed, err = %d\n", error);
        closesocket(sock);
        wolfSSL_free(ssl);
        wolfSSL_CTX_free(ctx);
        return;
    }
    do {
        error = 0; /* reset error */
        ret = wolfSSL_read(ssl, rx_buf, RX_BUF_SIZE - 1);
        if (ret < 0) {
            error = wolfSSL_get_error(ssl, 0);
            if (error != SSL_ERROR_WANT_READ) {
                printf("wolfSSL_read failed, error = %d\n", error);
                closesocket(sock);
                wolfSSL_free(ssl);
                wolfSSL_CTX_free(ctx);
                return;
            }
            /* goToSleep() for 1 second*/
        } else if (ret > 0) {
            rx_buf[ret] = 0;
            printf("%s\n", rx_buf);
        }
    } while (error == SSL_ERROR_WANT_READ);
    wolfSSL_shutdown(ssl);
    wolfSSL_free(ssl);
    wolfSSL_CTX_free(ctx);
    wolfSSL_Cleanup();
    closesocket(sock);
    return;
}

#endif /* NO_WOLFSSL_CLIENT */

#if !defined(NO_WOLFSSL_SERVER)

#define TLS_SERVER_PORT 11111
#define TX_BUF_SIZE 64
#define RX_BUF_SIZE 1024
#define TCP_SERVER_CONN_Q_SIZE 1

/* derived from wolfSSL/certs/server-ecc.der */

static const unsigned char server_ecc_der_256[] = { 0x30, 0x82, 0x03, 0x10,
        0x30, 0x82, 0x02, 0xB5, 0xA0, 0x03, 0x02, 0x01, 0x02, 0x02, 0x09, 0x00,
        0xEF, 0x46, 0xC7, 0xA4, 0x9B, 0xBB, 0x60, 0xD3, 0x30, 0x0A, 0x06, 0x08,
        0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x04, 0x03, 0x02, 0x30, 0x81, 0x8F, 0x31,
        0x0B, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x55, 0x53,
        0x31, 0x13, 0x30, 0x11, 0x06, 0x03, 0x55, 0x04, 0x08, 0x0C, 0x0A, 0x57,
        0x61, 0x73, 0x68, 0x69, 0x6E, 0x67, 0x74, 0x6F, 0x6E, 0x31, 0x10, 0x30,
        0x0E, 0x06, 0x03, 0x55, 0x04, 0x07, 0x0C, 0x07, 0x53, 0x65, 0x61, 0x74,
        0x74, 0x6C, 0x65, 0x31, 0x10, 0x30, 0x0E, 0x06, 0x03, 0x55, 0x04, 0x0A,
        0x0C, 0x07, 0x45, 0x6C, 0x69, 0x70, 0x74, 0x69, 0x63, 0x31, 0x0C, 0x30,
        0x0A, 0x06, 0x03, 0x55, 0x04, 0x0B, 0x0C, 0x03, 0x45, 0x43, 0x43, 0x31,
        0x18, 0x30, 0x16, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0C, 0x0F, 0x77, 0x77,
        0x77, 0x2E, 0x77, 0x6F, 0x6C, 0x66, 0x73, 0x73, 0x6C, 0x2E, 0x63, 0x6F,
        0x6D, 0x31, 0x1F, 0x30, 0x1D, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7,
        0x0D, 0x01, 0x09, 0x01, 0x16, 0x10, 0x69, 0x6E, 0x66, 0x6F, 0x40, 0x77,
        0x6F, 0x6C, 0x66, 0x73, 0x73, 0x6C, 0x2E, 0x63, 0x6F, 0x6D, 0x30, 0x1E,
        0x17, 0x0D, 0x31, 0x36, 0x30, 0x38, 0x31, 0x31, 0x32, 0x30, 0x30, 0x37,
        0x33, 0x38, 0x5A, 0x17, 0x0D, 0x31, 0x39, 0x30, 0x35, 0x30, 0x38, 0x32,
        0x30, 0x30, 0x37, 0x33, 0x38, 0x5A, 0x30, 0x81, 0x8F, 0x31, 0x0B, 0x30,
        0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x55, 0x53, 0x31, 0x13,
        0x30, 0x11, 0x06, 0x03, 0x55, 0x04, 0x08, 0x0C, 0x0A, 0x57, 0x61, 0x73,
        0x68, 0x69, 0x6E, 0x67, 0x74, 0x6F, 0x6E, 0x31, 0x10, 0x30, 0x0E, 0x06,
        0x03, 0x55, 0x04, 0x07, 0x0C, 0x07, 0x53, 0x65, 0x61, 0x74, 0x74, 0x6C,
        0x65, 0x31, 0x10, 0x30, 0x0E, 0x06, 0x03, 0x55, 0x04, 0x0A, 0x0C, 0x07,
        0x45, 0x6C, 0x69, 0x70, 0x74, 0x69, 0x63, 0x31, 0x0C, 0x30, 0x0A, 0x06,
        0x03, 0x55, 0x04, 0x0B, 0x0C, 0x03, 0x45, 0x43, 0x43, 0x31, 0x18, 0x30,
        0x16, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0C, 0x0F, 0x77, 0x77, 0x77, 0x2E,
        0x77, 0x6F, 0x6C, 0x66, 0x73, 0x73, 0x6C, 0x2E, 0x63, 0x6F, 0x6D, 0x31,
        0x1F, 0x30, 0x1D, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01,
        0x09, 0x01, 0x16, 0x10, 0x69, 0x6E, 0x66, 0x6F, 0x40, 0x77, 0x6F, 0x6C,
        0x66, 0x73, 0x73, 0x6C, 0x2E, 0x63, 0x6F, 0x6D, 0x30, 0x59, 0x30, 0x13,
        0x06, 0x07, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x02, 0x01, 0x06, 0x08, 0x2A,
        0x86, 0x48, 0xCE, 0x3D, 0x03, 0x01, 0x07, 0x03, 0x42, 0x00, 0x04, 0xBB,
        0x33, 0xAC, 0x4C, 0x27, 0x50, 0x4A, 0xC6, 0x4A, 0xA5, 0x04, 0xC3, 0x3C,
        0xDE, 0x9F, 0x36, 0xDB, 0x72, 0x2D, 0xCE, 0x94, 0xEA, 0x2B, 0xFA, 0xCB,
        0x20, 0x09, 0x39, 0x2C, 0x16, 0xE8, 0x61, 0x02, 0xE9, 0xAF, 0x4D, 0xD3,
        0x02, 0x93, 0x9A, 0x31, 0x5B, 0x97, 0x92, 0x21, 0x7F, 0xF0, 0xCF, 0x18,
        0xDA, 0x91, 0x11, 0x02, 0x34, 0x86, 0xE8, 0x20, 0x58, 0x33, 0x0B, 0x80,
        0x34, 0x89, 0xD8, 0xA3, 0x81, 0xF7, 0x30, 0x81, 0xF4, 0x30, 0x1D, 0x06,
        0x03, 0x55, 0x1D, 0x0E, 0x04, 0x16, 0x04, 0x14, 0x5D, 0x5D, 0x26, 0xEF,
        0xAC, 0x7E, 0x36, 0xF9, 0x9B, 0x76, 0x15, 0x2B, 0x4A, 0x25, 0x02, 0x23,
        0xEF, 0xB2, 0x89, 0x30, 0x30, 0x81, 0xC4, 0x06, 0x03, 0x55, 0x1D, 0x23,
        0x04, 0x81, 0xBC, 0x30, 0x81, 0xB9, 0x80, 0x14, 0x5D, 0x5D, 0x26, 0xEF,
        0xAC, 0x7E, 0x36, 0xF9, 0x9B, 0x76, 0x15, 0x2B, 0x4A, 0x25, 0x02, 0x23,
        0xEF, 0xB2, 0x89, 0x30, 0xA1, 0x81, 0x95, 0xA4, 0x81, 0x92, 0x30, 0x81,
        0x8F, 0x31, 0x0B, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02,
        0x55, 0x53, 0x31, 0x13, 0x30, 0x11, 0x06, 0x03, 0x55, 0x04, 0x08, 0x0C,
        0x0A, 0x57, 0x61, 0x73, 0x68, 0x69, 0x6E, 0x67, 0x74, 0x6F, 0x6E, 0x31,
        0x10, 0x30, 0x0E, 0x06, 0x03, 0x55, 0x04, 0x07, 0x0C, 0x07, 0x53, 0x65,
        0x61, 0x74, 0x74, 0x6C, 0x65, 0x31, 0x10, 0x30, 0x0E, 0x06, 0x03, 0x55,
        0x04, 0x0A, 0x0C, 0x07, 0x45, 0x6C, 0x69, 0x70, 0x74, 0x69, 0x63, 0x31,
        0x0C, 0x30, 0x0A, 0x06, 0x03, 0x55, 0x04, 0x0B, 0x0C, 0x03, 0x45, 0x43,
        0x43, 0x31, 0x18, 0x30, 0x16, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0C, 0x0F,
        0x77, 0x77, 0x77, 0x2E, 0x77, 0x6F, 0x6C, 0x66, 0x73, 0x73, 0x6C, 0x2E,
        0x63, 0x6F, 0x6D, 0x31, 0x1F, 0x30, 0x1D, 0x06, 0x09, 0x2A, 0x86, 0x48,
        0x86, 0xF7, 0x0D, 0x01, 0x09, 0x01, 0x16, 0x10, 0x69, 0x6E, 0x66, 0x6F,
        0x40, 0x77, 0x6F, 0x6C, 0x66, 0x73, 0x73, 0x6C, 0x2E, 0x63, 0x6F, 0x6D,
        0x82, 0x09, 0x00, 0xEF, 0x46, 0xC7, 0xA4, 0x9B, 0xBB, 0x60, 0xD3, 0x30,
        0x0C, 0x06, 0x03, 0x55, 0x1D, 0x13, 0x04, 0x05, 0x30, 0x03, 0x01, 0x01,
        0xFF, 0x30, 0x0A, 0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x04, 0x03,
        0x02, 0x03, 0x49, 0x00, 0x30, 0x46, 0x02, 0x21, 0x00, 0xF1, 0xD0, 0xA6,
        0x3E, 0x83, 0x33, 0x24, 0xD1, 0x7A, 0x05, 0x5F, 0x1E, 0x0E, 0xBD, 0x7D,
        0x6B, 0x33, 0xE9, 0xF2, 0x86, 0xF3, 0xF3, 0x3D, 0xA9, 0xEF, 0x6A, 0x87,
        0x31, 0xB3, 0xB7, 0x7E, 0x50, 0x02, 0x21, 0x00, 0xF0, 0x60, 0xDD, 0xCE,
        0xA2, 0xDB, 0x56, 0xEC, 0xD9, 0xF4, 0xE4, 0xE3, 0x25, 0xD4, 0xB0, 0xC9,
        0x25, 0x7D, 0xCA, 0x7A, 0x5D, 0xBA, 0xC4, 0xB2, 0xF6, 0x7D, 0x04, 0xC7,
        0xBD, 0x62, 0xC9, 0x20 };

/* derived from wolfSSL/certs/ecc-key.der */

static const unsigned char ecc_key_der_256[] = { 0x30, 0x77, 0x02, 0x01, 0x01,
        0x04, 0x20, 0x45, 0xB6, 0x69, 0x02, 0x73, 0x9C, 0x6C, 0x85, 0xA1, 0x38,
        0x5B, 0x72, 0xE8, 0xE8, 0xC7, 0xAC, 0xC4, 0x03, 0x8D, 0x53, 0x35, 0x04,
        0xFA, 0x6C, 0x28, 0xDC, 0x34, 0x8D, 0xE1, 0xA8, 0x09, 0x8C, 0xA0, 0x0A,
        0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x01, 0x07, 0xA1, 0x44,
        0x03, 0x42, 0x00, 0x04, 0xBB, 0x33, 0xAC, 0x4C, 0x27, 0x50, 0x4A, 0xC6,
        0x4A, 0xA5, 0x04, 0xC3, 0x3C, 0xDE, 0x9F, 0x36, 0xDB, 0x72, 0x2D, 0xCE,
        0x94, 0xEA, 0x2B, 0xFA, 0xCB, 0x20, 0x09, 0x39, 0x2C, 0x16, 0xE8, 0x61,
        0x02, 0xE9, 0xAF, 0x4D, 0xD3, 0x02, 0x93, 0x9A, 0x31, 0x5B, 0x97, 0x92,
        0x21, 0x7F, 0xF0, 0xCF, 0x18, 0xDA, 0x91, 0x11, 0x02, 0x34, 0x86, 0xE8,
        0x20, 0x58, 0x33, 0x0B, 0x80, 0x34, 0x89, 0xD8 };


void wolfssl_server_test(uintData_t statusPtr)
{
    int sock_listen;
    int bindStatus;
    int sock_req;
    sockaddr_in socketAddr;
    sockaddr_in server_addr;
    int  socketAddrLen=sizeof(sockaddr);
    char rx_buf[RX_BUF_SIZE];
    char tx_buf[TX_BUF_SIZE];
    clientConnectionHandleType TCPserverHandle;

    WOLFSSL * ssl;
    WOLFSSL_CTX * ctx;
    int tx_buf_sz = 0, ret = 0, error = 0;

    /* set up the mailbox transport */
    /* connectionId2 is defined in the mailbox-transport.config*/
    if (setupTransport(&TCPserverHandle, (char*)"connectionId2") != transportSuccess){
        printf("TCP transport set up failed \n");
        return;
      }

    /* SET UP NETWORK SOCKET */

    printf("Opening network socket...\n");
    sock_listen = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_listen == SOCKET_ERROR) {
        printf("ERROR: socket, err = %d\n", errno);
        return;
    }

    printf("Clearing memory for server_addr struct\n");
    XMEMSET((char *) &server_addr, 0u, sizeof(server_addr));

    printf("Setting up server_addr struct\n");
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(TLS_SERVER_PORT);

    bindStatus = bind(sock_listen, (sockaddr *) &server_addr, sizeof(server_addr));
    if (bindStatus == SOCKET_ERROR) {
       printf("ERROR: bind, err = %d\n", errno);
       closesocket(sock_listen);
       return;
    }

    /* wolfSSL INIT and CTX SETUP */

    wolfSSL_Init();

    /* chooses the highest possible TLS version */

    ctx = wolfSSL_CTX_new(wolfSSLv23_server_method());

    if (ctx == 0) {
        printf("ERROR: wolfSSL_CTX_new failed\n");
        closesocket(sock_listen);
        return;
    }
    WOLFSSL_MSG("wolfSSL_CTX_new done");

    ret = wolfSSL_CTX_use_certificate_buffer(ctx,
                                             server_ecc_der_256,
                                             sizeof(server_ecc_der_256),
                                             SSL_FILETYPE_ASN1);
    if (ret != SSL_SUCCESS) {
        printf("ERROR: wolfSSL_CTX_use_certificate_buffer() failed, \
                err = %d\n", ret);
        closesocket(sock_listen);
        wolfSSL_CTX_free(ctx);
        return;
    }
    ret = wolfSSL_CTX_use_PrivateKey_buffer(ctx,
                                            ecc_key_der_256,
                                            sizeof(ecc_key_der_256),
                                            SSL_FILETYPE_ASN1);
    if (ret != SSL_SUCCESS) {
        printf("ERROR: wolfSSL_CTX_use_PrivateKey_buffer() failed\n");
        closesocket(sock_listen);
        wolfSSL_CTX_free(ctx);
        return;
    }
    /* accept client socket connections */
    printf("Listening for client connection\n");
    printf("E.g, you can use ./examples/client/client.exe -h 192.168.219.100\n");
    printf("    \n");

    listen(sock_listen, TCP_SERVER_CONN_Q_SIZE);

    sock_req = accept(sock_listen,
                     (sockaddr *) &socketAddr,
                     &socketAddrLen);

    if (sock_req == -1) {
        printf("ERROR: accept, err = %d\n", errno);
        closesocket(sock_listen);
        return;
    }

    printf("Got client connection! Starting TLS negotiation\n");

    #ifdef DEBUG_WOLFSSL
        wolfSSL_Debugging_ON();
    #endif

    /* set up wolfSSL session */
    ssl = wolfSSL_new(ctx);
    if (ssl == NULL) {
        printf("ERROR: wolfSSL_new() failed\n");
        closesocket(sock_req);
        closesocket(sock_listen);
        wolfSSL_CTX_free(ctx);
        return;
    }

    WOLFSSL_MSG("wolfSSL_new done");
    ret = wolfSSL_set_fd(ssl, sock_req);
    if (ret != SSL_SUCCESS) {
        printf("ERROR: wolfSSL_set_fd() failed\n");
        closesocket(sock_req);
        closesocket(sock_listen);
        wolfSSL_free(ssl);
        wolfSSL_CTX_free(ctx);
        return;
    }

    WOLFSSL_MSG("wolfSSL_set_fd done");
    do {
        error = 0; /* reset error */
        if (ret != SSL_SUCCESS) {
            error = wolfSSL_get_error(ssl, 0);
            printf("ERROR: wolfSSL_accept() failed, err = %d\n", error);
            if (error != SSL_ERROR_WANT_READ) {
                closesocket(sock_req);
                closesocket(sock_listen);
                wolfSSL_free(ssl);
                wolfSSL_CTX_free(ctx);
                return;
            }
            /* goToSleep() for 500 milliseconds */
        }
    } while ((ret != SSL_SUCCESS) && (error == SSL_ERROR_WANT_READ));

    printf("wolfSSL_accept() ok...\n");

    /* read client data */

    error = 0;
    XMEMSET(rx_buf, 0u, RX_BUF_SIZE);
    ret = wolfSSL_read(ssl, rx_buf, RX_BUF_SIZE - 1);
    if (ret < 0) {
        error = wolfSSL_get_error(ssl, 0);
        if (error != SSL_ERROR_WANT_READ) {
            printf("wolfSSL_read failed, error = %d\n", error);
            closesocket(sock_req);
            closesocket(sock_listen);
            wolfSSL_free(ssl);
            wolfSSL_CTX_free(ctx);
            return;
        }
    }

    printf("AFTER wolfSSL_read() call, ret = %d\n", ret);
    if (ret > 0) {
        rx_buf[ret] = 0;
        printf("Client sent: %s\n", rx_buf);
    }
    /* write response to client */
    XMEMSET(tx_buf, 0u, TX_BUF_SIZE);
    tx_buf_sz = 22;
    XSTRNCPY(tx_buf, "I hear ya fa shizzle!\n", tx_buf_sz);
    if (wolfSSL_write(ssl, tx_buf, tx_buf_sz) != tx_buf_sz) {
        error = wolfSSL_get_error(ssl, 0);
        printf("ERROR: wolfSSL_write() failed, err = %d\n", error);
        closesocket(sock_req);
        closesocket(sock_listen);
        wolfSSL_free(ssl);
        wolfSSL_CTX_free(ctx);
        return;
    }
    ret = wolfSSL_shutdown(ssl);
    if (ret == SSL_SHUTDOWN_NOT_DONE)
        wolfSSL_shutdown(ssl);

        wolfSSL_free(ssl);
        wolfSSL_CTX_free(ctx);
        wolfSSL_Cleanup();
        closesocket(sock_req);
        closesocket(sock_listen);
    return;
}

#endif /* NO_WOLFSSL_SERVER */

int  wolfsslRunTests (void)
{
    thread_handle_t TCPhandle;
    threadStatus ts;

    #if !defined(NO_CRYPT_TEST)
        wolfcrypt_test(NULL);
    #endif
    #if !defined(NO_CRYPT_BENCHMARK)
        benchmark_test(NULL);
    #endif
    #if !defined(NO_WOLFSSL_CLIENT)
        ts = createThread("TCPclient", "TCPThreadTemplate", wolfssl_client_test,
                          0, &TCPhandle );
        if (ts != threadSuccess) {
            printf("Unable to create TCP client thread, %i ", (size_t)ts);
        }
    #endif
    #if !defined(NO_WOLFSSL_SERVER)
        ts = createThread("TCPserver", "TCPThreadTemplate", wolfssl_server_test,
                          0, &TCPhandle );
        if (ts != threadSuccess) {
            printf("Unable to create TCP server thread, %i ", (size_t)ts);
        }
    #endif

    return 0;
}
