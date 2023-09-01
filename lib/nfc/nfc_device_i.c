#include "nfc_device_i.h"
#include "protocols/nfc_device_defs.h"

#include <furi/furi.h>

static NfcDeviceData*
    nfc_device_search_base_protocol_data(NfcDevice* instance, NfcProtocol protocol) {
    NfcDevice nfc_dev_tmp = {
        .protocol = instance->protocol,
        .protocol_data = instance->protocol_data,
    };

    while(true) {
        NfcProtocol protocol_tmp = nfc_protocol_get_parent(nfc_dev_tmp.protocol);
        NfcDeviceData* dev_data_tmp =
            nfc_devices[nfc_dev_tmp.protocol]->get_base_data(nfc_dev_tmp.protocol_data);
        nfc_dev_tmp.protocol = protocol_tmp;
        nfc_dev_tmp.protocol_data = dev_data_tmp;
        if(nfc_dev_tmp.protocol == protocol) {
            break;
        }
    }

    return nfc_dev_tmp.protocol_data;
}

NfcDeviceData* nfc_device_get_data_ptr(NfcDevice* instance, NfcProtocol protocol) {
    furi_assert(instance);
    furi_assert(protocol < NfcProtocolNum);

    NfcDeviceData* dev_data = NULL;

    if(instance->protocol == protocol) {
        dev_data = instance->protocol_data;
    } else if(nfc_protocol_has_parent(instance->protocol, protocol)) {
        dev_data = nfc_device_search_base_protocol_data(instance, protocol);
    } else {
        furi_crash("Incorrect protocol");
    }

    return dev_data;
}
