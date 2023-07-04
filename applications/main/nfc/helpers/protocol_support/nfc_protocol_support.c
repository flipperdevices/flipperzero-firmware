#include "nfc_protocol_support.h"

#include "nfc_protocol_support_defs.h"
#include "nfc_protocol_support_gui_common.h"

#include "../../nfc_app_i.h"

typedef void (*NfcProtocolSupportCommonOnEnter)(NfcApp* instance);
typedef bool (*NfcProtocolSupportCommonOnEvent)(NfcApp* instance, SceneManagerEvent event);
typedef void (*NfcProtocolSupportCommonOnExit)(NfcApp* instance);

typedef struct {
    NfcProtocolSupportCommonOnEnter on_enter;
    NfcProtocolSupportCommonOnEvent on_event;
    NfcProtocolSupportCommonOnExit on_exit;
} NfcProtocolSupportCommonSceneBase;

static const NfcProtocolSupportCommonSceneBase nfc_protocol_support_scenes[];

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

void nfc_protocol_support_on_enter(NfcProtocolSupportScene scene, void* context) {
    furi_assert(scene < NfcProtocolSupportSceneCount);
    furi_assert(context);

    NfcApp* instance = context;
    nfc_protocol_support_scenes[scene].on_enter(instance);
}

bool nfc_protocol_support_on_event(
    NfcProtocolSupportScene scene,
    void* context,
    SceneManagerEvent event) {
    furi_assert(scene < NfcProtocolSupportSceneCount);
    furi_assert(context);

    NfcApp* instance = context;
    return nfc_protocol_support_scenes[scene].on_event(instance, event);
}

void nfc_protocol_support_on_exit(NfcProtocolSupportScene scene, void* context) {
    furi_assert(scene < NfcProtocolSupportSceneCount);
    furi_assert(context);

    NfcApp* instance = context;
    nfc_protocol_support_scenes[scene].on_exit(instance);
}

// TODO: Move to separate files?
// Info
static void nfc_protocol_support_scene_info_on_enter(NfcApp* instance) {
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

    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcViewWidget);
}

static bool nfc_protocol_support_scene_info_on_event(NfcApp* instance, SceneManagerEvent event) {
    bool consumed = false;
    const NfcProtocol protocol = nfc_device_get_protocol(instance->nfc_device);

    if(event.event == SceneManagerEventTypeCustom) {
        consumed = nfc_protocol_support[protocol]->scene_info.on_event(instance, event.event);
    }

    return consumed;
}

static void nfc_protocol_support_scene_info_on_exit(NfcApp* instance) {
    widget_reset(instance->widget);
}

// Read
static void nfc_protocol_support_scene_read_on_enter(NfcApp* instance) {
    UNUSED(instance);
}

static bool nfc_protocol_support_scene_read_on_event(NfcApp* instance, SceneManagerEvent event) {
    UNUSED(instance);
    UNUSED(event);
    bool consumed = false;

    return consumed;
}

static void nfc_protocol_support_scene_read_on_exit(NfcApp* instance) {
    UNUSED(instance);
}

// ReadMenu
static void nfc_protocol_support_scene_read_menu_on_enter(NfcApp* instance) {
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

    nfc_protocol_support[protocol]->scene_read_menu.on_enter(instance);

    submenu_add_item(
        submenu,
        "Info",
        SubmenuIndexCommonInfo,
        nfc_protocol_support_common_submenu_callback,
        instance);

    submenu_set_selected_item(
        instance->submenu,
        scene_manager_get_scene_state(instance->scene_manager, NfcSceneReadMenu));

    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcViewMenu);
}

static bool
    nfc_protocol_support_scene_read_menu_on_event(NfcApp* instance, SceneManagerEvent event) {
    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(instance->scene_manager, NfcSceneReadMenu, event.event);

        const NfcProtocol protocol = nfc_device_get_protocol(instance->nfc_device);

        switch(event.event) {
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
            return nfc_protocol_support[protocol]->scene_read_menu.on_event(instance, event.event);
        }
    }

    return false;
}

static void nfc_protocol_support_scene_read_menu_on_exit(NfcApp* instance) {
    submenu_reset(instance->submenu);
}

