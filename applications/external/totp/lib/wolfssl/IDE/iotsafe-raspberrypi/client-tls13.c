/* client-tls13.c
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

/* C Standard Library */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

/* POSIX Library */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

/* wolfSSL Library */
#include <wolfssl/options.h>
#include <wolfssl/ssl.h>
#include <wolfssl/wolfio.h>
#include <wolfssl/wolfcrypt/port/iotsafe/iotsafe.h>
#include <wolfssl/error-ssl.h>

/* IoTSAFE Certificate slots */

/* File Slot '02' is pre-provisioned with
 * the client ECC public key certificate.
 */
#define CRT_CLIENT_FILE_ID 0x02

/* File Slot '03' is pre-provisioned with the
 * server ECC public key certificate.
 */
#define CRT_SERVER_FILE_ID 0x03

/* IoTSAFE Key slots */

/* Key slot '01' is pre-provisioned with
 * the client ECC private key.
 */
#define PRIVKEY_ID      0x01

/* Key slot '04' is used to generate the ECDH
 * key pair that will be used during the TLS
 * session.
 */
#define ECDH_KEYPAIR_ID 0x04

/* Key slot '05' is used to store the ECDH public
 * key received from the peer during the TLS
 * session.
 */
#define PEER_PUBKEY_ID  0x05

/* Key slot '05' is used to store the public key
 * used for ECC verification - disabled in IoTSAFE.
 */
#define PEER_CERT_ID    0x05


/* Function Declarations */
extern int client_loop(const char *peer_ip, const char *peer_name,
 const char *peer_port, const char *temperature);
#if defined(USE_SECRET_CALLBACK)
static int Tls13SecretCallback(WOLFSSL* ssl, int id, const unsigned char* secret,
 int secretSz, void* ctx);
#endif


/* Function Definitions */
#if defined(USE_SECRET_CALLBACK)

#ifndef WOLFSSL_SSLKEYLOGFILE_OUTPUT
    #define WOLFSSL_SSLKEYLOGFILE_OUTPUT "sslkeylog.log"
#endif

/* Callback function for TLS v1.3 secrets for use with Wireshark */
static int Tls13SecretCallback(WOLFSSL* ssl, int id, const unsigned char* secret,
    int secretSz, void* ctx)
{
    int i;
    const char* str = NULL;
    unsigned char clientRandom[32];
    size_t clientRandomSz;
    XFILE fp = stderr;
    if (ctx) {
        fp = XFOPEN((const char*)ctx, "ab");
        if (fp == XBADFILE) {
            return BAD_FUNC_ARG;
        }
    }

    clientRandomSz = wolfSSL_get_client_random(ssl, clientRandom,
        sizeof(clientRandom));

    switch (id) {
        case CLIENT_EARLY_TRAFFIC_SECRET:
            str = "CLIENT_EARLY_TRAFFIC_SECRET"; break;
        case EARLY_EXPORTER_SECRET:
            str = "EARLY_EXPORTER_SECRET"; break;
        case CLIENT_HANDSHAKE_TRAFFIC_SECRET:
            str = "CLIENT_HANDSHAKE_TRAFFIC_SECRET"; break;
        case SERVER_HANDSHAKE_TRAFFIC_SECRET:
            str = "SERVER_HANDSHAKE_TRAFFIC_SECRET"; break;
        case CLIENT_TRAFFIC_SECRET:
            str = "CLIENT_TRAFFIC_SECRET_0"; break;
        case SERVER_TRAFFIC_SECRET:
            str = "SERVER_TRAFFIC_SECRET_0"; break;
        case EXPORTER_SECRET:
            str = "EXPORTER_SECRET"; break;
    }

    fprintf(fp, "%s ", str);
    for (i = 0; i < (int)clientRandomSz; i++) {
        fprintf(fp, "%02x", clientRandom[i]);
    }
    fprintf(fp, " ");
    for (i = 0; i < secretSz; i++) {
        fprintf(fp, "%02x", secret[i]);
    }
    fprintf(fp, "\n");

    if (fp != stderr) {
        XFCLOSE(fp);
    }

    return 0;
}
#endif /* USE_SECRET_CALLBACK */

