#include "iso14443_3a_i.h"

#include <nfc/protocols/iso14443_3a/iso14443_3a_poller.h>

#include "../../../nfc_app_i.h"

enum {
    SubmenuIndexEmulate = SubmenuIndexCommonMax,
    SubmenuIndexEdit,
};

void nfc_protocol_support_render_info_iso14443_3a_common(
    const Iso14443_3aData* data,
    NfcProtocolFormatType format_type,
    FuriString* str) {
    if(format_type == NfcProtocolFormatTypeFull) {
        const char iso_type = FURI_BIT(data->sak, 5) ? '4' : '3';
        furi_string_cat_printf(str, "ISO 14443-%c (NFC-A)\n", iso_type);
    }

    furi_string_cat_printf(str, "UID:");

    for(size_t i = 0; i < data->uid_len; i++) {
        furi_string_cat_printf(str, " %02X", data->uid[i]);
    }

    if(format_type == NfcProtocolFormatTypeFull) {
        furi_string_cat_printf(str, "\nATQA: %02X %02X ", data->atqa[1], data->atqa[0]);
        furi_string_cat_printf(str, " SAK: %02X", data->sak);
    }
}

static void nfc_protocol_support_render_info_iso14443_3a(
    const Iso14443_3aData* data,
    NfcProtocolFormatType format_type,
    FuriString* str) {
    UNUSED(format_type);
    nfc_protocol_support_render_info_iso14443_3a_common(data, NfcProtocolFormatTypeFull, str);
}

static NfcCustomEvent
    nfc_protocol_support_handle_poller_iso14443_3a(Iso14443_3aPollerEvent* event, void* context) {
    UNUSED(context);
    NfcCustomEvent custom_event = NfcCustomEventReadHandlerIgnore;

    if(event->type == Iso14443_3aPollerEventTypeReady) {
        custom_event = NfcCustomEventReadHandlerSuccess;
    }

    return custom_event;
}

void nfc_protocol_support_build_scene_saved_menu_iso14443_3a_common(NfcApp* instance) {
    Submenu* submenu = instance->submenu;
    submenu_add_item(
        submenu,
        "Emulate UID",
        SubmenuIndexEmulate,
        nfc_protocol_support_common_submenu_callback,
        instance);
    submenu_add_item(
        submenu,
        "Edit UID",
        SubmenuIndexEdit,
        nfc_protocol_support_common_submenu_callback,
        instance);
}

static void nfc_protocol_support_build_scene_saved_menu_iso14443_3a(NfcApp* instance) {
    nfc_protocol_support_build_scene_saved_menu_iso14443_3a_common(instance);
}

bool nfc_protocol_support_handle_scene_saved_menu_iso14443_3a_common(
    NfcApp* instance,
    uint32_t event) {
    switch(event) {
    case SubmenuIndexEmulate:
        scene_manager_next_scene(instance->scene_manager, NfcSceneNfcaEmulate);
        return true;
    case SubmenuIndexEdit:
        scene_manager_next_scene(instance->scene_manager, NfcSceneSetUid);
        return true;
    default:
        return false;
    }
}

static bool
    nfc_protocol_support_handle_scene_saved_menu_iso14443_3a(NfcApp* instance, uint32_t event) {
    return nfc_protocol_support_handle_scene_saved_menu_iso14443_3a_common(instance, event);
}

const NfcProtocolSupportBase nfc_protocol_support_iso14443_3a = {
    .features = NfcProtocolFeatureNone,
    .render_info = (NfcProtocolSupportRenderInfo)nfc_protocol_support_render_info_iso14443_3a,
    .handle_poller =
        (NfcProtocolSupportPollerHandler)nfc_protocol_support_handle_poller_iso14443_3a,
    .build_scene_saved_menu =
        (NfcProtocolSupportSceneBuilder)nfc_protocol_support_build_scene_saved_menu_iso14443_3a,
    .handle_scene_saved_menu =
        (NfcProtocolSupportSceneHandler)nfc_protocol_support_handle_scene_saved_menu_iso14443_3a,
};
