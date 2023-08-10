/* dtls13.c
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

#ifdef WOLFSSL_DTLS13

#include <wolfssl/error-ssl.h>
#include <wolfssl/internal.h>
#include <wolfssl/ssl.h>
#include <wolfssl/wolfcrypt/aes.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/kdf.h>
#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/types.h>

#ifdef NO_INLINE
#include <wolfssl/wolfcrypt/misc.h>
#else
#define WOLFSSL_MISC_INCLUDED
#include <wolfcrypt/src/misc.c>
#endif

/**
 * enum rnDirection - distinguish between RecordNumber Enc/Dec
 * PROTECT: encrypt the Record Number
 * DEPROTECT: decrypt the Record Number
 */
enum rnDirection {
    PROTECT = 0,
    DEPROTECT,
};

/**
 * struct Dtls13HandshakeHeader: represent DTLS Handshake header
 * @msg_type: type of message (client_hello,server_hello,etc)
 * @length: length of the message
 * @messageSeq: message sequence number (used for reordering and retransmission)
 * @fragmentOffset: this is the offset of the data in the complete message. For
 * an unfragmented message this is always zero
 * @fragmentLength: length of this fragment (if not fragmented @fragmentLength
 * is always equal to @length)
 */
typedef struct Dtls13HandshakeHeader {
    byte msg_type;
    byte length[3];
    byte messageSeq[2];
    byte fragmentOffset[3];
    byte fragmentLength[3];
} Dtls13HandshakeHeader;

/**
 * struct Dtls13Recordplaintextheader: represent header of unprotected DTLSv1.3
 * record
 * @contentType: content type of the record (handshake, applicationData, etc)
 * @legacyversionrecord: legacy version field
 * @epoch: epoch number (lower 16 bits)
 * @sequenceNumber: sequence number (lower 16 bits)
 * @length: length of the record
 */
typedef struct Dtls13RecordPlaintextHeader {
    byte contentType;
    ProtocolVersion legacyVersionRecord;
    byte epoch[2];
    byte sequenceNumber[6];
    byte length[2];
} Dtls13RecordPlaintextHeader;

/* size of the len field in the unified header */
#define DTLS13_LEN_SIZE 2
/* size of the flags in the unified header */
#define DTLS13_HDR_FLAGS_SIZE 1
/* size of the sequence number where SEQ_LEN_BIT is present */
#define DTLS13_SEQ_16_LEN 2
/* size of the sequence number where SEQ_LEN_BIT is not present */
#define DTLS13_SEQ_8_LEN 1

/* fixed bits mask to detect unified header  */
#define DTLS13_FIXED_BITS_MASK (0x111 << 5)
/* fixed bits value to detect unified header  */
#define DTLS13_FIXED_BITS (0x1 << 5)
/* ConnectionID present bit in the unified header flags */
#define DTLS13_CID_BIT (0x1 << 4)
/* Sequence number is 16 bits if this bit is into unified header flags */
#define DTLS13_SEQ_LEN_BIT (0x1 << 3)
/* Length field is present if this bit is into unified header flags */
#define DTLS13_LEN_BIT (0x1 << 2)

/* For now, the size of the outgoing DTLSv1.3 record header is fixed to 5 bytes
   (8 bit header flags + 16bit record number + 16 bit length). In the future, we
   can dynamically choose to remove the length from the header to save
   space. Also it will need to account for client connection ID when
   supported. */
#define DTLS13_UNIFIED_HEADER_SIZE 5
#define DTLS13_MIN_CIPHERTEXT 16
#define DTLS13_MIN_RTX_INTERVAL 1

#ifndef NO_WOLFSSL_CLIENT
WOLFSSL_METHOD* wolfDTLSv1_3_client_method_ex(void* heap)
{
    WOLFSSL_METHOD* method;

    WOLFSSL_ENTER("DTLSv1_3_client_method_ex");
    (void)heap;

    method = (WOLFSSL_METHOD*)XMALLOC(sizeof(WOLFSSL_METHOD), heap,
        DYNAMIC_TYPE_METHOD);
    if (method)
        InitSSL_Method(method, MakeDTLSv1_3());

    return method;
}

WOLFSSL_METHOD* wolfDTLSv1_3_client_method(void)
{
    return wolfDTLSv1_3_client_method_ex(NULL);
}
#endif /* !NO_WOLFSSL_CLIENT */


#ifndef NO_WOLFSSL_SERVER
WOLFSSL_METHOD* wolfDTLSv1_3_server_method_ex(void* heap)
{
    WOLFSSL_METHOD* method;

    WOLFSSL_ENTER("DTLSv1_3_server_method_ex");
    (void)heap;

    method = (WOLFSSL_METHOD*)XMALLOC(sizeof(WOLFSSL_METHOD), heap,
        DYNAMIC_TYPE_METHOD);
    if (method) {
        InitSSL_Method(method, MakeDTLSv1_3());
        method->side = WOLFSSL_SERVER_END;
    }

    return method;
}

WOLFSSL_METHOD* wolfDTLSv1_3_server_method(void)
{
    return wolfDTLSv1_3_server_method_ex(NULL);
}
#endif /* !NO_WOLFSSL_SERVER */

int Dtls13RlAddPlaintextHeader(WOLFSSL* ssl, byte* out,
    enum ContentType content_type, word16 length)
{
    Dtls13RecordPlaintextHeader* hdr;
    word32 seq[2];
    int ret;

    hdr = (Dtls13RecordPlaintextHeader*)out;
    hdr->contentType = content_type;
    hdr->legacyVersionRecord.major = DTLS_MAJOR;
    hdr->legacyVersionRecord.minor = DTLSv1_2_MINOR;

    ret = Dtls13GetSeq(ssl, CUR_ORDER, seq, 1);
    if (ret != 0)
        return ret;

    /* seq[0] combines the epoch and 16 MSB of sequence number. We write on the
       epoch field and will overflow to the first two bytes of the sequence
       number */
    c32toa(seq[0], hdr->epoch);
    c32toa(seq[1], &hdr->sequenceNumber[2]);

    c16toa(length, hdr->length);

    return 0;
}

static int Dtls13HandshakeAddHeaderFrag(WOLFSSL* ssl, byte* output,
    enum HandShakeType msg_type, word32 frag_offset, word32 frag_length,
    word32 msg_length)
{
    Dtls13HandshakeHeader* hdr;

    hdr = (Dtls13HandshakeHeader*)output;

    hdr->msg_type = msg_type;
    c32to24((word32)msg_length, hdr->length);
    c16toa(ssl->keys.dtls_handshake_number, hdr->messageSeq);

    c32to24(frag_offset, hdr->fragmentOffset);
    c32to24(frag_length, hdr->fragmentLength);

    return 0;
}

static byte Dtls13TypeIsEncrypted(enum HandShakeType hs_type)
{
    byte ret = 0;

    switch (hs_type) {
    case hello_request:
    case hello_verify_request:
    case client_hello:
    case hello_retry_request:
    case server_hello:
        break;
    case encrypted_extensions:
    case session_ticket:
    case end_of_early_data:
    case certificate:
    case server_key_exchange:
    case certificate_request:
    case server_hello_done:
    case certificate_verify:
    case client_key_exchange:
    case finished:
    case certificate_status:
    case key_update:
    case change_cipher_hs:
    case message_hash:
    case no_shake:
        ret = 1;
    }

    return ret;
}

static int Dtls13GetRnMask(WOLFSSL* ssl, const byte* ciphertext, byte* mask,
    enum rnDirection dir)
{
    RecordNumberCiphers* c;

    if (dir == PROTECT)
        c = &ssl->dtlsRecordNumberEncrypt;
    else
        c = &ssl->dtlsRecordNumberDecrypt;

#if defined(HAVE_AESGCM) || defined(HAVE_AESCCM)
    if (ssl->specs.bulk_cipher_algorithm == wolfssl_aes_gcm ||
        ssl->specs.bulk_cipher_algorithm == wolfssl_aes_ccm) {

        if (c->aes == NULL)
            return BAD_STATE_E;
#if !defined(HAVE_SELFTEST) && \
    (!defined(HAVE_FIPS) || (defined(FIPS_VERSION_GE) && FIPS_VERSION_GE(5,3)))
        return wc_AesEncryptDirect(c->aes, mask, ciphertext);
#else
        wc_AesEncryptDirect(c->aes, mask, ciphertext);
#endif
    }
#endif /* HAVE_AESGCM || HAVE_AESCCM */

#ifdef HAVE_CHACHA
    if (ssl->specs.bulk_cipher_algorithm == wolfssl_chacha) {
        word32 counter;
        int ret;

        if (c->chacha == NULL)
            return BAD_STATE_E;

        /* assuming CIPHER[0..3] should be interpreted as little endian 32-bits
           integer. The draft rfc isn't really clear on that. See sec 4.2.3 of
           the draft. See also Section 2.3 of the Chacha RFC. */
        ato32le(ciphertext, &counter);

        ret = wc_Chacha_SetIV(c->chacha, &ciphertext[4], counter);
        if (ret != 0)
            return ret;

        XMEMSET(mask, 0, DTLS13_RN_MASK_SIZE);

        return wc_Chacha_Process(c->chacha, mask, mask, DTLS13_RN_MASK_SIZE);
    }
#endif /* HAVE_CHACHA */

    return NOT_COMPILED_IN;
}

static int Dtls13EncryptDecryptRecordNumber(WOLFSSL* ssl, byte* seq,
    int SeqLength, const byte* ciphertext, enum rnDirection dir)
{
    byte mask[DTLS13_RN_MASK_SIZE];
    int ret;

#ifdef HAVE_NULL_CIPHER
    /* Do not encrypt record numbers with null cipher. See RFC 9150 Sec 9 */
    if (ssl->specs.bulk_cipher_algorithm == wolfssl_cipher_null)
        return 0;
#endif /*HAVE_NULL_CIPHER */

    ret = Dtls13GetRnMask(ssl, ciphertext, mask, dir);
    if (ret != 0)
        return ret;

    xorbuf(seq, mask, SeqLength);

    return 0;
}

static byte Dtls13RtxMsgNeedsAck(WOLFSSL* ssl, enum HandShakeType hs)
{

#ifndef NO_WOLFSSL_SERVER
    /* we send an ACK when processing the finished message. In this case either
       we already sent an ACK for client's Certificate/CertificateVerify or they
       are in our list of seen records and will be included in the ACK
       message */
    if (ssl->options.side == WOLFSSL_SERVER_END && (hs == finished))
        return 1;
#else
    (void)ssl;
#endif /* NO_WOLFSSL_SERVER */

    if (hs == session_ticket || hs == key_update)
        return 1;

    return 0;
}

static void Dtls13MsgWasProcessed(WOLFSSL* ssl, enum HandShakeType hs)
{
    if (ssl->options.dtlsStateful)
        ssl->keys.dtls_expected_peer_handshake_number++;

    /* we need to send ACKs on the last message of a flight that needs explicit
       acknowledgment */
    ssl->dtls13Rtx.sendAcks = Dtls13RtxMsgNeedsAck(ssl, hs);
}

int Dtls13ProcessBufferedMessages(WOLFSSL* ssl)
{
    DtlsMsg* msg = ssl->dtls_rx_msg_list;
    word32 idx = 0;
    int ret = 0;

    WOLFSSL_ENTER("Dtls13ProcessBufferedMessages");

    while (msg != NULL) {
        idx = 0;

        /* message not in order */
        if (ssl->keys.dtls_expected_peer_handshake_number != msg->seq)
            break;

        /* message not complete */
        if (!msg->ready)
            break;

        ret = DoTls13HandShakeMsgType(ssl, msg->fullMsg, &idx, msg->type,
                msg->sz, msg->sz);

        /* processing certificate_request triggers a connect. The error came
         * from there, the message can be considered processed successfully.
         * WANT_WRITE means that we are done with processing the msg and we are
         * waiting to flush the output buffer. */
        if ((ret == 0 || ret == WANT_WRITE) || (msg->type == certificate_request &&
                         ssl->options.handShakeDone && ret == WC_PENDING_E)) {
            Dtls13MsgWasProcessed(ssl, (enum HandShakeType)msg->type);

            ssl->dtls_rx_msg_list = msg->next;
            DtlsMsgDelete(msg, ssl->heap);
            msg = ssl->dtls_rx_msg_list;
            ssl->dtls_rx_msg_list_sz--;
        }

        if (ret != 0)
            break;
    }

    WOLFSSL_LEAVE("dtls13_process_buffered_messages()", ret);

    return ret;
}

