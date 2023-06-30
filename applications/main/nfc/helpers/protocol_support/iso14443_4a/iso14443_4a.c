#include <nfc/protocols/iso14443_4a/iso14443_4a_poller.h>

#include "../iso14443_3a/iso14443_3a_i.h"
#include "../../../nfc_app_i.h"

static void nfc_protocol_support_render_info_iso14443_4a(
    const Iso14443_4aData* data,
    NfcProtocolFormatType type,
    FuriString* str) {
    UNUSED(type);
    nfc_protocol_support_render_info_iso14443_3a_common(
        data->iso14443_3a_data, NfcProtocolFormatTypeFull, str);
    // TODO: Add RATS info?
}

static NfcCustomEvent
    nfc_protocol_support_handle_poller_iso14443_4a(Iso14443_4aPollerEvent* event, void* context) {
    UNUSED(context);
    NfcCustomEvent custom_event = NfcCustomEventReadHandlerIgnore;

    if(event->type == Iso14443_4aPollerEventTypeReady) {
        custom_event = NfcCustomEventReadHandlerSuccess;
    }

    return custom_event;
}

static void nfc_protocol_support_build_scene_saved_menu_iso14443_4a(NfcApp* instance) {
    nfc_protocol_support_build_scene_saved_menu_iso14443_3a_common(instance);
}

static bool
    nfc_protocol_support_handle_scene_saved_menu_iso14443_4a(NfcApp* instance, uint32_t event) {
    return nfc_protocol_support_handle_scene_saved_menu_iso14443_3a_common(instance, event);
}

const NfcProtocolSupportBase nfc_protocol_support_iso14443_4a = {
    .features = NfcProtocolFeatureNone,
    .render_info = (NfcProtocolSupportRenderInfo)nfc_protocol_support_render_info_iso14443_4a,
    .handle_poller =
        (NfcProtocolSupportPollerHandler)nfc_protocol_support_handle_poller_iso14443_4a,
    .build_scene_saved_menu =
        (NfcProtocolSupportSceneBuilder)nfc_protocol_support_build_scene_saved_menu_iso14443_4a,
    .handle_scene_saved_menu =
        (NfcProtocolSupportSceneHandler)nfc_protocol_support_handle_scene_saved_menu_iso14443_4a,
};
