/* echoclient.c
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


#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>
/* let's use cyassl layer AND cyassl openssl layer */
#undef TEST_OPENSSL_COEXIST /* can't use this option with this example */
#include <wolfssl/ssl.h>

/* Force enable the compatibility macros for this example */
#ifdef WOLFSSL_DTLS
    #include <wolfssl/error-ssl.h>
#endif

#if defined(WOLFSSL_MDK_ARM) || defined(WOLFSSL_KEIL_TCP_NET)
        #include <stdio.h>
        #include <string.h>
        #include "cmsis_os.h"
        #include "rl_fs.h"
        #include "rl_net.h"
        #include "wolfssl_MDK_ARM.h"
#endif

#include <wolfssl/test.h>

#ifndef OPENSSL_EXTRA_X509_SMALL
#define OPENSSL_EXTRA_X509_SMALL
#endif
#include <wolfssl/openssl/ssl.h>

#include <examples/echoclient/echoclient.h>

#ifndef NO_WOLFSSL_CLIENT


#ifdef NO_FILESYSTEM
#ifdef NO_RSA
#error currently the example only tries to load in a RSA buffer
#endif
#undef USE_CERT_BUFFERS_2048
#define USE_CERT_BUFFERS_2048
#include <wolfssl/certs_test.h>
#endif

#ifdef WOLFSSL_ASYNC_CRYPT
    static int devId = INVALID_DEVID;
#endif


