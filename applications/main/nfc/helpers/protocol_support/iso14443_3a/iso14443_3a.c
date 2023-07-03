#include "iso14443_3a_i.h"
#include "iso14443_3a_render.h"

#include <nfc/protocols/iso14443_3a/iso14443_3a_poller.h>

#include "../nfc_protocol_support_gui_common.h"
#include "../../../nfc_app_i.h"

static void nfc_protocol_support_render_info_iso14443_3a(
    const Iso14443_3aData* data,
    NfcProtocolFormatType format_type,
    FuriString* str) {
    nfc_render_iso14443_3a_info(data, format_type, str);
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

static void nfc_protocol_support_build_scene_read_menu_iso14443_3a(NfcApp* instance) {
    UNUSED(instance);
}

static void nfc_protocol_support_build_scene_saved_menu_iso14443_3a(NfcApp* instance) {
    UNUSED(instance);
}

static bool nfc_protocol_support_handle_scene_info_iso14443_3a(NfcApp* instance, uint32_t event) {
    if(event == GuiButtonTypeRight) {
        scene_manager_next_scene(instance->scene_manager, NfcSceneNotImplemented);
        return true;
    }

    return false;
}

static bool
    nfc_protocol_support_handle_scene_read_menu_iso14443_3a(NfcApp* instance, uint32_t event) {
    if(event == SubmenuIndexCommonEmulate) {
        scene_manager_next_scene(instance->scene_manager, NfcSceneNfcaEmulate);
        return true;
    }

    return false;
}

bool nfc_protocol_support_handle_scene_saved_menu_iso14443_3a_common(
    NfcApp* instance,
    uint32_t event) {
    switch(event) {
    case SubmenuIndexCommonEmulate:
        scene_manager_next_scene(instance->scene_manager, NfcSceneNfcaEmulate);
        return true;
    case SubmenuIndexCommonEdit:
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
    .features = NfcProtocolFeatureEmulateUid | NfcProtocolFeatureEditUid,

    .render_info = (NfcProtocolSupportRenderData)nfc_protocol_support_render_info_iso14443_3a,

    .handle_poller =
        (NfcProtocolSupportPollerHandler)nfc_protocol_support_handle_poller_iso14443_3a,

    .build_scene_read_menu =
        (NfcProtocolSupportSceneBuilder)nfc_protocol_support_build_scene_read_menu_iso14443_3a,
    .build_scene_saved_menu =
        (NfcProtocolSupportSceneBuilder)nfc_protocol_support_build_scene_saved_menu_iso14443_3a,

    .handle_scene_info =
        (NfcProtocolSupportSceneHandler)nfc_protocol_support_handle_scene_info_iso14443_3a,
    .handle_scene_read_menu =
        (NfcProtocolSupportSceneHandler)nfc_protocol_support_handle_scene_read_menu_iso14443_3a,
    .handle_scene_saved_menu =
        (NfcProtocolSupportSceneHandler)nfc_protocol_support_handle_scene_saved_menu_iso14443_3a,
};
