#include "mf_classic.h"
#include "mf_classic_render.h"

#include <nfc/protocols/mf_classic/mf_classic_poller.h>

#include "../nfc_protocol_support_gui_handlers.h"
#include "../../../nfc_app_i.h"

enum {
    SubmenuIndexDetectReader = SubmenuIndexCommonMax,
    SubmenuIndexWrite,
    SubmenuIndexUpdate,
};

static void nfc_protocol_support_render_info_mf_classic(
    const MfClassicData* data,
    NfcProtocolFormatType type,
    FuriString* str) {
    nfc_render_mf_classic_info(data, type, str);
}

static NfcCustomEvent
    nfc_protocol_support_handle_poller_mf_classic(MfClassicPollerEvent* event, void* context) {
    UNUSED(event);
    UNUSED(context);

    NfcCustomEvent custom_event = NfcCustomEventReadHandlerIgnore;
    // TODO: Implement read mf_classic using key cache
    return custom_event;
}

static void nfc_protocol_support_build_scene_saved_menu_mf_classic(NfcApp* instance) {
    Submenu* submenu = instance->submenu;
    const MfClassicData* data = nfc_device_get_data(instance->nfc_device, NfcProtocolMfClassic);

    if(!mf_classic_is_card_read(data)) {
        submenu_add_item(
            submenu,
            "Detect Reader",
            SubmenuIndexDetectReader,
            nfc_protocol_support_common_submenu_callback,
            instance);
    }
    submenu_add_item(
        submenu,
        "Write to Initial Card",
        SubmenuIndexWrite,
        nfc_protocol_support_common_submenu_callback,
        instance);
    submenu_add_item(
        submenu,
        "Update from Initial Card",
        SubmenuIndexUpdate,
        nfc_protocol_support_common_submenu_callback,
        instance);
}

static bool nfc_protocol_support_handle_scene_info_mf_classic(NfcApp* instance, uint32_t event) {
    if(event == GuiButtonTypeRight) {
        scene_manager_next_scene(instance->scene_manager, NfcSceneNotImplemented);
        return true;
    }

    return false;
}

static bool
    nfc_protocol_support_handle_scene_read_success_mf_classic(NfcApp* instance, uint32_t event) {
    if(event == GuiButtonTypeRight) {
        scene_manager_next_scene(instance->scene_manager, NfcSceneMfClassicMenu);
        return true;
    }

    return false;
}

static bool
    nfc_protocol_support_handle_scene_saved_menu_mf_classic(NfcApp* instance, uint32_t event) {
    switch(event) {
    case SubmenuIndexCommonEmulate:
        scene_manager_next_scene(instance->scene_manager, NfcSceneNotImplemented);
        return true;
    case SubmenuIndexDetectReader:
        scene_manager_next_scene(instance->scene_manager, NfcSceneNotImplemented);
        return true;
    case SubmenuIndexWrite:
        scene_manager_next_scene(instance->scene_manager, NfcSceneNotImplemented);
        return true;
    case SubmenuIndexUpdate:
        scene_manager_next_scene(instance->scene_manager, NfcSceneNotImplemented);
        return true;
    default:
        return false;
    }
}

const NfcProtocolSupportBase nfc_protocol_support_mf_classic = {
    .features = NfcProtocolFeatureMoreData | NfcProtocolFeatureEmulateFull,

    .render_info = (NfcProtocolSupportRenderData)nfc_protocol_support_render_info_mf_classic,

    .handle_poller =
        (NfcProtocolSupportPollerHandler)nfc_protocol_support_handle_poller_mf_classic,

    .build_scene_saved_menu =
        (NfcProtocolSupportSceneBuilder)nfc_protocol_support_build_scene_saved_menu_mf_classic,

    .handle_scene_info =
        (NfcProtocolSupportSceneHandler)nfc_protocol_support_handle_scene_info_mf_classic,
    .handle_scene_read_success =
        (NfcProtocolSupportSceneHandler)nfc_protocol_support_handle_scene_read_success_mf_classic,
    .handle_scene_saved_menu =
        (NfcProtocolSupportSceneHandler)nfc_protocol_support_handle_scene_saved_menu_mf_classic,
};
