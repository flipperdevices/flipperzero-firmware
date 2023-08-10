/* quic.c
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


  /* Name change compatibility layer no longer needs to be included here */

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>
#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

#ifndef WOLFCRYPT_ONLY
#ifdef WOLFSSL_QUIC

#include <wolfssl/error-ssl.h>
#include <wolfssl/ssl.h>
#include <wolfssl/internal.h>

#include <wolfssl/openssl/buffer.h>
#include <wolfssl/openssl/ecdsa.h>
#include <wolfssl/openssl/evp.h>
#include <wolfssl/openssl/kdf.h>


static int qr_length(const uint8_t *data, size_t len)
{
    word32 rlen;
    if (len < 4) {
        return 0;
    }
    c24to32(&data[1], &rlen);
    return (int)rlen + 4;
}

static void quic_record_free(WOLFSSL *ssl, QuicRecord *r)
{
    (void)ssl;
    if (r->data) {
        ForceZero(r->data, r->capacity);
        XFREE(r->data, ssl->heap, DYNAMIC_TYPE_TMP_BUFFER);
    }
    XFREE(r, ssl->heap, DYNAMIC_TYPE_TMP_BUFFER);
}


static QuicRecord *quic_record_make(WOLFSSL *ssl,
                                    WOLFSSL_ENCRYPTION_LEVEL level,
                                    const uint8_t *data, size_t len)
{
    QuicRecord *qr;

    qr = (QuicRecord*)XMALLOC(sizeof(*qr), ssl->heap, DYNAMIC_TYPE_TMP_BUFFER);
    if (qr) {
        memset(qr, 0, sizeof(*qr));
        qr->level = level;
        if (level == wolfssl_encryption_early_data) {
            qr->capacity = qr->len = (word32)len;
        }
        else {
            qr->capacity = qr->len = qr_length(data, len);
        }
        if (qr->capacity == 0) {
            qr->capacity = 2*1024;
        }
        qr->data = (uint8_t*)XMALLOC(qr->capacity, ssl->heap,
                                     DYNAMIC_TYPE_TMP_BUFFER);
        if (!qr->data) {
            quic_record_free(ssl, qr);
            return NULL;
        }
    }
    return qr;
}

static int quic_record_complete(QuicRecord *r)
{
    return r->len && r->end >= r->len;
}

static int quic_record_done(QuicRecord *r)
{
    return r->len && r->end >= r->len && r->start >= r->end;
}

static int quic_record_append(WOLFSSL *ssl, QuicRecord *qr, const uint8_t *data,
                              size_t len, size_t *pconsumed)
{
    size_t missing, consumed = 0;
    int ret = WOLFSSL_SUCCESS;

    (void)ssl;
    if (!qr->len && len) {
        missing = 4 - qr->end;
        if (len < missing) {
            XMEMCPY(qr->data + qr->end, data, len);
            qr->end += len;
            consumed = len;
            goto cleanup; /* len consumed, but qr->len still unknown */
        }
        XMEMCPY(qr->data + qr->end, data, missing);
        qr->end += missing;
        len -= missing;
        data += missing;
        consumed = missing;

        qr->len = qr_length(qr->data, qr->end);
        if (qr->len > qr->capacity) {
            uint8_t *ndata = (uint8_t*)XREALLOC(qr->data, qr->len, ssl->heap,
                                                DYNAMIC_TYPE_TMP_BUFFER);
            if (!ndata) {
                ret = WOLFSSL_FAILURE;
                goto cleanup;
            }
            qr->data = ndata;
            qr->capacity = qr->len;
        }
    }

    if (quic_record_complete(qr) || len == 0) {
        return 0;
    }

    missing = qr->len - qr->end;
    if (len > missing) {
        len = missing;
    }
    XMEMCPY(qr->data + qr->end, data, len);
    qr->end += len;
    consumed += len;

cleanup:
    *pconsumed = (ret == WOLFSSL_SUCCESS) ? consumed : 0;
    return ret;
}


static word32 add_rec_header(byte* output, word32 length, int type)
{
    RecordLayerHeader* rl;

    /* record layer header */
    rl = (RecordLayerHeader*)output;
    if (rl == NULL) {
        return 0;
    }
    rl->type    = type;
    rl->pvMajor = SSLv3_MAJOR;
    rl->pvMinor = TLSv1_2_MINOR;
    c16toa((word16)length, rl->length);
    return RECORD_HEADER_SZ;
}

static word32 quic_record_transfer(QuicRecord* qr, byte* buf, word32 sz)
{
    word32 len = qr->end - qr->start;
    word32 offset = 0;
    word16 rlen;

    if (len <= 0) {
        return 0;
    }
    if (qr->rec_hdr_remain == 0) {
        /* start a new TLS record */
        rlen = (qr->len <= (word32)MAX_RECORD_SIZE) ?
                qr->len : (word32)MAX_RECORD_SIZE;
        offset += add_rec_header(buf, rlen,
                                 (qr->level == wolfssl_encryption_early_data) ?
                                  application_data : handshake);
        qr->rec_hdr_remain = rlen;
        sz -= offset;
    }
    if (len > qr->rec_hdr_remain) {
        len = qr->rec_hdr_remain;
    }
    if (len > sz) {
        len = sz;
    }
    if (len > 0) {
        XMEMCPY(buf + offset, qr->data + qr->start, len);
        qr->start += len;
        qr->rec_hdr_remain -= len;
    }
    return len + offset;
}


const QuicTransportParam* QuicTransportParam_new(const uint8_t* data,
                                                 size_t len, void* heap)
{
    QuicTransportParam* tp;

    if (len > 65353) return NULL;
    tp = (QuicTransportParam*)XMALLOC(sizeof(*tp), heap, DYNAMIC_TYPE_TLSX);
    if (!tp) return NULL;
    tp->data = (uint8_t*)XMALLOC(len, heap, DYNAMIC_TYPE_TLSX);
    if (!tp->data) {
        XFREE(tp, heap, DYNAMIC_TYPE_TLSX);
        return NULL;
    }
    XMEMCPY((uint8_t*)tp->data, data, len);
    tp->len = len;
    return tp;
}

