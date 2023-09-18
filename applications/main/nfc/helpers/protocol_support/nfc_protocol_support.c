#include "nfc_protocol_support.h"

#include "nfc/nfc_app_i.h"
#include "nfc/helpers/nfc_supported_cards.h"

#include "nfc_protocol_support_defs.h"
#include "nfc_protocol_support_gui_common.h"

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

    if(event.type == SceneManagerEventTypeCustom) {
        const NfcProtocol protocol = nfc_device_get_protocol(instance->nfc_device);
        consumed = nfc_protocol_support[protocol]->scene_info.on_event(instance, event.event);
    } else if(event.type == SceneManagerEventTypeBack) {
        // If the card could not be parsed, return to the respective menu
        if(!scene_manager_get_scene_state(instance->scene_manager, NfcSceneSupportedCard)) {
            const uint32_t scenes[] = {NfcSceneSavedMenu, NfcSceneReadMenu};
            scene_manager_search_and_switch_to_previous_scene_one_of(
                instance->scene_manager, scenes, COUNT_OF(scenes));
            consumed = true;
        }
    }

    return consumed;
}

static void nfc_protocol_support_scene_info_on_exit(NfcApp* instance) {
    widget_reset(instance->widget);
}

static void nfc_protocol_support_scene_card_dump_on_enter(NfcApp* instance) {
    const NfcProtocol protocol = nfc_device_get_protocol(instance->nfc_device);
    nfc_protocol_support[protocol]->scene_card_dump.on_enter(instance);
    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcViewTextBox);
}

static bool
    nfc_protocol_support_scene_card_dump_on_event(NfcApp* instance, SceneManagerEvent event) {
    UNUSED(instance);
    UNUSED(event);
    return false;
}

static void nfc_protocol_support_scene_card_dump_on_exit(NfcApp* instance) {
    text_box_reset(instance->text_box);
    furi_string_reset(instance->text_box_store);
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
        if(event.event == NfcCustomEventPollerSuccess) {
            notification_message(instance->notifications, &sequence_success);
            scene_manager_next_scene(instance->scene_manager, NfcSceneReadSuccess);
            dolphin_deed(DolphinDeedNfcReadSuccess);
            consumed = true;
        } else if(event.event == NfcCustomEventPollerIncomplete) {
            bool card_read = nfc_supported_cards_read(instance->nfc_device, instance->nfc);
            if(card_read) {
                notification_message(instance->notifications, &sequence_success);
                scene_manager_next_scene(instance->scene_manager, NfcSceneReadSuccess);
                dolphin_deed(DolphinDeedNfcReadSuccess);
                consumed = true;
            } else {
                const NfcProtocol protocol =
                    instance->protocols_detected[instance->protocols_detected_idx];
                if(nfc_protocol_support[protocol]->scene_read.on_event) {
                    consumed =
                        nfc_protocol_support[protocol]->scene_read.on_event(instance, event.event);
                }
            }
        } else if(event.event == NfcCustomEventPollerFailure) {
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

    } else if(nfc_protocol_support_has_feature(protocol, NfcProtocolFeatureEmulateFull)) {
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
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(instance->scene_manager, NfcSceneReadMenu, event.event);

        if(event.event == SubmenuIndexCommonSave) {
            scene_manager_next_scene(instance->scene_manager, NfcSceneSaveName);
            consumed = true;
        } else if(event.event == SubmenuIndexCommonInfo) {
            scene_manager_next_scene(instance->scene_manager, NfcSceneInfo);
            consumed = true;
        } else if(event.event == SubmenuIndexCommonEmulate) {
            dolphin_deed(DolphinDeedNfcEmulate);
            scene_manager_next_scene(instance->scene_manager, NfcSceneEmulate);
            consumed = true;
        } else {
            const NfcProtocol protocol = nfc_device_get_protocol(instance->nfc_device);
            consumed =
                nfc_protocol_support[protocol]->scene_read_menu.on_event(instance, event.event);
        }

    } else if(event.type == SceneManagerEventTypeBack) {
        scene_manager_set_scene_state(instance->scene_manager, NfcSceneSavedMenu, 0);
    }

    return consumed;
}

static void nfc_protocol_support_scene_read_menu_on_exit(NfcApp* instance) {
    submenu_reset(instance->submenu);
}

