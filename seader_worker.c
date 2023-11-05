#include "seader_worker_i.h"

#include <flipper_format/flipper_format.h>
#include <lib/lfrfid/tools/bit_lib.h>

#define TAG "SeaderWorker"

#define APDU_HEADER_LEN 5
#define ASN1_PREFIX 6
//#define ASN1_DEBUG true

#define RFAL_PICOPASS_TXRX_FLAGS                                                    \
    (FURI_HAL_NFC_LL_TXRX_FLAGS_CRC_TX_MANUAL | FURI_HAL_NFC_LL_TXRX_FLAGS_AGC_ON | \
     FURI_HAL_NFC_LL_TXRX_FLAGS_PAR_RX_REMV | FURI_HAL_NFC_LL_TXRX_FLAGS_CRC_RX_KEEP)

// TODO: const
uint8_t GET_RESPONSE[] = {0x00, 0xc0, 0x00, 0x00, 0xff};

#ifdef ASN1_DEBUG
char payloadDebug[384] = {0};
#endif

char display[SEADER_UART_RX_BUF_SIZE * 2 + 1] = {0};
char asn1_log[SEADER_UART_RX_BUF_SIZE] = {0};
bool requestPacs = true;

// Forward declaration
void seader_send_card_detected(SeaderUartBridge* seader_uart, CardDetails_t* cardDetails);

/***************************** Seader Worker API *******************************/

SeaderWorker* seader_worker_alloc() {
    SeaderWorker* seader_worker = malloc(sizeof(SeaderWorker));

    // Worker thread attributes
    seader_worker->thread =
        furi_thread_alloc_ex("SeaderWorker", 8192, seader_worker_task, seader_worker);
    seader_worker->messages = furi_message_queue_alloc(2, SEADER_UART_RX_BUF_SIZE);
    seader_worker->mq_mutex = furi_mutex_alloc(FuriMutexTypeNormal);

    seader_worker->callback = NULL;
    seader_worker->context = NULL;
    seader_worker->storage = furi_record_open(RECORD_STORAGE);
    memset(seader_worker->sam_version, 0, sizeof(seader_worker->sam_version));

    seader_worker_change_state(seader_worker, SeaderWorkerStateReady);

    return seader_worker;
}

void seader_worker_free(SeaderWorker* seader_worker) {
    furi_assert(seader_worker);

    furi_thread_free(seader_worker->thread);
    furi_message_queue_free(seader_worker->messages);
    furi_mutex_free(seader_worker->mq_mutex);

    furi_record_close(RECORD_STORAGE);

    free(seader_worker);
}

SeaderWorkerState seader_worker_get_state(SeaderWorker* seader_worker) {
    return seader_worker->state;
}

void seader_worker_start(
    SeaderWorker* seader_worker,
    SeaderWorkerState state,
    SeaderUartBridge* uart,
    SeaderCredential* credential,
    SeaderWorkerCallback callback,
    void* context) {
    furi_assert(seader_worker);
    furi_assert(uart);
    furi_assert(credential);

    seader_worker->callback = callback;
    seader_worker->context = context;
    seader_worker->uart = uart;
    seader_worker->credential = credential;
    seader_worker_change_state(seader_worker, state);
    furi_thread_start(seader_worker->thread);
}

void seader_worker_stop(SeaderWorker* seader_worker) {
    furi_assert(seader_worker);
    if(seader_worker->state == SeaderWorkerStateBroken ||
       seader_worker->state == SeaderWorkerStateReady) {
        return;
    }
    // seader_worker_disable_field();
    // nfc_poller_stop(poller);
    // nfc_poller_free(poller);
    seader_worker_change_state(seader_worker, SeaderWorkerStateStop);
    furi_thread_join(seader_worker->thread);
}

void seader_worker_change_state(SeaderWorker* seader_worker, SeaderWorkerState state) {
    seader_worker->state = state;
}

/***************************** Seader Worker Thread *******************************/