const QuicTransportParam* QuicTransportParam_dup(const QuicTransportParam* tp,
                                                 void* heap)
{
    QuicTransportParam* tp2;
    tp2 = (QuicTransportParam*)XMALLOC(sizeof(*tp2), heap, DYNAMIC_TYPE_TLSX);
    if (!tp2) return NULL;
    tp2->data = (uint8_t*)XMALLOC(tp->len, heap, DYNAMIC_TYPE_TLSX);
    if (!tp2->data) {
        XFREE(tp2, heap, DYNAMIC_TYPE_TLSX);
        return NULL;
    }
    XMEMCPY((uint8_t*)tp2->data, tp->data, tp->len);
    tp2->len = tp->len;
    return tp2;
}

void QuicTransportParam_free(const QuicTransportParam* tp, void* heap)
{
    (void)heap;
    if (tp) {
        if (tp->data) XFREE((uint8_t*)tp->data, heap, DYNAMIC_TYPE_TLSX);
        XFREE((void*)tp, heap, DYNAMIC_TYPE_TLSX);
    }
}


void wolfSSL_quic_clear(WOLFSSL* ssl)
{
    QuicEncData* qd;

    /* keep
     * - ssl->quic.transport_local
     * - ssl->quic.method
     * - ssl->quic.transport_version
     * reset/free everything else
     */
    if (ssl->quic.transport_peer) {
        QTP_FREE(ssl->quic.transport_peer, ssl->heap);
        ssl->quic.transport_peer = NULL;
    }
    if (ssl->quic.transport_peer_draft) {
        QTP_FREE(ssl->quic.transport_peer_draft, ssl->heap);
        ssl->quic.transport_peer_draft = NULL;
    }
    ssl->quic.enc_level_write = wolfssl_encryption_initial;
    ssl->quic.enc_level_latest_recvd = wolfssl_encryption_initial;

    while ((qd = ssl->quic.input_head)) {
        ssl->quic.input_head = qd->next;
        quic_record_free(ssl, qd);
    }
    ssl->quic.input_tail = NULL;
    ssl->quic.output_rec_remain = 0;

    if (ssl->quic.scratch) {
        quic_record_free(ssl, ssl->quic.scratch);
        ssl->quic.scratch = NULL;
    }
}


void wolfSSL_quic_free(WOLFSSL* ssl)
{
    wolfSSL_quic_clear(ssl);
    if (ssl->quic.transport_local) {
        QTP_FREE(ssl->quic.transport_local, ssl->heap);
        ssl->quic.transport_local = NULL;
    }

    ssl->quic.method = NULL;
}


static int ctx_check_quic_compat(const WOLFSSL_CTX* ctx)
{
    WOLFSSL_ENTER("ctx_check_quic_compat");
    if (ctx->method->version.major != SSLv3_MAJOR
        || ctx->method->version.minor != TLSv1_3_MINOR
        || (ctx->method->downgrade && ctx->minDowngrade < TLSv1_3_MINOR)) {
        WOLFSSL_MSG_EX("ctx not quic compatible: vmajor=%d, vminor=%d, downgrade=%d",
                       ctx->method->version.major,
                       ctx->method->version.minor,
                       ctx->method->downgrade
                      );
        return WOLFSSL_FAILURE;
    }
    return WOLFSSL_SUCCESS;
}

static int check_method_sanity(const WOLFSSL_QUIC_METHOD* m)
{
    WOLFSSL_ENTER("check_method_sanity");
    if (m && m->set_encryption_secrets
        && m->add_handshake_data
        && m->flush_flight
        && m->send_alert) {
        return WOLFSSL_SUCCESS;
    }
    return WOLFSSL_FAILURE;
}

int wolfSSL_CTX_set_quic_method(WOLFSSL_CTX* ctx,
                                const WOLFSSL_QUIC_METHOD* quic_method)
{
    WOLFSSL_ENTER("wolfSSL_CTX_set_quic_method");
    if (ctx_check_quic_compat(ctx) != WOLFSSL_SUCCESS
        || check_method_sanity(quic_method) != WOLFSSL_SUCCESS) {
        return WOLFSSL_FAILURE;
    }
    ctx->quic.method = quic_method;
    return WOLFSSL_SUCCESS;
}


int wolfSSL_set_quic_method(WOLFSSL* ssl,
                            const WOLFSSL_QUIC_METHOD* quic_method)
{
    WOLFSSL_ENTER("wolfSSL_set_quic_method");
    if (ctx_check_quic_compat(ssl->ctx) != WOLFSSL_SUCCESS
        || check_method_sanity(quic_method) != WOLFSSL_SUCCESS) {
        return WOLFSSL_FAILURE;
    }
    ssl->quic.method = quic_method;
    return WOLFSSL_SUCCESS;
}


int wolfSSL_is_quic(WOLFSSL* ssl)
{
    return WOLFSSL_IS_QUIC(ssl);
}


WOLFSSL_ENCRYPTION_LEVEL wolfSSL_quic_read_level(const WOLFSSL* ssl)
{
    return ssl->quic.enc_level_read;
}


WOLFSSL_ENCRYPTION_LEVEL wolfSSL_quic_write_level(const WOLFSSL* ssl)
{
    return ssl->quic.enc_level_write;
}


int wolfSSL_set_quic_transport_params(WOLFSSL* ssl,
                                      const uint8_t* params,
                                      size_t params_len)
{
    const QuicTransportParam* tp;
    int ret = WOLFSSL_SUCCESS;

    WOLFSSL_ENTER("wolfSSL_set_quic_transport_params");

    if (!params || params_len == 0) {
        tp = NULL;
    }
    else {
        tp = QuicTransportParam_new(params, params_len, ssl->heap);
        if (!tp) {
            ret = WOLFSSL_FAILURE;
            goto cleanup;
        }
    }
    if (ssl->quic.transport_local)
        QTP_FREE(ssl->quic.transport_local, ssl->heap);
    ssl->quic.transport_local = tp;

cleanup:
    WOLFSSL_LEAVE("wolfSSL_set_quic_transport_params", ret);
    return ret;
}


