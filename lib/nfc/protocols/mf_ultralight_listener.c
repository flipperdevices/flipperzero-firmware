#include "mf_ultralight_listener.h"
#include "nfca_listener.h"

#include <furi.h>

#define TAG "MfUltralightListener"

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
    MfUltraligthListenerState state;
    uint16_t pages_total;
    MfUltralightListenerFeatureSupport features;
};

static void mf_ultralight_listener_event_handler(NfcaListenerEvent event, void* context) {
    furi_assert(context);

    MfUltralightListener* instance = context;
    uint8_t* rx_data = event.data.rx_data;
    uint16_t rx_bits = event.data.rx_bits;
    // TODO WHO own bufer???????
    // TODO implement rollback
    uint8_t tx_data[64] = {};
    uint16_t tx_bits = 0;
    if(event.type == NfcaListenerEventTypeReceivedStandartFrame) {
        if((rx_bits == 16) && (rx_data[0] == MF_ULTRALIGHT_CMD_READ_PAGE)) {
            uint8_t start_page = rx_data[1];
            if(instance->pages_total < start_page) {
                FURI_LOG_T(TAG, "Read page cmd: %d", start_page);
                instance->state = MfUltraligthListenerStateIdle;
                tx_data[0] = 0x00;
                tx_bits = 8;
                nfca_listener_tx(instance->nfca_listener, tx_data, tx_bits);
            } else {
                memcpy(tx_data, instance->data.page[start_page].data, 16);
                tx_bits = 16 * 8;
                nfca_listener_send_standart_frame(instance->nfca_listener, tx_data, tx_bits);
            }
        } else if((rx_bits == 8) && (rx_data[0] == MF_ULTRALIGHT_CMD_GET_VERSION)) {
            if(instance->features & MfUltralightListenerFeatureSupportReadVersion) {
                memcpy(tx_data, &instance->data.version, sizeof(instance->data.version));
                tx_bits = sizeof(instance->data.version) * 8;
                nfca_listener_send_standart_frame(instance->nfca_listener, tx_data, tx_bits);
            } else {
                instance->state = MfUltraligthListenerStateIdle;
                nfca_listener_sleep(instance->nfca_listener);
            }
        } else if((rx_bits == 16) && (rx_data[0] == MF_ULTRALIGTH_CMD_READ_SIG)) {
            if(instance->features & MfUltralightListenerFeatureSupportReadSignature) {
                // TODO implement rollback
                memcpy(tx_data, &instance->data.signature, sizeof(instance->data.signature));
                tx_bits = sizeof(instance->data.signature) * 8;
                nfca_listener_send_standart_frame(instance->nfca_listener, tx_data, tx_bits);
            } else {
                instance->state = MfUltraligthListenerStateIdle;
                nfca_listener_sleep(instance->nfca_listener);
            }
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
