/* testsuite.c
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
#include <wolfssl/wolfcrypt/types.h>

#include <wolfssl/ssl.h>
#include <wolfssl/test.h>
#include <wolfcrypt/test/test.h>


#ifndef SINGLE_THREADED

#ifdef OPENSSL_EXTRA
#include <wolfssl/openssl/ssl.h>
#endif
#include <wolfssl/wolfcrypt/sha256.h>
#include <wolfssl/wolfcrypt/ecc.h>

#include <examples/echoclient/echoclient.h>
#include <examples/echoserver/echoserver.h>
#include <examples/server/server.h>
#include <examples/client/client.h>

#define WOLFSSL_TEST_UTILS_INCLUDED
#include "tests/utils.c"

#ifndef NO_SHA256
void file_test(const char* file, byte* check);
#endif

#if !defined(NO_WOLFSSL_SERVER) && !defined(NO_WOLFSSL_CLIENT)

#ifdef HAVE_STACK_SIZE
static THREAD_RETURN simple_test(func_args *args);
#else
static void simple_test(func_args *args);
#endif
static int test_tls(func_args* server_args);
#if !defined(NO_WOLFSSL_SERVER) && !defined(NO_WOLFSSL_CLIENT) && \
    defined(HAVE_CRL) && defined(HAVE_CRL_MONITOR)
static int test_crl_monitor(void);
#endif
static void show_ciphers(void);
static void cleanup_output(void);
static int validate_cleanup_output(void);

enum {
    NUMARGS = 3
};

static const char *outputName;
#endif

int myoptind = 0;
char* myoptarg = NULL;

#ifndef NO_TESTSUITE_MAIN_DRIVER

    static int testsuite_test(int argc, char** argv);

    int main(int argc, char** argv)
    {
        return testsuite_test(argc, argv);
    }

#endif /* NO_TESTSUITE_MAIN_DRIVER */

#ifdef HAVE_STACK_SIZE
/* Wrap TLS echo client to free thread locals. */
static void *echoclient_test_wrapper(void* args) {
    echoclient_test(args);

#if defined(HAVE_ECC) && defined(FP_ECC) && defined(HAVE_THREAD_LS)
    wc_ecc_fp_free();  /* free per thread cache */
#endif

    return (void *)0;
}
#endif