int client_loop(const char *peer_ip, const char *peer_name,
    const char *peer_port, const char *temperature)
{
    int ret = 0;

    /* Declare TCP objects */
    int                sockfd   = -1;
    struct sockaddr_in servAddr = {0};

    /* Declare TCP messaging buffer */
    char         buff[2048] = {0};
    size_t       len        = 0;

    /* Declare certificate temporary buffer */
    uint8_t      cert_buffer[2048] = {0};
    uint32_t     cert_buffer_size  = 0;
    uint8_t     *cert_iter         = NULL;

    /* Declare wolfSSL objects */
    WOLFSSL_CTX* ctx = NULL;
    WOLFSSL*     ssl = NULL;
    WC_RNG       rng = {0};
    char         randombytes[16] = {0};

    /* Construct HTTP POST */

    /* Header */
    strcat(buff, "POST /iot/device HTTP/1.1\r\n");
    strcat(buff, "Content-Type: application/json\r\n");
    strcat(buff, "Content-Length: 1000\r\n");
    strcat(buff, "Accept: */*\r\n");
    strcat(buff, "Host: ");
    strcat(buff, peer_name);
    strcat(buff, ":");
    strcat(buff, peer_port);
    strcat(buff, "\r\n");

    /* Delimiter */
    strcat(buff, "\r\n");

    /* Body */
    srand(time(NULL));
    int devid    = rand() % 100;
    char snum[5] = {0};
    snprintf(snum, sizeof(snum), "%d", devid);

    strcat(buff, "{");
    strcat(buff, "\"deviceId\": \"");
    strcat(buff, snum);
    strcat(buff, "\",");
    strcat(buff, "\"sensorType\": \"Temperature\",");
    strcat(buff, "\"sensorValue\": \"");
    strcat(buff, temperature);
    strcat(buff, "\",");
    strcat(buff, "\"sensorUnit\": \"Celsius\",");
    strcat(buff, "\"sensorTime\": 1582181510");
    strcat(buff, "}");
    strcat(buff, "\r\n");

    printf("\n\nPOST REQUEST\n\n%s\n\n", buff);

    /*---------------------------------*/
    /* Start of Socket                 */
    /*---------------------------------*/
    printf("---- Preparing TCP socket\n");

    /* Create a socket that uses an internet IPv4 address,
     * Sets the socket to be stream based (TCP),
     * 0 means choose the default protocol. */
    printf("----     Creating socket\n");
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "ERROR: failed to create the socket\n");
        ret = -1;
        goto exit;
    }

    /* Fill in the server address */
    printf("Peer port: %s\n", peer_port);
    servAddr.sin_family = AF_INET; /* Using IPv4 */
    servAddr.sin_port   = htons(atoi(peer_port));

    /* Get the server IPv4 address from the command line call */
    printf("----     Checking peer IP address\n");
    printf("Peer IP address: %s <IPv4 address>\n", peer_ip);
    if (inet_pton(AF_INET, peer_ip, &servAddr.sin_addr) != 1)
    {
        fprintf(stderr, "ERROR: invalid peer IP address\n");
        ret = -1;
        goto exit;
    }

    /* Connect to the server */
    printf("----     Connecting to the peer socket\n");
    if ((ret = connect(sockfd, (struct sockaddr*) &servAddr, sizeof(servAddr)))
         == -1)
    {
        fprintf(stderr, "ERROR: failed to connect\n");
        goto exit;
    }

    printf("---- TCP socket connection established\n");

    /*---------------------------------*/
    /* Start of Security               */
    /*---------------------------------*/
    printf("---- Preparing wolfSSL TLS 1.3\n");

    /* Initialize wolfSSL */
    printf("----     Initializing wolfSSL\n");
    if ((ret = wolfSSL_Init()) != WOLFSSL_SUCCESS)
    {
        fprintf(stderr, "ERROR: Failed to initialize the library\n");
        goto exit;
    }

    /* Allow debug print in wolfSSL */
    printf("----     Allowing debug print in wolfSSL\n");
    if ((ret = wolfSSL_Debugging_ON()) != 0)
    {
        fprintf(stderr, "WARNING: Failed to enable wolfSSL debug print\n");
    }

    /* Initialize RNG */
    printf("----     Initializing RNG\n");
    if ((ret = wc_InitRng(&rng)) != 0)
    {
        fprintf(stderr, "ERROR: Failed to initialize the random number generator\n");
        goto exit;
    }

    /* Obtain a sample RND */
    printf("----     Testing getting RND\n");
    if ((ret = wc_RNG_GenerateBlock(
                        &rng,
                        (byte*)randombytes,
                        sizeof(randombytes)))
         != 0)
    {
        fprintf(stderr, "ERROR: Failed to get random numbers\n");
        goto exit;
    }
    else
    {
        printf("Random bytes: ");
        for (uint8_t i = 0; i < sizeof(randombytes); i++)
            printf("%02X", (unsigned int)randombytes[i]);
        printf("\n");
    }

    /* Create and initialize WOLFSSL_CTX */
    printf("----     Creating wolfSSL TLS 1.3 context object\n");
    if ((ctx = wolfSSL_CTX_new(wolfTLSv1_3_client_method())) == NULL)
    {
        fprintf(stderr, "ERROR: failed to create WOLFSSL_CTX\n");
        ret = -1;
        goto exit;
    }

    /* Initialize IoTSAFE */
    printf("----     Enabling IoTSAFE\n");
    if ((ret = wolfSSL_CTX_iotsafe_enable(ctx)) != 0)
    {
        fprintf(stderr, "ERROR: Failed to initialize IoTSAFE\n");
        goto exit;
    }

    /* Extract client certificate from IoTSAFE*/
    printf("----     Extracting client certificate from IoTSAFE\n");
    if ((cert_buffer_size = wolfIoTSafe_GetCert(
                                CRT_CLIENT_FILE_ID,
                                cert_buffer,
                                sizeof(cert_buffer)))
         < 1)
    {
        fprintf(stderr, "ERROR: Bad client certificate\n");
        ret = -1;
        goto exit;
    }

    /* Print extracted client certificate */
    printf("----     Printing extracted client certificate\n");
    cert_iter = (uint8_t*)&cert_buffer;
    printf("Extracted client certificate in HEX:\n");
    for (uint32_t i = 0; i < cert_buffer_size; i++)
        printf("%02X", (unsigned int)*(cert_iter++));
    printf("\n");

    /* Load client certificate */
    printf("----     Loading client certificate\n");
    if ((ret = wolfSSL_CTX_use_certificate_buffer(ctx, cert_buffer,
                cert_buffer_size, WOLFSSL_FILETYPE_ASN1))
         != WOLFSSL_SUCCESS)
    {
        fprintf(stderr, "ERROR: Failed to load client certificate\n");
        return -1;
    }

    /* Extract server certificate from IoTSAFE*/
    printf("----     Extracting server certificate from IoTSAFE\n");
    if ((cert_buffer_size = wolfIoTSafe_GetCert(
                                CRT_SERVER_FILE_ID,
                                cert_buffer,
                                sizeof(cert_buffer)))
         < 1)
    {
        fprintf(stderr, "ERROR: Bad server certificate\n");
        ret = -1;
        goto exit;
    }

    /* Print extracted server certificate */
    printf("----     Printing extracted server certificate\n");
    cert_iter = (uint8_t*)&cert_buffer;
    printf("Extracted server certificate in HEX:\n");
    for (uint32_t i = 0; i < cert_buffer_size; i++)
        printf("%02X", (unsigned int)*(cert_iter++));
    printf("\n");

    /* Load server certificate */
    printf("----     Loading server certificate\n");
    if ((ret = wolfSSL_CTX_trust_peer_buffer(ctx, cert_buffer,
                cert_buffer_size, WOLFSSL_FILETYPE_ASN1))
         != WOLFSSL_SUCCESS)
    {
        fprintf(stderr, "ERROR: Failed to load server certificate\n");
        return -1;
    }

    /* Set client to authenticate server */
    printf("----     Enabling client to authenticate server\n");
    wolfSSL_CTX_set_verify(ctx, WOLFSSL_VERIFY_PEER, NULL);

    /* Create a WOLFSSL object */
    printf("----     Creating wolfSSL TLS 1.3 connection object\n");
    if ((ssl = wolfSSL_new(ctx)) == NULL)
    {
        fprintf(stderr, "ERROR: failed to create WOLFSSL object\n");
        ret = -1;
        goto exit;
    }

    /* Attach IoTSAFE usage to the wolfSSL */
    printf("----     Using IoTSAFE for TLS 1.3\n");
    if ((ret = wolfSSL_iotsafe_on(
                ssl, PRIVKEY_ID, ECDH_KEYPAIR_ID,
                PEER_PUBKEY_ID, PEER_CERT_ID))
         != 0)
    {
        fprintf(stderr, "ERROR: Failed to use IoTSAFE\n");
        goto exit;
    }

    /* Attach wolfSSL to the socket */
    printf("----     Attaching TLS 1.3 to the socket\n");
    if ((ret = wolfSSL_set_fd(ssl, sockfd)) != WOLFSSL_SUCCESS)
    {
        fprintf(stderr, "ERROR: Failed to set the file descriptor\n");
        goto exit;
    }

