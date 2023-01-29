#include <furi/furi.h>

#include <lib/nfc/nfc_device.h>

#include "mf_ultralight/all_in_one.h"

#define TAG "NfcParser"

int32_t nfc_parser_app(void* p) {
    NfcDeviceData* data = p;

    int32_t ret = 0;

    if(data) {
        if(data->protocol == NfcDeviceProtocolMifareUl) {
            MfUltralightData* mf_ul_data = &data->mf_ul_data;
            FURI_LOG_I(TAG, "Parse Ul data");
            if(all_in_one_verify(mf_ul_data)) {
                ret = all_in_one_parse(mf_ul_data, data->parsed_data);
            }
        }
    }

    return ret;
}