void wolfSSL_get_peer_quic_transport_params(const WOLFSSL* ssl,
                                            const uint8_t** out_params,
                                            size_t* out_params_len)
{
    const QuicTransportParam* tp = ssl->quic.transport_peer ?
        ssl->quic.transport_peer : ssl->quic.transport_peer_draft;

    *out_params = tp ? tp->data : NULL;
    *out_params_len = tp ? tp->len : 0;
}


int wolfSSL_get_peer_quic_transport_version(const WOLFSSL* ssl)
{
    return ssl->quic.transport_peer ?
        TLSX_KEY_QUIC_TP_PARAMS : (ssl->quic.transport_peer_draft ?
        TLSX_KEY_QUIC_TP_PARAMS : -1);
}


void wolfSSL_set_quic_use_legacy_codepoint(WOLFSSL* ssl, int use_legacy)
{
    ssl->quic.transport_version = use_legacy ? TLSX_KEY_QUIC_TP_PARAMS_DRAFT
        : TLSX_KEY_QUIC_TP_PARAMS;
}

void wolfSSL_set_quic_transport_version(WOLFSSL* ssl, int version)
{
    if (version == TLSX_KEY_QUIC_TP_PARAMS
        || version == TLSX_KEY_QUIC_TP_PARAMS_DRAFT
        || !version) {
        ssl->quic.transport_version = version;
    }
    else {
        WOLFSSL_MSG("wolfSSL_set_quic_transport_version: invalid version");
    }
}


int wolfSSL_get_quic_transport_version(const WOLFSSL* ssl)
{
    return ssl->quic.transport_version;
}


int wolfSSL_quic_add_transport_extensions(WOLFSSL* ssl, int msg_type)
{
    /* RFC 9001, ch. 8.2: "The quic_transport_parameters extension is carried
     * in the ClientHello and the EncryptedExtensions messages during the
     * handshake. Endpoints MUST send the quic_transport_parameters extension;"
     * Which means, at least one. There can be more to signal compatibility to
     * older/newer versions.
     */
    int ret = 0, is_resp = (msg_type == encrypted_extensions);

    if (ssl->quic.transport_local == NULL) {
        return QUIC_TP_MISSING_E;
    }

    if (is_resp) {
        /* server response: time to decide which version to use */
        if (ssl->quic.transport_peer && ssl->quic.transport_peer_draft) {
            if (ssl->quic.transport_version == TLSX_KEY_QUIC_TP_PARAMS_DRAFT) {
                ret = TLSX_QuicTP_Use(ssl,
                                      TLSX_KEY_QUIC_TP_PARAMS_DRAFT, is_resp);
                QTP_FREE(ssl->quic.transport_peer, ssl->heap);
                ssl->quic.transport_peer = NULL;
            }
            else {
                ret = TLSX_QuicTP_Use(ssl, TLSX_KEY_QUIC_TP_PARAMS, is_resp);
                QTP_FREE(ssl->quic.transport_peer_draft,
                                        ssl->heap);
                ssl->quic.transport_peer_draft = NULL;
            }
        }
        else {
            if (ssl->quic.transport_version == TLSX_KEY_QUIC_TP_PARAMS_DRAFT
                && ssl->quic.transport_peer_draft) {
                ret = TLSX_QuicTP_Use(ssl, TLSX_KEY_QUIC_TP_PARAMS_DRAFT,
                                      is_resp);
            }
            else if (ssl->quic.transport_peer) {
                ret = TLSX_QuicTP_Use(ssl, TLSX_KEY_QUIC_TP_PARAMS, is_resp);
            }
            else {
                /* no match, send none, will let the client fail */
            }
        }
    }
    else {
        /* client hello */
        if (ssl->quic.transport_version == 0) {
            /* not being set to a particular id, we send both draft+v1 */
            ret = TLSX_QuicTP_Use(ssl, TLSX_KEY_QUIC_TP_PARAMS, is_resp)
                || TLSX_QuicTP_Use(ssl, TLSX_KEY_QUIC_TP_PARAMS_DRAFT, is_resp);
        }
        else {
            /* otherwise, send the version configured */
            ret = TLSX_QuicTP_Use(ssl, (TLSX_Type)ssl->quic.transport_version,
                                  is_resp);
        }
    }
    return ret;
}


#define QUIC_HS_FLIGHT_LIMIT_DEFAULT      (16*  1024)

size_t wolfSSL_quic_max_handshake_flight_len(const WOLFSSL* ssl,
                                             WOLFSSL_ENCRYPTION_LEVEL level)
{
    switch (level) {
        case wolfssl_encryption_initial:
        case wolfssl_encryption_application:
                return QUIC_HS_FLIGHT_LIMIT_DEFAULT;
        case wolfssl_encryption_early_data:
            return 0; /* QUIC does not send at this level */
        case wolfssl_encryption_handshake:
            /* during handshake itself, certificates may be exchanged which
             * exceed our default limit, advise a higher limit one.
             */
            if (ssl->options.side == WOLFSSL_SERVER_END) {
                if (ssl->options.verifyPeer
                    && MAX_CERTIFICATE_SZ > QUIC_HS_FLIGHT_LIMIT_DEFAULT)
                    return MAX_CERTIFICATE_SZ;
            }
            else {
                /* clients may receive the server cert chain
                 */
                if (2*MAX_CERTIFICATE_SZ > QUIC_HS_FLIGHT_LIMIT_DEFAULT)
                    return 2*MAX_CERTIFICATE_SZ;
            }
            return QUIC_HS_FLIGHT_LIMIT_DEFAULT;
    }
    return 0;
}


#ifdef WOLFSSL_EARLY_DATA
void wolfSSL_set_quic_early_data_enabled(WOLFSSL* ssl, int enabled)
{
    /* This only has effect on server and when the handshake has
     * not started yet.
     * This function is part of the quictls/openssl API and does
     * not return any error, sadly. So we just ignore any
     * unsuccessful use. But we can produce some warnings.
     */
    if (!WOLFSSL_IS_QUIC(ssl)) {
        WOLFSSL_MSG("wolfSSL_set_quic_early_data_enabled: not a QUIC SSL");
    }
    else if (ssl->options.handShakeState != NULL_STATE) {
        WOLFSSL_MSG("wolfSSL_set_quic_early_data_enabled: handshake started");
    }
    else {
        wolfSSL_set_max_early_data(ssl, enabled ? UINT32_MAX : 0);
    }
}
#endif /* WOLFSSL_EARLY_DATA */