static int Dtls13NextMessageComplete(WOLFSSL* ssl)
{
    return ssl->dtls_rx_msg_list != NULL &&
           ssl->dtls_rx_msg_list->ready &&
           ssl->dtls_rx_msg_list->seq ==
               ssl->keys.dtls_expected_peer_handshake_number;
}

static WC_INLINE int FragIsInOutputBuffer(WOLFSSL* ssl, const byte* frag)
{
    const byte* OutputBuffer = ssl->buffers.outputBuffer.buffer;
    word32 OutputBufferSize = ssl->buffers.outputBuffer.bufferSize;

    return frag >= OutputBuffer && frag < OutputBuffer + OutputBufferSize;
}

static int Dtls13SendFragFromBuffer(WOLFSSL* ssl, byte* output, word16 length)
{
    byte* buf;
    int ret;

    if (FragIsInOutputBuffer(ssl, output))
        return BAD_FUNC_ARG;

    ret = CheckAvailableSize(ssl, length);
    if (ret != 0)
        return ret;

    buf = GetOutputBuffer(ssl);

    XMEMCPY(buf, output, length);

    ssl->buffers.outputBuffer.length += length;

    return SendBuffered(ssl);
}

static int Dtls13SendNow(WOLFSSL* ssl, enum HandShakeType handshakeType)
{
    if (!ssl->options.groupMessages || ssl->dtls13SendingFragments)
        return 1;

    if (handshakeType == client_hello || handshakeType == hello_retry_request ||
        handshakeType == finished || handshakeType == session_ticket ||
        handshakeType == session_ticket || handshakeType == key_update ||
        (handshakeType == certificate_request &&
            ssl->options.handShakeState == HANDSHAKE_DONE))
        return 1;

    return 0;
}

/* Handshake header DTLS only fields are not included in the transcript hash.
 * body points to the body of the DTLSHandshake message. */
int Dtls13HashClientHello(const WOLFSSL* ssl, byte* hash, int* hashSz,
        const byte* body, word32 length, CipherSpecs* specs)
{
    /* msg_type(1) + length (3) */
    byte header[OPAQUE32_LEN];
    int ret;
    wc_HashAlg hashCtx;
    int type = wolfSSL_GetHmacType_ex(specs);

    header[0] = (byte)client_hello;
    c32to24(length, header + 1);

    ret = wc_HashInit_ex(&hashCtx, type, ssl->heap, ssl->devId);
    if (ret == 0) {
        ret = wc_HashUpdate(&hashCtx, type, header, OPAQUE32_LEN);
        if (ret == 0)
            ret = wc_HashUpdate(&hashCtx, type, body, length);
        if (ret == 0)
            ret = wc_HashFinal(&hashCtx, type, hash);
        if (ret == 0) {
            *hashSz = wc_HashGetDigestSize(type);
            if (*hashSz < 0)
                ret = *hashSz;
        }
        wc_HashFree(&hashCtx, type);
    }
    return ret;
}

/* Handshake header DTLS only fields are not included in the transcript hash */
int Dtls13HashHandshake(WOLFSSL* ssl, const byte* input, word16 length)
{
    int ret;

    if (length < DTLS_HANDSHAKE_HEADER_SZ)
        return BAD_FUNC_ARG;

    /* msg_type(1) + length (3) */
    ret = HashRaw(ssl, input, OPAQUE32_LEN);
    if (ret != 0)
        return ret;

    input += OPAQUE32_LEN;
    length -= OPAQUE32_LEN;

    /* message_seq(2) + fragment_offset(3) + fragment_length(3) */
    input += OPAQUE64_LEN;
    length -= OPAQUE64_LEN;

    return HashRaw(ssl, input, length);
}

static int Dtls13SendFragment(WOLFSSL* ssl, byte* output, word16 output_size,
    word16 length, enum HandShakeType handshakeType, int hashOutput,
    int sendImmediately)
{
    word16 recordHeaderLength;
    word16 recordLength;
    byte isProtected;
    int sendLength;
    byte* msg;
    int ret;

    if (output_size < length)
        return BUFFER_ERROR;

    isProtected = Dtls13TypeIsEncrypted(handshakeType);
    recordHeaderLength = Dtls13GetRlHeaderLength(ssl, isProtected);

    if (length <= recordHeaderLength)
        return BUFFER_ERROR;

    recordLength = length - recordHeaderLength;

    if (!isProtected) {
        ret = Dtls13RlAddPlaintextHeader(ssl, output, handshake, recordLength);
        if (ret != 0)
            return ret;
    }
    else {
        msg = output + recordHeaderLength;

        if (length <= recordHeaderLength)
            return BUFFER_ERROR;

        if (hashOutput) {
            ret = Dtls13HashHandshake(ssl, msg, recordLength);
            if (ret != 0)
                return ret;
        }

        sendLength = BuildTls13Message(ssl, output, output_size, msg,
            recordLength, handshake, 0, 0, 0);
        if (sendLength < 0)
            return sendLength;

        length = (word16)sendLength;
    }

    if (!FragIsInOutputBuffer(ssl, output))
        return Dtls13SendFragFromBuffer(ssl, output, length);

    ssl->buffers.outputBuffer.length += length;

    ret = 0;
    if (sendImmediately)
        ret = SendBuffered(ssl);

    return ret;
}

static void Dtls13FreeFragmentsBuffer(WOLFSSL* ssl)
{
    XFREE(ssl->dtls13FragmentsBuffer.buffer, ssl->heap,
        DYNAMIC_TYPE_TMP_BUFFER);
    ssl->dtls13FragmentsBuffer.buffer = NULL;
    ssl->dtls13SendingFragments = 0;
    ssl->dtls13MessageLength = ssl->dtls13FragOffset = 0;
}

static WC_INLINE void Dtls13FreeRtxBufferRecord(WOLFSSL* ssl,
    Dtls13RtxRecord* r)
{
    (void)ssl;

    XFREE(r->data, ssl->heap, DYNAMIC_TYPE_DTLS_MSG);
    XFREE(r, ssl->heap, DYNAMIC_TYPE_DTLS_MSG);
}

static Dtls13RtxRecord* Dtls13RtxNewRecord(WOLFSSL* ssl, byte* data,
    word16 length, enum HandShakeType handshakeType, w64wrapper seq)
{
    w64wrapper epochNumber;
    Dtls13RtxRecord* r;

    WOLFSSL_ENTER("Dtls13RtxNewRecord");

    if (ssl->dtls13EncryptEpoch == NULL)
        return NULL;

    epochNumber = ssl->dtls13EncryptEpoch->epochNumber;

    r = (Dtls13RtxRecord*)XMALLOC(sizeof(*r), ssl->heap, DYNAMIC_TYPE_DTLS_MSG);
    if (r == NULL)
        return NULL;

    r->data = (byte*)XMALLOC(length, ssl->heap, DYNAMIC_TYPE_DTLS_MSG);
    if (r->data == NULL) {
        XFREE(r, ssl->heap, DYNAMIC_TYPE_DTLS_MSG);
        return NULL;
    }

    XMEMCPY(r->data, data, length);
    r->epoch = epochNumber;
    r->length = length;
    r->next = NULL;
    r->handshakeType = handshakeType;
    r->seq[0] = seq;
    r->rnIdx = 1;

    return r;
}

static void Dtls13RtxAddRecord(Dtls13Rtx* fsm, Dtls13RtxRecord* r)
{
    WOLFSSL_ENTER("Dtls13RtxAddRecord");

    *fsm->rtxRecordTailPtr = r;
    fsm->rtxRecordTailPtr = &r->next;
    r->next = NULL;
}

static void Dtls13RtxRecordUnlink(WOLFSSL* ssl, Dtls13RtxRecord** prevNext,
    Dtls13RtxRecord* r)
{
    /* if r was at the tail of the list, update the tail pointer */
    if (r->next == NULL)
        ssl->dtls13Rtx.rtxRecordTailPtr = prevNext;

    /* unlink */
    *prevNext = r->next;
}

static void Dtls13RtxFlushBuffered(WOLFSSL* ssl, byte keepNewSessionTicket)
{
    Dtls13RtxRecord *r, **prevNext;

    WOLFSSL_ENTER("Dtls13RtxFlushBuffered");

    prevNext = &ssl->dtls13Rtx.rtxRecords;
    r = ssl->dtls13Rtx.rtxRecords;

    /* we process the head at the end */
    while (r != NULL) {

        if (keepNewSessionTicket && r->handshakeType == session_ticket) {
            prevNext = &r->next;
            r = r->next;
            continue;
        }

        *prevNext = r->next;
        Dtls13FreeRtxBufferRecord(ssl, r);
        r = *prevNext;
    }

    ssl->dtls13Rtx.rtxRecordTailPtr = prevNext;
}

static Dtls13RecordNumber* Dtls13NewRecordNumber(w64wrapper epoch,
    w64wrapper seq, void* heap)
{
    Dtls13RecordNumber* rn;

    (void)heap;

    rn = (Dtls13RecordNumber*)XMALLOC(sizeof(*rn), heap,
        DYNAMIC_TYPE_DTLS_MSG);
    if (rn == NULL)
        return NULL;

    rn->next = NULL;
    rn->epoch = epoch;
    rn->seq = seq;

    return rn;
}

static int Dtls13RtxAddAck(WOLFSSL* ssl, w64wrapper epoch, w64wrapper seq)
{
    Dtls13RecordNumber* rn;

    WOLFSSL_ENTER("Dtls13RtxAddAck");

    rn = Dtls13NewRecordNumber(epoch, seq, ssl->heap);
    if (rn == NULL)
        return MEMORY_E;

    rn->next = ssl->dtls13Rtx.seenRecords;
    ssl->dtls13Rtx.seenRecords = rn;

    return 0;
}

static void Dtls13RtxFlushAcks(WOLFSSL* ssl)
{
    Dtls13RecordNumber *list, *rn;

    (void)ssl;

    WOLFSSL_ENTER("Dtls13RtxFlushAcks");

    list = ssl->dtls13Rtx.seenRecords;

    while (list != NULL) {
        rn = list;
        list = rn->next;
        XFREE(rn, ssl->heap, DYNAMIC_TYPE_DTLS_MSG);
    }

    ssl->dtls13Rtx.seenRecords = NULL;
}

static int Dtls13DetectDisruption(WOLFSSL* ssl, word32 fragOffset)
{
    /* retransmission. The other peer may have lost our flight or our ACKs. We
       don't account this as a disruption */
    if (ssl->keys.dtls_peer_handshake_number <
        ssl->keys.dtls_expected_peer_handshake_number)
        return 0;

    /* out of order message */
    if (ssl->keys.dtls_peer_handshake_number >
        ssl->keys.dtls_expected_peer_handshake_number) {
        return 1;
    }

    /* first fragment of in-order message */
    if (fragOffset == 0)
        return 0;

    /* is not the next fragment in the message (the check is not 100% perfect,
       in the worst case, we don't detect the disruption and wait for the other
       peer retransmission) */
    if (ssl->dtls_rx_msg_list != NULL) {
        DtlsFragBucket* last = ssl->dtls_rx_msg_list->fragBucketList;
        while (last != NULL && last->m.m.next != NULL)
            last = last->m.m.next;
        /* Does this fragment start right after the last fragment we
         * have stored? */
        if (last != NULL && (last->m.m.offset + last->m.m.sz) != fragOffset)
            return 1;
    }
    else {
        /* ssl->dtls_rx_msg_list is NULL and fragOffset != 0 so this is not in
         * order */
        return 1;
    }

    return 0;
}