#ifdef USE_SECRET_CALLBACK
    /* Logging usage for wireshark */
    printf("----     Attaching TLS 1.3 secret callback for wireshark\n");
    if ((ret = wolfSSL_set_tls13_secret_cb(
                    ssl,
                    Tls13SecretCallback,
                    (void*)WOLFSSL_SSLKEYLOGFILE_OUTPUT))
         != WOLFSSL_SUCCESS)
    {
        fprintf(stderr, "ERROR: Failed to set the secret callback\n");
        goto exit;
    }
#endif

    /* Connect to wolfSSL on the server side */
    printf("----     Start TLS 1.3 handshaking\n");
    if ((ret = wolfSSL_connect(ssl)) != WOLFSSL_SUCCESS)
    {
        fprintf(stderr, "ERROR: failed to connect to wolfSSL\n");
        goto exit;
    }

    printf("---- wolfSSL TLS 1.3 connection established\n");

    /*---------------------------------*/
    /* Start of Communication          */
    /*---------------------------------*/
    printf("---- Communication starts\n");

    /* Get a message for the server from stdin */
    printf("Enter message for server:\n");
    printf("[OUT]:\n");
    printf("\n\n%s\n\n", buff);

    len = strlen(buff);

    /* Send the message to the server */
    printf("Sending message to the server\n");
    if ((size_t) (ret = wolfSSL_write(ssl, buff, len)) != len)
    {
        fprintf(stderr, "ERROR: failed to write entire message\n");
        fprintf(stderr, "%d bytes of %d bytes were sent\n", ret, (int) len);
        goto exit;
    }

    /* Read the server data into our buff array */
    printf("Receiving message from the server\n");
    memset(buff, 0, sizeof(buff));
    if ((ret = wolfSSL_read(ssl, buff, sizeof(buff) - 1)) < 0)
    {
        fprintf(stderr, "ERROR: failed to read\n");
        goto exit;
    }

    /* Print to stdout any data the server sends */
    printf("[IN] :\n%s\n", buff);

    /* Return reporting a success */
    ret = 0;

    printf("---- Communication ends\n");

exit:
    /* Cleanup and return */
    if (sockfd != -1)
        close(sockfd);          /* Close the connection to the server       */
    if (ssl)
        wolfSSL_free(ssl);      /* Free the wolfSSL object                  */
    if (ctx)
        wolfSSL_CTX_free(ctx);  /* Free the wolfSSL context object          */
    wc_FreeRng(&rng);           /* Cleanup the RNG                          */
    wolfSSL_Cleanup();          /* Cleanup the wolfSSL environment          */

    return ret;
}
