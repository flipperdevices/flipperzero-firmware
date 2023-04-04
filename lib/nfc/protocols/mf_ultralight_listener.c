#include "mf_ultralight_listener.h"
#include "nfca_listener.h"

#include <furi.h>

#define TAG "MfUltralightListener"

#define MF_ULTRALIGHT_LISTENER_MAX_TX_BUFF_SIZE (32)

typedef enum {
    MfUltraligthListenerStateIdle,
    MfUltraligthListenerStateAuthSuccess,
} MfUltraligthListenerState;

typedef enum {
    MfUltralightListenerFeatureSupportReadVersion = (1U << 0),
    MfUltralightListenerFeatureSupportReadSignature = (1U << 1),
    MfUltralightListenerFeatureSupportReadCounter = (1U << 2),
    MfUltralightListenerFeatureSupportCheckTearingFlag = (1U << 3),
} MfUltralightListenerFeatureSupport;

struct MfUltralightListener {
    NfcaListener* nfca_listener;
    MfUltralightData data;
    uint8_t tx_data[MF_ULTRALIGHT_LISTENER_MAX_TX_BUFF_SIZE];
    uint16_t tx_bits;
    MfUltraligthListenerState state;
    uint16_t pages_total;
    MfUltralightListenerFeatureSupport features;
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
        memcpy(instance->tx_data, instance->data.page[start_page].data, 16);
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
    if((instance->features & MfUltralightListenerFeatureSupportReadVersion)) {
        memcpy(instance->tx_data, &instance->data.version, sizeof(instance->data.version));
        instance->tx_bits = sizeof(instance->data.version) * 8;
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
    if((instance->features & MfUltralightListenerFeatureSupportReadSignature)) {
        memcpy(instance->tx_data, &instance->data.signature, sizeof(instance->data.signature));
        instance->tx_bits = sizeof(instance->data.signature) * 8;
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
    MfUltralightData* data = &instance->data;
    if(data->type == MfUltralightTypeUnknown) {
        instance->features = MfUltralightListenerFeatureSupportReadVersion |
                             MfUltralightListenerFeatureSupportReadSignature;
        instance->pages_total = 16;
    }
}

MfUltralightListener* mf_ultralight_listener_alloc(MfUltralightData* data) {
    furi_assert(data);

    MfUltralightListener* instance = malloc(sizeof(MfUltralightListener));
    instance->data = *data;
    mf_ultralight_listener_prepare_emulation(instance);
    instance->nfca_listener = nfca_listener_alloc(&data->nfca_data);
    nfca_listener_set_callback(
        instance->nfca_listener, mf_ultralight_listener_event_handler, instance);

    return instance;
}

void mf_ultralight_listener_free(MfUltralightListener* instance) {
    furi_assert(instance);

    nfca_listener_free(instance->nfca_listener);
    free(instance);
}