static void Dtls13RtxRemoveCurAck(WOLFSSL* ssl)
{
    Dtls13RecordNumber *rn, **prevNext;

    prevNext = &ssl->dtls13Rtx.seenRecords;
    rn = ssl->dtls13Rtx.seenRecords;

    while (rn != NULL) {
        if (w64Equal(rn->epoch, ssl->keys.curEpoch64) &&
            w64Equal(rn->seq, ssl->keys.curSeq)) {
            *prevNext = rn->next;
            XFREE(rn, ssl->heap, DYNAMIC_TYPE_DTLS_MSG);
            return;
        }

        prevNext = &rn->next;
        rn = rn->next;
    }
}

static void Dtls13MaybeSaveClientHello(WOLFSSL* ssl)
{
    Dtls13RtxRecord *r, **prev_next;

    r = ssl->dtls13Rtx.rtxRecords;
    prev_next = &ssl->dtls13Rtx.rtxRecords;

    if (ssl->options.side == WOLFSSL_CLIENT_END &&
        ssl->options.connectState >= CLIENT_HELLO_SENT &&
        ssl->options.connectState <= HELLO_AGAIN_REPLY &&
        ssl->options.downgrade && ssl->options.minDowngrade >= DTLSv1_2_MINOR) {
        while (r != NULL) {
            if (r->handshakeType == client_hello) {
                Dtls13RtxRecordUnlink(ssl, prev_next, r);
                if (ssl->dtls13ClientHello != NULL)
                    XFREE(ssl->dtls13ClientHello, ssl->heap,
                        DYNAMIC_TYPE_DTLS_MSG);
                ssl->dtls13ClientHello = r->data;
                ssl->dtls13ClientHelloSz = r->length;
                r->data = NULL;
                Dtls13FreeRtxBufferRecord(ssl, r);
                return;
            }
            prev_next = &r->next;
            r = r->next;
        }
    }
}

static int Dtls13RtxMsgRecvd(WOLFSSL* ssl, enum HandShakeType hs,
    word32 fragOffset)
{
    WOLFSSL_ENTER("Dtls13RtxMsgRecvd");

    if (!ssl->options.handShakeDone &&
        ssl->keys.dtls_peer_handshake_number >=
            ssl->keys.dtls_expected_peer_handshake_number) {

        if (hs == server_hello)
            Dtls13MaybeSaveClientHello(ssl);

        /* In the handshake, receiving part of the next flight, acknowledge the
           sent flight. The only exception is, on the server side, receiving the
           last client flight does not ACK any sent new_session_ticket
           messages. */
        Dtls13RtxFlushBuffered(ssl, 1);
    }

    if (ssl->keys.dtls_peer_handshake_number <
        ssl->keys.dtls_expected_peer_handshake_number) {

        /* retransmission detected. */
        ssl->dtls13Rtx.retransmit = 1;

        /* the other peer may have retransmitted because an ACK for a flight
           that needs explicit ACK was lost.*/
        if (ssl->dtls13Rtx.seenRecords != NULL)
            ssl->dtls13Rtx.sendAcks = (byte)ssl->options.dtls13SendMoreAcks;
    }

    if (ssl->keys.dtls_peer_handshake_number ==
            ssl->keys.dtls_expected_peer_handshake_number &&
        ssl->options.handShakeDone && hs == certificate_request) {

        /* the current record, containing a post-handshake certificate request,
           is implicitly acknowledged by the
           certificate/certificate_verify/finished flight we are about to
           send. Please note that if the certificate request came out-of-order
           and we didn't send an ACK (sendMoreAcks == 0 and the missing
           packet(s) arrive before that fast timeout expired), then we will send
           both the ACK and the flight. While unnecessary this it's harmless, it
           should be rare and simplifies the code. Otherwise, it would be
           necessary to track which record number contained a CertificateRequest
           with a particular context id */
        Dtls13RtxRemoveCurAck(ssl);
    }

    if (ssl->options.dtls13SendMoreAcks &&
        Dtls13DetectDisruption(ssl, fragOffset)) {
        WOLFSSL_MSG("Disruption detected");
        ssl->dtls13Rtx.sendAcks = 1;
    }

    return 0;
}

void Dtls13FreeFsmResources(WOLFSSL* ssl)
{
    Dtls13RtxFlushAcks(ssl);
    Dtls13RtxFlushBuffered(ssl, 0);
}

static int Dtls13SendOneFragmentRtx(WOLFSSL* ssl,
    enum HandShakeType handshakeType, word16 outputSize, byte* message,
    word32 length, int hashOutput)
{
    Dtls13RtxRecord* rtxRecord = NULL;
    word16 recordHeaderLength;
    byte isProtected;
    int ret;

    isProtected = Dtls13TypeIsEncrypted(handshakeType);
    recordHeaderLength = Dtls13GetRlHeaderLength(ssl, isProtected);

    if (handshakeType != hello_retry_request) {
        rtxRecord = Dtls13RtxNewRecord(ssl, message + recordHeaderLength,
            (word16)(length - recordHeaderLength), handshakeType,
            ssl->dtls13EncryptEpoch->nextSeqNumber);
        if (rtxRecord == NULL)
            return MEMORY_E;
    }

    ret = Dtls13SendFragment(ssl, message, outputSize, (word16)length,
        handshakeType, hashOutput, Dtls13SendNow(ssl, handshakeType));

    if (rtxRecord != NULL) {
        if (ret == 0 || ret == WANT_WRITE)
            Dtls13RtxAddRecord(&ssl->dtls13Rtx, rtxRecord);
        else
            Dtls13FreeRtxBufferRecord(ssl, rtxRecord);
    }

    return ret;
}

static int Dtls13SendFragmentedInternal(WOLFSSL* ssl)
{
    int fragLength, rlHeaderLength;
    int remainingSize, maxFragment;
    int recordLength;
    byte isEncrypted;
    byte* output;
    int ret;

    isEncrypted = Dtls13TypeIsEncrypted(
        (enum HandShakeType)ssl->dtls13FragHandshakeType);
    rlHeaderLength = Dtls13GetRlHeaderLength(ssl, isEncrypted);
    maxFragment = wolfSSL_GetMaxFragSize(ssl, MAX_RECORD_SIZE);

    remainingSize = ssl->dtls13MessageLength - ssl->dtls13FragOffset;

    while (remainingSize > 0) {

        fragLength = maxFragment - rlHeaderLength - DTLS_HANDSHAKE_HEADER_SZ;

        recordLength = maxFragment;

        if (fragLength > remainingSize) {
            fragLength = remainingSize;
            recordLength =
                fragLength + rlHeaderLength + DTLS_HANDSHAKE_HEADER_SZ;
        }

        ret = CheckAvailableSize(ssl, recordLength + MAX_MSG_EXTRA);
        if (ret != 0)
            return ret;

        output = GetOutputBuffer(ssl);

        ret = Dtls13HandshakeAddHeaderFrag(ssl, output + rlHeaderLength,
            (enum HandShakeType)ssl->dtls13FragHandshakeType,
            ssl->dtls13FragOffset, fragLength, ssl->dtls13MessageLength);
        if (ret != 0) {
            Dtls13FreeFragmentsBuffer(ssl);
            return ret;
        }

        XMEMCPY(output + rlHeaderLength + DTLS_HANDSHAKE_HEADER_SZ,
            ssl->dtls13FragmentsBuffer.buffer + ssl->dtls13FragOffset,
            fragLength);

        ret = Dtls13SendOneFragmentRtx(ssl,
            (enum HandShakeType)ssl->dtls13FragHandshakeType,
            (word16)recordLength + MAX_MSG_EXTRA, output, (word32)recordLength, 0);
        if (ret == WANT_WRITE) {
            ssl->dtls13FragOffset += fragLength;
            return ret;
        }

        if (ret != 0) {
            Dtls13FreeFragmentsBuffer(ssl);
            return ret;
        }

        ssl->dtls13FragOffset += fragLength;
        remainingSize -= fragLength;
    }

    /* we sent all fragments */
    Dtls13FreeFragmentsBuffer(ssl);
    return 0;
}

static int Dtls13SendFragmented(WOLFSSL* ssl, byte* message, word16 length,
    enum HandShakeType handshake_type, int hash_output)
{
    int rlHeaderLength;
    byte isEncrypted;
    int messageSize;
    int ret;

    if (ssl->dtls13SendingFragments != 0) {
        WOLFSSL_MSG(
            "dtls13_send_fragmented() invoked while already sending fragments");
        return BAD_STATE_E;
    }

    isEncrypted = Dtls13TypeIsEncrypted(handshake_type);
    rlHeaderLength = Dtls13GetRlHeaderLength(ssl, isEncrypted);

    if (length < rlHeaderLength)
        return INCOMPLETE_DATA;

    /* DTLSv1.3 do not consider fragmentation for hash transcript. Build the
       hash now pretending fragmentation will not happen */
    if (hash_output) {
        ret = Dtls13HashHandshake(ssl, message + rlHeaderLength,
            length - (word16)rlHeaderLength);
        if (ret != 0)
            return ret;
    }

    messageSize = length - rlHeaderLength - DTLS_HANDSHAKE_HEADER_SZ;

    ssl->dtls13FragmentsBuffer.buffer =
        (byte*)XMALLOC(messageSize, ssl->heap, DYNAMIC_TYPE_TMP_BUFFER);

    if (ssl->dtls13FragmentsBuffer.buffer == NULL)
        return MEMORY_E;

    XMEMCPY(ssl->dtls13FragmentsBuffer.buffer,
        message + rlHeaderLength + DTLS_HANDSHAKE_HEADER_SZ, messageSize);

    ssl->dtls13MessageLength = messageSize;
    ssl->dtls13FragHandshakeType = handshake_type;
    ssl->dtls13SendingFragments = 1;

    return Dtls13SendFragmentedInternal(ssl);
}

static WC_INLINE word8 Dtls13GetEpochBits(w64wrapper epoch)
{
    return w64GetLow32(epoch) & EE_MASK;
}

#ifdef WOLFSSL_DTLS_CID
static byte Dtls13GetCidTxSize(WOLFSSL* ssl)
{
    unsigned int cidSz;
    int ret;
    ret = wolfSSL_dtls_cid_get_tx_size(ssl, &cidSz);
    if (ret != WOLFSSL_SUCCESS)
        return 0;
    return (byte)cidSz;
}

static byte Dtls13GetCidRxSize(WOLFSSL* ssl)
{
    unsigned int cidSz;
    int ret;
    ret = wolfSSL_dtls_cid_get_rx_size(ssl, &cidSz);
    if (ret != WOLFSSL_SUCCESS)
        return 0;
    return (byte)cidSz;
}

static int Dtls13AddCID(WOLFSSL* ssl, byte* flags, byte* out, word16* idx)
{
    byte cidSize;
    int ret;

    if (!wolfSSL_dtls_cid_is_enabled(ssl))
        return 0;

    cidSize = Dtls13GetCidTxSize(ssl);

    /* no cid */
    if (cidSize == 0)
        return 0;
    *flags |= DTLS13_CID_BIT;
    /* we know that we have at least cidSize of space */
    ret = wolfSSL_dtls_cid_get_tx(ssl, out + *idx, cidSize);
    if (ret != WOLFSSL_SUCCESS)
        return ret;
    *idx += cidSize;
    return 0;
}

