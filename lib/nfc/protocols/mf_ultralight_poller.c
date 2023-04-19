#include "mf_ultralight_poller.h"

#include <furi.h>
#include "nfca_poller.h"
#include <nfc/helpers/nfc_poller_buffer.h>

#define TAG "MfUltralightPoller"

#define MF_ULTRALIGHT_MAX_BUFF_SIZE (64)

#define MF_ULTRALIGHT_POLLER_STANDART_FWT_FC (12000)

#define MF_ULTRALIGHT_POLLER_COMPLETE_EVENT (1UL << 0)

typedef struct {
    MfUltralightPoller* instance;
    FuriThreadId thread_id;
    MfUltralightError error;
} MfUltralightPollerContext;

typedef enum {
    MfUltralightPollerStateIdle,
    MfUltralightPollerStateReadVersion,
    MfUltralightPollerStateDetectNtag203,
    MfUltralightPollerStateReadSignature,
    MfUltralightPollerStateRead
} MfUltralightPollerState;

struct MfUltralightPoller {
    NfcaPoller* nfca_poller;
    MfUltralightPollerState state;
    NfcPollerBuffer* buffer;
    MfUltralightData* data;
    MfUltralightPollerCallback callback;
    void* context;
};

static MfUltralightError mf_ultralight_process_error(NfcaError error) {
    MfUltralightError ret = MfUltralightErrorNone;

    switch(error) {
    case NfcaErrorNone:
        ret = MfUltralightErrorNone;
        break;
    case NfcaErrorNotPresent:
        ret = MfUltralightErrorNotPresent;
        break;
    case NfcaErrorColResFailed:
    case NfcaErrorCommunication:
    case NfcaErrorWrongCrc:
        ret = MfUltralightErrorProtocol;
        break;
    case NfcaErrorTimeout:
        ret = MfUltralightErrorTimeout;
        break;
    default:
        ret = MfUltralightErrorProtocol;
        break;
    }

    return ret;
}

MfUltralightPoller* mf_ultralight_poller_alloc(NfcaPoller* nfca_poller) {
    MfUltralightPoller* instance = malloc(sizeof(MfUltralightPoller));
    instance->nfca_poller = nfca_poller;

    return instance;
}

void mf_ultralight_poller_free(MfUltralightPoller* instance) {
    furi_assert(instance);

    free(instance);
}

static MfUltralightError mf_ultralight_poller_get_version(MfUltralightPoller* instance) {
    NfcPollerBuffer* buff = instance->buffer;
    buff->tx_data[0] = MF_ULTRALIGHT_CMD_GET_VERSION;
    buff->tx_bits = 8;

    MfUltralightError ret = MfUltralightErrorNone;
    NfcaError error = NfcaErrorNone;
    do {
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            buff->tx_data,
            buff->tx_bits,
            buff->rx_data,
            buff->rx_data_size,
            &buff->rx_bits,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        if(error != NfcaErrorNone) {
            ret = mf_ultralight_process_error(error);
            break;
        }
        if(buff->rx_bits != 8 * 8) {
            ret = MfUltralightErrorProtocol;
            break;
        }
        memcpy(&instance->data->version, buff->rx_data, sizeof(MfUltralightVersion));
    } while(false);

    return ret;
}

static void mf_ultralight_nfca_poller_event_callback(NfcaPollerEvent event, void* context) {
    furi_assert(context);

    MfUltralightPoller* instance = context;
    MfUltralightPollerEvent mf_ul_poller_event = {};
    NfcaError error = NfcaErrorNone;

    if(event.type == NfcaPollerEventTypeReady) {
        NfcPollerBuffer* buff = instance->buffer;
        nfc_poller_buffer_reset(buff);

        if(instance->state == MfUltralightPollerStateReadVersion) {
            mf_ultralight_poller_get_version(instance);
        }
        // Test: read 0 page
        buff->tx_data[0] = MF_ULTRALIGHT_CMD_READ_PAGE;
        buff->tx_data[1] = 0;
        buff->tx_bits = 16;
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            buff->tx_data,
            buff->tx_bits,
            buff->rx_data,
            buff->rx_data_size,
            &buff->rx_bits,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        if(error == NfcaErrorNone) {
            FURI_LOG_T(TAG, "Read page 0 success");
            memcpy(&instance->data->page[0], buff->rx_data, buff->rx_bits / 8);
        } else {
            FURI_LOG_E(TAG, "Error reading 0 page");
        }
        mf_ul_poller_event.type = MfUltralightPollerEventTypeReadComplete;
        instance->callback(mf_ul_poller_event, instance->context);
    }
}

