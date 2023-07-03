#pragma once

#include <gui/modules/widget.h>

enum {
    SubmenuIndexCommonEmulate,
    SubmenuIndexCommonEdit,
    SubmenuIndexCommonInfo,
    SubmenuIndexCommonRename,
    SubmenuIndexCommonDelete,
    SubmenuIndexCommonMax,
};

void nfc_protocol_support_common_submenu_callback(void* context, uint32_t index);

void nfc_protocol_support_common_widget_callback(
    GuiButtonType result,
    InputType type,
    void* context);
