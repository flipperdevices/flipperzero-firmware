#pragma once

#include "nfc_device.h"

#ifdef __cplusplus
extern "C" {
#endif

struct NfcDevice {
    NfcProtocol protocol;
    NfcDeviceData* protocol_data;

    NfcLoadingCallback loading_callback;
    void* loading_callback_context;
};

NfcDeviceData* nfc_device_get_data_ptr(NfcDevice* instance, NfcProtocol protocol);

#ifdef __cplusplus
}
#endif