int wolfSSL_quic_do_handshake(WOLFSSL* ssl)
{
    int ret = WOLFSSL_SUCCESS;

    WOLFSSL_ENTER("wolfSSL_quic_do_handshake");

    if (!wolfSSL_is_quic(ssl)) {
        WOLFSSL_MSG("WOLFSSL_QUIC_DO_HANDSHAKE not a QUIC SSL");
        ret = WOLFSSL_FAILURE;
        goto cleanup;
    }

    while (ssl->options.handShakeState != HANDSHAKE_DONE) {
        /* Peculiar: do_handshake() is successful, but the state
         * indicates that we are not DONE. This seems to happen
         * when resuming sessions and an EARLY_DATA indicator
         * is presented by the client.
         * Theory: wolfSSL expects the APP to read the early data
         * and silently continues the handshake when the EndOfEarlyData
         * and the client Finished arrives.
         * This confuses the QUIC state handling.
         */
#ifdef WOLFSSL_EARLY_DATA
        if (ssl->options.maxEarlyDataSz) {
            byte tmpbuffer[256];
            int len;

            if (ssl->options.side == WOLFSSL_CLIENT_END) {
                if (ssl->options.resuming) {
                    ret = wolfSSL_write_early_data(ssl, tmpbuffer, 0, &len);
                }
            }
            else {
                ret = wolfSSL_read_early_data(ssl, tmpbuffer,
                                              sizeof(tmpbuffer), &len);
                if (ret < 0 && ssl->error == ZERO_RETURN) {
                    /* this is expected, since QUIC handles the actual early
                     * data separately. */
                    ret = WOLFSSL_SUCCESS;
                }
            }
            if (ret < 0) {
                goto cleanup;
            }
        }
#endif /* WOLFSSL_EARLY_DATA */

        ret = wolfSSL_SSL_do_handshake_internal(ssl);
        if (ret <= 0)
            goto cleanup;
    }

cleanup:
    if (ret <= 0
        && ssl->options.handShakeState == HANDSHAKE_DONE
        && (ssl->error == ZERO_RETURN || ssl->error == WANT_READ)) {
        ret = WOLFSSL_SUCCESS;
    }
    if (ret == WOLFSSL_SUCCESS) {
        ssl->error = WOLFSSL_ERROR_NONE;
    }
    WOLFSSL_LEAVE("wolfSSL_quic_do_handshake", ret);
    return ret;
}

int wolfSSL_quic_read_write(WOLFSSL* ssl)
{
    int ret = WOLFSSL_SUCCESS;

    WOLFSSL_ENTER("wolfSSL_quic_read_write");

    if (!wolfSSL_is_quic(ssl)) {
        WOLFSSL_MSG("WOLFSSL_QUIC_READ_WRITE not a QUIC SSL");
        ret = WOLFSSL_FAILURE;
        goto cleanup;
    }

    if (ssl->options.handShakeState != HANDSHAKE_DONE) {
        ret = wolfSSL_quic_do_handshake(ssl);
        if (ret != WOLFSSL_SUCCESS)
            goto cleanup;
    }

    ret = wolfSSL_process_quic_post_handshake(ssl);

cleanup:
    WOLFSSL_LEAVE("wolfSSL_quic_read_write", ret);
    return ret;
}

int wolfSSL_process_quic_post_handshake(WOLFSSL* ssl)
{
    int ret = WOLFSSL_SUCCESS, nret;

    WOLFSSL_ENTER("wolfSSL_process_quic_post_handshake");

    if (!wolfSSL_is_quic(ssl)) {
        WOLFSSL_MSG("WOLFSSL_QUIC_POST_HS not a QUIC SSL");
        ret = WOLFSSL_FAILURE;
        goto cleanup;
    }

    if (ssl->options.handShakeState != HANDSHAKE_DONE) {
        WOLFSSL_MSG("WOLFSSL_QUIC_POST_HS handshake is not done yet");
        ret = WOLFSSL_FAILURE;
        goto cleanup;
    }

    while (ssl->quic.input_head != NULL
           || ssl->buffers.inputBuffer.length > 0) {
        if ((nret = ProcessReply(ssl)) < 0) {
            ret = nret;
            break;
        }
    }
    while (ssl->buffers.outputBuffer.length > 0) {
        SendBuffered(ssl);
    }

cleanup:
    WOLFSSL_LEAVE("wolfSSL_process_quic_post_handshake", ret);
    return ret;
}


int wolfSSL_provide_quic_data(WOLFSSL* ssl, WOLFSSL_ENCRYPTION_LEVEL level,
                              const uint8_t* data, size_t len)
{
    int ret = WOLFSSL_SUCCESS;
    size_t l;

    WOLFSSL_ENTER("wolfSSL_provide_quic_data");
    if (!wolfSSL_is_quic(ssl)) {
        WOLFSSL_MSG("WOLFSSL_QUIC_PROVIDE_DATA not a QUIC SSL");
        ret = WOLFSSL_FAILURE;
        goto cleanup;
    }

    if (level < wolfSSL_quic_read_level(ssl)
        || (ssl->quic.input_tail && level < ssl->quic.input_tail->level)
        || level < ssl->quic.enc_level_latest_recvd) {
        WOLFSSL_MSG("WOLFSSL_QUIC_PROVIDE_DATA wrong encryption level");
        ret = WOLFSSL_FAILURE;
        goto cleanup;
    }

    while (len > 0) {
        if (ssl->quic.scratch) {
            if (ssl->quic.scratch->level != level) {
                WOLFSSL_MSG("WOLFSSL_QUIC_PROVIDE_DATA wrong encryption level");
                ret = WOLFSSL_FAILURE;
                goto cleanup;
            }

            ret = quic_record_append(ssl, ssl->quic.scratch, data, len, &l);
            if (ret != WOLFSSL_SUCCESS) {
                goto cleanup;
            }
            data += l;
            len -= l;
            if (quic_record_complete(ssl->quic.scratch)) {
                if (ssl->quic.input_tail) {
                    ssl->quic.input_tail->next = ssl->quic.scratch;
                    ssl->quic.input_tail = ssl->quic.scratch;
                }
                else {
                    ssl->quic.input_head = ssl->quic.input_tail =
                        ssl->quic.scratch;
                }
                ssl->quic.scratch = NULL;
            }
        }
        else {
            /* start of next record with all bytes for the header */
            ssl->quic.scratch = quic_record_make(ssl, level, data, len);
            if (!ssl->quic.scratch) {
                ret = WOLFSSL_FAILURE;
                goto cleanup;
            }
        }
    }

    ssl->quic.enc_level_latest_recvd = level;

cleanup:
    WOLFSSL_LEAVE("wolfSSL_provide_quic_data", ret);
    return ret;
}


