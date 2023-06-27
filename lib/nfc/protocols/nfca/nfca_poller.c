#include "nfca_poller_i.h"

#include <nfc/protocols/nfc_poller_base.h>

#include <furi.h>

#define TAG "NFCA"

const NfcaData* nfca_poller_get_data(NfcaPoller* instance) {
    furi_assert(instance);
    furi_assert(instance->data);

    return instance->data;
}

static NfcaPoller* nfca_poller_alloc(Nfc* nfc) {
    furi_assert(nfc);

    NfcaPoller* instance = malloc(sizeof(NfcaPoller));
    instance->nfc = nfc;
    instance->tx_buffer = bit_buffer_alloc(NFCA_POLLER_MAX_BUFFER_SIZE);
    instance->rx_buffer = bit_buffer_alloc(NFCA_POLLER_MAX_BUFFER_SIZE);

    nfc_config(instance->nfc, NfcModeNfcaPoller);
    nfc_set_guard_time_us(instance->nfc, NFCA_GUARD_TIME_US);
    nfc_set_fdt_poll_fc(instance->nfc, NFCA_FDT_POLL_FC);
    nfc_set_fdt_poll_poll_us(instance->nfc, NFCA_POLL_POLL_MIN_US);
    instance->data = nfca_alloc();

    instance->nfca_event.data = &instance->nfca_event_data;
    instance->general_event.protocol_type = NfcProtocolTypeIso14443_3a;
    instance->general_event.data = &instance->nfca_event;
    instance->general_event.poller = instance;

    return instance;
}

static void nfca_poller_free_new(NfcaPoller* nfca_poller) {
    furi_assert(nfca_poller);

    NfcaPoller* instance = nfca_poller;
    furi_assert(instance->tx_buffer);
    furi_assert(instance->rx_buffer);
    furi_assert(instance->data);

    bit_buffer_free(instance->tx_buffer);
    bit_buffer_free(instance->rx_buffer);
    nfca_free(instance->data);
    free(instance);
}

static void
    nfca_poller_set_callback(NfcaPoller* instance, NfcPollerCallback callback, void* context) {
    furi_assert(instance);
    furi_assert(callback);

    instance->callback = callback;
    instance->context = context;
}

static NfcCommand nfca_poller_run(NfcPollerEvent event, void* context) {
    furi_assert(context);
    furi_assert(event.protocol_type == NfcProtocolTypeInvalid);
    furi_assert(event.data);

    NfcaPoller* instance = context;
    NfcEvent* nfc_event = event.data;
    NfcCommand command = NfcCommandContinue;

    if(nfc_event->type == NfcEventTypePollerReady) {
        if(instance->state != NfcaPollerStateActivated) {
            NfcaData data = {};
            NfcaError error = nfca_poller_async_activate(instance, &data);
            if(error == NfcaErrorNone) {
                instance->state = NfcaPollerStateActivated;
                instance->nfca_event.type = NfcaPollerEventTypeReady;
                instance->nfca_event_data.error = error;
                command = instance->callback(instance->general_event, instance->context);
            } else {
                instance->nfca_event.type = NfcaPollerEventTypeError;
                instance->nfca_event_data.error = error;
                command = instance->callback(instance->general_event, instance->context);
                // Add delay to switch context
                furi_delay_ms(100);
            }
        } else {
            instance->nfca_event.type = NfcaPollerEventTypeReady;
            instance->nfca_event_data.error = NfcaErrorNone;
            command = instance->callback(instance->general_event, instance->context);
        }
    }

    return command;
}

static bool nfca_poller_detect(NfcPollerEvent event, void* context) {
    furi_assert(context);
    furi_assert(event.data);
    furi_assert(event.poller);
    furi_assert(event.protocol_type = NfcProtocolTypeInvalid);

    bool protocol_detected = false;
    NfcaPoller* instance = context;
    NfcEvent* nfc_event = event.data;
    furi_assert(instance->state == NfcaPollerStateIdle);

    if(nfc_event->type == NfcEventTypePollerReady) {
        NfcaError error = nfca_poller_async_activate(instance, NULL);
        protocol_detected = (error == NfcaErrorNone);
    }

    return protocol_detected;
}

const NfcPollerBase nfc_poller_iso14443_3a = {
    .alloc = (NfcPollerAlloc)nfca_poller_alloc,
    .free = (NfcPollerFree)nfca_poller_free_new,
    .set_callback = (NfcPollerSetCallback)nfca_poller_set_callback,
    .run = (NfcPollerRun)nfca_poller_run,
    .detect = (NfcPollerDetect)nfca_poller_detect,
    .get_data = (NfcPollerGetData)nfca_poller_get_data,
};
