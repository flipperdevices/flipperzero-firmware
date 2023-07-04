#include "nfc_protocol_support.h"

#include "nfc_protocol_support_defs.h"
#include "nfc_protocol_support_gui_common.h"

#include "../../nfc_app_i.h"

static bool nfc_protocol_support_has_feature(NfcProtocol protocol, NfcProtocolFeature feature) {
    return nfc_protocol_support[protocol]->features & feature;
}

static void nfc_protocol_support_render_info(
    const NfcDevice* device,
    NfcProtocolFormatType format_type,
    FuriString* str) {
    const NfcProtocol protocol = nfc_device_get_protocol(device);
    const NfcDeviceData* data = nfc_device_get_data(device, protocol);
    furi_string_cat_printf(str, "\e#%s\n", nfc_device_get_name(device, NfcDeviceNameTypeFull));
    nfc_protocol_support[protocol]->render_info(data, format_type, str);
}

NfcCustomEvent nfc_protocol_support_handle_poller(NfcGenericEvent event, void* context) {
    furi_assert(context);
    NfcApp* nfc_app = context;

    NfcCustomEvent custom_event =
        nfc_protocol_support[event.protocol]->handle_poller(event.data, context);
    if(custom_event == NfcCustomEventReadHandlerSuccess) {
        nfc_device_set_data(
            nfc_app->nfc_device, event.protocol, nfc_poller_get_data(nfc_app->poller));
    }

    return custom_event;
}

// Scene builders

void nfc_protocol_support_build_scene_info(NfcApp* instance) {
    const NfcProtocol protocol = nfc_device_get_protocol(instance->nfc_device);
    Widget* widget = instance->widget;

    FuriString* temp_str = furi_string_alloc();
    nfc_protocol_support_render_info(instance->nfc_device, NfcProtocolFormatTypeFull, temp_str);

    uint8_t text_scroll_height;

    if(nfc_protocol_support_has_feature(protocol, NfcProtocolFeatureMoreData)) {
        widget_add_button_element(
            widget,
            GuiButtonTypeRight,
            "More",
            nfc_protocol_support_common_widget_callback,
            instance);
        text_scroll_height = 52;
    } else {
        text_scroll_height = 64;
    }

    widget_add_text_scroll_element(
        widget, 0, 0, 128, text_scroll_height, furi_string_get_cstr(temp_str));
    furi_string_free(temp_str);
}

void nfc_protocol_support_build_scene_read_menu(NfcApp* instance) {
    const NfcProtocol protocol = nfc_device_get_protocol(instance->nfc_device);

    Submenu* submenu = instance->submenu;

    submenu_add_item(
        submenu,
        "Save",
        SubmenuIndexCommonSave,
        nfc_protocol_support_common_submenu_callback,
        instance);

    if(nfc_protocol_support_has_feature(protocol, NfcProtocolFeatureEmulateUid)) {
        submenu_add_item(
            submenu,
            "Emulate UID",
            SubmenuIndexCommonEmulate,
            nfc_protocol_support_common_submenu_callback,
            instance);
    }

    if(nfc_protocol_support_has_feature(protocol, NfcProtocolFeatureEmulateFull)) {
        submenu_add_item(
            submenu,
            "Emulate",
            SubmenuIndexCommonEmulate,
            nfc_protocol_support_common_submenu_callback,
            instance);
    }

    nfc_protocol_support[protocol]->build_scene_read_menu(instance);

    submenu_add_item(
        submenu,
        "Info",
        SubmenuIndexCommonInfo,
        nfc_protocol_support_common_submenu_callback,
        instance);
}

void nfc_protocol_support_build_scene_read_success(NfcApp* instance) {
    Widget* widget = instance->widget;

    FuriString* temp_str = furi_string_alloc();
    nfc_protocol_support_render_info(instance->nfc_device, NfcProtocolFormatTypeShort, temp_str);

    widget_add_text_scroll_element(widget, 0, 0, 128, 52, furi_string_get_cstr(temp_str));
    furi_string_free(temp_str);

    widget_add_button_element(
        widget, GuiButtonTypeLeft, "Retry", nfc_protocol_support_common_widget_callback, instance);
    widget_add_button_element(
        widget, GuiButtonTypeRight, "More", nfc_protocol_support_common_widget_callback, instance);
}