void* calloc(size_t count, size_t size) {
    return malloc(count * size);
}

bool seader_send_apdu(
    SeaderUartBridge* seader_uart,
    uint8_t CLA,
    uint8_t INS,
    uint8_t P1,
    uint8_t P2,
    uint8_t* payload,
    uint8_t length) {
    if(APDU_HEADER_LEN + length > SEADER_UART_RX_BUF_SIZE) {
        FURI_LOG_E(TAG, "Cannot send message, too long: %d", APDU_HEADER_LEN + length);
        return false;
    }

    uint8_t* apdu = malloc(APDU_HEADER_LEN + length);
    apdu[0] = CLA;
    apdu[1] = INS;
    apdu[2] = P1;
    apdu[3] = P2;
    apdu[4] = length;
    memcpy(apdu + APDU_HEADER_LEN, payload, length);

    seader_ccid_XfrBlock(seader_uart, apdu, APDU_HEADER_LEN + length);
    free(apdu);
    return true;
}

static int seader_asn_to_string(const void* buffer, size_t size, void* app_key) {
    if(app_key) {
        char* str = (char*)app_key;
        size_t next = strlen(str);
        strncpy(str + next, buffer, size);
    } else {
        uint8_t next = strlen(asn1_log);
        strncpy(asn1_log + next, buffer, size);
    }
    return 0;
}

bool seader_read_nfc(SeaderUartBridge* seader_uart) {
    UNUSED(seader_uart);
    return false;
}

bool seader_detect_nfc(SeaderWorker* seader_worker) {
    SeaderUartBridge* seader_uart = seader_worker->uart;

    while(seader_worker->state == SeaderWorkerStateRead14a) {
        // Card found
        if(seader_read_nfc(seader_uart)) {
            return true;
        }

        furi_delay_ms(100);
    }
    return false;
}

void seader_send_payload(
    SeaderUartBridge* seader_uart,
    Payload_t* payload,
    uint8_t to,
    uint8_t from,
    uint8_t replyTo) {
    uint8_t rBuffer[SEADER_UART_RX_BUF_SIZE] = {0};

    asn_enc_rval_t er = der_encode_to_buffer(
        &asn_DEF_Payload, payload, rBuffer + ASN1_PREFIX, sizeof(rBuffer) - ASN1_PREFIX);

#ifdef ASN1_DEBUG
    if(er.encoded > -1) {
        memset(payloadDebug, 0, sizeof(payloadDebug));
        (&asn_DEF_Payload)
            ->op->print_struct(&asn_DEF_Payload, payload, 1, seader_asn_to_string, payloadDebug);
        if(strlen(payloadDebug) > 0) {
            FURI_LOG_D(TAG, "Sending payload[%d %d %d]: %s", to, from, replyTo, payloadDebug);
        }
    }
#endif
    //0xa0, 0xda, 0x02, 0x63, 0x00, 0x00, 0x0a,
    //0x44, 0x0a, 0x44, 0x00, 0x00, 0x00, 0xa0, 0x02, 0x96, 0x00
    rBuffer[0] = to;
    rBuffer[1] = from;
    rBuffer[2] = replyTo;

    seader_send_apdu(seader_uart, 0xA0, 0xDA, 0x02, 0x63, rBuffer, 6 + er.encoded);
}

void seader_send_response(
    SeaderUartBridge* seader_uart,
    Response_t* response,
    uint8_t to,
    uint8_t from,
    uint8_t replyTo) {
    Payload_t* payload = 0;
    payload = calloc(1, sizeof *payload);
    assert(payload);

    payload->present = Payload_PR_response;
    payload->choice.response = *response;

    seader_send_payload(seader_uart, payload, to, from, replyTo);

    ASN_STRUCT_FREE(asn_DEF_Payload, payload);
}

