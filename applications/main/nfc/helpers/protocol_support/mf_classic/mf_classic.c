#include "mf_classic.h"
#include "mf_classic_render.h"

#include <nfc/protocols/mf_classic/mf_classic_poller.h>

#include "nfc/nfc_app_i.h"

#include "../nfc_protocol_support_gui_common.h"

enum {
    SubmenuIndexDetectReader = SubmenuIndexCommonMax,
    SubmenuIndexWrite,
    SubmenuIndexUpdate,
};

static void nfc_scene_info_on_enter_mf_classic(NfcApp* instance) {
    const NfcDevice* device = instance->nfc_device;
    const MfClassicData* data = nfc_device_get_data(device, NfcProtocolMfClassic);

    FuriString* temp_str = furi_string_alloc();
    furi_string_cat_printf(
        temp_str, "\e#%s\n", nfc_device_get_name(device, NfcDeviceNameTypeFull));
    nfc_render_mf_classic_info(data, NfcProtocolFormatTypeFull, temp_str);

    widget_add_text_scroll_element(
        instance->widget, 0, 0, 128, 52, furi_string_get_cstr(temp_str));

    widget_add_button_element(
        instance->widget,
        GuiButtonTypeRight,
        "More",
        nfc_protocol_support_common_widget_callback,
        instance);

    furi_string_free(temp_str);
}

static NfcCommand nfc_scene_read_poller_callback_mf_classic(NfcGenericEvent event, void* context) {
    furi_assert(event.protocol == NfcProtocolMfClassic);

    NfcApp* instance = context;
    const MfClassicPollerEvent* mf_classic_event = event.data;

    // TODO: Implement read mf_classic using key cache
    if(mf_classic_event->type == MfClassicPollerEventTypeReadComplete) {
        nfc_device_set_data(
            instance->nfc_device, NfcProtocolMfClassic, nfc_poller_get_data(instance->poller));
        view_dispatcher_send_custom_event(instance->view_dispatcher, NfcCustomEventPollerSuccess);
        return NfcCommandStop;
    }

    return NfcCommandContinue;
}

static void nfc_scene_read_on_enter_mf_classic(NfcApp* instance) {
    nfc_poller_start(instance->poller, nfc_scene_read_poller_callback_mf_classic, instance);
}

static void nfc_scene_read_menu_on_enter_mf_classic(NfcApp* instance) {
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
}

static void nfc_scene_read_success_on_enter_mf_classic(NfcApp* instance) {
    const NfcDevice* device = instance->nfc_device;
    const MfClassicData* data = nfc_device_get_data(device, NfcProtocolMfClassic);

    FuriString* temp_str = furi_string_alloc();
    furi_string_cat_printf(
        temp_str, "\e#%s\n", nfc_device_get_name(device, NfcDeviceNameTypeFull));
    nfc_render_mf_classic_info(data, NfcProtocolFormatTypeShort, temp_str);

    widget_add_text_scroll_element(
        instance->widget, 0, 0, 128, 52, furi_string_get_cstr(temp_str));

    furi_string_free(temp_str);
}

static void nfc_scene_saved_menu_on_enter_mf_classic(NfcApp* instance) {
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

static void nfc_scene_emulate_on_enter_mf_classic(NfcApp* instance) {
    const MfClassicData* data = nfc_device_get_data(instance->nfc_device, NfcProtocolMfClassic);
    instance->listener = nfc_listener_alloc(instance->nfc, NfcProtocolMfClassic, data);
    nfc_listener_start(instance->listener, NULL, NULL);
}

static bool nfc_scene_info_on_event_mf_classic(NfcApp* instance, uint32_t event) {
    if(event == GuiButtonTypeRight) {
        scene_manager_next_scene(instance->scene_manager, NfcSceneNotImplemented);
        return true;
    }

    return false;
}

static bool nfc_scene_read_menu_on_event_mf_classic(NfcApp* instance, uint32_t event) {
    if(event == SubmenuIndexDetectReader) {
        scene_manager_next_scene(instance->scene_manager, NfcSceneNotImplemented);
        dolphin_deed(DolphinDeedNfcDetectReader);
        return true;
    }

    return false;
}

static bool nfc_scene_saved_menu_on_event_mf_classic(NfcApp* instance, uint32_t event) {
    bool consumed = false;

    if(event == SubmenuIndexDetectReader) {
        scene_manager_next_scene(instance->scene_manager, NfcSceneNotImplemented);
        consumed = true;
    } else if(event == SubmenuIndexWrite) {
        scene_manager_next_scene(instance->scene_manager, NfcSceneNotImplemented);
        consumed = true;
    } else if(event == SubmenuIndexUpdate) {
        scene_manager_next_scene(instance->scene_manager, NfcSceneNotImplemented);
        consumed = true;
    }

    return consumed;
}

const NfcProtocolSupportBase nfc_protocol_support_mf_classic = {
    .features = NfcProtocolFeatureEmulateFull,

    .scene_info =
        {
            .on_enter = nfc_scene_info_on_enter_mf_classic,
            .on_event = nfc_scene_info_on_event_mf_classic,
        },
    .scene_read =
        {
            .on_enter = nfc_scene_read_on_enter_mf_classic,
            .on_event = NULL,
        },
    .scene_read_menu =
        {
            .on_enter = nfc_scene_read_menu_on_enter_mf_classic,
            .on_event = nfc_scene_read_menu_on_event_mf_classic,
        },
    .scene_read_success =
        {
            .on_enter = nfc_scene_read_success_on_enter_mf_classic,
            .on_event = NULL,
        },
    .scene_saved_menu =
        {
            .on_enter = nfc_scene_saved_menu_on_enter_mf_classic,
            .on_event = nfc_scene_saved_menu_on_event_mf_classic,
        },
    .scene_emulate = {
        .on_enter = nfc_scene_emulate_on_enter_mf_classic,
        .on_event = NULL,
    }};
