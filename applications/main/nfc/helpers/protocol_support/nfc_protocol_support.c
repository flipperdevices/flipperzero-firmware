#include "nfc_protocol_support.h"

#include "nfc_protocol_support_defs.h"
#include "../../nfc_app_i.h"

NfcProtocolFeature nfc_protocol_support_get_features(const NfcDevice* device) {
    const NfcProtocol protocol = nfc_device_get_protocol(device);
    return nfc_protocol_support[protocol]->features;
}

void nfc_protocol_support_render_info(
    const NfcDevice* device,
    NfcProtocolFormatType format_type,
    FuriString* str) {
    const NfcProtocol protocol = nfc_device_get_protocol(device);
    const NfcDeviceData* data = nfc_device_get_data(device, protocol);
    furi_string_cat_printf(str, "\e#%s\n", nfc_device_get_name(device, NfcDeviceNameTypeFull));
    nfc_protocol_support[protocol]->render_info(data, format_type, str);
}

NfcCustomEvent nfc_protocol_support_handle_read(NfcGenericEvent event, void* context) {
    furi_assert(context);
    NfcApp* nfc_app = context;

    NfcCustomEvent custom_event =
        nfc_protocol_support[event.protocol]->handle_read(event.data, context);
    if(custom_event == NfcCustomEventReadHandlerSuccess) {
        nfc_device_set_data(
            nfc_app->nfc_device, event.protocol, nfc_poller_get_data(nfc_app->poller));
    }

    return custom_event;
}
