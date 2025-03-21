#include "nfc_cli_dump_mf_ultralight.h"
#include <nfc/protocols/mf_ultralight/mf_ultralight_poller.h>

NfcCommand nfc_cli_dump_poller_callback_mf_ultralight(NfcGenericEvent event, void* context) {
    furi_assert(event.protocol == NfcProtocolMfUltralight);
    NfcCliDumpContext* instance = context;
    const MfUltralightPollerEvent* mf_ultralight_event = event.event_data;
    if(mf_ultralight_event->type == MfUltralightPollerEventTypeReadSuccess) {
        nfc_device_set_data(
            instance->nfc_device, NfcProtocolMfUltralight, nfc_poller_get_data(instance->poller));

        // const MfUltralightData* data =
        //     nfc_device_get_data(instance->nfc_device, NfcProtocolMfUltralight);
        // uint32_t event = (data->pages_read == data->pages_total) ? NfcCustomEventPollerSuccess :
        //                                                            NfcCustomEventPollerIncomplete;
        furi_semaphore_release(instance->sem_done);
        return NfcCommandStop;
    } else if(mf_ultralight_event->type == MfUltralightPollerEventTypeAuthRequest) {
        nfc_device_set_data(
            instance->nfc_device, NfcProtocolMfUltralight, nfc_poller_get_data(instance->poller));
        // const MfUltralightData* data =
        //     nfc_device_get_data(instance->nfc_device, NfcProtocolMfUltralight);
        mf_ultralight_event->data->auth_context.skip_auth = true;
        mf_ultralight_event->data->auth_context.password = instance->key.password;
        mf_ultralight_event->data->auth_context.tdes_key = instance->key.tdes_key;

    } else if(mf_ultralight_event->type == MfUltralightPollerEventTypeAuthSuccess) {
        //instance->pack = mf_ultralight_event->data->auth_context.pack;
    }
    return NfcCommandContinue;
}
