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

// Interface functions
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

static bool nfc_protocol_support_has_feature(NfcProtocol protocol, NfcProtocolFeature feature) {
    return nfc_protocol_support[protocol]->features & feature;
}

// Common scene handlers
// SceneInfo
static void nfc_protocol_support_scene_info_on_enter(NfcApp* instance) {
    const NfcProtocol protocol = nfc_device_get_protocol(instance->nfc_device);
    nfc_protocol_support[protocol]->scene_info.on_enter(instance);
    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcViewWidget);
}

static bool nfc_protocol_support_scene_info_on_event(NfcApp* instance, SceneManagerEvent event) {
    bool consumed = false;
    const NfcProtocol protocol = nfc_device_get_protocol(instance->nfc_device);

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = nfc_protocol_support[protocol]->scene_info.on_event(instance, event.event);
    }

    return consumed;
}

static void nfc_protocol_support_scene_info_on_exit(NfcApp* instance) {
    widget_reset(instance->widget);
}

// SceneRead
static void nfc_protocol_support_scene_read_on_enter(NfcApp* instance) {
    popup_set_header(
        instance->popup, "Reading card\nDon't move...", 85, 24, AlignCenter, AlignTop);
    popup_set_icon(instance->popup, 12, 23, &A_Loading_24);

    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcViewPopup);

    const NfcProtocol protocol = instance->protocols_detected[instance->protocols_detected_idx];
    instance->poller = nfc_poller_alloc(instance->nfc, protocol);

    // Start poller with the appropriate callback
    nfc_protocol_support[protocol]->scene_read.on_enter(instance);
    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcViewPopup);

    nfc_blink_detect_start(instance);
}

static bool nfc_protocol_support_scene_read_on_event(NfcApp* instance, SceneManagerEvent event) {
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == NfcCustomEventReadHandlerSuccess) {
            notification_message(instance->notifications, &sequence_success);
            scene_manager_next_scene(instance->scene_manager, NfcSceneReadSuccess);
            dolphin_deed(DolphinDeedNfcReadSuccess);
            consumed = true;
        } else if(event.event == NfcCustomEventReadHandlerFailure) {
            if(scene_manager_has_previous_scene(instance->scene_manager, NfcSceneDetect)) {
                scene_manager_search_and_switch_to_previous_scene(
                    instance->scene_manager, NfcSceneDetect);
            }
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        static const uint32_t possible_scenes[] = {NfcSceneSelectProtocol, NfcSceneStart};
        scene_manager_search_and_switch_to_previous_scene_one_of(
            instance->scene_manager, possible_scenes, COUNT_OF(possible_scenes));
        consumed = true;
    }

    return consumed;
}

static void nfc_protocol_support_scene_read_on_exit(NfcApp* instance) {
    nfc_poller_stop(instance->poller);
    nfc_poller_free(instance->poller);
    popup_reset(instance->popup);

    nfc_blink_stop(instance);
}

// SceneReadMenu
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
            scene_manager_next_scene(instance->scene_manager, NfcSceneSaveName);
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

// SceneReadSuccess
static void nfc_protocol_support_scene_read_success_on_enter(NfcApp* instance) {
    Widget* widget = instance->widget;

    const NfcProtocol protocol = nfc_device_get_protocol(instance->nfc_device);
    nfc_protocol_support[protocol]->scene_read_success.on_enter(instance);

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

// SceneSavedMenu
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