// SceneReadSuccess
static void nfc_protocol_support_scene_read_success_on_enter(NfcApp* instance) {
    Widget* widget = instance->widget;

    FuriString* temp_str = furi_string_alloc();
    if(nfc_supported_cards_parse(instance->nfc_device, temp_str)) {
        widget_add_text_scroll_element(
            instance->widget, 0, 0, 128, 52, furi_string_get_cstr(temp_str));
    } else {
        const NfcProtocol protocol = nfc_device_get_protocol(instance->nfc_device);
        nfc_protocol_support[protocol]->scene_read_success.on_enter(instance);
    }

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
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == GuiButtonTypeLeft) {
            scene_manager_next_scene(instance->scene_manager, NfcSceneRetryConfirm);
            consumed = true;

        } else if(event.event == GuiButtonTypeRight) {
            scene_manager_next_scene(instance->scene_manager, NfcSceneReadMenu);
            consumed = true;
        }

    } else if(event.type == SceneManagerEventTypeBack) {
        scene_manager_next_scene(instance->scene_manager, NfcSceneExitConfirm);
        consumed = true;
    }

    return consumed;
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

    } else if(nfc_protocol_support_has_feature(protocol, NfcProtocolFeatureEmulateFull)) {
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

    if(nfc_has_shadow_file(instance)) {
        submenu_add_item(
            submenu,
            "Restore Data Changes",
            SubmenuIndexCommonRestore,
            nfc_protocol_support_common_submenu_callback,
            instance);
    }

    submenu_set_selected_item(
        instance->submenu,
        scene_manager_get_scene_state(instance->scene_manager, NfcSceneSavedMenu));

    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcViewMenu);
}

static bool
    nfc_protocol_support_scene_saved_menu_on_event(NfcApp* instance, SceneManagerEvent event) {
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(instance->scene_manager, NfcSceneSavedMenu, event.event);

        if(event.event == SubmenuIndexCommonRestore) {
            scene_manager_next_scene(instance->scene_manager, NfcSceneRestoreOriginalConfirm);
            consumed = true;
        } else if(event.event == SubmenuIndexCommonInfo) {
            scene_manager_next_scene(instance->scene_manager, NfcSceneSupportedCard);
            consumed = true;
        } else if(event.event == SubmenuIndexCommonRename) {
            scene_manager_next_scene(instance->scene_manager, NfcSceneSaveName);
            consumed = true;
        } else if(event.event == SubmenuIndexCommonDelete) {
            scene_manager_next_scene(instance->scene_manager, NfcSceneDelete);
            consumed = true;
        } else if(event.event == SubmenuIndexCommonEmulate) {
            const bool is_added =
                scene_manager_has_previous_scene(instance->scene_manager, NfcSceneSetType);
            dolphin_deed(is_added ? DolphinDeedNfcAddEmulate : DolphinDeedNfcEmulate);
            scene_manager_next_scene(instance->scene_manager, NfcSceneEmulate);
            consumed = true;
        } else if(event.event == SubmenuIndexCommonEdit) {
            scene_manager_next_scene(instance->scene_manager, NfcSceneSetUid);
            consumed = true;
        } else {
            const NfcProtocol protocol = nfc_device_get_protocol(instance->nfc_device);
            consumed =
                nfc_protocol_support[protocol]->scene_saved_menu.on_event(instance, event.event);
        }

    } else if(event.type == SceneManagerEventTypeBack) {
        scene_manager_set_scene_state(instance->scene_manager, NfcSceneSavedMenu, 0);
    }

    return consumed;
}

static void nfc_protocol_support_scene_saved_menu_on_exit(NfcApp* instance) {
    submenu_reset(instance->submenu);
}

// SceneEmulate
enum {
    NfcSceneEmulateStateWidget,
    NfcSceneEmulateStateTextBox,
};