void sendRequestPacs(SeaderUartBridge* seader_uart) {
    RequestPacs_t* requestPacs = 0;
    requestPacs = calloc(1, sizeof *requestPacs);
    assert(requestPacs);

    requestPacs->contentElementTag = ContentElementTag_implicitFormatPhysicalAccessBits;

    SamCommand_t* samCommand = 0;
    samCommand = calloc(1, sizeof *samCommand);
    assert(samCommand);

    samCommand->present = SamCommand_PR_requestPacs;
    samCommand->choice.requestPacs = *requestPacs;

    Payload_t* payload = 0;
    payload = calloc(1, sizeof *payload);
    assert(payload);

    payload->present = Payload_PR_samCommand;
    payload->choice.samCommand = *samCommand;

    seader_send_payload(seader_uart, payload, 0x44, 0x0a, 0x44);

    ASN_STRUCT_FREE(asn_DEF_RequestPacs, requestPacs);
    ASN_STRUCT_FREE(asn_DEF_SamCommand, samCommand);
    ASN_STRUCT_FREE(asn_DEF_Payload, payload);
}

void seader_worker_send_version(SeaderWorker* seader_worker) {
    SeaderUartBridge* seader_uart = seader_worker->uart;
    SamCommand_t* samCommand = 0;
    samCommand = calloc(1, sizeof *samCommand);
    assert(samCommand);

    samCommand->present = SamCommand_PR_version;

    Payload_t* payload = 0;
    payload = calloc(1, sizeof *payload);
    assert(payload);

    payload->present = Payload_PR_samCommand;
    payload->choice.samCommand = *samCommand;

    seader_send_payload(seader_uart, payload, 0x44, 0x0a, 0x44);

    ASN_STRUCT_FREE(asn_DEF_SamCommand, samCommand);
    ASN_STRUCT_FREE(asn_DEF_Payload, payload);
}

void seader_send_card_detected(SeaderUartBridge* seader_uart, CardDetails_t* cardDetails) {
    CardDetected_t* cardDetected = 0;
    cardDetected = calloc(1, sizeof *cardDetected);
    assert(cardDetected);

    cardDetected->detectedCardDetails = *cardDetails;

    SamCommand_t* samCommand = 0;
    samCommand = calloc(1, sizeof *samCommand);
    assert(samCommand);

    samCommand->present = SamCommand_PR_cardDetected;
    samCommand->choice.cardDetected = *cardDetected;

    Payload_t* payload = 0;
    payload = calloc(1, sizeof *payload);
    assert(payload);

    payload->present = Payload_PR_samCommand;
    payload->choice.samCommand = *samCommand;

    seader_send_payload(seader_uart, payload, 0x44, 0x0a, 0x44);

    ASN_STRUCT_FREE(asn_DEF_CardDetected, cardDetected);
    ASN_STRUCT_FREE(asn_DEF_SamCommand, samCommand);
    ASN_STRUCT_FREE(asn_DEF_Payload, payload);
}

bool seader_unpack_pacs(
    SeaderWorker* seader_worker,
    SeaderCredential* seader_credential,
    uint8_t* buf,
    size_t size) {
    PAC_t* pac = 0;
    pac = calloc(1, sizeof *pac);
    assert(pac);
    bool rtn = false;

    asn_dec_rval_t rval = asn_decode(0, ATS_DER, &asn_DEF_PAC, (void**)&pac, buf, size);

    if(rval.code == RC_OK) {
        char pacDebug[384] = {0};
        (&asn_DEF_PAC)->op->print_struct(&asn_DEF_PAC, pac, 1, seader_asn_to_string, pacDebug);
        if(strlen(pacDebug) > 0) {
            // FURI_LOG_D(TAG, "Received pac: %s", pacDebug);

            memset(display, 0, sizeof(display));
            if(seader_credential->sio[0] == 0x30) {
                for(uint8_t i = 0; i < sizeof(seader_credential->sio); i++) {
                    snprintf(
                        display + (i * 2), sizeof(display), "%02x", seader_credential->sio[i]);
                }
                // FURI_LOG_D(TAG, "SIO %s", display);
            }
        }

        if(pac->size <= sizeof(seader_credential->credential)) {
            // TODO: make credential into a 12 byte array
            seader_credential->bit_length = pac->size * 8 - pac->bits_unused;
            memcpy(&seader_credential->credential, pac->buf, pac->size);
            seader_credential->credential = __builtin_bswap64(seader_credential->credential);
            seader_credential->credential = seader_credential->credential >>
                                            (64 - seader_credential->bit_length);
            rtn = true;
        } else {
            // PACS too big (probably bad data)
            if(seader_worker->callback) {
                seader_worker->callback(SeaderWorkerEventFail, seader_worker->context);
            }
        }
    }

    ASN_STRUCT_FREE(asn_DEF_PAC, pac);
    return rtn;
}

