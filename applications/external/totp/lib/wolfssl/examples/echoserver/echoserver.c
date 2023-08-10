/* echoserver.c
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

#include <wolfssl/ssl.h> /* name change portability layer */
#include <wolfssl/wolfcrypt/settings.h>
#ifdef HAVE_ECC
    #include <wolfssl/wolfcrypt/ecc.h>   /* ecc_fp_free */
#endif

#if defined(WOLFSSL_MDK_ARM) || defined(WOLFSSL_KEIL_TCP_NET)
        #include <stdio.h>
        #include <string.h>
        #include "cmsis_os.h"
        #include "rl_fs.h"
        #include "rl_net.h"
        #include "wolfssl_MDK_ARM.h"
#endif

#include <wolfssl/ssl.h>
#include <wolfssl/test.h>

#ifndef NO_MAIN_DRIVER
    #define ECHO_OUT
#endif

#include "examples/echoserver/echoserver.h"

#ifndef NO_WOLFSSL_SERVER

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

#define SVR_COMMAND_SIZE 256

static void SignalReady(void* args, word16 port)
{
#if defined(NO_MAIN_DRIVER) && defined(WOLFSSL_COND)
    /* signal ready to tcp_accept */
    func_args* server_args = (func_args*)args;
    tcp_ready* ready = server_args->signal;
    THREAD_CHECK_RET(wc_LockMutex(&ready->mutex));
    ready->ready = 1;
    ready->port = port;
    THREAD_CHECK_RET(wc_UnLockMutex(&ready->mutex));
    THREAD_CHECK_RET(wolfSSL_CondSignal(&ready->cond));
#endif /* NO_MAIN_DRIVER && WOLFSSL_COND */
    (void)args;
    (void)port;
}


THREAD_RETURN WOLFSSL_THREAD echoserver_test(void* args)
{
    SOCKET_T       sockfd = 0;
    WOLFSSL_METHOD* method = 0;
    WOLFSSL_CTX*    ctx    = 0;

    int    ret = 0;
    int    doDTLS = 0;
    int    doPSK;
    int    outCreated = 0;
    int    shutDown = 0;
    int    useAnyAddr = 0;
    word16 port;
    int    argc = ((func_args*)args)->argc;
    char** argv = ((func_args*)args)->argv;
    char   buffer[WOLFSSL_MAX_ERROR_SZ];
#ifdef HAVE_TEST_SESSION_TICKET
    MyTicketCtx myTicketCtx;
#endif

#ifdef ECHO_OUT
    FILE* fout = stdout;
    if (argc >= 2) {
        fout = fopen(argv[1], "w");
        outCreated = 1;
    }
    if (!fout) err_sys("can't open output file");
#endif
    (void)outCreated;
    (void)argc;
    (void)argv;

    ((func_args*)args)->return_code = -1; /* error state */

#ifdef WOLFSSL_DTLS
    doDTLS  = 1;
#endif

#if (defined(NO_RSA) && !defined(HAVE_ECC) && !defined(HAVE_ED25519) && \
                                !defined(HAVE_ED448)) || defined(WOLFSSL_LEANPSK)
    doPSK = 1;
#else
    doPSK = 0;
#endif

#if defined(NO_MAIN_DRIVER) && !defined(WOLFSSL_SNIFFER) && \
     !defined(WOLFSSL_MDK_SHELL) && !defined(WOLFSSL_TIRTOS) && \
     !defined(USE_WINDOWS_API)
    /* Let tcp_listen assign port */
    port = 0;
#else
    /* Use default port */
    port = wolfSSLPort;
#endif

#if defined(USE_ANY_ADDR)
    useAnyAddr = 1;
#endif

#ifdef WOLFSSL_TIRTOS
    fdOpenSession(Task_self());
#endif

    tcp_listen(&sockfd, &port, useAnyAddr, doDTLS, 0);

#if defined(WOLFSSL_DTLS)
    #ifdef WOLFSSL_DTLS13
    method = wolfDTLSv1_3_server_method();
    #elif !defined(WOLFSSL_NO_TLS12)
    method  = wolfDTLSv1_2_server_method();
    #endif
#elif !defined(NO_TLS)
    #if defined(WOLFSSL_TLS13) && defined(WOLFSSL_SNIFFER)
    method = wolfTLSv1_2_server_method();
    #else
    method = wolfSSLv23_server_method();
    #endif
#elif defined(WOLFSSL_ALLOW_SSLV3)
    method = wolfSSLv3_server_method();
#else
    #error "no valid server method built in"
#endif
    ctx    = wolfSSL_CTX_new(method);
    /* wolfSSL_CTX_set_session_cache_mode(ctx, WOLFSSL_SESS_CACHE_OFF); */