void echoclient_test(void* args)
{
    SOCKET_T sockfd = 0;

    FILE* fin   = stdin  ;
    FILE* fout = stdout;

#ifndef WOLFSSL_MDK_SHELL
    int inCreated  = 0;
    int outCreated = 0;
#endif

    char msg[1024];
    char reply[1024+1];

    SSL_METHOD* method = 0;
    SSL_CTX*    ctx    = 0;
    SSL*        ssl    = 0;

    int ret = 0, err = 0;
    int doDTLS = 0;
    int doPSK = 0;
    int sendSz;
#ifndef WOLFSSL_MDK_SHELL
    int argc    = 0;
    char** argv = 0;
#endif
    word16 port;
    char buffer[WOLFSSL_MAX_ERROR_SZ];

    ((func_args*)args)->return_code = -1; /* error state */

#ifndef WOLFSSL_MDK_SHELL
    argc = ((func_args*)args)->argc;
    argv = ((func_args*)args)->argv;

    if (argc >= 2) {
        fin  = fopen(argv[1], "r");
        inCreated = 1;
    }
    if (argc >= 3) {
        fout = fopen(argv[2], "w");
        outCreated = 1;
    }
#endif

    if (!fin)  err_sys("can't open input file");
    if (!fout) err_sys("can't open output file");

#ifdef WOLFSSL_DTLS
    doDTLS  = 1;
#endif

#ifdef WOLFSSL_LEANPSK
    doPSK = 1;
#endif
#if defined(NO_RSA) && !defined(HAVE_ECC) && !defined(HAVE_ED25519) && \
                                                            !defined(HAVE_ED448)
    doPSK = 1;
#endif
    (void)doPSK;

#if defined(NO_MAIN_DRIVER) && !defined(USE_WINDOWS_API) && !defined(WOLFSSL_MDK_SHELL)
    port = ((func_args*)args)->signal->port;
#else
    port = wolfSSLPort;
#endif

#if defined(WOLFSSL_DTLS)
    #ifdef WOLFSSL_DTLS13
    method = wolfDTLSv1_3_client_method();
    #elif !defined(WOLFSSL_NO_TLS12)
    method  = DTLSv1_2_client_method();
    #endif
#elif !defined(NO_TLS)
    #if defined(WOLFSSL_TLS13) && defined(WOLFSSL_SNIFFER)
    method = wolfTLSv1_2_client_method();
    #else
    method = wolfSSLv23_client_method();
    #endif
#elif defined(WOLFSSL_ALLOW_SSLV3)
    method = SSLv3_client_method();
#else
    #error "no valid client method type"
#endif
    ctx    = SSL_CTX_new(method);

#ifndef NO_FILESYSTEM
    #ifndef NO_RSA
    if (SSL_CTX_load_verify_locations(ctx, caCertFile, 0) != WOLFSSL_SUCCESS)
        err_sys("can't load ca file, Please run from wolfSSL home dir");
    #endif
    #ifdef HAVE_ECC
        if (SSL_CTX_load_verify_locations(ctx, caEccCertFile, 0) != WOLFSSL_SUCCESS)
            err_sys("can't load ca file, Please run from wolfSSL home dir");
    #elif defined(HAVE_ED25519)
        if (SSL_CTX_load_verify_locations(ctx, caEdCertFile, 0) != WOLFSSL_SUCCESS)
            err_sys("can't load ca file, Please run from wolfSSL home dir");
    #elif defined(HAVE_ED448)
        if (SSL_CTX_load_verify_locations(ctx, caEd448CertFile, 0) != WOLFSSL_SUCCESS)
            err_sys("can't load ca file, Please run from wolfSSL home dir");
    #endif
#elif !defined(NO_CERTS)
    if (!doPSK)
        if (wolfSSL_CTX_load_verify_buffer(ctx, ca_cert_der_2048,
            sizeof_ca_cert_der_2048, WOLFSSL_FILETYPE_ASN1) != WOLFSSL_SUCCESS)
            err_sys("can't load ca buffer");
#endif

#if defined(WOLFSSL_SNIFFER)
    /* Only set if not running testsuite */
    if (XSTRSTR(argv[0], "testsuite") == NULL) {
        /* don't use EDH, can't sniff tmp keys */
        SSL_CTX_set_cipher_list(ctx, "AES256-SHA");
    }
#endif
#ifndef NO_PSK
    if (doPSK) {
        const char *defaultCipherList;

        wolfSSL_CTX_set_psk_client_callback(ctx, my_psk_client_cb);
        #ifdef HAVE_NULL_CIPHER
            defaultCipherList = "PSK-NULL-SHA256";
        #elif defined(HAVE_AESGCM) && !defined(NO_DH)
            #ifdef WOLFSSL_TLS13
            defaultCipherList = "TLS13-AES128-GCM-SHA256"
                #ifndef WOLFSSL_NO_TLS12
                                ":DHE-PSK-AES128-GCM-SHA256"
                #endif
                ;
            #else
            defaultCipherList = "DHE-PSK-AES128-GCM-SHA256";
            #endif
        #elif defined(HAVE_AESGCM) && defined(WOLFSSL_TLS13)
            defaultCipherList = "TLS13-AES128-GCM-SHA256"
                #ifndef WOLFSSL_NO_TLS12
                                ":DHE-PSK-AES128-GCM-SHA256"
                #endif
                ;
        #else
            defaultCipherList = "PSK-AES128-CBC-SHA256";
        #endif
        if (wolfSSL_CTX_set_cipher_list(ctx,defaultCipherList) !=WOLFSSL_SUCCESS)
            err_sys("client can't set cipher list 2");
        wolfSSL_CTX_set_psk_callback_ctx(ctx, (void*)defaultCipherList);
    }
#endif

#ifdef WOLFSSL_ENCRYPTED_KEYS
    SSL_CTX_set_default_passwd_cb(ctx, PasswordCallBack);
#endif

#if defined(WOLFSSL_MDK_ARM)
    wolfSSL_CTX_set_verify(ctx, WOLFSSL_VERIFY_NONE, 0);
#endif

#ifdef WOLFSSL_ASYNC_CRYPT
    ret = wolfAsync_DevOpen(&devId);
    if (ret < 0) {
        fprintf(stderr, "Async device open failed\nRunning without async\n");
    }
    wolfSSL_CTX_SetDevId(ctx, devId);
#endif /* WOLFSSL_ASYNC_CRYPT */

    ssl = SSL_new(ctx);
    tcp_connect(&sockfd, wolfSSLIP, port, doDTLS, 0, ssl);

    SSL_set_fd(ssl, sockfd);
#if defined(USE_WINDOWS_API) && defined(WOLFSSL_DTLS) && defined(NO_MAIN_DRIVER)
    /* let echoserver bind first, TODO: add Windows signal like pthreads does */
    Sleep(100);
#endif

    do {
        err = 0; /* Reset error */
        ret = SSL_connect(ssl);
        if (ret != WOLFSSL_SUCCESS) {
            err = SSL_get_error(ssl, 0);
        #ifdef WOLFSSL_ASYNC_CRYPT
            if (err == WC_PENDING_E) {
                ret = wolfSSL_AsyncPoll(ssl, WOLF_POLL_FLAG_CHECK_HW);
                if (ret < 0) break;
            }
        #endif
        }
    } while (err == WC_PENDING_E);
    if (ret != WOLFSSL_SUCCESS) {
        fprintf(stderr, "SSL_connect error %d, %s\n", err,
            ERR_error_string(err, buffer));
        err_sys("SSL_connect failed");
    }

    while (fgets(msg, sizeof(msg), fin) != 0) {

        sendSz = (int)XSTRLEN(msg);

        do {
            err = 0; /* reset error */
            ret = SSL_write(ssl, msg, sendSz);
            if (ret <= 0) {
                err = SSL_get_error(ssl, 0);
            #ifdef WOLFSSL_ASYNC_CRYPT
                if (err == WC_PENDING_E) {
                    ret = wolfSSL_AsyncPoll(ssl, WOLF_POLL_FLAG_CHECK_HW);
                    if (ret < 0) break;
                }
            #endif
            }
        } while (err == WC_PENDING_E);
        if (ret != sendSz) {
            fprintf(stderr, "SSL_write msg error %d, %s\n", err,
                ERR_error_string(err, buffer));
            err_sys("SSL_write failed");
        }

        if (strncmp(msg, "quit", 4) == 0) {
            LIBCALL_CHECK_RET(fputs("sending server shutdown command: quit!\n",
                                    fout));
            break;
        }

        if (strncmp(msg, "break", 5) == 0) {
            LIBCALL_CHECK_RET(fputs("sending server session close: break!\n",
                                    fout));
            break;
        }

    #ifndef WOLFSSL_MDK_SHELL
        while (sendSz)
    #endif
        {
            do {
                err = 0; /* reset error */
                ret = SSL_read(ssl, reply, sizeof(reply)-1);
                if (ret <= 0) {
                    err = SSL_get_error(ssl, 0);
                #ifdef WOLFSSL_ASYNC_CRYPT
                    if (err == WC_PENDING_E) {
                        ret = wolfSSL_AsyncPoll(ssl, WOLF_POLL_FLAG_CHECK_HW);
                        if (ret < 0) break;
                    }
                #endif
                }
            } while (err == WC_PENDING_E);
            if (ret > 0) {
                reply[ret] = 0;
                LIBCALL_CHECK_RET(fputs(reply, fout));
                LIBCALL_CHECK_RET(fflush(fout));
                sendSz -= ret;
            }
#ifdef WOLFSSL_DTLS
            else if (wolfSSL_dtls(ssl) && err == DECRYPT_ERROR) {
                /* This condition is OK. The packet should be dropped
                 * silently when there is a decrypt or MAC error on
                 * a DTLS record. */
                sendSz = 0;
            }
#endif
            else {
                fprintf(stderr, "SSL_read msg error %d, %s\n", err,
                    ERR_error_string(err, buffer));
                err_sys("SSL_read failed");
            }
        }
    }


#ifdef WOLFSSL_DTLS
    strncpy(msg, "break", 6);
    sendSz = (int)strlen(msg);
    /* try to tell server done */
    do {
        err = 0; /* reset error */
        ret = SSL_write(ssl, msg, sendSz);
        if (ret <= 0) {
            err = SSL_get_error(ssl, 0);
        #ifdef WOLFSSL_ASYNC_CRYPT
            if (err == WC_PENDING_E) {
                ret = wolfSSL_AsyncPoll(ssl, WOLF_POLL_FLAG_CHECK_HW);
                if (ret < 0) break;
            }
        #endif
        }
    } while (err == WC_PENDING_E);
#else
    SSL_shutdown(ssl);
#endif

    SSL_free(ssl);
    SSL_CTX_free(ctx);

#ifdef WOLFSSL_ASYNC_CRYPT
    wolfAsync_DevClose(&devId);
#endif

    LIBCALL_CHECK_RET(fflush(fout));
#ifndef WOLFSSL_MDK_SHELL
    if (inCreated)  fclose(fin);
    if (outCreated) fclose(fout);
#endif

    CloseSocket(sockfd);
    ((func_args*)args)->return_code = 0;
}

#endif /* !NO_WOLFSSL_CLIENT */

/* so overall tests can pull in test function */
#ifndef NO_MAIN_DRIVER

    int main(int argc, char** argv)
    {
        func_args args;

#ifdef HAVE_WNR
        if (wc_InitNetRandom(wnrConfig, NULL, 5000) != 0)
            err_sys("Whitewood netRandom global config failed");
#endif

        StartTCP();

        args.argc = argc;
        args.argv = argv;
        args.return_code = 0;

        wolfSSL_Init();
#if defined(DEBUG_WOLFSSL) && !defined(WOLFSSL_MDK_SHELL)
        wolfSSL_Debugging_ON();
#endif
#ifndef WOLFSSL_TIRTOS
        ChangeToWolfRoot();
#endif
#ifndef NO_WOLFSSL_CLIENT
        echoclient_test(&args);
#endif

        wolfSSL_Cleanup();

#ifdef HAVE_WNR
        if (wc_FreeNetRandom() < 0)
            err_sys("Failed to free netRandom context");
#endif /* HAVE_WNR */

        return args.return_code;
    }

#endif /* NO_MAIN_DRIVER */