//    800201298106683d052026b6820101
//300F800201298106683D052026B6820101
bool seader_parse_version(SeaderWorker* seader_worker, uint8_t* buf, size_t size) {
    SamVersion_t* version = 0;
    version = calloc(1, sizeof *version);
    assert(version);

    bool rtn = false;
    if(size > 30) {
        // Too large to handle now
        FURI_LOG_W(TAG, "Version of %d is to long to parse", size);
        return false;
    }
    // Add sequence prefix
    uint8_t seq[32] = {0x30};
    seq[1] = (uint8_t)size;
    memcpy(seq + 2, buf, size);

    asn_dec_rval_t rval =
        asn_decode(0, ATS_DER, &asn_DEF_SamVersion, (void**)&version, seq, size + 2);

    if(rval.code == RC_OK) {
        char versionDebug[128] = {0};
        (&asn_DEF_SamVersion)
            ->op->print_struct(
                &asn_DEF_SamVersion, version, 1, seader_asn_to_string, versionDebug);
        if(strlen(versionDebug) > 0) {
            // FURI_LOG_D(TAG, "Received version: %s", versionDebug);
        }
        if(version->version.size == 2) {
            memcpy(seader_worker->sam_version, version->version.buf, version->version.size);
        }

        rtn = true;
    }

    ASN_STRUCT_FREE(asn_DEF_SamVersion, version);
    return rtn;
}

bool seader_parse_sam_response(SeaderWorker* seader_worker, SamResponse_t* samResponse) {
    SeaderUartBridge* seader_uart = seader_worker->uart;
    SeaderCredential* credential = seader_worker->credential;

    if(samResponse->size == 0) {
        if(requestPacs) {
            // FURI_LOG_D(TAG, "samResponse %d => requesting PACS", samResponse->size);
            sendRequestPacs(seader_uart);
            requestPacs = false;
        } else {
            // FURI_LOG_D(TAG, "samResponse %d, no action", samResponse->size);
            if(seader_worker->callback) {
                seader_worker->callback(SeaderWorkerEventFail, seader_worker->context);
            }
        }
    } else if(seader_parse_version(seader_worker, samResponse->buf, samResponse->size)) {
        // no-op
    } else if(seader_unpack_pacs(seader_worker, credential, samResponse->buf, samResponse->size)) {
        if(seader_worker->callback) {
            seader_worker->callback(SeaderWorkerEventSuccess, seader_worker->context);
        }
    } else {
        memset(display, 0, sizeof(display));
        for(uint8_t i = 0; i < samResponse->size; i++) {
            snprintf(display + (i * 2), sizeof(display), "%02x", samResponse->buf[i]);
        }
        // FURI_LOG_D(TAG, "unknown samResponse %d: %s", samResponse->size, display);
    }

    return false;
}

bool seader_parse_response(SeaderWorker* seader_worker, Response_t* response) {
    switch(response->present) {
    case Response_PR_samResponse:
        seader_parse_sam_response(seader_worker, &response->choice.samResponse);
        break;
    default:
        break;
    };
    return false;
}

