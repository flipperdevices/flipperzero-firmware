/* quic.c QUIC unit tests
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

#include <tests/unit.h>

#ifdef WOLFSSL_QUIC

#include <wolfssl/ssl.h>
#include <wolfssl/quic.h>
#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif
#include <wolfssl/error-ssl.h>
#include <wolfssl/internal.h>


#define testingFmt "   %s:"
#define resultFmt  " %s\n"
static const char* passed = "passed";
static const char* failed = "failed";

typedef struct {
    const char *name;
    WOLFSSL_METHOD *method;
    int is_server;
} ctx_setups;

static int dummy_set_encryption_secrets(WOLFSSL *ssl, WOLFSSL_ENCRYPTION_LEVEL level,
                                         const uint8_t *read_secret,
                                         const uint8_t *write_secret, size_t secret_len)
{
    (void)ssl;
    printf("QUIC_set_encryption_secrets(level=%d, length=%d, rx=%s, tx=%s)\n",
           level, (int)secret_len, read_secret? "yes" : "no", write_secret? "yes" : "no");
    return 1;
}

static int dummy_add_handshake_data(WOLFSSL *ssl, WOLFSSL_ENCRYPTION_LEVEL level,
                                    const uint8_t *data, size_t len)
{
    (void)ssl;
    (void)data;
    printf("QUIC_add_handshake_data(level=%d, length=%d)\n", level, (int)len);
    return 1;
}

static int dummy_flush_flight(WOLFSSL *ssl)
{
    (void)ssl;
    printf("QUIC_flush_flight()\n");
    return 1;
}

static int dummy_send_alert(WOLFSSL *ssl, WOLFSSL_ENCRYPTION_LEVEL level, uint8_t err)
{
    (void)ssl;
    printf("QUIC_send_alert(level=%d, err=%d)\n", level, err);
    return 1;
}

static WOLFSSL_QUIC_METHOD dummy_method = {
    dummy_set_encryption_secrets,
    dummy_add_handshake_data,
    dummy_flush_flight,
    dummy_send_alert,
};

static WOLFSSL_QUIC_METHOD null_method = {
    NULL, NULL, NULL, NULL
};

static int test_set_quic_method(void) {
    WOLFSSL_CTX *ctx;
    WOLFSSL *ssl;
    int ret = 0, i;
    const uint8_t *data;
    size_t data_len;
    ctx_setups valids[] = {
#ifdef WOLFSSL_TLS13
        { "TLSv1.3 server", wolfTLSv1_3_server_method(), 1},
        { "TLSv1.3 client", wolfTLSv1_3_client_method(), 0},
#endif
        { NULL, NULL, 0}
    };
    ctx_setups invalids[] = {
#ifndef WOLFSSL_NO_TLS12
        { "TLSv1.2 server", wolfTLSv1_2_server_method(), 1},
        { "TLSv1.2 client", wolfTLSv1_2_client_method(), 0},
#endif
#ifndef NO_OLD_TLS
        { "TLSv1.1 server", wolfTLSv1_1_server_method(), 1},
        { "TLSv1.1 client", wolfTLSv1_1_client_method(), 0},
#endif
        { NULL, NULL, 0}
    };

    for (i = 0; valids[i].name != NULL; ++i) {
        AssertNotNull(ctx = wolfSSL_CTX_new(valids[i].method));
        if (valids[i].is_server) {
            AssertTrue(wolfSSL_CTX_use_certificate_file(ctx, svrCertFile,
                                                        WOLFSSL_FILETYPE_PEM));
            AssertTrue(wolfSSL_CTX_use_PrivateKey_file(ctx, svrKeyFile,
                                                       WOLFSSL_FILETYPE_PEM));
        }
        /* ctx does not have quic enabled, so will SSL* derived from it */
        AssertNotNull(ssl = wolfSSL_new(ctx));
        AssertFalse(wolfSSL_is_quic(ssl));
        /* Enable quic on the SSL* */
        AssertFalse(wolfSSL_set_quic_method(ssl, &null_method) == WOLFSSL_SUCCESS);
        AssertTrue(wolfSSL_set_quic_method(ssl, &dummy_method) == WOLFSSL_SUCCESS);
        AssertTrue(wolfSSL_is_quic(ssl));
        /* Check some default, initial behaviour */
        AssertTrue(wolfSSL_set_quic_transport_params(ssl, NULL, 0) == WOLFSSL_SUCCESS);
        wolfSSL_get_peer_quic_transport_params(ssl, &data, &data_len);
        AssertNull(data);
        AssertTrue(data_len == 0);
        AssertTrue(wolfSSL_quic_read_level(ssl) == wolfssl_encryption_initial);
        AssertTrue(wolfSSL_quic_write_level(ssl) == wolfssl_encryption_initial);
        AssertTrue(wolfSSL_get_quic_transport_version(ssl) == 0);
        wolfSSL_set_quic_transport_version(ssl, TLSX_KEY_QUIC_TP_PARAMS);
        AssertTrue(wolfSSL_get_quic_transport_version(ssl) == TLSX_KEY_QUIC_TP_PARAMS);
        wolfSSL_set_quic_use_legacy_codepoint(ssl, 1);
        AssertTrue(wolfSSL_get_quic_transport_version(ssl) == TLSX_KEY_QUIC_TP_PARAMS_DRAFT);
        wolfSSL_set_quic_use_legacy_codepoint(ssl, 0);
        AssertTrue(wolfSSL_get_quic_transport_version(ssl) == TLSX_KEY_QUIC_TP_PARAMS);
        /* max flight len during stages of handhshake, we us 16k initial and on
         * app data, and during handshake allow larger for cert exchange. This is
         * more advisory for the network code. ngtcp2 has its own ideas, for example.
         */
        data_len = wolfSSL_quic_max_handshake_flight_len(ssl, wolfssl_encryption_initial);
        AssertTrue(data_len == 16*1024);
        data_len = wolfSSL_quic_max_handshake_flight_len(ssl, wolfssl_encryption_early_data);
        AssertTrue(data_len == 0);
        data_len = wolfSSL_quic_max_handshake_flight_len(ssl, wolfssl_encryption_handshake);
        AssertTrue(data_len >= 16*1024);
        data_len = wolfSSL_quic_max_handshake_flight_len(ssl, wolfssl_encryption_application);
        AssertTrue(data_len == 16*1024);
        wolfSSL_free(ssl);
        /* Enabled quic on the ctx */
        AssertTrue(wolfSSL_CTX_set_quic_method(ctx, &dummy_method) == WOLFSSL_SUCCESS);
        /* It will be enabled on the SSL* */
        AssertNotNull(ssl = wolfSSL_new(ctx));
        AssertTrue(wolfSSL_is_quic(ssl));
        wolfSSL_free(ssl);

        wolfSSL_CTX_free(ctx);
    }

    for (i = 0; invalids[i].name != NULL; ++i) {

        AssertNotNull(ctx = wolfSSL_CTX_new(invalids[i].method));
        AssertTrue(wolfSSL_CTX_use_certificate_file(ctx, svrCertFile,
                                                    WOLFSSL_FILETYPE_PEM));
        AssertTrue(wolfSSL_CTX_use_PrivateKey_file(ctx, svrKeyFile,
                                                   WOLFSSL_FILETYPE_PEM));
        AssertFalse(wolfSSL_CTX_set_quic_method(ctx, &dummy_method) == WOLFSSL_SUCCESS);
        AssertNotNull(ssl = wolfSSL_new(ctx));
        AssertFalse(wolfSSL_set_quic_method(ssl, &dummy_method) == WOLFSSL_SUCCESS);
        AssertFalse(wolfSSL_is_quic(ssl));
        /* even though not quic, this is the only level we can return */
        AssertTrue(wolfSSL_quic_read_level(ssl) == wolfssl_encryption_initial);
        AssertTrue(wolfSSL_quic_write_level(ssl) == wolfssl_encryption_initial);
        wolfSSL_free(ssl);
        wolfSSL_CTX_free(ctx);
    }

    printf("    test_set_quic_method: %s\n", (ret == 0)? passed : failed);
    return ret;
}

static size_t fake_record(byte rtype, word32 rlen, uint8_t *rec)
{
    rec[0] = (uint8_t)rtype;
    c32to24(rlen, rec+1);
    return rlen + 4;
}

static size_t shift_record(uint8_t *rec, size_t len, size_t written)
{
    len -= written;
    XMEMMOVE(rec, rec+written, len);
    return len;
}

static void dump_buffer(const char *name, const byte *p, size_t len, int indent)
{
    size_t i = 0;

    printf("%s[%d] = {", name, (int)len);
    while((p != NULL) && (i < len)) {
        if((i % 0x10) == 0) {
            printf("\n");
            printf("%*s  %04X - ", indent, " ", (int)i);
        }
        else if((i % 0x08) == 0) {
            printf("  ");
        }
        printf("%02X ", p[i]);
        i++;
    }
    printf("\n%*s};\n", indent, " ");
}