#ifdef WOLFSSL_ENCRYPTED_KEYS
    wolfSSL_CTX_set_default_passwd_cb(ctx, PasswordCallBack);
#endif

#ifdef HAVE_TEST_SESSION_TICKET
    if (TicketInit() != 0)
        err_sys("unable to setup Session Ticket Key context");
    wolfSSL_CTX_set_TicketEncCb(ctx, myTicketEncCb);
    XMEMSET(&myTicketCtx, 0, sizeof(myTicketCtx));
    wolfSSL_CTX_set_TicketEncCtx(ctx, &myTicketCtx);
#endif

#ifndef NO_FILESYSTEM
    if (doPSK == 0) {
    #if defined(HAVE_ECC) && !defined(WOLFSSL_SNIFFER)
        /* ecc */
        if (wolfSSL_CTX_use_certificate_file(ctx, eccCertFile, WOLFSSL_FILETYPE_PEM)
                != WOLFSSL_SUCCESS)
            err_sys("can't load server cert file, "
                    "Please run from wolfSSL home dir");

        if (wolfSSL_CTX_use_PrivateKey_file(ctx, eccKeyFile, WOLFSSL_FILETYPE_PEM)
                != WOLFSSL_SUCCESS)
            err_sys("can't load server key file, "
                    "Please run from wolfSSL home dir");
    #elif defined(HAVE_ED25519) && !defined(WOLFSSL_SNIFFER)
        /* ed25519 */
        if (wolfSSL_CTX_use_certificate_chain_file(ctx, edCertFile)
                != WOLFSSL_SUCCESS)
            err_sys("can't load server cert file, "
                    "Please run from wolfSSL home dir");

        if (wolfSSL_CTX_use_PrivateKey_file(ctx, edKeyFile, WOLFSSL_FILETYPE_PEM)
                != WOLFSSL_SUCCESS)
            err_sys("can't load server key file, "
                    "Please run from wolfSSL home dir");
    #elif defined(HAVE_ED448) && !defined(WOLFSSL_SNIFFER)
        /* ed448 */
        if (wolfSSL_CTX_use_certificate_chain_file(ctx, ed448CertFile)
                != WOLFSSL_SUCCESS)
            err_sys("can't load server cert file, "
                    "Please run from wolfSSL home dir");

        if (wolfSSL_CTX_use_PrivateKey_file(ctx, ed448KeyFile,
                WOLFSSL_FILETYPE_PEM) != WOLFSSL_SUCCESS)
            err_sys("can't load server key file, "
                    "Please run from wolfSSL home dir");
    #elif defined(NO_CERTS)
        /* do nothing, just don't load cert files */
    #else
        /* normal */
        if (wolfSSL_CTX_use_certificate_file(ctx, svrCertFile, WOLFSSL_FILETYPE_PEM)
                != WOLFSSL_SUCCESS)
            err_sys("can't load server cert file, "
                    "Please run from wolfSSL home dir");

        if (wolfSSL_CTX_use_PrivateKey_file(ctx, svrKeyFile, WOLFSSL_FILETYPE_PEM)
                != WOLFSSL_SUCCESS)
            err_sys("can't load server key file, "
                    "Please run from wolfSSL home dir");
    #endif
    } /* doPSK */
#elif !defined(NO_CERTS)
    if (!doPSK) {
        if (wolfSSL_CTX_use_certificate_buffer(ctx, server_cert_der_2048,
            sizeof_server_cert_der_2048, WOLFSSL_FILETYPE_ASN1)
            != WOLFSSL_SUCCESS)
            err_sys("can't load server cert buffer");

        if (wolfSSL_CTX_use_PrivateKey_buffer(ctx, server_key_der_2048,
            sizeof_server_key_der_2048, WOLFSSL_FILETYPE_ASN1)
            != WOLFSSL_SUCCESS)
            err_sys("can't load server key buffer");
    }
#endif

#if defined(WOLFSSL_SNIFFER)
    /* Only set if not running testsuite */
    if (XSTRSTR(argv[0], "testsuite") == NULL) {
        /* don't use EDH, can't sniff tmp keys */
        wolfSSL_CTX_set_cipher_list(ctx, "AES256-SHA");
    }
#endif

    if (doPSK) {
#ifndef NO_PSK
        const char *defaultCipherList;

        wolfSSL_CTX_set_psk_server_callback(ctx, my_psk_server_cb);
        wolfSSL_CTX_use_psk_identity_hint(ctx, "cyassl server");
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
                                ":PSK-AES128-GCM-SHA256"
                #endif
                ;
        #else
            defaultCipherList = "PSK-AES128-CBC-SHA256";
        #endif
        if (wolfSSL_CTX_set_cipher_list(ctx, defaultCipherList) != WOLFSSL_SUCCESS)
            err_sys("server can't set cipher list 2");
        wolfSSL_CTX_set_psk_callback_ctx(ctx, (void*)defaultCipherList);
#endif
    }

