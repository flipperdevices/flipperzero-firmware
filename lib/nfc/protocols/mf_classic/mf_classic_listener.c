#include "mf_classic_listener_i.h"
#include "mf_classic_listener_defs.h"

#include <furi.h>

#define TAG "MfClassicListener"

MfClassicListener*
    mf_classic_listener_alloc(Iso14443_3aListener* iso14443_3a_listener, MfClassicData* data) {
    MfClassicListener* instance = malloc(sizeof(MfClassicListener));
    instance->iso14443_3a_listener = iso14443_3a_listener;
    instance->data = mf_classic_alloc();
    mf_classic_copy(instance->data, data);

    return instance;
}

void mf_classic_listener_free(MfClassicListener* instance) {
    furi_assert(instance);

    free(instance);
}

void mf_classic_listener_set_callback(
    MfClassicListener* instance,
    NfcGenericCallback callback,
    void* context) {
    furi_assert(instance);

    instance->callback = callback;
    instance->context = context;
}

const MfClassicData* mf_classic_listener_get_data(const MfClassicListener* instance) {
    furi_assert(instance);
    furi_assert(instance->data);

    return instance->data;
}

NfcCommand mf_classic_listener_run(NfcGenericEvent event, void* context) {
    furi_assert(context);
    furi_assert(event.data);
    furi_assert(event.protocol == NfcProtocolIso14443_3a);

    NfcCommand command = NfcCommandContinue;

    return command;
}

const NfcListenerBase mf_classic_listener = {
    .alloc = (NfcListenerAlloc)mf_classic_listener_alloc,
    .free = (NfcListenerFree)mf_classic_listener_free,
    .set_callback = (NfcListenerSetCallback)mf_classic_listener_set_callback,
    .get_data = (NfcListenerGetData)mf_classic_listener_get_data,
    .run = (NfcListenerRun)mf_classic_listener_run,
};
