#include "iso15693_3_listener_i.h"

#define TAG "Iso15693_3Listener"

// static Iso15693_3Error iso15693_3_listener_process_nfc_error(NfcError error) {
//     Iso15693_3Error ret = Iso15693_3ErrorNone;
//
//     if(error == NfcErrorNone) {
//         ret = Iso15693_3ErrorNone;
//     } else if(error == NfcErrorTimeout) {
//         ret = Iso15693_3ErrorTimeout;
//     } else {
//         ret = Iso15693_3ErrorFieldOff;
//     }
//
//     return ret;
// }

Iso15693_3Error iso15693_3_listener_sleep(Iso15693_3Listener* instance) {
    furi_assert(instance);
    UNUSED(instance);

    return Iso15693_3ErrorNone;
}

Iso15693_3Error
    iso15693_3_listener_send_frame(Iso15693_3Listener* instance, const BitBuffer* tx_buffer) {
    furi_assert(instance);
    furi_assert(tx_buffer);
    UNUSED(instance);
    UNUSED(tx_buffer);

    return Iso15693_3ErrorNone;
}