// ReadSuccess
static void nfc_protocol_support_scene_read_success_on_enter(NfcApp* instance) {
    Widget* widget = instance->widget;

    FuriString* temp_str = furi_string_alloc();
    nfc_protocol_support_render_info(instance->nfc_device, NfcProtocolFormatTypeShort, temp_str);

    widget_add_text_scroll_element(widget, 0, 0, 128, 52, furi_string_get_cstr(temp_str));
    furi_string_free(temp_str);

    widget_add_button_element(
        widget, GuiButtonTypeLeft, "Retry", nfc_protocol_support_common_widget_callback, instance);
    widget_add_button_element(
        widget, GuiButtonTypeRight, "More", nfc_protocol_support_common_widget_callback, instance);

    notification_message_block(instance->notifications, &sequence_set_green_255);
    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcViewWidget);
}

static bool
    nfc_protocol_support_scene_read_success_on_event(NfcApp* instance, SceneManagerEvent event) {
    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case GuiButtonTypeLeft:
            scene_manager_next_scene(instance->scene_manager, NfcSceneRetryConfirm);
            return true;
        case GuiButtonTypeRight:
            scene_manager_next_scene(instance->scene_manager, NfcSceneReadMenu);
            return true;
        default:
            return false;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        scene_manager_next_scene(instance->scene_manager, NfcSceneExitConfirm);
        return true;
    }

    return false;
}

static void nfc_protocol_support_scene_read_success_on_exit(NfcApp* instance) {
    notification_message_block(instance->notifications, &sequence_reset_green);
    widget_reset(instance->widget);
}

// SavedMenu
static void nfc_protocol_support_scene_saved_menu_on_enter(NfcApp* instance) {
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
    nfc_protocol_support[protocol]->scene_saved_menu.on_enter(instance);

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

    submenu_set_selected_item(
        instance->submenu,
        scene_manager_get_scene_state(instance->scene_manager, NfcSceneSavedMenu));

    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcViewMenu);
}

static bool
    nfc_protocol_support_scene_saved_menu_on_event(NfcApp* instance, SceneManagerEvent event) {
    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(instance->scene_manager, NfcSceneSavedMenu, event.event);

        const NfcProtocol protocol = nfc_device_get_protocol(instance->nfc_device);

        // TODO: Implement restore from shadow file

        switch(event.event) {
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
            return nfc_protocol_support[protocol]->scene_saved_menu.on_event(
                instance, event.event);
        }
    }
    return false;
}

static void nfc_protocol_support_scene_saved_menu_on_exit(NfcApp* instance) {
    submenu_reset(instance->submenu);
}

static const NfcProtocolSupportCommonSceneBase
    nfc_protocol_support_scenes[NfcProtocolSupportSceneCount] = {
        [NfcProtocolSupportSceneInfo] =
            {
                .on_enter = nfc_protocol_support_scene_info_on_enter,
                .on_event = nfc_protocol_support_scene_info_on_event,
                .on_exit = nfc_protocol_support_scene_info_on_exit,
            },
        [NfcProtocolSupportSceneRead] =
            {
                .on_enter = nfc_protocol_support_scene_read_on_enter,
                .on_event = nfc_protocol_support_scene_read_on_event,
                .on_exit = nfc_protocol_support_scene_read_on_exit,
            },
        [NfcProtocolSupportSceneReadMenu] =
            {
                .on_enter = nfc_protocol_support_scene_read_menu_on_enter,
                .on_event = nfc_protocol_support_scene_read_menu_on_event,
                .on_exit = nfc_protocol_support_scene_read_menu_on_exit,
            },
        [NfcProtocolSupportSceneReadSuccess] =
            {
                .on_enter = nfc_protocol_support_scene_read_success_on_enter,
                .on_event = nfc_protocol_support_scene_read_success_on_event,
                .on_exit = nfc_protocol_support_scene_read_success_on_exit,
            },
        [NfcProtocolSupportSceneSavedMenu] =
            {
                .on_enter = nfc_protocol_support_scene_saved_menu_on_enter,
                .on_event = nfc_protocol_support_scene_saved_menu_on_event,
                .on_exit = nfc_protocol_support_scene_saved_menu_on_exit,
            },
};

// ----------------------------- Deprecated code -------------------------------------------
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