int testsuite_test(int argc, char** argv)
{
#if !defined(NO_WOLFSSL_SERVER) && !defined(NO_WOLFSSL_CLIENT) && \
    (!defined(WOLF_CRYPTO_CB_ONLY_RSA) && !defined(WOLF_CRYPTO_CB_ONLY_ECC))
    func_args server_args;

    tcp_ready ready;
#if !defined(NETOS)
    THREAD_TYPE serverThread;

    int ret;
#endif

#ifndef USE_WINDOWS_API
    const char *tempDir = NULL;
    char tempName[128];
    int tempName_len;
    int tempName_Xnum;
#else
    char tempName[] = "fnXXXXXX";
    const int tempName_len = 8;
    const int tempName_Xnum = 6;
#endif
#ifdef HAVE_STACK_SIZE
    void *serverThreadStackContext = NULL;
#endif

#ifndef USE_WINDOWS_API
#ifdef XGETENV
    tempDir = XGETENV("TMPDIR");
    if (tempDir == NULL)
#endif
    {
        tempDir = "/tmp";
    }
    XSTRLCPY(tempName, tempDir, sizeof(tempName));
    XSTRLCAT(tempName, "/testsuite-output-XXXXXX", sizeof(tempName));
    tempName_len = (int)XSTRLEN(tempName);
    tempName_Xnum = 6;
#endif /* !USE_WINDOWS_API */

#ifdef HAVE_WNR
    if (wc_InitNetRandom(wnrConfig, NULL, 5000) != 0) {
        err_sys("Whitewood netRandom global config failed");
        return -1237;
    }
#endif /* HAVE_WNR */

    StartTCP();

    server_args.argc = argc;
    server_args.argv = argv;

    wolfSSL_Init();
#if defined(DEBUG_WOLFSSL) && !defined(HAVE_VALGRIND)
    wolfSSL_Debugging_ON();
#endif

#if !defined(WOLFSSL_TIRTOS)
    ChangeToWolfRoot();
#endif

#ifdef WOLFSSL_TIRTOS
    fdOpenSession(Task_self());
#endif

    server_args.signal = &ready;
    InitTcpReady(&ready);

#ifndef NO_CRYPT_TEST
    /* wc_ test */
    #ifdef HAVE_STACK_SIZE
        StackSizeCheck(&server_args, wolfcrypt_test);
    #else
        wolfcrypt_test(&server_args);
    #endif
    if (server_args.return_code != 0) return server_args.return_code;
#endif

    /* Simple wolfSSL client server test */
    #ifdef HAVE_STACK_SIZE
        StackSizeCheck(&server_args, (THREAD_RETURN (*)(void *))simple_test);
    #else
        simple_test(&server_args);
    #endif
    if (server_args.return_code != 0) return server_args.return_code;
#if !defined(NETOS)
    /* Echo input wolfSSL client server test */
    #ifdef HAVE_STACK_SIZE
        StackSizeCheck_launch(&server_args, echoserver_test, &serverThread,
                              &serverThreadStackContext);
    #else
        start_thread(echoserver_test, &server_args, &serverThread);
    #endif

    /* Create unique file name */
    outputName = mymktemp(tempName, tempName_len, tempName_Xnum);
    if (outputName == NULL) {
        printf("Could not create unique file name");
        return EXIT_FAILURE;
    }

    ret = test_tls(&server_args);
    if (ret != 0) {
        cleanup_output();
        return ret;
    }

    /* Server won't quit unless TLS test has worked. */
#ifdef HAVE_STACK_SIZE
    fputs("reaping echoserver_test: ", stdout);
    StackSizeCheck_reap(serverThread, serverThreadStackContext);
#else
    join_thread(serverThread);
#endif
    if (server_args.return_code != 0) {
        cleanup_output();
        return server_args.return_code;
    }

#if !defined(NO_WOLFSSL_SERVER) && !defined(NO_WOLFSSL_CLIENT) && \
    defined(HAVE_CRL) && defined(HAVE_CRL_MONITOR)
    ret = test_crl_monitor();
    if (ret != 0) {
        cleanup_output();
        return ret;
    }
#endif

#endif /* !NETOS */

    show_ciphers();

#if !defined(NETOS)
    ret = validate_cleanup_output();
    if (ret != 0)
        return EXIT_FAILURE;
#endif

    wolfSSL_Cleanup();
    FreeTcpReady(&ready);

#ifdef WOLFSSL_TIRTOS
    fdCloseSession(Task_self());
#endif

#ifdef HAVE_WNR
    if (wc_FreeNetRandom() < 0)
        err_sys("Failed to free netRandom context");
#endif /* HAVE_WNR */

    printf("\nAll tests passed!\n");

#else
    (void)argc;
    (void)argv;
#endif /* !NO_WOLFSSL_SERVER && !NO_WOLFSSL_CLIENT */

    return EXIT_SUCCESS;
}

#if !defined(NO_WOLFSSL_SERVER) && !defined(NO_WOLFSSL_CLIENT) && \
    defined(HAVE_CRL) && defined(HAVE_CRL_MONITOR)
#define CRL_MONITOR_TEST_ROUNDS 6
#define CRL_MONITOR_REM_FILE_ATTEMPTS 20