/* Called internally when SSL wants a certain amount of input. */
int wolfSSL_quic_receive(WOLFSSL* ssl, byte* buf, word32 sz)
{
    word32 n = 0;
    int transferred = 0;

    WOLFSSL_ENTER("wolfSSL_quic_receive");
    while (sz > 0) {
        n = 0;
        if (ssl->quic.input_head) {
            n = quic_record_transfer(ssl->quic.input_head, buf, sz);
            if (quic_record_done(ssl->quic.input_head)) {
                QuicRecord* qr = ssl->quic.input_head;
                ssl->quic.input_head = qr->next;
                if (!qr->next) {
                    ssl->quic.input_tail = NULL;
                }
                quic_record_free(ssl, qr);
            }
        }

        if (n == 0) {
            if (transferred > 0) {
                goto cleanup;
            }
            ssl->error = transferred = WANT_READ;
            goto cleanup;
        }
        sz -= n;
        buf += n;
        transferred += n;
    }
cleanup:
    WOLFSSL_LEAVE("wolfSSL_quic_receive", transferred);
    return transferred;
}

/**
 * We need to forward the HANDSHAKE messages to the QUIC protocol stack
 * via ssl->quic.method->add_handshake_data().
 * The messages in the output buffer are unencrypted TLS records. We need
 * to forward the content of those records.
 */
static int wolfSSL_quic_send_internal(WOLFSSL* ssl)
{
    int ret = 0, aret;
    size_t len;
    RecordLayerHeader* rl;
    word16 rlen;
    word32 idx, length;
    byte* output;

    WOLFSSL_ENTER("wolfSSL_quic_send");

    idx = ssl->buffers.outputBuffer.idx;
    length = ssl->buffers.outputBuffer.length;
    output = ssl->buffers.outputBuffer.buffer + idx;
    while (length > 0) {
        if (ssl->quic.output_rec_remain > 0) {
            len = ssl->quic.output_rec_remain;
            if (len > length) {
                len = length;
            }

            aret = ssl->quic.method->add_handshake_data(ssl,
                ssl->quic.output_rec_level, (const uint8_t*)output, len);
            if (aret != 1) {
                /* The application has an error. General disaster. */
                WOLFSSL_MSG("WOLFSSL_QUIC_SEND application failed");
                ret = FWRITE_ERROR;
                goto cleanup;
            }
            output += len;
            length -= len;
            ssl->quic.output_rec_remain -= len;
        }
        else {
            /* at start of a TLS Record */
            rl = (RecordLayerHeader*)output;
            ato16(rl->length, &rlen);
            output += RECORD_HEADER_SZ;
            length -= RECORD_HEADER_SZ;
            ssl->quic.output_rec_remain = rlen;
            ssl->quic.output_rec_level = ssl->quic.enc_level_write;
            if (rl->type == application_data) {
                if (ssl->options.handShakeState != HANDSHAKE_DONE) {
                    ssl->quic.output_rec_level = wolfssl_encryption_early_data;
                }
                else {
                    WOLFSSL_MSG("WOLFSSL_QUIC_SEND app data after handshake");
                    ret = FWRITE_ERROR;
                    goto cleanup;
                }
            }
        }
    }

    ssl->buffers.outputBuffer.idx = 0;
    ssl->buffers.outputBuffer.length = 0;

cleanup:
    WOLFSSL_LEAVE("wolfSSL_quic_send", ret);
    return ret;
}

int wolfSSL_quic_send(WOLFSSL* ssl)
{
    return wolfSSL_quic_send_internal(ssl);
}

int wolfSSL_quic_forward_secrets(WOLFSSL* ssl, int ktype, int side)
{
    const uint8_t* rx_secret = NULL, *tx_secret = NULL;
    WOLFSSL_ENCRYPTION_LEVEL level;
    int ret = 0;

    WOLFSSL_ENTER("wolfSSL_quic_forward_secrets");
    switch (ktype) {
        case early_data_key:
            level = wolfssl_encryption_early_data;
            break;
        case handshake_key:
            level = wolfssl_encryption_handshake;
            break;
        case traffic_key:
            FALL_THROUGH;
        case update_traffic_key:
            level = wolfssl_encryption_application;
            break;
        case no_key:
            FALL_THROUGH;
        default:
            /* ignore */
            goto cleanup;
    }

    if (side == ENCRYPT_AND_DECRYPT_SIDE || side == ENCRYPT_SIDE_ONLY) {
        tx_secret = (ssl->options.side == WOLFSSL_CLIENT_END) ?
            ssl->clientSecret : ssl->serverSecret;
    }
    if (side == ENCRYPT_AND_DECRYPT_SIDE || side == DECRYPT_SIDE_ONLY) {
        rx_secret = (ssl->options.side == WOLFSSL_CLIENT_END) ?
            ssl->serverSecret : ssl->clientSecret;
    }

    if (!tx_secret && !rx_secret) {
        WOLFSSL_MSG("WOLFSSL_QUIC_FORWARD_SECRETS neither "
                    "enc- nor decrypt specified");
        goto cleanup;
    }

    ret = !ssl->quic.method->set_encryption_secrets(
        ssl, level, rx_secret, tx_secret, ssl->specs.hash_size);

    /* Having installed the secrets, any future read/write will happen
     * at the level. Except early data, which is detected on the record
     * type and the handshake state. */
     if (ktype == early_data_key) {
        goto cleanup;
     }

     if (tx_secret && ssl->quic.enc_level_write != level) {
        ssl->quic.enc_level_write_next = level;
     }
     if (rx_secret && ssl->quic.enc_level_read != level) {
        ssl->quic.enc_level_read_next = level;
     }

cleanup:
    WOLFSSL_LEAVE("wolfSSL_quic_forward_secrets", ret);
    return ret;
}

