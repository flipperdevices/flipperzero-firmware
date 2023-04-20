#include "mf_ultralight_listener.h"
#include "nfca_listener.h"

#include <furi.h>

#define TAG "MfUltralightListener"

#define MF_ULTRALIGHT_LISTENER_MAX_TX_BUFF_SIZE (32)

typedef enum {
    MfUltraligthListenerStateIdle,
    MfUltraligthListenerStateAuthSuccess,
} MfUltraligthListenerState;

struct MfUltralightListener {
    NfcaListener* nfca_listener;
    MfUltraligthListenerState state;
    MfUltralightData* data;
    uint8_t tx_data[MF_ULTRALIGHT_LISTENER_MAX_TX_BUFF_SIZE];
    uint16_t tx_bits;
    uint16_t pages_total;
    MfUltralightFeatureSupport features;
    MfUltralightListenerEventcallback callback;
    void* context;
};

typedef bool (*MfUltralightListenerCommandCallback)(
    MfUltralightListener* instance,
    uint8_t* rx_data,
    uint16_t rx_bits);

typedef struct {
    uint8_t cmd;
    uint16_t cmd_len_bits;
    MfUltralightListenerCommandCallback callback;
} MfUltralightListenerCommand;

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

static void mf_ultralight_listener_send_short_resp(MfUltralightListener* instance, uint8_t data) {
    nfca_listener_tx(instance->nfca_listener, &data, 4);
};

static bool mf_ultralight_listener_read_page_handler(
    MfUltralightListener* instance,
    uint8_t* rx_data,
    uint16_t rx_bits) {
    UNUSED(rx_bits);
    bool command_processed = false;
    uint8_t start_page = rx_data[1];
    if(instance->pages_total < start_page) {
        mf_ultralight_listener_send_short_resp(instance, MF_ULTRALIGHT_CMD_NACK);
        instance->state = MfUltraligthListenerStateIdle;
    } else {
        memcpy(instance->tx_data, instance->data->page[start_page].data, 16);
        instance->tx_bits = 16 * 8;
        nfca_listener_send_standart_frame(
            instance->nfca_listener, instance->tx_data, instance->tx_bits);
    }
    command_processed = true;

    return command_processed;
}

static bool mf_ultralight_listener_read_version_handler(
    MfUltralightListener* instance,
    uint8_t* rx_data,
    uint16_t rx_bits) {
    UNUSED(rx_bits);
    UNUSED(rx_data);
    bool command_processed = false;
    if((instance->features & MfUltralightFeatureSupportReadVersion)) {
        memcpy(instance->tx_data, &instance->data->version, sizeof(instance->data->version));
        instance->tx_bits = sizeof(instance->data->version) * 8;
        nfca_listener_send_standart_frame(
            instance->nfca_listener, instance->tx_data, instance->tx_bits);
    } else {
        nfca_listener_sleep(instance->nfca_listener);
        instance->state = MfUltraligthListenerStateIdle;
    }
    command_processed = true;

    return command_processed;
}

static bool mf_ultralight_listener_read_signature_handler(
    MfUltralightListener* instance,
    uint8_t* rx_data,
    uint16_t rx_bits) {
    UNUSED(rx_bits);
    UNUSED(rx_data);
    bool command_processed = false;
    if((instance->features & MfUltralightFeatureSupportReadSignature)) {
        memcpy(instance->tx_data, &instance->data->signature, sizeof(instance->data->signature));
        instance->tx_bits = sizeof(instance->data->signature) * 8;
        nfca_listener_send_standart_frame(
            instance->nfca_listener, instance->tx_data, instance->tx_bits);
    } else {
        nfca_listener_sleep(instance->nfca_listener);
        instance->state = MfUltraligthListenerStateIdle;
    }
    command_processed = true;

    return command_processed;
}

static const MfUltralightListenerCommand mf_ultralight_command[] = {
    {
        .cmd = MF_ULTRALIGHT_CMD_READ_PAGE,
        .cmd_len_bits = 16,
        .callback = mf_ultralight_listener_read_page_handler,
    },
    {
        .cmd = MF_ULTRALIGHT_CMD_GET_VERSION,
        .cmd_len_bits = 8,
        .callback = mf_ultralight_listener_read_version_handler,
    },
    {
        .cmd = MF_ULTRALIGTH_CMD_READ_SIG,
        .cmd_len_bits = 16,
        .callback = mf_ultralight_listener_read_signature_handler,
    },
};

static void mf_ultralight_listener_event_handler(NfcaListenerEvent event, void* context) {
    furi_assert(context);

    MfUltralightListener* instance = context;
    uint8_t* rx_data = event.data.rx_data;
    uint16_t rx_bits = event.data.rx_bits;
    if(event.type == NfcaListenerEventTypeReceivedStandartFrame) {
        bool cmd_processed = false;
        for(size_t i = 0; i < COUNT_OF(mf_ultralight_command); i++) {
            if(rx_bits != mf_ultralight_command[i].cmd_len_bits) continue;
            if(rx_data[0] != mf_ultralight_command[i].cmd) continue;
            cmd_processed = mf_ultralight_command[i].callback(instance, rx_data, rx_bits);
            if(cmd_processed) break;
        }
        if(!cmd_processed) {
            mf_ultralight_listener_send_short_resp(instance, MF_ULTRALIGHT_CMD_NACK);
        }
    }
}

static void mf_ultralight_listener_prepare_emulation(MfUltralightListener* instance) {
    MfUltralightData* data = instance->data;
    if(data->type == MfUltralightTypeUnknown) {
        instance->features = MfUltralightFeatureSupportReadVersion |
                             MfUltralightFeatureSupportReadSignature;
        instance->pages_total = 16;
    }
}

MfUltralightListener* mf_ultralight_listener_alloc(NfcaListener* nfca_listener) {
    furi_assert(nfca_listener);

    MfUltralightListener* instance = malloc(sizeof(MfUltralightListener));

    return instance;
}

MfUltralightError mf_ultralight_listener_start(
    MfUltralightListener* instance,
    MfUltralightData* data,
    MfUltralightListenerEventcallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(data);

    instance->data = malloc(sizeof(MfUltralightData));
    *instance->data = *data;
    mf_ultralight_listener_prepare_emulation(instance);

    instance->callback = callback;
    instance->context = context;

    NfcaError error = nfca_listener_start(
        instance->nfca_listener,
        &instance->data->nfca_data,
        mf_ultralight_listener_event_handler,
        instance);
    
    return mf_ultralight_process_error(error);
}

void mf_ultralight_listener_free(MfUltralightListener* instance) {
    furi_assert(instance);

    free(instance);
}

MfUltralightError mf_ultralight_listener_reset(MfUltralightListener* instance) {
    furi_assert(instance);
    furi_assert(instance->data);

    NfcaError error = nfca_listener_reset(instance->nfca_listener);
    instance->state = MfUltraligthListenerStateIdle;
    free(instance->data);

    return mf_ultralight_process_error(error);
}
