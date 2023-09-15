#pragma once

#include <gui/modules/widget.h>

#include "nfc/nfc_app.h"

enum {
    SubmenuIndexCommonSave,
    SubmenuIndexCommonEmulate,
    SubmenuIndexCommonEdit,
    SubmenuIndexCommonInfo,
    SubmenuIndexCommonRename,
    SubmenuIndexCommonDelete,
    SubmenuIndexCommonRestore,
    SubmenuIndexCommonMax,
};

void nfc_protocol_support_common_submenu_callback(void* context, uint32_t index);

void nfc_protocol_support_common_widget_callback(
    GuiButtonType result,
    InputType type,
    void* context);

void nfc_protocol_support_common_byte_input_done_callback(void* context);

void nfc_protocol_support_common_on_enter_empty(NfcApp* instance);

bool nfc_protocol_support_common_on_event_empty(NfcApp* instance, uint32_t event);
