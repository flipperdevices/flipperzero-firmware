#include <nfc/protocols/mf_classic/mf_classic_poller.h>

#include "../iso14443_3a/iso14443_3a_i.h"
#include "../../../nfc_app_i.h"

enum {
    SubmenuIndexEmulate = SubmenuIndexCommonMax,
    SubmenuIndexDetectReader,
    SubmenuIndexWrite,
    SubmenuIndexUpdate,
};

static void nfc_protocol_support_render_info_mf_classic(
    const MfClassicData* data,
    NfcProtocolFormatType type,
    FuriString* str) {
    nfc_protocol_support_render_info_iso14443_3a_common(data->iso14443_3a_data, type, str);

    uint8_t sectors_total = mf_classic_get_total_sectors_num(data->type);
    uint8_t keys_total = sectors_total * 2;
    uint8_t keys_found = 0;
    uint8_t sectors_read = 0;
    mf_classic_get_read_sectors_and_keys(data, &sectors_read, &keys_found);

    furi_string_cat_printf(str, "\nKeys Found: %u/%u", keys_found, keys_total);
    furi_string_cat_printf(str, "\nSectors Read: %u/%u", sectors_read, sectors_total);

    // TODO: Something else?
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
    submenu_add_item(
        submenu,
        "Emulate",
        SubmenuIndexEmulate,
        nfc_protocol_support_common_submenu_callback,
        instance);

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

static bool
    nfc_protocol_support_handle_scene_saved_menu_mf_classic(NfcApp* instance, uint32_t event) {
    switch(event) {
    case SubmenuIndexEmulate:
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
    .features = NfcProtocolFeatureMoreData,
    .render_info = (NfcProtocolSupportRenderInfo)nfc_protocol_support_render_info_mf_classic,
    .handle_poller =
        (NfcProtocolSupportPollerHandler)nfc_protocol_support_handle_poller_mf_classic,
    .build_scene_saved_menu =
        (NfcProtocolSupportSceneBuilder)nfc_protocol_support_build_scene_saved_menu_mf_classic,
    .handle_scene_saved_menu =
        (NfcProtocolSupportSceneHandler)nfc_protocol_support_handle_scene_saved_menu_mf_classic,
};
