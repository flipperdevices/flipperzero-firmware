#include <nfc/protocols/mf_desfire/mf_desfire_poller.h>

#include "../iso14443_3a/iso14443_3a_i.h"
#include "../../../nfc_app_i.h"

static void nfc_protocol_support_render_info_mf_desfire(
    const MfDesfireData* data,
    NfcProtocolFormatType type,
    FuriString* str) {
    nfc_protocol_support_render_info_iso14443_3a_common(
        data->iso14443_4a_data->iso14443_3a_data, type, str);

    const uint32_t bytes_total = 1UL << (data->version.sw_storage >> 1);
    const uint32_t bytes_free = data->free_memory.is_present ? data->free_memory.bytes_free : 0;

    furi_string_cat_printf(str, "\n%lu", bytes_total);

    if(data->version.sw_storage & 1) {
        furi_string_push_back(str, '+');
    }

    furi_string_cat_printf(str, " bytes, %lu bytes free\n", bytes_free);

    const uint32_t app_count = simple_array_get_count(data->applications);
    uint32_t file_count = 0;

    for(uint32_t i = 0; i < app_count; ++i) {
        const MfDesfireApplication* app = simple_array_cget(data->applications, i);
        file_count += simple_array_get_count(app->file_ids);
    }

    furi_string_cat_printf(str, "%lu Application%s", app_count, app_count != 1 ? "s" : "");
    furi_string_cat_printf(str, ", %lu File%s", file_count, file_count != 1 ? "s" : "");
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

static bool nfc_protocol_support_handle_scene_read_success_mf_desfire(NfcApp* instance, uint32_t event) {
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
    .render_info = (NfcProtocolSupportRenderInfo)nfc_protocol_support_render_info_mf_desfire,
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