int wolfSSL_quic_keys_active(WOLFSSL* ssl, enum encrypt_side side)
{
    int ret = 0;

    WOLFSSL_ENTER("wolfSSL_quic_keys_active");
    /* Keys derived from recent secrets have been activated */
    if (side == ENCRYPT_AND_DECRYPT_SIDE || side == ENCRYPT_SIDE_ONLY) {
        /* If there is data in the output buffers, it was supposed to be
         * encrypted at the previous level. We need to remember that when
         * forwarding this data to the QUIC protocol application. */
        if (ssl->buffers.outputBuffer.length > 0) {
            ret = wolfSSL_quic_send_internal(ssl);
            if (ret)
                goto cleanup;
        }
        ssl->quic.enc_level_write = ssl->quic.enc_level_write_next;
    }
    if (side == ENCRYPT_AND_DECRYPT_SIDE || side == DECRYPT_SIDE_ONLY) {
        ssl->quic.enc_level_read = ssl->quic.enc_level_read_next;
    }
cleanup:
    WOLFSSL_LEAVE("wolfSSL_quic_keys_active", ret);
    return ret;
}

const WOLFSSL_EVP_CIPHER* wolfSSL_quic_get_aead(WOLFSSL* ssl)
{
    WOLFSSL_CIPHER* cipher = wolfSSL_get_current_cipher(ssl);
    const WOLFSSL_EVP_CIPHER* evp_cipher;

    switch (cipher->cipherSuite) {
#if !defined(NO_AES) && defined(HAVE_AESGCM)
        case TLS_AES_128_GCM_SHA256:
            evp_cipher = wolfSSL_EVP_aes_128_gcm();
            break;
        case TLS_AES_256_GCM_SHA384:
            evp_cipher = wolfSSL_EVP_aes_256_gcm();
            break;
#endif
#if defined(HAVE_CHACHA) && defined(HAVE_POLY1305)
        case TLS_CHACHA20_POLY1305_SHA256:
            evp_cipher = wolfSSL_EVP_chacha20_poly1305();
            break;
#endif
#if defined(WOLFSSL_AES_COUNTER) && defined(WOLFSSL_AES_128)
        case TLS_AES_128_CCM_SHA256:
            FALL_THROUGH;
        case TLS_AES_128_CCM_8_SHA256:
            evp_cipher = wolfSSL_EVP_aes_128_ctr();
            break;
#endif

        default:
            evp_cipher = NULL;
            break;
    }

    if (!evp_cipher) {
        /* should not happen, as SSL* should not have negotiated it? */
        WOLFSSL_MSG("wolfSSL_quic_get_aead: current cipher not supported");
        return NULL;
    }
    return evp_cipher;
}

static int evp_cipher_eq(const WOLFSSL_EVP_CIPHER* c1,
                         const WOLFSSL_EVP_CIPHER* c2)
{
    /* We could check on nid equality, but we seem to have singulars */
    return c1 == c2;
}

const WOLFSSL_EVP_CIPHER* wolfSSL_quic_get_hp(WOLFSSL* ssl)
{
    WOLFSSL_CIPHER* cipher = wolfSSL_get_current_cipher(ssl);
    const WOLFSSL_EVP_CIPHER* evp_cipher;

    switch (cipher->cipherSuite) {
#if !defined(NO_AES) && defined(HAVE_AESGCM)
        case TLS_AES_128_GCM_SHA256:
            evp_cipher = wolfSSL_EVP_aes_128_ctr();
            break;
        case TLS_AES_256_GCM_SHA384:
            evp_cipher = wolfSSL_EVP_aes_256_ctr();
            break;
#endif
#if defined(HAVE_CHACHA) && defined(HAVE_POLY1305)
        case TLS_CHACHA20_POLY1305_SHA256:
            evp_cipher = wolfSSL_EVP_chacha20();
            break;
#endif
#if defined(WOLFSSL_AES_COUNTER) && defined(WOLFSSL_AES_128)
        case TLS_AES_128_CCM_SHA256:
            FALL_THROUGH;
        case TLS_AES_128_CCM_8_SHA256:
            evp_cipher = wolfSSL_EVP_aes_128_ctr();
            break;
#endif

        default:
            evp_cipher = NULL;
            break;
    }

    if (!evp_cipher) {
        /* should not happen, as SSL* should not have negotiated it? */
        WOLFSSL_MSG("wolfSSL_quic_get_hp: current cipher not supported");
        return NULL;
    }
    return evp_cipher;
}

