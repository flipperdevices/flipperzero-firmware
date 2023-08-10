/* wolfssl_demo.c
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
#include <stdlib.h>




#include <wolfssl/wolfcrypt/settings.h>
#include "wolfssl/ssl.h"
#include <wolfssl/wolfio.h>
#include "wolfssl/certs_test.h"
#include "wolfssl/wolfcrypt/types.h"
#include "wolfssl_demo.h"
#include <wolfcrypt/test/test.h>
#include <wolfcrypt/benchmark/benchmark.h>

#include "FreeRTOS.h"
#if defined(FREERTOS_TCP)
    #include "FreeRTOS_IP.h"
    #include "FreeRTOS_Sockets.h"
    #include "platform/iot_network.h"
    #include "platform.h"
#endif

#if defined(BENCHMARK)
    #include "r_cmt_rx_if.h"
#endif


#if defined(TLS_CLIENT)
#if defined(WOLFSSL_RENESAS_TSIP_TLS)
    #include "key_data.h"
    #include <wolfssl/wolfcrypt/port/Renesas/renesas-tsip-crypt.h>

    extern const st_key_block_data_t g_key_block_data;
    uint32_t                         g_encrypted_root_public_key[140];
   #if defined(TLS_MULTITHREAD_TEST)
    static TsipUserCtx userContext_taskA;
    static TsipUserCtx userContext_taskB;
   #else
    static TsipUserCtx userContext;
   #endif
#endif /* WOLFSSL_RENESAS_TSIP_TLS */

    static WOLFSSL_CTX* client_ctx;
#endif /* TLS_CLIENT */

#define TLSSERVER_IP      "192.168.11.49"
#define TLSSERVER_PORT    11111
#define YEAR 2023
#define MON  3
#define FREQ 10000 /* Hz */
#define MAX_MSGSTR  80

static long         tick;
static int          tmTick;


#if defined(TSIP_CRYPT_UNIT_TEST)
 int tsip_crypt_test();
 int tsip_crypt_sha_multitest();
 int tsip_crypt_AesCbc_multitest();
 int tsip_crypt_AesGcm_multitest();
 int tsip_crypt_Sha_AesCbcGcm_multitest();
#endif

#if defined(TLS_MULTITHREAD_TEST)
xSemaphoreHandle exit_semaph;
static xSemaphoreHandle Mutex;
#endif

static int msg(const char* pname, int l,
                         const char * sFormat, ...)
{
    int ret = 0;
    char buf[MAX_MSGSTR] = {0};

    va_list ParamList;

#if defined(TLS_MULTITHREAD_TEST)
    xSemaphoreTake(Mutex, portMAX_DELAY);
#endif
    va_start(ParamList, sFormat);

    printf("[%s][%02d] ", pname, l);
    ret = vsnprintf(buf, sizeof(buf), sFormat, ParamList);
    printf(buf);

    va_end(ParamList);

#if defined(TLS_MULTITHREAD_TEST)
    xSemaphoreGive(Mutex);
#endif

    return ret;
}


#if defined(TLS_MULTITHREAD_TEST)
static void my_Logging_cb(const int logLevel, const char *const logMessage)
{
    (void)logLevel;
    msg("custom-log", logLevel, "%s\n", logMessage);
}
#endif

/* time
 * returns seconds from EPOCH
 */
time_t time(time_t *t)
{
    (void)t;
    return ((YEAR-1970)*365+30*MON)*24*60*60 + tmTick++;
}

/* timeTick
 * called periodically by H/W timer to increase tmTick.
 */
#if defined(BENCHMARK)
static void timeTick(void* pdata)
{
    (void)pdata;
    tick++;
}
#endif

double current_time(int reset)
{
      if(reset) tick = 0 ;
      return ((double)tick/FREQ) ;
}

