#include "nfc_protocol_support_common.h"

#include "../../nfc_app_i.h"

void nfc_protocol_support_common_submenu_callback(void* context, uint32_t index) {
    furi_assert(context);
    NfcApp* nfc = context;
    view_dispatcher_send_custom_event(nfc->view_dispatcher, index);
}