static void dump_ssl_buffers(WOLFSSL *ssl, FILE *fp)
{
    QuicRecord *qr = ssl->quic.input_head;

    fprintf(fp, "SSL quic data buffered: \n");
    while (qr) {
        fprintf(fp, "  - %d-%d/%d (cap %d, level=%d)\n",
                qr->start, qr->end, qr->len, qr->capacity, qr->level);
        qr = qr->next;
    }
    if ((qr = ssl->quic.scratch)) {
        fprintf(fp, "  scratch: %d-%d/%d (cap %d, level=%d)\n",
                qr->start, qr->end, qr->len, qr->capacity, qr->level);
    }
    else {
        fprintf(fp, "  scratch: -\n");
    }
}

static int provide_data(WOLFSSL *ssl, WOLFSSL_ENCRYPTION_LEVEL level,
                        const uint8_t *data, size_t len, int excpect_fail)
{
    int ret;

    ret = (wolfSSL_provide_quic_data(ssl, level, data, len) == WOLFSSL_SUCCESS);
    if (!!ret != !excpect_fail) {
        dump_ssl_buffers(ssl, stdout);
        return 0;
    }
    return 1;
}

static int test_provide_quic_data(void) {
    WOLFSSL_CTX *ctx;
    WOLFSSL *ssl;
    uint8_t lbuffer[16*1024];
    size_t len;
    int ret = 0;

    AssertNotNull(ctx = wolfSSL_CTX_new(wolfTLSv1_3_client_method()));
    AssertTrue(wolfSSL_CTX_set_quic_method(ctx, &dummy_method) == WOLFSSL_SUCCESS);
    /* provide_quic_data() feeds CRYPTO packets inside a QUIC Frame into
    * the TLSv1.3 state machine.
     * The data fed is not the QUIC frame, but the TLS record inside it.
     * This may be called several times before SSL_do_handshake() is invoked
     * to process them.
     * During buffering this data, the code checks that:
     * - encryption level only ever increases for subsequent TLS records
     * - a TLS record is received complete before the encryption level increases
     */
    AssertNotNull(ssl = wolfSSL_new(ctx));
    len = fake_record(1, 100, lbuffer);
    AssertTrue(provide_data(ssl, wolfssl_encryption_initial, lbuffer, len, 0));
    len = fake_record(2, 1523, lbuffer);
    AssertTrue(provide_data(ssl, wolfssl_encryption_handshake, lbuffer, len, 0));
    len = fake_record(2, 1, lbuffer);
    len += fake_record(3, 190, lbuffer+len);
    AssertTrue(provide_data(ssl, wolfssl_encryption_handshake, lbuffer, len, 0));
    len = fake_record(5, 2049, lbuffer);
    AssertTrue(provide_data(ssl, wolfssl_encryption_application, lbuffer, len, 0));
    /* adding another record with decreased level must fail */
    len = fake_record(1, 100, lbuffer);
    AssertTrue(provide_data(ssl, wolfssl_encryption_initial, lbuffer, len, 1));
    wolfSSL_free(ssl);

    AssertNotNull(ssl = wolfSSL_new(ctx));
    len = fake_record(1, 100, lbuffer);
    AssertTrue(provide_data(ssl, wolfssl_encryption_initial, lbuffer, 24, 0));
    len = shift_record(lbuffer, len, 24);
    len += fake_record(2, 4000, lbuffer+len);
    AssertTrue(provide_data(ssl, wolfssl_encryption_initial, lbuffer, len - 99, 0));
    len = shift_record(lbuffer, len, len - 99);
    len += fake_record(5, 2049, lbuffer+len);
    AssertTrue(provide_data(ssl, wolfssl_encryption_initial, lbuffer, len, 0));
    /* should be recognized as complete and level increase needs to be accepted */
    len = fake_record(2, 1, lbuffer);
    len += fake_record(3, 190, lbuffer+len);
    AssertTrue(provide_data(ssl, wolfssl_encryption_handshake, lbuffer, len - 10, 0));
    len = shift_record(lbuffer, len, len - 10);
    /* Change level with incomplete record in lbuffer, needs to fail */
    len += fake_record(5, 8102, lbuffer+len);
    AssertTrue(provide_data(ssl, wolfssl_encryption_application, lbuffer, len - 10, 1));
    wolfSSL_free(ssl);

    wolfSSL_CTX_free(ctx);

    printf("    test_provide_quic_data: %s\n", (ret == 0)? passed : failed);
    return 0;
}