size_t wolfSSL_quic_get_aead_tag_len(const WOLFSSL_EVP_CIPHER* aead_cipher)
{
    size_t ret;
#ifdef WOLFSSL_SMALL_STACK
    WOLFSSL_EVP_CIPHER_CTX *ctx = (WOLFSSL_EVP_CIPHER_CTX *)XMALLOC(
        sizeof(*ctx), NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (ctx == NULL)
        return 0;
#else
    WOLFSSL_EVP_CIPHER_CTX ctx[1];
#endif

    XMEMSET(ctx, 0, sizeof(*ctx));
    if (wolfSSL_EVP_CipherInit(ctx, aead_cipher, NULL, NULL, 0)
        == WOLFSSL_SUCCESS) {
        ret = ctx->authTagSz;
    } else {
        ret = 0;
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(ctx, NULL, DYNAMIC_TYPE_TMP_BUF);
#endif

    return ret;
}

int wolfSSL_quic_aead_is_gcm(const WOLFSSL_EVP_CIPHER* aead_cipher)
{
#if !defined(NO_AES) && defined(HAVE_AESGCM)
    if (evp_cipher_eq(aead_cipher, wolfSSL_EVP_aes_128_gcm())
#ifdef WOLFSSL_AES_256
        || evp_cipher_eq(aead_cipher, wolfSSL_EVP_aes_256_gcm())
#endif
    ) {
        return 1;
    }
#else
    (void)aead_cipher;
#endif
    return 0;
}

int wolfSSL_quic_aead_is_ccm(const WOLFSSL_EVP_CIPHER* aead_cipher)
{
#if defined(WOLFSSL_AES_COUNTER) && defined(WOLFSSL_AES_128)
    if (evp_cipher_eq(aead_cipher, wolfSSL_EVP_aes_128_ctr())) {
        return 1;
    }
#else
    (void)aead_cipher;
#endif
    return 0;
}

int wolfSSL_quic_aead_is_chacha20(const WOLFSSL_EVP_CIPHER* aead_cipher)
{
#if defined(HAVE_CHACHA) && defined(HAVE_POLY1305)
    return evp_cipher_eq(aead_cipher, wolfSSL_EVP_chacha20_poly1305());
#else
    (void)aead_cipher;
    return 0;
#endif
}

const WOLFSSL_EVP_MD* wolfSSL_quic_get_md(WOLFSSL* ssl)
{
    /* a copy from the handshake md setup */
    switch(ssl->specs.mac_algorithm) {
        case no_mac:
    #ifndef NO_MD5
        case md5_mac:
            return wolfSSL_EVP_md5();
    #endif
    #ifndef NO_SHA
        case sha_mac:
            return wolfSSL_EVP_sha1();
    #endif
    #ifdef WOLFSSL_SHA224
        case sha224_mac:
            return wolfSSL_EVP_sha224();
    #endif
        case sha256_mac:
            return wolfSSL_EVP_sha256();
    #ifdef WOLFSSL_SHA384
        case sha384_mac:
            return wolfSSL_EVP_sha384();
    #endif
    #ifdef WOLFSSL_SHA512
        case sha512_mac:
            return wolfSSL_EVP_sha512();
    #endif
        case rmd_mac:
        case blake2b_mac:
            WOLFSSL_MSG("no suitable EVP_MD");
            return NULL;
        default:
            WOLFSSL_MSG("Unknown mac algorithm");
            return NULL;
    }
}

#ifdef OPENSSL_EXTRA

int wolfSSL_quic_hkdf_extract(uint8_t* dest, const WOLFSSL_EVP_MD* md,
                              const uint8_t* secret, size_t secretlen,
                              const uint8_t* salt, size_t saltlen)
{
    WOLFSSL_EVP_PKEY_CTX* pctx = NULL;
    size_t destlen = (size_t)wolfSSL_EVP_MD_size(md);
    int ret = WOLFSSL_SUCCESS;

    WOLFSSL_ENTER("wolfSSL_quic_hkdf_extract");

    pctx = wolfSSL_EVP_PKEY_CTX_new_id(NID_hkdf, NULL);
    if (pctx == NULL) {
        ret = WOLFSSL_FAILURE;
        goto cleanup;
    }

    if (wolfSSL_EVP_PKEY_derive_init(pctx) != WOLFSSL_SUCCESS
        || wolfSSL_EVP_PKEY_CTX_hkdf_mode(
                pctx, EVP_PKEY_HKDEF_MODE_EXTRACT_ONLY) != WOLFSSL_SUCCESS
        || wolfSSL_EVP_PKEY_CTX_set_hkdf_md(pctx, md) != WOLFSSL_SUCCESS
        || wolfSSL_EVP_PKEY_CTX_set1_hkdf_salt(
                pctx, (byte*)salt, (int)saltlen) != WOLFSSL_SUCCESS
        || wolfSSL_EVP_PKEY_CTX_set1_hkdf_key(
                pctx, (byte*)secret, (int)secretlen) != WOLFSSL_SUCCESS
        || wolfSSL_EVP_PKEY_derive(pctx, dest, &destlen) != WOLFSSL_SUCCESS) {
        ret = WOLFSSL_FAILURE;
        goto cleanup;
    }

cleanup:
    if (pctx)
        wolfSSL_EVP_PKEY_CTX_free(pctx);
    WOLFSSL_LEAVE("wolfSSL_quic_hkdf_extract", ret);
    return ret;
}


int wolfSSL_quic_hkdf_expand(uint8_t* dest, size_t destlen,
                             const WOLFSSL_EVP_MD* md,
                             const uint8_t* secret, size_t secretlen,
                             const uint8_t* info, size_t infolen)
{
    WOLFSSL_EVP_PKEY_CTX* pctx = NULL;
    int ret = WOLFSSL_SUCCESS;

    WOLFSSL_ENTER("wolfSSL_quic_hkdf_expand");

    pctx = wolfSSL_EVP_PKEY_CTX_new_id(NID_hkdf, NULL);
    if (pctx == NULL) {
        ret = WOLFSSL_FAILURE;
        goto cleanup;
    }

    if (wolfSSL_EVP_PKEY_derive_init(pctx) != WOLFSSL_SUCCESS
        || wolfSSL_EVP_PKEY_CTX_hkdf_mode(
                pctx, EVP_PKEY_HKDEF_MODE_EXPAND_ONLY) != WOLFSSL_SUCCESS
        || wolfSSL_EVP_PKEY_CTX_set_hkdf_md(pctx, md) != WOLFSSL_SUCCESS
        || wolfSSL_EVP_PKEY_CTX_set1_hkdf_salt(
                pctx, (byte*)"", 0) != WOLFSSL_SUCCESS
        || wolfSSL_EVP_PKEY_CTX_set1_hkdf_key(
                pctx, (byte*)secret, (int)secretlen) != WOLFSSL_SUCCESS
        || wolfSSL_EVP_PKEY_CTX_add1_hkdf_info(
                pctx, (byte*)info, (int)infolen) != WOLFSSL_SUCCESS
        || wolfSSL_EVP_PKEY_derive(pctx, dest, &destlen) != WOLFSSL_SUCCESS) {
        ret = WOLFSSL_FAILURE;
        goto cleanup;
    }

cleanup:
    if (pctx)
        EVP_PKEY_CTX_free(pctx);
    WOLFSSL_LEAVE("wolfSSL_quic_hkdf_expand", ret);
    return ret;
}


int wolfSSL_quic_hkdf(uint8_t* dest, size_t destlen,
                      const WOLFSSL_EVP_MD* md,
                      const uint8_t* secret, size_t secretlen,
                      const uint8_t* salt, size_t saltlen,
                      const uint8_t* info, size_t infolen)
{
    WOLFSSL_EVP_PKEY_CTX* pctx = NULL;
    int ret = WOLFSSL_SUCCESS;

    WOLFSSL_ENTER("wolfSSL_quic_hkdf");

    pctx = wolfSSL_EVP_PKEY_CTX_new_id(NID_hkdf, NULL);
    if (pctx == NULL) {
        ret = WOLFSSL_FAILURE;
        goto cleanup;
    }

    if (wolfSSL_EVP_PKEY_derive_init(pctx) != WOLFSSL_SUCCESS
        || wolfSSL_EVP_PKEY_CTX_hkdf_mode(
                pctx, EVP_PKEY_HKDEF_MODE_EXTRACT_AND_EXPAND) != WOLFSSL_SUCCESS
        || wolfSSL_EVP_PKEY_CTX_set_hkdf_md(pctx, md) != WOLFSSL_SUCCESS
        || wolfSSL_EVP_PKEY_CTX_set1_hkdf_salt(
                pctx, (byte*)salt, (int)saltlen) != WOLFSSL_SUCCESS
        || wolfSSL_EVP_PKEY_CTX_set1_hkdf_key(
                pctx, (byte*)secret, (int)secretlen) != WOLFSSL_SUCCESS
        || wolfSSL_EVP_PKEY_CTX_add1_hkdf_info(
                pctx, (byte*)info, (int)infolen) != WOLFSSL_SUCCESS
        || wolfSSL_EVP_PKEY_derive(pctx, dest, &destlen) != WOLFSSL_SUCCESS) {
        ret = WOLFSSL_FAILURE;
        goto cleanup;
    }

cleanup:
    if (pctx)
        EVP_PKEY_CTX_free(pctx);
    WOLFSSL_LEAVE("wolfSSL_quic_hkdf", ret);
    return ret;
}

#endif /* OPENSSL_EXTRA */


WOLFSSL_EVP_CIPHER_CTX* wolfSSL_quic_crypt_new(const WOLFSSL_EVP_CIPHER* cipher,
                                               const uint8_t* key,
                                               const uint8_t* iv,
                                               int encrypt)
{
    WOLFSSL_EVP_CIPHER_CTX* ctx;

    ctx = wolfSSL_EVP_CIPHER_CTX_new();
    if (ctx == NULL) {
        return NULL;
    }

    if (wolfSSL_EVP_CipherInit(ctx, cipher, key, iv, encrypt)
            != WOLFSSL_SUCCESS) {
        wolfSSL_EVP_CIPHER_CTX_free(ctx);
        return NULL;
    }

    return ctx;
}


int wolfSSL_quic_aead_encrypt(uint8_t* dest, WOLFSSL_EVP_CIPHER_CTX* ctx,
                              const uint8_t* plain, size_t plainlen,
                              const uint8_t* iv, const uint8_t* aad,
                              size_t aadlen)
{
    int len;

    /* A case can be made if this really should be a function in wolfSSL, since
     * the same should be doable from the API by a QUIC protocol stack.
     * What speaks for this:
     * - it gives us a decent testing point
     * - API users do not have to re-invent (it fits into ngtcp2 use).
     *   picotls offers a similar abstraction level for AEAD.
     * TODO: there is some fiddling in OpenSSL+quic in regard to CCM ciphers
     *       which we need to check.
     */
    if (wolfSSL_EVP_CipherInit(ctx, NULL, NULL, iv, 1) != WOLFSSL_SUCCESS
        || wolfSSL_EVP_CipherUpdate(
                ctx, NULL, &len, aad, (int)aadlen) != WOLFSSL_SUCCESS
        || wolfSSL_EVP_CipherUpdate(
                ctx, dest, &len, plain, (int)plainlen) != WOLFSSL_SUCCESS
        || wolfSSL_EVP_CipherFinal(ctx, dest + len, &len) != WOLFSSL_SUCCESS
        || wolfSSL_EVP_CIPHER_CTX_ctrl(
                ctx, EVP_CTRL_AEAD_GET_TAG, ctx->authTagSz, dest + plainlen)
           != WOLFSSL_SUCCESS) {
        return WOLFSSL_FAILURE;
    }

    return WOLFSSL_SUCCESS;
}


int wolfSSL_quic_aead_decrypt(uint8_t* dest, WOLFSSL_EVP_CIPHER_CTX* ctx,
                              const uint8_t* enc, size_t enclen,
                              const uint8_t* iv, const uint8_t* aad,
                              size_t aadlen)
{
    int len;
    const uint8_t* tag;

    /* See rationale for wolfSSL_quic_aead_encrypt() on why this is here */
    if (enclen > INT_MAX || ctx->authTagSz > (int)enclen) {
        return WOLFSSL_FAILURE;
    }

    enclen -= ctx->authTagSz;
    tag = enc + enclen;

    if (wolfSSL_EVP_CipherInit(ctx, NULL, NULL, iv, 0) != WOLFSSL_SUCCESS
        || wolfSSL_EVP_CIPHER_CTX_ctrl(
                ctx, EVP_CTRL_AEAD_SET_TAG, ctx->authTagSz, (uint8_t*)tag)
            != WOLFSSL_SUCCESS
        || wolfSSL_EVP_CipherUpdate(ctx, NULL, &len, aad, (int)aadlen)
            != WOLFSSL_SUCCESS
        || wolfSSL_EVP_CipherUpdate(ctx, dest, &len, enc, (int)enclen)
            != WOLFSSL_SUCCESS
        || wolfSSL_EVP_CipherFinal(ctx, dest, &len) != WOLFSSL_SUCCESS) {
        return WOLFSSL_FAILURE;
    }

    return WOLFSSL_SUCCESS;
}


#endif /* WOLFSSL_QUIC */
#endif /* WOLFCRYPT_ONLY */