static int test_crl_monitor(void)
{
    func_args server_args;
    func_args client_args;
    THREAD_TYPE serverThread;
    tcp_ready ready;
    char buf[128];
    char tmpDir[16];
    char rounds[4];
    char portNum[8];
    const char* serverArgv[] = {
        "testsuite",
        "-A", "certs/ca-cert.pem",
        "--crl-dir", tmpDir,
        "-C", rounds,
        "--quieter",
        "-x",
        "-p", "0"
    };
    const char* clientArgv[] = {
        "testsuite",
        "-C",
        "-c", "certs/server-cert.pem",
        "-k", "certs/server-key.pem",
        "--quieter",
        "-H", "exitWithRet",
        "-p", portNum
    };
    int ret = -1;
    int i = -1, j;

    printf("\nRunning CRL monitor test\n");

    sprintf(rounds, "%d", CRL_MONITOR_TEST_ROUNDS);

    XMEMSET(&server_args, 0, sizeof(func_args));
    XMEMSET(&client_args, 0, sizeof(func_args));

    /* Create temp dir */
    if (create_tmp_dir(tmpDir, sizeof(tmpDir) - 1) == NULL) {
        fprintf(stderr, "Failed to create tmp dir");
        goto cleanup;
    }

    server_args.argv = (char**)serverArgv;
    server_args.argc = sizeof(serverArgv) / sizeof(*serverArgv);
    client_args.signal = server_args.signal = &ready;
    client_args.argv = (char**)clientArgv;
    client_args.argc = sizeof(clientArgv) / sizeof(*clientArgv);

    InitTcpReady(&ready);
    start_thread(server_test, &server_args, &serverThread);
    wait_tcp_ready(&server_args);
    sprintf(portNum, "%d", server_args.signal->port);

    for (i = 0; i < CRL_MONITOR_TEST_ROUNDS; i++) {
        int expectFail;
        if (i % 2 == 0) {
            /* succeed on even rounds */
            sprintf(buf, "%s/%s", tmpDir, "crl.pem");
            if (copy_file("certs/crl/crl.pem", buf) != 0) {
                fprintf(stderr, "[%d] Failed to copy file to %s\n", i, buf);
                goto cleanup;
            }
            sprintf(buf, "%s/%s", tmpDir, "crl.revoked");
            /* The monitor can be holding the file handle and this will cause
             * the remove call to fail. Let's give the monitor a some time to
             * finish up. */
            for (j = 0; j < CRL_MONITOR_REM_FILE_ATTEMPTS; j++) {
                /* i == 0 since there is nothing to delete in the first round */
                if (i == 0 || rem_file(buf) == 0)
                    break;
                XSLEEP_MS(100);
            }
            if (j == CRL_MONITOR_REM_FILE_ATTEMPTS) {
                fprintf(stderr, "[%d] Failed to remove file %s\n", i, buf);
                goto cleanup;
            }
            expectFail = 0;
        }
        else {
            /* fail on odd rounds */
            sprintf(buf, "%s/%s", tmpDir, "crl.revoked");
            if (copy_file("certs/crl/crl.revoked", buf) != 0) {
                fprintf(stderr, "[%d] Failed to copy file to %s\n", i, buf);
                goto cleanup;
            }
            sprintf(buf, "%s/%s", tmpDir, "crl.pem");
            /* The monitor can be holding the file handle and this will cause
             * the remove call to fail. Let's give the monitor a some time to
             * finish up. */
            for (j = 0; j < CRL_MONITOR_REM_FILE_ATTEMPTS; j++) {
                if (rem_file(buf) == 0)
                    break;
                XSLEEP_MS(100);
            }
            if (j == CRL_MONITOR_REM_FILE_ATTEMPTS) {
                fprintf(stderr, "[%d] Failed to remove file %s\n", i, buf);
                goto cleanup;
            }
            expectFail = 1;
        }
        /* Give server a moment to register the file change */
        XSLEEP_MS(100);

        client_args.return_code = 0;
        client_test(&client_args);

        if (!expectFail) {
            if (client_args.return_code != 0) {
                fprintf(stderr, "[%d] Incorrect return %d\n", i,
                        client_args.return_code);
                goto cleanup;
            }
        }
        else {
            if (client_args.return_code == 0) {
                fprintf(stderr, "[%d] Expected failure\n", i);
                goto cleanup;
            }
        }
    }

    join_thread(serverThread);
    ret = 0;
cleanup:
    if (ret != 0 && i >= 0)
        fprintf(stderr, "test_crl_monitor failed on iteration %d\n", i);
    sprintf(buf, "%s/%s", tmpDir, "crl.pem");
    rem_file(buf);
    sprintf(buf, "%s/%s", tmpDir, "crl.revoked");
    rem_file(buf);
    (void)rem_dir(tmpDir);
    return ret;
}
#endif