static int Dtls13UnifiedHeaderParseCID(WOLFSSL* ssl, byte flags,
    const byte* input, word16 inputSize, word16* idx)
{
    unsigned int _cidSz;
    int ret;

    if (flags & DTLS13_CID_BIT) {
        if (!wolfSSL_dtls_cid_is_enabled(ssl)) {
            WOLFSSL_MSG("CID while no negotiated CID, ignoring");
            return DTLS_CID_ERROR;
        }

        if (!DtlsCIDCheck(ssl, input + *idx, inputSize - *idx)) {
            WOLFSSL_MSG("Not matching or wrong CID, ignoring");
            return DTLS_CID_ERROR;
        }

        ret = wolfSSL_dtls_cid_get_rx_size(ssl, &_cidSz);
        if (ret != WOLFSSL_SUCCESS)
            return ret;

        *idx += (word16)_cidSz;
        return 0;
    }

    /* CID not present */
    if (wolfSSL_dtls_cid_is_enabled(ssl)) {
        ret = wolfSSL_dtls_cid_get_rx_size(ssl, &_cidSz);
        if (ret != WOLFSSL_SUCCESS)
            return ret;

        if (_cidSz != 0) {
            WOLFSSL_MSG("expecting CID, ignoring");
            return DTLS_CID_ERROR;
        }
    }

    return 0;
}

#else
#define Dtls13AddCID(a, b, c, d) 0
#define Dtls13GetCidRxSize(a) 0
#define Dtls13GetCidTxSize(a) 0
#define Dtls13UnifiedHeaderParseCID(a, b, c, d, e) 0
#endif /* WOLFSSL_DTLS_CID */

/**
 * dtls13RlAddCiphertextHeader() - add record layer header in the buffer
 * @ssl: ssl object
 * @out: output buffer where to put the header
 * @length: length of the record
 */
int Dtls13RlAddCiphertextHeader(WOLFSSL* ssl, byte* out, word16 length)
{
    word16 seqNumber, idx;
    byte* flags;
    int ret;

    if (out == NULL)
        return BAD_FUNC_ARG;

    if (ssl->dtls13EncryptEpoch == NULL)
        return BAD_STATE_E;

    flags = out;

    /* header fixed bits */
    *flags = DTLS13_FIXED_BITS;
    /* epoch bits */
    *flags |= Dtls13GetEpochBits(ssl->dtls13EncryptEpoch->epochNumber);

    idx = DTLS13_HDR_FLAGS_SIZE;
    ret = Dtls13AddCID(ssl, flags, out, &idx);
    if (ret != 0)
        return ret;

    /* include 16-bit seq */
    *flags |= DTLS13_SEQ_LEN_BIT;
    /* include 16-bit length */
    *flags |= DTLS13_LEN_BIT;

    seqNumber = (word16)w64GetLow32(ssl->dtls13EncryptEpoch->nextSeqNumber);
    c16toa(seqNumber, out + idx);
    idx += OPAQUE16_LEN;
    c16toa(length, out + idx);

    return 0;
}

/**
 * Dtls13HandshakeAddHeader() - add handshake layer header
 * @ssl: ssl object
 * @output: output buffer
 * @msg_type: handshake type
 * @length: length of the message
 */
int Dtls13HandshakeAddHeader(WOLFSSL* ssl, byte* output,
    enum HandShakeType msg_type, word32 length)
{
    Dtls13HandshakeHeader* hdr;

    hdr = (Dtls13HandshakeHeader*)output;

    hdr->msg_type = msg_type;
    c32to24((word32)length, hdr->length);
    c16toa(ssl->keys.dtls_handshake_number, hdr->messageSeq);

    /* send unfragmented first */
    c32to24(0, hdr->fragmentOffset);
    c32to24((word32)length, hdr->fragmentLength);

    return 0;
}

/**
 * Dtls13EncryptRecordNumber() - encrypt record number in the header
 * @ssl: ssl object
 * @hdr: header
 *
 * Further info rfc draft 43 sec 4.2.3
 */
int Dtls13EncryptRecordNumber(WOLFSSL* ssl, byte* hdr, word16 recordLength)
{
    int seqLength;
    int hdrLength;
    int cidSz;

    if (ssl == NULL || hdr == NULL)
        return BAD_FUNC_ARG;

    /* we need at least a 16 bytes of ciphertext to encrypt record number see
       4.2.3*/
    if (recordLength < Dtls13GetRlHeaderLength(ssl, 1) + DTLS13_MIN_CIPHERTEXT)
        return BUFFER_ERROR;

    seqLength = (*hdr & DTLS13_LEN_BIT) ? DTLS13_SEQ_16_LEN : DTLS13_SEQ_8_LEN;

    cidSz = Dtls13GetCidTxSize(ssl);
    /* header flags + seq number + CID size*/
    hdrLength = OPAQUE8_LEN + seqLength + cidSz;

    /* length present */
    if (*hdr & DTLS13_LEN_BIT)
        hdrLength += DTLS13_LEN_SIZE;

    return Dtls13EncryptDecryptRecordNumber(ssl,
        /* seq number offset */
        hdr + OPAQUE8_LEN + cidSz,
        /* seq size */
        seqLength,
        /* cipher text */
        hdr + hdrLength, PROTECT);
}

/**
 * Dtls13GetRlHeaderLength() - get record layer header length
 * @ssl: ssl object
 * @isEncrypted: whether the record will be protected or not
 *
 * returns the length of the record layer header in bytes.
 */
word16 Dtls13GetRlHeaderLength(WOLFSSL* ssl, byte isEncrypted)
{
    (void)ssl;

    if (!isEncrypted)
        return DTLS_RECORD_HEADER_SZ;

    return DTLS13_UNIFIED_HEADER_SIZE + Dtls13GetCidTxSize(ssl);
}

/**
 * Dtls13GetHeadersLength() - return length of record + handshake header
 * @ssl: ssl object
 * @type: type of handshake in the message
 */
word16 Dtls13GetHeadersLength(WOLFSSL* ssl, enum HandShakeType type)
{
    byte isEncrypted;

    isEncrypted = Dtls13TypeIsEncrypted(type);

    return Dtls13GetRlHeaderLength(ssl, isEncrypted) + DTLS_HANDSHAKE_HEADER_SZ;
}

/**
 * Dtls13IsUnifiedHeader() - check if header is a DTLS unified header
 * @header_flags: first byte of the header
 *
 * Further info: dtls v1.3 draft43 section 4
 */
int Dtls13IsUnifiedHeader(byte hdrFirstByte)
{
    if (hdrFirstByte == alert || hdrFirstByte == handshake ||
        hdrFirstByte == ack)
        return 0;

    return ((hdrFirstByte & DTLS13_FIXED_BITS_MASK) == DTLS13_FIXED_BITS);
}

int Dtls13ReconstructSeqNumber(WOLFSSL* ssl, Dtls13UnifiedHdrInfo* hdrInfo,
    w64wrapper* out)
{
    word16 expectedLowBits;
    word16 seqLowBits;
    w64wrapper temp;
    word32 out32;
    word32 shift;
    word16 mask;
    byte wrap = 0;

    if (hdrInfo->seqHiPresent) {
        seqLowBits = (hdrInfo->seqHi << 8) | hdrInfo->seqLo;
        mask = 0xffff;
        shift = (1 << 16);
    }
    else {
        seqLowBits = hdrInfo->seqLo;
        mask = 0xff;
        shift = (1 << 8);
    }

    /* *out = (nextPeerSeqNumber & ~mask) | seqLowbits */
    out32 = w64GetLow32(ssl->dtls13DecryptEpoch->nextPeerSeqNumber);
    expectedLowBits = out32 & mask;
    out32 = (out32 & ~mask) | seqLowBits;
    *out = ssl->dtls13DecryptEpoch->nextPeerSeqNumber;
    w64SetLow32(out, out32);
    if (seqLowBits >= expectedLowBits) {
        if ((word32)(seqLowBits - expectedLowBits) > shift / 2) {
            temp = w64Sub32(*out, shift, &wrap);
            if (!wrap)
                *out = temp;
            return 0;
        }
    }
    else {
        /*  seqLowbits < expectedLowBits */
        if ((word32)(expectedLowBits - seqLowBits) > shift / 2) {
            temp = w64Add32(*out, shift, &wrap);
            if (!wrap)
                *out = temp;
            return 0;
        }
    }

    return 0;
}

int Dtls13ReconstructEpochNumber(WOLFSSL* ssl, byte epochBits,
    w64wrapper* epoch)
{
    w64wrapper _epoch;
    Dtls13Epoch* e;
    byte found = 0;
    int i;

    if (Dtls13GetEpochBits(ssl->dtls13PeerEpoch) == epochBits) {
        *epoch = ssl->dtls13PeerEpoch;
        return 0;
    }

    w64Zero(&_epoch);

    for (i = 0; i < DTLS13_EPOCH_SIZE; ++i) {
        e = &ssl->dtls13Epochs[i];

        if (!e->isValid)
            continue;

        if (Dtls13GetEpochBits(e->epochNumber) != epochBits)
            continue;

        if (w64GT(e->epochNumber, _epoch)) {
            found = 1;
            _epoch = e->epochNumber;
        }
    }

    if (found) {
        *epoch = _epoch;
        return 0;
    }

    return SEQUENCE_ERROR;
}

int Dtls13GetUnifiedHeaderSize(WOLFSSL* ssl, const byte input, word16* size)
{
    (void)ssl;

    if (size == NULL)
        return BAD_FUNC_ARG;

    /* flags (1) + CID + seq 8bit (1) */
    *size = OPAQUE8_LEN + Dtls13GetCidRxSize(ssl) + OPAQUE8_LEN;
    if (input & DTLS13_SEQ_LEN_BIT)
        *size += OPAQUE8_LEN;
    if (input & DTLS13_LEN_BIT)
        *size += OPAQUE16_LEN;

    return 0;
}

/**
 * Dtls13ParseUnifiedRecordLayer() - parse DTLS unified header
 * @ssl: [in] ssl object
 * @input: [in] buffer where the header is
 * @inputSize: [in] size of the input buffer
 * @hdrInfo: [out] header info struct
 *
 * It parse the header and put the relevant information inside @hdrInfo. Further
 * info: draft43 section 4
 *
 * return 0 on success
 */
int Dtls13ParseUnifiedRecordLayer(WOLFSSL* ssl, const byte* input,
    word16 inputSize, Dtls13UnifiedHdrInfo* hdrInfo)
{
    byte seqLen, hasLength;
    byte* seqNum;
    byte flags;
    word16 idx;
    int ret;

    if (input == NULL || inputSize < DTLS13_HDR_FLAGS_SIZE)
        return BAD_FUNC_ARG;

    flags = *input;
    idx = DTLS13_HDR_FLAGS_SIZE;
    ret = Dtls13UnifiedHeaderParseCID(ssl, flags, input, inputSize, &idx);
    if (ret != 0)
        return ret;

    seqNum = (byte*)input + idx;
    seqLen = (flags & DTLS13_SEQ_LEN_BIT) != 0 ? DTLS13_SEQ_16_LEN
                                               : DTLS13_SEQ_8_LEN;
    hasLength = flags & DTLS13_LEN_BIT;
    hdrInfo->epochBits = flags & EE_MASK;

    idx += seqLen;

    if (inputSize < idx)
        return BUFFER_ERROR;

    if (hasLength) {
        if (inputSize < idx + DTLS13_LEN_SIZE)
            return BUFFER_ERROR;

        ato16(input + idx, &hdrInfo->recordLength);
        idx += DTLS13_LEN_SIZE;
    }
    else {
        /* length not present. The size of the record is the all the remaining
           data received with this datagram */
        hdrInfo->recordLength = inputSize - idx;
    }

    /* minimum size for a dtls1.3 packet is 16 bytes (to have enough ciphertext
       to create record number xor mask). (draft 43 - Sec 4.2.3) */
    if (hdrInfo->recordLength < DTLS13_RN_MASK_SIZE)
        return LENGTH_ERROR;
    if (inputSize < idx + DTLS13_RN_MASK_SIZE)
        return BUFFER_ERROR;

    ret = Dtls13EncryptDecryptRecordNumber(ssl, seqNum, seqLen, input + idx,
        DEPROTECT);
    if (ret != 0)
        return ret;

    if (seqLen == DTLS13_SEQ_16_LEN) {
        hdrInfo->seqHiPresent = 1;
        hdrInfo->seqHi = seqNum[0];
        hdrInfo->seqLo = seqNum[1];
    }
    else {
        hdrInfo->seqHiPresent = 0;
        hdrInfo->seqLo = seqNum[0];
    }

    return 0;
}