void nfc_protocol_support_build_scene_saved_menu(NfcApp* instance) {
    const NfcProtocol protocol = nfc_device_get_protocol(instance->nfc_device);

    Submenu* submenu = instance->submenu;

    // Header submenu items
    if(nfc_protocol_support_has_feature(protocol, NfcProtocolFeatureEmulateUid)) {
        submenu_add_item(
            submenu,
            "Emulate UID",
            SubmenuIndexCommonEmulate,
            nfc_protocol_support_common_submenu_callback,
            instance);
    }

    if(nfc_protocol_support_has_feature(protocol, NfcProtocolFeatureEmulateFull)) {
        submenu_add_item(
            submenu,
            "Emulate",
            SubmenuIndexCommonEmulate,
            nfc_protocol_support_common_submenu_callback,
            instance);
    }

    if(nfc_protocol_support_has_feature(protocol, NfcProtocolFeatureEditUid)) {
        submenu_add_item(
            submenu,
            "Edit UID",
            SubmenuIndexCommonEdit,
            nfc_protocol_support_common_submenu_callback,
            instance);
    }

    // Protocol-dependent menu items
    nfc_protocol_support[protocol]->build_scene_saved_menu(instance);

    // Trailer submenu items
    submenu_add_item(
        submenu,
        "Info",
        SubmenuIndexCommonInfo,
        nfc_protocol_support_common_submenu_callback,
        instance);
    submenu_add_item(
        submenu,
        "Rename",
        SubmenuIndexCommonRename,
        nfc_protocol_support_common_submenu_callback,
        instance);
    submenu_add_item(
        submenu,
        "Delete",
        SubmenuIndexCommonDelete,
        nfc_protocol_support_common_submenu_callback,
        instance);

    // TODO: Implement restore from shadow file
}

// Scene handlers

bool nfc_protocol_support_handle_scene_info(NfcApp* instance, uint32_t event) {
    const NfcProtocol protocol = nfc_device_get_protocol(instance->nfc_device);
    return nfc_protocol_support[protocol]->handle_scene_info(instance, event);
}

bool nfc_protocol_support_handle_scene_read_menu(NfcApp* instance, uint32_t event) {
    const NfcProtocol protocol = nfc_device_get_protocol(instance->nfc_device);

    switch(event) {
    case SubmenuIndexCommonSave:
        scene_manager_next_scene(instance->scene_manager, NfcSceneNotImplemented);
        return true;
    case SubmenuIndexCommonInfo:
        scene_manager_next_scene(instance->scene_manager, NfcSceneInfo);
        return true;
    case SubmenuIndexCommonEmulate:
        dolphin_deed(DolphinDeedNfcEmulate);
        // FALLTHRU
    default:
        return nfc_protocol_support[protocol]->handle_scene_read_menu(instance, event);
    }
}

bool nfc_protocol_support_handle_scene_read_success(NfcApp* instance, uint32_t event) {
    switch(event) {
    case GuiButtonTypeLeft:
        scene_manager_next_scene(instance->scene_manager, NfcSceneRetryConfirm);
        return true;
    case GuiButtonTypeRight:
        scene_manager_next_scene(instance->scene_manager, NfcSceneReadMenu);
        return true;
    default:
        return false;
    }
}

bool nfc_protocol_support_handle_scene_saved_menu(NfcApp* instance, uint32_t event) {
    const NfcProtocol protocol = nfc_device_get_protocol(instance->nfc_device);

    // TODO: Implement restore from shadow file

    switch(event) {
    case SubmenuIndexCommonInfo:
        scene_manager_next_scene(instance->scene_manager, NfcSceneInfo);
        return true;
    case SubmenuIndexCommonRename:
        scene_manager_next_scene(instance->scene_manager, NfcSceneSaveName);
        return true;
    case SubmenuIndexCommonDelete:
        scene_manager_next_scene(instance->scene_manager, NfcSceneDelete);
        return true;
    case SubmenuIndexCommonEmulate:
        if(scene_manager_has_previous_scene(instance->scene_manager, NfcSceneSetType)) {
            dolphin_deed(DolphinDeedNfcAddEmulate);
        } else {
            dolphin_deed(DolphinDeedNfcEmulate);
        }
        // FALLTHRU
    default:
        return nfc_protocol_support[protocol]->handle_scene_saved_menu(instance, event);
    }
}