/* --------------------------------------------------------*/
/*  Benchmark_demo                                         */
/* --------------------------------------------------------*/
#if defined(BENCHMARK)
static void Benchmark_demo(void)
{
    uint32_t channel;
    R_CMT_CreatePeriodic(FREQ, &timeTick, &channel);

    printf("Start wolfCrypt Benchmark\n");
    benchmark_test(NULL);
    printf("End wolfCrypt Benchmark\n");
}
#endif /* BENCHMARK */
/* --------------------------------------------------------*/
/*  CryptTest_demo                                         */
/* --------------------------------------------------------*/
#if defined(CRYPT_TEST)
static void CryptTest_demo(void)
{
    int ret;

    if ((ret = wolfCrypt_Init()) != 0) {
         printf("wolfCrypt_Init failed %d\n", ret);
    }

    printf("Start wolfCrypt Test\n");
    wolfcrypt_test(NULL);
    printf("End wolfCrypt Test\n");

    if ((ret = wolfCrypt_Cleanup()) != 0) {
        printf("wolfCrypt_Cleanup failed %d\n", ret);
    }
}
#endif /* CRYPT_TEST */
/* --------------------------------------------------------*/
/*  Tls_client_demo                                        */
/* --------------------------------------------------------*/
#if defined(TLS_CLIENT)
static void Tls_client_init()
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


    client_ctx = NULL;

    wolfSSL_Init();

    /* Create and initialize WOLFSSL_CTX */
    if ((client_ctx = 
        wolfSSL_CTX_new(wolfSSLv23_client_method_ex((void *)NULL))) == NULL) {
        printf("ERROR: failed to create WOLFSSL_CTX\n");
        return;
    }

    #ifdef WOLFSSL_RENESAS_TSIP_TLS
    tsip_set_callbacks(client_ctx);
    #endif

    /* load root CA certificate */
    #if defined(NO_FILESYSTEM)
    if (wolfSSL_CTX_load_verify_buffer(client_ctx, cert, 
                            SIZEOF_CERT, SSL_FILETYPE_ASN1) != SSL_SUCCESS) {
           printf("ERROR: can't load certificate data\n");
       return;
    }
    #else
    if (wolfSSL_CTX_load_verify_locations(client_ctx, cert, 0) != SSL_SUCCESS) {
        printf("ERROR: can't load \"%s\"\n", cert);
        return NULL;
    }
    #endif
     
    #if defined(WOLFSSL_TLS13) && defined(WOLFSSL_RENESAS_TSIP_TLS)

    if (wolfSSL_CTX_UseSupportedCurve(client_ctx, WOLFSSL_ECC_SECP256R1)
                                                         != WOLFSSL_SUCCESS) {
        wolfSSL_CTX_free(client_ctx); client_ctx = NULL;
        printf("client can't set use supported curves\n");
        return;
    }
    #endif
}