void seader_send_nfc_rx(SeaderUartBridge* seader_uart, uint8_t* buffer, size_t len) {
    OCTET_STRING_t rxData = {.buf = buffer, .size = len};
    uint8_t status[] = {0x00, 0x00};
    RfStatus_t rfStatus = {.buf = status, .size = 2};

    NFCRx_t* nfcRx = 0;
    nfcRx = calloc(1, sizeof *nfcRx);
    assert(nfcRx);

    nfcRx->rfStatus = rfStatus;
    nfcRx->data = &rxData;

    NFCResponse_t* nfcResponse = 0;
    nfcResponse = calloc(1, sizeof *nfcResponse);
    assert(nfcResponse);

    nfcResponse->present = NFCResponse_PR_nfcRx;
    nfcResponse->choice.nfcRx = *nfcRx;

    Response_t* response = 0;
    response = calloc(1, sizeof *response);
    assert(response);

    response->present = Response_PR_nfcResponse;
    response->choice.nfcResponse = *nfcResponse;

    seader_send_response(seader_uart, response, 0x14, 0x0a, 0x0);

    ASN_STRUCT_FREE(asn_DEF_NFCRx, nfcRx);
    ASN_STRUCT_FREE(asn_DEF_NFCResponse, nfcResponse);
    ASN_STRUCT_FREE(asn_DEF_Response, response);
}

/* Assumes this is called in the context of the NFC API callback */
NfcCommand seader_iso14443a_transmit(
    Seader* seader,
    uint8_t* buffer,
    size_t len,
    uint16_t timeout,
    uint8_t format[3]) {
    UNUSED(timeout);
    UNUSED(format);

    BitBuffer* tx_buffer = bit_buffer_alloc(len);
    BitBuffer* rx_buffer = bit_buffer_alloc(SEADER_POLLER_MAX_BUFFER_SIZE);
    NfcCommand ret = NfcCommandContinue;

    do {
        // bit_buffer_reset(tx_buffer);
        bit_buffer_append_bytes(tx_buffer, buffer, len);
        NfcError error = iso14443_4a_poller_send_block(seader->poller, tx_buffer, rx_buffer);
        if(error != NfcErrorNone) {
            FURI_LOG_W(TAG, "nfc_poller_trx error %d", error);
            ret = NfcCommandStop;
            break;
        }

        FURI_LOG_I(TAG, "NFC incoming %d bytes", bit_buffer_get_size_bytes(rx_buffer));

    } while(false);

    bit_buffer_free(tx_buffer);
    bit_buffer_free(rx_buffer);

    return ret;
}

NfcCommand seader_parse_nfc_command_transmit(Seader* seader, NFCSend_t* nfcSend) {
    long timeOut = nfcSend->timeOut;
    Protocol_t protocol = nfcSend->protocol;
    FrameProtocol_t frameProtocol = protocol.buf[1];

#ifdef ASN1_DEBUG
    memset(display, 0, sizeof(display));
    for(uint8_t i = 0; i < nfcSend->data.size; i++) {
        snprintf(display + (i * 2), sizeof(display), "%02x", nfcSend->data.buf[i]);
    }

    char protocolName[8] = {0};
    memset(protocolName, 0, sizeof(protocolName));
    (&asn_DEF_FrameProtocol)
        ->op->print_struct(
            &asn_DEF_FrameProtocol, &frameProtocol, 1, seader_asn_to_string, protocolName);

    FURI_LOG_D(
        TAG,
        "Transmit (%ld timeout) %d bytes [%s] via %s",
        timeOut,
        nfcSend->data.size,
        display,
        protocolName);
#endif

    if(frameProtocol == FrameProtocol_iclass) {
        // TODO
    } else if(frameProtocol == FrameProtocol_nfc) {
        return seader_iso14443a_transmit(
            seader, nfcSend->data.buf, nfcSend->data.size, (uint16_t)timeOut, nfcSend->format->buf);
    }
    return NfcCommandContinue;
}

