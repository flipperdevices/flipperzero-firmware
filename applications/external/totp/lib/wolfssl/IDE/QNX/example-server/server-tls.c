/* server-tls.c
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* socket includes */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

/* wolfSSL */
#include <wolfssl/options.h>
#include <wolfssl/ssl.h>
#include <wolfssl/wolfcrypt/ecc.h>
#include <wolfssl/wolfcrypt/port/caam/wolfcaam.h> /* functions for blob/cover*/

#define DEFAULT_PORT 11111
#define ECC_KEY_SIZE 32

#undef  USE_CERT_BUFFERS_256
#define USE_CERT_BUFFERS_256
#include <wolfssl/certs_test.h>

/* creates a blob then opens it, returns 0 on success */
static int test_blob(byte* key, int keySz)
{
    int ret, i;
    byte out[keySz + WC_CAAM_BLOB_SZ];
    int  outSz;
    int  keyOutSz;

    byte keymod[WC_CAAM_BLACK_KEYMOD_SZ];
    int  keymodSz = WC_CAAM_BLACK_KEYMOD_SZ;

    /* using a key mod of all 1's */
    XMEMSET(keymod, 1, keymodSz);

    /* example of creating black blog with key for later */
    outSz = keySz + WC_CAAM_BLOB_SZ;
    ret = wc_caamCreateBlob_ex(key, keySz, out, (word32*)&outSz,
            WC_CAAM_BLOB_BLACK, keymod, keymodSz);
    if (ret != 0) {
        printf("Error creating black blob\n");
        return -1;
    }

    printf("created black blob :");
    for (i = 0; i < outSz; i++)
        printf("%02X,", out[i]);
    printf("\n");


    /* open black blob and overwrite key, this is to show that the opened blob
     * can be used for the upcoming TLS connection */
    memset(key, 0, keySz);
    keyOutSz = keySz;
    ret = wc_caamOpenBlob_ex(out, outSz, key, (word32*)&keyOutSz,
            WC_CAAM_BLOB_BLACK, keymod, keymodSz);
    if (ret != 0) {
        printf("Error opening black blob\n");
    }

    return ret;
}


/* example of covering an existing key and then making a black blob with it
 * note that generating an ECC completely in secure memory already covered can
 * be done by call wc_ecc_make_key */
int cover(ecc_key* keyOut, const byte* der, word32 derSz)
{
    int ret;
    ecc_key notSecure;
    word32  idx = 0;

    /* format bit plus public key x and y parameter */
    byte x963[(ECC_KEY_SIZE*2) + 1];
    word32 x963Sz = (ECC_KEY_SIZE*2) + 1;

    /* uncovered private key */
    byte d[ECC_KEY_SIZE];
    word32 dSz = ECC_KEY_SIZE;

    byte blackKey[ECC_KEY_SIZE + WC_CAAM_MAC_SZ];
    word32 blackKeySz = ECC_KEY_SIZE + WC_CAAM_MAC_SZ;


    /* The DER buffer for test case does not contain a black key, here we will
     * extract the key information, cover it (make it a black key), and then
     * import that into 'keyOut' which will create a secure partition on import.
     *
     * These steps of covering and importing can be avoided with a call to
     * wc_ecc_make_key which creates a new black key that never leaves the CAAM.
     * The cover process is only being used to make use of an existing key that
     * matches the test certificate.
     */
    wc_ecc_init(&notSecure);
    if (wc_EccPrivateKeyDecode(der, &idx, &notSecure, derSz) != 0) {
        printf("Error extracting ECC der buffer\n");
        ret = -1;
        goto done;
    }

    /* Get the uncovered key values */
    if (wc_ecc_export_private_raw(&notSecure, NULL, NULL, NULL, NULL, d, &dSz)
            != 0) {
        printf("Error getting ecc key values\n");
        ret = -1;
        goto done;
    }

    if (wc_ecc_export_x963(&notSecure, x963, &x963Sz) != 0) {
        printf("Error getting ecc public key\n");
        ret = -1;
        goto done;
    }

    /* Cover up the private key (make it a black key, encrypting with AES-CCM)*/
    XMEMSET(blackKey, 0, blackKeySz);
    if (wc_caamCoverKey(d, dSz, blackKey, &blackKeySz, 0) != 0) {
        printf("Error covering up the private key\n");
        ret = -1;
        goto done;
    }

    /* Sanity check to compare covered key versus plain text */
    if (XMEMCMP(d, blackKey, dSz) == 0) {
        printf("Something went wrong with key cover!!\n");
        ret = -1;
        goto done;
    }

    if (test_blob(blackKey, blackKeySz - WC_CAAM_MAC_SZ) != 0) {
        printf("test blob failed\n");
        ret = -1;
        goto done;
    }

    /* zero out the plain text buffer and free key */
    XMEMSET(d, 0, dSz);

    /* Import the black key into a ecc_key structure (this step tries to create
     * a partition of secure memory marked CPS)
     */
    ret = wc_ecc_import_private_key(blackKey, blackKeySz, x963, x963Sz, keyOut);
    if (ret != 0) {
        printf("Error importing black key into secure memory %d\n", ret);
        wc_ecc_free(keyOut);
        goto done;
    }
    printf("blackKeySz = %d, virtual secure address ecc_key.blackKey = 0x%08X\n",
            blackKeySz, keyOut->blackKey);

    ret = 0;
done:

    wc_ecc_free(&notSecure);
    return ret;
}