static void Tls_client(void *pvParam)
{
    #define BUFF_SIZE 256
    #define ADDR_SIZE 16
    int             ret;
    int             err;
#if defined(TLS_MULTITHREAD_TEST)
    BaseType_t xStatus;
#endif
    TestInfo* p = (TestInfo*)pvParam;
    WOLFSSL_CTX*    ctx = (WOLFSSL_CTX *)client_ctx;
    WOLFSSL*        ssl = NULL;
    Socket_t        socket;
    socklen_t       socksize = sizeof(struct freertos_sockaddr);
    struct freertos_sockaddr    PeerAddr;
    char    addrBuff[ADDR_SIZE] = {0};
    const char* pcName = p->name;

    static const char sendBuff[]= "Hello Server\n" ;
    char    rcvBuff[BUFF_SIZE] = {0};

    if (!p) {
        printf("Unexpected error. Thread parameter is null\n");
        return;
    }
    /* create TCP socket */

    socket = FreeRTOS_socket(FREERTOS_AF_INET,
                             FREERTOS_SOCK_STREAM,
                             FREERTOS_IPPROTO_TCP);

    configASSERT(socket != FREERTOS_INVALID_SOCKET);

    FreeRTOS_bind(socket, NULL, socksize);

    /* attempt to connect TLS server */

    PeerAddr.sin_addr = FreeRTOS_inet_addr(TLSSERVER_IP);
    PeerAddr.sin_port = FreeRTOS_htons(p->port);

    ret = FreeRTOS_connect(socket, &PeerAddr, sizeof(PeerAddr));

    if (ret != 0) {
        msg(pcName, p->id, "ERROR FreeRTOS_connect: %d\n",ret);
        ret = -1;
    }

   #if defined(TLS_MULTITHREAD_TEST)
    msg(pcName, p->id, " Ready to connect.\n");
    xStatus = xSemaphoreTake(p->xBinarySemaphore, portMAX_DELAY);
    if (xStatus != pdTRUE) {
        msg(pcName, p->id, " Error : Failed to xSemaphoreTake\n");
        goto out;
    }
   #endif

    /* create WOLFSSL object */
    if (ret == 0) {
        ssl = wolfSSL_new(ctx);
        if (ssl == NULL) {
            msg(pcName, p->id, "ERROR wolfSSL_new: %d\n", 
                                        wolfSSL_get_error(ssl, 0));
            ret = -1;
        }
    }

    if (ret == 0) {
        #ifdef WOLFSSL_RENESAS_TSIP_TLS
            #if !defined(TLS_MULTITHREAD_TEST)
            memset(&userContext, 0, sizeof(TsipUserCtx));
            tsip_set_callback_ctx(ssl, &userContext);
            #else
            if (p->port - TLSSERVER_PORT == 0) {
                memset(&userContext_taskA, 0, sizeof(TsipUserCtx));
                tsip_set_callback_ctx(ssl, (void*)&userContext_taskA);
            }
            else {
                memset(&userContext_taskB, 0, sizeof(TsipUserCtx));
                tsip_set_callback_ctx(ssl, (void*)&userContext_taskB);
            }
            #endif
        #endif
    }

    msg(pcName, p->id, "  Cipher : %s\n", p->cipher);
    /* use specific cipher */
    if (p->cipher != NULL &&
        wolfSSL_set_cipher_list(ssl, p->cipher) != WOLFSSL_SUCCESS) {
        ret = -1;
    }

    if (ret == 0) {
        /* associate socket with ssl object */
        if (wolfSSL_set_fd(ssl, (int)socket) != WOLFSSL_SUCCESS) {
            msg(pcName, p->id, "ERROR wolfSSL_set_fd: %d\n", 
                                                    wolfSSL_get_error(ssl, 0));
            ret = -1;
        }
    }

    /* set client certificate */
    #if defined(USE_ECC_CERT)
    if (ret == 0) {
        err = wolfSSL_use_certificate_buffer(ssl,
                                    cliecc_cert_der_256,
                                    sizeof_cliecc_cert_der_256,
                                    WOLFSSL_FILETYPE_ASN1);
        if(err != SSL_SUCCESS) {
            printf("ERROR: can't load client-certificate\n");
            ret = -1;
        }
    }
    #else
    if (ret == 0) {
        err = wolfSSL_use_certificate_buffer(ssl,
                                    client_cert_der_2048,
                                    sizeof_client_cert_der_2048,
                                    WOLFSSL_FILETYPE_ASN1);
        if (err != SSL_SUCCESS) {
            printf("ERROR: can't load client-certificate\n");
            ret = -1;
        }
    }
    #endif /* USE_ECC_CERT */

    /* set client key(s) */
#if defined(WOLFSSL_RENESAS_TSIP_TLS)

    #if defined(USE_ECC_CERT)
    /* Client authentication using ECDSA certificate can be handled by TSIP.
     * Therefore, the client private key should be TSIP-specific format
     * and be set by tsip_use_PrivateKey_buffer_TLS.
     */
    if (ret == 0){
        ret = tsip_use_PrivateKey_buffer_TLS(ssl,
                (const char*)g_key_block_data.encrypted_user_ecc256_private_key,
                sizeof(g_key_block_data.encrypted_user_ecc256_private_key),
                TSIP_ECCP256);
        if (ret != 0) {
            printf("ERROR tsip_use_PrivateKey_buffer_TLS\n");
        }
    }
# if defined(WOLFSSL_CHECK_SIG_FAULTS)
    if (ret == 0){
        ret = tsip_use_PublicKey_buffer(ssl,
                (const char*)g_key_block_data.encrypted_user_ecc256_public_key,
                sizeof(g_key_block_data.encrypted_user_ecc256_public_key),
                TSIP_ECCP256);
        if (ret != 0) {
            printf("ERROR tsip_use_PublicKey_buffer\n");
        }
    }
#endif /* WOLFSSL_CHECK_SIG_FAULTS */
    
    #else
    /* Client authentication using RSA certificate can be handled by TSIP.
     * Note that the internal verification of the signature process requires
     * not only the client's private key but also its public key, so pass them 
     * using tsip_use_PrivateKey_buffer_TLS and tsip_use_PublicKey_buffer_TLS
     * respectively.
         */
        if (ret == 0) {
            ret = tsip_use_PrivateKey_buffer_TLS(ssl,
               (const char*)g_key_block_data.encrypted_user_rsa2048_private_key,
               sizeof(g_key_block_data.encrypted_user_rsa2048_private_key),
                                                            TSIP_RSA2048);
            if (ret != 0) {
                printf("ERROR tsip_use_PrivateKey_buffer_TLS :%d\n", ret);
            }
        }
        if (ret == 0) {
            ret = tsip_use_PublicKey_buffer(ssl,
                (const char*)g_key_block_data.encrypted_user_rsa2048_public_key,
                sizeof(g_key_block_data.encrypted_user_rsa2048_public_key),
                                                            TSIP_RSA2048);
            if (ret != 0) {
                printf("ERROR tsip_use_PublicKey_buffer: %d\n", ret);
            }
        }

    #endif /* USE_ECC_CERT */
#else
    #if defined(USE_ECC_CERT)
    if (ret == 0) {
        err = wolfSSL_use_PrivateKey_buffer(ssl, 
                                    ecc_clikey_der_256,
                                    sizeof_ecc_clikey_der_256, 
                                    WOLFSSL_FILETYPE_ASN1);
        if (err != SSL_SUCCESS) {
            printf("ERROR wolfSSL_use_PrivateKey_buffer: %d\n",
                                                wolfSSL_get_error(ssl, 0));
            ret = -1;
        }
    }
    #else
    if (ret == 0) { 
        err = wolfSSL_use_PrivateKey_buffer(ssl, client_key_der_2048,
                            sizeof_client_key_der_2048, WOLFSSL_FILETYPE_ASN1);
         
        if (err != SSL_SUCCESS) {
            printf("ERROR wolfSSL_use_PrivateKey_buffer: %d\n",
                                                wolfSSL_get_error(ssl, 0));
            ret = -1;
        }
    }
    #endif /* USE_ECC_CERT */
#endif /* WOLFSSL_RENESAS_TSIP_TLS */

#ifdef DEBUG_WOLFSSL
    wolfSSL_Debugging_ON();
#endif
    if (ret == 0) {
        if (wolfSSL_connect(ssl) != WOLFSSL_SUCCESS) {
            msg(pcName, p->id, "ERROR wolfSSL_connect: %d\n", 
                                                    wolfSSL_get_error(ssl, 0));
            ret = -1;
        }
    }
#ifdef DEBUG_WOLFSSL
   wolfSSL_Debugging_OFF();
#endif
    if (ret == 0) {
        if (wolfSSL_write(ssl, sendBuff, strlen(sendBuff)) != 
                                                            strlen(sendBuff)) {
            msg(pcName, p->id, "ERROR wolfSSL_write: %d\n", 
                                                    wolfSSL_get_error(ssl, 0));
            ret = -1;
        }
    }

    if (ret == 0) {
        if ((ret=wolfSSL_read(ssl, rcvBuff, BUFF_SIZE -1)) < 0) {
            msg(pcName, p->id, "ERROR wolfSSL_read: %d\n", 
                                                    wolfSSL_get_error(ssl, 0));
            ret = -1;
        }
        else {
            rcvBuff[ret] = '\0';
            msg(pcName, p->id, "Received: %s\n\n", rcvBuff);
            ret = 0;
        }
    }
    
#if defined(TLS_MULTITHREAD_TEST)
out:
#endif
    if (ssl) {
        wolfSSL_shutdown(ssl);
        wolfSSL_free(ssl);
        ssl = NULL;
        /* reset call backs */
    #ifdef WOLFSSL_RENESAS_TSIP_TLS
        tsip_set_callbacks(client_ctx);
    #endif
    }

    if (socket) {
        FreeRTOS_shutdown(socket, FREERTOS_SHUT_RDWR);
        while (FreeRTOS_recv(socket, rcvBuff, BUFF_SIZE -1, 0) >=0) {
            vTaskDelay(250);
        }
        FreeRTOS_closesocket(socket);
        socket = NULL;
    }

#ifdef TLS_MULTITHREAD_TEST
    xSemaphoreGive(exit_semaph);
    vTaskDelete(NULL);
#endif
    return;
}