NfcCommand seader_parse_nfc_off(SeaderUartBridge* seader_uart) {
    // FURI_LOG_D(TAG, "Set Field Off");
    // seader_worker_disable_field();
    // seader_nfc_scene_field_on_exit();
    // nfc_poller_stop(poller);
    // nfc_poller_free(poller);

    NFCResponse_t* nfcResponse = 0;
    nfcResponse = calloc(1, sizeof *nfcResponse);
    assert(nfcResponse);

    nfcResponse->present = NFCResponse_PR_nfcAck;

    Response_t* response = 0;
    response = calloc(1, sizeof *response);
    assert(response);

    response->present = Response_PR_nfcResponse;
    response->choice.nfcResponse = *nfcResponse;

    seader_send_response(seader_uart, response, 0x44, 0x0a, 0);

    ASN_STRUCT_FREE(asn_DEF_Response, response);
    ASN_STRUCT_FREE(asn_DEF_NFCResponse, nfcResponse);

    return NfcCommandStop;
}

NfcCommand seader_parse_nfc_command(Seader* seader, NFCCommand_t* nfcCommand) {
    // TODO: THIS IS WHERE I WANT TO PUSH EVENTS TO MESSAGE QUEUE

    SeaderWorker* seader_worker = seader->worker;
    SeaderUartBridge* seader_uart = seader_worker->uart;
    switch(nfcCommand->present) {
    case NFCCommand_PR_nfcSend:
        return seader_parse_nfc_command_transmit(seader, &nfcCommand->choice.nfcSend);
    case NFCCommand_PR_nfcOff:
        seader_parse_nfc_off(seader_uart);
        break;
    default:
        FURI_LOG_W(TAG, "unparsed NFCCommand");
        break;
    };

    return NfcCommandContinue;
}

bool seader_worker_state_machine(Seader* seader, Payload_t* payload) {
    SeaderWorker* seader_worker = seader->worker;
    FURI_LOG_D(TAG, "seader_worker_state_machine");
    switch(payload->present) {
    case Payload_PR_response:
        seader_parse_response(seader_worker, &payload->choice.response);
        break;
    case Payload_PR_nfcCommand:
        seader_parse_nfc_command(seader, &payload->choice.nfcCommand);
        break;
    case Payload_PR_errorResponse:
        // TODO: screen saying this was a failure, or maybe start over?
        if(seader_worker->callback) {
            seader_worker->callback(SeaderWorkerEventFail, seader_worker->context);
        }
        break;
    default:
        FURI_LOG_W(TAG, "unhandled payload");
        break;
    };

    return false;
}

bool seader_process_success_response_i(Seader* seader, uint8_t* apdu, size_t len) {
    Payload_t* payload = 0;
    payload = calloc(1, sizeof *payload);
    assert(payload);
    FURI_LOG_D(TAG, "seader_process_success_response_i");

    asn_dec_rval_t rval =
        asn_decode(0, ATS_DER, &asn_DEF_Payload, (void**)&payload, apdu + 6, len - 6);
    if(rval.code == RC_OK) {
#ifdef ASN1_DEBUG
        memset(payloadDebug, 0, sizeof(payloadDebug));
        (&asn_DEF_Payload)
            ->op->print_struct(&asn_DEF_Payload, payload, 1, seader_asn_to_string, payloadDebug);
        if(strlen(payloadDebug) > 0) {
            FURI_LOG_D(TAG, "Received payload: %s", payloadDebug);
        }
#endif
        seader_worker_state_machine(seader, payload);
    } else {
      FURI_LOG_D(TAG, "Failed to decode APDU payload");
    }

    ASN_STRUCT_FREE(asn_DEF_Payload, payload);
    return (rval.code == RC_OK);
}