static void nfc_protocol_support_scene_emulate_on_enter(NfcApp* instance) {
    Widget* widget = instance->widget;
    TextBox* text_box = instance->text_box;

    FuriString* temp_str = furi_string_alloc();
    const NfcProtocol protocol = nfc_device_get_protocol(instance->nfc_device);

    widget_add_icon_element(widget, 0, 3, &I_NFC_dolphin_emulation_47x61);

    if(nfc_protocol_support_has_feature(protocol, NfcProtocolFeatureEmulateUid)) {
        widget_add_string_element(
            widget, 90, 13, AlignCenter, AlignTop, FontPrimary, "Emulating UID");

        size_t uid_len;
        const uint8_t* uid = nfc_device_get_uid(instance->nfc_device, &uid_len);

        for(size_t i = 0; i < uid_len; ++i) {
            furi_string_cat_printf(temp_str, "%02X ", uid[i]);
        }

        furi_string_trim(temp_str);

    } else {
        widget_add_string_element(widget, 90, 13, AlignCenter, AlignTop, FontPrimary, "Emulating");
        furi_string_set(
            temp_str, nfc_device_get_name(instance->nfc_device, NfcDeviceNameTypeFull));
    }

    widget_add_text_box_element(
        widget, 56, 28, 68, 25, AlignCenter, AlignTop, furi_string_get_cstr(temp_str), false);

    furi_string_free(temp_str);

    text_box_set_font(text_box, TextBoxFontHex);
    text_box_set_focus(text_box, TextBoxFocusEnd);
    furi_string_reset(instance->text_box_store);

    // instance->listener is allocated in the respective on_enter() handler
    nfc_protocol_support[protocol]->scene_emulate.on_enter(instance);

    scene_manager_set_scene_state(
        instance->scene_manager, NfcSceneEmulate, NfcSceneEmulateStateWidget);

    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcViewWidget);
    nfc_blink_emulate_start(instance);
}

static bool
    nfc_protocol_support_scene_emulate_on_event(NfcApp* instance, SceneManagerEvent event) {
    bool consumed = false;

    const uint32_t state = scene_manager_get_scene_state(instance->scene_manager, NfcSceneEmulate);

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == NfcCustomEventListenerUpdate) {
            // Add data button to widget if data is received for the first time
            if(furi_string_size(instance->text_box_store)) {
                widget_add_button_element(
                    instance->widget,
                    GuiButtonTypeCenter,
                    "Log",
                    nfc_protocol_support_common_widget_callback,
                    instance);
            }
            // Update TextBox data
            text_box_set_text(instance->text_box, furi_string_get_cstr(instance->text_box_store));
            consumed = true;
        } else if(event.event == GuiButtonTypeCenter) {
            if(state == NfcSceneEmulateStateWidget) {
                view_dispatcher_switch_to_view(instance->view_dispatcher, NfcViewTextBox);
                scene_manager_set_scene_state(
                    instance->scene_manager, NfcSceneEmulate, NfcSceneEmulateStateTextBox);
                consumed = true;
            }
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        if(state == NfcSceneEmulateStateTextBox) {
            view_dispatcher_switch_to_view(instance->view_dispatcher, NfcViewWidget);
            scene_manager_set_scene_state(
                instance->scene_manager, NfcSceneEmulate, NfcSceneEmulateStateWidget);
            consumed = true;
        }
    }

    return consumed;
}

static void nfc_protocol_support_scene_emulate_on_exit(NfcApp* instance) {
    nfc_listener_stop(instance->listener);

    NfcDevice* nfc_stub = nfc_device_alloc();
    NfcProtocol protocol = nfc_device_get_protocol(instance->nfc_device);
    const NfcDeviceData* data = nfc_listener_get_data(instance->listener, protocol);
    nfc_device_set_data(nfc_stub, protocol, data);

    //TODO: think about nfc_device_is_equal(NfcDevice*,NfcDeviceData*);
    if(!nfc_device_is_equal(nfc_stub, instance->nfc_device)) {
        nfc_device_set_data(instance->nfc_device, protocol, data);
        nfc_save_shadow_file(instance);
    }
    nfc_device_free(nfc_stub);

    nfc_listener_free(instance->listener);

    // Clear view
    widget_reset(instance->widget);
    text_box_reset(instance->text_box);
    furi_string_reset(instance->text_box_store);

    nfc_blink_stop(instance);
}

static const NfcProtocolSupportCommonSceneBase
    nfc_protocol_support_scenes[NfcProtocolSupportSceneCount] = {
        [NfcProtocolSupportSceneInfo] =
            {
                .on_enter = nfc_protocol_support_scene_info_on_enter,
                .on_event = nfc_protocol_support_scene_info_on_event,
                .on_exit = nfc_protocol_support_scene_info_on_exit,
            },
        [NfcProtocolSupportSceneCardDump] =
            {
                .on_enter = nfc_protocol_support_scene_card_dump_on_enter,
                .on_event = nfc_protocol_support_scene_card_dump_on_event,
                .on_exit = nfc_protocol_support_scene_card_dump_on_exit,
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
        [NfcProtocolSupportSceneEmulate] =
            {
                .on_enter = nfc_protocol_support_scene_emulate_on_enter,
                .on_event = nfc_protocol_support_scene_emulate_on_event,
                .on_exit = nfc_protocol_support_scene_emulate_on_exit,
            },
};