int Dtls13RecordRecvd(WOLFSSL* ssl)
{
    int ret;

    if (ssl->curRL.type != handshake)
        return 0;

    if (!ssl->options.dtls13SendMoreAcks)
        ssl->dtls13FastTimeout = 1;

    ret = Dtls13RtxAddAck(ssl, ssl->keys.curEpoch64, ssl->keys.curSeq);
    if (ret != 0)
        WOLFSSL_MSG("can't save ack fragment");

    return ret;
}

static void Dtls13RtxMoveToEndOfList(WOLFSSL* ssl, Dtls13RtxRecord** prevNext,
    Dtls13RtxRecord* r)
{
    /* already at the end */
    if (r->next == NULL)
        return;

    Dtls13RtxRecordUnlink(ssl, prevNext, r);
    /* add to the end */
    Dtls13RtxAddRecord(&ssl->dtls13Rtx, r);
}

static int Dtls13RtxSendBuffered(WOLFSSL* ssl)
{
    word16 headerLength;
    Dtls13RtxRecord *r, **prevNext;
    w64wrapper seq;
    byte* output;
    int isLast;
    int sendSz;
    word32 now;
    int ret;

    WOLFSSL_ENTER("Dtls13RtxSendBuffered");

    now = LowResTimer();
    if (now - ssl->dtls13Rtx.lastRtx < DTLS13_MIN_RTX_INTERVAL) {
#ifdef WOLFSSL_DEBUG_TLS
        WOLFSSL_MSG("Avoid too fast retransmission");
#endif /* WOLFSSL_DEBUG_TLS */
        return 0;
    }

    ssl->dtls13Rtx.lastRtx = now;

    r = ssl->dtls13Rtx.rtxRecords;
    prevNext = &ssl->dtls13Rtx.rtxRecords;
    while (r != NULL) {
        isLast = r->next == NULL;
        WOLFSSL_MSG("Dtls13Rtx One Record");

        headerLength = Dtls13GetRlHeaderLength(ssl, !w64IsZero(r->epoch));

        sendSz = r->length + headerLength;

        if (!w64IsZero(r->epoch))
            sendSz += MAX_MSG_EXTRA;

        ret = CheckAvailableSize(ssl, sendSz);
        if (ret != 0)
            return ret;

        output = GetOutputBuffer(ssl);

        XMEMCPY(output + headerLength, r->data, r->length);

        if (!w64Equal(ssl->dtls13EncryptEpoch->epochNumber, r->epoch)) {
            ret = Dtls13SetEpochKeys(ssl, r->epoch, ENCRYPT_SIDE_ONLY);
            if (ret != 0)
                return ret;
        }

        seq = ssl->dtls13EncryptEpoch->nextSeqNumber;

        ret = Dtls13SendFragment(ssl, output, (word16)sendSz, r->length + headerLength,
            (enum HandShakeType)r->handshakeType, 0,
            isLast || !ssl->options.groupMessages);
        if (ret != 0 && ret != WANT_WRITE)
            return ret;

        if (r->rnIdx >= DTLS13_RETRANS_RN_SIZE)
            r->rnIdx = 0;

#ifdef WOLFSSL_DEBUG_TLS
        WOLFSSL_MSG_EX("tracking r hs: %d with seq: %ld", r->handshakeType,
            seq);
#endif /* WOLFSSL_DEBUG_TLS */

        r->seq[r->rnIdx] = seq;
        r->rnIdx++;

        if (ret == WANT_WRITE) {
            /* this fragment will be sent eventually. Move it to the end of the
               list so next time we start with a new one. */
            Dtls13RtxMoveToEndOfList(ssl, prevNext, r);
            return ret;
        }

        prevNext = &r->next;
        r = r->next;
    }

    return 0;
}

/**
 * Dtls13HandshakeRecv() - process an handshake message. Deal with
 fragmentation if needed
 * @ssl: [in] ssl object
 * @input: [in] input buffer
 * @size: [in] input buffer size
 * @type: [out] content type
 * @processedSize: [out] amount of byte processed
 *
 * returns 0 on success
 */
static int _Dtls13HandshakeRecv(WOLFSSL* ssl, byte* input, word32 size,
    word32* processedSize)
{
    word32 fragOff, fragLength;
    byte isComplete, isFirst;
    byte usingAsyncCrypto;
    word32 messageLength;
    byte handshakeType;
    word32 idx;
    int ret;

    idx = 0;
    ret = GetDtlsHandShakeHeader(ssl, input, &idx, &handshakeType,
        &messageLength, &fragOff, &fragLength, size);
    if (ret != 0)
        return PARSE_ERROR;

    if (ssl->options.side == WOLFSSL_SERVER_END &&
            ssl->options.acceptState < TLS13_ACCEPT_FIRST_REPLY_DONE) {
        if (handshakeType != client_hello) {
            WOLFSSL_MSG("Ignoring other messages before we verify a ClientHello");
            *processedSize = size;
            return 0;
        }
        /* To be able to operate in stateless mode, we assume the ClientHello
         * is in order and we use its Handshake Message number and Sequence
         * Number for our Tx. */
        ssl->keys.dtls_expected_peer_handshake_number =
            ssl->keys.dtls_handshake_number =
                ssl->keys.dtls_peer_handshake_number;
        ssl->dtls13Epochs[0].nextSeqNumber = ssl->keys.curSeq;
    }

    if (idx + fragLength > size) {
        WOLFSSL_ERROR(INCOMPLETE_DATA);
        return INCOMPLETE_DATA;
    }

    if (fragOff + fragLength > messageLength)
        return BUFFER_ERROR;

    ret = Dtls13RtxMsgRecvd(ssl, (enum HandShakeType)handshakeType, fragOff);
    if (ret != 0)
        return ret;

    if (ssl->keys.dtls_peer_handshake_number <
        ssl->keys.dtls_expected_peer_handshake_number) {

#ifdef WOLFSSL_DEBUG_TLS
        WOLFSSL_MSG(
            "DTLS1.3 retransmission detected - discard and schedule a rtx");
#endif /* WOLFSSL_DEBUG_TLS */

        /* ignore the message */
        *processedSize = idx + fragLength + ssl->keys.padSz;

        return 0;
    }

    isFirst = fragOff == 0;
    isComplete = isFirst && fragLength == messageLength;

    if (!isComplete && !IsEncryptionOn(ssl, 0)) {
#ifdef WOLFSSL_DEBUG_TLS
        WOLFSSL_MSG("DTLS1.3 not accepting fragmented plaintext message");
#endif /* WOLFSSL_DEBUG_TLS */
        /* ignore the message */
        *processedSize = idx + fragLength + ssl->keys.padSz;
        return 0;
    }

    usingAsyncCrypto = ssl->devId != INVALID_DEVID;

    /* store the message if any of the following: (a) incomplete message, (b)
     * out of order message or (c) if using async crypto. In (c) the processing
     * of the message can return WC_PENDING_E, it's easier to handle this error
     * if the message is stored in the buffer.
     */
    if (!isComplete ||
        ssl->keys.dtls_peer_handshake_number >
            ssl->keys.dtls_expected_peer_handshake_number ||
        usingAsyncCrypto) {
        if (ssl->dtls_rx_msg_list_sz < DTLS_POOL_SZ) {
            DtlsMsgStore(ssl, (word16)w64GetLow32(ssl->keys.curEpoch64),
                ssl->keys.dtls_peer_handshake_number,
                input + DTLS_HANDSHAKE_HEADER_SZ, messageLength, handshakeType,
                fragOff, fragLength, ssl->heap);
        }
        else {
            /* DTLS_POOL_SZ outstanding messages is way more than enough for any
             * valid peer */
            return DTLS_TOO_MANY_FRAGMENTS_E;
        }

        *processedSize = idx + fragLength + ssl->keys.padSz;
        if (Dtls13NextMessageComplete(ssl))
            return Dtls13ProcessBufferedMessages(ssl);

        return 0;
    }

    ret = DoTls13HandShakeMsgType(ssl, input, &idx, handshakeType,
        messageLength, size);
    if (ret != 0)
        return ret;

    Dtls13MsgWasProcessed(ssl, (enum HandShakeType)handshakeType);
    *processedSize = idx;

    /* check if we have buffered some message */
    if (Dtls13NextMessageComplete(ssl))
        return Dtls13ProcessBufferedMessages(ssl);

    return 0;
}

int Dtls13HandshakeRecv(WOLFSSL* ssl, byte* input, word32* inOutIdx,
    word32 totalSz)
{
    word32 maxSize, processedSize = 0;
    byte* message;
    int ret;

    message = input + *inOutIdx;
    maxSize = totalSz - *inOutIdx;

    ret = _Dtls13HandshakeRecv(ssl, message, maxSize, &processedSize);

    *inOutIdx += processedSize;

    return ret;
}

/**
 * Dtls13FragmentsContinue() - keep sending pending fragments
 * @ssl: ssl object
 */
int Dtls13FragmentsContinue(WOLFSSL* ssl)
{
    int ret;

    ret = Dtls13SendFragmentedInternal(ssl);
    if (ret == 0)
        ssl->keys.dtls_handshake_number++;

    return ret;
}

/**
 * Dtls13AddHeaders() - setup handshake header
 * @output: output buffer at the start of the record
 * @length: length of the full message, included headers
 * @hsType: handshake type
 * @ssl: ssl object
 *
 * This function add the handshake headers and leaves space for the record
 * layer. The real record layer will be added in dtls_send() for unprotected
 * messages and in BuildTls13message() for protected messages.
 *
 * returns 0 on success, -1 otherwise
 */
int Dtls13AddHeaders(byte* output, word32 length, enum HandShakeType hsType,
    WOLFSSL* ssl)
{
    word16 handshakeOffset;
    byte isEncrypted;

    isEncrypted = Dtls13TypeIsEncrypted(hsType);
    handshakeOffset = Dtls13GetRlHeaderLength(ssl, isEncrypted);

    /* The record header is placed by either Dtls13HandshakeSend() or
       BuildTls13Message() */

    return Dtls13HandshakeAddHeader(ssl, output + handshakeOffset, hsType,
        length);
}

/**
 * Dtls13HandshakeSend() - send an handshake message. Fragment if necessary.
 *
 * @ssl: ssl object
 * @message: message where the buffer is in. Handshake header already in place.
 * @output_size: size of the @message buffer
 * @length: length of the message including headers
 * @handshakeType: handshake type of the message
 * @hashOutput: if true add the message to the transcript hash
 *
 */