#ifdef WOLFSSL_ASYNC_CRYPT
    ret = wolfAsync_DevOpen(&devId);
    if (ret < 0) {
        fprintf(stderr, "Async device open failed\nRunning without async\n");
    }
    wolfSSL_CTX_SetDevId(ctx, devId);
#endif /* WOLFSSL_ASYNC_CRYPT */

    SignalReady(args, port);

    while (!shutDown) {
        WOLFSSL* ssl = NULL;
        WOLFSSL* write_ssl = NULL;   /* may have separate w/ HAVE_WRITE_DUP */
        char    command[SVR_COMMAND_SIZE+1];
        int     clientfd;
        int     firstRead = 1;
        int     gotFirstG = 0;
        int     err = 0;
        SOCKADDR_IN_T client;
        socklen_t     client_len = sizeof(client);
#ifndef WOLFSSL_DTLS
        clientfd = accept(sockfd, (struct sockaddr*)&client,
                         (ACCEPT_THIRD_T)&client_len);
#else
        clientfd = sockfd;
        {
            /* For DTLS, peek at the next datagram so we can get the client's
             * address and set it into the ssl object later to generate the
             * cookie. */
            int n;
            byte b[1500];
            n = (int)recvfrom(clientfd, (char*)b, sizeof(b), MSG_PEEK,
                              (struct sockaddr*)&client, &client_len);
            if (n <= 0)
                err_sys("recvfrom failed");
        }
#endif
        if (WOLFSSL_SOCKET_IS_INVALID(clientfd)) err_sys("tcp accept failed");

        ssl = wolfSSL_new(ctx);
        if (ssl == NULL) err_sys("SSL_new failed");
        wolfSSL_set_fd(ssl, clientfd);
        #ifdef WOLFSSL_DTLS
            wolfSSL_dtls_set_peer(ssl, &client, client_len);
        #endif
        #if !defined(NO_FILESYSTEM) && !defined(NO_DH) && !defined(NO_ASN)
            wolfSSL_SetTmpDH_file(ssl, dhParamFile, WOLFSSL_FILETYPE_PEM);
        #elif !defined(NO_DH)
            SetDH(ssl);  /* will repick suites with DHE, higher than PSK */
        #endif

        do {
            err = 0; /* Reset error */
            ret = wolfSSL_accept(ssl);
            if (ret != WOLFSSL_SUCCESS) {
                err = wolfSSL_get_error(ssl, 0);
            #ifdef WOLFSSL_ASYNC_CRYPT
                if (err == WC_PENDING_E) {
                    ret = wolfSSL_AsyncPoll(ssl, WOLF_POLL_FLAG_CHECK_HW);
                    if (ret < 0) break;
                }
            #endif
            }
        } while (err == WC_PENDING_E);
        if (ret != WOLFSSL_SUCCESS) {
            fprintf(stderr, "SSL_accept error = %d, %s\n", err,
                wolfSSL_ERR_error_string(err, buffer));
            fprintf(stderr, "SSL_accept failed\n");
            wolfSSL_free(ssl);
            CloseSocket(clientfd);
            continue;
        }
#if defined(PEER_INFO)
        showPeer(ssl);
#endif

#ifdef HAVE_WRITE_DUP
        write_ssl = wolfSSL_write_dup(ssl);
        if (write_ssl == NULL) {
            fprintf(stderr, "wolfSSL_write_dup failed\n");
            wolfSSL_free(ssl);
            CloseSocket(clientfd);
            continue;
        }
#else
        write_ssl = ssl;
#endif

        while (1) {
            int echoSz;

            do {
                err = 0; /* reset error */
                ret = wolfSSL_read(ssl, command, sizeof(command)-1);
                if (ret <= 0) {
                    err = wolfSSL_get_error(ssl, 0);
                #ifdef WOLFSSL_ASYNC_CRYPT
                    if (err == WC_PENDING_E) {
                        ret = wolfSSL_AsyncPoll(ssl, WOLF_POLL_FLAG_CHECK_HW);
                        if (ret < 0) break;
                    }
                #endif
                }
            } while (err == WC_PENDING_E);
            if (ret <= 0) {
                if (err != WOLFSSL_ERROR_WANT_READ && err != WOLFSSL_ERROR_ZERO_RETURN){
                    fprintf(stderr, "SSL_read echo error %d, %s!\n", err,
                        wolfSSL_ERR_error_string(err, buffer));
                }
                break;
            }

            echoSz = ret;

            if (firstRead == 1) {
                firstRead = 0;  /* browser may send 1 byte 'G' to start */
                if (echoSz == 1 && command[0] == 'G') {
                    gotFirstG = 1;
                    continue;
                }
            }
            else if (gotFirstG == 1 && strncmp(command, "ET /", 4) == 0) {
                strncpy(command, "GET", 4);
                /* fall through to normal GET */
            }

            if ( strncmp(command, "quit", 4) == 0) {
                printf("client sent quit command: shutting down!\n");
                shutDown = 1;
                break;
            }
            if ( strncmp(command, "break", 5) == 0) {
                printf("client sent break command: closing session!\n");
                break;
            }
#ifdef PRINT_SESSION_STATS
            if ( strncmp(command, "printstats", 10) == 0) {
                wolfSSL_PrintSessionStats();
                break;
            }
#endif
            if (strncmp(command, "GET", 3) == 0) {
                const char resp[] =
                    "HTTP/1.0 200 ok\r\nContent-type: text/html\r\n\r\n"
                    "<html><body BGCOLOR=\"#ffffff\"><pre>\r\n"
                    "greetings from wolfSSL\r\n</pre></body></html>\r\n\r\n";

                echoSz = (int)strlen(resp) + 1;
                if (echoSz > (int)sizeof(command)) {
                    /* Internal error. */
                    err_sys("HTTP response greater than buffer.");
                }
                strncpy(command, resp, sizeof(command));

                do {
                    err = 0; /* reset error */
                    ret = wolfSSL_write(write_ssl, command, echoSz);
                    if (ret <= 0) {
                        err = wolfSSL_get_error(write_ssl, 0);
                    #ifdef WOLFSSL_ASYNC_CRYPT
                        if (err == WC_PENDING_E) {
                            ret = wolfSSL_AsyncPoll(write_ssl, WOLF_POLL_FLAG_CHECK_HW);
                            if (ret < 0) break;
                        }
                    #endif
                    }
                } while (err == WC_PENDING_E);
                if (ret != echoSz) {
                    fprintf(stderr, "SSL_write get error = %d, %s\n", err,
                        wolfSSL_ERR_error_string(err, buffer));
                    err_sys("SSL_write get failed");
                }
                break;
            }
            command[echoSz] = 0;

        #ifdef ECHO_OUT
            LIBCALL_CHECK_RET(fputs(command, fout));
        #endif

            do {
                err = 0; /* reset error */
                ret = wolfSSL_write(write_ssl, command, echoSz);
                if (ret <= 0) {
                    err = wolfSSL_get_error(write_ssl, 0);
                #ifdef WOLFSSL_ASYNC_CRYPT
                    if (err == WC_PENDING_E) {
                        ret = wolfSSL_AsyncPoll(write_ssl, WOLF_POLL_FLAG_CHECK_HW);
                        if (ret < 0) break;
                    }
                #endif
                }
            } while (err == WC_PENDING_E);

            if (ret != echoSz) {
                fprintf(stderr, "SSL_write echo error = %d, %s\n", err,
                        wolfSSL_ERR_error_string(err, buffer));
                err_sys("SSL_write echo failed");
            }
        }
#ifndef WOLFSSL_DTLS
        wolfSSL_shutdown(ssl);
#endif
#ifdef HAVE_WRITE_DUP
        wolfSSL_free(write_ssl);
#endif
        wolfSSL_free(ssl);
        CloseSocket(clientfd);
#ifdef WOLFSSL_DTLS
        tcp_listen(&sockfd, &port, useAnyAddr, doDTLS, 0);
        SignalReady(args, port);
#endif
    }

    CloseSocket(sockfd);
    wolfSSL_CTX_free(ctx);

#ifdef ECHO_OUT
    if (outCreated)
        fclose(fout);
#endif

    ((func_args*)args)->return_code = 0;

#if defined(NO_MAIN_DRIVER) && defined(HAVE_ECC) && defined(FP_ECC) \
                            && defined(HAVE_THREAD_LS)
    wc_ecc_fp_free();  /* free per thread cache */
#endif

#ifdef WOLFSSL_TIRTOS
    fdCloseSession(Task_self());
#endif

#ifdef HAVE_TEST_SESSION_TICKET
    TicketCleanup();
#endif

#ifdef WOLFSSL_ASYNC_CRYPT
    wolfAsync_DevClose(&devId);
#endif

    WOLFSSL_RETURN_FROM_THREAD(0);
}

#endif /* !NO_WOLFSSL_SERVER */


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
        ChangeToWolfRoot();
#ifndef NO_WOLFSSL_SERVER
        echoserver_test(&args);
#endif
        wolfSSL_Cleanup();

#ifdef HAVE_WNR
        if (wc_FreeNetRandom() < 0)
            err_sys("Failed to free netRandom context");
#endif /* HAVE_WNR */

        return args.return_code;
    }

#endif /* NO_MAIN_DRIVER */