/* callback function for creating ECC signatures */
static int TLS_ECC_Sign_callback(WOLFSSL* ssl, const unsigned char* in,
        unsigned int inSz, unsigned char* out, word32* outSz,
        const unsigned char* keyDer, unsigned int keySz,
        void* ctx)
{
    ecc_key* blackKey;

    printf("Using ECC sign callback\n");

    if (ctx == NULL) {
        printf("Was expecting a black key passed along with WOLFSSL\n");
        return -1;
    }
    blackKey = (ecc_key*)ctx;

    return wc_ecc_sign_hash(in, inSz, out, outSz, NULL, blackKey);
}


/* callback function to do ECDH operation in TLS handshake */
static int TLS_ECDH_callback(WOLFSSL* ssl, struct ecc_key* otherKey,
        unsigned char* pubKeyDer, word32* pubKeySz,
        unsigned char* out, word32* outlen,
        int side, void* ctx)
{
    int ret= -1;

    printf("Using ECDH callback\n");

    if (ctx == NULL) {
        printf("Was expecting a black key passed along with WOLFSSH\n");
        return -1;
    }

    /* this is being called from client end */
    if (side == WOLFSSL_CLIENT_END) {
    }

    /* this is being called from server end */
    if (side == WOLFSSL_SERVER_END) {
        ecc_key pub;

        wc_ecc_init(&pub);
        if (wc_ecc_import_x963(pubKeyDer, *pubKeySz, &pub) != 0) {
            printf("Issue decoding the public key to generate shared secret\n");
            wc_ecc_free(&pub);
            return  -1;
        }

        /* otherKey is the ephemeral private key created with ECDHE cipher
         * suites. As long as EccKeyGenCb has not been set this key was created
         * with wc_ecc_make_key_ex, which when used with NIST P256 it will
         * automatically try to create a secure black key
         */
        ret = wc_ecc_shared_secret(otherKey, &pub, out, outlen);
        wc_ecc_free(&pub);
    }

    return ret;
}


int main()
{
    int                sockfd;
    int                connd = 0;
    struct sockaddr_in servAddr;
    struct sockaddr_in clientAddr;
    socklen_t          size = sizeof(clientAddr);
    char               buff[256];
    size_t             len;
    int                ret;
    const char*        reply = "I hear ya fa shizzle!\n";

    ecc_key blackKey;

    /* declare wolfSSL objects */
    WOLFSSL_CTX* ctx = NULL;
    WOLFSSL*     ssl = NULL;

    /* Initialize wolfSSL */
    if (wolfSSL_Init() != WOLFSSL_SUCCESS) {
        fprintf(stderr, "issue with wolfSSL_Init()\n");
        return -1;
    }
    wolfSSL_Debugging_ON();
    wc_ecc_init(&blackKey);

    /* Create a socket that uses an internet IPv4 address,
     * Sets the socket to be stream based (TCP),
     * 0 means choose the default protocol. */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "ERROR: failed to create the socket\n");
        goto end;
    }

    /* Create and initialize WOLFSSL_CTX */
    if ((ctx = wolfSSL_CTX_new(wolfTLSv1_2_server_method())) == NULL) {
        fprintf(stderr, "ERROR: failed to create WOLFSSL_CTX\n");
        goto end;
    }

    /* Load server certificates into WOLFSSL_CTX */
    if (wolfSSL_CTX_use_certificate_buffer(ctx, serv_ecc_der_256,
                sizeof_serv_ecc_der_256, SSL_FILETYPE_ASN1) != SSL_SUCCESS) {
        printf("error loading in certificate buffer\n");
        goto end;
    }