int Dtls13HandshakeSend(WOLFSSL* ssl, byte* message, word16 outputSize,
    word16 length, enum HandShakeType handshakeType, int hashOutput)
{
    int maxFrag;
    int maxLen;
    int ret;

    if (ssl->dtls13EncryptEpoch == NULL)
        return BAD_STATE_E;

    /* if we are here, the message is built */
    ssl->options.buildingMsg = 0;

    if (!ssl->options.handShakeDone) {

        /* during the handshake, if we are sending a new flight, we can flush
           our ACK list. When sending client
           [certificate/certificate_verify]/finished flight, we may flush an ACK
           for a newSessionticket message, sent by the server just after sending
           its finished message. This should not be a problem. That message
           arrived out-of-order (before the server finished) so likely an ACK
           was already sent. In the worst case we will ACK the server
           retranmission*/
        if (handshakeType == certificate || handshakeType == finished ||
            handshakeType == server_hello || handshakeType == client_hello)
            Dtls13RtxFlushAcks(ssl);
    }

    /* we want to send always with the highest epoch  */
    if (!w64Equal(ssl->dtls13EncryptEpoch->epochNumber, ssl->dtls13Epoch)) {
        ret = Dtls13SetEpochKeys(ssl, ssl->dtls13Epoch, ENCRYPT_SIDE_ONLY);
        if (ret != 0)
            return ret;
    }

    maxFrag = wolfSSL_GetMaxFragSize(ssl, MAX_RECORD_SIZE);
    maxLen = length;

    if (handshakeType == key_update)
        ssl->dtls13WaitKeyUpdateAck = 1;

    if (maxLen < maxFrag) {
        ret = Dtls13SendOneFragmentRtx(ssl, handshakeType, outputSize, message,
            length, hashOutput);
        if (ret == 0 || ret == WANT_WRITE)
            ssl->keys.dtls_handshake_number++;
    }
    else {
        ret = Dtls13SendFragmented(ssl, message, length, handshakeType,
            hashOutput);
        if (ret == 0)
            ssl->keys.dtls_handshake_number++;
    }

    return ret;
}

#define SN_LABEL_SZ 2
static const byte snLabel[SN_LABEL_SZ + 1] = "sn";

/**
 * Dtls13DeriveSnKeys() - derive the key used to encrypt the record number
 * @ssl: ssl object
 * @provision: which side (CLIENT or SERVER) to provision
 */
int Dtls13DeriveSnKeys(WOLFSSL* ssl, int provision)
{
    byte key_dig[MAX_PRF_DIG];
    int ret = 0;

    if (provision & PROVISION_CLIENT) {
        WOLFSSL_MSG("Derive SN Client key");
        ret = Tls13DeriveKey(ssl, key_dig, ssl->specs.key_size,
            ssl->clientSecret, snLabel, SN_LABEL_SZ, ssl->specs.mac_algorithm,
            0, WOLFSSL_CLIENT_END);
        if (ret != 0)
            goto end;

        XMEMCPY(ssl->keys.client_sn_key, key_dig, ssl->specs.key_size);
    }

    if (provision & PROVISION_SERVER) {
        WOLFSSL_MSG("Derive SN Server key");
        ret = Tls13DeriveKey(ssl, key_dig, ssl->specs.key_size,
            ssl->serverSecret, snLabel, SN_LABEL_SZ, ssl->specs.mac_algorithm,
            0, WOLFSSL_SERVER_END);
        if (ret != 0)
            goto end;

        XMEMCPY(ssl->keys.server_sn_key, key_dig, ssl->specs.key_size);
    }

end:
    ForceZero(key_dig, MAX_PRF_DIG);
    return ret;
}

static int Dtls13InitAesCipher(WOLFSSL* ssl, RecordNumberCiphers* cipher,
    const byte* key, word16 keySize)
{
    int ret;
    if (cipher->aes == NULL) {
        cipher->aes =
            (Aes*)XMALLOC(sizeof(Aes), ssl->heap, DYNAMIC_TYPE_CIPHER);
        if (cipher->aes == NULL)
            return MEMORY_E;
    }
    else {
        wc_AesFree(cipher->aes);
    }

    XMEMSET(cipher->aes, 0, sizeof(*cipher->aes));

    ret = wc_AesInit(cipher->aes, ssl->heap, INVALID_DEVID);
    if (ret != 0)
        return ret;

    return wc_AesSetKey(cipher->aes, key, keySize, NULL, AES_ENCRYPTION);
}

#ifdef HAVE_CHACHA
static int Dtls13InitChaChaCipher(RecordNumberCiphers* c, byte* key,
    word16 keySize, void* heap)
{
    (void)heap;

    if (c->chacha == NULL) {
        c->chacha = (ChaCha*)XMALLOC(sizeof(ChaCha), heap, DYNAMIC_TYPE_CIPHER);

        if (c->chacha == NULL)
            return MEMORY_E;
    }

    return wc_Chacha_SetKey(c->chacha, key, keySize);
}
#endif /* HAVE_CHACHA */

struct Dtls13Epoch* Dtls13GetEpoch(WOLFSSL* ssl, w64wrapper epochNumber)
{
    Dtls13Epoch* e;
    int i;

    for (i = 0; i < DTLS13_EPOCH_SIZE; ++i) {
        e = &ssl->dtls13Epochs[i];
        if (w64Equal(e->epochNumber, epochNumber) && e->isValid)
            return e;
    }

    return NULL;
}

void Dtls13SetOlderEpochSide(WOLFSSL* ssl, w64wrapper epochNumber,
                                    int side)
{
    Dtls13Epoch* e;
    int i;

    for (i = 0; i < DTLS13_EPOCH_SIZE; ++i) {
        e = &ssl->dtls13Epochs[i];
        if (e->isValid && w64LT(e->epochNumber, epochNumber)) {
            e->side = (byte)side;
        }
    }
}

static void Dtls13EpochCopyKeys(WOLFSSL* ssl, Dtls13Epoch* e, Keys* k, int side)
{
    byte clientWrite, serverWrite;
    byte enc, dec;

    WOLFSSL_ENTER("Dtls13SetEpochKeys");

    clientWrite = serverWrite = 0;
    enc = dec = 0;
    switch (side) {

    case ENCRYPT_SIDE_ONLY:
        if (ssl->options.side == WOLFSSL_CLIENT_END)
            clientWrite = 1;
        if (ssl->options.side == WOLFSSL_SERVER_END)
            serverWrite = 1;
        enc = 1;
        break;

    case DECRYPT_SIDE_ONLY:
        if (ssl->options.side == WOLFSSL_CLIENT_END)
            serverWrite = 1;
        if (ssl->options.side == WOLFSSL_SERVER_END)
            clientWrite = 1;
        dec = 1;
        break;

    case ENCRYPT_AND_DECRYPT_SIDE:
        clientWrite = serverWrite = 1;
        enc = dec = 1;
        break;
    }

    if (clientWrite) {
        XMEMCPY(e->client_write_key, k->client_write_key,
            sizeof(e->client_write_key));

        XMEMCPY(e->client_write_IV, k->client_write_IV,
            sizeof(e->client_write_IV));

        XMEMCPY(e->client_sn_key, k->client_sn_key, sizeof(e->client_sn_key));
    }

    if (serverWrite) {
        XMEMCPY(e->server_write_key, k->server_write_key,
            sizeof(e->server_write_key));
        XMEMCPY(e->server_write_IV, k->server_write_IV,
            sizeof(e->server_write_IV));
        XMEMCPY(e->server_sn_key, k->server_sn_key, sizeof(e->server_sn_key));
    }

    if (enc)
        XMEMCPY(e->aead_enc_imp_IV, k->aead_enc_imp_IV,
            sizeof(e->aead_enc_imp_IV));

    if (dec)
        XMEMCPY(e->aead_dec_imp_IV, k->aead_dec_imp_IV,
            sizeof(e->aead_dec_imp_IV));
}

/* For storing the sequence number we use a word32[2] array here, instead of
   word64. This is to reuse existing code */
int Dtls13GetSeq(WOLFSSL* ssl, int order, word32* seq, byte increment)
{
    w64wrapper* nativeSeq;

    if (order == PEER_ORDER) {
        nativeSeq = &ssl->keys.curSeq;
        /* never increment seq number for current record. In DTLS seq number are
           explicit */
        increment = 0;
    }
    else if (order == CUR_ORDER) {

        if (ssl->dtls13EncryptEpoch == NULL) {
            return BAD_STATE_E;
        }

        nativeSeq = &ssl->dtls13EncryptEpoch->nextSeqNumber;
    }
    else {
        return BAD_FUNC_ARG;
    }

    seq[0] = w64GetHigh32(*nativeSeq);
    seq[1] = w64GetLow32(*nativeSeq);

#ifdef WOLFSSL_DEBUG_TLS
    WOLFSSL_MSG_EX("Dtls13GetSeq(): using seq: %ld", *nativeSeq);
#endif /* WOLFSSL_DEBUG_TLS */

    if (increment) {
        w64Increment(nativeSeq);

        /* seq number wrapped up */
        if (w64IsZero(*nativeSeq))
            return BAD_STATE_E;
    }

    return 0;
}

static Dtls13Epoch* Dtls13NewEpochSlot(WOLFSSL* ssl)
{
    Dtls13Epoch *e, *oldest = NULL;
    w64wrapper oldestNumber;
    int i;

    /* FIXME: add max function */
    oldestNumber = w64From32((word32)-1, (word32)-1);
    oldest = NULL;

    for (i = 0; i < DTLS13_EPOCH_SIZE; ++i) {
        e = &ssl->dtls13Epochs[i];
        if (!e->isValid)
            return e;

        if (!w64Equal(e->epochNumber, ssl->dtls13Epoch) &&
            !w64Equal(e->epochNumber, ssl->dtls13PeerEpoch) &&
            w64LT(e->epochNumber, oldestNumber))
            oldest = e;
    }

    if (oldest == NULL)
        return NULL;

    e = oldest;

#ifdef WOLFSSL_DEBUG_TLS
    WOLFSSL_MSG_EX("Delete epoch: %d", e->epochNumber);
#endif /* WOLFSSL_DEBUG_TLS */

    XMEMSET(e, 0, sizeof(*e));

    return e;
}

int Dtls13NewEpoch(WOLFSSL* ssl, w64wrapper epochNumber, int side)
{
    Dtls13Epoch* e;

#ifdef WOLFSSL_DEBUG_TLS
    WOLFSSL_MSG_EX("New epoch: %d", w64GetLow32(epochNumber));
#endif /* WOLFSSL_DEBUG_TLS */

    e = Dtls13GetEpoch(ssl, epochNumber);
    if (e == NULL) {
        e = Dtls13NewEpochSlot(ssl);
        if (e == NULL)
            return BAD_STATE_E;
    }

    Dtls13EpochCopyKeys(ssl, e, &ssl->keys, side);

    if (!e->isValid) {
        /* fresh epoch, initialize fields */
        e->epochNumber = epochNumber;
        e->isValid = 1;
        e->side = (byte)side;
    }
    else if (e->side != side) {
        /* epoch used for the other side already. update side */
        e->side = ENCRYPT_AND_DECRYPT_SIDE;
    }

    /* Once handshake is done. Mark epochs older than the last one as encrypt
     * only so that they can't be used for decryption. */
    if (ssl->options.handShakeDone && (e->side == ENCRYPT_AND_DECRYPT_SIDE ||
            e->side == DECRYPT_SIDE_ONLY)) {
        w64Decrement(&epochNumber);
        Dtls13SetOlderEpochSide(ssl, epochNumber, ENCRYPT_SIDE_ONLY);
    }

    return 0;
}

int Dtls13SetEpochKeys(WOLFSSL* ssl, w64wrapper epochNumber,
    enum encrypt_side side)
{
    byte clientWrite, serverWrite;
    Dtls13Epoch* e;
    byte enc, dec;

    WOLFSSL_ENTER("Dtls13SetEpochKeys");