MfUltralightError mf_ultralight_poller_start(
    MfUltralightPoller* instance,
    NfcaPollerEventCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(instance->state == MfUltralightPollerStateIdle);
    furi_assert(instance->nfca_poller);
    furi_assert(callback);

    instance->data = malloc(sizeof(MfUltralightData));
    instance->buffer =
        nfc_poller_buffer_alloc(MF_ULTRALIGHT_MAX_BUFF_SIZE, MF_ULTRALIGHT_MAX_BUFF_SIZE);

    nfca_poller_start(instance->nfca_poller, callback, context);

    return MfUltralightErrorNone;
}

MfUltralightError mf_ultralight_poller_read(
    MfUltralightPoller* instance,
    MfUltralightPollerCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(instance->state == MfUltralightPollerStateIdle);
    furi_assert(instance->nfca_poller);
    furi_assert(callback);

    instance->callback = callback;
    instance->context = context;

    return mf_ultralight_poller_start(
        instance, mf_ultralight_nfca_poller_event_callback, instance);
}

MfUltralightError
    mf_ultralight_poller_get_data(MfUltralightPoller* instance, MfUltralightData* data) {
    furi_assert(instance);
    furi_assert(instance->data);
    furi_assert(data);

    *data = *instance->data;

    return MfUltralightErrorNone;
}

MfUltralightError mf_ultralight_poller_reset(MfUltralightPoller* instance) {
    furi_assert(instance);
    furi_assert(instance->data);
    furi_assert(instance->buffer);
    furi_assert(instance->nfca_poller);

    NfcaError error = nfca_poller_reset(instance->nfca_poller);

    nfc_poller_buffer_free(instance->buffer);
    instance->callback = NULL;
    instance->context = NULL;
    free(instance->data);
    instance->state = MfUltralightPollerStateIdle;

    return mf_ultralight_process_error(error);
}

// Called from NfcWorker thread

MfUltralightError mf_ultralight_poller_stop(MfUltralightPoller* instance) {
    furi_assert(instance);
    furi_assert(instance->nfca_poller);

    nfca_poller_stop(instance->nfca_poller);

    return MfUltralightErrorNone;
}

MfUltralightError mf_ultralight_poller_read_page(
    MfUltralightPoller* instance,
    uint16_t page,
    MfUltralightPage* data) {
    furi_assert(instance);
    furi_assert(data);

    uint8_t tx_data[] = {MF_ULTRALIGHT_CMD_READ_PAGE, page};
    uint16_t tx_bits = sizeof(tx_data) * 8;
    uint8_t rx_data[MF_ULTRALIGHT_MAX_BUFF_SIZE] = {};
    uint16_t rx_bits = 0;
    MfUltralightError ret = MfUltralightErrorNone;
    NfcaError error = NfcaErrorNone;

    do {
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            tx_data,
            tx_bits,
            rx_data,
            sizeof(rx_data),
            &rx_bits,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        if(error != NfcaErrorNone) {
            ret = mf_ultralight_process_error(error);
            break;
        }
        if(rx_bits != (MF_ULTRALIGHT_PAGE_SIZE * 4) * 8) {
            FURI_LOG_E(
                TAG, "Received %d bits. Expected %d bits", rx_bits, (MF_ULTRALIGHT_PAGE_SIZE)*8);
            ret = MfUltralightErrorProtocol;
            break;
        }
        memcpy(data, rx_data, MF_ULTRALIGHT_PAGE_SIZE);
    } while(false);

    return ret;
}

MfUltralightError mf_ultralight_poller_write_page(
    MfUltralightPoller* instance,
    uint16_t page,
    MfUltralightPage* data) {
    furi_assert(instance);
    furi_assert(data);

    uint8_t tx_data[MF_ULTRALIGHT_PAGE_SIZE + 2] = {MF_ULTRALIGHT_CMD_WRITE_PAGE, page};
    memcpy(&tx_data[2], data, MF_ULTRALIGHT_PAGE_SIZE);
    uint16_t tx_bits = (MF_ULTRALIGHT_PAGE_SIZE + 2) * 8;
    uint8_t rx_data[MF_ULTRALIGHT_MAX_BUFF_SIZE] = {};
    uint16_t rx_bits = 0;
    NfcaError error = NfcaErrorNone;
    MfUltralightError ret = MfUltralightErrorNone;

    do {
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            tx_data,
            tx_bits,
            rx_data,
            sizeof(rx_data),
            &rx_bits,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        if(!(error == NfcaErrorNone || error == NfcaErrorWrongCrc)) {
            ret = mf_ultralight_process_error(error);
            break;
        }
        if(rx_bits != 4) {
            ret = MfUltralightErrorProtocol;
            FURI_LOG_E(TAG, "Received %d bits. Expected %d bits", rx_bits, 4);
            break;
        }
        if(rx_data[0] != MF_ULTRALIGHT_CMD_ACK) {
            ret = MfUltralightErrorProtocol;
            FURI_LOG_E(TAG, "Nack received");
            break;
        }
    } while(false);

    return ret;
}