bool seader_process_success_response(Seader* seader, uint8_t* apdu, size_t len) {
    SeaderWorker* seader_worker = seader->worker;

    // FIXME: use a more semantic method to break nfc related stuff out
    if (seader->poller) {
      FURI_LOG_I(TAG, "New SAM Message, %d bytes", len);
      uint32_t space = furi_message_queue_get_space(seader_worker->messages);
      if(space > 0) {
          BitBuffer* buffer = bit_buffer_alloc(SEADER_POLLER_MAX_BUFFER_SIZE);
          bit_buffer_append_bytes(buffer, apdu, len);

          if (furi_mutex_acquire(seader_worker->mq_mutex, FuriWaitForever) == FuriStatusOk) {
            furi_message_queue_put(seader_worker->messages, buffer, FuriWaitForever);
            furi_mutex_release(seader_worker->mq_mutex);
          }
          //bit_buffer_free(buffer);
      }
    } else {
      seader_process_success_response_i(seader, apdu, len);
    }
    return true;
}


bool seader_process_apdu(Seader* seader, uint8_t* apdu, size_t len) {
    SeaderWorker* seader_worker = seader->worker;
    SeaderUartBridge* seader_uart = seader_worker->uart;
    if(len < 2) {
        return false;
    }
    memset(display, 0, sizeof(display));
    for(uint8_t i = 0; i < len; i++) {
        snprintf(display + (i * 2), sizeof(display), "%02x", apdu[i]);
    }
    FURI_LOG_I(TAG, "APDU: %s", display);

    uint8_t SW1 = apdu[len - 2];
    uint8_t SW2 = apdu[len - 1];

    switch(SW1) {
    case 0x61:
        FURI_LOG_I(TAG, "Request %d bytes", SW2);
        GET_RESPONSE[4] = SW2;
        seader_ccid_XfrBlock(seader_uart, GET_RESPONSE, sizeof(GET_RESPONSE));
        return true;
        break;

    case 0x90:
        if(SW2 == 0x00) {
            if(len > 2) {
                return seader_process_success_response(seader, apdu, len - 2);
            }
        }
        break;
    }

    return false;
}

void seader_worker_process_sam_message(Seader* seader, CCID_Message* message) {
    seader_process_apdu(seader, message->payload, message->dwLength);
}

int32_t seader_worker_task(void* context) {
    SeaderWorker* seader_worker = context;
    SeaderUartBridge* seader_uart = seader_worker->uart;

    if(seader_worker->state == SeaderWorkerStateCheckSam) {
        seader_ccid_check_for_sam(seader_uart);
    } else if(seader_worker->state == SeaderWorkerStateReadPicopass) {
        // FURI_LOG_D(TAG, "Read Picopass");
        requestPacs = true;
        seader_credential_clear(seader_worker->credential);
        seader_worker->credential->type = SeaderCredentialTypePicopass;
        // TODO
    } else if(seader_worker->state == SeaderWorkerStateRead14a) {
        // FURI_LOG_D(TAG, "Read 14a");
        requestPacs = true;
        seader_credential_clear(seader_worker->credential);
        seader_worker->credential->type = SeaderCredentialType14A;
        // seader_nfc_scene_field_on_enter();
        if(!seader_detect_nfc(seader_worker)) {
            // Turn off if cancelled / no card found
            // seader_nfc_scene_field_on_exit();
        }
    }
    // FURI_LOG_D(TAG, "Worker Task Complete");
    seader_worker_change_state(seader_worker, SeaderWorkerStateReady);

    return 0;
}


NfcCommand seader_worker_card_detect(Seader* seader, uint8_t sak, uint8_t* atqa, const uint8_t* uid, uint8_t uid_len, uint8_t* ats, uint8_t ats_len) {
  UNUSED(ats);
  UNUSED(ats_len);
  SeaderWorker* seader_worker = seader->worker;
    SeaderUartBridge* seader_uart = seader_worker->uart;
    CardDetails_t* cardDetails = 0;
    cardDetails = calloc(1, sizeof *cardDetails);
    assert(cardDetails);

    OCTET_STRING_fromBuf(&cardDetails->csn, (const char*)uid, uid_len);
    uint8_t protocolBytes[] = {0x00, FrameProtocol_nfc};
    OCTET_STRING_fromBuf(
        &cardDetails->protocol, (const char*)protocolBytes, sizeof(protocolBytes));

    OCTET_STRING_t sak_string = {.buf = &sak, .size = 1};
    cardDetails->sak = &sak_string;

    OCTET_STRING_t atqa_string = {.buf = atqa, .size = 2};
    cardDetails->atqa = &atqa_string;

    seader_send_card_detected(seader_uart, cardDetails);

    ASN_STRUCT_FREE(asn_DEF_CardDetails, cardDetails);

    return NfcCommandContinue;
}