//    wolfSSL_CTX_use_PrivateKey_buffer(ctx, ecc_key_der_256, sizeof_ecc_key_der_256, SSL_FILETYPE_ASN1);

    if (cover(&blackKey, ecc_key_der_256, sizeof_ecc_key_der_256) != 0) {
        printf("error covering up key\n");
        goto end;
    }

    /* set signing callback to use during TLS connection */
    wolfSSL_CTX_SetEccSignCb(ctx, TLS_ECC_Sign_callback);

    /* set ECDH shared secret callback to use during TLS connection */
    wolfSSL_CTX_SetEccSharedSecretCb(ctx, TLS_ECDH_callback);

    /* Initialize the server address struct with zeros */
    memset(&servAddr, 0, sizeof(servAddr));

    /* Fill in the server address */
    servAddr.sin_family      = AF_INET;             /* using IPv4      */
    servAddr.sin_port        = htons(DEFAULT_PORT); /* on DEFAULT_PORT */
    servAddr.sin_addr.s_addr = INADDR_ANY;          /* from anywhere   */

    /* Bind the server socket to our port */
    if (bind(sockfd, (struct sockaddr*)&servAddr, sizeof(servAddr)) == -1) {
        fprintf(stderr, "ERROR: failed to bind\n");
        goto end;
    }

    /* Listen for a new connection, allow 5 pending connections */
    if (listen(sockfd, 5) == -1) {
        fprintf(stderr, "ERROR: failed to listen\n");
        goto end;
    }

    /* Continue to accept clients until shutdown is issued */
    printf("Waiting for a connection...\n");

    /* Accept client connections */
    if ((connd = accept(sockfd, (struct sockaddr*)&clientAddr, &size)) == -1) {
        fprintf(stderr, "ERROR: failed to accept the connection\n\n");
        goto end;
    }

    /* Create a WOLFSSL object */
    if ((ssl = wolfSSL_new(ctx)) == NULL) {
        fprintf(stderr, "ERROR: failed to create WOLFSSL object\n");
        goto end;
    }

    /* Attach wolfSSL to the socket */
    wolfSSL_set_fd(ssl, connd);

    /* associate the covered up black key with the WOLFSSH structure,
     * this will then get passed as the last argument to the callbacks */
    wolfSSL_SetEccSignCtx(ssl, (void*)&blackKey);
    wolfSSL_SetEccSharedSecretCtx(ssl, (void*)&blackKey);

    /* Establish TLS connection */
    ret = wolfSSL_accept(ssl);
    if (ret != SSL_SUCCESS) {
        fprintf(stderr, "wolfSSL_accept error = %d\n",
                wolfSSL_get_error(ssl, ret));
        goto end;
    }
    printf("Client connected successfully\n");

    /* Read the client data into our buff array */
    memset(buff, 0, sizeof(buff));
    if (wolfSSL_read(ssl, buff, sizeof(buff)-1) == -1) {
        fprintf(stderr, "ERROR: failed to read\n");
        goto end;
    }

    /* Print to stdout any data the client sends */
    printf("Client: %s\n", buff);

    /* Write our reply into buff */
    memset(buff, 0, sizeof(buff));
    memcpy(buff, reply, strlen(reply));
    len = strnlen(buff, sizeof(buff));

    /* Reply back to the client */
    if (wolfSSL_write(ssl, buff, len) != len) {
        fprintf(stderr, "ERROR: failed to write\n");
        goto end;
    }

end:
    /* Cleanup after this connection */
    wolfSSL_free(ssl);      /* Free the wolfSSL object              */
    close(connd);           /* Close the connection to the client   */
    wc_ecc_free(&blackKey);

    printf("Shutdown complete\n");

    /* Cleanup and return */
    wolfSSL_CTX_free(ctx);  /* Free the wolfSSL context object          */
    wolfSSL_Cleanup();      /* Cleanup the wolfSSL environment          */
    close(sockfd);          /* Close the socket listening for clients   */
    return 0;               /* Return reporting a success               */
}
