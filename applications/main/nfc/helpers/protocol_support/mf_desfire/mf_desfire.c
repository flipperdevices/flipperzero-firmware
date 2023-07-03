#include "mf_desfire.h"
#include "mf_desfire_render.h"

#include <nfc/protocols/mf_desfire/mf_desfire_poller.h>

#include "../nfc_protocol_support_gui_common.h"
#include "../../../nfc_app_i.h"

static void nfc_protocol_support_render_info_mf_desfire(
    const MfDesfireData* data,
    NfcProtocolFormatType format_type,
    FuriString* str) {
    nfc_render_mf_desfire_info(data, format_type, str);
}

static NfcCustomEvent
    nfc_protocol_support_handle_poller_mf_desfire(MfDesfirePollerEvent* event, void* context) {
    UNUSED(context);
    NfcCustomEvent custom_event = NfcCustomEventReadHandlerIgnore;

    if(event->type == MfDesfirePollerEventTypeReadSuccess) {
        custom_event = NfcCustomEventReadHandlerSuccess;
    }

    return custom_event;
}

static void nfc_protocol_support_build_scene_saved_menu_mf_desfire(NfcApp* instance) {
    UNUSED(instance);
}

static bool nfc_protocol_support_handle_scene_info_mf_desfire(NfcApp* instance, uint32_t event) {
    if(event == GuiButtonTypeRight) {
        scene_manager_next_scene(instance->scene_manager, NfcSceneMfDesfireData);
        return true;
    }

    return false;
}

static bool
    nfc_protocol_support_handle_scene_read_success_mf_desfire(NfcApp* instance, uint32_t event) {
    if(event == GuiButtonTypeRight) {
        scene_manager_next_scene(instance->scene_manager, NfcSceneMfDesfireMenu);
        return true;
    }

    return false;
}

static bool
    nfc_protocol_support_handle_scene_saved_menu_mf_desfire(NfcApp* instance, uint32_t event) {
    switch(event) {
    case SubmenuIndexCommonEmulate:
        scene_manager_next_scene(instance->scene_manager, NfcSceneNfcaEmulate);
        return true;
    default:
        return false;
    }
}

const NfcProtocolSupportBase nfc_protocol_support_mf_desfire = {
    .features = NfcProtocolFeatureMoreData | NfcProtocolFeatureEmulateUid,

    .render_info = (NfcProtocolSupportRenderData)nfc_protocol_support_render_info_mf_desfire,

    .handle_poller =
        (NfcProtocolSupportPollerHandler)nfc_protocol_support_handle_poller_mf_desfire,

    .build_scene_saved_menu =
        (NfcProtocolSupportSceneBuilder)nfc_protocol_support_build_scene_saved_menu_mf_desfire,

    .handle_scene_info =
        (NfcProtocolSupportSceneHandler)nfc_protocol_support_handle_scene_info_mf_desfire,
    .handle_scene_read_success =
        (NfcProtocolSupportSceneHandler)nfc_protocol_support_handle_scene_read_success_mf_desfire,
    .handle_scene_saved_menu =
        (NfcProtocolSupportSceneHandler)nfc_protocol_support_handle_scene_saved_menu_mf_desfire,
};