static void Tls_client_demo(void)
{

    /* setup ciphersuite list to use for TLS handshake */

#if defined(WOLFSSL_RENESAS_TSIP_TLS)

  #ifdef USE_ECC_CERT
    const char* cipherlist[] = {
    #if defined(WOLFSSL_TLS13)
        "TLS13-AES128-GCM-SHA256",
        "TLS13-AES128-CCM-SHA256",
    #endif
        "ECDHE-ECDSA-AES128-SHA256",
        "ECDHE-ECDSA-AES128-GCM-SHA256",
    };
    #if defined(WOLFSSL_TLS13)
        #define cipherlist_sz 2
    #else
        #define cipherlist_sz 2
    #endif
    TestInfo info[cipherlist_sz];
  #else
        const char* cipherlist[] = {
    #if defined(WOLFSSL_TLS13)
        "TLS13-AES128-GCM-SHA256",
        "TLS13-AES128-CCM-SHA256",
    #endif
        "ECDHE-RSA-AES128-GCM-SHA256",
        "ECDHE-RSA-AES128-SHA256",
        "AES128-SHA",
        "AES128-SHA256",
        "AES256-SHA",
        "AES256-SHA256"
        };
        #if defined(WOLFSSL_TLS13)
            #define cipherlist_sz 2
        #else
            #define cipherlist_sz 6
        #endif
        TestInfo info[cipherlist_sz];
    #endif
#else
    const char* cipherlist[] = { NULL };
    #define cipherlist_sz 1
    TestInfo info[cipherlist_sz];
#endif

    int i = 0;
#ifdef TLS_MULTITHREAD_TEST
    int j = 0;
    BaseType_t xReturned;
    BaseType_t xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;

    Mutex = xSemaphoreCreateMutex();
#endif

    printf("/*------------------------------------------------*/\n");
    printf("    TLS_Client demo\n");
    printf("    - TLS server address:" TLSSERVER_IP " port: %d\n",
                                                             TLSSERVER_PORT);

#if defined(WOLFSSL_RENESAS_TSIP_TLS) && (WOLFSSL_RENESAS_TSIP_VER >=109)
    printf("    - with TSIP\n");
#endif
    printf("/*------------------------------------------------*/\n");

    /* setup credentials for TLS handshake */
#if defined(WOLFSSL_RENESAS_TSIP_TLS) && (WOLFSSL_RENESAS_TSIP_VER >=109)

    #if defined(USE_ECC_CERT)

    /* Root CA cert has ECC-P256 public key */
    tsip_inform_cert_sign((const byte*)ca_ecc_cert_der_sig);

    #else
    
    /* Root CA cert has RSA public key */
    tsip_inform_cert_sign((const byte*)ca_cert_der_sig);

    #endif

    wc_tsip_inform_user_keys_ex(
            (byte*)&g_key_block_data.encrypted_provisioning_key,
            (byte*)&g_key_block_data.iv,
            (byte*)&g_key_block_data.encrypted_user_rsa2048_ne_key,
            encrypted_user_key_type);

#endif /* WOLFSSL_RENESAS_TSIP_TLS && (WOLFSSL_RENESAS_TSIP_VER >=109) */

    Tls_client_init();

#ifdef TLS_MULTITHREAD_TEST

    exit_semaph = xSemaphoreCreateCounting(cipherlist_sz, 0);

#ifdef DEBUG_WOLFSSL
    wolfSSL_SetLoggingCb(my_Logging_cb);
#endif

    do {
        for (j = i; j < (i+2); j++) {
            info[j].id = j;
            info[j].port = TLSSERVER_PORT + (j%2);
            info[j].cipher = cipherlist[j];
            info[j].ctx = client_ctx;
            info[j].xBinarySemaphore = xSemaphoreCreateBinary();
            info[j].log_f = my_Logging_cb;

            memset(info[j].name, 0, sizeof(info[j].name));
            sprintf(info[j].name, "clt_thd_%s", ((j%2) == 0) ?
                                                            "taskA" : "taskB");

            printf(" %s connecting to %d port\n", info[j].name, info[j].port);

            xReturned = xTaskCreate(Tls_client, info[j].name, 
                                        THREAD_STACK_SIZE, &info[j], 3, NULL);
            if (xReturned != pdPASS) {
                printf("Failed to create task\n");
            }
        }

        for (j = i; j < (i+2); j++) {
            xSemaphoreGiveFromISR(info[j].xBinarySemaphore, 
                                                    &xHigherPriorityTaskWoken);
        }

        /* check if all tasks are completed */
        for (j = i; j < (i+2); j++) {
            if(!xSemaphoreTake(exit_semaph, portMAX_DELAY)) {
                printf("a semaphore was not given by a test task.");
            }
        }

        i += 2;

    } while (i < cipherlist_sz);

    vSemaphoreDelete(exit_semaph);
    vSemaphoreDelete(Mutex);

#else

    do {

        info[i].port = TLSSERVER_PORT;
        info[i].cipher = cipherlist[i];
        info[i].ctx = client_ctx;
        info[i].id = i;

        memset(info[i].name, 0, sizeof(info[i].name));
        sprintf(info[i].name, "wolfSSL_TLS_client_do(%02d)", i);

        Tls_client(&info[i]);

        i++;
    } while (i < cipherlist_sz);

    if (client_ctx) {
         wolfSSL_CTX_free(client_ctx);
    }

#endif

    wolfSSL_Cleanup();

    printf("End of TLS_Client demo.\n");
}
#endif /* TLS_CLIENT */