static int test_quic_crypt(void) {
    WOLFSSL_CTX *ctx;
    WOLFSSL *ssl;
    const WOLFSSL_EVP_CIPHER *aead_cipher;
    int ret = 0;

    AssertNotNull(ctx = wolfSSL_CTX_new(wolfTLSv1_3_client_method()));
    AssertTrue(wolfSSL_CTX_set_quic_method(ctx, &dummy_method) == WOLFSSL_SUCCESS);
    AssertNotNull(ssl = wolfSSL_new(ctx));

    /* don't have an AEAD cipher selected before start */
    AssertTrue(wolfSSL_CIPHER_get_id(wolfSSL_get_current_cipher(ssl)) == 0);
    AssertNotNull(aead_cipher = wolfSSL_EVP_aes_128_gcm());
    AssertTrue(wolfSSL_quic_aead_is_gcm(aead_cipher) != 0);
    AssertTrue(wolfSSL_quic_aead_is_ccm(aead_cipher) == 0);
    AssertTrue(wolfSSL_quic_aead_is_chacha20(aead_cipher) == 0);

    if (1) {
        /* check that our enc-/decrypt support in quic rount-trips */
        static const uint8_t key[16] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                                        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
        static const uint8_t aad[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
        static const uint8_t iv[] = {20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};
        static const uint8_t plaintext[] = "hello world\nhello world\nhello world\nhello world\nhello world\nhello world\nhello world\n";
        static const uint8_t expected[] = {0xd3, 0xa8, 0x1d, 0x96, 0x4c, 0x9b, 0x02, 0xd7, 0x9a, 0xb0, 0x41, 0x07, 0x4c, 0x8c, 0xe2,
                                           0xe0, 0x2e, 0x83, 0x54, 0x52, 0x45, 0xcb, 0xd4, 0x68, 0xc8, 0x43, 0x45, 0xca, 0x91, 0xfb,
                                           0xa3, 0x7a, 0x67, 0xed, 0xe8, 0xd7, 0x5e, 0xe2, 0x33, 0xd1, 0x3e, 0xbf, 0x50, 0xc2, 0x4b,
                                           0x86, 0x83, 0x55, 0x11, 0xbb, 0x17, 0x4f, 0xf5, 0x78, 0xb8, 0x65, 0xeb, 0x9a, 0x2b, 0x8f,
                                           0x77, 0x08, 0xa9, 0x60, 0x17, 0x73, 0xc5, 0x07, 0xf3, 0x04, 0xc9, 0x3f, 0x67, 0x4d, 0x12,
                                           0xa1, 0x02, 0x93, 0xc2, 0x3c, 0xd3, 0xf8, 0x59, 0x33, 0xd5, 0x01, 0xc3, 0xbb, 0xaa, 0xe6,
                                           0x3f, 0xbb, 0x23, 0x66, 0x94, 0x26, 0x28, 0x43, 0xa5, 0xfd, 0x2f};
        WOLFSSL_EVP_CIPHER_CTX *enc_ctx, *dec_ctx;
        uint8_t *encrypted, *decrypted;
        size_t tag_len, enc_len, dec_len;

        AssertTrue((tag_len = wolfSSL_quic_get_aead_tag_len(aead_cipher)) == 16);
        dec_len = sizeof(plaintext);
        enc_len = dec_len + tag_len;
        encrypted = (uint8_t*)XMALLOC(enc_len, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        AssertNotNull(encrypted);
        decrypted = (uint8_t*)XMALLOC(dec_len, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        AssertNotNull(decrypted);

        AssertNotNull(enc_ctx = wolfSSL_quic_crypt_new(aead_cipher, key, iv, 1));
        AssertTrue(wolfSSL_quic_aead_encrypt(encrypted, enc_ctx,
                                             plaintext, sizeof(plaintext),
                                             NULL, aad, sizeof(aad)) == WOLFSSL_SUCCESS);
        AssertTrue(memcmp(expected, encrypted, dec_len) == 0);
        AssertTrue(memcmp(expected+dec_len, encrypted+dec_len, tag_len) == 0);

        AssertNotNull(dec_ctx = wolfSSL_quic_crypt_new(aead_cipher, key, iv, 0));
        AssertTrue(wolfSSL_quic_aead_decrypt(decrypted, dec_ctx,
                                             encrypted, enc_len,
                                             NULL, aad, sizeof(aad)) == WOLFSSL_SUCCESS);
        AssertTrue(memcmp(plaintext, decrypted, dec_len) == 0);

        XFREE(encrypted, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(decrypted, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        wolfSSL_EVP_CIPHER_CTX_free(enc_ctx);
        wolfSSL_EVP_CIPHER_CTX_free(dec_ctx);
    }

    wolfSSL_free(ssl);
    wolfSSL_CTX_free(ctx);

    printf("    test_quic_crypt: %s\n", (ret == 0)? passed : failed);
    return ret;
}

typedef struct OutputBuffer {
    byte data[64*1024];
    size_t len;
    WOLFSSL_ENCRYPTION_LEVEL level;
    struct OutputBuffer *next;
} OutputBuffer;

typedef struct {
    const char *name;
    WOLFSSL *ssl;
    OutputBuffer output;
    byte rx_secret[4][1024];
    size_t rx_secret_len[4];
    byte tx_secret[4][1024];
    size_t tx_secret_len[4];
    int handshake_done;
    int alert_level;
    int alert;
    int flushed;
    int verbose;
    byte ticket[16*1024];
    word32 ticket_len;
    byte session[16*1024];
    word32 session_len;
} QuicTestContext;

static int ctx_set_encryption_secrets(WOLFSSL *ssl, WOLFSSL_ENCRYPTION_LEVEL level,
                                      const uint8_t *read_secret,
                                      const uint8_t *write_secret, size_t secret_len);
static int ctx_add_handshake_data(WOLFSSL *ssl, WOLFSSL_ENCRYPTION_LEVEL level,
                                  const uint8_t *data, size_t len);
static int ctx_flush_flight(WOLFSSL *ssl);
static int ctx_send_alert(WOLFSSL *ssl, WOLFSSL_ENCRYPTION_LEVEL level, uint8_t err);
#ifdef HAVE_SESSION_TICKET
static int ctx_session_ticket_cb(WOLFSSL* ssl,
                                 const unsigned char* ticket, int ticketSz,
                                 void* cb_ctx);
#endif

static WOLFSSL_QUIC_METHOD ctx_method = {
    ctx_set_encryption_secrets,
    ctx_add_handshake_data,
    ctx_flush_flight,
    ctx_send_alert,
};

static void QuicTestContext_init(QuicTestContext *tctx, WOLFSSL_CTX *ctx,
                                 const char *name, int verbose)
{
    static const byte tp_params_c[] = {0, 1, 2, 3, 4, 5, 6, 7};
    static const byte tp_params_s[] = {7, 6, 5, 4, 3, 2, 1, 0, 1};

    AssertNotNull(tctx);
    memset(tctx, 0, sizeof(*tctx));
    tctx->name = name;
    AssertNotNull((tctx->ssl = wolfSSL_new(ctx)));
    tctx->verbose = verbose;
    wolfSSL_set_app_data(tctx->ssl, tctx);
    AssertTrue(wolfSSL_set_quic_method(tctx->ssl, &ctx_method) == WOLFSSL_SUCCESS);
    wolfSSL_set_verify(tctx->ssl, SSL_VERIFY_NONE, 0);
#ifdef HAVE_SESSION_TICKET
    wolfSSL_UseSessionTicket(tctx->ssl);
    wolfSSL_set_SessionTicket_cb(tctx->ssl, ctx_session_ticket_cb, NULL);
#endif
    if (wolfSSL_is_server(tctx->ssl)) {
        wolfSSL_set_quic_transport_version(tctx->ssl, 0);
        wolfSSL_set_quic_transport_params(tctx->ssl, tp_params_s, sizeof(tp_params_s));
    }
    else {
        wolfSSL_set_quic_transport_version(tctx->ssl, 0);
        wolfSSL_set_quic_transport_params(tctx->ssl, tp_params_c, sizeof(tp_params_c));
    }
}

static void QuicTestContext_free(QuicTestContext *tctx)
{
    OutputBuffer *out, *n;

    if (tctx->ssl) {
        wolfSSL_free(tctx->ssl);
        tctx->ssl = NULL;
    }
    out = tctx->output.next;
    while (out) {
        n = out->next;
        free(out);
        out = n;
    }
}

static int ctx_set_encryption_secrets(WOLFSSL *ssl, WOLFSSL_ENCRYPTION_LEVEL level,
                                      const uint8_t *read_secret,
                                      const uint8_t *write_secret, size_t secret_len)
{
    QuicTestContext *ctx = (QuicTestContext*)wolfSSL_get_app_data(ssl);

    AssertNotNull(ctx);
    AssertTrue(secret_len <= sizeof(ctx->rx_secret[0]));
    if (read_secret) {
        memcpy(ctx->rx_secret[level], read_secret, secret_len);
        ctx->rx_secret_len[level] = secret_len;
    }
    if (write_secret) {
        memcpy(ctx->tx_secret[level], write_secret, secret_len);
        ctx->tx_secret_len[level] = secret_len;
    }
    AssertNotNull(ctx);
    return 1;
}

static int ctx_add_handshake_data(WOLFSSL *ssl, WOLFSSL_ENCRYPTION_LEVEL level,
                                  const uint8_t *data, size_t len)
{
    QuicTestContext *ctx = (QuicTestContext*)wolfSSL_get_app_data(ssl);
    OutputBuffer *out;

    AssertNotNull(ctx);
    out = &ctx->output;
    while (out->next) {
        out = out->next;
    }
    if (out->level != level) {
        if (out->len > 0) {
            out->next = (OutputBuffer*)calloc(1, sizeof(OutputBuffer));
            out = out->next;
            AssertNotNull(out);
        }
        out->level = level;
    }
    if (ctx->verbose) {
        printf("[%s] add_handshake[enc_level=%d]: %d bytes\n", ctx->name, level, (int)len);
        /* dump_buffer("add", data, len, 0); */
    }
    if (len > 0) {
        AssertTrue(out->len + len < sizeof(out->data));
        memcpy(out->data + out->len, data, len);
        out->len += len;
    }
    return 1;
}

static int ctx_flush_flight(WOLFSSL *ssl)
{
    QuicTestContext *ctx = (QuicTestContext*)wolfSSL_get_app_data(ssl);

    AssertNotNull(ctx);
    ctx->flushed = 1;
    return 1;
}

static int ctx_send_alert(WOLFSSL *ssl, WOLFSSL_ENCRYPTION_LEVEL level, uint8_t err)
{
    QuicTestContext *ctx = (QuicTestContext*)wolfSSL_get_app_data(ssl);

    if (ctx->verbose) {
        printf("[%s] send_alert: level=%d, err=%d\n", ctx->name, level, err);
    }
    AssertNotNull(ctx);
    ctx->alert_level = level;
    ctx->alert = alert;
    return 1;
}

#ifdef HAVE_SESSION_TICKET
static int ctx_session_ticket_cb(WOLFSSL* ssl,
                                 const unsigned char* ticket, int ticketSz,
                                 void* cb_ctx)
{
    QuicTestContext *ctx = (QuicTestContext*)wolfSSL_get_app_data(ssl);

    (void)cb_ctx;
    if (ticketSz < 0 || (size_t)ticketSz > sizeof(ctx->ticket)) {
        printf("SESSION TICKET callback: ticket given is too large: %d bytes\n", ticketSz);
        return 1;
    }
    memset(ctx->ticket, 0, sizeof(ctx->ticket));
    ctx->ticket_len = (word32)ticketSz;
    memcpy(ctx->ticket, ticket, ticketSz);
    if (ctx->verbose) {
        printf("Session Ticket[%s]: ", ctx->name);
        dump_buffer("", ticket, ticketSz, 4);
    }
    return 0;
}
#endif

static void ctx_dump_output(QuicTestContext *ctx)
{
    dump_buffer("Output", ctx->output.data, ctx->output.len, 0);
}

static void check_handshake_record(const byte *data, size_t data_len, int *ptype, size_t *prlen)
{
    word32 rlen;
    AssertTrue(data_len >= HANDSHAKE_HEADER_SZ);
    *ptype = data[0];
    c24to32(&data[1], &rlen);
    *prlen = rlen + HANDSHAKE_HEADER_SZ;
}

static void ext_dump(const byte *data, size_t data_len, int indent)
{
    size_t idx = 0;
    word16 len16, etype, i;

    printf("%*sextensions:\n", indent, " ");
    while (idx < data_len) {
        ato16(&data[idx], &etype); /* extension type */
        ato16(&data[idx+2], &len16); /* extension length */
        printf("  extension: %04x [", etype);
        for (i = 0; i < len16; ++i) {
            printf("%s0x%02x", (i? ", ": ""), data[idx+4+i]);
        }
        printf("]\n");
        idx += 2 + 2 + len16;
    }
}

static const byte *ext_find(const byte *data, size_t data_len, int ext_type)
{
    size_t idx = 0;
    word16 len16, etype;

    while (idx < data_len) {
        ato16(&data[idx], &etype); /* extension type */
        if (etype == ext_type) {
            return data + idx;
        }
        ato16(&data[idx+2], &len16); /* extension length */
        idx += 2 + 2 + len16;
    }
    return NULL;
}

static int ext_has(const byte *data, size_t data_len, int ext_type)
{
    return ext_find(data, data_len,ext_type) != NULL;
}

static void ext_equals(const byte *data, size_t data_len, int ext_type,
                       const byte *exp_data, size_t exp_len)
{
    const byte *ext;
    word16 len16;

    AssertNotNull(ext = ext_find(data, data_len, ext_type));
    ato16(&ext[2], &len16);
    AssertTrue(len16 == exp_len);
    AssertTrue(memcmp(ext + 4, exp_data, exp_len) == 0);
}

static void check_quic_client_hello(const byte *data, size_t data_len, int verbose, int indent)
{
    size_t idx;
    word16 len16;
    const byte *exts;
    size_t exts_len, rec_len;
    int rec_type;
    static byte ext_sup_version[3] = {0x02, 0x03, 0x04};

    check_handshake_record(data, data_len, &rec_type, &rec_len);
    AssertIntEQ(rec_type, client_hello);
    idx = HANDSHAKE_HEADER_SZ;
    /* the client hello arrives alone */
    AssertIntEQ(rec_len, data_len);
    AssertTrue(data[idx++] == SSLv3_MAJOR);
    AssertTrue(data[idx++] == TLSv1_2_MINOR);
    idx += 32; /* 32 bytes RANDOM */
    AssertIntEQ(data[idx], 0);  /* session id length MUST be 0, RFC9001 ch. 8.4 */
    idx += 1 + data[idx];
    ato16(&data[idx], &len16); /* ciphers length */
    AssertTrue(len16 > 0);
    idx += 2 + len16;
    AssertTrue(data[idx] == 1);   /* compressions */
    AssertTrue(data[idx+1] == 0);   /* no compression */
    idx += 2;
    ato16(&data[idx], &len16); /* extensions length */
    AssertTrue(len16 > 0);
    exts_len = len16;
    idx += 2;
    exts = &data[idx];
    idx += exts_len;
    AssertTrue(idx <= rec_len); /* should fit */
    for (; idx < rec_len; ++idx) {
        AssertTrue(data[idx] == 0); /* padding */
    }
    ext_equals(exts, exts_len, TLSX_SUPPORTED_VERSIONS,
               ext_sup_version, sizeof(ext_sup_version));
    if (verbose) {
        ext_dump(exts, exts_len, indent);
        dump_buffer("", data, data_len, indent);
    }
}

static void check_quic_client_hello_tp(OutputBuffer *out, int tp_v1, int tp_draft)
{
    size_t idx;
    word16 len16;
    const byte *exts;
    size_t exts_len, rec_len;
    int rec_type;

    check_handshake_record(out->data, out->len, &rec_type, &rec_len);
    AssertIntEQ(rec_type, client_hello);
    idx = HANDSHAKE_HEADER_SZ;
    idx += 2; /* old version */
    idx += 32; /* 32 bytes RANDOM */
    idx += 1 + out->data[idx]; /* session id */
    ato16(&out->data[idx], &len16); /* ciphers length */
    idx += 2 + len16;
    idx += 2; /* compression */
    ato16(&out->data[idx], &len16); /* extensions length */
    AssertTrue(len16 > 0);
    exts_len = len16;
    idx += 2;
    exts = &out->data[idx];

    AssertTrue(!ext_has(exts, exts_len, TLSX_KEY_QUIC_TP_PARAMS) == !tp_v1);
    AssertTrue(!ext_has(exts, exts_len, TLSX_KEY_QUIC_TP_PARAMS_DRAFT) == !tp_draft);
}

static void check_secrets(QuicTestContext *ctx, WOLFSSL_ENCRYPTION_LEVEL level, size_t rx_len, size_t tx_len)
{
    int idx = (int)level;
    AssertTrue(idx < 4);
    AssertIntEQ(ctx->rx_secret_len[idx], rx_len);
    AssertIntEQ(ctx->tx_secret_len[idx], tx_len);
}

static void assert_secrets_EQ(QuicTestContext *ctx1, QuicTestContext *ctx2,
                              WOLFSSL_ENCRYPTION_LEVEL level)
{
    int idx = (int)level;
    /* rx secrets are the other ones tx secrets */
    AssertIntEQ(ctx1->rx_secret_len[idx], ctx2->tx_secret_len[idx]);
    AssertIntEQ(ctx1->tx_secret_len[idx], ctx2->rx_secret_len[idx]);
    AssertIntEQ(memcmp(ctx1->rx_secret[idx], ctx2->tx_secret[idx], ctx1->rx_secret_len[idx]), 0);
    AssertIntEQ(memcmp(ctx1->tx_secret[idx], ctx2->rx_secret[idx], ctx1->tx_secret_len[idx]), 0);
}

static void check_ee(const byte *data, size_t data_len, int verbose, int indent)
{
    size_t rec_len, exts_len, idx;
    word16 len16;
    const byte *exts;
    int rec_type;

    check_handshake_record(data, data_len, &rec_type, &rec_len);
    AssertIntEQ(rec_type, encrypted_extensions);
    idx = HANDSHAKE_HEADER_SZ;
    ato16(&data[idx], &len16); /* extensions length */
    AssertTrue(len16 > 0);
    exts_len = len16;
    idx += 2;
    exts = &data[idx];
    if (verbose) {
        ext_dump(exts, exts_len, indent);
        dump_buffer("", data, data_len, indent);
    }
}

static void check_quic_server_hello(const byte *data, size_t data_len, int verbose, int indent)
{
    size_t idx;
    word16 len16, cipher;
    const byte *exts;
    size_t exts_len, rec_len;
    static byte ext_sup_version[2] = {0x03, 0x04};
    int rec_type;

    check_handshake_record(data, data_len, &rec_type, &rec_len);
    AssertIntEQ(rec_type, server_hello);
    idx = HANDSHAKE_HEADER_SZ;
    AssertTrue(data[idx++] == SSLv3_MAJOR);
    AssertTrue(data[idx++] == TLSv1_2_MINOR);
    idx += 32; /* 32 bytes RANDOM */
    /* AssertIntEQ(data[idx], 0);  session id of len 0 */
    idx += 1 + data[idx];
    ato16(&data[idx], &cipher); /* cipher selected */
    AssertTrue(cipher != 0);
    idx += 2;
    AssertTrue(data[idx] == 0);   /* null compression */
    idx += 1;
    ato16(&data[idx], &len16); /* extensions length */
    AssertTrue(len16 > 0);
    exts_len = len16;
    idx += 2;
    exts = &data[idx];
    idx += exts_len;
    AssertTrue(idx <= rec_len); /* should fit */
    for (; idx < rec_len; ++idx) {
        AssertTrue(data[idx] == 0); /* padding */
    }
    if (verbose) {
        ext_dump(exts, exts_len, indent);
        dump_buffer("", data, rec_len, indent);
    }
    ext_equals(exts, exts_len, TLSX_SUPPORTED_VERSIONS,
               ext_sup_version, sizeof(ext_sup_version));
}

static void check_crypto_rec(const byte *data, size_t data_len, int verbose, int indent)
{
    size_t rec_len;
    int rec_type;

    check_handshake_record(data, data_len, &rec_type, &rec_len);
    if (verbose) {
        dump_buffer("", data, rec_len, indent);
    }
}

static void check_crypto_records(QuicTestContext *from, OutputBuffer *out, int indent, char *rec_log, size_t rec_log_size)
{
    const byte *data = out->data;
    size_t data_len = out->len;
    size_t rec_len;
    int rec_type;
    const char *rec_name;
    char lbuffer[128];
    void (*check_rec) (const byte *d, size_t l, int v, int indent);

    while (data_len > 0) {
        check_handshake_record(data, data_len, &rec_type, &rec_len);
        if (rec_len > data_len) {
            printf("%*sINCOMPLETE CRYPTO?: ", indent, " ");
            dump_buffer("", data, data_len, indent);
        }
        AssertTrue(rec_len <= data_len);
        check_rec = check_crypto_rec;
        switch (rec_type) {
            case client_hello:
                rec_name = "ClientHello";
                check_rec = check_quic_client_hello;
                break;
            case server_hello:
                rec_name = "ServerHello";
                check_rec = check_quic_server_hello;
                break;
            case session_ticket:
                rec_name = "SessionTicket";
                break;
            case encrypted_extensions:
                rec_name = "EncryptedExtension";
                check_rec = check_ee;
                break;
            case certificate:
                rec_name = "Certificate";
                break;
            case certificate_verify:
                rec_name = "CertificateVerify";
                break;
            case finished:
                rec_name = "Finished";
                break;
            default:
                sprintf(lbuffer, "%d", rec_type);
                rec_name = lbuffer;
                break;
        }

        if (rec_log) {
            if (*rec_log) XSTRLCAT(rec_log, ":", rec_log_size);
            XSTRLCAT(rec_log, rec_name, rec_log_size);
        }
        if (from->verbose) printf("%*sCRYPTO[%s]: ", indent, " ", rec_name);
        check_rec(data, rec_len, from->verbose, indent);
        if (from->verbose) printf("\n");
        data += rec_len;
        data_len -= rec_len;
    }
}

static void QuicTestContext_forward(QuicTestContext *from, QuicTestContext *to, char *rec_log, size_t rec_log_size)
{
    int ret;
    OutputBuffer *out, *old;

    out = &from->output;
    while (out->len > 0) {
        if (from->verbose) {
            printf("[%s -> %s] forward %d bytes at level %d\n",
                   from->name, to->name, (int)out->len, out->level);
        }
        if (out->level == wolfssl_encryption_early_data) {
            if (from->verbose) dump_buffer("EarlyData", out->data, out->len, 4);
        }
        else {
            check_crypto_records(from, out, 4, rec_log, rec_log_size);
        }
        ret = wolfSSL_provide_quic_data(to->ssl, out->level, out->data, out->len);
        out->len = 0;
        AssertIntEQ(ret, WOLFSSL_SUCCESS);
        if (out->next) {
            old = out->next;
            memcpy(out, out->next, sizeof(*out));
            free(old);
        }
    }
}

typedef struct {
    QuicTestContext *client;
    QuicTestContext *server;
    int started;
    int verbose;
    char rec_log[16*1024];
    int sent_early_data;
    int accept_early_data;
    char early_data[16*1024];
    size_t early_data_len;
} QuicConversation;

static void QuicConversation_init(QuicConversation *conv,
                                  QuicTestContext *tclient, QuicTestContext *tserver)
{
    memset(conv, 0, sizeof(*conv));
    conv->client = tclient;
    conv->server = tserver;
    conv->verbose = tclient->verbose && tserver->verbose;
}

static int QuicConversation_start(QuicConversation *conv, const byte *data,
                                  size_t data_len, size_t *pwritten)
{
    int ret;

    AssertFalse(conv->started);

    if (conv->verbose) {
        printf("[%s <-> %s] starting\n", conv->client->name, conv->server->name);
    }
    if (data && data_len > 0) {
#ifdef WOLFSSL_EARLY_DATA
        int written;
        ret = wolfSSL_write_early_data(conv->client->ssl, data, (int)data_len, &written);
        if (ret < 0) {
            int err = wolfSSL_get_error(conv->client->ssl, ret);
            char lbuffer[1024];
            printf("EARLY DATA ret = %d, error = %d, %s\n", ret, err, wolfSSL_ERR_error_string(err, lbuffer));
            AssertTrue(0);
        }
        *pwritten = (size_t)written;
        conv->sent_early_data = 1;
#else
        fprintf(stderr, "Cannot send EARLY DATA without feature enabled!\n");
        AssertTrue(0);
#endif
    }
    else {
        ret = wolfSSL_connect(conv->client->ssl);
        if (ret != WOLFSSL_SUCCESS) {
            AssertIntEQ(wolfSSL_get_error(conv->client->ssl, 0), SSL_ERROR_WANT_READ);
        }
        if (pwritten) *pwritten = 0;
    }
    conv->started = 1;
    return ret;
}

static int QuicConversation_step(QuicConversation *conv, int may_fail)
{
    int n;

    if (!conv->started) {
        n = wolfSSL_connect(conv->client->ssl);
        if (n != WOLFSSL_SUCCESS
            && wolfSSL_get_error(conv->client->ssl, 0) != SSL_ERROR_WANT_READ) {
            if (may_fail) return 0;
            AssertIntEQ(SSL_ERROR_WANT_READ, wolfSSL_get_error(conv->client->ssl, 0));
        }
        conv->started = 1;
    }
    if (conv->server->output.len > 0) {
        QuicTestContext_forward(conv->server, conv->client, conv->rec_log, sizeof(conv->rec_log));
        n = wolfSSL_quic_read_write(conv->client->ssl);
        if (n != WOLFSSL_SUCCESS
            && wolfSSL_get_error(conv->client->ssl, 0) != SSL_ERROR_WANT_READ) {
            if (may_fail) return 0;
            AssertIntEQ(SSL_ERROR_WANT_READ, wolfSSL_get_error(conv->client->ssl, 0));
        }
        return 1;
    }
    else if (conv->client->output.len > 0) {
        QuicTestContext_forward(conv->client, conv->server, conv->rec_log, sizeof(conv->rec_log));
#ifdef WOLFSSL_EARLY_DATA
        if (conv->accept_early_data) {
            int written;
            n = wolfSSL_read_early_data(conv->server->ssl,
                                        conv->early_data + conv->early_data_len,
                                        (int)(sizeof(conv->early_data) - conv->early_data_len),
                                        &written);
            if (n < 0) {
                if (wolfSSL_get_error(conv->server->ssl, 0) != SSL_ERROR_WANT_READ) {
                    if (may_fail) return 0;
                    AssertIntEQ(wolfSSL_get_error(conv->server->ssl, 0), SSL_ERROR_WANT_READ);
                }
            }
            else if (n > 0) {
                conv->early_data_len += n;
                if (conv->verbose)
                    printf("RECVed early data, len now=%d\n", (int)conv->early_data_len);
            }
        }
        else
 #endif /* WOLFSSL_EARLY_DATA */
        {
            n = wolfSSL_quic_read_write(conv->server->ssl);
            if (n != WOLFSSL_SUCCESS
                && wolfSSL_get_error(conv->server->ssl, 0) != SSL_ERROR_WANT_READ) {
                if (may_fail) return 0;
                AssertIntEQ(wolfSSL_get_error(conv->server->ssl, 0), SSL_ERROR_WANT_READ);
            }
        }
        return 1;
    }
    return 0;
}

static void QuicConversation_do(QuicConversation *conv)
{
    if (!conv->started) {
        QuicConversation_start(conv, NULL, 0, NULL);
    }

    while (1) {
        if (!QuicConversation_step(conv, 0)) {
            int c_err = wolfSSL_get_error(conv->client->ssl, 0);
            int s_err = wolfSSL_get_error(conv->server->ssl, 0);
            if (c_err == 0 && s_err == 0) {
                break;  /* handshake done */
            }
            printf("Neither tclient nor server have anything to send, "
                   "but client_error=%d, server_error=%d\n",
                   c_err, s_err);
            AssertFalse(1);
        }
    }
}

#ifdef HAVE_SESSION_TICKET

static void QuicConversation_fail(QuicConversation *conv)
{
    if (!conv->started) {
        QuicConversation_start(conv, NULL, 0, NULL);
    }

    while (1) {
        if (!QuicConversation_step(conv, 1)) {
            int c_err = wolfSSL_get_error(conv->client->ssl, 0);
            int s_err = wolfSSL_get_error(conv->server->ssl, 0);
            AssertTrue(c_err != 0 || s_err != 0);
            break;
        }
    }
}

#endif /* HAVE_SESSION_TICKET */

static int test_quic_client_hello(int verbose) {
    WOLFSSL_CTX *ctx;
    int ret = 0;
    QuicTestContext tctx;

    (void)ctx_dump_output;

    AssertNotNull(ctx = wolfSSL_CTX_new(wolfTLSv1_3_client_method()));

    QuicTestContext_init(&tctx, ctx, "client", verbose);
    /* Without any QUIC transport params, this needs to fail */
    AssertTrue(wolfSSL_set_quic_transport_params(tctx.ssl, NULL, 0) == WOLFSSL_SUCCESS);
    AssertTrue(wolfSSL_quic_read_write(tctx.ssl) != 0);
    AssertIntEQ(wolfSSL_get_error(tctx.ssl, 0), QUIC_TP_MISSING_E);
    QuicTestContext_free(&tctx);

    /* Set transport params, expect both extensions */
    QuicTestContext_init(&tctx, ctx, "client", verbose);
#ifdef HAVE_SNI
    wolfSSL_UseSNI(tctx.ssl, WOLFSSL_SNI_HOST_NAME,
                   "wolfssl.com", sizeof("wolfssl.com")-1);
#endif
    AssertTrue(wolfSSL_connect(tctx.ssl) != 0);
    AssertIntEQ(wolfSSL_get_error(tctx.ssl, 0), SSL_ERROR_WANT_READ);
    check_quic_client_hello_tp(&tctx.output, 1, 1);
    QuicTestContext_free(&tctx);

    /* Set transport params v1, expect v1 extension */
    QuicTestContext_init(&tctx, ctx, "client", verbose);
    wolfSSL_set_quic_transport_version(tctx.ssl, TLSX_KEY_QUIC_TP_PARAMS);
    AssertTrue(wolfSSL_connect(tctx.ssl) != 0);
    check_quic_client_hello_tp(&tctx.output, 1, 0);
    QuicTestContext_free(&tctx);

    /* Set transport params draft, expect draft extension */
    QuicTestContext_init(&tctx, ctx, "client", verbose);
    wolfSSL_set_quic_transport_version(tctx.ssl, TLSX_KEY_QUIC_TP_PARAMS_DRAFT);
    AssertTrue(wolfSSL_connect(tctx.ssl) != 0);
    check_quic_client_hello_tp(&tctx.output, 0, 1);
    QuicTestContext_free(&tctx);

    /* Set transport params 0, expect both extension */
    QuicTestContext_init(&tctx, ctx, "client", verbose);
    wolfSSL_set_quic_transport_version(tctx.ssl, 0);
    AssertTrue(wolfSSL_connect(tctx.ssl) != 0);
    check_quic_client_hello_tp(&tctx.output, 1, 1);
    QuicTestContext_free(&tctx);

    wolfSSL_CTX_free(ctx);
    printf("    test_quic_client_hello: %s\n", (ret == 0)? passed : failed);

    return ret;
}

static int test_quic_server_hello(int verbose) {
    WOLFSSL_CTX *ctx_c, *ctx_s;
    int ret = 0;
    QuicTestContext tclient, tserver;
    QuicConversation conv;

    AssertNotNull(ctx_c = wolfSSL_CTX_new(wolfTLSv1_3_client_method()));
    AssertNotNull(ctx_s = wolfSSL_CTX_new(wolfTLSv1_3_server_method()));
    AssertTrue(wolfSSL_CTX_use_certificate_file(ctx_s, svrCertFile, WOLFSSL_FILETYPE_PEM));
    AssertTrue(wolfSSL_CTX_use_PrivateKey_file(ctx_s, svrKeyFile, WOLFSSL_FILETYPE_PEM));

    /* setup ssls */
    QuicTestContext_init(&tclient, ctx_c, "client", verbose);
    QuicTestContext_init(&tserver, ctx_s, "server", verbose);

    /* connect */
    QuicConversation_init(&conv, &tclient, &tserver);
    QuicConversation_step(&conv, 0);
    /* check established/missing secrets */
    check_secrets(&tserver, wolfssl_encryption_initial, 0, 0);
    check_secrets(&tserver, wolfssl_encryption_handshake, 32, 32);
    check_secrets(&tserver, wolfssl_encryption_application, 32, 32);
    check_secrets(&tclient, wolfssl_encryption_handshake, 0, 0);
    /* feed the server data to the client */
    QuicConversation_step(&conv, 0);
    /* client has generated handshake secret */
    check_secrets(&tclient, wolfssl_encryption_handshake, 32, 32);
    /* continue the handshake till done */
    conv.started = 1;
    /* run till end */
    QuicConversation_do(&conv);
    AssertIntEQ(tclient.output.len, 0);
    AssertIntEQ(tserver.output.len, 0);
    /* what have we seen? */
#ifdef HAVE_SESSION_TICKET
    AssertStrEQ(conv.rec_log, "ClientHello:ServerHello:EncryptedExtension:"
        "Certificate:CertificateVerify:Finished:Finished:SessionTicket");
#else
    AssertStrEQ(conv.rec_log, "ClientHello:ServerHello:EncryptedExtension:"
        "Certificate:CertificateVerify:Finished:Finished");
#endif
    /* we are at application encryption level */
    AssertTrue(wolfSSL_quic_read_level(tclient.ssl) == wolfssl_encryption_application);
    AssertTrue(wolfSSL_quic_write_level(tclient.ssl) == wolfssl_encryption_application);
    AssertTrue(wolfSSL_quic_read_level(tserver.ssl) == wolfssl_encryption_application);
    AssertTrue(wolfSSL_quic_write_level(tserver.ssl) == wolfssl_encryption_application);
    /* the last client write (FINISHED) was at handshake level */
    AssertTrue(tclient.output.level == wolfssl_encryption_handshake);
    /* we have the app secrets */
    check_secrets(&tclient, wolfssl_encryption_application, 32, 32);
    check_secrets(&tserver, wolfssl_encryption_application, 32, 32);
    /* verify client and server have the same secrets establishd */
    assert_secrets_EQ(&tclient, &tserver, wolfssl_encryption_handshake);
    assert_secrets_EQ(&tclient, &tserver, wolfssl_encryption_application);
    /* AEAD cipher should be known */
    AssertNotNull(wolfSSL_quic_get_aead(tclient.ssl));
    AssertNotNull(wolfSSL_quic_get_aead(tserver.ssl));
    /* What was negiotiated and is it the same? */
    AssertIntEQ(wolfSSL_get_peer_quic_transport_version(tclient.ssl),
                wolfSSL_get_peer_quic_transport_version(tserver.ssl));

    QuicTestContext_free(&tclient);
    QuicTestContext_free(&tserver);

    wolfSSL_CTX_free(ctx_c);
    wolfSSL_CTX_free(ctx_s);
    printf("    test_quic_server_hello: %s\n", (ret == 0)? passed : failed);

    return ret;
}

/* This has gotten a bit out of hand. */
#if (defined(OPENSSL_ALL) || (defined(OPENSSL_EXTRA) && \
    (defined(HAVE_STUNNEL) || defined(WOLFSSL_NGINX) || \
    defined(HAVE_LIGHTY) || defined(WOLFSSL_HAPROXY) || \
    defined(WOLFSSL_OPENSSH) || defined(HAVE_SBLIM_SFCB)))) \
    && defined(HAVE_ALPN) && defined(HAVE_SNI)
#define REALLY_HAVE_ALPN_AND_SNI
#else
#undef REALLY_HAVE_ALPN_AND_SNI
#endif

#ifdef REALLY_HAVE_ALPN_AND_SNI
struct stripe_buffer {
    char stripe[256];
};

static int inspect_SNI(WOLFSSL *ssl, int *ad, void *baton)
{
    struct stripe_buffer *stripe = (struct stripe_buffer *)baton;

    (void)ssl;
    *ad = 0;
    XSTRLCAT(stripe->stripe, "S", sizeof(stripe->stripe));
    return 0;
}

static int select_ALPN(WOLFSSL *ssl,
            const unsigned char **out,
            unsigned char *outlen,
            const unsigned char *in,
            unsigned int inlen,
            void *baton)
{
    struct stripe_buffer *stripe = (struct stripe_buffer *)baton;

    (void)ssl;
    (void)inlen;
    /* just select the first */
    *out = in + 1;
    *outlen = in[0];
    XSTRLCAT(stripe->stripe, "A", sizeof(stripe->stripe));
    return 0;
}

static int test_quic_alpn(int verbose) {
    WOLFSSL_CTX *ctx_c, *ctx_s;
    int ret = 0;
    QuicTestContext tclient, tserver;
    QuicConversation conv;
    struct stripe_buffer stripe;
    unsigned char alpn_protos[256];

    AssertNotNull(ctx_c = wolfSSL_CTX_new(wolfTLSv1_3_client_method()));
    AssertNotNull(ctx_s = wolfSSL_CTX_new(wolfTLSv1_3_server_method()));
    AssertTrue(wolfSSL_CTX_use_certificate_file(ctx_s, svrCertFile, WOLFSSL_FILETYPE_PEM));
    AssertTrue(wolfSSL_CTX_use_PrivateKey_file(ctx_s, svrKeyFile, WOLFSSL_FILETYPE_PEM));

    stripe.stripe[0] = '\0';
    wolfSSL_CTX_set_servername_callback(ctx_s, inspect_SNI);
    wolfSSL_CTX_set_servername_arg(ctx_s, &stripe);
    wolfSSL_CTX_set_alpn_select_cb(ctx_s, select_ALPN, &stripe);

    /* setup ssls */
    QuicTestContext_init(&tclient, ctx_c, "client", verbose);
    QuicTestContext_init(&tserver, ctx_s, "server", verbose);

    /* set SNI and ALPN callbacks on server side,
     * provide values on client side */
    wolfSSL_UseSNI(tclient.ssl, WOLFSSL_SNI_HOST_NAME,
                   "wolfssl.com", sizeof("wolfssl.com")-1);
    /* connect */
    QuicConversation_init(&conv, &tclient, &tserver);

    XSTRLCPY((char*)(alpn_protos + 1), "test", sizeof(alpn_protos));
    alpn_protos[0] = strlen("test");
    wolfSSL_set_alpn_protos(tclient.ssl, alpn_protos, 1 + strlen("test"));

    QuicConversation_do(&conv);
    AssertIntEQ(tclient.output.len, 0);
    AssertIntEQ(tserver.output.len, 0);

    /* SNI callback needs to be called before ALPN callback */
    AssertStrEQ(stripe.stripe, "SA");

    QuicTestContext_free(&tclient);
    QuicTestContext_free(&tserver);

    wolfSSL_CTX_free(ctx_c);
    wolfSSL_CTX_free(ctx_s);
    printf("    test_quic_alpn: %s\n", (ret == 0)? passed : failed);

    return ret;
}
#endif /* REALLY_HAVE_ALPN_AND_SNI */


#ifdef HAVE_SESSION_TICKET

static int test_quic_key_share(int verbose) {
    WOLFSSL_CTX *ctx_c, *ctx_s;
    int ret = 0;
    QuicTestContext tclient, tserver;
    QuicConversation conv;

    AssertNotNull(ctx_c = wolfSSL_CTX_new(wolfTLSv1_3_client_method()));
    AssertNotNull(ctx_s = wolfSSL_CTX_new(wolfTLSv1_3_server_method()));
    AssertTrue(wolfSSL_CTX_use_certificate_file(ctx_s, svrCertFile, WOLFSSL_FILETYPE_PEM));
    AssertTrue(wolfSSL_CTX_use_PrivateKey_file(ctx_s, svrKeyFile, WOLFSSL_FILETYPE_PEM));

    /* setup & handshake defaults */
    QuicTestContext_init(&tclient, ctx_c, "client", verbose);
    QuicTestContext_init(&tserver, ctx_s, "server", verbose);
    QuicConversation_init(&conv, &tclient, &tserver);
    QuicConversation_do(&conv);
    AssertStrEQ(conv.rec_log, "ClientHello:ServerHello:EncryptedExtension:"
        "Certificate:CertificateVerify:Finished:Finished:SessionTicket");
    QuicTestContext_free(&tclient);
    QuicTestContext_free(&tserver);

    /* setup & handshake, restricted groups. KEY_SHARE should use
     * the first configured group. */
    /*If that is supported by the server, expect a smooth handshake.*/
    QuicTestContext_init(&tclient, ctx_c, "client", verbose);
    QuicTestContext_init(&tserver, ctx_s, "server", verbose);
    AssertTrue(wolfSSL_set1_curves_list(tclient.ssl, "X25519:P-256")
               == WOLFSSL_SUCCESS);
    AssertTrue(wolfSSL_set1_curves_list(tserver.ssl, "X25519")
               == WOLFSSL_SUCCESS);
    QuicConversation_init(&conv, &tclient, &tserver);
    QuicConversation_do(&conv);
    AssertStrEQ(conv.rec_log,
        "ClientHello:ServerHello:EncryptedExtension:"
            "Certificate:CertificateVerify:Finished:Finished:SessionTicket");
    QuicTestContext_free(&tclient);
    QuicTestContext_free(&tserver);
    printf("    test_quic_key_share: priority ok\n");

    /* If group is not supported by server, expect HelloRetry */
    QuicTestContext_init(&tclient, ctx_c, "client", verbose);
    QuicTestContext_init(&tserver, ctx_s, "server", verbose);
    AssertTrue(wolfSSL_set1_curves_list(tclient.ssl, "X25519:P-256")
               == WOLFSSL_SUCCESS);
    AssertTrue(wolfSSL_set1_curves_list(tserver.ssl, "P-256")
               == WOLFSSL_SUCCESS);
    QuicConversation_init(&conv, &tclient, &tserver);
    QuicConversation_do(&conv);
    AssertStrEQ(conv.rec_log,
        "ClientHello:ServerHello:ClientHello:ServerHello:EncryptedExtension:"
            "Certificate:CertificateVerify:Finished:Finished:SessionTicket");
    QuicTestContext_free(&tclient);
    QuicTestContext_free(&tserver);
    printf("    test_quic_key_share: retry ok\n");

    /* If no group overlap, expect failure */
    QuicTestContext_init(&tclient, ctx_c, "client", verbose);
    QuicTestContext_init(&tserver, ctx_s, "server", verbose);
    AssertTrue(wolfSSL_set1_curves_list(tclient.ssl, "P-256")
               == WOLFSSL_SUCCESS);
    AssertTrue(wolfSSL_set1_curves_list(tserver.ssl, "X25519")
               == WOLFSSL_SUCCESS);
    QuicConversation_init(&conv, &tclient, &tserver);
    QuicConversation_fail(&conv);
    AssertIntEQ(wolfSSL_get_error(tserver.ssl, 0), SSL_ERROR_WANT_READ);
    AssertIntEQ(wolfSSL_get_error(tclient.ssl, 0), BAD_KEY_SHARE_DATA);
    QuicTestContext_free(&tclient);
    QuicTestContext_free(&tserver);
    printf("    test_quic_key_share: no match ok\n");

    wolfSSL_CTX_free(ctx_c);
    wolfSSL_CTX_free(ctx_s);
    printf("    test_quic_key_share: %s\n", (ret == 0)? passed : failed);
    return ret;
}

static int test_quic_resumption(int verbose) {
    WOLFSSL_CTX *ctx_c, *ctx_s;
    WOLFSSL_SESSION *session, *session_restored;
    int ret = 0;
    QuicTestContext tclient, tserver;
    QuicConversation conv;
    unsigned char session_buffer[16 * 1024], *session_data;
    const unsigned char *session_data2;
    unsigned int session_size;

    AssertNotNull(ctx_c = wolfSSL_CTX_new(wolfTLSv1_3_client_method()));
    AssertNotNull(ctx_s = wolfSSL_CTX_new(wolfTLSv1_3_server_method()));
    AssertTrue(wolfSSL_CTX_use_certificate_file(ctx_s, svrCertFile, WOLFSSL_FILETYPE_PEM));
    AssertTrue(wolfSSL_CTX_use_PrivateKey_file(ctx_s, svrKeyFile, WOLFSSL_FILETYPE_PEM));

    /* setup ssls */
    QuicTestContext_init(&tclient, ctx_c, "client", verbose);
    QuicTestContext_init(&tserver, ctx_s, "server", verbose);

    QuicConversation_init(&conv, &tclient, &tserver);
    /* run till end */
    QuicConversation_do(&conv);
    /* what have we seen? */
    AssertStrEQ(conv.rec_log, "ClientHello:ServerHello:EncryptedExtension:"
        "Certificate:CertificateVerify:Finished:Finished:SessionTicket");

    /* Should have received a session ticket, save the session
     * and also make a serialized/deserialized copy to check that persisting
     * a session works. */
    AssertTrue(tclient.ticket_len > 0);
    AssertNotNull(session = wolfSSL_get1_session(tclient.ssl));
    AssertTrue((session_size = wolfSSL_i2d_SSL_SESSION(session, NULL)) > 0);
    AssertTrue((size_t)session_size < sizeof(session_buffer));
    session_data2 = session_data = session_buffer;
    session_size = wolfSSL_i2d_SSL_SESSION(session, &session_data);
    session_restored = wolfSSL_d2i_SSL_SESSION(NULL, &session_data2, session_size);
    AssertNotNull(session_restored);

    QuicTestContext_free(&tserver);
    QuicTestContext_free(&tclient);

    /* Do a Session resumption with the session object */
    QuicTestContext_init(&tserver, ctx_s, "server", verbose);
    QuicTestContext_init(&tclient, ctx_c, "client_resume", verbose);
    AssertIntEQ(wolfSSL_set_session(tclient.ssl, session), WOLFSSL_SUCCESS);
    /* let them talk */
    QuicConversation_init(&conv, &tclient, &tserver);
    QuicConversation_do(&conv);
    /* this is what should happen. Look Ma, no certificate! */
    AssertStrEQ(conv.rec_log, "ClientHello:ServerHello:EncryptedExtension:"
        "Finished:Finished:SessionTicket");
    QuicTestContext_free(&tclient);
    QuicTestContext_free(&tserver);

    /* Do a Session resumption with the restored session object */
    QuicTestContext_init(&tserver, ctx_s, "server", verbose);
    QuicTestContext_init(&tclient, ctx_c, "client_resume_restored", verbose);
    AssertIntEQ(wolfSSL_set_session(tclient.ssl, session_restored), WOLFSSL_SUCCESS);
    /* let them talk */
    QuicConversation_init(&conv, &tclient, &tserver);
    QuicConversation_do(&conv);
    /* this is what should happen. Look Ma, no certificate! */
    AssertStrEQ(conv.rec_log, "ClientHello:ServerHello:EncryptedExtension:"
        "Finished:Finished:SessionTicket");
    QuicTestContext_free(&tclient);
    QuicTestContext_free(&tserver);

    {
        /* Do a Session resumption with a new server ctx */
        WOLFSSL_CTX *ctx_s2;
        AssertNotNull(ctx_s2 = wolfSSL_CTX_new(wolfTLSv1_3_server_method()));
        AssertTrue(wolfSSL_CTX_use_certificate_file(ctx_s2, eccCertFile, WOLFSSL_FILETYPE_PEM));
        AssertTrue(wolfSSL_CTX_use_PrivateKey_file(ctx_s2, eccKeyFile, WOLFSSL_FILETYPE_PEM));

        QuicTestContext_init(&tserver, ctx_s2, "server2", verbose);
        QuicTestContext_init(&tclient, ctx_c, "client_resume2", verbose);
        AssertIntEQ(wolfSSL_set_session(tclient.ssl, session_restored), WOLFSSL_SUCCESS);
        /* let them talk */
        QuicConversation_init(&conv, &tclient, &tserver);
        QuicConversation_do(&conv);
        AssertStrEQ(conv.rec_log, "ClientHello:ServerHello:EncryptedExtension:"
            "Certificate:CertificateVerify:Finished:Finished:SessionTicket");
        QuicTestContext_free(&tclient);
        QuicTestContext_free(&tserver);
        wolfSSL_CTX_free(ctx_s2);
    }

    wolfSSL_SESSION_free(session);
    wolfSSL_SESSION_free(session_restored);
    wolfSSL_CTX_free(ctx_c);
    wolfSSL_CTX_free(ctx_s);

    printf("    test_quic_resumption: %s\n", (ret == 0)? passed : failed);
    return ret;
}

#ifdef WOLFSSL_EARLY_DATA
static int test_quic_early_data(int verbose) {
    WOLFSSL_CTX *ctx_c, *ctx_s;
    int ret = 0;
    QuicTestContext tclient, tserver;
    QuicConversation conv;
    const byte early_data[] = "Nulla dies sine linea!";
    size_t ed_written;
    WOLFSSL_SESSION *session;
    unsigned int max_early_sz;

    AssertNotNull(ctx_c = wolfSSL_CTX_new(wolfTLSv1_3_client_method()));
    wolfSSL_CTX_UseSessionTicket(ctx_c);

    AssertNotNull(ctx_s = wolfSSL_CTX_new(wolfTLSv1_3_server_method()));
    AssertTrue(wolfSSL_CTX_use_certificate_file(ctx_s, svrCertFile, WOLFSSL_FILETYPE_PEM));
    AssertTrue(wolfSSL_CTX_use_PrivateKey_file(ctx_s, svrKeyFile, WOLFSSL_FILETYPE_PEM));

    /* setup ssls */
    QuicTestContext_init(&tclient, ctx_c, "client", verbose);
    QuicTestContext_init(&tserver, ctx_s, "server", verbose);
    wolfSSL_set_quic_early_data_enabled(tserver.ssl, 1);
    /* QUIC only allows 0xffffffff or 0x0 as values */
    AssertIntEQ(wolfSSL_get_max_early_data(tserver.ssl), UINT32_MAX);

    QuicConversation_init(&conv, &tclient, &tserver);
    /* run till end */
    QuicConversation_do(&conv);
    /* what have we seen? */
    AssertStrEQ(conv.rec_log, "ClientHello:ServerHello:EncryptedExtension:"
        "Certificate:CertificateVerify:Finished:Finished:SessionTicket");

    /* Should have received a session ticket, save the session */
    AssertTrue(tclient.ticket_len > 0);
    AssertNotNull(session = wolfSSL_get1_session(tclient.ssl));
    QuicTestContext_free(&tclient);
    QuicTestContext_free(&tserver);

    /* QUIC requires 0 or 0xffffffff as only allowed values.
     * Since we enabled early data in the server that created the session,
     * we need to see it here. */
    max_early_sz = wolfSSL_SESSION_get_max_early_data(session);
    AssertIntEQ(max_early_sz, UINT32_MAX);

    /* Do a Session resumption with the ticket */
    QuicTestContext_init(&tserver, ctx_s, "server", verbose);
    QuicTestContext_init(&tclient, ctx_c, "client", verbose);
    AssertIntEQ(wolfSSL_set_session(tclient.ssl, session), WOLFSSL_SUCCESS);
    /* enable early data -*/
    wolfSSL_set_quic_early_data_enabled(tserver.ssl, 1);
    /* client will send, and server will receive implicitly */
    QuicConversation_init(&conv, &tclient, &tserver);
    QuicConversation_start(&conv, early_data, sizeof(early_data), &ed_written);
    QuicConversation_do(&conv);
    AssertIntEQ(wolfSSL_get_early_data_status(tclient.ssl), WOLFSSL_EARLY_DATA_ACCEPTED);

    QuicTestContext_free(&tclient);
    QuicTestContext_free(&tserver);

    QuicTestContext_init(&tserver, ctx_s, "server", verbose);
    QuicTestContext_init(&tclient, ctx_c, "client", verbose);
    AssertIntEQ(wolfSSL_set_session(tclient.ssl, session), WOLFSSL_SUCCESS);
    /* client will send, and server will receive */
    QuicConversation_init(&conv, &tclient, &tserver);
    /* make QuicConversation_do() use wolfSSL_read_early_data() */
    conv.accept_early_data = 1;
    QuicConversation_start(&conv, early_data, sizeof(early_data), &ed_written);
    QuicConversation_do(&conv);
    AssertIntEQ(wolfSSL_get_early_data_status(tclient.ssl), WOLFSSL_EARLY_DATA_ACCEPTED);
    AssertIntEQ(conv.early_data_len, sizeof(early_data));
    AssertStrEQ(conv.early_data, (const char*)early_data);

    QuicTestContext_free(&tclient);
    QuicTestContext_free(&tserver);

    wolfSSL_SESSION_free(session);
    wolfSSL_CTX_free(ctx_c);
    wolfSSL_CTX_free(ctx_s);
    printf("    test_quic_early_data: %s\n", (ret == 0)? passed : failed);

    return ret;
}
#endif /* WOLFSSL_EARLY_DATA */

static int new_session_cb(WOLFSSL *ssl, WOLFSSL_SESSION *session)
{
    QuicTestContext *ctx = (QuicTestContext*)wolfSSL_get_app_data(ssl);
    byte *data;
    int ret = 0;
    int sz;

    sz = wolfSSL_i2d_SSL_SESSION(session, NULL);
    if (sz <= 0) {
        printf("[%s] session serialization error: %d <- ", ctx->name, sz);
        return sz;
    }
    if ((size_t)sz > sizeof(ctx->session)) {
        printf("[%s] session serialization too large: %d <- ", ctx->name, sz);
        return -1;
    }
    data = ctx->session;
    ctx->session_len = wolfSSL_i2d_SSL_SESSION(session, &data);
    if (ctx->verbose) {
        printf("[%s]", ctx->name);
        dump_buffer(" new SESSION", ctx->session, ctx->session_len, 4);
    }
    return ret;
}

static int test_quic_session_export(int verbose)
{
    WOLFSSL_CTX *ctx_c, *ctx_s;
    WOLFSSL_SESSION *session = NULL;
    int ret = 0;
    QuicTestContext tclient, tserver;
    QuicConversation conv;
    byte session_data[16*1024];
    const byte *bp;
    word32 session_len;

    AssertNotNull(ctx_c = wolfSSL_CTX_new(wolfTLSv1_3_client_method()));
    AssertNotNull(ctx_s = wolfSSL_CTX_new(wolfTLSv1_3_server_method()));
    AssertTrue(wolfSSL_CTX_use_certificate_file(ctx_s, svrCertFile, WOLFSSL_FILETYPE_PEM));
    AssertTrue(wolfSSL_CTX_use_PrivateKey_file(ctx_s, svrKeyFile, WOLFSSL_FILETYPE_PEM));

    /* Uses CTX session callback for new sessions */
    wolfSSL_CTX_sess_set_new_cb(ctx_c, new_session_cb);

    /* setup ssls */
    QuicTestContext_init(&tclient, ctx_c, "client", verbose);
    QuicTestContext_init(&tserver, ctx_s, "server", verbose);

    QuicConversation_init(&conv, &tclient, &tserver);
    /* run till end */
    QuicConversation_do(&conv);
    /* what have we seen? */
    AssertStrEQ(conv.rec_log, "ClientHello:ServerHello:EncryptedExtension:Certificate:CertificateVerify:Finished:Finished:SessionTicket");

    /* Should have received a session, save it */
    AssertTrue(tclient.session_len > 0);
    memcpy(session_data, tclient.session, tclient.session_len);
    session_len = tclient.session_len;
    if (verbose)
        dump_buffer("copied SESSION", session_data, session_len, 0);

    QuicTestContext_free(&tserver);
    QuicTestContext_free(&tclient);

    /* Do a Session resumption with the ticket */
    QuicTestContext_init(&tserver, ctx_s, "server", verbose);
    QuicTestContext_init(&tclient, ctx_c, "client_resume", verbose);
    bp = session_data;
    AssertNotNull(session = wolfSSL_d2i_SSL_SESSION(NULL, &bp, session_len));
    AssertIntEQ(wolfSSL_set_session(tclient.ssl, session), WOLFSSL_SUCCESS);
    wolfSSL_SESSION_free(session);

    /* let them talk */
    QuicConversation_init(&conv, &tclient, &tserver);
    QuicConversation_do(&conv);
    /* this is what should happen. Look Ma, no certificate! */
    AssertStrEQ(conv.rec_log, "ClientHello:ServerHello:EncryptedExtension:Finished:Finished:SessionTicket");

    QuicTestContext_free(&tclient);
    QuicTestContext_free(&tserver);

    wolfSSL_CTX_free(ctx_c);
    wolfSSL_CTX_free(ctx_s);

    printf("    test_quic_session_export: %s\n", (ret == 0)? passed : failed);
    return ret;
}
#endif /* WOLFSSL_SESSION_EXPORT */

#endif /* WOLFSSL_QUIC */


int QuicTest(void)
{
    int ret = 0;
#ifdef WOLFSSL_QUIC
    int verbose = 0;
    printf(" Begin QUIC Tests\n");

    if ((ret = test_set_quic_method()) != 0) goto leave;
    if ((ret = test_provide_quic_data()) != 0) goto leave;
    if ((ret = test_quic_crypt()) != 0) goto leave;
    if ((ret = test_quic_client_hello(verbose)) != 0) goto leave;
    if ((ret = test_quic_server_hello(verbose)) != 0) goto leave;
#ifdef REALLY_HAVE_ALPN_AND_SNI
    if ((ret = test_quic_alpn(verbose)) != 0) goto leave;
#endif /* REALLY_HAVE_ALPN_AND_SNI */
#ifdef HAVE_SESSION_TICKET
    if ((ret = test_quic_key_share(verbose)) != 0) goto leave;
    if ((ret = test_quic_resumption(verbose)) != 0) goto leave;
#ifdef WOLFSSL_EARLY_DATA
    if ((ret = test_quic_early_data(verbose)) != 0) goto leave;
#endif /* WOLFSSL_EARLY_DATA */
    if ((ret = test_quic_session_export(verbose)) != 0) goto leave;
#endif /* HAVE_SESSION_TICKET */

leave:
    if (ret != 0)
        printf("  FAILED: some tests did not pass.\n");
    printf(" End QUIC Tests\n");
#endif
    return ret;
}