#if !defined(NO_WOLFSSL_SERVER) && !defined(NO_WOLFSSL_CLIENT) && \
   (!defined(WOLF_CRYPTO_CB_ONLY_RSA) && !defined(WOLF_CRYPTO_CB_ONLY_ECC))
/* Perform a basic TLS handshake.
 *
 * First connection to echo a file.
 * Second to tell TLS server to quit.
 *
 * @param [in,out] server_args   Object sent to server thread.
 * @return  0 on success.
 * @return  echoclient error return code on failure.
 */
static int test_tls(func_args* server_args)
{
    func_args echo_args;
    char* myArgv[NUMARGS];
    char arg[3][128];

    printf("\nRunning TLS test\n");

    /* Set up command line arguments for echoclient to send input file
     * and write echoed data to temporary output file. */
    myArgv[0] = arg[0];
    myArgv[1] = arg[1];
    myArgv[2] = arg[2];

    echo_args.argc = 3;
    echo_args.argv = myArgv;

    XSTRLCPY(arg[0], "testsuite", sizeof(arg[0]));
    XSTRLCPY(arg[1], "input", sizeof(arg[1]));
    XSTRLCPY(arg[2], outputName, sizeof(arg[2]));

    /* Share the signal, it has the new port number in it. */
    echo_args.signal = server_args->signal;

    /* Ready to execute client - wait for server to be ready. */
    wait_tcp_ready(server_args);

    /* Do a client TLS connection. */
#ifdef HAVE_STACK_SIZE
    fputs("echoclient_test #1: ", stdout);
    StackSizeCheck(&echo_args, echoclient_test_wrapper);
#else
    echoclient_test(&echo_args);
#endif
    if (echo_args.return_code != 0)
        return echo_args.return_code;

#ifdef WOLFSSL_DTLS
    /* Ensure server is ready for UDP data. */
    wait_tcp_ready(server_args);
#endif

    /* Next client connection - send quit to shutdown server. */
    echo_args.argc = 2;
    XSTRLCPY(arg[1], "quit", sizeof(arg[1]));

    /* Do a client TLS connection. */
#ifdef HAVE_STACK_SIZE
    fputs("echoclient_test #2: ", stdout);
    StackSizeCheck(&echo_args, echoclient_test_wrapper);
#else
    echoclient_test(&echo_args);
#endif
    if (echo_args.return_code != 0)
        return echo_args.return_code;

    return 0;
}

/* Show cipher suites available. */
static void show_ciphers(void)
{
    char ciphers[WOLFSSL_CIPHER_LIST_MAX_SIZE];
    XMEMSET(ciphers, 0, sizeof(ciphers));
    wolfSSL_get_ciphers(ciphers, sizeof(ciphers)-1);
    printf("ciphers = %s\n", ciphers);
}

/* Cleanup temporary output file. */
static void cleanup_output(void)
{
    remove(outputName);
}

/* Validate output equals input using a hash. Remove temporary output file.
 *
 * @return  0 on success.
 * @return  1 on failure.
 */
static int validate_cleanup_output(void)
{
#ifndef NO_SHA256
    byte input[WC_SHA256_DIGEST_SIZE];
    byte output[WC_SHA256_DIGEST_SIZE];

    file_test("input",  input);
    file_test(outputName, output);
#endif
    cleanup_output();
#ifndef NO_SHA256
    if (memcmp(input, output, sizeof(input)) != 0)
        return 1;
#endif
    return 0;
}

/* Simple server.
 *
 * @param [in] args  Object for server data in thread.
 * @return  Return code.
 */