/* Demo entry function called by iot_demo_runner
 * To run this entry function as an aws_iot_demo, define this as 
 * DEMO_entryFUNCTION in aws_demo_config.h.
 */
void wolfSSL_demo_task(bool         awsIotMqttMode,
                       const char*  pIdentifier,
                       void*        pNetworkServerInfo,
                       void*        pNetworkCredentialInfo,
                       const IotNetworkInterface_t* pNetworkInterface)
{

    (void)awsIotMqttMode;
    (void)pIdentifier;
    (void)pNetworkServerInfo;
    (void)pNetworkCredentialInfo;
    (void)pNetworkInterface;


#if defined(CRYPT_TEST)

    CryptTest_demo();

#elif defined(BENCHMARK)

    Benchmark_demo();

#elif defined(TSIP_CRYPT_UNIT_TEST)
    int ret = 0;

    if ((ret = wolfCrypt_Init()) != 0) {
        printf("wolfCrypt_Init failed %d\n", ret);
    }

    printf("Start wolf tsip crypt Test\n");

    printf(" \n");
    printf(" simple crypt test by using TSIP\n");
    tsip_crypt_test();

    printf(" \n");
    printf(" multi sha thread test\n");
    tsip_crypt_sha_multitest();

    printf(" \n");
    printf(" multi aes cbc thread test\n");

    tsip_crypt_AesCbc_multitest();

    printf(" \n");
    printf(" multi aes gcm thread test\n");

    tsip_crypt_AesGcm_multitest();

    printf(" \n");
    printf(" multi sha aescbc aesgcm thread test\n");
    tsip_crypt_Sha_AesCbcGcm_multitest();

    printf(" \n");
    printf("End wolf tsip crypt Test\n");

    if ((ret = wolfCrypt_Cleanup()) != 0) {
        printf("wolfCrypt_Cleanup failed %d\n", ret);
    }

#elif defined(TLS_CLIENT)

    Tls_client_demo();

#endif

    while (1) {
        vTaskDelay(10000);
    }
}