    clientWrite = serverWrite = 0;
    enc = dec = 0;
    switch (side) {

    case ENCRYPT_SIDE_ONLY:
        if (ssl->options.side == WOLFSSL_CLIENT_END)
            clientWrite = 1;
        if (ssl->options.side == WOLFSSL_SERVER_END)
            serverWrite = 1;
        enc = 1;
        break;

    case DECRYPT_SIDE_ONLY:
        if (ssl->options.side == WOLFSSL_CLIENT_END)
            serverWrite = 1;
        if (ssl->options.side == WOLFSSL_SERVER_END)
            clientWrite = 1;
        dec = 1;
        break;

    case ENCRYPT_AND_DECRYPT_SIDE:
        clientWrite = serverWrite = 1;
        enc = dec = 1;
        break;
    }

    e = Dtls13GetEpoch(ssl, epochNumber);
    /* we don't have the requested key */
    if (e == NULL)
        return BAD_STATE_E;

    if (e->side != ENCRYPT_AND_DECRYPT_SIDE && e->side != side)
        return BAD_STATE_E;

    if (enc)
        ssl->dtls13EncryptEpoch = e;
    if (dec)
        ssl->dtls13DecryptEpoch = e;

    /* epoch 0 has no key to copy */
    if (w64IsZero(epochNumber))
        return 0;

    if (clientWrite) {
        XMEMCPY(ssl->keys.client_write_key, e->client_write_key,
            sizeof(ssl->keys.client_write_key));

        XMEMCPY(ssl->keys.client_write_IV, e->client_write_IV,
            sizeof(ssl->keys.client_write_IV));

        XMEMCPY(ssl->keys.client_sn_key, e->client_sn_key,
            sizeof(ssl->keys.client_sn_key));
    }

    if (serverWrite) {
        XMEMCPY(ssl->keys.server_write_key, e->server_write_key,
            sizeof(ssl->keys.server_write_key));

        XMEMCPY(ssl->keys.server_write_IV, e->server_write_IV,
            sizeof(ssl->keys.server_write_IV));

        XMEMCPY(ssl->keys.server_sn_key, e->server_sn_key,
            sizeof(ssl->keys.server_sn_key));
    }

    if (enc)
        XMEMCPY(ssl->keys.aead_enc_imp_IV, e->aead_enc_imp_IV,
            sizeof(ssl->keys.aead_enc_imp_IV));
    if (dec)
        XMEMCPY(ssl->keys.aead_dec_imp_IV, e->aead_dec_imp_IV,
            sizeof(ssl->keys.aead_dec_imp_IV));

    return SetKeysSide(ssl, side);
}

int Dtls13SetRecordNumberKeys(WOLFSSL* ssl, enum encrypt_side side)
{
    RecordNumberCiphers* enc = NULL;
    RecordNumberCiphers* dec = NULL;
    byte *encKey = NULL, *decKey = NULL;
    int ret;

    if (ssl == NULL) {
        return BAD_FUNC_ARG;
    }

    switch (side) {
    case ENCRYPT_SIDE_ONLY:
        enc = &ssl->dtlsRecordNumberEncrypt;
        break;
    case DECRYPT_SIDE_ONLY:
        dec = &ssl->dtlsRecordNumberDecrypt;
        break;
    case ENCRYPT_AND_DECRYPT_SIDE:
        enc = &ssl->dtlsRecordNumberEncrypt;
        dec = &ssl->dtlsRecordNumberDecrypt;
        break;
    }

    if (enc) {
        if (ssl->options.side == WOLFSSL_CLIENT_END)
            encKey = ssl->keys.client_sn_key;
        else
            encKey = ssl->keys.server_sn_key;
    }

    if (dec) {
        if (ssl->options.side == WOLFSSL_CLIENT_END)
            decKey = ssl->keys.server_sn_key;
        else
            decKey = ssl->keys.client_sn_key;
    }

    /* DTLSv1.3 supports only AEAD algorithm.  */
#if defined(BUILD_AESGCM) || defined(HAVE_AESCCM)
    if (ssl->specs.bulk_cipher_algorithm == wolfssl_aes_gcm ||
        ssl->specs.bulk_cipher_algorithm == wolfssl_aes_ccm) {

        if (enc) {
            ret = Dtls13InitAesCipher(ssl, enc, encKey, ssl->specs.key_size);
            if (ret != 0)
                return ret;
#ifdef WOLFSSL_DEBUG_TLS
            WOLFSSL_MSG("Provisioning AES Record Number enc key:");
            WOLFSSL_BUFFER(encKey, ssl->specs.key_size);
#endif /* WOLFSSL_DEBUG_TLS */
        }

        if (dec) {
            ret = Dtls13InitAesCipher(ssl, dec, decKey, ssl->specs.key_size);
            if (ret != 0)
                return ret;
#ifdef WOLFSSL_DEBUG_TLS
            WOLFSSL_MSG("Provisioning AES Record Number dec key:");
            WOLFSSL_BUFFER(decKey, ssl->specs.key_size);
#endif /* WOLFSSL_DEBUG_TLS */
        }

        return 0;
    }
#endif /* BUILD_AESGCM || HAVE_AESCCM */

#ifdef HAVE_CHACHA
    if (ssl->specs.bulk_cipher_algorithm == wolfssl_chacha) {
        if (enc) {
            ret = Dtls13InitChaChaCipher(enc, encKey, ssl->specs.key_size,
                ssl->heap);
            if (ret != 0)
                return ret;
#ifdef WOLFSSL_DEBUG_TLS
            WOLFSSL_MSG("Provisioning CHACHA Record Number enc key:");
            WOLFSSL_BUFFER(encKey, ssl->specs.key_size);
#endif /* WOLFSSL_DEBUG_TLS */
        }

        if (dec) {
            ret = Dtls13InitChaChaCipher(dec, decKey, ssl->specs.key_size,
                ssl->heap);
            if (ret != 0)
                return ret;
#ifdef WOLFSSL_DEBUG_TLS
            WOLFSSL_MSG("Provisioning CHACHA Record Number dec key:");
            WOLFSSL_BUFFER(decKey, ssl->specs.key_size);
#endif /* WOLFSSL_DEBUG_TLS */
        }

        return 0;
    }
#endif /* HAVE_CHACHA */

#ifdef HAVE_NULL_CIPHER
    if (ssl->specs.bulk_cipher_algorithm == wolfssl_cipher_null) {
#ifdef WOLFSSL_DEBUG_TLS
        WOLFSSL_MSG("Skipping Record Number key provisioning with null cipher");
#endif /* WOLFSSL_DEBUG_TLS */
        return 0;
    }
#endif /* HAVE_NULL_CIPHER */

    return NOT_COMPILED_IN;
}

/* 64 bits epoch + 64 bits sequence */
#define DTLS13_RN_SIZE 16

static int Dtls13GetAckListLength(Dtls13RecordNumber* list, word16* length)
{
    int numberElements;

    numberElements = 0;

    /* TODO: check that we don't exceed the maximum length */

    while (list != NULL) {
        list = list->next;
        numberElements++;
    }

    *length = (word16)(DTLS13_RN_SIZE * numberElements);
    return 0;
}

static int Dtls13WriteAckMessage(WOLFSSL* ssl,
    Dtls13RecordNumber* recordNumberList, word32* length)
{
    word16 msgSz, headerLength;
    byte *output, *ackMessage;
    word32 sendSz;
    int ret;

    sendSz = 0;

    if (ssl->dtls13EncryptEpoch == NULL)
        return BAD_STATE_E;

    if (w64IsZero(ssl->dtls13EncryptEpoch->epochNumber)) {
        /* unprotected ACK */
        headerLength = DTLS_RECORD_HEADER_SZ;
    }
    else {
        headerLength = Dtls13GetRlHeaderLength(ssl, 1);
        sendSz += MAX_MSG_EXTRA;
    }

    ret = Dtls13GetAckListLength(recordNumberList, &msgSz);
    if (ret != 0)
        return ret;

    sendSz += headerLength;

    /* ACK list 2 bytes length field */
    sendSz += OPAQUE16_LEN;

    /* ACK list */
    sendSz += msgSz;

    ret = CheckAvailableSize(ssl, sendSz);
    if (ret != 0)
        return ret;

    output = GetOutputBuffer(ssl);

    ackMessage = output + headerLength;

    c16toa(msgSz, ackMessage);
    ackMessage += OPAQUE16_LEN;

    while (recordNumberList != NULL) {
        c64toa(&recordNumberList->epoch, ackMessage);
        ackMessage += OPAQUE64_LEN;
        c64toa(&recordNumberList->seq, ackMessage);
        ackMessage += OPAQUE64_LEN;
        recordNumberList = recordNumberList->next;
    }

    *length = msgSz + OPAQUE16_LEN;

    return 0;
}

static int Dtls13RtxIsTrackedByRn(const Dtls13RtxRecord* r, w64wrapper epoch,
    w64wrapper seq)
{
    int i;
    if (!w64Equal(r->epoch, epoch))
        return 0;

    for (i = 0; i < r->rnIdx; ++i) {
        if (w64Equal(r->seq[i], seq))
            return 1;
    }

    return 0;
}

static int Dtls13KeyUpdateAckReceived(WOLFSSL* ssl)
{
    int ret;
    w64Increment(&ssl->dtls13Epoch);

    /* Epoch wrapped up */
    if (w64IsZero(ssl->dtls13Epoch))
        return BAD_STATE_E;

    ret = DeriveTls13Keys(ssl, update_traffic_key, ENCRYPT_SIDE_ONLY, 1);
    if (ret != 0)
        return ret;

    ret = Dtls13NewEpoch(ssl, ssl->dtls13Epoch, ENCRYPT_SIDE_ONLY);
    if (ret != 0)
        return ret;

    return Dtls13SetEpochKeys(ssl, ssl->dtls13Epoch, ENCRYPT_SIDE_ONLY);
}

#ifdef WOLFSSL_DEBUG_TLS
static void Dtls13PrintRtxRecord(Dtls13RtxRecord* r)
{
    int i;

    WOLFSSL_MSG_EX("r: hs: %d epoch: %ld", r->handshakeType, r->epoch);
    for (i = 0; i < r->rnIdx; i++)
        WOLFSSL_MSG_EX("seq: %ld", r->seq[i]);
}
#endif /* WOLFSSL_DEBUG_TLS */

static void Dtls13RtxRemoveRecord(WOLFSSL* ssl, w64wrapper epoch,
    w64wrapper seq)
{
    Dtls13RtxRecord *r, **prevNext;

    prevNext = &ssl->dtls13Rtx.rtxRecords;
    r = ssl->dtls13Rtx.rtxRecords;

    while (r != NULL) {
#ifdef WOLFSSL_DEBUG_TLS
        Dtls13PrintRtxRecord(r);
#endif /* WOLFSSL_DEBUG_TLS */

        if (Dtls13RtxIsTrackedByRn(r, epoch, seq)) {
#ifdef WOLFSSL_DEBUG_TLS
            WOLFSSL_MSG("removing record");
#endif /* WOLFSSL_DEBUG_TLS */
            Dtls13RtxRecordUnlink(ssl, prevNext, r);
            Dtls13FreeRtxBufferRecord(ssl, r);
            return;
        }
        prevNext = &r->next;
        r = r->next;
    }

    return;
}

int Dtls13DoScheduledWork(WOLFSSL* ssl)
{
    int ret;

    WOLFSSL_ENTER("Dtls13DoScheduledWork");

    ssl->dtls13SendingAckOrRtx = 1;

    if (ssl->dtls13Rtx.sendAcks) {
        ssl->dtls13Rtx.sendAcks = 0;
        ret = SendDtls13Ack(ssl);
        if (ret != 0)
            return ret;
    }

    if (ssl->dtls13Rtx.retransmit) {
        ssl->dtls13Rtx.retransmit = 0;
        ret = Dtls13RtxSendBuffered(ssl);
        if (ret != 0)
            return ret;
    }

    ssl->dtls13SendingAckOrRtx = 0;

    if (ssl->dtls13DoKeyUpdate) {
        ssl->dtls13DoKeyUpdate = 0;
        ret = Tls13UpdateKeys(ssl);
        if (ret != 0)
            return ret;
    }

    return 0;
}

