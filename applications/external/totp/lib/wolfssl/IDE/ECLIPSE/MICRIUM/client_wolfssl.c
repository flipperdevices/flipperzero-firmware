/* client_wolfssl.c
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

#include  <Source/net_sock.h>
#include  <Source/net_app.h>
#include  <Source/net_ascii.h>
#include  <Source/net_util.h>
#include  <lib_str.h>
#include  <app_cfg.h>

#include  <wolfssl/ssl.h>
#include  "client_wolfssl.h"

/* 172.217.3.174 is the IP address of https://www.google.com */
#define TCP_SERVER_IP_ADDR "172.217.3.174"
#define TCP_SERVER_DOMAIN_NAME "www.google.com"
#define TCP_SERVER_PORT 443

#define TX_BUF_SIZE 64
#define RX_BUF_SIZE 1024

#define TX_MSG "GET /index.html HTTP/1.0\r\n\r\n"
#define TX_MSG_SIZE sizeof(TX_MSG)

static const CPU_INT08U google_certs_ca[]="\n\
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

int wolfssl_client_test(void) {
    NET_ERR err;
    NET_SOCK_ID sock;
    NET_IPv4_ADDR server_ip_addr;
    NET_SOCK_ADDR_IPv4 server_addr;
    CPU_CHAR rx_buf[RX_BUF_SIZE];
    CPU_CHAR tx_buf[TX_BUF_SIZE];
    OS_ERR os_err;
    int ret = 0, error = 0;

    WOLFSSL* ssl;
    WOLFSSL_CTX* ctx;

    #ifdef DEBUG_WOLFSSL
        wolfSSL_Debugging_ON();
    #endif

    /* wolfSSL INIT and CTX SETUP */

    wolfSSL_Init();

    /* SET UP NETWORK SOCKET */

    APP_TRACE_INFO(("Opening a network socket...\r\n"));

    sock = NetSock_Open(NET_SOCK_ADDR_FAMILY_IP_V4,
                        NET_SOCK_TYPE_STREAM,
                        NET_SOCK_PROTOCOL_TCP,
                        &err);
    if (err != NET_SOCK_ERR_NONE) {
        APP_TRACE_INFO(("ERROR: NetSock_Open, err = %d\r\n", (int) err));
        return -1;
    }

#ifdef NET_SECURE_MODULE_EN
    APP_TRACE_INFO(("Setting the socket as secure...\r\n"));

    (void)NetSock_CfgSecure(sock,
                            DEF_YES,
                            &err);
    if (err != NET_SOCK_ERR_NONE) {
        APP_TRACE_INFO(("ERROR: NetSock_CfgSecure, err = %d\r\n", (int) err));
        NetSock_Close(sock, &err);
        return -1;
    }

    APP_TRACE_INFO(("Configure the common name of the server...\r\n"));
    (void)NetSock_CfgSecureClientCommonName(sock,
                                           TCP_SERVER_DOMAIN_NAME,
                                           &err);
    if (err != NET_SOCK_ERR_NONE) {
        APP_TRACE_INFO(("ERROR: NetSock_CfgSecureClientCommonName, \
                        err = %d\r\n", (int) err));
        NetSock_Close(sock, &err);
        return -1;
    }
#endif /* NET_SECURE_MODULE_EN */

    APP_TRACE_INFO(("Calling NetASCII_Str_to_IPv4...\r\n"));
    server_ip_addr = NetASCII_Str_to_IPv4(TCP_SERVER_IP_ADDR, &err);
    if (err != NET_ASCII_ERR_NONE) {
        APP_TRACE_INFO(("ERROR: NetASCII_Str_to_IPv4, err = %d\r\n", (int) err));
        NetSock_Close(sock, &err);
        return -1;
    }

    APP_TRACE_INFO(("Clearing memory for server_addr struct\r\n"));

    Mem_Clr((void *) &server_addr, (CPU_SIZE_T) sizeof(server_addr));

    APP_TRACE_INFO(("Setting server IP address: %s, port: %d\r\n",
                    TCP_SERVER_IP_ADDR, TCP_SERVER_PORT));

    server_addr.AddrFamily = NET_SOCK_ADDR_FAMILY_IP_V4;
    server_addr.Addr = NET_UTIL_HOST_TO_NET_32(server_ip_addr);
    server_addr.Port = NET_UTIL_HOST_TO_NET_16(TCP_SERVER_PORT);

    /* CONNECT SOCKET */

    APP_TRACE_INFO(("Calling NetSock_Conn on socket\r\n"));
    NetSock_Conn((NET_SOCK_ID) sock,
                (NET_SOCK_ADDR *) &server_addr,
                (NET_SOCK_ADDR_LEN) sizeof(server_addr),
                (NET_ERR*) &err);
    if (err != NET_SOCK_ERR_NONE) {
        APP_TRACE_INFO(("ERROR: NetSock_Conn, err = %d\r\n", (int) err));
        NetSock_Close(sock, &err);
        return -1;
    }

    ctx = wolfSSL_CTX_new(wolfTLSv1_2_client_method());
    if (ctx == 0) {
        APP_TRACE_INFO(("ERROR: wolfSSL_CTX_new failed\r\n"));
        NetSock_Close(sock, &err);
        return -1;
    }

    APP_TRACE_INFO(("wolfSSL_CTX_new done\r\n"));

    wolfSSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);

    ret = wolfSSL_CTX_load_verify_buffer(ctx,
                                         google_certs_ca,
                                         sizeof(google_certs_ca),
                                         SSL_FILETYPE_PEM);

    if (ret != SSL_SUCCESS) {
        APP_TRACE_INFO(("ERROR: wolfSSL_CTX_load_verify_buffer() failed\r\n"));
        NetSock_Close(sock, &err);
        wolfSSL_CTX_free(ctx);
        return -1;
    }

    if ((ssl = wolfSSL_new(ctx)) == NULL) {
        APP_TRACE_INFO(("ERROR: wolfSSL_new() failed\r\n"));
        NetSock_Close(sock, &err);
        wolfSSL_CTX_free(ctx);
        return -1;
    }

    APP_TRACE_INFO(("wolfSSL_new done\r\n"));
    ret = wolfSSL_set_fd(ssl, sock);
    if (ret != SSL_SUCCESS) {
        APP_TRACE_INFO(("ERROR: wolfSSL_set_fd() failed\r\n"));
        NetSock_Close(sock, &err);
        wolfSSL_free(ssl);
        wolfSSL_CTX_free(ctx);
        return -1;
    }
    APP_TRACE_INFO(("wolfSSL_set_fd done\r\n"));
    do {
        error = 0; /* reset error */
        ret = wolfSSL_connect(ssl);
        if (ret != SSL_SUCCESS) {
            error = wolfSSL_get_error(ssl, 0);
            APP_TRACE_INFO(
                    ("ERROR: wolfSSL_connect() failed, err = %d\r\n", error));
            if (error != SSL_ERROR_WANT_READ) {
                NetSock_Close(sock, &err);
                wolfSSL_free(ssl);
                wolfSSL_CTX_free(ctx);
                return -1;
            }
            OSTimeDlyHMSM(0u, 0u, 1u, 0u, OS_OPT_TIME_HMSM_STRICT, &os_err);
        }
    } while ((ret != SSL_SUCCESS) && (error == SSL_ERROR_WANT_READ));

    APP_TRACE_INFO(("wolfSSL_connect() ok... sending GET\r\n"));
    Str_Copy_N(tx_buf, TX_MSG, TX_MSG_SIZE);
    if (wolfSSL_write(ssl, tx_buf, TX_MSG_SIZE) != TX_MSG_SIZE) {
        error = wolfSSL_get_error(ssl, 0);
        APP_TRACE_INFO(("ERROR: wolfSSL_write() failed, err = %d\r\n", error));
        NetSock_Close(sock, &err);
        wolfSSL_free(ssl);
        wolfSSL_CTX_free(ctx);
        return -1;
    }
    do {
        error = 0; /* reset error */
        ret = wolfSSL_read(ssl, rx_buf, RX_BUF_SIZE - 1);
        if (ret < 0) {
            error = wolfSSL_get_error(ssl, 0);
            if (error != SSL_ERROR_WANT_READ) {
                APP_TRACE_INFO(("wolfSSL_read failed, error = %d\r\n", error));
                NetSock_Close(sock, &err);
                wolfSSL_free(ssl);
                wolfSSL_CTX_free(ctx);
                return -1;
            }
            OSTimeDlyHMSM(0u, 0u, 1u, 0u, OS_OPT_TIME_HMSM_STRICT, &os_err);
        } else if (ret > 0) {
            rx_buf[ret] = 0;
            APP_TRACE_INFO(("%s\r\n", rx_buf));
        }
    } while (error == SSL_ERROR_WANT_READ);
    wolfSSL_shutdown(ssl);
    wolfSSL_free(ssl);
    wolfSSL_CTX_free(ctx);
    wolfSSL_Cleanup();
    NetSock_Close(sock, &err);
    return 0;
}
