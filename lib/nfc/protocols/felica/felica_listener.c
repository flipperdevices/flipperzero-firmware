#include "felica_listener_i.h"

#include "nfc/protocols/nfc_listener_base.h"

#define FELICA_LISTENER_MAX_BUFFER_SIZE (64)
#define TAG "Felica"

FelicaListener* felica_listener_alloc(Nfc* nfc, FelicaData* data) {
    furi_assert(nfc);
    furi_assert(data);

    FelicaListener* instance = malloc(sizeof(FelicaListener));
    instance->nfc = nfc;
    instance->data = data;
    instance->tx_buffer = bit_buffer_alloc(FELICA_LISTENER_MAX_BUFFER_SIZE);

    //nfc_set_guard_time_us(instance->nfc, FELICA_GUARD_TIME_US);
    nfc_set_fdt_listen_fc(
        instance->nfc, FELICA_FDT_LISTEN_FC); //TODO verify FELICA_FDT_LISTEN_FC value

    nfc_config(instance->nfc, NfcModeListener, NfcTechFelica);
    //nfc_set_fdt_poll_poll_us(instance->nfc, FELICA_POLL_POLL_MIN_US);

    return instance;
}

void felica_listener_free(FelicaListener* instance) {
    furi_assert(instance);
    furi_assert(instance->tx_buffer);

    bit_buffer_free(instance->tx_buffer);
    free(instance);
}

void felica_listener_set_callback(
    FelicaListener* listener,
    NfcGenericCallback callback,
    void* context) {
    UNUSED(listener);
    UNUSED(callback);
    UNUSED(context);
}

const FelicaData* felica_listener_get_data(const FelicaListener* instance) {
    furi_assert(instance);
    furi_assert(instance->data);

    return instance->data;
}

NfcCommand felica_listener_run(NfcGenericEvent event, void* context) {
    furi_assert(context);
    furi_assert(event.protocol == NfcProtocolInvalid);
    furi_assert(event.event_data);
    furi_assert(context);
    furi_assert(event.protocol == NfcProtocolInvalid);
    furi_assert(event.event_data);

    FelicaListener* instance = context;
    NfcEvent* nfc_event = event.event_data;
    NfcCommand command = NfcCommandContinue;

    if(nfc_event->type == NfcEventTypeListenerActivated) {
        instance->state = Felica_ListenerStateActivated;
    } else if(nfc_event->type == NfcEventTypeFieldOff) {
        instance->state = Felica_ListenerStateIdle;
        /* if(instance->callback) {
            instance->iso14443_3a_event.type = Iso14443_3aListenerEventTypeFieldOff;
            instance->callback(instance->generic_event, instance->context);
        }*/
        command = NfcCommandSleep;
    } else if(nfc_event->type == NfcEventTypeRxEnd) {
        FURI_LOG_D(TAG, "Test");
    }
    return command;
}

const NfcListenerBase nfc_listener_felica = {
    .alloc = (NfcListenerAlloc)felica_listener_alloc,
    .free = (NfcListenerFree)felica_listener_free,
    .set_callback = (NfcListenerSetCallback)felica_listener_set_callback,
    .get_data = (NfcListenerGetData)felica_listener_get_data,
    .run = (NfcListenerRun)felica_listener_run,
};