/* Send ACKs when available after a timeout but only retransmit the last
 * flight after a long timeout */
int Dtls13RtxTimeout(WOLFSSL* ssl)
{
    int ret = 0;

    if (ssl->dtls13Rtx.seenRecords != NULL) {
        ssl->dtls13Rtx.sendAcks = 0;
        /* reset fast timeout as we are sending ACKs */
        ssl->dtls13FastTimeout = 0;
        ret = SendDtls13Ack(ssl);
        if (ret != 0)
            return ret;
    }

    /* we have two timeouts, a shorter (dtls13FastTimeout = 1) and a longer
       one. When the shorter expires we only send ACKs, as it normally means
       that some messages we are waiting for don't arrive yet. But we
       retransmit our buffered messages only if the longer timeout
       expires. fastTimeout is 1/4 of the longer timeout */
    if (ssl->dtls13FastTimeout) {
        ssl->dtls13FastTimeout = 0;
        return 0;
    }

    /* Increase timeout on long timeout */
    if (DtlsMsgPoolTimeout(ssl) != 0)
        return -1;

    return Dtls13RtxSendBuffered(ssl);
}

static int Dtls13RtxHasKeyUpdateBuffered(WOLFSSL* ssl)
{
    Dtls13RtxRecord* r = ssl->dtls13Rtx.rtxRecords;

    while (r != NULL) {
        if (r->handshakeType == key_update)
            return 1;

        r = r->next;
    }

    return 0;
}

int DoDtls13Ack(WOLFSSL* ssl, const byte* input, word32 inputSize,
    word32* processedSize)
{
    const byte* ackMessage;
    w64wrapper epoch, seq;
    word16 length;
    int ret;
    int i;

    if (inputSize < OPAQUE16_LEN)
        return BUFFER_ERROR;

    ato16(input, &length);

    if (inputSize < (word32)(OPAQUE16_LEN + length))
        return BUFFER_ERROR;

    if (length % (DTLS13_RN_SIZE) != 0)
        return PARSE_ERROR;

    ackMessage = input + OPAQUE16_LEN;
    for (i = 0; i < length; i += DTLS13_RN_SIZE) {
        ato64(ackMessage + i, &epoch);
        ato64(ackMessage + i + OPAQUE64_LEN, &seq);
        Dtls13RtxRemoveRecord(ssl, epoch, seq);
    }

    /* last client flight was completely acknowledged by the server. Handshake
       is complete. */
    if (ssl->options.side == WOLFSSL_CLIENT_END &&
        ssl->options.connectState == WAIT_FINISHED_ACK &&
        ssl->dtls13Rtx.rtxRecords == NULL) {
        ssl->options.serverState = SERVER_FINISHED_ACKED;
    }

    if (ssl->dtls13WaitKeyUpdateAck) {
        if (!Dtls13RtxHasKeyUpdateBuffered(ssl)) {
            /* we removed the KeyUpdate message because it was ACKed */
            ssl->dtls13WaitKeyUpdateAck = 0;
            ret = Dtls13KeyUpdateAckReceived(ssl);
            if (ret != 0)
                return ret;
        }
    }

    *processedSize = length + OPAQUE16_LEN;

    /* After the handshake, not retransmitting here may incur in some extra time
       in case a post-handshake authentication message is lost, because the ACK
       mechanism does not shortcut the retransmission timer. If, on the other
       hand, we retransmit we may do extra retransmissions of unrelated messages
       in the queue. ex: we send KeyUpdate, CertificateRequest that are
       unrelated between each other, receiving the ACK for the KeyUpdate will
       trigger re-sending the CertificateRequest before the timeout.*/
    /* TODO: be more smart about when doing retransmission looking in the
       retransmission queue or based on the type of message removed from the
       seen record list */
    if (ssl->dtls13Rtx.rtxRecords != NULL)
        ssl->dtls13Rtx.retransmit = 1;

    return 0;
}

int SendDtls13Ack(WOLFSSL* ssl)
{
    word32 outputSize;
    int headerSize;
    word32 length;
    byte* output;
    int ret;

    if (ssl->dtls13EncryptEpoch == NULL)
        return BAD_STATE_E;

    WOLFSSL_ENTER("SendDtls13Ack");

    ret = 0;

    /* The handshake is not complete and the client didn't setup the TRAFFIC0
       epoch yet */
    if (ssl->options.side == WOLFSSL_SERVER_END &&
        !ssl->options.handShakeDone &&
        w64GTE(ssl->dtls13Epoch, w64From32(0, DTLS13_EPOCH_TRAFFIC0))) {
        ret = Dtls13SetEpochKeys(ssl, w64From32(0, DTLS13_EPOCH_HANDSHAKE),
            ENCRYPT_SIDE_ONLY);
    }
    else if (!w64Equal(ssl->dtls13Epoch,
                 ssl->dtls13EncryptEpoch->epochNumber)) {
        ret = Dtls13SetEpochKeys(ssl, ssl->dtls13Epoch, ENCRYPT_SIDE_ONLY);
    }

    if (ret != 0)
        return ret;

    if (w64IsZero(ssl->dtls13EncryptEpoch->epochNumber)) {

        ret = Dtls13WriteAckMessage(ssl, ssl->dtls13Rtx.seenRecords, &length);
        if (ret != 0)
            return ret;

        output = GetOutputBuffer(ssl);

        ret = Dtls13RlAddPlaintextHeader(ssl, output, ack, (word16)length);
        if (ret != 0)
            return ret;

        ssl->buffers.outputBuffer.length += length + DTLS_RECORD_HEADER_SZ;
    }
    else {

        ret = Dtls13WriteAckMessage(ssl, ssl->dtls13Rtx.seenRecords, &length);
        if (ret != 0)
            return ret;

        output = GetOutputBuffer(ssl);

        outputSize = ssl->buffers.outputBuffer.bufferSize -
                     ssl->buffers.outputBuffer.idx -
                     ssl->buffers.outputBuffer.length;

        headerSize = Dtls13GetRlHeaderLength(ssl, 1);

        ret = BuildTls13Message(ssl, output, outputSize, output + headerSize,
            length, ack, 0, 0, 0);
        if (ret < 0)
            return ret;

        ssl->buffers.outputBuffer.length += ret;
    }

    Dtls13RtxFlushAcks(ssl);

    return SendBuffered(ssl);
}

static int Dtls13RtxRecordMatchesReqCtx(Dtls13RtxRecord* r, byte* ctx,
    byte ctxLen)
{
    if (r->handshakeType != certificate_request)
        return 0;
    if (r->length <= ctxLen + 1)
        return 0;
    return XMEMCMP(ctx, r->data + 1, ctxLen) == 0;
}

int Dtls13RtxProcessingCertificate(WOLFSSL* ssl, byte* input, word32 inputSize)
{
    Dtls13RtxRecord* rtxRecord = ssl->dtls13Rtx.rtxRecords;
    Dtls13RtxRecord** prevNext = &ssl->dtls13Rtx.rtxRecords;
    byte ctxLength;

    WOLFSSL_ENTER("Dtls13RtxProcessingCertificate");

    if (inputSize <= 1) {
        WOLFSSL_MSG("Malformed Certificate");
        return BAD_FUNC_ARG;
    }

    ctxLength = *input;

    if (inputSize < (word32)ctxLength + OPAQUE8_LEN) {
        WOLFSSL_MSG("Malformed Certificate");
        return BAD_FUNC_ARG;
    }

    while (rtxRecord != NULL) {
        if (Dtls13RtxRecordMatchesReqCtx(rtxRecord, input + 1, ctxLength)) {
            Dtls13RtxRecordUnlink(ssl, prevNext, rtxRecord);
            Dtls13FreeRtxBufferRecord(ssl, rtxRecord);
            return 0;
        }
        prevNext = &rtxRecord->next;
        rtxRecord = rtxRecord->next;
    }

    /* This isn't an error since we just can't find a Dtls13RtxRecord that
     * matches the Request Context. Request Context validity is checked
     * later. */
    WOLFSSL_MSG("Can't find any previous Certificate Request");
    return 0;
}

int wolfSSL_dtls13_has_pending_msg(WOLFSSL* ssl)
{
    return ssl->dtls13Rtx.rtxRecords != NULL;
}

#ifndef WOLFSSL_TLS13_IGNORE_AEAD_LIMITS
/* Limits specified by
 * https://www.rfc-editor.org/rfc/rfc9147.html#name-aead-limits
 * We specify the limit by which we need to do a key update as the halfway point
 * to the hard decryption fail limit. */
int Dtls13CheckAEADFailLimit(WOLFSSL* ssl)
{
    w64wrapper keyUpdateLimit;
    w64wrapper hardLimit;
    switch (ssl->specs.bulk_cipher_algorithm) {
#if defined(BUILD_AESGCM) || (defined(HAVE_CHACHA) && defined(HAVE_POLY1305))
        case wolfssl_aes_gcm:
        case wolfssl_chacha:
            hardLimit = DTLS_AEAD_AES_GCM_CHACHA_FAIL_LIMIT;
            keyUpdateLimit = DTLS_AEAD_AES_GCM_CHACHA_FAIL_KU_LIMIT;
            break;
#endif
#ifdef HAVE_AESCCM
        case wolfssl_aes_ccm:
            if (ssl->specs.aead_mac_size == AES_CCM_8_AUTH_SZ) {
                /* Limit is 2^7. The RFC recommends that
                 * "TLS_AES_128_CCM_8_SHA256 is not suitable for general use".
                 * We still should enforce the limit. */
                hardLimit = DTLS_AEAD_AES_CCM_8_FAIL_LIMIT;
                keyUpdateLimit = DTLS_AEAD_AES_CCM_8_FAIL_KU_LIMIT;
            }
            else {
                /* Limit is 2^23.5.
                 * Without the fraction is 11863283 (0x00B504F3)
                 * Half of this value is    5931641 (0x005A8279) */
                hardLimit = DTLS_AEAD_AES_CCM_FAIL_LIMIT;
                keyUpdateLimit = DTLS_AEAD_AES_CCM_FAIL_KU_LIMIT;
            }
            break;
#endif
        case wolfssl_cipher_null:
            /* No encryption being done. The MAC check must have failed. */
            return 0;
        default:
            WOLFSSL_MSG("Unrecognized ciphersuite for AEAD limit check");
            WOLFSSL_ERROR_VERBOSE(DECRYPT_ERROR);
            return DECRYPT_ERROR;
    }
    if (ssl->dtls13DecryptEpoch == NULL) {
        WOLFSSL_MSG("Dtls13CheckAEADFailLimit: ssl->dtls13DecryptEpoch should "
                    "not be NULL");
        WOLFSSL_ERROR_VERBOSE(BAD_STATE_E);
        return BAD_STATE_E;
    }
    w64Increment(&ssl->dtls13DecryptEpoch->dropCount);
    if (w64GT(ssl->dtls13DecryptEpoch->dropCount, hardLimit)) {
        /* We have reached the hard limit for failed decryptions. */
        WOLFSSL_MSG("Connection exceeded hard AEAD limit");
        WOLFSSL_ERROR_VERBOSE(DECRYPT_ERROR);
        return DECRYPT_ERROR;
    }
    else if (w64GT(ssl->dtls13DecryptEpoch->dropCount, keyUpdateLimit)) {
        WOLFSSL_MSG("Connection exceeded key update limit. Issuing key update");
        /* If not waiting for a response then request a key update. */
        if (!ssl->keys.updateResponseReq) {
            ssl->dtls13DoKeyUpdate = 1;
            ssl->dtls13InvalidateBefore = ssl->dtls13PeerEpoch;
            w64Increment(&ssl->dtls13InvalidateBefore);
        }
    }
    return 0;
}
#endif

#endif /* WOLFSSL_DTLS13 */