#ifdef HAVE_STACK_SIZE
static THREAD_RETURN simple_test(func_args* args)
#else
static void simple_test(func_args* args)
#endif
{
    THREAD_TYPE serverThread;

    int i;

    func_args svrArgs;
    char *svrArgv[9];
    char argvs[9][32];

    func_args cliArgs;
    char *cliArgv[NUMARGS];
    char argvc[3][32];

    printf("\nRunning simple test\n");

    for (i = 0; i < 9; i++)
        svrArgv[i] = argvs[i];
    for (i = 0; i < 3; i++)
        cliArgv[i] = argvc[i];

    XSTRLCPY(argvs[0], "SimpleServer", sizeof(argvs[0]));
    svrArgs.argc = 1;
    svrArgs.argv = svrArgv;
    svrArgs.return_code = 0;
    #if !defined(USE_WINDOWS_API) && !defined(WOLFSSL_SNIFFER)  && \
                                     !defined(WOLFSSL_TIRTOS)
        XSTRLCPY(argvs[svrArgs.argc++], "-p", sizeof(argvs[svrArgs.argc]));
        XSTRLCPY(argvs[svrArgs.argc++], "0", sizeof(argvs[svrArgs.argc]));
    #endif
    /* Set the last arg later, when it is known. */

    args->return_code = 0;
    svrArgs.signal = args->signal;
    start_thread(server_test, &svrArgs, &serverThread);
    wait_tcp_ready(&svrArgs);

    /* Setting the actual port number. */
    XSTRLCPY(argvc[0], "SimpleClient", sizeof(argvc[0]));
    cliArgs.argv = cliArgv;
    cliArgs.return_code = 0;
#ifndef USE_WINDOWS_API
    cliArgs.argc = NUMARGS;
    XSTRLCPY(argvc[1], "-p", sizeof(argvc[1]));
    (void)snprintf(argvc[2], sizeof(argvc[2]), "%d", (int)svrArgs.signal->port);
#else
    cliArgs.argc = 1;
#endif

    client_test(&cliArgs);
    if (cliArgs.return_code != 0) {
        args->return_code = cliArgs.return_code;
    #ifdef HAVE_STACK_SIZE
        return (THREAD_RETURN)0;
    #else
        return;
    #endif
    }
    join_thread(serverThread);
    if (svrArgs.return_code != 0) args->return_code = svrArgs.return_code;
#ifdef HAVE_STACK_SIZE
    return (THREAD_RETURN)0;
#endif
}
#endif /* !NO_WOLFSSL_SERVER && !NO_WOLFSSL_CLIENT */

#ifndef NO_SHA256
/* Create SHA-256 hash of the file based on filename.
 *
 * @param [in]  file   Name of file.
 * @param [out] check  Buffer to hold SHA-256 hash.
 */
void file_test(const char* file, byte* check)
{
    FILE* f;
    int   i = 0, j, ret;
    wc_Sha256   sha256;
    byte  buf[1024];
    byte  shasum[WC_SHA256_DIGEST_SIZE];

    ret = wc_InitSha256(&sha256);
    if (ret != 0) {
        printf("Can't wc_InitSha256 %d\n", ret);
        return;
    }
    if( !( f = fopen( file, "rb" ) )) {
        printf("Can't open %s\n", file);
        return;
    }
    while( ( i = (int)fread(buf, 1, sizeof(buf), f )) > 0 ) {
        ret = wc_Sha256Update(&sha256, buf, i);
        if (ret != 0) {
            printf("Can't wc_Sha256Update %d\n", ret);
            fclose(f);
            return;
        }
    }

    ret = wc_Sha256Final(&sha256, shasum);
    wc_Sha256Free(&sha256);

    if (ret != 0) {
        printf("Can't wc_Sha256Final %d\n", ret);
        fclose(f);
        return;
    }

    XMEMCPY(check, shasum, sizeof(shasum));

    for(j = 0; j < WC_SHA256_DIGEST_SIZE; ++j )
        printf( "%02x", shasum[j] );

    printf("  %s\n", file);

    fclose(f);
}
#endif

#else /* SINGLE_THREADED */


int myoptind = 0;
char* myoptarg = NULL;


int main(int argc, char** argv)
{
    func_args wolfcrypt_test_args;

    wolfcrypt_test_args.argc = argc;
    wolfcrypt_test_args.argv = argv;

    wolfSSL_Init();
    ChangeToWolfRoot();

    /* No TLS - only doing cryptographic algorithm testing. */
    wolfcrypt_test(&wolfcrypt_test_args);
    if (wolfcrypt_test_args.return_code != 0)
        return wolfcrypt_test_args.return_code;

    wolfSSL_Cleanup();
    printf("\nAll tests passed!\n");
    return EXIT_SUCCESS;
}


#endif /* SINGLE_THREADED */