typedef enum {
    SeaderPollerEventTypeCardDetect,
    SeaderPollerEventTypeConversation,
    SeaderPollerEventTypeComplete,

    SeaderPollerEventTypeSuccess,
    SeaderPollerEventTypeFail,
} SeaderPollerEventType;

SeaderPollerEventType stage = SeaderPollerEventTypeCardDetect;

NfcCommand seader_worker_poller_callback_iso14443_4a(NfcGenericEvent event, void* context) {
    furi_assert(event.protocol == NfcProtocolIso14443_4a);

    Seader* seader = context;
    SeaderWorker* seader_worker = seader->worker;

    const Iso14443_4aPollerEvent* iso14443_4a_event = event.event_data;

    if(iso14443_4a_event->type == Iso14443_4aPollerEventTypeReady) {
        if(stage == SeaderPollerEventTypeCardDetect) {
            FURI_LOG_D(TAG, "Card Detect");
            nfc_device_set_data(
                seader->nfc_device, NfcProtocolIso14443_4a, nfc_poller_get_data(seader->poller));

            size_t uid_len;
            const uint8_t* uid = nfc_device_get_uid(seader->nfc_device, &uid_len);

            const Iso14443_3aData* iso14443_3a_data =
                nfc_device_get_data(seader->nfc_device, NfcProtocolIso14443_3a);
            uint8_t sak = iso14443_3a_get_sak(iso14443_3a_data);

            seader_worker_card_detect(seader, sak, (uint8_t*)iso14443_3a_data->atqa, uid, uid_len, NULL, 0);


            nfc_set_fdt_poll_fc(event.instance, SEADER_POLLER_MAX_FWT);
            furi_thread_set_current_priority(FuriThreadPriorityLowest);
            stage = SeaderPollerEventTypeConversation;
        } else if(stage == SeaderPollerEventTypeConversation) {

          if (furi_mutex_acquire(seader_worker->mq_mutex, 0) == FuriStatusOk) {
            furi_thread_set_current_priority(FuriThreadPriorityHighest);
            uint32_t count = furi_message_queue_get_count(seader_worker->messages);
            if(count > 0) {
                FURI_LOG_D(TAG, "Conversation: %ld messages", count);

                BitBuffer* message = bit_buffer_alloc(furi_message_queue_get_message_size(seader_worker->messages));
                FuriStatus status =
                    furi_message_queue_get(seader_worker->messages, message, FuriWaitForever);
                if(status != FuriStatusOk) {
                    FURI_LOG_W(TAG, "furi_message_queue_get fail %d", status);
                    return NfcCommandStop;
                }
                size_t len = bit_buffer_get_size_bytes(message);
                uint8_t* payload = (uint8_t*)bit_buffer_get_data(message);
                FURI_LOG_D(TAG, "Conversation: message length %d: %p", len, payload);

                seader_process_success_response_i(seader, payload, len);
                //bit_buffer_free(message);
            }
            furi_mutex_release(seader_worker->mq_mutex);
          }
          furi_thread_set_current_priority(FuriThreadPriorityLowest);
            // TESTING
            //return NfcCommandStop;

            // stage = SeaderPollerEventTypeComplete;
        } else if(stage == SeaderPollerEventTypeComplete) {
            FURI_LOG_D(TAG, "Complete");
            return NfcCommandStop;
        }
    }

    return NfcCommandContinue;
}