static void mf_ultraight_read_version_callback(NfcaPollerEvent event, void* context) {
    furi_assert(context);

    MfUltralightPollerContext* poller_context = context;
    if(event.type == NfcaPollerEventTypeReady) {
        poller_context->error = mf_ultralight_poller_get_version(poller_context->instance);
        nfca_poller_halt(poller_context->instance->nfca_poller);
    } else if(event.type == NfcaPollerEventTypeError) {
        poller_context->error = mf_ultralight_process_error(event.data.error);
    }
    mf_ultralight_poller_stop(poller_context->instance);
    furi_thread_flags_set(poller_context->thread_id, MF_ULTRALIGHT_POLLER_COMPLETE_EVENT);
}

MfUltralightError
    mf_ultralight_poller_read_version(MfUltralightPoller* instance, MfUltralightVersion* data) {
    furi_assert(instance);
    furi_assert(data);
    MfUltralightPollerContext poller_context = {};
    poller_context.instance = instance;
    poller_context.thread_id = furi_thread_get_current_id();

    mf_ultralight_poller_start(instance, mf_ultraight_read_version_callback, &poller_context);
    furi_thread_flags_wait(MF_ULTRALIGHT_POLLER_COMPLETE_EVENT, FuriFlagWaitAny, FuriWaitForever);
    *data = instance->data->version;
    mf_ultralight_poller_reset(instance);

    return poller_context.error;
}

MfUltralightError
    mf_ultralight_poller_read_signature(MfUltralightPoller* instance, MfUltralightSignature* data) {
    furi_assert(instance);
    furi_assert(data);

    uint8_t tx_data[] = {MF_ULTRALIGTH_CMD_READ_SIG, 0x00};
    uint16_t tx_bits = sizeof(tx_data) * 8;
    uint8_t rx_data[MF_ULTRALIGHT_MAX_BUFF_SIZE] = {};
    uint16_t rx_bits = 0;
    NfcaError error = NfcaErrorNone;
    MfUltralightError ret = MfUltralightErrorNone;

    do {
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            tx_data,
            tx_bits,
            rx_data,
            sizeof(rx_data),
            &rx_bits,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        if(error != NfcaErrorNone) {
            ret = mf_ultralight_process_error(error);
            break;
        }
        if(rx_bits != 32 * 8) {
            ret = MfUltralightErrorProtocol;
            FURI_LOG_E(TAG, "Received %d bits. Expected %d bits", rx_bits, 32 * 8);
            break;
        }
        memcpy(data, rx_data, sizeof(MfUltralightSignature));
    } while(false);

    return ret;
}

MfUltralightError mf_ultralight_poller_read_counter(
    MfUltralightPoller* instance,
    uint8_t counter_num,
    MfUltralightCounter* data) {
    furi_assert(instance);
    furi_assert(data);

    uint8_t tx_data[] = {MF_ULTRALIGHT_CMD_READ_CNT, counter_num};
    uint16_t tx_bits = sizeof(tx_data) * 8;
    uint8_t rx_data[MF_ULTRALIGHT_MAX_BUFF_SIZE] = {};
    uint16_t rx_bits = 0;
    NfcaError error = NfcaErrorNone;
    MfUltralightError ret = MfUltralightErrorNone;

    do {
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            tx_data,
            tx_bits,
            rx_data,
            sizeof(rx_data),
            &rx_bits,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        if(error != NfcaErrorNone) {
            ret = mf_ultralight_process_error(error);
            break;
        }
        if(rx_bits != 3 * 8) {
            ret = MfUltralightErrorProtocol;
            FURI_LOG_E(TAG, "Received %d bits. Expected %d bits", rx_bits, 32 * 8);
            break;
        }
        memcpy(data, rx_data, sizeof(MfUltralightCounter));
    } while(false);

    return ret;
}

MfUltralightError mf_ultralight_poller_read_tearing_flag(
    MfUltralightPoller* instance,
    uint8_t flag_num,
    MfUltralightTearingFlag* data) {
    furi_assert(instance);
    furi_assert(data);

    uint8_t tx_data[] = {MF_ULTRALIGHT_CMD_CHECK_TEARING, flag_num};
    uint16_t tx_bits = sizeof(tx_data) * 8;
    uint8_t rx_data[MF_ULTRALIGHT_MAX_BUFF_SIZE] = {};
    uint16_t rx_bits = 0;
    NfcaError error = NfcaErrorNone;
    MfUltralightError ret = MfUltralightErrorNone;

    do {
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            tx_data,
            tx_bits,
            rx_data,
            sizeof(rx_data),
            &rx_bits,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        if(error != NfcaErrorNone) {
            ret = mf_ultralight_process_error(error);
            break;
        }
        if(rx_bits != 8) {
            ret = MfUltralightErrorProtocol;
            FURI_LOG_E(TAG, "Received %d bits. Expected %d bits", rx_bits, 8);
            break;
        }
        memcpy(data, rx_data, sizeof(MfUltralightTearingFlag));
    } while(false);

    return ret;
}
